// ## LCD FUNCTIONS
  void white( void ){ u8g2.setDrawColor(1); /*white*/ }
  void black( void ){ u8g2.setDrawColor(0); /*black*/ }
  void setup_lcd_after_SD() {
    u8g2.initInterface();
    u8g2.setPowerSave(0); 
  }
// ## END LCD FUNCTIONS


// ## LCD Data displaying
   
  void display_draw_weather(){ 
    // ## black out section which are suppose to updated
    setup_lcd_after_SD();
    black();
    //          left-right, top-bottom, and dimensions to the bottom
    u8g2.drawBox(10, 0, 128, 8); //temp readings
    if( ext_temperature.exit_code == 0){ //erase ext temp if not connected
      u8g2.drawBox(80, 0, 128, 8);
    }
    u8g2.drawBox(8, 8, 128, 8); //hum readings
    u8g2.drawBox(12, 17, 128, 8); //press readings
    u8g2.drawBox(8, 26, 128, 8); //gas readings
    u8g2.drawBox(12, 35, 128, 8); //alt readings
    if ( scd40_readings.exit_code == 0 ){  //co2 readings only when there are really new data
      u8g2.drawBox(8, 44, 128, 8);
    }
    u8g2.drawBox(8, 53, 128, 8); 
    // ## provide new data
    white();
    //Temp normally:
    //u8g2.drawStr(0,8,    String( String("Temp: ") + String((ext_temperature.value + bme680_readings.temp)/2) + String(" C" ) ).c_str()   );
    
    //Temp aligned to the right (Value):
    u8g2.drawStr(0,8,   String( String("T: ") + String(bme680_readings.temp) + String(" C") ).c_str()  ) ; 
    if( ext_temperature.exit_code == 0){ //good data
      u8g2.drawStr(128-u8g2.getStrWidth(String( String("ExT: ") + String(ext_temperature.value) + String(" C" ) ).c_str() ), 8,  String( String("ExT: ") + String(ext_temperature.value) + String(" C" ) ).c_str() );
    } 
    u8g2.drawStr(0,17,  String( String("Hum: ") + String(bme680_readings.hum) + String(" %" ) ).c_str()  ) ;
    u8g2.drawStr(0,26,  String( String("Press: ") + String( bme680_readings.press / 100.0) + String(" hPa") ).c_str()    );
    u8g2.drawStr(0,35,  String( String("Gas: ") + String(bme680_readings.gas / 1000.0) + String(" KOhms") ).c_str()   );
    u8g2.drawStr(0,44,  String( String("~Alt: ") + String(bme680_readings.alti) + String(" m" )  ).c_str()   );
    if ( scd40_readings.exit_code == 0 ){
      u8g2.drawStr(0,53,  String( String("CO2: ") + String(scd40_readings.co2) ).c_str()   );
    }
    u8g2.drawStr(0,62,  String( String("Time: ") + String(SDStringTime) ).c_str()   );

    u8g2.sendBuffer();
  }

  void display_draw_temgraph(){
    setup_lcd_after_SD();
    u8g2.clearDisplay();
    white();
    u8g2.drawStr(0,8, "display_draw_temgraph");
    u8g2.sendBuffer();
  }

  void display_draw_humgraph(){
    setup_lcd_after_SD();
    u8g2.clearDisplay();
    white();
    u8g2.drawStr(0,8, "display_draw_humgraph");
    u8g2.sendBuffer();
  }

  void display_draw_pressgraph(){
    setup_lcd_after_SD();
    u8g2.clearDisplay();
    white();
    u8g2.drawStr(0,8, "display_draw_pressgraph");
    u8g2.sendBuffer();
  }

  void display_draw_c02graph(){
    setup_lcd_after_SD();
    u8g2.clearDisplay();
    white();
    u8g2.drawStr(0,8, "display_draw_c02graph");
    u8g2.sendBuffer();
  }
  
  void display_draw_stats(){
    setup_lcd_after_SD();
    u8g2.clearDisplay();
    white();
    u8g2.drawStr(0,8, "display_draw_stats");
    u8g2.sendBuffer();
  }
  
  void display_draw_credits(){
    setup_lcd_after_SD();
    u8g2.clearDisplay();
    white();
    u8g2.drawStr(0,8, "This is credits screen");
    u8g2.sendBuffer();
  }
// ## END LCD Data displaying


