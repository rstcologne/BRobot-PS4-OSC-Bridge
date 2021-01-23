#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <PS4Controller.h>
#include "config.h"
#include "Ticker.h"


#define OSCPATH_THROTTLE "/1/fader1"    // Path for throttle messages (0.0-1.0)
#define OSCPATH_STEERING "/1/fader2"    // Path for steering messages (0.0-1.0)
#define OSCPATH_HAND "/1/push1"         // Path for triggering the hand
#define OSCPATH_PRO "/1/toggle1"        // Path for switching to pro



WiFiUDP Udp;                            // A UDP instance to let us send and receive packets over UDP
Ticker controller_ticker;

float old_fader1 = 0.5;
float old_fader2 = 0.5;
int old_cross_button = 0;
int old_circle_button = 0;
int old_triangle_button = 0;

bool pro = false;

#define CONNECTION_LED 32               // GPIO for connection indicator LED
bool ps4initialized = false;            // state of the PS4 library, uninitialized at start, initialized while waiting
                                        // for connections, uninitialized once connection is established
                                        // if connection fails, system is reinitialized. 

// if not yet done, Initialize PS4 Library and wait for controller connections. 
// state is stored in variable ps4initialized
void ps4begin() {
  if (! ps4initialized) {
    ps4initialized = true;
    PS4.begin(btmac);
  }
}

void sendOSCMessage(const char* path, float value) {
    OSCMessage msg(path);
    msg.add(value);
    Udp.beginPacket(outIp, OUTPORT);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
}


// remaps PS4 controller values to expected OSC value ranges and applies a deadzone for the center. 
float remapControllerValues(long val) {
  float newval = 0;
  if (pro == true) {
    newval = map(val, -128,127,0,100) / 100.0;
  } else {
    newval = map(val, -128,127,0,100) / 100.0;
  }
  // Set Deadzone for proper centering (poor man's calibration)
  if (newval > 0.5 - DEADZONEAREA && newval < 0.5 + DEADZONEAREA) {
    newval = 0.5;
  }
  if (pro == false) {
    newval = newval * 1;
  }
  return newval;
}

float reverse_stick (float val) {
 return map(val, -128,127,127,-128);
}

void setLEDNormal() {
  PS4.setLed(0, 255, 0);
}

void setLEDPro() {
  PS4.setLed(255, 0, 0);
}

void setLEDOff() {
  PS4.setLed(0, 0, 0);
}

void processController() {
  if (PS4.isConnected()){

    float fader1 = remapControllerValues(PS4.data.analog.stick.ly);
    float fader2 = remapControllerValues(reverse_stick(PS4.data.analog.stick.rx));
    int cross_button = PS4.data.button.cross;
    int circle_button = PS4.data.button.circle;
    int triangle_button = PS4.data.button.triangle;

    // Debug controller axis values to serial port
    Serial.printf("LY: %f\n", fader1);
    //Serial.printf("LX: %f\n", remapControllerValues(PS4.data.analog.stick.lx));
    //Serial.printf("RY: %f\n", remapControllerValues(PS4.data.analog.stick.ry));
    Serial.printf("RX: %f\n", fader2);
    Serial.printf("Cross: %f", (cross_button));
    Serial.printf("Circle: %f", (circle_button));


    if (old_fader1 != fader1) {
      sendOSCMessage(OSCPATH_THROTTLE, fader1);
      old_fader1 = fader1;
    }

    if (old_fader2 != fader2) {
      sendOSCMessage(OSCPATH_STEERING, fader2);
      old_fader2 = fader2;
    }

    if (old_circle_button != circle_button) {
      if (circle_button == 1) {
        setLEDPro();
        pro = true;
        sendOSCMessage(OSCPATH_PRO, 1);
      }
      old_circle_button = circle_button;
    }

    if (old_triangle_button != triangle_button) {
      if (triangle_button == 1) {
        pro = false;
        sendOSCMessage(OSCPATH_PRO, 0);
      }
      old_triangle_button == triangle_button;
    }

    if (old_cross_button != cross_button) {
      sendOSCMessage(OSCPATH_HAND, cross_button);    
      old_cross_button = cross_button;
    }

    if(pro == true) {
    } else {
      setLEDNormal();
    }
    

    PS4.sendToController();
  }
}



void setup() {
    //Setup pin for connectio indicator led
    pinMode(CONNECTION_LED, OUTPUT);

    // setup serial communication for debugging
    Serial.begin(115200);

    // Connect to WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    digitalWrite(CONNECTION_LED, HIGH);

    Serial.println("Starting UDP");

    //setup for UDP OSC messaging
    Udp.begin(LOCALPORT);

    Serial.print("Local port: ");
    Serial.println(LOCALPORT);

    //Initialize PS4 system and wait for controller connections
    ps4begin();


    controller_ticker.attach(0.1, processController);

}

void loop() {
  //ArduinoOTA.handle();  
  // Check if controller is connected and process
  if (PS4.isConnected()){
    // If controller just connected, set color and set 
    // status that the PS4 system is not anymore freshly initialized.
    if (ps4initialized == true) {
      setLEDNormal();
      ps4initialized = false;
      Serial.println("Controller Connected");
    }

  } else {
    // if we get here, PS4 controller is not connected. 
    // make sure we wait for connections (again)
    ps4begin();
  }
}