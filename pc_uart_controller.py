#!/usr/bin/env python3
"""
Pseudo F1 RC Car — PC UART 제어 UI
ESP32-C3 USB Serial/JTAG (플래싱 포트) 로 6바이트 패킷을 전송합니다.

패킷 구조: [0xAA] [CMD] [P1] [P2] [CMD^P1^P2] [0x55]

필요 패키지:
    pip install pyserial
"""

import tkinter as tk
from tkinter import ttk, messagebox
import serial
import serial.tools.list_ports
import threading
import time


# ─── 프로토콜 상수 ────────────────────────────────────────────────────────────
STX = 0xAA
ETX = 0x55
CMD_HEARTBEAT    = 0x00
CMD_CONTROL      = 0x01   # P1=스로틀(-127~+127), P2=조향(-127~+127)
CMD_BRAKE        = 0x03   # P1=전륜 강도(0~127),  P2=후륜 강도(0~127)
CMD_EMERGENCY_STOP = 0xFF


def make_packet(cmd: int, p1: int, p2: int) -> bytes:
    """6바이트 패킷 생성. p1, p2는 signed int8 (-128~127)."""
    b1 = p1 & 0xFF
    b2 = p2 & 0xFF
    checksum = (cmd ^ b1 ^ b2) & 0xFF
    return bytes([STX, cmd, b1, b2, checksum, ETX])