// ## SD CARD FUNCTIONS
  #if defined(ENABLE_SD)
    bool appendFile(fs::FS &fs, const char * path, const char * message){
      Serial.printf("Appending to file: %s\n", path);
    
      File file = fs.open(path, FILE_APPEND);
      if(!file){
        Serial.println("Failed to open file for appending");
        return 1;
      }
      if(file.print(message)){
          Serial.println("Message appended");
      } else {
        Serial.println("Append failed");
        file.close();
        return 2;
      }
      file.close();
      return 0;
    }

    bool setup_sd_and_log(const String& LogMessage, const bool logORnot ){
      spiSD.begin(MY_SCLK, MY_MISO,MY_MOSI, MY_CS_SD);
      spiSD.setFrequency(SD_SPI_SPEED);

      if( !SD.begin( MY_CS_SD, spiSD, SD_SPI_SPEED ) )
      {
        int tries = 0;
        Serial.println("Card Mount Failed, trying again:");
        while ( !SD.begin( MY_CS_SD, spiSD, SD_SPI_SPEED ) && tries < 2)
        {
          tries = tries + 1;
          Serial.println("Trying again SD card: " + String(tries));
        }
        //add failed sd
        Serial.println("SD card mount FINAL fail :(");
        SD.end();
        spiSD.end();
        
        return 3;
      }
      Serial.println("SD OK");
      if( logORnot ){ //check if message is empty, if it is skip. Used to Init SD card
        String SDlog = LogMessage + "\r\n";
        char char_ming_message[SDlog.length()];
        SDlog.toCharArray(char_ming_message, SDlog.length());
        sd_append_success = appendFile(SD, LOGGING_FILE_NAME, char_ming_message);
      }
      SD.end();
      spiSD.end();
      return sd_append_success;
    }
  #endif
// ## END SD CARD FUNCTIONS


// ## GATHER ALL DATA

  // // ## External Temperature 
    dane_z_sensora_ext_temp data_extTemp(){
      float ext_temperature; //in degC
      ext_temp_sensor.requestTemperatures();
      ext_temperature = ext_temp_sensor.getTempCByIndex(0);
      if ( ext_temperature < -126.00 ){ return { 1, -126.00         };    } 
      else                            { return { 0, ext_temperature };    }   
    }
  // // ## END External Temperature 

  // // ## BME680 temp, hum, press
    dane_z_sensora_BME680 data_bme680(){
      // Tell BME680 to begin measurement:
      if (BME680.beginReading() == 1) { return {1, -999, -999, -999, -999, -999 }; }
      delay(50);
      if (BME680.endReading() == 2) {  return {2, -999, -999, -999, -999, -999 };   }
      else {
        return {  0, BME680.temperature, BME680.pressure / 100.0, BME680.humidity, BME680.gas_resistance / 1000.0, BME680.readAltitude(SEALEVELPRESSURE_HPA) };
      }
    }
  // // ## END BME680 temp, hum, press

  // // ## RTCDS3231
    //nothing for now, since everything i would ever need from this sensor is already implemented in the library provided
  // // ## END RTCDS3231

  // // ## SCD40 c02, temp, hum
    dane_z_sensora_SCD40 data_SCD40(){
      uint16_t error;
      //char errorMessage[256];
      uint16_t co2;
      float temperature;
      float humidity;
      error = SCD40.readMeasurement(co2, temperature, humidity);

      if (error || co2 == 0) {
        //errorToString(error, errorMessage, 256);
        return { 1, -999, -999, -999 };
      } 
      else 
      {
        return { 0, co2, temperature, humidity };
      }
    }
  // // ## END CD40 c02, temp, hum

// ## END GATHER ALL DATA


