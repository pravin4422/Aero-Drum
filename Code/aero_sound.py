import serial
import pygame
import time
import os

pygame.mixer.init(frequency=44100, size=-16, channels=2, buffer=512)


sounds = {

    "MPU1_X": pygame.mixer.Sound("snare.wav"),   
    "MPU1_Y": pygame.mixer.Sound("hihat.wav"),   
    "MPU1_Z": pygame.mixer.Sound("bass.wav"),    
    

    "MPU2_X": pygame.mixer.Sound("tom.wav"),     
    "MPU2_Y": pygame.mixer.Sound("crash.wav"),   
    "MPU2_Z": pygame.mixer.Sound("floortom.wav") 
}

# Set volume for all sounds (maximum volume)
for sound in sounds.values():
    sound.set_volume(1.0)

# Function to connect to Arduino
def connect_to_serial(port_name, baud_rate):
    try:
        print(f"Connecting to Arduino on {port_name}...")
        serial_conn = serial.Serial(port_name, baud_rate, timeout=1)
        print(f"Connected successfully!")
        return serial_conn
    except serial.SerialException as e:
        print(f"Error connecting: {e}")
        print("Please check:")
        print("1. The Arduino is connected to the correct port")
        print("2. No other program is using the port")
        print("3. You have permission to access the port")
        return None

def main():
  
    port = 'COM12'  
    baud_rate = 9600
    required_files = ["snare.wav", "hihat.wav", "bass.wav", "tom.wav", "crash.wav", "floortom.wav"]
    missing_files = [f for f in required_files if not os.path.exists(f)]
    
    if missing_files:
        print(f"ERROR: Missing sound files: {', '.join(missing_files)}")
        print("Please make sure these WAV files are in the same directory as this script.")
        return
    ser = connect_to_serial(port, baud_rate)
    if not ser:
        return
    
    print("\n=== MPU6050 Electronic Drum Kit ===")
    print("\nSensor mappings:")
    print("- MPU1_X (Sensor 1, left-right): Snare drum")
    print("- MPU1_Y (Sensor 1, forward-backward): Hi-hat")
    print("- MPU1_Z (Sensor 1, up-down): Bass drum")
    print("- MPU2_X (Sensor 2, left-right): Tom")
    print("- MPU2_Y (Sensor 2, forward-backward): Crash cymbal")
    print("- MPU2_Z (Sensor 2, up-down): Floor tom")
    print("\nWaiting for Arduino to initialize...")
    print("\nTesting sounds...")
    for sound_name, sound in sounds.items():
        print(f"Playing {sound_name} sound...")
        sound.play()
        time.sleep(0.7)  # Allow each sound to be heard clearly
    ready = False
    timeout_counter = 0
    while not ready and timeout_counter < 50:  # 5 second timeout
        if ser.in_waiting:
            line = ser.readline().decode().strip()
            if "READY" in line:
                ready = True
                print("\nArduino sensors calibrated and ready!")
                print("\nStart playing your drum kit!\n")
        else:
            time.sleep(0.1)
            timeout_counter += 1
    
    if not ready:
        print("Arduino didn't send READY signal. Continuing anyway...")
    

    last_hit_time = {}
    
    try:
        while True:
            if ser.in_waiting:
    
                trigger = ser.readline().decode().strip()
                
    
                current_time = time.time()
                
    
                if trigger in sounds:
    
                    if trigger not in last_hit_time or (current_time - last_hit_time[trigger]) > 0.1:
                        print(f"Playing: {trigger}")
                        sounds[trigger].play()
                        last_hit_time[trigger] = current_time
                else:
                    print(f"Received: {trigger}")
            
    
            time.sleep(0.01)
            
    except KeyboardInterrupt:
        print("\nExiting program")
    except Exception as e:
        print(f"Error: {e}")
    finally:
    
        if ser and ser.is_open:
            ser.close()
            print("Serial connection closed.")

if __name__ == "__main__":
    main()
    