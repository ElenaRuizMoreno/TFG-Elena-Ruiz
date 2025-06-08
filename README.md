# Desarrollo del software de un pulsioxímetro externo integrado en la incubadora In3ator

## Elena Ruiz Moreno

### Ingeniería de la Salud, Universidad de Burgos, curso 2024/2025

Se estima que en 2020 nacieron de forma prematura 13,4 millones de bebés en todo el mundo. Uno de cada diez nacimientos. Pero detrás de esa cifra hay una desigualdad enorme: mientras que en países de altos ingresos la mayoría de estos recién nacidos sobrevive, en los países con menos recursos muchos no lo consiguen, sobre todo por la falta de cuidados básicos como una fuente de calor, oxígeno o sistemas de monitorización.

Más del 90% de los bebés nacidos antes de las 28 semanas fallecen en los primeros días en los países con bajos ingresos, frente a menos del 10% en países desarrollados. No es solo una cuestión médica; es también tecnológica y estructural. Mejorar el acceso a equipos sanitarios adecuados marca la diferencia entre vivir o no.

Conscientes de esta realidad, en 2014 un grupo de profesionales de la salud y la ingeniería puso en marcha el proyecto In3ator, una incubadora neonatal de bajo coste, accesible y de código abierto. Desde 2019, la ONG Medicina Abierta al Mundo lidera su fabricación y distribución en aquellos lugares que lo necesitan. Pero el proyecto sigue evolucionando. Una de sus necesidades actuales es integrar un sistema de monitorización fisiológica sencillo, fiable y adaptado a sus limitaciones técnicas.

Desde el inicio, el reto ha sido claro: convertir señales ópticas crudas en información médica fiable dentro de un sistema embebido y de bajo coste. A lo largo del proyecto se ha desarrollado un pipeline completo de tratamiento de señal, adaptando algoritmos a las limitaciones del hardware de la incubadora y validando su funcionamiento con datos reales.

### Introducción 

El desarrollo de este proyecto surge de un primer contacto con la ONG [Medicina Abierta al Mundo]([https://www.ejemplo.com](https://github.com/medicalopenworld) en la que nos introducen la necesidad de integrar una mejora a la incubadora, y esa mejora es la incorporación de un pulsioxímetro integrado que sea capaz de estimar la frecuencia cardíaca y la saturación de oxígeno de un neonato. Para ello, nos hacen llegar todo lo necesario para el desarrollo del proyecto, y comenzamos a trabajar investigando el funcionamiento de un pulsioxímetro convencional e intentar acercar esa teoría a los materiales con los que disponíamos.

### Contenido del repositorio

El proyecto se ha organizado en una estructura de carpetas diferenciadas. A continuación, se describe el contenido y propósito de cada directorio:

- **Artículos**: contiene una recopilación de artículos científicos y documentación técnica utilizada para la elaboración del estado del arte y la justificación de decisiones técnicas a lo largo del TFG. Dentro de ella, además de diferentes técnicas y métodos, hay un subdirectorio llamado `TFGs Pulsioxímetro`, en el que se pueden encontrar diferentes trabajos o proyectos que han desarrollado soluciones similares y que han servido de ayuda para entender cómo se han abordado problemas parecidos y qué soluciones se han propuesto en otros contextos.

- **Código**: incluye todos los scripts, algoritmos y archivos fuente del proyecto. Está dividido en subcarpetas:
  - **adquisición**: contiene los scripts de Python que hay que ejecutar en el prompt de Anaconda para registrar los datos y guardarlos en local en formato `.csv` en la ruta que se indique.
  - **comparacion**: contiene scripts para comparar los diferentes filtros descritos en el apartado de conceptos teóricos. Además, realiza una representación gráfica para observar el efecto de cada filtro sobre una señal PPG de ejemplo.
  - **diagramas de bloques**: incluye esquemas visuales que representan el funcionamiento de los algoritmos y la arquitectura general del sistema.
  - **firmware**: contiene el código embebido **original** que se ejecuta en la PCB de la incubadora.
  - **funciones modificadas**: en este directorio se encuentran los archivos `get_AFE44XX_Data_modificada.cpp` (para la recogida de datos) y `SPO2_modificado.cpp` (que implementa el algoritmo de estimación de constantes fisiológicas).
  - **procesamiento**: contiene notebooks y scripts de Python para el tratamiento, filtrado y análisis de las señales registradas. También se calculan aquí los parámetros fisiológicos. Está organizado en dos carpetas principales, una para cada parámetro estimado, y dentro de cada una de ellas hay subcarpetas con pruebas, incluyendo procedimientos descartados.
  - **resultados**: recoge las gráficas y tablas generadas a partir del análisis de los algoritmos implementados en Python.

- **Datos**: estructura donde se almacenan los datos recogidos durante las sesiones de prueba. Se organiza en:
  - **Datos crudos**: señales originales sin procesar. Se dividen en los directorios `save_log` y `save_log2`, según la versión de la función de adquisición que se haya utilizado.
  - **Datos_limpios**: señales tras la limpieza inicial, con un recorte **personalizado** para cada registro, según lo que sobrara de cada señal.
  - **Procesados**: datos limpios, pero habiendo recortado para todos los registros los primeros y últimos 5 segundos.

- **Demostración**: vídeo corto de una simulación funcional de la solución desarrollada (formato `.mp4`).

- **Documentación de referencia**: manuales técnicos de los componentes utilizados (AFE4490, sensor UA401-D, normas ISO aplicables, documentación del proyecto *In³ator*, etc.). La mayoría cedidos por la ONG Medicina Abierta al Mundo.

- **Documentación Overleaf**: este es el proyecto principal en Overleaf donde se redacta tanto la memoria del TFG como los anexos. Está organizado en las siguientes carpetas y archivos:
  - `tex/`: contiene los archivos `.tex` que forman los distintos capítulos de la memoria (introducción, metodología, resultados, etc.).
  - `img/`: carpeta con las imágenes, figuras y diagramas utilizados a lo largo del documento.
  - `memoria.tex`: archivo principal que compila toda la memoria del TFG importando los capítulos desde la carpeta `tex`.
  - `anexos.tex`: archivo donde se redactan todos los anexos del trabajo, también de forma modular.
  - `bibliografia.bib`: fichero BibTeX con las referencias bibliográficas utilizadas en la memoria principal.
  - `bibliografiaAnexos.bib`: archivo separado con las referencias citadas únicamente en los anexos, para mantener la bibliografía organizada y evitar duplicidades.

- **README.md**: archivo de texto que resume el propósito del trabajo y explica brevemente cómo utilizar el proyecto.
