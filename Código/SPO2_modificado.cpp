#include "SPO2.h"


extern AFE44XX afe44xx;
#define SAMPLE_SIZE 256 // 1024
#define AFE44XX_SPI_SPEED_AFE49XX 2000000
SPISettings SPI_SETTINGS_SPO2(AFE44XX_SPI_SPEED_AFE49XX, MSBFIRST, SPI_MODE0);

volatile int32_t n_buffer_count; // data length

unsigned long IRtemp, A_IRtemp, REDtemp, A_REDtemp;

uint32_t arrayCounter;

int32_t n_spo2;       // SPO2 value
int32_t n_heart_rate; // heart rate value

uint16_t aun_ir_buffer[SAMPLE_SIZE];      // infrared LED sensor data
uint16_t aun_red_buffer[SAMPLE_SIZE];     // red LED sensor data
uint16_t aun_amb_ir_buffer[SAMPLE_SIZE];  // ambien infrared LED sensor data
uint16_t aun_amb_red_buffer[SAMPLE_SIZE]; // ambien red LED sensor data

int8_t ch_spo2_valid; // indicator to show if the SPO2 calculation is valid
int8_t ch_hr_valid;   // indicator to show if the heart rate calculation is valid

const uint8_t uch_spo2_table[184] = {95, 95, 95, 96, 96, 96, 97, 97, 97, 97, 97, 98, 98, 98, 98, 98, 99, 99, 99, 99,
                                     99, 99, 99, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
                                     100, 100, 100, 100, 99, 99, 99, 99, 99, 99, 99, 99, 98, 98, 98, 98, 98, 98, 97, 97,
                                     97, 97, 96, 96, 96, 96, 95, 95, 95, 94, 94, 94, 93, 93, 93, 92, 92, 92, 91, 91,
                                     90, 90, 89, 89, 89, 88, 88, 87, 87, 86, 86, 85, 85, 84, 84, 83, 82, 82, 81, 81,
                                     80, 80, 79, 78, 78, 77, 76, 76, 75, 74, 74, 73, 72, 72, 71, 70, 69, 69, 68, 67,
                                     66, 66, 65, 64, 63, 62, 62, 61, 60, 59, 58, 57, 56, 56, 55, 54, 53, 52, 51, 50,
                                     49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 31, 30, 29,
                                     28, 27, 26, 25, 23, 22, 21, 20, 19, 17, 16, 15, 14, 12, 11, 10, 9, 7, 6, 5,
                                     3, 2, 1};

spo2_algorithm Spo2;
hr_algo hral;
unsigned long peakWindowHP[21], lastOnsetValueLED1, lastPeakValueLED1;
unsigned char HR[12];
unsigned char temp;
unsigned int lastPeak, lastOnset;
unsigned long movingWindowHP;
unsigned char ispeak = 0;
unsigned char movingWindowCount, movingWindowSize, smallest, foundPeak, totalFoundPeak;
unsigned int freq;
unsigned long currentRatio = 0;
static int32_t an_x[BUFFER_SIZE];
static int32_t an_y[BUFFER_SIZE];

AFE44XX::AFE44XX(int cs_pin, int pwdn_pin)
{
    _cs_pin = cs_pin;

    _pwdn_pin = pwdn_pin;

    pinMode(_cs_pin, OUTPUT);
    digitalWrite(_cs_pin, HIGH);

    pinMode(_pwdn_pin, OUTPUT);

    hral.initStatHRM();

    /*pinMode (_drdy_pin,INPUT);// data ready

    digitalWrite(_pwdn_pin, LOW);
    delay(500);
    digitalWrite(_pwdn_pin, HIGH);
    delay(500);
    */
}