// ## FUNCTION GATHER ALL DATA

  void gather_ALL_data( bool serial_yes_no ){
    current_time = RTCDS3231.now();
    SDStringTime = ((current_time.hour() < 10 ? "0" : "") + String(current_time.hour()) + ":" + (current_time.minute() < 10 ? "0" : "") + String(current_time.minute()) + ":" + (current_time.second() < 10 ? "0" : "") + String(current_time.second()) + ", " + String(days[current_time.dayOfTheWeek()]) + " " + (current_time.day() < 10 ? "0" : "") + String(current_time.day()) + "." + (current_time.month() < 10 ? "0" : "") + String(current_time.month()) + "." + String(current_time.year(), DEC) + " ->  ");
    if( serial_yes_no ) Serial.print(SDStringTime);


    ext_temperature = data_extTemp();
    if (ext_temperature.exit_code){
      SDStringExttemp = "ExtTemp: error  ";
      if( serial_yes_no )  Serial.print(SDStringExttemp);}
    else {
      SDStringExttemp = "Ext temp: " + String(ext_temperature.value) + " degC  ";
      if( serial_yes_no )  Serial.print(SDStringExttemp);
    }


    bme680_readings = data_bme680();
    if( bme680_readings.exit_code == 1){
      SDStringBME680 = "BME680: begin readings error\n";
      if( serial_yes_no )  Serial.print(SDStringBME680);
    }
    else if (bme680_readings.exit_code == 2) {
      SDStringBME680 = "BME680: end readings error\n";
      if( serial_yes_no )  Serial.print(SDStringBME680);
    }
    else { 
      SDStringBME680 = "Temp: " + String(bme680_readings.temp) + " degC " + "Pressure: " + String(bme680_readings.press / 100.0) + " hPa " + "Humidity: " + String(bme680_readings.hum) + " % " + "Gas: " + String(bme680_readings.gas / 1000.0) + " KOhms " + "~Altitude: " + String(bme680_readings.alti) + " m  ";
      if( serial_yes_no )  Serial.print(SDStringBME680);
    }

    scd40_readings = data_SCD40();
    if(scd40_readings.exit_code == 1){
      //on display do not refresh parts of the screen which have previous readings
      SDStringSCD40 = "SCD40: error reading data, or not enough time for measurments, or the data is invalid";
      if( serial_yes_no )  Serial.print(SDStringSCD40 + "\n");
    }
    else{
      SDStringSCD40 = "Co2: " + String(scd40_readings.co2) + " Temp: " + String(scd40_readings.temperature) + " degC Humidity: " + String(scd40_readings.humidity) + " %";
      if( serial_yes_no )  Serial.print(SDStringSCD40 + "\n");
    }

    //return String( SDStringTime + SDStringExttemp + SDStringBME680 + SDStringSCD40 );
  }

// ## END FUNCTION GATHER ALL DATA


