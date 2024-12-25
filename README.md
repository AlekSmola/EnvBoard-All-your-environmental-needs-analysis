# EnvBoard-All-your-environmental-needs-analysis

Board for collecting data about environment you live in.
Data it handles: temperature, humidity, pressure, external temperature, altitude (more-or-less), co2 amount in air.
another data to be described

## Changelog:
- 25.12.2024: 	Interrupts and Deep Sleep (DS) now works. Interrupts move from TIMER to external (EXT1) and are now handled by DS3231 an "extremely accurate I2C real-time clock (RTC)" ~datasheet,  using first alarm. First alarm is alarm(1) not zero. Also added presets for Graph screens with historical data and statistics, etc. 
- 05.12.2024: 	Initial code upload, some stable stuff with not many features, but core is set up.