// codigo original
/*boolean AFE44XX::get_AFE44XX_Data(afe44xx_data *afe44xx_raw_data)
{
    afe44xxWrite(CONTROL0, 0x000001);
    IRtemp = afe44xxRead(LED1VAL);
    afe44xxWrite(CONTROL0, 0x000001);
    REDtemp = afe44xxRead(LED2VAL);
    afe44xxWrite(CONTROL0, 0x000001);
    A_IRtemp = afe44xxRead(ALED1VAL);
    afe44xxWrite(CONTROL0, 0x000001);
    A_REDtemp = afe44xxRead(ALED2VAL);

    IRtemp = (unsigned long)(IRtemp << 10);
    afe44xx_raw_data->IR_data = (signed long)(IRtemp);
    afe44xx_raw_data->IR_data = (signed long)((afe44xx_raw_data->IR_data) >> 10);

    REDtemp = (unsigned long)(REDtemp << 10);
    afe44xx_raw_data->RED_data = (signed long)(REDtemp);
    afe44xx_raw_data->RED_data = (signed long)((afe44xx_raw_data->RED_data) >> 10);

    A_IRtemp = (unsigned long)(A_IRtemp << 10);
    afe44xx_raw_data->AMB_IR_data = (signed long)(A_IRtemp);
    afe44xx_raw_data->AMB_IR_data = (signed long)((afe44xx_raw_data->AMB_IR_data) >> 10);

    A_REDtemp = (unsigned long)(A_REDtemp << 10);
    afe44xx_raw_data->AMB_RED_data = (signed long)(A_REDtemp);
    afe44xx_raw_data->AMB_RED_data = (signed long)((afe44xx_raw_data->AMB_RED_data) >> 10);

    // if (arrayCounter == 20)
    // {
    //     aun_ir_buffer[n_buffer_count] = (uint16_t)((afe44xx_raw_data->IR_data) >> 4);
    //     aun_red_buffer[n_buffer_count] = (uint16_t)((afe44xx_raw_data->RED_data) >> 4);
    //     n_buffer_count++;
    //     arrayCounter = 0;
    // }

    // arrayCounter++;

    aun_ir_buffer[n_buffer_count] = (uint16_t)((afe44xx_raw_data->IR_data) >> 4);
    aun_red_buffer[n_buffer_count] = (uint16_t)((afe44xx_raw_data->RED_data) >> 4);
    aun_amb_ir_buffer[n_buffer_count] = (uint16_t)((afe44xx_raw_data->AMB_IR_data) >> 4);
    aun_amb_red_buffer[n_buffer_count] = (uint16_t)((afe44xx_raw_data->AMB_RED_data) >> 4);

    Serial.print(aun_ir_buffer[n_buffer_count]);
    Serial.print(",");
    Serial.print(aun_amb_ir_buffer[n_buffer_count]);
    Serial.print(",");
    Serial.print(aun_red_buffer[n_buffer_count]);
    Serial.print(",");
    Serial.print(aun_amb_red_buffer[n_buffer_count]);
    Serial.println("");
    n_buffer_count++;

    if (n_buffer_count > SAMPLE_SIZE - 1)
    {
        Spo2.estimate_spo2(aun_ir_buffer, SAMPLE_SIZE, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
        afe44xx_raw_data->spo2 = n_spo2;
        // afe44xx_raw_data->heart_rate = n_heart_rate;
        n_buffer_count = 0;
        afe44xx_raw_data->buffer_count_overflow = true;
        // Serial.println(String(millis()) + ": -> New data, SPO2 is " + String(n_spo2));
        // for (int i = 0; i < SAMPLE_SIZE; i++)
        // {
        //     Serial.println(aun_ir_buffer[i]);
        // }
    }

    hral.statHRMAlgo(afe44xx_raw_data->RED_data);
    afe44xx_raw_data->heart_rate = hral.HeartRate;

    return true;
}*/
// codigo nuevo modificado 
boolean AFE44XX::get_AFE44XX_Data(afe44xx_data *afe44xx_raw_data) {
    /*static unsigned long start_time = millis();
    unsigned long current_time = millis();
    //static unsigned long last_time = 0;

    // Detener la captura tras 30s
    if (current_time - start_time > 30000) {
        // Serial.println("LOG_END");
        return false;
    }*/

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

// primer codigo de prueba 01/03/2025
/*boolean AFE44XX::get_AFE44XX_Data(afe44xx_data *afe44xx_raw_data) {
    static unsigned long start_time = millis(); // Marca de tiempo de inicio
    //static unsigned long last_time = 0; // Última marca de tiempo registrada
    unsigned long current_time = millis();

    // Detener la captura tras 40s
    if (current_time - start_time > 30000) {
        //Serial.println("LOG_END"); // Indicar fin de registro
        return false;
    }

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
    aun_ir_buffer[n_buffer_count] = (uint16_t)((afe44xx_raw_data->IR_data) >> 4);
    aun_red_buffer[n_buffer_count] = (uint16_t)((afe44xx_raw_data->RED_data) >> 4);
    aun_amb_ir_buffer[n_buffer_count] = (uint16_t)((afe44xx_raw_data->AMB_IR_data) >> 4);
    aun_amb_red_buffer[n_buffer_count] = (uint16_t)((afe44xx_raw_data->AMB_RED_data) >> 4);

    // Imprimir datos por Serial con timestamp
    Serial.print(current_time - start_time);
    Serial.print(",");
    Serial.print(aun_ir_buffer[n_buffer_count]);
    Serial.print(",");
    Serial.print(aun_amb_ir_buffer[n_buffer_count]);
    Serial.print(",");
    Serial.print(aun_red_buffer[n_buffer_count]);
    Serial.print(",");
    Serial.println(aun_amb_red_buffer[n_buffer_count]);

    n_buffer_count++;

    // Calcular SpO2 cada SAMPLE_SIZE mediciones
    if (n_buffer_count >= SAMPLE_SIZE) {
        Spo2.estimate_spo2(aun_ir_buffer, SAMPLE_SIZE, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
        afe44xx_raw_data->spo2 = n_spo2;
        afe44xx_raw_data->buffer_count_overflow = true;
        n_buffer_count = 0;
    }

    // Calcular frecuencia cardíaca en cada iteración
    hral.statHRMAlgo(afe44xx_raw_data->RED_data);
    afe44xx_raw_data->heart_rate = hral.HeartRate;

    return true;
}*/

void AFE44XX::afe44xx_init()
{
    // afe44xxWrite(CONTROL0, 0x000008);
    // vTaskDelay(pdMS_TO_TICKS(SPO2_INIT_TIME));
    // digitalWrite(_pwdn_pin, LOW);
    // delay(500);
    // digitalWrite(_pwdn_pin, HIGH);
    // delay(500);
    afe44xxWrite(CONTROL0, 0x000008);
    // vTaskDelay(pdMS_TO_TICKS(SPO2_INIT_TIME));
    afe44xxWrite(CONTROL0, 0x000000);
    // afe44xxWrite(LED2STC, 0X001770);
    afe44xxWrite(LED2STC, 0X0017C0);
    afe44xxWrite(LED2ENDC, 0X001F3E);
    afe44xxWrite(LED2LEDSTC, 0X001770);
    afe44xxWrite(LED2LEDENDC, 0X001F3F);
    // afe44xxWrite(ALED2STC, 0X000000);
    afe44xxWrite(ALED2STC, 0X000050);
    afe44xxWrite(ALED2ENDC, 0X0007CE);
    // afe44xxWrite(LED1STC, 0X0007D0);
    afe44xxWrite(LED1STC, 0X000820);
    afe44xxWrite(LED1ENDC, 0X000F9E);
    afe44xxWrite(LED1LEDSTC, 0X0007D0);
    afe44xxWrite(LED1LEDENDC, 0X000F9F);
    // afe44xxWrite(ALED1STC, 0X000FA0);
    afe44xxWrite(ALED1STC, 0X000FF0);
    afe44xxWrite(ALED1ENDC, 0X00176E);
    // afe44xxWrite(LED2CONVST, 0X000002);
    afe44xxWrite(LED2CONVST, 0X000006);
    afe44xxWrite(LED2CONVEND, 0X0007CF);
    // afe44xxWrite(ALED2CONVST, 0X0007D2);
    afe44xxWrite(ALED2CONVST, 0X0007D6);
    afe44xxWrite(ALED2CONVEND, 0X000F9F);
    // afe44xxWrite(LED1CONVST, 0X000FA2);
    afe44xxWrite(LED1CONVST, 0X000FA6);
    afe44xxWrite(LED1CONVEND, 0X00176F);
    // afe44xxWrite(ALED1CONVST, 0X001772);
    afe44xxWrite(ALED1CONVST, 0X001776);
    afe44xxWrite(ALED1CONVEND, 0X001F3F);
    afe44xxWrite(ADCRSTCNT0, 0X000000);
    // afe44xxWrite(ADCRSTENDCT0, 0X000000);
    afe44xxWrite(ADCRSTENDCT0, 0X000005);
    afe44xxWrite(ADCRSTCNT1, 0X0007D0);
    // afe44xxWrite(ADCRSTENDCT1, 0X0007D0);
    afe44xxWrite(ADCRSTENDCT1, 0X0007D5);
    afe44xxWrite(ADCRSTCNT2, 0X000FA0);
    // afe44xxWrite(ADCRSTENDCT2, 0X000FA0);
    afe44xxWrite(ADCRSTENDCT2, 0X000FA5);
    afe44xxWrite(ADCRSTCNT3, 0X001770);
    // afe44xxWrite(ADCRSTENDCT3, 0X001770);
    afe44xxWrite(ADCRSTENDCT3, 0X001775);
    afe44xxWrite(PRPCOUNT, 0X001F3F);
    // afe44xxWrite(CONTROL1, 0x010707); // Timers ON, average 3 samples
    afe44xxWrite(CONTROL1, 0x0101);
    afe44xxWrite(SPARE1, 0x0000);
    afe44xxWrite(TIAGAIN, 0x0000);
    // afe44xxWrite(TIA_AMB_GAIN, 0x000001);
    afe44xxWrite(TIA_AMB_GAIN, 0x000000);
    // afe44xxWrite(LEDCNTRL, 0x001414);
    afe44xxWrite(LEDCNTRL, 0x11414);
    afe44xxWrite(CONTROL2, 0x000000);
    // LED_RANGE=100mA, LED=50mA
    // afe44xxWrite(SPARE2, 0x0000);
    // afe44xxWrite(SPARE3, 0x0000);
    // afe44xxWrite(SPARE4, 0x0000);
    // afe44xxWrite(ALARM, 0x0000);
    // afe44xxWrite(LED2VAL, 0x0000);
    // afe44xxWrite(ALED2VAL, 0x0000);
    // delay(1000);
}

void AFE44XX ::afe44xxWrite(uint8_t address, uint32_t data)
{
    SPI.beginTransaction(SPI_SETTINGS_SPO2);
    digitalWrite(_cs_pin, LOW);        // enable device
    SPI.transfer(address);             // send address to device
    SPI.transfer((data >> 16) & 0xFF); // write top 8 bits
    SPI.transfer((data >> 8) & 0xFF);  // write middle 8 bits
    SPI.transfer(data & 0xFF);         // write bottom 8 bits
    digitalWrite(_cs_pin, HIGH);       // disable device
    SPI.endTransaction();
}

unsigned long AFE44XX ::afe44xxRead(uint8_t address)
{
    unsigned long data = 0;

    SPI.beginTransaction(SPI_SETTINGS_SPO2);
    digitalWrite(_cs_pin, LOW);                     // enable device
    SPI.transfer(address);                          // send address to device
    data |= ((unsigned long)SPI.transfer(0) << 16); // read top 8 bits data
    data |= ((unsigned long)SPI.transfer(0) << 8);  // read middle 8 bits  data
    data |= SPI.transfer(0);                        // read bottom 8 bits data
    digitalWrite(_cs_pin, HIGH);                    // disable device
    SPI.endTransaction();

    return data; // return with 24 bits of read data
}

void hr_algo::initStatHRM(void)
{
    unsigned char i;

    // Init HR variables
    lastPeak = 0;
    lastOnset = 0;
    movingWindowHP = 0;
    movingWindowCount = 0;

    for (i = 20; i >= 1; i--)
        peakWindowHP[(unsigned char)(i - 1)] = 0;

    for (i = 12; i >= 1; i--)
        HR[(unsigned char)(i - 1)] = 0;

    // Sampling frequency
    freq = 125;
    // Moving average window size (removes high frequency noise)
    movingWindowSize = freq / 50;
    // Length of the shortest pulse possible
    smallest = freq * 60 / 220;
    foundPeak = 0;
    totalFoundPeak = 0;
    HeartRate = 0;
}

void hr_algo::statHRMAlgo(unsigned long ppgData)
{
    static const int buffer_len = 128;
    static unsigned long ir_buffer[buffer_len] = {0};
    static int ir_index = 0;

    static unsigned long time_buffer[10] = {0};
    static int peak_count = 0;

    static unsigned long timestamp = 0;
    static unsigned long last_hr_time = 0;

    static bool buffer_full = false;

    // Guardar muestra en el buffer circular
    ir_buffer[ir_index] = ppgData;
    ir_index = (ir_index + 1) % buffer_len;
    if (ir_index == 0) buffer_full = true;

    timestamp = millis();

    // Aplicar mediana (ventana de 5 muestras)
    unsigned long win[5];
    for (int i = 0; i < 5; i++) {
        int idx = (ir_index - i - 1 + buffer_len) % buffer_len;
        win[i] = ir_buffer[idx];
    }
    // Ordenar la ventana para obtener la mediana
    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 5; j++) {
            if (win[j] < win[i]) {
                unsigned long temp = win[i];
                win[i] = win[j];
                win[j] = temp;
            }
        }
    }
    unsigned long filtered_ppg = win[2];  // (aunque no usamos el filtrado más allá de almacenar)

    // Detección de pico (máximo local en ventana de 11)
    if (buffer_full) {
        int center = (ir_index - 6 + buffer_len) % buffer_len;
        bool is_peak = true;
        for (int i = -5; i <= 5; i++) {
            if (i == 0) continue;
            int idx = (center + i + buffer_len) % buffer_len;
            if (ir_buffer[center] <= ir_buffer[idx]) {
                is_peak = false;
                break;
            }
        }

        // Validar si es un nuevo pico (con separación mínima)
        if (is_peak && (timestamp - last_hr_time > 400)) {
            last_hr_time = timestamp;

            time_buffer[peak_count % 10] = timestamp;
            peak_count++;

            if (peak_count >= 2) {
                float rr_sum = 0;
                int valid_rr = 0;
                for (int i = 1; i < peak_count && i < 10; i++) {
                    int j = (peak_count - i + 9) % 10;
                    int k = (peak_count - i - 1 + 9) % 10;
                    float rr = (time_buffer[j] - time_buffer[k]) / 1000.0;
                    if (rr > 0.3 && rr < 2.0) {
                        rr_sum += rr;
                        valid_rr++;
                    }
                }
                if (valid_rr > 0) {
                    float mean_rr = rr_sum / valid_rr;
                    int bpm = (int)(60.0 / mean_rr);
                    if (bpm >= 40 && bpm <= 220) {
                        HeartRate = bpm;
                    }
                }
            }
        }
    }
}


