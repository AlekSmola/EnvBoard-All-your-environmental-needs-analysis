/* TO-DOs:
-dodać te wsszystkie moduły że jak ich nie ma to nie ma  i zeby działało dalej 
  z SD też  
- poprawić w setupie gdy dane urządzenie nie ejst wykryte
- zrobić coś exit codem funkcji setup_sd_and_log("asfd", true );
-zrobić kalibrację tego cyrku
- serial output do logów w sprawie SD card OK and appended succesfful
- poprawić DateTime - używane na ekranie ze stringa a trzeba by napisać coś customowego co się mieści 
- delay w setup loop w sekcji lcd setup
- external temp w weather is used as average, make it individual field
- moduł od c02 powinien przy wyjsciu z deepsleepa mieć trochę czas na zebranie danych.
- dodać od c02 jakieś skale czy jest ok czy nie 
- jakiś wireless module?
- add ifdefined(SDCARD) where required
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

  // ## ESP DEEP SLEEP
    #include <esp_sleep.h>
  // ## END ESP DEEP SLEEP

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
      #define LOGGING_FILE_NAME "/test_karty_2331.txt"
      #define SD_SPI_SPEED 4000000 //8000000
      const int MY_CS_SD = 12;
      SPIClass spiSD = SPIClass(HSPI);
      bool sd_append_success;
    #endif
  // ## END REQUIREMENTS for SD CARD logging

  // ## Deep Sleep defines and interrupts
    
    // Time for which ESP32 will go to sleep (in seconds)
    #define TIME_TO_SLEEP  5 
    // RTC_DATA_ATTR int bootCount = 0; //can be removed, more for giggles than useful stuff
  
    // Variable to set new alarm when device has woken up. Has to be since I2C can't happen in interrupt routine.
    volatile bool AlarmHappened = false; //volatile so that it does not interfere with ISR routine

    const uint8_t ButtonTrigger = 2;  //button pin ( external button ) , also known as Menu Button
    const uint8_t DS3231InterruptPin = 4; //Pin at which DS3231 has its Interrupt pin connected to (with 5k pullup)
    
    //below 3 defines defines when next wake up by DS3231 should occur. Probably u can do it in one define but it works this way and i have a lot of memory.
    #define TIME_SET_HOUR 10
    #define TIME_SET_MINUTE 55
    #define TIME_SET_SECOND 0

    //for defining two Pin for wakeup, and then which one really woke device.
    uint64_t WakeUp_pin_mask = 0;
    uint64_t WakeUp_pin_mask_after_WakeUp = 0;
  
    // Time for which ESP32 will go to sleep (in seconds), i.e.:
    // wakeup timer set to 5s. So it will wakeup every 5s.
    #define DS_SleepDelay  5

    // Amount of times it woke up, more like to remember how to save variable to this memory which survives deepsleep
    //RTC_DATA_ATTR int DS_BootCount = 0;

    // to store time it was woken up or menu changed.
    uint32_t DS_LastPressed;

    // For how long display shoul display info before deep sleep (DS)
    const long DS_DisplayDelay = 150*1000;//150 since its is > 2 min to cjeck interrupts  //10*1000; //10s until unit will go to deep sleep

  // ## END Deep Sleep defines and interrupts
  
  // ## Multiple Screens and Their refreshes + Button handling

    const uint16_t MenusNumber = 7; //like real number, not current menu, 2 means two screens
    uint16_t CurrentMenu = 0;
    const uint8_t ButtonDebounce = 200; //ms super long but whatever
    uint32_t DisplayLastRefreshed = 0;
    const uint16_t DisplayRefreshInterval = 1000; //ms
    bool LCDForceRefresh = false;

    bool IsRefreshRequired = {}; //just to check if each screen needs to be refreshed
  
  // ## END Multiple Screens and Their refreshes + Button handling
  
  // ## REQUIREMENTS additional stuff
    const String script_version = "v.beta (25.12.2024)";
  // ## END REQUIREMENTS additional stuff

  //just temp
  const uint8_t builtinled = 15;

// ### END REQUIREMENTS


// ### Variables which need to be GLOBAL
  //, cause I am lazy

  String SDStringTime, SDStringExttemp, SDStringBME680, SDStringSCD40;
  DateTime current_time, next_alarm_time;
  dane_z_sensora_ext_temp ext_temperature;
  dane_z_sensora_BME680 bme680_readings;
  dane_z_sensora_SCD40 scd40_readings;

// ### END Variables which need to be GLOBAL


void setup() {
  Serial.begin(115200);
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
      gather_ALL_data(0);
      SetAlarmsDS3231(); 
      attachInterrupt(digitalPinToInterrupt(DS3231InterruptPin), Interrupt_AlarmByDS3231, FALLING);
      //#error here also set interrupts, remember to detach them as well. TRACK 16
      DS_LastPressed = millis();
      while ( millis() - DS_LastPressed < DS_DisplayDelay){ 
        if ( AlarmHappened && RTCDS3231.alarmFired(1)){
          detachInterrupt(digitalPinToInterrupt(DS3231InterruptPin));
          Serial.print("\tDuring device being woken up, the alarm occured: Setting new alarm.\n");
          RTCDS3231.clearAlarm(1);
          SetAlarmsDS3231();
          Serial.print("\tLogging Data to SD card...\n");
          setup_sd_and_log( (SDStringTime + SDStringExttemp + SDStringBME680 + SDStringSCD40), true );
          attachInterrupt(digitalPinToInterrupt(DS3231InterruptPin), Interrupt_AlarmByDS3231, FALLING);
          AlarmHappened = false;
          Serial.print("\tInterrupt completed. Following with screen refresdes and so on.\n");
        }
        loop(); 
        delay(1); 
      }
      Serial.print("DS: Going to sleep now, because there was no acitivity for " + String(DS_DisplayDelay/1000) + "seconds: {initial_bootup}\n");
      break;

    case ESP_SLEEP_WAKEUP_EXT1:
      WakeUp_pin_mask_after_WakeUp = esp_sleep_get_ext1_wakeup_status();

      if(WakeUp_pin_mask_after_WakeUp & (1ULL << ButtonTrigger)){
        Serial.print("DS: Wakeup caused by external signal using RTC_IO. Button was pressed and devices should display data on LCD.\n"); 
        later_setups();
        attachInterrupt(digitalPinToInterrupt(DS3231InterruptPin), Interrupt_AlarmByDS3231, FALLING);
        DS_LastPressed = millis();
        while ( millis() - DS_LastPressed < DS_DisplayDelay){ 
          if ( AlarmHappened && RTCDS3231.alarmFired(1)){
            detachInterrupt(digitalPinToInterrupt(DS3231InterruptPin));
            Serial.print("\tDuring device being woken up, the alarm occured: Setting new alarm.\n");
            RTCDS3231.clearAlarm(1);
            SetAlarmsDS3231();
            Serial.print("\tLogging Data to SD card...\n");
            setup_sd_and_log( SDStringTime + SDStringExttemp + SDStringBME680 + SDStringSCD40, true  );
            attachInterrupt(digitalPinToInterrupt(DS3231InterruptPin), Interrupt_AlarmByDS3231, FALLING);
            AlarmHappened = false;
            Serial.print("\tInterrupt completed. Following with screen refresdes and so on.\n");
          }
          loop(); 
          delay(1); 
        }
        Serial.print("DS: Going to sleep now, because there was no acitivity for " + String(DS_DisplayDelay/1000) + " seconds: {ESP_SLEEP_WAKEUP_EXT1, button}\n");    
      }

      #if defined(ENABLE_SD) 
        if(WakeUp_pin_mask_after_WakeUp & (1ULL << DS3231InterruptPin)){
          Serial.println("DS: Wakeup caused by external signal using RTC_IO. DS3231 sent an active alarm wakeup. Should gather and log data if so desired.\n");
          deepsleep_setups();
          gather_ALL_data(1);
          SetAlarmsDS3231();
          setup_sd_and_log( SDStringTime + SDStringExttemp + SDStringBME680 + SDStringSCD40, true  );
          Serial.print("DS: Going to sleep now: {ESP_SLEEP_WAKEUP_EXT1, ds3231}\n");
        }
      #endif
      

      break;
    
    // case ESP_SLEEP_WAKEUP_TIMER : //just for reference, obsolete since DS3231 took over
    //   #if defined(ENABLE_SD) 
    //     Serial.print("DS: Wakeup caused by timer. This is time for gathering and logging data. No display involved.\n"); 
    //     deepsleep_setups();
    //     gather_ALL_data(1);
    //     setup_sd_and_log( SDStringTime + SDStringExttemp + SDStringBME680 + SDStringSCD40, true  );
    //     Serial.print("DS: Going to sleep now: {ESP_SLEEP_WAKEUP_TIMER}\n");
    //   #endif
    //   break;
      
    case ESP_SLEEP_WAKEUP_EXT0 :      Serial.println("DS: Wakeup caused by external signal using single external trigger. No custom operation defined."); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD :  Serial.println("DS: Wakeup caused by touchpad. No custom operation defined."); break;
    case ESP_SLEEP_WAKEUP_ULP :       Serial.println("DS: Wakeup caused by ULP program. No custom operation defined."); break;

    default: 
      Serial.printf("DS: Wakeup was not caused by deep sleep: %d\n",wakeup_reason); 
      break;

  } //END switch()


  /* 
  //obsolete bcse now we use two 'buttons' inbstread of 1 and timer 
  #if defined(ENABLE_SD)
    Serial.print("DS: Setup ESP32 to sleep for every " + String(DS_SleepDelay) + " Seconds. ");
    esp_sleep_enable_timer_wakeup(DS_SleepDelay * 1000000 ); //1000000 is conversion factor for micro seconds to seconds
  #endif
  //Setup wakeup from deepsleep when button is pressed
  Serial.println("DS: Setup ESP32 to sleep until button at GPIO2 is pressed high");
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_2, 1); //1 = High, 0 = Low
  */

  //i guess pinMode(both, INPUT_PULLUP); is not required?
  #if defined(ENABLE_SD)
    WakeUp_pin_mask |= (1ULL << DS3231InterruptPin);
  #endif
  WakeUp_pin_mask |= (1ULL << ButtonTrigger);
  esp_sleep_enable_ext1_wakeup(WakeUp_pin_mask, ESP_EXT1_WAKEUP_ANY_LOW); // Wake up on LOW level

  delay(500); // it is here for some reason, worry to remove :()

  //COMMENT IF PROBLEMS?
  //RTCDS3231.clearAlarm(1); //CLEAR 1ST ALARM WHICH WE CARE ABOUT AFTER IT FIRED SO IT IS READY FOR NEXT ONE?
  // RTCDS3231.disableAlarm(1);
  //

  Serial.flush(); 
  esp_deep_sleep_start();
  Serial.println("DS: This will never be printed");

} //END VOID SETUP()

