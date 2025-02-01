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
    u8g2.setFont(u8g2_font_ncenB08_tr);
    
    black();
    u8g2.drawBox(0, 0, 128, 64); //EVERYTHING SINCE IT GETS RID OF ARTEFACTS AFTER LAST SCREEN AS WELL


    //          left-right, top-bottom, and dimensions to the bottom
    u8g2.drawBox(10, 0, 128, 8); //temp readings
    if( ext_temperature.exit_code == 0){ //erase ext temp if not connected
      u8g2.drawBox(80, 0, 128, 8);
    }
    // u8g2.drawBox(8, 8, 128, 8); //hum readings
    // u8g2.drawBox(12, 17, 128, 8); //press readings
    // u8g2.drawBox(8, 26, 128, 8); //gas readings
    // u8g2.drawBox(12, 35, 128, 8); //alt readings
    // if ( scd40_readings.exit_code == 0 ){  //co2 readings only when there are really new data
    //   u8g2.drawBox(8, 44, 128, 8);
    // }
    // u8g2.drawBox(8, 53, 128, 8); //time box

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
    }else{ //no new readings, so use old? not really this one is most likely very obsolete:
      u8g2.drawStr(0,53,  String( String("CO2: ") + String(MeasuredDataMinAvgMax[3][HistoricalDataBuffer-1]) ).c_str()   );
    }
    Serial.println("AKSJDB: " + String(MeasuredDataMinAvgMax[3][HistoricalDataBuffer-1]) );

    u8g2.drawStr(0,62,  String( String("Time: ") + String(SDStringTime) ).c_str()   );

    u8g2.sendBuffer();
  }

  void display_draw_temgraph(){
    /* legacy
    setup_lcd_after_SD();
    u8g2.clearDisplay();
    white();
    u8g2.drawStr(0,8, "display_draw_temgraph");
    u8g2.sendBuffer();
    */
    setup_lcd_after_SD();
    u8g2.clearDisplay();
    white();
    // float result1  = random(0,25);
    // Updater_MeasuredDataHistorical(0, result1);
    // Serial.print("New data: " + String(result1) + " added to MeasuredDataHistorical. ExeTime: ");
    Serial.print("Updating TempGraph with new data...\n");
    DataShrinkerForGraphs(); // prepare updated data to be printed
    // graphDataAdder(0, result1); not needed since we included new data already
    graphUpdater(0, tempGraph);
  }

  void display_draw_humgraph(){
    /* legacy
    setup_lcd_after_SD();
    u8g2.clearDisplay();
    white();
    u8g2.drawStr(0,8, "display_draw_humgraph");
    u8g2.sendBuffer();
    */
    setup_lcd_after_SD();
    u8g2.clearDisplay();
    white();
    Serial.print("Updating HumGraph with new data...\n");
    DataShrinkerForGraphs(); // prepare updated data to be printed
    // graphDataAdder(0, result1); not needed since we included new data already
    graphUpdater(1, humGraph);
  }

  void display_draw_pressgraph(){
    /* legacy
    setup_lcd_after_SD();
    u8g2.clearDisplay();
    white();
    u8g2.drawStr(0,8, "display_draw_pressgraph");
    u8g2.sendBuffer();
    */
    setup_lcd_after_SD();
    u8g2.clearDisplay();
    white();
    Serial.print("Updating pressGraph with new data...\n");
    DataShrinkerForGraphs(); // prepare updated data to be printed
    // graphDataAdder(0, result1); not needed since we included new data already
    graphUpdater(2, pressGraph);
  }

  void display_draw_c02graph(){
    /* legacy
    setup_lcd_after_SD();
    u8g2.clearDisplay();
    white();
    u8g2.drawStr(0,8, "display_draw_c02graph");
    u8g2.sendBuffer();
    */
    setup_lcd_after_SD();
    u8g2.clearDisplay();
    white();
    Serial.print("Updating CO2 with new data...\n");
    DataShrinkerForGraphs(); // prepare updated data to be printed
    // graphDataAdder(0, result1); not needed since we included new data already
    graphUpdater(3, co2Graph);

  }
  
  void display_draw_stats(){
    /* legacy
    setup_lcd_after_SD();
    u8g2.clear();
    u8g2.clearDisplay();
    white();
    u8g2.drawStr(0,8, "display_draw_stats");
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(200, 100);
    u8g2.print( "Really whatever" ) ;
    u8g2.sendBuffer();
    */

    setup_lcd_after_SD();
    u8g2.clear();
    white();
    u8g2.setFont(u8g2_font_ncenB08_tr);

    u8g2.drawStr(21,8,"Min | Max | Avg"); // u8g2.drawStr(50,8,"Max"); u8g2.drawStr(85,8,"Avg"); u8g2.drawStr(112,8,"#");
    u8g2.drawVLine(15,0,63-10); // righ to vertical legend
    u8g2.drawHLine(0,10,128); // below horizontal legend

    u8g2.drawStr(0,23,"T");u8g2.setCursor(11,23);u8g2.print(":");
                            u8g2.setCursor(18,23);u8g2.printf("%.1f",MeasuredDataMinAvgMax[0][0]);
                            u8g2.setCursor(54,23);u8g2.printf("%.1f",MeasuredDataMinAvgMax[0][1]);
                            u8g2.setCursor(90,23);u8g2.printf("%.1f",MeasuredDataMinAvgMax[0][2]);
                            //u8g2.setCursor(80,26);u8g2.print(minavegmaxy[0][3]);
                               
    u8g2.drawStr(0,33,"H");u8g2.setCursor(11,33);u8g2.print(":");
                            u8g2.setCursor(18,33);u8g2.printf("%.1f",MeasuredDataMinAvgMax[1][0]);
                            u8g2.setCursor(54,33);u8g2.printf("%.1f",MeasuredDataMinAvgMax[1][1]);
                            u8g2.setCursor(90,33);u8g2.printf("%.1f",MeasuredDataMinAvgMax[1][2]);
                            //u8g2.setCursor(110,35);u8g2.print(minavegmaxy[1][3]);
                               
    u8g2.drawStr(0,43,"P");u8g2.setCursor(11,43);u8g2.print(":");
                            u8g2.setCursor(18,43);u8g2.printf("%.1f",MeasuredDataMinAvgMax[2][0]);
                            u8g2.setCursor(54,43);u8g2.printf("%.1f",MeasuredDataMinAvgMax[2][1]);
                            u8g2.setCursor(90,43);u8g2.printf("%.1f",MeasuredDataMinAvgMax[2][2]);
                            //u8g2.setCursor(55+35*2,44);u8g2.print(minavegmaxy[2][3]);
                               
    u8g2.drawStr(0,53,"C");u8g2.setCursor(11,53);u8g2.print(":");
                            u8g2.setCursor(18,53);u8g2.printf("%.1f", MeasuredDataMinAvgMax[3][0]);
                            u8g2.setCursor(54,53);u8g2.printf("%.1f", MeasuredDataMinAvgMax[3][1]);
                            u8g2.setCursor(90,53);u8g2.printf("%.1f", MeasuredDataMinAvgMax[3][2]);
                            //u8g2.setCursor(55+35*2,53);u8g2.printf("%.1f", minavegmaxy[3][3]);
  
    //  u8g2.drawStr( ( LCDWidth - u8g2.getUTF8Width(String(resulting_date))  / 2) ,62, resulting_date);
    u8g2.setCursor(0,63); 
    u8g2.print( \
    ((current_time.hour() < 10 ? "0" : "") + String(current_time.hour()) \
    + ":" + (current_time.minute() < 10 ? "0" : "") + String(current_time.minute()) \
    + ", " + String(days[current_time.dayOfTheWeek()]) + " " + (current_time.day() < 10 ? "0" : "") \
    + String(current_time.day()) + "." + (current_time.month() < 10 ? "0" : "") \
    + String(current_time.month()) + "." + String(current_time.year(), DEC))
    );


    /* for some unknown reason these two lines below make the display actually display stuff 
    ( probably since there is some stuff going on with graphs functions, 
    but no idea, weather info is just fine without them)
    */
    u8g2.setCursor(200, 100);
    u8g2.print( "Really whatever" ) ;

    u8g2.sendBuffer();

    Serial.print("Updating StatsScreen with new data...\n");
  }
  
  void display_draw_credits(){
    /* legacy 
    setup_lcd_after_SD();
    u8g2.clear();
    white();
    u8g2.drawStr(0,8, "This is credits screen");
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(200, 100);
    u8g2.print( "Really whatever" ) ;
    u8g2.sendBuffer();
    */
    setup_lcd_after_SD();
    u8g2.clear();
    white();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr( ((u8g2.getDisplayWidth() - (u8g2.getUTF8Width("<< Credits >>"))) / 2)         ,8, "<< Credits >>");
    u8g2.drawStr( ((u8g2.getDisplayWidth() - (u8g2.getUTF8Width("by AlekSmola"))) / 2)          ,17,"by AlekSmola");
    u8g2.drawStr( ((u8g2.getDisplayWidth() - (u8g2.getUTF8Width("github.com/aleksmola"))) / 2)  ,26,"github.com/aleksmola");

    char script_version_char[script_version.length()+1]; //last character cut thats why +1 (maybe \n?)
    script_version.toCharArray(script_version_char, script_version.length()+1);
    u8g2.drawStr( ((u8g2.getDisplayWidth() - (u8g2.getUTF8Width(script_version_char))) / 2),35, script_version_char );


    // black();
    // u8g2.drawXBM( 54, 63-20-3, 20, 20, bitmap_smile_face); //-3 for spacing and nice placing
    // white();


    /* for some unknown reason these two lines below make the display actually display stuff 
    ( probably since there is some stuff going on with graphs functions, 
    but no idea, weather info is just fine without them)
    */
    u8g2.setCursor(200, 100);
    u8g2.print( "Really whatever" ) ;

    u8g2.sendBuffer();

    Serial.print("Updating CreditsScreen with new data...\n");
  }