void hr_algo::updateWindow(unsigned long *peakWindow, unsigned long Y, unsigned char n)
{
    // Moving average buffer for LED data
    unsigned char i;
    for (i = 20; i >= 1; i--)
    {
        peakWindow[i] = peakWindow[(unsigned char)(i - 1)];
    }
    peakWindow[0] = (Y / n);
}

unsigned char hr_algo::chooseRate(unsigned char *rate)
{
    // Returns the average rate, after removing the lowest and highest values (based on the number of found HR removing 2-4-6 values).
    unsigned char max, min, i, nb;
    unsigned int sum, fullsum;
    max = rate[0];
    min = rate[0];
    sum = 0;
    nb = 0;
    for (i = 7; i >= 1; i--)
    {
        if (rate[(unsigned int)(i - 1)] > 0)
        {
            if (rate[(unsigned int)(i - 1)] > max)
            {
                max = rate[(unsigned int)(i - 1)];
            }
            if (rate[(unsigned int)(i - 1)] < min)
            {
                min = rate[(unsigned int)(i - 1)];
            }
            sum += rate[(unsigned int)(i - 1)];
            nb++;
        }
    }

    if (nb > 2)
        fullsum = (sum - max - min) * 10 / (nb - 2);
    else if (nb)
        fullsum = (sum)*10 / (nb);

    sum = fullsum / 10;

    if (fullsum - sum * 10 > 4)
        sum++;
    return sum;
}

