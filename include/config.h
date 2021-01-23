// CONFIGURATION

char ssid[] = "JJROBOTS_63";                // your network SSID (name)
char pass[] = "87654321";      // your network password

const IPAddress outIp(192,168,4,1);   // IP of your robot
#define OUTPORT 2222                    // remote port to receive OSC (EVO 2 defaults to 2222)
#define LOCALPORT 2223                  // local port to listen for OSC packets (actually not used for sending)


char  *btmac = "34:42:62:64:ec:19";     // This is the BT MAC address the controller want's to bind to. 


#define DEADZONEAREA 0.05               // if a controller value is within this value left or right of the center (0.5), 0.5 is returned

// END CONFIGURATION
