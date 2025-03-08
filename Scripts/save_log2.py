import serial
import csv
import time

# Configura el puerto serie
SERIAL_PORT = "COM9"  # Asegúrate de que este puerto es el correcto
BAUD_RATE = 115200    # Debe coincidir con el de la PCB
OUTPUT_FILE = r"C:\Users\Elena\Downloads\pulsioximetro_datos.csv"

# Abre el puerto serie
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
time.sleep(2)  # Espera a que el puerto se estabilice

# Crea el archivo CSV y escribe la cabecera
with open(OUTPUT_FILE, mode="w", newline="") as file:
    writer = csv.writer(file)
    writer.writerow(["Tiempo (ms)", "IR", "AMB_IR", "RED", "AMB_RED"])  # Cabecera del CSV

    start_time = time.time()

    while True:
        # Lee una línea del puerto serie
        line = ser.readline().decode(errors='ignore').strip()

        if line:
            values = line.split(",")  # Divide la línea por comas
            
            # Asegura que haya 5 valores antes de escribir
            if len(values) == 5:
                writer.writerow(values)
                print(values)  # Muestra los datos en pantalla

        # Detiene la captura después de 30 segundos
        if time.time() - start_time > 30:
            print(f"✅ Medición finalizada, datos guardados en: {OUTPUT_FILE}")
            break

# Cierra la conexión serie
ser.close()