// ## END LCD Data displaying


// ## LittleFS Functions
  void writeFileLittleFS(fs::FS &fs, const char * path, const char * message){
      Serial.printf("Writing file: %s\r", path);

      File file = fs.open(path, FILE_WRITE);
      if(!file){
          Serial.println(" - failed to open file for writing");
          return;
      }
      if(file.print(message)){
          Serial.println(" - file written");
      } else {
          Serial.println(" - write failed");
      }
      file.close();
  }

  void writeFileLittleFS2(fs::FS &fs, const char * path, const char * message){
      //suppose to be more optimized version of writeFileLittleFS
      Serial.printf("Writing2 file: %s\r", path);

      File file = fs.open(path, FILE_WRITE);
      if(!file){
          Serial.println(" - failed to open file for writing");
          return;
      }

      // if(file.print(message)){
      //     Serial.println(" - file written");
      // } else {
      //     Serial.println(" - write failed");
      // }
      size_t bytesWritten = file.write(reinterpret_cast<const uint8_t*>(message), strlen(message));

      if (bytesWritten == strlen(message)) {
          Serial.println(" - file written successfully");
      } else {
          Serial.println(" - write failed");
      }


      file.close();
  } //suppose to be more optimized version of writeFileLittleFS (under construction)

  void appendFileLittleFS(fs::FS &fs, const char * path, const char * message){
      Serial.printf("Appending to file: %s\r\n", path);

      File file = fs.open(path, FILE_APPEND);
      if(!file){
          Serial.println("- failed to open file for appending");
          return;
      }
      if(file.print(message)){
          Serial.println("- message appended");
      } else {
          Serial.println("- append failed");
      }
      file.close();
  }

  void readFileLittleFS(fs::FS &fs, const char * path){
      Serial.printf("Reading file: %s\r", path);

      File file = fs.open(path);
      if(!file || file.isDirectory()){
          Serial.println(" - failed to open file for reading");
          return;
      }

      Serial.println(" - read from file:");
      while(file.available()){
          Serial.write(file.read());
      }
      file.close();
  }

  void listDirLittleFS(fs::FS &fs, const char * dirname, uint8_t levels){
      Serial.printf("Listing directory: %s\r\n", dirname);

      File root = fs.open(dirname);
      if(!root){
          Serial.println("- failed to open directory");
          return;
      }
      if(!root.isDirectory()){
          Serial.println(" - not a directory");
          return;
      }

      File file = root.openNextFile();
      while(file){
          if(file.isDirectory()){
              Serial.print("  DIR : ");
              Serial.println(file.name());
              if(levels){
                  listDirLittleFS(fs, file.path(), levels -1);
              }
          } else {
              Serial.print("  FILE: ");
              Serial.print(file.name());
              Serial.print("\tSIZE: ");
              Serial.println(file.size());
          }
          file = root.openNextFile();
      }
  }

  void deleteFileLittleFS(fs::FS &fs, const char * path){
      Serial.printf("Deleting file: %s\r\n", path);
      if(fs.remove(path)){
          Serial.println("- file deleted");
      } else {
          Serial.println("- delete failed");
      }
  }