// ## Alternative void setups() functions
  
  //used instead of void setup():
  void init_setup( void ) {

    // ## SERIAL
      Serial.begin(115200);
    // ## END SERIAL

    // ## LCD BAKCLIGHT
      ledcSetup(0, 5000, 8);//led channel, freq, resolution (8 == 2^8 mean 0 <-> 255 )
      ledcAttachPin(lcd_backlight_pin, 0 ); //gpio, led channel
      ledcWrite(0, 128); //led channel, pwm fill
    // ## END LCD BAKCLIGHT

    // ## LCD SETUP
      u8g2.setBusClock(SD_SPI_SPEED);
      u8g2.begin();
      u8g2.setContrast(640);
      u8g2.setFont(u8g2_font_ncenB08_tr);
      //DO NOT UNCOMMENT FOLLOWING LINES, 1s and 0s are really desperated to crash when following lines are called
      // white();
      // u8g2.drawStr(0,8,"Starting up...");
      // u8g2.drawStr(0,16,".");
      // u8g2.sendBuffer();
      // delay(1000);
      u8g2.clearDisplay();
      u8g2.sendBuffer();
    // ## END LCD SETUP

    // ## SD LOGGING
      #if defined(ENABLE_SD)
        setup_sd_and_log(String("Starting up..." + String(millis()) + "ms"), true );
      #endif
    // ## END SD LOGGING

    // ## external temp probe setup
      ext_temp_sensor.begin();
    // ## END external temp probe setup

    // ## BME680 setup
      Wire.begin(13, 14);
      if (!BME680.begin(0x77, true)) 
      {
        Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
        while (1);
      }
      BME680.setTemperatureOversampling(BME680_OS_8X);
      BME680.setHumidityOversampling(BME680_OS_2X);
      BME680.setPressureOversampling(BME680_OS_4X);
      BME680.setIIRFilterSize(BME680_FILTER_SIZE_3);
      BME680.setGasHeater(320, 150);  // 320*C for 150 ms
    // ## END BME680 setup

    // ## RTC DS3231 setup
      if (!RTCDS3231.begin()) 
      {
        Serial.println("Could not find RTC! Check circuit.");
        while (1);
      }
      //RTCDS3231.adjust(DateTime(__DATE__, __TIME__)); //set time like PCs
      PrepareAlarmsDS3231(); //disables stuff and prepares for alarms
    // ## END RTC DS3231 setup

    // ## SCD40 CO2 module
      uint16_t error;
      char errorMessage[256];
      SCD40.begin(Wire);

      error = SCD40.stopPeriodicMeasurement();  // stop potentially previously started measurement
      if (error) {
        Serial.print(F("Error trying to execute stopPeriodicMeasurement(): "));
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
      }
      const String scd40_serial_number = "0x3E8DCF073B87";

      error = SCD40.startPeriodicMeasurement();  // Start Measurement
      if (error) {
        Serial.print(F("Error trying to execute startPeriodicMeasurement(): "));
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
      }
    // ## END SCD40 CO2 module

    // ## DS setup
      // while(!Serial){} //it should wait for serial but it does not?
      // pinMode(builtinled, OUTPUT);
      // digitalWrite(builtinled, HIGH);
      // Serial.println("DS: LED configured");
      // ++DS_BootCount;
      // Serial.println("Boot number: " + String(DS_BootCount) );

      //esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    // ## END DS setup

  } // END init_setup() 


  //used to when the button is pressed and user want to see data on display, etc:
  void later_setups( void ) {

    // ## SERIAL
      //Serial.begin(115200);
    // ## END SERIAL

    // ## LCD BAKCLIGHT
      ledcSetup(0, 5000, 8);//led channel, freq, resolution (8 == 2^8 mean 0 <-> 255 )
      ledcAttachPin(lcd_backlight_pin, 0 ); //gpio, led channel
      ledcWrite(0, 128); //led channel, pwm fill
    // ## END LCD BAKCLIGHT

    // ## LCD SETUP
      u8g2.setBusClock(SD_SPI_SPEED);
      u8g2.begin();
      u8g2.setContrast(640);
      u8g2.setFont(u8g2_font_ncenB08_tr);
      //DO NOT UNCOMMENT FOLLOWING LINES, 1s and 0s are really desperated to crash when following lines are called
      // white();
      // u8g2.drawStr(0,8,"Starting up...");
      // u8g2.drawStr(0,16,".");
      // u8g2.sendBuffer();
      // delay(1000);
      u8g2.clearDisplay();
      u8g2.sendBuffer();
    // ## END LCD SETUP

    // ## SD LOGGING
      #if defined(ENABLE_SD)
        setup_sd_and_log("", false );
      #endif
    // ## END SD LOGGING

    // ## external temp probe setup
      ext_temp_sensor.begin();
    // ## END external temp probe setup

    // ## BME680 setup
      Wire.begin(13, 14);
      if (!BME680.begin(0x77, true)) 
      {
        Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
        while (1);
      }
      BME680.setTemperatureOversampling(BME680_OS_8X);
      BME680.setHumidityOversampling(BME680_OS_2X);
      BME680.setPressureOversampling(BME680_OS_4X);
      BME680.setIIRFilterSize(BME680_FILTER_SIZE_3);
      BME680.setGasHeater(320, 150);  // 320*C for 150 ms
    // ## END BME680 setup

    // ## RTC DS3231 setup
      if (!RTCDS3231.begin()) 
      {
        Serial.println("Could not find RTC! Check circuit.");
        while (1);
      }
      //RTCDS3231.adjust(DateTime(__DATE__, __TIME__)); //set time like PCs
      PrepareAlarmsDS3231(); //disables stuff and prepares for alarms
    // ## END RTC DS3231 setup

    // ## SCD40 CO2 module
      uint16_t error;
      char errorMessage[256];
      SCD40.begin(Wire);

      error = SCD40.stopPeriodicMeasurement();  // stop potentially previously started measurement
      if (error) {
        Serial.print(F("Error trying to execute stopPeriodicMeasurement(): "));
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
      }
      const String scd40_serial_number = "0x3E8DCF073B87";

      error = SCD40.startPeriodicMeasurement();  // Start Measurement
      if (error) {
        Serial.print(F("Error trying to execute startPeriodicMeasurement(): "));
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
      }
    // ## END SCD40 CO2 module

    // ## DS setup
      while(!Serial){} //it should wait for serial but it does not?
      // pinMode(builtinled, OUTPUT);
      // digitalWrite(builtinled, HIGH);
      // Serial.println("DS: LED configured");
      // ++DS_BootCount;
      // Serial.println("Boot number: " + String(DS_BootCount) );

      //esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    // ## END DS setup
      
  } //END later_setups()


  //used to just gather data and log them, no display no user interface:
  void deepsleep_setups( void ) {
    // remove serial
    // ## SERIAL
    //  Serial.begin(115200);
    // ## END SERIAL

    // ## SD LOGGING
      #if defined(ENABLE_SD)
        setup_sd_and_log("", false );
      #endif
    // ## END SD LOGGING

    // ## external temp probe setup
      ext_temp_sensor.begin();
    // ## END external temp probe setup

    // ## BME680 setup
      Wire.begin(13, 14);
      if (!BME680.begin(0x77, true)) 
      {
        Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
        while (1);
      }
      BME680.setTemperatureOversampling(BME680_OS_8X);
      BME680.setHumidityOversampling(BME680_OS_2X);
      BME680.setPressureOversampling(BME680_OS_4X);
      BME680.setIIRFilterSize(BME680_FILTER_SIZE_3);
      BME680.setGasHeater(320, 150);  // 320*C for 150 ms
    // ## END BME680 setup

    // ## RTC DS3231 setup
      if (!RTCDS3231.begin()) 
      {
        Serial.println("Could not find RTC! Check circuit.");
        while (1);
      }
      //RTCDS3231.adjust(DateTime(__DATE__, __TIME__)); //set time like PCs
      PrepareAlarmsDS3231(); //disables stuff and prepares for alarms
    // ## END RTC DS3231 setup

    // ## SCD40 CO2 module
      uint16_t error;
      char errorMessage[256];
      SCD40.begin(Wire);

      error = SCD40.stopPeriodicMeasurement();  // stop potentially previously started measurement
      if (error) {
        Serial.print(F("Error trying to execute stopPeriodicMeasurement(): "));
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
      }
      const String scd40_serial_number = "0x3E8DCF073B87";

      error = SCD40.startPeriodicMeasurement();  // Start Measurement
      if (error) {
        Serial.print(F("Error trying to execute startPeriodicMeasurement(): "));
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
      } 
    // ## END SCD40 CO2 module

    // ## DS setup
      while(!Serial){} //it should wait for serial but it does not?
      // pinMode(builtinled, OUTPUT);
      // digitalWrite(builtinled, HIGH);
      // Serial.println("DS: LED configured");
      // ++DS_BootCount;
      // Serial.println("Boot number: " + String(DS_BootCount) );
      //esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    // ## END DS setup

  } // END deepsleep_setups()

