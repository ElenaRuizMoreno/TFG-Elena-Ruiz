# Carpeta de Datos

Esta carpeta almacena los archivos generados por el pulsioxímetro, organizados en diferentes subdirectorios según su estado de procesamiento.

## Estructura

**Datos crudos/:** Contiene los datos sin procesar capturados directamente del pulsioxímetro en formato CSV.

- save_log/: Archivos generados con el script save_log.py.

- save_log2/: Archivos generados con el script save_log2.py.

**Datos limpios/:** Contiene los datos a los que se les ha quitado la parte de la señal que eran artefactos. Ha sido un recorte específico para cada señal.

**Procesados/:** Contiene los datos a los que se les han recortado los primeros y últimos 5 segundos.

README.md: Este archivo con la descripción de la carpeta.

## Formato de los Archivos CSV

Cada archivo CSV generado sigue el siguiente formato:

- Primera fila: Cabecera con nombres de las columnas.

- Columnas principales:

- Tiempo (ms): Momento en que se tomó la muestra.

- IR: Señal del LED IR.

- AMB_IR: Señal ambiente del LED IR.

- RED: Señal del LED rojo.

- AMB_RED: Señal ambiente del LED rojo.



## Generación de Datos

Los archivos en esta carpeta son generados por los scripts save_log.py y save_log2.py, que registran datos durante 30 segundos y los guardan automáticamente en formato CSV.


## Uso

Para procesar los datos, utilizar los scripts en la carpeta procesamiento/ dentro de Código.

Una vez procesados, se recomienda almacenar los archivos en una carpeta específica para diferenciarlos de los datos sin procesar.

