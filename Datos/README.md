# Carpeta de Datos

Esta carpeta almacena los archivos generados por el pulsioxímetro, organizados en diferentes subdirectorios según su estado de procesamiento.

## Estructura

**brutos/:** Contiene los datos sin procesar capturados directamente del pulsioxímetro en formato CSV.

- save_log/: Archivos generados con el script save_log.py.

- save_log2/: Archivos generados con el script save_log2.py.

README.md: Este archivo con la descripción de la carpeta.

## Formato de los Archivos CSV

Cada archivo CSV generado sigue el siguiente formato:

- Primera fila: Cabecera con nombres de las columnas.

- Columnas principales:

- timestamp: Momento en que se tomó la muestra.

- led1: Señal del LED rojo.

- led1_ambiente: Señal ambiente del LED rojo.

- led2: Señal del LED infrarrojo.

- led2_ambiente: Señal ambiente del LED infrarrojo.



## Generación de Datos

Los archivos en esta carpeta son generados por los scripts save_log.py y save_log2.py, que registran datos durante 30 segundos y los guardan automáticamente en formato CSV.


## Uso

Para procesar los datos, utilizar los scripts en la carpeta procesamiento/ dentro de Código.

Una vez procesados, se recomienda almacenar los archivos en una carpeta específica para diferenciarlos de los datos sin procesar.