void hr_algo::updateHeartRate(unsigned char *rate, unsigned int freq, unsigned int last)
{
    // Adds a new Heart rate into the array and lose the oldest
    unsigned char i;
    i = 60 * freq / last;
    if ((i > 40) && (i < 220))
    {
        for (i = 11; i >= 1; i--)
        {
            rate[i] = rate[(unsigned char)(i - 1)];
        }
        rate[0] = 60 * freq / last;
    }
}

unsigned long hr_algo::findMax(unsigned long *X)
{
    // Finds the maximum around the center of the buffer
    unsigned long res = X[8];
    unsigned char i;
    for (i = 12; i >= 9; i--)
    {
        if (res < X[i])
            res = X[i];
    }
    return res;
}

unsigned long hr_algo::findMin(unsigned long *X)
{
    // Finds the minimum around the center of the buffer
    unsigned long res = X[8];
    unsigned char i;
    for (i = 12; i >= 9; i--)
    {
        if (res > X[i])
            res = X[i];
    }
    return res;
}

void spo2_algorithm::estimate_spo2(
    uint16_t *pun_ir_buffer,
    int32_t n_ir_buffer_length,
    uint16_t *pun_red_buffer,
    int32_t *pn_spo2,
    int8_t *pch_spo2_valid,
    int32_t *pn_heart_rate,
    int8_t *pch_hr_valid)
{
    *pn_spo2 = -999;
    *pn_heart_rate = -999;
    *pch_spo2_valid = 0;
    *pch_hr_valid = 0;

    int32_t k, i;
    int32_t ir_mean = 0;
    int32_t an_ir_valley_locs[15] = {0};
    int32_t n_npks = 0;
    int32_t peak_interval_sum = 0;

    // Calcular media de IR (DC)
    for (k = 0; k < n_ir_buffer_length; k++)
        ir_mean += pun_ir_buffer[k];
    ir_mean /= n_ir_buffer_length;

    // Centrar e invertir la señal IR
    for (k = 0; k < n_ir_buffer_length; k++)
        an_x[k] = -1 * (pun_ir_buffer[k] - ir_mean);

    // Suavizado con media móvil 4
    for (k = 0; k < BUFFER_SIZE - MA4_SIZE; k++)
        an_x[k] = (an_x[k] + an_x[k+1] + an_x[k+2] + an_x[k+3]) / 4;

    // Calcular umbral de detección de picos (valores invertidos = valles)
    int32_t threshold = 0;
    for (k = 0; k < BUFFER_SIZE; k++)
        threshold += an_x[k];
    threshold /= BUFFER_SIZE;
    if (threshold < 30) threshold = 30;
    if (threshold > 60) threshold = 60;

    // Buscar picos en la señal IR invertida (valleys)
    find_peak(an_ir_valley_locs, &n_npks, an_x, BUFFER_SIZE, threshold, 4, 15);

    if (n_npks >= 2) {
        for (k = 1; k < n_npks; k++)
            peak_interval_sum += (an_ir_valley_locs[k] - an_ir_valley_locs[k - 1]);

        peak_interval_sum /= (n_npks - 1);
        *pn_heart_rate = (int32_t)((SF_spo2 * 60) / peak_interval_sum);
        *pch_hr_valid = 1;
    } else {
        *pn_heart_rate = -999;
        *pch_hr_valid = 0;
    }

    // Copiar señales originales sin invertir
    for (k = 0; k < n_ir_buffer_length; k++) {
        an_x[k] = pun_ir_buffer[k]; // IR
        an_y[k] = pun_red_buffer[k]; // RED
    }

    // Estimación de SpO2 por método ratio
    int32_t ratios[5] = {0};
    int32_t ratio_count = 0;

    for (k = 0; k < n_npks - 1; k++) {
        int32_t ir_max = -1, red_max = -1;
        int32_t ir_min = 0x7FFFFFFF, red_min = 0x7FFFFFFF;

        int32_t start = an_ir_valley_locs[k];
        int32_t end = an_ir_valley_locs[k+1];
        if (end - start < 4 || end >= BUFFER_SIZE) continue;

        for (i = start; i < end; i++) {
            if (an_x[i] > ir_max) ir_max = an_x[i];
            if (an_x[i] < ir_min) ir_min = an_x[i];
            if (an_y[i] > red_max) red_max = an_y[i];
            if (an_y[i] < red_min) red_min = an_y[i];
        }

        int32_t ac_ir = ir_max - ir_min;
        int32_t ac_red = red_max - red_min;
        int32_t dc_ir = ir_max;
        int32_t dc_red = red_max;

        if (ac_ir > 5 && ac_red > 5 && dc_ir > 100 && dc_red > 100 && ratio_count < 5) {
            int32_t ratio = (ac_red * dc_ir * 100) / (ac_ir * dc_red);
            ratios[ratio_count++] = ratio;
        }
    }

    if (ratio_count == 0) {
        *pn_spo2 = -999;
        *pch_spo2_valid = 0;
        return;
    }

    // Media de ratios o mediana
    sort_ascend(ratios, ratio_count);
    int32_t ratio_avg = (ratio_count % 2 == 0) ?
        (ratios[ratio_count/2 - 1] + ratios[ratio_count/2]) / 2 :
        ratios[ratio_count/2];

    // Tabla lookup
    if (ratio_avg > 2 && ratio_avg < 184) {
        *pn_spo2 = uch_spo2_table[ratio_avg];
        *pch_spo2_valid = 1;
    } else {
        *pn_spo2 = -999;
        *pch_spo2_valid = 0;
    }
}

