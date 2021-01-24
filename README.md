# BRobot-PS4-OSC-Bridge
This code can be run on an ESP32 board and allows controlling a BRobot EVO 2 with a PS4 controller. 

I compile the program with VS Code and PlatformIO, however the Arduino IDE should also work. The steps below assume VS Code and PlatformIO

## Dependencies

You need the following library downloaded and added to the lib directory:

https://github.com/aed3/PS4-esp32

Please follow the documentation provided for the PS4 library to identify your PS2 controller's address. 

## Configuration
Before compiling and uploading to your board, modify config.h in the include dir to match your configuration. 

Required configurations are:
1. The SSID and password of your EVO Robot (or your home wifi) 
2. Your controller's MAC adress as identified before

If you connect the ESP to your home WIFI (because your EVO 2 is also connected to your home WIFI), you need to configure your EVO's IP in the home network as well (outIP). By default it is assumed you connect to your EVO's WIFI directly. 

You can also increase or decrease a dead-zone around the stick center. This is implemented to really stop when sticks are centered even if they are not fully hitting the center. Consider it to be a poor man's controller calibration. 

You can connect a LED to GPIO 32. It lights up when the WIFI connection was successful. 

Also please configure the board you are using in platformio.ini

## Startup Sequence
Once you flashed an ESP32 board (Tested with DEVkITC V4 and bare ESP32 WROOM modules), the following steps should be performed:

1. Start your robot and wait until it wiggles after calibration
2. Start the ESP with this code flashed. If you added the LED, wait for it to light up. Otherwise monitor the serial port for status updates. 
3. Switch on the PS4 controller, once connected the controller LED should light up green

## Controller assignment
- Throttle: Left stick up and down
- Steering: Right stick left and right
- Cross button: Trigger the arm
- Circle button: Toggle "pro" mode
- Triangle button: Toggle "normal" mode

### LED:
- Off: Controller not connected
- Green: Controller connected and normal mode active
- Red: Controller connected and pro mode active


