/* TO-DOs:
-dodać te wsszystkie moduły że jak ich nie ma to nie ma  i zeby działało dalej 
  z SD też  
- poprawić w setupie gdy dane urządzenie nie ejst wykryte
- zrobić coś exit codem funkcji setup_sd_and_log("asfd");
-zrobić kalibrację tego cyrku
- serial output do logów w sprawie SD card OK and appended succesfful
- poprawić DateTime - używane na ekranie ze stringa a trzeba by napisać coś customowego co się mieści 
- delay w setup loop w sekcji lcd setup
- external temp w weather is used as average, make it individual field
- moduł od c02 powinien przy wyjsciu z deepsleepa mieć trochę czas na zebranie danych.
- dodać od c02 jakieś skale czy jest ok czy nie 
- jakiś wireless module?
*/

/* IMPORTANT NOTES:
!!! USE esp32 by Espressif library in version 2.0.17 !!!
!!! board: LOLIN S2 Mini !!!

Everything 'DS' is DeepSleep related


I2C device found at address 0x62  !
I2C device found at address 0x68  !
I2C device found at address 0x77  !
*/

#define ENABLE_SD //if you want SD logging or not. comment out to disable SD card functionality completely. Saves 4% of program storage space

// ### IMPORT LIBRARIES IMPORT

  // ## EXTERNAL TEMP SENSOR DS18B20
    #include <OneWire.h>
    #include <DallasTemperature.h>
  // ## END EXTERNAL TEMP SENSOR DS18B20

  // ## BME680 Temp+hum+Press+Gas sensor
    #include <Wire.h>
    #include <Adafruit_Sensor.h>
    #include "Adafruit_BME680.h"
  // ## END BME680 Temp+hum+Press+Gas sensor

  // ## DS3231 Real Time Clock
    #include <Adafruit_GFX.h>
    #include <Adafruit_SSD1306.h>
    #include "RTClib.h"
  // ## END DS3231 Real Time Clock

  // ## SCD40 CO2 sensor
    #include <SensirionI2CScd4x.h>
  // ## END SCD40 CO2 sensor

  // ## LCD setup
    #include <Arduino.h>
    #include <U8g2lib.h>
    #include "SPI.h"
  // ## LCD setup

  // ## SD CARD logging
    #if defined(ENABLE_SD)
      #include "SPI.h"
      #include "FS.h"
      #include "SD.h"
      #include "SPI.h"
    #endif
  // ## END SD CARD logging


// ### END IMPORT LIBRARIES 


