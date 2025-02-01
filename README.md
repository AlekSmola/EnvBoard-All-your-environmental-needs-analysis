# EnvBoard-All-your-environmental-needs-analysis

Board for collecting data about environment you live in.
Data it handles: temperature, humidity, pressure, external temperature, altitude (more-or-less), co2 amount in air.
another data to be described

## Changelog:
- 01.02.2025:   
    - First version which satisfies what I wanted :) Unfortunately, it has a lot of bugs which are mentioned in `TO-DOs` in `MAINMAINmain.ino` file and will be corrected;
    - Added option for retaining Graphs data in DeepSleep using too much data, i.e. 4 arrays for 4 graphs each consisting of 1440 `float`s :heart: -> it obviously won't fit in `RTC_DATA_ATTR` and EEPROM would be great since it's easier to handle, but has limited write cycles which would kill it after about ~2 months. Used **LittleFS** storage which is like a USB drive. All data is converted into `.txt` file and then read from it into appropriate variables (`MeasuredDataMinAvgMax` and `MeasuredDataHistorical`) every SINGLE minute. Hey, it’s not me who calculates all of that. *don't judge*;
    - Graphs now work. They also scale appropriate and display midpoint (last 24h); 
    - Corrected Updating of important variables for Graphs and Statistics;
    - Corrected Statistics which now display actual data and even proper one;
    - Corrected Credits screens with nice formatting;

- 25.12.2024: 	
    - Interrupts and Deep Sleep (DS) now works. Interrupts moved from TIMER to external (EXT1) and are now handled by DS3231 an "extremely accurate I2C real-time clock (RTC)" ~datasheet,  using first alarm. First alarm is alarm(1) not zero;
    - Added presets for Graph screens with historical data and statistics, etc; 
    - Added graphs powered by https://github.com/kampidh/U8g2_Graphing;

- 05.12.2024: 	
    - Initial code upload, some stable stuff with not many features, but core is set up;