// ## END Alternative void setups() functions


// ## Helpers functions (DS3231 alarms preparations, )

  //setup DS3231 for alarms (usually after first boot after programming, but it will do it every signle time.)
  void PrepareAlarmsDS3231( void ){

    //we don't need the 32K Pin, so disable it
    RTCDS3231.disable32K();

    // Clear alarm flag on RTC (important for the first run after programming)
    RTCDS3231.clearAlarm(1);
    RTCDS3231.clearAlarm(2);

    // stop oscillating signals at SQW Pin
    // otherwise setAlarm1 will fail
    RTCDS3231.writeSqwPinMode(DS3231_OFF);

    // turn off alarm 2 (in case it isn't off already)
    // again, this isn't done at reboot, so a previously set alarm could easily go overlooked
    RTCDS3231.disableAlarm(2);
    
  }

  //set next alarm for +1min. Should always be called after PrepareAlarmsDS3231() and gather_ALL_data(); since it relies on time collected there.
  void SetAlarmsDS3231 ( void ){
    next_alarm_time = current_time + TimeSpan(0, 0, 1, 0); //add 1min
    RTCDS3231.clearAlarm(1);
    RTCDS3231.disableAlarm(1);
    RTCDS3231.setAlarm1(next_alarm_time, DS3231_A1_Second);
    Serial.print("Setting up next alarm. Current time:" + String(SDStringTime) + " + 1min = " + String(next_alarm_time.hour()) + ":" + String(next_alarm_time.minute()) + ":" + String(next_alarm_time.second()) + "\n");
  }

  void Interrupt_AlarmByDS3231 ( void ){
    Serial.print("DS3231 caused an alarm. Setting bool to handle alarm.\n");
    AlarmHappened = true; //so that in loop next alarm can be set :/

    //can't put any I2C here bcse Interrupt routine doesn't like them. ChatGPT explained it like that:
    // Reset Issue: The reset you're experiencing is most likely a watchdog timer timeout. The watchdog is triggered because the I2C communication inside the interrupt routine freezes the system, preventing the loop() from running and performing its usual tasks.
    // Solution: Move Alarm Setting Outside of ISR
    //well it works, and maybe it makes sense.

  }

// ## END Helpers functions


// ##

// ## END