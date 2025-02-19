#ifndef _AFE4490
#define _AFE4490

#include "Arduino.h"
#include <SPI.h>
#include <string.h>
#include <math.h>
#include "main.h"

#define SF_spo2 25 // sampling frequency
#define BUFFER_SIZE (SF_spo2 * 4)
#define MA4_SIZE 4 // DONOT CHANGE
#define min(x, y) ((x) < (y) ? (x) : (y))

#define SPO2_INIT_TIME 1000

// afe44xx Register definition
#define CONTROL0 0x00
#define LED2STC 0x01
#define LED2ENDC 0x02
#define LED2LEDSTC 0x03
#define LED2LEDENDC 0x04
#define ALED2STC 0x05
#define ALED2ENDC 0x06
#define LED1STC 0x07
#define LED1ENDC 0x08
#define LED1LEDSTC 0x09
#define LED1LEDENDC 0x0a
#define ALED1STC 0x0b
#define ALED1ENDC 0x0c
#define LED2CONVST 0x0d
#define LED2CONVEND 0x0e
#define ALED2CONVST 0x0f
#define ALED2CONVEND 0x10
#define LED1CONVST 0x11
#define LED1CONVEND 0x12
#define ALED1CONVST 0x13
#define ALED1CONVEND 0x14
#define ADCRSTCNT0 0x15
#define ADCRSTENDCT0 0x16
#define ADCRSTCNT1 0x17
#define ADCRSTENDCT1 0x18
#define ADCRSTCNT2 0x19
#define ADCRSTENDCT2 0x1a
#define ADCRSTCNT3 0x1b
#define ADCRSTENDCT3 0x1c
#define PRPCOUNT 0x1d
#define CONTROL1 0x1e
#define SPARE1 0x1f
#define TIAGAIN 0x20
#define TIA_AMB_GAIN 0x21
#define LEDCNTRL 0x22
#define CONTROL2 0x23
#define SPARE2 0x24
#define SPARE3 0x25
#define SPARE4 0x26
#define SPARE4 0x26
#define RESERVED1 0x27
#define RESERVED2 0x28
#define ALARM 0x29
#define LED2VAL 0x2a
#define ALED2VAL 0x2b
#define LED1VAL 0x2c
#define ALED1VAL 0x2d
#define LED2ABSVAL 0x2e
#define LED1ABSVAL 0x2f
#define DIAG 0x30

class hr_algo
{
public:
  void initStatHRM(void);
  void statHRMAlgo(unsigned long ppgData);

  void updateWindow(unsigned long *peakWindow, unsigned long Y, unsigned char n);
  unsigned char chooseRate(unsigned char *rate);
  void updateHeartRate(unsigned char *rate, unsigned int freq, unsigned int last);
  unsigned long findMax(unsigned long *X);
  unsigned long findMin(unsigned long *X);

  unsigned char HeartRate = 0;
};

typedef struct afe44xx_Record
{
  int32_t heart_rate;
  int32_t spo2;
  signed long IR_data;
  signed long RED_data;
  signed long AMB_IR_data;
  signed long AMB_RED_data;
  boolean buffer_count_overflow = false;
} afe44xx_data;

class AFE44XX
{
public:
  AFE44XX(int cs_pin, int pwdn_pin);
  void afe44xx_init();

  boolean get_AFE44XX_Data(afe44xx_data *afe44xx_raw_data);

private:
  void afe44xxWrite(uint8_t address, uint32_t data);
  unsigned long afe44xxRead(uint8_t address);

  int _cs_pin;
  int _pwdn_pin;
  int _drdy_pin;
};

class spo2_algorithm
{
public:
  void estimate_spo2(uint16_t *pun_ir_buffer, int32_t n_ir_buffer_length, uint16_t *pun_red_buffer, int32_t *pn_spo2, int8_t *pch_spo2_valid, int32_t *pn_heart_rate, int8_t *pch_hr_valid);
  void find_peak(int32_t *pn_locs, int32_t *n_npks, int32_t *pn_x, int32_t n_size, int32_t n_min_height, int32_t n_min_distance, int32_t n_max_num);
  void find_peak_above(int32_t *pn_locs, int32_t *n_npks, int32_t *pn_x, int32_t n_size, int32_t n_min_height);
  void remove_close_peaks(int32_t *pn_locs, int32_t *pn_npks, int32_t *pn_x, int32_t n_min_distance);
  void sort_ascend(int32_t *pn_x, int32_t n_size);
  void sort_indices_descend(int32_t *pn_x, int32_t *pn_indx, int32_t n_size);
};
#endif
