#include "logging.h"

extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
File myFile;

LogEntry *entry = 0;


constexpr size_t CANBUFSIZE = 100;
LogEntry lbuf[CANBUFSIZE];
volatile unsigned int lbuf_head = 0;
volatile unsigned int lbuf_tail = 0;

void getTimeStamp(LogEntry* logEntry)
{
    // read rtc (64bit, number of 32,768 kHz crystal periods)
    uint64_t periods;
    uint32_t hi1 = SNVS_HPRTCMR, lo1 = SNVS_HPRTCLR;
    while (true)
    {
        uint32_t hi2 = SNVS_HPRTCMR, lo2 = SNVS_HPRTCLR;
        if (lo1 == lo2 && hi1 == hi2)
        {
            periods = (uint64_t)hi2 << 32 | lo2;
            break;
        }
        hi1 = hi2;
        lo1 = lo2;
    }

    // calculate seconds and milliseconds
    uint32_t ms = (1000 * (periods % 32768)) / 32768;
    time_t sec  = periods / 32768;

    tm t                  = *gmtime(&sec); // calculate calendar data
    logEntry->second      = t.tm_sec;
    logEntry->minute      = t.tm_min;
    logEntry->hour        = t.tm_hour;
    logEntry->day         = t.tm_mday;
    logEntry->month       = t.tm_mon + 1;
    logEntry->year        = t.tm_year + 1900;
    logEntry->millisecond = ms;
}

void setup_log() {

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

    myFile = SD.open("Test02.txt", FILE_WRITE);
    myFile.printf("Current, Voltage, MinTmp, MaxTmp, AvgTmp, Apps1, Apps2, Brake \n");
    myFile.close();


    //Serial.print("Initialization completed");
}

void write_to_file(int current, int voltage, int mintmp, int maxtmp, int avgtmp, int apps1, int apps2, int brake) {
    
        myFile = SD.open("Test02.txt", FILE_WRITE);

        Serial.print("Starting to write...");


        //myFile.println(t);

        //getTimeStamp(entry);

        //myFile.printf("%d-%02d-%02d %02d:%02d:%02d.%03u -> ", entry->year, entry->month, entry->day, entry->hour, entry->minute, entry->second, entry->millisecond);
        
        myFile.printf("%d, ",voltage);
        myFile.printf("%d, ", mintmp);
        myFile.printf("%d, ",maxtmp);
        myFile.printf("%d,",avgtmp);
        myFile.printf("%d, ",apps1);
        myFile.printf("%d, ", apps2);
        myFile.printf("%d \n", brake);

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