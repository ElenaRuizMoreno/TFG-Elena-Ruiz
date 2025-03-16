# Carpeta de Código

Esta carpeta contiene el código fuente relacionado con la implementación del pulsioxímetro en la incubadora In3ator.

## Estructura

- firmware/: Contiene el código que corre en la PCB de la incubadora para la lectura y procesamiento de datos del sensor AFE4490.

- procesamiento/: Scripts en Python para analizar los datos obtenidos del pulsioxímetro y extraer información relevante (frecuencia cardíaca, SpO2, filtrado de datos, etc.).

- README.md: Este archivo con la descripción de la carpeta.

## Requisitos

Dependiendo del tipo de desarrollo, pueden ser necesarios los siguientes entornos y herramientas:

**Para el Firmware:**

- PlatformIO (Extensión de VS Code recomendada)

- Microcontrolador compatible con la PCB de In3ator

- Librerías necesarias para comunicación con el AFE4490

**Para el Procesamiento de Datos:**

- Python 3.x

- Bibliotecas recomendadas: numpy, pandas, matplotlib, scipy

**Uso**

Firmware

Abre el proyecto en Visual Studio Code con PlatformIO.

Compila y sube el código al microcontrolador.

Verifica la salida de datos usando el monitor serie.

**Procesamiento de Datos**

Ejecuta los scripts en procesamiento/ para analizar los archivos CSV generados.

Ajusta los parámetros de los filtros según sea necesario.


