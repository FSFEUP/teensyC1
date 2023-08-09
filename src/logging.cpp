#include "logging.h"

extern FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;
File myFile;

CAN_message_t Logging::get_Nact_filtered(){
    return Nact_filtered;
}
CAN_message_t Logging::get_Vout_msg(){
    return Vout_msg;
}
CAN_message_t Logging::get_Iq_cmd_msg(){
    return Iq_cmd_msg;
}
CAN_message_t Logging::get_Iq_actual_msg(){
    return Iq_actual_msg;
}
CAN_message_t Logging::get_Mout_msg(){
    return Mout_msg;
}
CAN_message_t Logging::get_I_lim_inuse_msg(){
    return I_lim_inuse_msg;
}
CAN_message_t Logging::get_I_actual_filtered_msg(){
    return I_actual_filtered_msg;
}
CAN_message_t Logging::get_Tpeak_msg(){
    return Tpeak_msg;
}
CAN_message_t Logging::get_Imax_peak_msg(){
    return Imax_peak_msg;
}
CAN_message_t Logging::get_I_con_eff_msg(){
    return I_con_eff_msg;
}
CAN_message_t Logging::get_powerStageTempRequest(){
    return powerStageTempRequest;
}
CAN_message_t Logging::get_motorTempRequest(){
    return motorTempRequest;
}

uint16_t Logging::get_soc(){
    return soc;
}
uint16_t Logging::get_current(){
    return current;
}
uint16_t Logging::get_packVoltage(){
    return packVoltage;
}
uint16_t Logging::get_Nact(){
    return Nact;
}
uint16_t Logging::get_Vout(){
    return Vout;
}
uint16_t Logging::get_Iq_cmd(){
    return Iq_cmd;
}
uint16_t Logging::get_Iq_actual(){
    return Iq_actual;
}
uint16_t Logging::get_Mout(){
    return Mout;
}
uint16_t Logging::get_I_lim_inuse(){
    return I_lim_inuse;
}
uint16_t Logging::get_I_actual_filtered(){
    return I_actual_filtered;
}
uint16_t Logging::get_Tpeak(){
    return Tpeak;
}
uint16_t Logging::get_Imax_peak(){
    return Imax_peak;
}
uint16_t Logging::get_I_con_eff(){
    return I_con_eff;
}
uint16_t Logging::get_motorTemp(){
    return motorTemp;
}
uint16_t Logging::get_powerStageTemp(){
    return powerStageTemp;
}
uint16_t Logging::get_ACCurrent(){
    return ACCurrent;
}

void Logging::set_soc(int a){
    soc = a;
}
void Logging::set_current(int a){
    current = a;
}
void Logging::set_packVoltage(int a){
    packVoltage = a;
}
void Logging::set_Nact(int a){
    Nact = a;
}
void Logging::set_Vout(int a){
    Vout = a;
}
void Logging::set_Iq_cmd(int a){
    Iq_cmd = a;
}
void Logging::set_Iq_actual(int a){
    Iq_actual = a;
}
void Logging::set_Mout(int a){
    Mout = a;
}
void Logging::set_I_lim_inuse(int a){
    I_lim_inuse = a;
}
void Logging::set_I_actual_filtered(int a){
    I_actual_filtered = a;
}
void Logging::set_Tpeak(int a){
    Tpeak = a;
}
void Logging::set_Imax_peak(int a){
    Imax_peak = a;
}
void Logging::set_I_con_eff(int a){
    I_con_eff = a;
}
void Logging::set_motorTemp(int a){
    motorTemp = a;
}
void Logging::set_powerStageTemp(int a){
    powerStageTemp = a;
}
void Logging::set_ACCurrent(int a){
    ACCurrent = a;
}


