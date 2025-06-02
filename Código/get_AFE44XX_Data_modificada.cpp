// Sustituir esta función por la del firmware original

boolean AFE44XX::get_AFE44XX_Data(afe44xx_data *afe44xx_raw_data) {
    static unsigned long start_time = millis();
    unsigned long current_time = millis();
    //static unsigned long last_time = 0;

    // Detener la captura tras 30s
    if (current_time - start_time > 30000) {
        // Serial.println("LOG_END");
        return false;
    }

    // Medir el tiempo de inicio
    unsigned long startMicros = micros();

    // Leer datos del AFE4490
    afe44xxWrite(CONTROL0, 0x000001);
    IRtemp = afe44xxRead(LED1VAL);
    afe44xxWrite(CONTROL0, 0x000001);
    REDtemp = afe44xxRead(LED2VAL);
    afe44xxWrite(CONTROL0, 0x000001);
    A_IRtemp = afe44xxRead(ALED1VAL);
    afe44xxWrite(CONTROL0, 0x000001);
    A_REDtemp = afe44xxRead(ALED2VAL);

    afe44xx_raw_data->IR_data = (signed long)((unsigned long)(IRtemp << 10) >> 10);
    afe44xx_raw_data->RED_data = (signed long)((unsigned long)(REDtemp << 10) >> 10);
    afe44xx_raw_data->AMB_IR_data = (signed long)((unsigned long)(A_IRtemp << 10) >> 10);
    afe44xx_raw_data->AMB_RED_data = (signed long)((unsigned long)(A_REDtemp << 10) >> 10);

    // Guardar datos en buffers
    aun_ir_buffer[n_buffer_count]      = (uint16_t)((afe44xx_raw_data->IR_data) >> 4);
    aun_red_buffer[n_buffer_count]     = (uint16_t)((afe44xx_raw_data->RED_data) >> 4);
    aun_amb_ir_buffer[n_buffer_count]  = (uint16_t)((afe44xx_raw_data->AMB_IR_data) >> 4);
    aun_amb_red_buffer[n_buffer_count] = (uint16_t)((afe44xx_raw_data->AMB_RED_data) >> 4);

    // Imprimir datos con timestamp
    /*Serial.print(current_time - start_time);
    Serial.print(",");
    Serial.print(aun_ir_buffer[n_buffer_count]);
    Serial.print(",");
    Serial.print(aun_amb_ir_buffer[n_buffer_count]);
    Serial.print(",");
    Serial.print(aun_red_buffer[n_buffer_count]);
    Serial.print(",");
    Serial.println(aun_amb_red_buffer[n_buffer_count]);*/

    n_buffer_count++;

    // Calcular SpO2 cada SAMPLE_SIZE mediciones
    if (n_buffer_count >= SAMPLE_SIZE) {
    Spo2.estimate_spo2(aun_ir_buffer, SAMPLE_SIZE, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
    afe44xx_raw_data->spo2 = n_spo2;
    afe44xx_raw_data->buffer_count_overflow = true;
    n_buffer_count = 0;

    // Mostrar resultados procesados por Serial
    if (afe44xx_raw_data->IR_data < 10000) {
        Serial.println("  Dedo no detectado");
    } else {
        if (ch_hr_valid && afe44xx_raw_data->heart_rate > 30 && afe44xx_raw_data->heart_rate < 220) {
            Serial.print("Frecuencia cardiaca: ");
            Serial.print(afe44xx_raw_data->heart_rate);
            Serial.print(" BPM, ");
        } else {
            Serial.print("Frecuencia cardiaca: --- BPM, ");
        }

        if (ch_spo2_valid && afe44xx_raw_data->spo2 > 50 && afe44xx_raw_data->spo2 <= 100) {
            Serial.print("Saturación de oxígeno: ");
            Serial.print(afe44xx_raw_data->spo2);
            Serial.println(" %");
        } else {
            Serial.println("Saturación de oxígeno: --- %");
        }
    }
}



    // Calcular frecuencia cardíaca
    hral.statHRMAlgo(afe44xx_raw_data->RED_data);
    afe44xx_raw_data->heart_rate = hral.HeartRate;

    // Ajustar a 60 Hz (16.67 ms por muestra)
    unsigned long endMicros = micros();
    unsigned long execution_time = endMicros - startMicros;
    if (execution_time < 16670) {
        delayMicroseconds(16670 - execution_time);
    }

    return true;
}