// ## END LittleFS Functions


// ## LittleFS my Functions

  void SaveDataToLittleFS_minavgmax( float floatvar[][MinAvgMaxDataBuffer], byte rows, int cols ){ /*fs::FS &fs, const char * path,*/ 
    String temp = "";
    for(int row = 0; row < rows; row++){
      temp += "{";
      for(int col = 0; col < cols; col++){

        // make it so it always has 4 digit number even it if it < 1000;
        // assuming no reading is in value below -999
        if(floatvar[row][col] <= -100) {temp += String(floatvar[row][col]);} 
        else if(floatvar[row][col] <= -10) {temp += "-0" + String(abs(floatvar[row][col]));}
        else if(floatvar[row][col] < 0) {temp += "-00" + String(abs(floatvar[row][col]));}
        else if(floatvar[row][col] < 10) {temp += "000" + String(floatvar[row][col]);}
        else if(floatvar[row][col] < 100) {temp += "00" + String(floatvar[row][col]);} 
        else if(floatvar[row][col] < 1000) {temp += "0" + String(floatvar[row][col]);} 
        else { temp += String(floatvar[row][col]); } 

        if (col < cols - 1) {
          temp += ",";  // Add a comma between elements (except the last one in the row)
        } else {temp += "},";}
      }
    }
    temp.remove(temp.length() - 1, 1); //remove last character, that is ',', works fast.
    writeFileLittleFS(LittleFS, MEASUREDDATAMINAVGMAX_FILE, temp.c_str());
  }

  void SaveDataToLittleFS_historical( float floatvar[][HistoricalDataBuffer], byte rows, int cols ){ /*fs::FS &fs, const char * path,*/ //only for 2D arrays
    String temp = "";
    for(int row = 0; row < rows; row++){
      temp += "{";
      for(int col = 0; col < cols; col++){

        // make it so it always has 4 digit number even  if it  is < 1000;
        // assuming no reading is in value below -999
        if(floatvar[row][col] <= -100) {temp += String(floatvar[row][col]);} 
        else if(floatvar[row][col] <= -10) {temp += "-0" + String(abs(floatvar[row][col]));}
        else if(floatvar[row][col] < 0) {temp += "-00" + String(abs(floatvar[row][col]));}
        else if(floatvar[row][col] < 10) {temp += "000" + String(floatvar[row][col]);}
        else if(floatvar[row][col] < 100) {temp += "00" + String(floatvar[row][col]);} 
        else if(floatvar[row][col] < 1000) {temp += "0" + String(floatvar[row][col]);} 
        else { temp += String(floatvar[row][col]); } 

        if (col < cols - 1) {
          temp += ",";  // Add a comma between elements (except the last one in the row)
        } else {temp += "},";}
      }
    }
    temp.remove(temp.length() - 1, 1); //remove last character, that is ',', works fast.
    writeFileLittleFS(LittleFS, MEASUREDDATAHISTORICAL_FILE, temp.c_str());
  } 

  String readFileOutLittleFS(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r", path);

    File file = fs.open(path, "r");
    if(!file || file.isDirectory()){
        Serial.println(" - failed to open file for reading");
        return "error"; }

    Serial.println(" - read from file.");

    String outString = "";
    while(file.available()){
      // Serial.write(file.read());
      char byte = file.read();
      outString += byte;
    }
    file.close();
    return outString;
  }

  String readFileOutLittleFS2(fs::FS &fs, const char * path){
    // suppose to be more optimized version of readFileOutLittleFS
    Serial.printf("Reading file: %s\r", path);

    File file = fs.open(path, "r");
    if(!file || file.isDirectory()){
        Serial.println(" - failed to open file for reading");
        return "error";
    }

    
    Serial.println(" - read from file.");

    String outString = "";
    while(file.available()){
      // Serial.write(file.read());
      char byte = file.read();
      outString += byte;
    }

    //This below or this above commented. This below is around 400ms (at 1024 buffer) in reading but sometimes reads garbage, and above around 750ms.

    // const size_t bufferSize = 1024;  // 1024, 1kB buffer size
    // char buffer[bufferSize];
    // String outString = "";
    // while (file.available()) {
    //   size_t bytesRead = file.readBytes(buffer, bufferSize);
    //   // Append the read data to outString
    //   outString += String(buffer);  
    // }


    file.close();
    //Serial.println(outString);
    return outString;
  } // suppose to be more optimized version of readFileOutLittleFS (under construction)

  void ReadDataFromLittleFS_minavgmax2( void ){ /*fs::FS &fs, const char * path, float floatvar */ 
    String input = readFileOutLittleFS(LittleFS, MEASUREDDATAMINAVGMAX_FILE);
    String group1 = input.substring( 1, input.indexOf('}') ); //skip first '{' and cut up to first '}' (without it)

    input = input.substring(input.indexOf('}')+2); 
    String group2 = input.substring( 1, input.indexOf('}') ); //skip first '{' and cut up to first '}' (without it)
    
    input = input.substring(input.indexOf('}')+2); 
    String group3 = input.substring( 1, input.indexOf('}') ); //skip first '{' and cut up to first '}' (without it)
    
    input = input.substring(1);// skip '{'
    String group4 =  input.substring( input.indexOf('{')+1, input.length()-1 ); //skip last '}' 

    for( int row = 0; row < GraphsNumber; row++){
      // String u; //very temp but required
      switch(row){
        case 0:
          for ( int col = 0; col < MinAvgMaxDataBuffer; col++){
            String u = group1.substring(col*8, col*8+7); //end exclusive
            MeasuredDataMinAvgMax[row][col] = u.toFloat();
          }
          break;

        case 1:
          for ( int col = 0; col < MinAvgMaxDataBuffer; col++){
            String u = group2.substring(col*8, col*8+7);
            MeasuredDataMinAvgMax[row][col] = u.toFloat();
          }
          break;

        case 2:
          for ( int col = 0; col < MinAvgMaxDataBuffer; col++){
            String u = group3.substring(col*8, col*8+7);
            MeasuredDataMinAvgMax[row][col] = u.toFloat();
          }
          break;

        case 3:
          for ( int col = 0; col < MinAvgMaxDataBuffer; col++){
            String u = group4.substring(col*8, col*8+7);
            MeasuredDataMinAvgMax[row][col] = u.toFloat();
          }
          break;

        default:
          break; 
      }//end switch

    }//end for 
  } // END ReadDataFromLittleFS_minavgmax2

  void ReadDataFromLittleFS_historical2( void ){/*fs::FS &fs, const char * path, float floatvar */ 
    String input = readFileOutLittleFS(LittleFS, MEASUREDDATAHISTORICAL_FILE);
    String group1 = input.substring( 1, input.indexOf('}') ); //skip first '{' and cut up to first '}' (without it)


    input = input.substring(input.indexOf('}')+2); 
    String group2 = input.substring( 1, input.indexOf('}') ); //skip first '{' and cut up to first '}' (without it)
    
    input = input.substring(input.indexOf('}')+2); 
    String group3 = input.substring( 1, input.indexOf('}') ); //skip first '{' and cut up to first '}' (without it)
    
    input = input.substring(1);// skip '{'
    String group4 =  input.substring( input.indexOf('{')+1, input.length()-1 ); //skip last '}'     

    for( int row = 0; row < GraphsNumber; row++){
      // String u; //very temp but required
      switch(row){
        case 0:
          for ( int col = 0; col < HistoricalDataBuffer; col++){
            String u = group1.substring(col*8, col*8+7); //end exclusive
            MeasuredDataHistorical[row][col] = u.toFloat();
          }
          break;

        case 1:
          for ( int col = 0; col < HistoricalDataBuffer; col++){
            String u = group2.substring(col*8, col*8+7);
            MeasuredDataHistorical[row][col] = u.toFloat();
          }
          break;

        case 2:
          for ( int col = 0; col < HistoricalDataBuffer; col++){
            String u = group3.substring(col*8, col*8+7);
            MeasuredDataHistorical[row][col] = u.toFloat();
          }
          break;

        case 3:
          for ( int col = 0; col < HistoricalDataBuffer; col++){
            String u = group4.substring(col*8, col*8+7);
            MeasuredDataHistorical[row][col] = u.toFloat();
          }
          break;

        default:
          break; 
      }//end switch

    }//end for 

  } // END ReadDataFromLittleFS_historical2

  void nuke_little_fs_just_here_me_out( void ){

    if(!LittleFS.begin(true)){ // true == FORMAT_LITTLEFS_IF_FAILED
      Serial.print("LittleFS Mount Failed\n");}
    else {Serial.print("Little FS Mounted Successfully\n");}
    
    if (!LittleFS.format()) {    Serial.println("LittleFS Format Failed");  } 
    else {    Serial.println("LittleFS Formatted Successfully");  }

    LittleFS.end();
    Serial.println("LittleFS unmounted after formatting");

    if(!LittleFS.begin(false)){ // true == FORMAT_LITTLEFS_IF_FAILED
      Serial.print("LittleFS Mount Failed\n");}
    else {Serial.print("Little FS Mounted Successfully\n");}

    listDirLittleFS(LittleFS, "/", 2);

    Serial.println("Creating test file...");
    writeFileLittleFS(LittleFS, "/data.txt", "MY ESP32 DATA \r\n");
    listDirLittleFS(LittleFS, "/", 2);
    appendFileLittleFS(LittleFS, "/data.txt", (String("SomeRandomTExt")+ "\r\n").c_str()); //Append data to the file
    listDirLittleFS(LittleFS, "/", 2);
    readFileLittleFS(LittleFS, "/data.txt"); // Read the contents of the file
    deleteFileLittleFS(LittleFS, "/data.txt");
    listDirLittleFS(LittleFS, "/", 2);
    
    LittleFS.end();
    Serial.println("LittleFS has been unmounted.");
  } //just in case, not used as far as i know but was used one time

  // ## Prepare files on LittleFS:
    void PrepareLittleFSFiles( void ){
      if(false){Serial.println("Nuking LittleFS..."); nuke_little_fs_just_here_me_out();} //used when littlefs starts doing weird things

      bool fileexists;

      if(!LittleFS.begin(false)){ // true == FORMAT_LITTLEFS_IF_FAILED
        Serial.print("LittleFS Mount Failed\n");}
      //else {Serial.print("Little FS Mounted Successfully\n");}
      
      listDirLittleFS(LittleFS, "/", 2);

      // Check if the files already exists to prevent overwritting existing data
      fileexists = LittleFS.exists(MEASUREDDATAMINAVGMAX_FILE);
      if(!fileexists) {
        Serial.print("File doesn't exist. Creating file " + String(MEASUREDDATAMINAVGMAX_FILE) + " with default values...\n");
        // Create File and add header
        writeFileLittleFS(LittleFS, MEASUREDDATAMINAVGMAX_FILE, "injtial text with typo\r\n");
      }
      //else {Serial.print("File already exists " + String(MEASUREDDATAMINAVGMAX_FILE) + ".\n" );}

      fileexists = LittleFS.exists(MEASUREDDATAHISTORICAL_FILE);
      if(!fileexists) {
        Serial.print("File doesn't exist. Creating file " + String(MEASUREDDATAHISTORICAL_FILE) + " with default values...\n");
        // Create File and add header
        writeFileLittleFS(LittleFS, MEASUREDDATAHISTORICAL_FILE, "injtial text with typo\r\n");
      }
      //else {Serial.print("File already exists " + String(MEASUREDDATAHISTORICAL_FILE) + ".\n" );}
    }
  // ## Prepare files on LittleFS:

// ## END LittleFS my Functions


// ## Plotting graphs functions 

  // 1. first based on updated MeasuredDataHistorical shrink it down to GraphDataHistorical
  void DataShrinkerForGraphs( void ){
    for (int graphnumber = 0; graphnumber < GraphsNumber; graphnumber++){
      for (int i = 0; i < GraphDataLimit-1; i++) {
        float sum = 0;
        // collect sum of 13 values to sum it up. 
        // (105)*13 + 13 = 1378. 1440 - 1378 = 62 last records are converted into one saved into graph variable for plotting
        for (int j = 0; j < 13; j++) {  
          sum += MeasuredDataHistorical[graphnumber][i * 13 + j];
        }
      GraphDataHistorical[graphnumber][i] = sum / 13;
      }

      float sum = 0;
      for( int i = 106*13; i < 1440; i++){
        sum += MeasuredDataHistorical[graphnumber][i];
      }
      GraphDataHistorical[graphnumber][GraphDataLimit-1] = sum / 62;
    }
  } // converts MeasuredDataHistorical to GraphDataHistorical

  // // 2. second, add this new data to GraphDataHistorical
  // //   controversial move: shrink data every time there was update in MeasuredDataHistorical isnetad of updating GraphDataHistorical 
  // void graphDataAdder ( byte graphnumber, float new_value ){
  //   for (int i = 0; i < GraphDataLimit; i++) {
  //     GraphDataHistorical[graphnumber][i] = GraphDataHistorical[graphnumber][i + 1];
  //   }
  //   // Adding new variable
  //   GraphDataHistorical[graphnumber][GraphDataLimit - 1] = new_value;
  // } //adds new data to GraphDataHistorical

  // 3. third, draw and display graphs based on GraphDataHistorical content
  void graphUpdater( byte graphnumber, U8g2Graphing &graph ){
    
    u8g2.setFont(u8g2_font_ncenB08_tr);
    switch (graphnumber) {
      case 0:
        u8g2.drawStr(0,8,"Temp. Graph  24h");
        break;
      case 1:
        u8g2.drawStr(0,8,"Hum. Graph  24h");
        break;
      case 2:
        u8g2.drawStr(0,8,"Press. Graph  24h");
        break;
      case 3:
        u8g2.drawStr(0,8,"CO2. Graph  24h");
        break;
      default:
        break;  
    } //end switch 

    graph.clearData();
    graph.rangeSet(false);
    for (int i = 0; i < GraphDataLimit; i++) {
      graph.inputValue(GraphDataHistorical[graphnumber][i]);
    }
    graph.displayGraph();

    //add midpoijnt on Yaxis scale
    u8g2.setFont(u8g2_font_tom_thumb_4x6_tn);
    u8g2.setCursor(0, 38);
    u8g2.print( String( float(  (graph.getMin()+graph.getMax())/2 ), 1  ) );

    u8g2.sendBuffer();
  } //this prints all graph from scratch

  void Updater_MeasuredDataHistorical( byte graphnumber, float readout ){
      for (int i = 0; i < HistoricalDataBuffer; i++) {
      MeasuredDataHistorical[graphnumber][i] = MeasuredDataHistorical[graphnumber][i + 1];
    }
    // Adding new variable
    MeasuredDataHistorical[graphnumber][HistoricalDataBuffer - 1] = readout;
  } //adds new data to MeasuredDataHistorical

  void Updater_MeasuredDataMinAvgMax( byte graphnumber, float readout ){
    if( MeasuredDataMinAvgMax[graphnumber][0] > readout ) { MeasuredDataMinAvgMax[graphnumber][0] = readout; } // min updater
    if( MeasuredDataMinAvgMax[graphnumber][1] < readout ) { MeasuredDataMinAvgMax[graphnumber][1] = readout; } // max updater
    //update average by applying formula: new_average = ( old_average * readings_number ) +  new_reading )/(readings_number + 1)
    MeasuredDataMinAvgMax[graphnumber][2] = float( (MeasuredDataMinAvgMax[graphnumber][2] * MeasuredDataMinAvgMax[graphnumber][3] + readout)/(MeasuredDataMinAvgMax[graphnumber][3] + 1) );
    MeasuredDataMinAvgMax[graphnumber][3] = MeasuredDataMinAvgMax[graphnumber][3]+1; // update amount of readings
  } //updates MeasuredDataMinAvgMax

