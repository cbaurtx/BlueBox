# BlueBox

## License
Refer to LICENSE.md

## Known Issues
1. The power supply is not separated trom USB power and in circuit
programming can damage the ESP32  
   So remove the ESP32 or disconnect DC power before connecting to USB
2. No terminating resistors for I2S interface  
   Add 47 Ohm resistors to the cable connected to J303 (SCLK, SDO, LRCK)

#Test Status

|Board            |Test status           |Result                     |
|-----------------|----------------------|---------------------------|
|BlueBox_board_TH |I2S source tested     |Missing 5V power isolation |
|                 |                      |Missing I2S bus termination|
|BlueBox_board_SMD|Not tested at all     |                           |

# Roadmap
Support Bluetooth sink as well (if ESP32 allow to do so).

## Supported Hardware
### Wrover B ESP32 module (Devkit C)
**Caution!**  
The board needs to have the wide form factor.
The distance between the headers needs to be 25.4mm  

### I2S output port
Tested I2S sink: PCM5102A codec (RetroBox).

### Hardware

## OLED Screen (option, not supported)

# Board Pinout

### BlueBox

| Component | Signal    | Devkit C |   | PCM5102A Board |   |
|-----------|-----------|----------|---|----------------|---|
|           |           | IO       |Pin|Connector       |Pin|
|I2S out    | I2S SCLK  | GPIO33   | 8 | J303           | 2 |
|           | I2S SDO   | GPIO32   | 7 |                | 3 |
|           | I2S LRCK  | GPIO13   | 15|                | 4 |
|Mux control| MUXCTRL   | GPIO14   | 12|                | 5 |
|I2S in     | I2S SCLK  | GPIO25   | 9 | J302           | 4 |
|           | I2S SDI   | GPIO26   | 10|                | 3 |
|           | I2S LRCK  | GPIO27   | 11|                | 2 |

## Notes
Devkit C:
The pins shown are for the 38 pin version.

Color of wires for I2S out

| Component | Signal    | Devkit C |   |Board           |   |Wire  |
|-----------|-----------|----------|---|----------------|---|------|
|           |           | IO       |Pin|Connector       |Pin|Color |
|GND        | GND       | GND      |   |                | 1 |Red   |
|I2S out    | I2S SCLK  | GPIO33   | 8 | J303           | 2 |Black |
|           | I2S SDO   | GPIO32   | 7 |                | 3 |Yellow|
|           | I2S LRCK  | GPIO13   | 15|                | 4 |Green |
|Mux control| MUXCTRL   | GPIO14   | 12|                | 5 |Blue  |
|3V3        | 3V3       |          |   |                | 6 |White |

## Cables BlueBox -- RetroBox

Color of wires for I2S out

|BlueBox|Color |Signal |RetroBox|Color |
|-------|------|-------|--------|------|
|1      |Red   |GND    | 6      |White | 
|2      |Black |SCLK   | 5      |Blue  |
|3      |Yellow|SDO    | 4      |Green |
|4      |Green |LRCK   | 3      |Yellow|
|5      |Blue  |MUXCTRL| 2      |Black |
|6      |White |Open   | 1      |Red   |

Insert 47 Ohm resistor into SCLK, SDO and LRCK.


### Software

Adpated from Espressif pipeline_bt_sink example.  
For development documentation refer to    
[Doxygen documentation](./doc.html) (./doc.html).


