import serial
import csv
import time

# Configura el puerto serie (ajusta según tu PC)
SERIAL_PORT = "COM9"  # Cambiar segun el puerto en el que se conecta el programador, entrar en el device manager y comrpobarlo
BAUD_RATE = 115200  # Asegurar de que coincida con el código en la PCB
OUTPUT_FILE = r"C:\Users\Elena\Downloads\pulsioximetro_datos.csv" # ruta donde quiero guardar los datos que me genera el script

# Abre el puerto serie
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
time.sleep(2)  # Espera a que el puerto se estabilice

# Crea el archivo CSV y escribe la cabecera
with open(OUTPUT_FILE, mode="w", newline="") as file:
    writer = csv.writer(file)
    writer.writerow(["Tiempo (ms)", "IR", "AMB_IR", "RED", "AMB_RED"])  # Cabecera del CSV

    start_time = time.time()
    
    while True:
        line = ser.readline().decode(errors='ignore').strip()  # Lee una línea del puerto serie
        if line:
            values = line.split(",")  # Separa los valores por coma
            if len(values) == 5:  # Asegurar que hay 5 valores (tiempo + 4 señales)
                writer.writerow(values)
                print(values) 
                
        # Detener después de 30 segundos, modificar según el tiempo de espera del pulsioximetro externo
        if time.time() - start_time > 30:
            print("Medición finalizada, datos guardados en:", OUTPUT_FILE)
            break

ser.close()  # Cierra la conexión serie