void spo2_algorithm ::find_peak(int32_t *pn_locs, int32_t *n_npks, int32_t *pn_x, int32_t n_size, int32_t n_min_height, int32_t n_min_distance, int32_t n_max_num)
/**
  \brief        Find peaks
  \par          Details
                Find at most MAX_NUM peaks above MIN_HEIGHT separated by at least MIN_DISTANCE

  \retval       None
*/
{
    find_peak_above(pn_locs, n_npks, pn_x, n_size, n_min_height);
    remove_close_peaks(pn_locs, n_npks, pn_x, n_min_distance);
    *n_npks = min(*n_npks, n_max_num);
}

void spo2_algorithm ::find_peak_above(int32_t *pn_locs, int32_t *n_npks, int32_t *pn_x, int32_t n_size, int32_t n_min_height)
/**
  \brief        Find peaks above n_min_height
  \par          Details
                Find all peaks above MIN_HEIGHT

  \retval       None
*/
{
    int32_t i = 1, n_width;
    *n_npks = 0;

    while (i < n_size - 1)
    {

        if (pn_x[i] > n_min_height && pn_x[i] > pn_x[i - 1])
        { // find left edge of potential peaks
            n_width = 1;

            while (i + n_width < n_size && pn_x[i] == pn_x[i + n_width]) // find flat peaks
            {
                n_width++;
            }

            if (pn_x[i] > pn_x[i + n_width] && (*n_npks) < 15)
            { // find right edge of peaks
                pn_locs[(*n_npks)++] = i;
                // for flat peaks, peak location is left edge
                i += n_width + 1;
            }
            else
            {
                i += n_width;
            }
        }
        else
        {
            i++;
        }
    }
}

