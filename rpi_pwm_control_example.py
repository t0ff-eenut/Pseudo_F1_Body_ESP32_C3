#!/usr/bin/env python3
"""
ESP32-C3 RC Car Control - PWM Direct Control Example
라즈베리파이에서 ESP32로 PWM 값을 직접 제어하는 예제
"""

import serial
import time

class RCCarController:
    """RC Car 제어 클래스"""
    
    # 명령어 정의
    CMD_HEARTBEAT = 0x00
    CMD_CONTROL = 0x01          # 풀 레인지 제어 (-127 ~ +127)
    CMD_SET_MODE = 0x02
    CMD_CONTROL_RAW = 0x10      # 직접 PWM 제어 (모터)
    CMD_SERVO_RAW = 0x11        # 직접 PWM 제어 (서보)
    CMD_EMERGENCY_STOP = 0xFF
    
    # 프로토콜 상수
    STX = 0xAA
    ETX = 0x55
    
    def __init__(self, port='/dev/ttyAMA0', baudrate=115200):
        """
        초기화
        :param port: UART 포트
        :param baudrate: 통신 속도
        """
        self.ser = serial.Serial(port, baudrate, timeout=1)
        time.sleep(0.1)  # 연결 안정화
        
    def _create_packet(self, cmd, param1, param2):
        """
        패킷 생성
        :param cmd: 명령어
        :param param1: 파라미터 1
        :param param2: 파라미터 2
        :return: 6바이트 패킷
        """
        # 파라미터를 unsigned byte로 변환
        p1 = param1 & 0xFF
        p2 = param2 & 0xFF
        
        # 체크섬 계산 (XOR)
        checksum = cmd ^ p1 ^ p2
        
        # 패킷 구성
        packet = bytes([self.STX, cmd, p1, p2, checksum, self.ETX])
        return packet
    
    def send_heartbeat(self):
        """하트비트 전송"""
        packet = self._create_packet(self.CMD_HEARTBEAT, 0, 0)
        self.ser.write(packet)
        
    def control(self, speed, steering):
        """
        풀 레인지 제어 (권장 방식)
        :param speed:    속도 (-127 ~ +127)  음수=후진, 양수=전진
        :param steering: 조향 (-127 ~ +127)  음수=좌,  양수=우
        """
        # 범위 제한
        speed = max(-127, min(127, speed))
        steering = max(-127, min(127, steering))
        
        # signed int8을 unsigned byte로 변환
        speed_byte    = speed    if speed    >= 0 else (256 + speed)
        steering_byte = steering if steering >= 0 else (256 + steering)
        
        packet = self._create_packet(self.CMD_CONTROL, speed_byte, steering_byte)
        self.ser.write(packet)

    def control_percentage(self, speed, steering):
        """
        퍼센테이지 → 풀 레인지 변환 헬퍼
        :param speed:    속도 (-100 ~ 100)
        :param steering: 조향 (-100 ~ 100)
        """
        speed    = max(-100, min(100, speed))
        steering = max(-100, min(100, steering))
        self.control(speed * 127 // 100, steering * 127 // 100)
        
    def control_motor_pwm(self, pwm_value):
        """
        모터 PWM 직접 제어
        :param pwm_value: PWM 값 (0 ~ 1023)
        """
        # 범위 제한
        pwm_value = max(0, min(1023, pwm_value))
        
        # 16-bit 값을 상위/하위 바이트로 분할
        param1 = (pwm_value >> 8) & 0xFF  # 상위 바이트
        param2 = pwm_value & 0xFF          # 하위 바이트
        
        packet = self._create_packet(self.CMD_CONTROL_RAW, param1, param2)
        self.ser.write(packet)
        
    def control_servo_pwm(self, pwm_value):
        """
        서보 PWM 직접 제어
        :param pwm_value: PWM 값 (0 ~ 8191)
        """
        # 범위 제한
        pwm_value = max(0, min(8191, pwm_value))
        
        # 16-bit 값을 상위/하위 바이트로 분할
        param1 = (pwm_value >> 8) & 0xFF  # 상위 바이트
        param2 = pwm_value & 0xFF          # 하위 바이트
        
        packet = self._create_packet(self.CMD_SERVO_RAW, param1, param2)
        self.ser.write(packet)
        
    def control_servo_angle(self, angle):
        """
        서보 각도로 제어 (PWM으로 변환)
        :param angle: 각도 (-90 ~ 90)
        """
        # 범위 제한
        angle = max(-90, min(90, angle))
        
        # 각도를 펄스 폭으로 변환
        pulse_us = 1500 + (angle * 1000 // 90)
        
        # 펄스 폭을 PWM 듀티로 변환
        # PWM = (pulse_us * 8192) / 20000
        pwm_value = (pulse_us * 8192) // 20000
        
        self.control_servo_pwm(pwm_value)
        
    def emergency_stop(self):
        """비상 정지"""
        packet = self._create_packet(self.CMD_EMERGENCY_STOP, 0, 0)
        self.ser.write(packet)
        
    def close(self):
        """연결 종료"""
        self.emergency_stop()
        time.sleep(0.1)
        self.ser.close()


def demo_percentage_control():
    """퍼센테이지 제어 데모"""
    print("=== 퍼센테이지 제어 데모 ===")
    car = RCCarController()
    
    try:
        # 전진 50%
        print("전진 50%")
        car.control_percentage(speed=50, steering=0)
        time.sleep(2)
        
        # 우회전하며 전진
        print("우회전 30도, 전진 50%")
        car.control(speed=64, steering=43)  # 50% → 64, 30% → 43
        time.sleep(2)
        
        # 정지
        print("정지")
        car.control_percentage(speed=0, steering=0)
        time.sleep(1)
        
    finally:
        car.close()


def demo_pwm_control():
    """PWM 직접 제어 데모"""
    print("\n=== PWM 직접 제어 데모 ===")
    car = RCCarController()
    
    try:
        # 모터 PWM 512 (약 50%)
        print("모터 PWM 512 (50%)")
        car.control_motor_pwm(512)
        time.sleep(2)
        
        # 서보 중앙 (1500μs)
        print("서보 중앙 위치")
        car.control_servo_angle(0)
        time.sleep(1)
        
        # 서보 우회전 45도
        print("서보 우회전 45도")
        car.control_servo_angle(45)
        time.sleep(2)
        
        # 정지
        print("정지")
        car.control_motor_pwm(0)
        car.control_servo_angle(0)
        time.sleep(1)
        
    finally:
        car.close()


def demo_precise_control():
    """정밀 PWM 제어 데모 (고급)"""
    print("\n=== 정밀 PWM 제어 데모 ===")
    car = RCCarController()
    
    try:
        # PWM 값을 점진적으로 증가
        print("PWM 0 → 1023 (점진적 가속)")
        for pwm in range(0, 1024, 100):
            print(f"  PWM: {pwm}")
            car.control_motor_pwm(pwm)
            time.sleep(0.5)
        
        # 감속
        print("PWM 1023 → 0 (점진적 감속)")
        for pwm in range(1023, -1, -100):
            print(f"  PWM: {pwm}")
            car.control_motor_pwm(pwm)
            time.sleep(0.5)
        
        # 서보 스윕
        print("서보 스윕 (-90° → +90°)")
        for angle in range(-90, 91, 15):
            print(f"  각도: {angle}°")
            car.control_servo_angle(angle)
            time.sleep(0.3)
        
        # 중앙 복귀
        car.control_servo_angle(0)
        
    finally:
        car.close()


if __name__ == "__main__":
    print("ESP32-C3 RC Car PWM Control Demo")
    print("=" * 50)
    
    # 데모 선택
    print("\n사용 가능한 데모:")
    print("1. 퍼센테이지 제어 (권장)")
    print("2. PWM 직접 제어")
    print("3. 정밀 PWM 제어 (고급)")
    
    choice = input("\n데모 선택 (1-3): ").strip()
    
    if choice == "1":
        demo_percentage_control()
    elif choice == "2":
        demo_pwm_control()
    elif choice == "3":
        demo_precise_control()
    else:
        print("잘못된 선택입니다.")
