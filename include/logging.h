#ifndef LOGGING_H
#define LOGGING_H

/*
Esta classe vai ter uma ou mais funções que se vão responsabilizar em
escrever os dados num ficheiro pedidos por VD (Lemos):
  BMS:
  int soc -> state of charge;
  int current -> current;
  int packVoltage -> pack voltage;
  
  BAMOCAR:
  int Nact -> Actual speed value filtered;
  int Vout -> Output-voltage usage;
  int Iq_cmd -> command current;
  int Iq_actual -> Q-current actual;
  int Mout -> Digital Torque Intern;
  int I_lim_inuse -> actual current limit;
  int I_actual_filtered -> Filtered actual current;
  int Tpeak -> Timing for peak current;
  int Imax_peak -> Limit for peak current;
  int I_con_eff -> Limit for continius current;
  int motorTemp -> motor temperature;
  int powerStageTemp -> power stage temperature;
  int ACCurrent -> current actual value;


Tenho de perguntar ao Lemos se é a corrente do BAMO ou a do BMS que eles querem
*/

#include <SD.h>
#include <FlexCAN_T4.h>
#include <SPI.h>

#define PIN_SPI_CLK 45
#define PIN_SPI_MOSI 43
#define PIN_SPI_MISO 42
#define PIN_SD_CS 44
// If you have connected other SPI device then
// put here number of pin for disable its.
// Provide -1 if you don't have other devices.
#define PIN_OTHER_DEVICE_CS -1
// Change this value if you have problems with SD card
// Available values: SPI_QUARTER_SPEED //SPI_HALF_SPEED
// It is enum from SdFat
#define SD_CARD_SPEED SPI_FULL_SPEED

#define BMS_ID 0x666

struct LogEntry
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    uint32_t millisecond;
};

class Logging {

 public:
  
  void write_to_file(int current, int voltage, int mintmp, int maxtmp, int avgtmp, int apps1, int apps2, int brake);
  void setup_log();
 
 private:

};

#endif