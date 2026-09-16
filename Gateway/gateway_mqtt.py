import serial
import time
import paho.mqtt.client as mqtt
from paho.mqtt.enums import CallbackAPIVersion

# --- PODESAVANJA ---
COM_PORT = 'COM5'        
BAUD_RATE = 9600

# HiveMQ Cloud podaci sa naloga
HIVEMQ_HOST = "028c3514ff754447ba034dc347d72a2c.s1.eu.hivemq.cloud"
HIVEMQ_PORT = 8883
HIVEMQ_USER = "marko555"     
HIVEMQ_PASS = "marko555"      

# --- INICIJALIZACIJA MQTT KLIJENTA ---
client = mqtt.Client(CallbackAPIVersion.VERSION2)
client.username_pw_set(HIVEMQ_USER, HIVEMQ_PASS)
client.tls_set()

print("Povezivanje na HiveMQ Cloud...")
client.connect(HIVEMQ_HOST, HIVEMQ_PORT)
client.loop_start()
print("Uspešno povezano na HiveMQ!")

# --- INICIJALIZACIJA SERIJSKOG PORTA ---
try:
    ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)
    print(f"Slušam STM32 preko {COM_PORT}...")
except Exception as e:
    print(f"Greška pri otvaranju porta {COM_PORT}: {e}")
    exit()

# --- GLAVNA PETLJA ---
try:
    while True:
        if ser.in_waiting > 0:
            linija = ser.readline().decode('utf-8', errors='ignore').strip()
            
            if ":" in linija:
                topic, poruka = linija.split(":", 1)
                client.publish(topic, poruka, qos=1)
                print(f"[MQTT -> HiveMQ] Tema: {topic} | Poruka: {poruka}")
                
        time.sleep(0.05)

except KeyboardInterrupt:
    print("\nZaustavljanje...")
    ser.close()
    client.loop_stop()
    client.disconnect()
