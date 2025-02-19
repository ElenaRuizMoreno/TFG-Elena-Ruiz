/*
  MIT License

  Copyright (c) 2022 Medical Open World, Pablo Sánchez Bergasa

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

// Firmware version and head title of UI screen

#include "main.h"

#include <Arduino.h>

TwoWire *wire;
MAM_in3ator_Humidifier in3_hum(DEFAULT_ADDRESS);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
SHTC3 mySHTC3; // Declare an instance of the SHTC3 class
Adafruit_SHT4x sht4 = Adafruit_SHT4x();
RotaryEncoder encoder(ENC_A, ENC_B, RotaryEncoder::LatchMode::TWO03);
Beastdevices_INA3221 mainDigitalCurrentSensor(INA3221_ADDR41_VCC);
Beastdevices_INA3221 secundaryDigitalCurrentSensor(INA3221_ADDR40_GND);

bool WIFI_EN = true;
long lastDebugUpdate;
long loopCounts;
int page;
long lastNTCmeasurement;

double errorTemperature[SENSOR_TEMP_QTY], temperatureCalibrationPoint;
double ReferenceTemperatureRange, ReferenceTemperatureLow;
double provisionalReferenceTemperatureLow;
double fineTuneSkinTemperature, fineTuneAirTemperature;
float diffSkinTemperature,
    diffAirTemperature; // difference between measured temperature and user
                        // input real temperature
double RawTemperatureLow[SENSOR_TEMP_QTY], RawTemperatureRange[SENSOR_TEMP_QTY];
double provisionalRawTemperatureLow[SENSOR_TEMP_QTY];
double temperatureMax[SENSOR_TEMP_QTY], temperatureMin[SENSOR_TEMP_QTY];
int temperature_array_pos; // temperature sensor number turn to measure
bool humidifierState, humidifierStateChange;
int previousHumidity; // previous sampled humidity
float diffHumidity;   // difference between measured humidity and user input real
                      // humidity

byte autoCalibrationProcess;

// Sensor check rate (in ms). Both sensors are checked in same interrupt and
// they have different check rates
byte encoderRate = true;
byte encoderCount = false;

volatile long lastEncPulse;
volatile bool statusEncSwitch;

bool roomSensorPresent = false;
bool ambientSensorPresent = false;
bool digitalCurrentSensorPresent[2];

// room variables
float minDesiredTemp[2] = {35, 20};   // minimum allowed temperature to be set
float maxDesiredTemp[2] = {37.5, 37}; // maximum allowed temperature to be set
int presetTemp[2] = {36, 32};         // preset baby skin temperature

boolean A_set;
boolean B_set;
int encoderpinA = ENC_A;         // pin  encoder A
int encoderpinB = ENC_B;         // pin  encoder B
bool encPulsed, encPulsedBefore; // encoder switch status
bool updateUIData;
volatile int EncMove;                 // moved encoder
volatile int lastEncMove;             // moved last encoder
volatile int EncMoveOrientation = -1; // set to -1 to increase values clockwise
volatile int last_encoder_move;       // moved encoder
long encoder_debounce_time =
    true;            // in milliseconds, debounce time in encoder to filter signal bounces
long last_encPulsed; // last time encoder was pulsed

// Text Graphic position variables
int humidityX;
int humidityY;
int temperatureX;
int temperatureY;
int separatorTopYPos, separatorMidYPos, separatorBotYPos;
int ypos;
bool print_text;
int initialSensorPosition = separatorPosition - letter_width;
bool pos_text[8];

bool enableSet;
float temperaturePercentage, temperatureAtStart;
float humidityPercentage, humidityAtStart;
int barWidth, barHeight, tempBarPosX, tempBarPosY, humBarPosX, humBarPosY;
int screenTextColor, screenTextBackgroundColor;

// User Interface display variables
bool goToSettings = false;
bool autoLock;             // setting that enables backlight switch OFF after a given time
                           // of no user actions
long lastbacklightHandler; // last time there was a encoder movement or pulse
long sensorsUpdatePeriod = 1000;

bool selected;
char cstring[128];
char *textToWrite;
char *words[12];
char *helpMessage;
byte bar_pos = true;
byte menu_rows;
byte length;
long lastGraphicSensorsUpdate;
long lastSensorsUpdate;
bool enableSetProcess;
long blinking;
bool state_blink;
bool blinkSetMessageState;
long lastBlinkSetMessage;

long lastSuccesfullSensorUpdate[SENSOR_TEMP_QTY];

int ScreenBacklightMode;
long lastRoomSensorUpdate, lastCurrentSensorUpdate;

int testNumber = false;

int pulsioximeterTestArray[] = {70, 71, 75, 90, 65, 76, 43, 65, 75, 80, 90, 87, 76, 66};

in3ator_parameters in3;

QueueHandle_t sharedSensorQueue;
SemaphoreHandle_t spi_semaphore;

AFE44XX afe44xx(AFE44XX_CS, AFE44XX_PWDN_PIN);

afe44xx_data afe44xx_raw_data;
int32_t heart_rate_prev = 0;
int32_t spo2_prev = 0;

void GPRS_Task(void *pvParameters)
{
  initGPRS();
  GPRS_TB_Init();
  for (;;)
  {
    if (!WIFIIsConnected())
    {
      GPRS_Handler();
    }
    vTaskDelay(pdMS_TO_TICKS(GPRS_TASK_PERIOD));
  }
}

void Backlight_Task(void *pvParameters)
{
  for (;;)
  {
    backlightHandler();
    vTaskDelay(pdMS_TO_TICKS(BACKLIGHT_TASK_PERIOD));
  }
}

void sensors_Task(void *pvParameters)
{
  for (;;)
  {
    fanSpeedHandler();
    measureNTCTemperature();
    if (millis() - lastRoomSensorUpdate > ROOM_SENSOR_UPDATE_PERIOD)
    {
      updateRoomSensor();
      updateAmbientSensor();
      lastRoomSensorUpdate = millis();
    }
    if (millis() - lastCurrentSensorUpdate > DIGITAL_CURRENT_SENSOR_PERIOD)
    {
      powerMonitor();
      lastCurrentSensorUpdate = millis();
    }
    vTaskDelay(pdMS_TO_TICKS(SENSORS_TASK_PERIOD));
  }
}

void OTA_Task(void *pvParameters)
{
  WIFI_TB_Init();
  for (;;)
  {
    WifiOTAHandler();
    vTaskDelay(pdMS_TO_TICKS(OTA_TASK_PERIOD));
  }
}

void buzzer_Task(void *pvParameters)
{
  for (;;)
  {
    buzzerHandler();
    vTaskDelay(pdMS_TO_TICKS(BUZZER_TASK_PERIOD));
  }
}

void security_Task(void *pvParameters)
{
  for (;;)
  {
    if (ALARM_SYSTEM_ENABLED && in3.alarmsEnabled)
    {
      securityCheck();
    }
    vTaskDelay(pdMS_TO_TICKS(SECURITY_TASK_PERIOD));
  }
}

void UI_Task(void *pvParameters)
{
  if (goToSettings)
  {
    UI_settings();
  }
  else
  {
    UI_mainMenu();
  }
  for (;;)
  {
    xSemaphoreTake(spi_semaphore, portMAX_DELAY);
    userInterfaceHandler(page);
    xSemaphoreGive(spi_semaphore);
    vTaskDelay(pdMS_TO_TICKS(SECURITY_TASK_PERIOD));
  }
}

bool afe4490_ADC_ready = false;
long timesInTask = false;

void SPO2_Task(void *pvParameters)
{
  SPI.begin();
  pinMode(AFE4490_ADC_READY, INPUT_PULLUP);
  afe44xx.afe44xx_init();
  attachInterrupt(AFE4490_ADC_READY, afe4490ADCReady, RISING);
  for (;;)
  {
    if (afe4490_ADC_ready)
    {
      afe4490_ADC_ready = false;
      // xSemaphoreTake(spi_semaphore, portMAX_DELAY);
      afe44xx.get_AFE44XX_Data(&afe44xx_raw_data);

      if (afe44xx_raw_data.buffer_count_overflow)
      {
        if (afe44xx_raw_data.spo2 == -999)
        {
          // Serial.println("Probe error!!!!");
        }
        else if ((heart_rate_prev != afe44xx_raw_data.heart_rate) || (spo2_prev != afe44xx_raw_data.spo2))
        {
          heart_rate_prev = afe44xx_raw_data.heart_rate;
          spo2_prev = afe44xx_raw_data.spo2;

          Serial.print("calculating sp02...");
          Serial.print(" Sp02 : ");
          Serial.print(afe44xx_raw_data.spo2);
          Serial.print("% ,");
          Serial.print("Pulse rate :");
          Serial.print(afe44xx_raw_data.heart_rate);
          Serial.println(" bpm");
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

void setup()
{
  Serial.begin(115200);
  sharedSensorQueue = xQueueCreate(SENSOR_TEMP_QTY, sizeof(long));
  spi_semaphore = xSemaphoreCreateBinary();

  // if (!GPIORead(ENC_SWITCH))
  // {
  //   goToSettings = true;
  // }
  // initHardware(false);

  // logI("Creating UI task ...\n");
  // while (xTaskCreatePinnedToCore(UI_Task, (const char *)"UI", 4096,
  //                                NULL, UI_TASK_PRIORITY, NULL, CORE_ID_FREERTOS) != pdPASS)
  //   ;
  // ;
  // logI("UI task successfully created!\n");

  // logI("Creating buzzer task ...\n");
  // while (xTaskCreatePinnedToCore(buzzer_Task, (const char *)"BUZZER", 4096,
  //                                NULL, BUZZER_TASK_PRIORITY, NULL, CORE_ID_FREERTOS) != pdPASS)
  //   ;
  // ;
  // logI("Buzzer task successfully created!\n");
  // logI("Creating sensors task ...\n");
  // while (xTaskCreatePinnedToCore(sensors_Task, (const char *)"SENSORS", 4096,
  //                                NULL, SENSORS_TASK_PRIORITY, NULL, CORE_ID_FREERTOS) != pdPASS)
  //   ;
  // ;
  // logI("sensors task successfully created!\n");
  // Task generation
  // logI("Creating security task ...\n");
  // while (xTaskCreatePinnedToCore(security_Task, (const char *)"SECURITY", 4096,
  //                                NULL, SECURITY_TASK_PRIORITY, NULL, CORE_ID_FREERTOS) != pdPASS)
  //   ;
  // ;
  // logI("sensors task successfully created!\n");
  // logI("Creating GPRS task ...\n");
  // while (xTaskCreatePinnedToCore(GPRS_Task, (const char *)"GPRS", 8192, NULL, GPRS_TAST_PRIORITY,
  //                                NULL, CORE_ID_FREERTOS) != pdPASS)
  //   ;
  // logI("GPRS task successfully created!\n");

  // logI("Creating OTA task ...\n");
  // while (xTaskCreatePinnedToCore(OTA_Task, (const char *)"OTA", 8192, NULL, OTA_TASK_PRIORITY,
  //                                NULL, CORE_ID_FREERTOS) != pdPASS)
  //   ;
  // logI("OTA task successfully created!\n");

  // logI("Creating Backlight task ...\n");
  // while (xTaskCreatePinnedToCore(Backlight_Task, (const char *)"BACKLIGHT",
  //                                4096, NULL, BACKLIGHT_TASK_PRIORITY, NULL,
  //                                CORE_ID_FREERTOS) != pdPASS)
  //   ;
  // ;
  // logI("Backlight task successfully created!\n");
  logI("Creating SPO2 task ...\n");
  while (xTaskCreatePinnedToCore(SPO2_Task, (const char *)"SPO2",
                                 4096, NULL, SPO2_TASK_PRIORITY, NULL,
                                 CORE_ID_FREERTOS) != pdPASS)
    ;
  ;
  logI("SPO2 task successfully created!\n");
  /*
  logI("Creating time track task ...\n");
  while (xTaskCreatePinnedToCore(time_track_Task, (const char *)"SENSORS", 4096,
  NULL, 1, NULL, CORE_ID_FREERTOS) != pdPASS)
    ;
  ;
  logI("Time track task successfully created!\n");
  */
}

void loop()
{
  // watchdogReload();
  // timeTrackHandler();

  // xSemaphoreGive(spi_semaphore);

  // vTaskDelay(pdMS_TO_TICKS(LOOP_TASK_PERIOD));
}