// ## END Plotting graphs functions 


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
      SCD40.begin(Wire, SCD40_I2C_ADDR_62); //0x62 addreess

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

    // ## LittleFS reading
      PrepareLittleFSFiles();
      ReadDataFromLittleFS_minavgmax2();
      ReadDataFromLittleFS_historical2();
    // ## END LittleFS reading

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
      SCD40.begin(Wire, SCD40_I2C_ADDR_62); //0x62 addreess

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

    // ## LittleFS reading
      PrepareLittleFSFiles();
      ReadDataFromLittleFS_minavgmax2();
      ReadDataFromLittleFS_historical2();
    // ## END LittleFS reading

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
      SCD40.begin(Wire, SCD40_I2C_ADDR_62); //0x62 addreess

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

    // ## LittleFS reading
      PrepareLittleFSFiles();
      ReadDataFromLittleFS_minavgmax2();
      ReadDataFromLittleFS_historical2();
    // ## END LittleFS reading

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


// ## Helpers functions (DS3231 alarms preparations, Updating Historical and MinMaxAvg)

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


  void UPDATEHistoricalANDMinMaxAvg ( void ){
    Updater_MeasuredDataHistorical(0, float(bme680_readings.temp)); //temp
    Updater_MeasuredDataHistorical(1, float(bme680_readings.hum)); //hum
    Updater_MeasuredDataHistorical(2, float(bme680_readings.press / 100.0)); //press
    if ( scd40_readings.exit_code == 0 ){ Updater_MeasuredDataHistorical(3, scd40_readings.co2 );} //co2
    else{ 
      Updater_MeasuredDataHistorical(3, MeasuredDataHistorical[3][HistoricalDataBuffer-1]); 
    } //just copy last valid reading
    
    Updater_MeasuredDataMinAvgMax(0, float(bme680_readings.temp)); //temp
    Updater_MeasuredDataMinAvgMax(1, float(bme680_readings.hum)); //hum
    Updater_MeasuredDataMinAvgMax(2, float(bme680_readings.press / 100.0)); //press
    if ( scd40_readings.exit_code == 0 ){ Updater_MeasuredDataMinAvgMax(3, scd40_readings.co2 );} //co2
    else{ 
      Updater_MeasuredDataMinAvgMax(3, MeasuredDataHistorical[3][HistoricalDataBuffer-1]); //just copy the last valid reading
      //required cause we need number of readings to go up so let it do its thing
    }

  }

// ## END Helpers functions


// ##

// ## END