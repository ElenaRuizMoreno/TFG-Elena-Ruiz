Archivos Python utilizados para el proceso de adquisición de datos en formato csv y posterior almacenamiento en la carpeta de descargas. 

La última versión es el archivo llamado "save_log2.py".

Ejecutar en la terminal de Anaconda Promt con el comando "python save_log2.py" en la ruta donde se haya almacenado el archivo. Esto debe de hacerse tras haber subido el firmware a vscode y mientras se tiene el sensor midiendo los parámetros en el dedo del usuario.

Si se quiere utilizar, cambiar la ruta donde se quieren guardar los datos y la configuración indicada en el anexo C.2.

Debería de dar como resultado un archivo .csv con 5 columnas (Tiempo (ms), IR, AMB_IR, RED, AMB_RED). Debe de haber valores consistentes y no constantes y variables durante el tiempo que dura el registro. Los archivos se guardan con el nombre "pulsioximeter_data" se recomienda nombrar el archivo con los datos fisiológicos de referencia.