void Logging::setup_log() {

    //SPI.setCS(PIN_SD_CS);    
    //SPI.setMISO(PIN_SPI_MISO);
    //SPI.setMOSI(PIN_SPI_MOSI);
    //SPI.setSCK(PIN_SPI_SCK);
    // Setup pinout
    pinMode(PIN_SPI_MOSI, OUTPUT);
    pinMode(PIN_SPI_MISO, INPUT);
    pinMode(PIN_SPI_SCK, OUTPUT);
    // Disable SPI devices
    pinMode(PIN_SD_CS, OUTPUT);
    digitalWrite(PIN_SD_CS, HIGH);


    // Setup serial
    Serial.begin(9600);

    // Setup SD card
    if (!SD.begin(BUILTIN_SDCARD)) {
        Serial.println("initialization failed!");
        return;
    }

    Serial.print("Initialization completed");

    
    set_CAN_messages();

    can2.write(Nact_filtered); 
    can2.write(Vout_msg);
    can2.write(Iq_cmd_msg);
    can2.write(Iq_actual_msg);
    can2.write(Mout_msg);
    can2.write(I_lim_inuse_msg);
    can2.write(I_actual_filtered_msg);
    can2.write(Tpeak_msg);
    can2.write(Imax_peak_msg);
    can2.write(I_con_eff_msg);
    can2.write(motorTempRequest);
    can2.write(powerStageTempRequest);
    
}

void Logging::set_CAN_messages() {
    Nact_filtered.id = BAMO_COMMAND_ID;
    Nact_filtered.len = 3;
    Nact_filtered.buf[0] = 0x3D;
    Nact_filtered.buf[1] = 0xA8;
    Nact_filtered.buf[2] = 0x64;

    Vout_msg.id = BAMO_COMMAND_ID;
    Vout_msg.len = 3;
    Vout_msg.buf[0] = 0x3D;
    Vout_msg.buf[1] = 0x8A;
    Vout_msg.buf[2] = 0x64;

    Iq_cmd_msg.id = BAMO_COMMAND_ID;
    Iq_cmd_msg.len = 3;
    Iq_cmd_msg.buf[0] = 0x3D;
    Iq_cmd_msg.buf[1] = 0x26;
    Iq_cmd_msg.buf[2] = 0x64;

    Iq_actual_msg.id = BAMO_COMMAND_ID;
    Iq_actual_msg.len = 3;
    Iq_actual_msg.buf[0] = 0x3D;
    Iq_actual_msg.buf[1] = 0x27;
    Iq_actual_msg.buf[2] = 0x64;

    Mout_msg.id = BAMO_COMMAND_ID;
    Mout_msg.len = 3;
    Mout_msg.buf[0] = 0x3D;
    Mout_msg.buf[1] = 0xA0;
    Mout_msg.buf[2] = 0x64;

    I_lim_inuse_msg.id = BAMO_COMMAND_ID;
    I_lim_inuse_msg.len = 3;
    I_lim_inuse_msg.buf[0] = 0x3D;
    I_lim_inuse_msg.buf[1] = 0x48;
    I_lim_inuse_msg.buf[2] = 0x64;

    I_actual_filtered_msg.id = BAMO_COMMAND_ID;
    I_actual_filtered_msg.len = 3;
    I_actual_filtered_msg.buf[0] = 0x3D;
    I_actual_filtered_msg.buf[1] = 0x5F;
    I_actual_filtered_msg.buf[2] = 0x64;

    Tpeak_msg.id = BAMO_COMMAND_ID;
    Tpeak_msg.len = 3;
    Tpeak_msg.buf[0] = 0x3D;
    Tpeak_msg.buf[1] = 0xF0;
    Tpeak_msg.buf[2] = 0x64;

    Imax_peak_msg.id = BAMO_COMMAND_ID;
    Imax_peak_msg.len = 3;
    Imax_peak_msg.buf[0] = 0x3D;
    Imax_peak_msg.buf[1] = 0xC4;
    Imax_peak_msg.buf[2] = 0x64;

    I_con_eff_msg.id = BAMO_COMMAND_ID;
    I_con_eff_msg.len = 3;
    I_con_eff_msg.buf[0] = 0x3D;
    I_con_eff_msg.buf[1] = 0xC5;
    I_con_eff_msg.buf[2] = 0x64;

    powerStageTempRequest.id = BAMO_COMMAND_ID;
    powerStageTempRequest.len = 3;
    powerStageTempRequest.buf[0] = 0x3D;
    powerStageTempRequest.buf[1] = 0x4A;
    powerStageTempRequest.buf[0] = 0x64;
    
    motorTempRequest.id = BAMO_COMMAND_ID;
    motorTempRequest.len = 3;
    motorTempRequest.buf[0] = 0x3D;
    motorTempRequest.buf[1] = 0x49;
    motorTempRequest.buf[2] = 0x64;
}

