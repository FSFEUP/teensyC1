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

#include <SdFat.h>
#include <FlexCAN_T4.h>
#include <CSVFile.h>


#define PIN_SD_CS 44                  //! Não sei se está certo, coloquei para remover os erros de compilação
#define SD_CARD_SPEED SD_SCK_MHZ(50)
// SPI pinout
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


#define BAMO_RESPONSE_ID 0x181
#define BMS_ID 0x675
#define BAMO_COMMAND_ID 0x201

class Logging {

 public:
  
  void set_CAN_messages(); //sets all the messages in the private field
  void write_to_file();
  void setup_log();

  CAN_message_t get_Nact_filtered();
  CAN_message_t get_Vout_msg();
  CAN_message_t get_Iq_cmd_msg();
  CAN_message_t get_Iq_actual_msg();
  CAN_message_t get_Mout_msg();
  CAN_message_t get_I_lim_inuse_msg();
  CAN_message_t get_I_actual_filtered_msg();
  CAN_message_t get_Tpeak_msg();
  CAN_message_t get_Imax_peak_msg();
  CAN_message_t get_I_con_eff_msg();
  CAN_message_t get_powerStageTempRequest();
  CAN_message_t get_motorTempRequest();

  int get_soc();
  int get_current();
  int get_packVoltage();
  int get_Nact();
  int get_Vout();
  int get_Iq_cmd();
  int get_Iq_actual();
  int get_Mout();
  int get_I_lim_inuse();
  int get_I_actual_filtered();
  int get_Tpeak();
  int get_Imax_peak();
  int get_I_con_eff();
  int get_motorTemp();
  int get_powerStageTemp();
  int get_ACCurrent();

  void set_soc(int a);
  void set_current(int a);
  void set_packVoltage(int a);
  void set_Nact(int a);
  void set_Vout(int a);
  void set_Iq_cmd(int a);
  void set_Iq_actual(int a);
  void set_Mout(int a);
  void set_I_lim_inuse(int a);
  void set_I_actual_filtered(int a);
  void set_Tpeak(int a);
  void set_Imax_peak(int a);
  void set_I_con_eff(int a);
  void set_motorTemp(int a);
  void set_powerStageTemp(int a);
  void set_ACCurrent(int a);

  void increment_t();
 
 private:
  
  CAN_message_t Nact_filtered;
  CAN_message_t Vout_msg;
  CAN_message_t Iq_cmd_msg;
  CAN_message_t Iq_actual_msg;
  CAN_message_t Mout_msg;
  CAN_message_t I_lim_inuse_msg;
  CAN_message_t I_actual_filtered_msg;
  CAN_message_t Tpeak_msg;
  CAN_message_t Imax_peak_msg;
  CAN_message_t I_con_eff_msg;
  CAN_message_t powerStageTempRequest;
  CAN_message_t motorTempRequest;


  int soc;
  int current;
  int packVoltage;
  int Nact;
  int Vout;
  int Iq_cmd;
  int Iq_actual;
  int Mout;
  int I_lim_inuse;
  int I_actual_filtered;
  int Tpeak;
  int Imax_peak;
  int I_con_eff;
  int motorTemp;
  int powerStageTemp;
  int ACCurrent;

  SdFat sd;
  elapsedMillis writeTIMER;


  int t = 0;
};

#endif