# ─── 메인 앱 ─────────────────────────────────────────────────────────────────
class RCCarApp(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Pseudo F1 RC Car — UART 제어")
        self.resizable(False, False)
        self.protocol("WM_DELETE_WINDOW", self._on_close)

        self.ser: serial.Serial | None = None
        self._hb_running = False
        self._hb_thread: threading.Thread | None = None
        self._log_lines = []

        # 주기적 재전송 (Failsafe 방지: ESP는 500ms 무신호 시 감속)
        self._repeat_running = False
        self._repeat_thread: threading.Thread | None = None
        self._REPEAT_INTERVAL = 0.15  # 150ms 마다 재전송 (500ms failsafe보다 충분히 짧게)

        self._build_connection_frame()
        self._build_control_frame()
        self._build_brake_frame()
        self._build_log_frame()

        # 키보드 단축키 (WASD / Space)
        self.bind("<KeyPress>", self._on_key_press)
        self.bind("<KeyRelease>", self._on_key_release)
        self._pressed_keys: set[str] = set()
        self._kb_after_id = None

    # ── UI 구성 ───────────────────────────────────────────────────────────────

    def _build_connection_frame(self):
        f = ttk.LabelFrame(self, text="연결 설정", padding=8)
        f.grid(row=0, column=0, columnspan=2, padx=10, pady=6, sticky="ew")

        ttk.Label(f, text="COM 포트:").grid(row=0, column=0, sticky="w")
        self.port_var = tk.StringVar()
        self.port_combo = ttk.Combobox(f, textvariable=self.port_var, width=18, state="readonly")
        self.port_combo.grid(row=0, column=1, padx=4)

        ttk.Button(f, text="새로고침", command=self._refresh_ports).grid(row=0, column=2, padx=4)
        self.btn_connect = ttk.Button(f, text="연결", command=self._toggle_connect)
        self.btn_connect.grid(row=0, column=3, padx=4)

        self.status_label = ttk.Label(f, text="● 미연결", foreground="red")
        self.status_label.grid(row=0, column=4, padx=8)

        self._refresh_ports()

    def _build_control_frame(self):
        f = ttk.LabelFrame(self, text="주행 제어  (CMD_CONTROL 0x01)", padding=8)
        f.grid(row=1, column=0, padx=10, pady=4, sticky="nsew")

        # 스로틀
        ttk.Label(f, text="스로틀 (전진+/후진-)").grid(row=0, column=0, columnspan=3, sticky="w")
        self.throttle_var = tk.IntVar(value=0)
        self.throttle_slider = ttk.Scale(f, from_=-127, to=127, orient="vertical",
                                          variable=self.throttle_var, length=200,
                                          command=lambda _: self._send_control())
        self.throttle_slider.grid(row=1, column=0, padx=10, pady=4)
        self.throttle_label = ttk.Label(f, text="0", width=5, anchor="center")
        self.throttle_label.grid(row=2, column=0)
        ttk.Button(f, text="리셋", command=lambda: self._reset_slider(self.throttle_var)).grid(row=3, column=0)

        # 구분선
        ttk.Separator(f, orient="vertical").grid(row=1, column=1, sticky="ns", padx=6)

        # 조향
        ttk.Label(f, text="조향 (좌-/우+)").grid(row=0, column=2, columnspan=3, sticky="w")
        self.steering_var = tk.IntVar(value=0)
        self.steering_slider = ttk.Scale(f, from_=-127, to=127, orient="horizontal",
                                          variable=self.steering_var, length=200,
                                          command=lambda _: self._send_control())
        self.steering_slider.grid(row=1, column=2, padx=10, pady=4)
        self.steering_label = ttk.Label(f, text="0", width=5, anchor="center")
        self.steering_label.grid(row=2, column=2)
        ttk.Button(f, text="리셋", command=lambda: self._reset_slider(self.steering_var)).grid(row=3, column=2)

        # 키보드 힌트
        hint = ttk.Label(f, text="키보드: W/S=전진/후진  A/D=조향  Space=전체 정지", foreground="gray")
        hint.grid(row=4, column=0, columnspan=5, pady=(8, 0))

    def _build_brake_frame(self):
        f = ttk.LabelFrame(self, text="브레이크 / 긴급정지", padding=8)
        f.grid(row=1, column=1, padx=10, pady=4, sticky="nsew")

        ttk.Label(f, text="전륜 강도 (0~127)").grid(row=0, column=0, sticky="w")
        self.brake_front_var = tk.IntVar(value=127)
        ttk.Scale(f, from_=0, to=127, variable=self.brake_front_var, orient="horizontal", length=150).grid(row=1, column=0, pady=2)

        ttk.Label(f, text="후륜 강도 (0~127)").grid(row=2, column=0, sticky="w")
        self.brake_rear_var = tk.IntVar(value=127)
        ttk.Scale(f, from_=0, to=127, variable=self.brake_rear_var, orient="horizontal", length=150).grid(row=3, column=0, pady=2)

        ttk.Button(f, text="브레이크 (CMD 0x03)", command=self._send_brake,
                   width=22).grid(row=4, column=0, pady=6)

        ttk.Separator(f, orient="horizontal").grid(row=5, column=0, sticky="ew", pady=4)

        btn_estop = tk.Button(f, text="⛔  긴급 정지  (0xFF)", bg="red", fg="white",
                               font=("", 11, "bold"), command=self._send_estop, width=22)
        btn_estop.grid(row=6, column=0, pady=4)

        ttk.Separator(f, orient="horizontal").grid(row=7, column=0, sticky="ew", pady=4)

        ttk.Label(f, text="하트비트 (1초 간격)").grid(row=8, column=0, sticky="w")
        self.hb_var = tk.BooleanVar(value=False)
        ttk.Checkbutton(f, text="자동 전송", variable=self.hb_var,
                        command=self._toggle_heartbeat).grid(row=9, column=0, sticky="w")

    def _build_log_frame(self):
        f = ttk.LabelFrame(self, text="전송 로그", padding=6)
        f.grid(row=2, column=0, columnspan=2, padx=10, pady=6, sticky="ew")

        self.log_text = tk.Text(f, height=6, state="disabled", font=("Courier", 9))
        self.log_text.pack(fill="x")
        ttk.Button(f, text="지우기", command=self._clear_log).pack(anchor="e")

    # ── 연결 관리 ─────────────────────────────────────────────────────────────

    def _refresh_ports(self):
        ports = [p.device for p in serial.tools.list_ports.comports()]
        self.port_combo["values"] = ports
        if ports:
            self.port_combo.current(0)

    def _toggle_connect(self):
        if self.ser and self.ser.is_open:
            self._disconnect()
        else:
            self._connect()

    def _connect(self):
        port = self.port_var.get()
        if not port:
            messagebox.showwarning("경고", "COM 포트를 선택하세요.")
            return
        try:
            # 포트를 열기 전에 DTR/RTS를 False로 설정해야
            # open() 순간 DTR 토글로 인한 ESP32 자동 리셋을 방지할 수 있음
            self.ser = serial.Serial()
            self.ser.port = port
            self.ser.baudrate = 115200
            self.ser.timeout = 0.1
            self.ser.dtr = False
            self.ser.rts = False
            self.ser.open()
            time.sleep(0.2)
            self.status_label.config(text=f"● {port} 연결됨", foreground="green")
            self.btn_connect.config(text="연결 해제")
            self._log(f"연결: {port} @ 115200")
            # 재전송 루프 시작
            self._repeat_running = True
            self._repeat_thread = threading.Thread(target=self._repeat_loop, daemon=True)
            self._repeat_thread.start()
        except serial.SerialException as e:
            messagebox.showerror("연결 실패", str(e))

    def _disconnect(self):
        # 재전송 루프를 먼저 중단하고 스레드 종료 대기
        self._repeat_running = False
        if self._repeat_thread and self._repeat_thread.is_alive():
            self._repeat_thread.join(timeout=0.5)

        self._hb_running = False
        self.hb_var.set(False)

        if self.ser and self.ser.is_open:
            try:
                # 연결 해제 전 긴급정지 패킷 전송 → ESP32가 즉시 정지
                self.ser.write(make_packet(CMD_EMERGENCY_STOP, 0, 0))
                self.ser.flush()
                time.sleep(0.05)
            except Exception:
                pass
            try:
                # DTR/RTS를 닫기 전에 명시적으로 비활성화
                # → COM 포트 close 시 발생하는 DTR 토글이 ESP32 리셋을 트리거하는 것을 방지
                self.ser.dtr = False
                self.ser.rts = False
                time.sleep(0.05)
                self.ser.close()
            except Exception:
                pass
            self.ser = None

        self.throttle_var.set(0)
        self.steering_var.set(0)
        self.throttle_label.config(text="0")
        self.steering_label.config(text="0")
        self.status_label.config(text="● 미연결", foreground="red")
        self.btn_connect.config(text="연결")
        self._log("연결 해제")

    # ── 패킷 전송 ─────────────────────────────────────────────────────────────

    def _send(self, pkt: bytes, label: str):
        if not self.ser or not self.ser.is_open:
            return
        try:
            self.ser.write(pkt)
            hex_str = " ".join(f"{b:02X}" for b in pkt)
            self._log(f"{label:20s} → [{hex_str}]")
        except serial.SerialException as e:
            self._log(f"전송 오류: {e}")

    def _send_control(self):
        t = int(self.throttle_var.get())
        s = int(self.steering_var.get())
        self.throttle_label.config(text=str(t))
        self.steering_label.config(text=str(s))
        pkt = make_packet(CMD_CONTROL, t, s)
        # 슬라이더 조작 시에만 로그 출력 (재전송 루프는 로그 없이 전송)
        self._send(pkt, f"CTRL T={t:+4d} S={s:+4d}")

    def _send_control_silent(self):
        """로그 없이 현재 값 재전송 (Failsafe 방지용)"""
        if not self.ser or not self.ser.is_open:
            return
        t = int(self.throttle_var.get())
        s = int(self.steering_var.get())
        pkt = make_packet(CMD_CONTROL, t, s)
        try:
            self.ser.write(pkt)
        except serial.SerialException:
            pass

    def _repeat_loop(self):
        """연결 중 현재 슬라이더 값을 주기적으로 재전송 (Failsafe 타이머 유지)"""
        while self._repeat_running:
            self._send_control_silent()
            time.sleep(self._REPEAT_INTERVAL)

    def _send_brake(self):
        pf = int(self.brake_front_var.get())
        pr = int(self.brake_rear_var.get())
        pkt = make_packet(CMD_BRAKE, pf, pr)
        self._send(pkt, f"BRAKE F={pf} R={pr}")

    def _send_estop(self):
        pkt = make_packet(CMD_EMERGENCY_STOP, 0, 0)
        self._send(pkt, "EMERGENCY STOP")
        self.throttle_var.set(0)
        self.steering_var.set(0)
        self.throttle_label.config(text="0")
        self.steering_label.config(text="0")

    def _reset_slider(self, var: tk.IntVar):
        var.set(0)
        self._send_control()

    # ── 하트비트 ──────────────────────────────────────────────────────────────

    def _toggle_heartbeat(self):
        if self.hb_var.get():
            self._hb_running = True
            self._hb_thread = threading.Thread(target=self._heartbeat_loop, daemon=True)
            self._hb_thread.start()
        else:
            self._hb_running = False

    def _heartbeat_loop(self):
        while self._hb_running:
            pkt = make_packet(CMD_HEARTBEAT, 0, 0)
            self._send(pkt, "HEARTBEAT")
            time.sleep(1.0)

    # ── 키보드 제어 ───────────────────────────────────────────────────────────

    def _on_key_press(self, event: tk.Event):
        key = event.keysym.lower()
        if key not in self._pressed_keys:
            self._pressed_keys.add(key)
            self._apply_keyboard()

    def _on_key_release(self, event: tk.Event):
        key = event.keysym.lower()
        self._pressed_keys.discard(key)
        self._apply_keyboard()

    def _apply_keyboard(self):
        keys = self._pressed_keys

        if "space" in keys:
            self.throttle_var.set(0)
            self.steering_var.set(0)
            self._send_control()
            return

        throttle_step = 20
        steer_step = 20

        t = 0
        if "w" in keys:
            t = throttle_step
        elif "s" in keys:
            t = -throttle_step

        s = 0
        if "d" in keys:
            s = steer_step
        elif "a" in keys:
            s = -steer_step

        # 키를 누르는 동안 누적 증가
        cur_t = int(self.throttle_var.get())
        cur_s = int(self.steering_var.get())

        new_t = max(-127, min(127, cur_t + t)) if t != 0 else cur_t
        new_s = max(-127, min(127, cur_s + s)) if s != 0 else cur_s

        # 키를 모두 떼면 슬라이더 0으로
        if "w" not in keys and "s" not in keys:
            new_t = 0
        if "a" not in keys and "d" not in keys:
            new_s = 0

        self.throttle_var.set(new_t)
        self.steering_var.set(new_s)
        self._send_control()

    # ── 로그 ──────────────────────────────────────────────────────────────────

    def _log(self, msg: str):
        ts = time.strftime("%H:%M:%S")
        line = f"[{ts}] {msg}"
        self._log_lines.append(line)
        if len(self._log_lines) > 200:
            self._log_lines.pop(0)

        self.log_text.config(state="normal")
        self.log_text.insert("end", line + "\n")
        self.log_text.see("end")
        self.log_text.config(state="disabled")

    def _clear_log(self):
        self._log_lines.clear()
        self.log_text.config(state="normal")
        self.log_text.delete("1.0", "end")
        self.log_text.config(state="disabled")

    # ── 종료 ──────────────────────────────────────────────────────────────────

    def _on_close(self):
        self._disconnect()
        self.destroy()


if __name__ == "__main__":
    app = RCCarApp()
    app.mainloop()