void Logging::write_to_file(int t) {
    
        Serial.print("Starting to write...");

        myFile = SD.open("reading_values.txt", FILE_WRITE);

        myFile.println(t);

        myFile.printf("N act filtered - %d \n",Nact);

        myFile.printf("Vout - %d \n",Vout);

        myFile.printf("Iq command - %d \n",Iq_cmd);

        myFile.printf("Iq actual - %d \n",Iq_actual);

        myFile.printf("M out - %d \n",Mout);

        myFile.printf("I lim inuse - %d \n",I_lim_inuse);

        myFile.printf("I actual filtered - %d \n",I_actual_filtered);

        myFile.printf("T-peak - %d \n",Tpeak);

        myFile.printf("I max peak - %d \n",Imax_peak);
        
        myFile.printf("I con eff - %d \n",I_con_eff);

        myFile.printf("T-motor - %d \n",motorTemp);

        myFile.printf("T-igbt - %d \n",powerStageTemp);

        myFile.printf("SoC - %d \n",soc);

        myFile.printf("V bat - %d \n",packVoltage);

        myFile.printf("I bat - %d \n",current);

        myFile.close();
        

        /*
        if (!csv.open("data.csv", O_RDWR | O_CREAT)) {
            Serial.println("Failed open file");
        }

        Serial.print("File opened");

        csv.write("Hellow World!");

        Serial.print("Printed into the sd hello world");
        
        csv.close();
        */

        /*
        // N act (filt) - 0xA8
        csv.addField(Nact);
        Serial.printf("N act filtered - %d \n",Nact);

        // Vout - 0x8A
        csv.addField(Vout);
        Serial.printf("Vout - %d \n",Vout);

        // Iq cmd - 0x26
        csv.addField(Iq_cmd);
        Serial.printf("Iq command - %d \n",Iq_cmd);

        // Iq actual - 0x27
        csv.addField(Iq_actual);
        Serial.printf("Iq actual - %d \n",Iq_actual);

        // M out - 0xA0
        csv.addField(Mout);
        Serial.printf("M out - %d \n",Mout);

        // I lim inuse - 0x48
        csv.addField(I_lim_inuse);
        Serial.printf("I lim inuse - %d \n",I_lim_inuse);

        // I act (filt) - 0x5F
        csv.addField(I_actual_filtered);
        Serial.printf("I actual filtered - %d \n",I_actual_filtered);

        // T-peak - 0xF0
        csv.addField(Tpeak);
        Serial.printf("T-peak - %d \n",Tpeak);

        // Imax pk - 0xC4
        csv.addField(Imax_peak);
        Serial.printf("I max peak - %d \n",Imax_peak);

        // I con eff - 0xC5
        csv.addField(I_con_eff);
        Serial.printf("I con eff - %d \n",I_con_eff);

        // T-motor - 0x49
        csv.addField(motorTemp);
        Serial.printf("T-motor - %d \n",motorTemp);

        // T-igbt - 0x4A
        csv.addField(powerStageTemp);
        Serial.printf("T-igbt - %d \n",powerStageTemp);

        // SoC
        csv.addField(soc);
        Serial.printf("SoC - %d \n",soc);

        // V bat
        csv.addField(packVoltage);
        Serial.printf("V bat - %d \n",packVoltage);

        // I bat
        csv.addField(current);
        Serial.printf("I bat - %d \n",current);

        csv.addLine();

        // We don't add empty line at the end of file.
        // CSV file shouldn't end by '\n' char.
        increment_t();
        // Don't forget close the file.
        */
}