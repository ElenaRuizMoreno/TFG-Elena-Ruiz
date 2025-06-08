# Carpeta de Código

Esta carpeta contiene el código fuente relacionado con la implementación del pulsioxímetro en la incubadora In3ator.

## Estructura

**adquisición:** Contiene los scripts de python que hay que ejecutar en el promt de Anaconda para poder registrar los datos y guardarlos en local en formato .csv en la ruta que se indique.

**comparación:** Contiene dos notebooks que se han utilizado para crear dos imágenes ilustrativas del uso de filtros. Han sido empleadas para los conceptos teóricos.

**diagramas de bloques:** Este directorio no contiene código si no los diagramas de los principales algoritmos explicados. Se divide en dos carpetas: frecuencia cardiaca y saturación de oxígeno, segun a los diagramas que se quiera acceder. Existe un directorio adicional con los esquemas de las conexiones hardware que se han empleado para registrar los datos. Y por último, dentro del directorio principal se encuentra el caso de uso descrito en el anexo F y el diagrama de bloques que representa cómo se ha desarrollado el código hasta llegar al firmware que estima los parámetros de interés.

**firmware/:** Contiene el código original que corre en la PCB de la incubadora para la lectura y procesamiento de datos del sensor AFE4490.

**funciones modificadas:** En este directorio se encuentran las funciones modificadas del firmware que hay que sustituir en el codigo fuente del proyecto original para obtener los resultados que obtiene este proyecto (cambiar nombre del archivo).

**procesamiento/:** Scripts en Python para analizar los datos obtenidos del pulsioxímetro y extraer información relevante. Se diferencian dos carpetas: una para los notebooks de estimación de frecuencia cardiaca y otra para los notebooks de estimacion de spo2 (y dentro de el las pruebas fallidas para la estimación de cada uno de los parámetros). 

Tambien hay 3 archivos a parte:

- uno llamado benchmark_filtros para estimar el tiempo de computo de los dos algoritmos de estimacion de frecuencia cardiaca con los que se obtuvo mejores resultados.
- otro llamado limpieza_datos para analizar las partes de la señal de cada registro que sobraban y asi poder hacer análisis mas precisos.
- otro para obtener un ejemplo de visualización de la señal PPG de un archivo en concreto. La imagen se utilizó para el anexo D.

**resultados:** Directorio que a su vez se divide en dos carpetas (fc y spo2) para mostrar los resultados de aplicar los algoritmos implementados en Python sobre los registros recogidos.

## Requisitos

Dependiendo del tipo de desarrollo, pueden ser necesarios los siguientes entornos y herramientas:

**Para el Firmware:**

- PlatformIO (Extensión de VS Code recomendada)
- Microcontrolador compatible con la PCB de In3ator
- Librerías necesarias para comunicación con el AFE4490

**Para el Procesamiento de Datos:**

- Python 3.x
- Bibliotecas recomendadas: numpy, pandas, matplotlib, scipy

## Uso

### Firmware

- Abre el proyecto en Visual Studio Code con PlatformIO.
- Compila y sube el código al microcontrolador.
- Verifica la salida de datos usando el monitor serie.

### Adquisición de Datos

- Ejecuta save_log.py o save_log2.py según la frecuencia de muestreo deseada.
- Los datos se guardarán automáticamente en la carpeta de descargas en formato CSV.

## Procesamiento de Datos

- Ejecuta los scripts en procesamiento/ para analizar los archivos CSV generados.
- Quedate con los algoritmos que mejor resultados den en Python y los migras al firmware original en el archivo SPO2.cpp.
- Una alternativa es utilizar las funciones modificadas que han sido descritas.