// ### REQUIREMENTS

  // ## REQUIREMENTS for SPI
    const int MY_MOSI = 11;
    const int MY_MISO = 9;
    const int MY_SCLK = 7;
  // ## END REQUIREMENTS for SPI

  // ## structure for gathering readings (ext_temp_probe)
    struct dane_z_sensora_ext_temp {
      /*
      Data from sensor + exit code. 
      0 means everything went OK, 
      1 means there was an error like communication problem
      */
      bool exit_code;
      float value;
    };
  // ## END structure for gathering readings (ext_temp_probe)

  // ## structure for gathering readings (BME680)
    struct dane_z_sensora_BME680 {
      /*
      Data from sensor + exit code. 
      0 means everything went OK, 
      1 failed to begin reading
      2 failed to complete reading
      */
      byte exit_code;
      float temp;
      float press;
      float hum;
      float gas;
      float alti;
      
    };
  // ## END structure for gathering readings (BME680)
  
  // ## structure for gathering readings (SCD40)
    struct dane_z_sensora_SCD40 {
      /*
      Data from sensor + exit code. 
      0 means everything went OK, 
      1 failed to get data reading
      */
      uint16_t exit_code;
      // String error_reason;
      // char errorMessage[256];
      uint16_t co2;
      float temperature;
      float humidity;
      
    };
  // ## END structure for gathering readings (SCD40)

  // ## REQUIREMENTS for EXTERNAL TEMP SENSOR DS18B20
    const int oneWireBus = 33;                    // GPIO where the DS18B20 is connected to
    OneWire oneWire(oneWireBus);                  // Setup a oneWire instance to communicate with any OneWire devices
    DallasTemperature ext_temp_sensor(&oneWire);  // Pass our oneWire reference to Dallas Temperature sensor
  // ## END REQUIREMENTS for EXTERNAL TEMP SENSOR DS18B20

  // ## REQUIREMENTS for BME680 Temp+hum+Press+Gas sensor
    #define SEALEVELPRESSURE_HPA (1013.25)
    Adafruit_BME680 BME680;  // I2C
  // ## END REQUIREMENTS for BME680 Temp+hum+Press+Gas sensor

  // ## REQUIREMENTS for RTC DS3231
    RTC_DS3231 RTCDS3231;
    const char days[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
  // ## END REQUIREMENTS for RTC DS3231

  // ## REQUIREMENTS for SCD40
    SensirionI2CScd4x SCD40;
  // ## END REQUIREMENTS for SCD40

  // ## REQUIREMENTS for LCD setup
    const int MY_CS_LCD = 39;
    const int lcd_backlight_pin = 16;
    U8G2_ST7565_NHD_C12864_F_4W_SW_SPI u8g2(U8G2_R2, MY_SCLK, MY_MOSI, MY_CS_LCD, 35, 37);  // dc CD (on display RS),reset rst (on display RSE)
    //U8G2_ST7565_NHD_C12864_F_4W_HW_SPI u8g2(U8G2_R0, MY_SCLK, MY_MOSI, MY_CS_LCD, 35, 37);

  // ## END REQUIREMENTS for LCD setup

  // ## REQUIREMENTS for SD CARD logging
    #if defined(ENABLE_SD)
      #define LOGGING_FILE_NAME "/test_karty_2118.txt"
      #define SD_SPI_SPEED 4000000 //8000000
      const int MY_CS_SD = 12;
      SPIClass spiSD = SPIClass(HSPI);
      bool sd_append_success;
    #endif
  // ## END REQUIREMENTS for SD CARD logging

  // ## REQUIREMENTS for sleep and interrupts
    #define TIME_TO_SLEEP  5 // Time for which ESP32 will go to sleep (in seconds)
    // RTC_DATA_ATTR int bootCount = 0; //can be removed, more for giggles than useful stuff
  // ## END REQUIREMENTS for sleep and interrupts

  // ## REQUIREMENTS additional stuff
    const String script_version = "v.beta (28.11.2024)";
  // ## END REQUIREMENTS additional stuff

  // ## Deep Sleep defines
    // Time for which ESP32 will go to sleep (in seconds), i.e.:
    // wakeup timer set to 5s. So it will wakeup every 5s.
    #define DS_SleepDelay  5

    // Amount of times it woke up, more like to remember how to save variable to this memory which survives deepsleep
    //RTC_DATA_ATTR int DS_BootCount = 0;

    // to store time it was woken up or menu changed.
    uint32_t DS_LastPressed;

    // For how long display shoul display info before deep sleep (DS)
    const long DS_DisplayDelay = 10*1000; //30s until unit will go to deep sleep

  // ## END Deep Sleep defines
  
  // ## Other stuff

    // External button used to control display and for waking up device 
    const uint8_t ButtonTrigger = 2;

    //just temp
    const uint8_t builtinled = 15;

  // ## END Other stuff

// ### END REQUIREMENTS


// ### Variables which need to be GLOBAL
  //, cause I am lazy

  String SDStringTime, SDStringExttemp, SDStringBME680, SDStringSCD40;
  DateTime current_time;
  dane_z_sensora_ext_temp ext_temperature;
  dane_z_sensora_BME680 bme680_readings;
  dane_z_sensora_SCD40 scd40_readings;

// ### END Variables which need to be GLOBAL


void setup() {

  // ## DS setup
    while(!Serial){} //it should wait for serial but it does not?
    // pinMode(builtinled, OUTPUT);
    // digitalWrite(builtinled, HIGH);
    // Serial.println("DS: LED configured");
    // ++DS_BootCount;
    // Serial.println("Boot number: " + String(DS_BootCount) );

    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  // ## END DS setup

  switch(wakeup_reason){   

    case 0: 
      Serial.print("DS: Initial bootup\n");
      init_setup();
      DS_LastPressed = millis();
      while ( millis() - DS_LastPressed < DS_DisplayDelay){ 
        loop(); 
        Serial.print("loop() was enetered bu Init"); 
        delay(50); 
      }
      Serial.print("DS: Going to sleep now, because there was no acitivity for " + String(DS_DisplayDelay/1000) + "seconds: {initial_bootup}\n");
      break;

    case ESP_SLEEP_WAKEUP_EXT0 : 
      Serial.print("DS: Wakeup caused by external signal using RTC_IO. Button was pressed and devices should display data on LCD.\n"); 
      later_setups();
      DS_LastPressed = millis();
      while ( millis() - DS_LastPressed < DS_DisplayDelay){ 
        loop(); 
        Serial.print("loop() was enetered bu button\n"); 
        delay(50); 
      }
      Serial.print("DS: Going to sleep now, because there was no acitivity for " + String(DS_DisplayDelay/1000) + " seconds: {ESP_SLEEP_WAKEUP_EXT0}\n");
      break;
    
    case ESP_SLEEP_WAKEUP_TIMER :
      #if defined(ENABLE_SD) 
        Serial.print("DS: Wakeup caused by timer. This is time for gathering and logging data. No display involved.\n"); 
        deepsleep_setups();
        gather_ALL_data();
        setup_sd_and_log( SDStringTime + SDStringExttemp + SDStringBME680 + SDStringSCD40 );
        Serial.print("DS: Going to sleep now: {ESP_SLEEP_WAKEUP_TIMER}\n");
      #endif
      break;
      
    case ESP_SLEEP_WAKEUP_EXT1 :      Serial.println("DS: Wakeup caused by external signal using RTC_CNTL. No custom operation defined."); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD :  Serial.println("DS: Wakeup caused by touchpad. No custom operation defined."); break;
    case ESP_SLEEP_WAKEUP_ULP :       Serial.println("DS: Wakeup caused by ULP program. No custom operation defined."); break;

    default: 
      Serial.printf("DS: Wakeup was not caused by deep sleep: %d\n",wakeup_reason); 
      break;

  } //END switch()

  #if defined(ENABLE_SD)
    Serial.println("DS: Setup ESP32 to sleep for every " + String(DS_SleepDelay) + " Seconds");
    esp_sleep_enable_timer_wakeup(DS_SleepDelay * 1000000 ); //1000000 is conversion factor for micro seconds to seconds
  #endif

  //Setup wakeup from deepsleep when button is pressed
  Serial.println("DS: Setup ESP32 to sleep until button at GPIO2 is pressed high");
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_2, 1); //1 = High, 0 = Low
  
  delay(500);
  Serial.flush(); 
  esp_deep_sleep_start();
  Serial.println("DS: This will never be printed");

} //END VOID SETUP()

//Serial.print("E" + String(12) + "" + "\n");



void loop() {

  gather_ALL_data();

  display_draw_weather( );

  #if defined(ENABLE_SD)
    setup_sd_and_log( SDStringTime + SDStringExttemp + SDStringBME680 + SDStringSCD40 );
  #endif
 
 
} // END VOID LOOP()