void spo2_algorithm ::remove_close_peaks(int32_t *pn_locs, int32_t *pn_npks, int32_t *pn_x, int32_t n_min_distance)
/**
  \brief        Remove peaks
  \par          Details
                Remove peaks separated by less than MIN_DISTANCE

  \retval       None
*/
{
    int32_t i, j, n_old_npks, n_dist; /* Order peaks from large to small */
    sort_indices_descend(pn_x, pn_locs, *pn_npks);

    for (i = -1; i < *pn_npks; i++)
    {
        n_old_npks = *pn_npks;
        *pn_npks = i + 1;

        for (j = i + 1; j < n_old_npks; j++)
        {
            n_dist = pn_locs[j] - (i == -1 ? -1 : pn_locs[i]); // lag-zero peak of autocorr is at index -1

            if (n_dist > n_min_distance || n_dist < -n_min_distance)
            {
                pn_locs[(*pn_npks)++] = pn_locs[j];
            }
        }
    }
    // Resort indices int32_to ascending order
    sort_ascend(pn_locs, *pn_npks);
}

void spo2_algorithm ::sort_ascend(int32_t *pn_x, int32_t n_size)
/**
  \brief        Sort array
  \par          Details
                Sort array in ascending order (insertion sort algorithm)

  \retval       None
*/
{
    int32_t i, j, n_temp;

    for (i = 1; i < n_size; i++)
    {
        n_temp = pn_x[i];

        for (j = i; j > 0 && n_temp < pn_x[j - 1]; j--)
        {
            pn_x[j] = pn_x[j - 1];
        }
        pn_x[j] = n_temp;
    }
}

void spo2_algorithm ::sort_indices_descend(int32_t *pn_x, int32_t *pn_indx, int32_t n_size)
/**
  \brief        Sort indices
  \par          Details
                Sort indices according to descending order (insertion sort algorithm)

  \retval       None
*/
{
    int32_t i, j, n_temp;

    for (i = 1; i < n_size; i++)
    {
        n_temp = pn_indx[i];

        for (j = i; j > 0 && pn_x[n_temp] > pn_x[pn_indx[j - 1]]; j--)
        {
            pn_indx[j] = pn_indx[j - 1];
        }
        pn_indx[j] = n_temp;
    }
}