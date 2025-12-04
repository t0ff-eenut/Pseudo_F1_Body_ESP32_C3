import serial
import time
import struct

# UART Configuration
# Raspberry Pi UART pins: TX(GPIO14), RX(GPIO15) -> /dev/ttyS0 or /dev/serial0
# USB-to-TTL adapter: /dev/ttyUSB0
SERIAL_PORT = '/dev/ttyUSB0'  # Change this to match your setup
BAUD_RATE = 115200

def calc_checksum(data):
    checksum = 0
    for byte in data:
        checksum ^= byte
    return checksum

def send_command(ser, cmd_type, throttle, steering):
    # Packet Format: [HEADER][CMD][THROTTLE][STEERING][CHECKSUM]
    header = 0xAA
    
    # Clamp values
    throttle = max(-100, min(100, throttle))
    steering = max(-45, min(45, steering))
    
    # Pack data (b: signed char, B: unsigned char)
    # Checksum calculation excludes the checksum byte itself
    payload = struct.pack('BBbb', header, cmd_type, throttle, steering)
    checksum = calc_checksum(payload)
    
    packet = payload + struct.pack('B', checksum)
    
    ser.write(packet)
    print(f"Sent: CMD={cmd_type:02X}, THR={throttle}, STR={steering}")

def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud")
        time.sleep(2)  # Wait for connection to stabilize

        # 1. Manual Control Test
        print("\n--- Manual Control Test ---")
        
        # Forward
        print("Forward 50%")
        send_command(ser, 0x01, 50, 0)
        time.sleep(2)
        
        # Stop
        print("Stop")
        send_command(ser, 0x01, 0, 0)
        time.sleep(1)
        
        # Backward
        print("Backward 50%")
        send_command(ser, 0x01, -50, 0)
        time.sleep(2)
        
        # Stop
        send_command(ser, 0x01, 0, 0)
        time.sleep(1)

        # 2. Steering Test
        print("\n--- Steering Test ---")
        
        # Left
        print("Left 30 deg")
        send_command(ser, 0x01, 0, -30)
        time.sleep(1)
        
        # Right
        print("Right 30 deg")
        send_command(ser, 0x01, 0, 30)
        time.sleep(1)
        
        # Center
        print("Center")
        send_command(ser, 0x01, 0, 0)
        time.sleep(1)

        # 3. Emergency Stop Test
        print("\n--- Emergency Stop Test ---")
        print("Forward 80%")
        send_command(ser, 0x01, 80, 0)
        time.sleep(1)
        
        print("EMERGENCY STOP!")
        send_command(ser, 0x03, 0, 0)
        
    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")
    except KeyboardInterrupt:
        print("\nExiting...")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("Serial port closed")

if __name__ == "__main__":
    main()