//Serial.print("E" + String(12) + "" + "\n");



void loop() {


    
    // local TO-DO:
    //add SDcard logging when needed

    //Deal with buttons and menus
    if( !digitalRead(ButtonTrigger) == HIGH && millis() - DS_LastPressed >= ButtonDebounce ){
      CurrentMenu += 1;
      if(CurrentMenu > MenusNumber-1){ CurrentMenu = 0;} //-1 becasue we're counting from 0
      DS_LastPressed = millis();
      LCDForceRefresh = true;
      Serial.print( "Button was pressed. Current menu: " + String(CurrentMenu) + "\n" );    
    }
        
  
    if( LCDForceRefresh || millis() - DisplayLastRefreshed >= DisplayRefreshInterval ){ //is it time to refresh display?
      gather_ALL_data(1); //gather data only when screen menu changes or when time comes
      switch(CurrentMenu){
        case 0:
          display_draw_weather();
          break;
          
        case 1:
          display_draw_temgraph();
          break;
  
        case 2:
          display_draw_humgraph();
          break;
  
        case 3:
          display_draw_pressgraph();
          break;
  
        case 4:
          display_draw_c02graph();
          break;
  
        case 5:
          display_draw_stats();
          break;
  
        case 6:
          display_draw_credits();
          break;
  
        default:
          Serial.println("You should never see this message like EVER!");
          break;
  
      }  // END switch(CurrentMenu)

      LCDForceRefresh = false;
      DisplayLastRefreshed = millis();
  
    } // END IF display refresh
  
  
  //if(millis() - last_log_saved >= sd_log_interval){  sd_log(); last_log_saved = millis();}
  // #if defined(ENABLE_SD)
  //   setup_sd_and_log( SDStringTime + SDStringExttemp + SDStringBME680 + SDStringSCD40, true  );
  // #endif
 
 
} // END VOID LOOP()



