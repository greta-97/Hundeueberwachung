//Matrix
// ----------------------------------------------------------
// Arduino Nano steuert 8x8 RGB-Matrix
// ----------------------------------------------------------

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> 
#endif

#define LED_PIN    12
#define LED_COUNT 64

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//Speaker
#include "DFRobotDFPlayerMini.h"
#include <Arduino.h>
#include <HardwareSerial.h>

HardwareSerial MySerial(1);
int volume = 30;


// Create the Player object
DFRobotDFPlayerMini dfPlayer;

//For TOF sensor:
#include <Wire.h>
#include <VL53L1X.h>

VL53L1X sensor;


//For CAM input pins
const int Cam_pin_1 = 25;     // coneted to cam pin 14
const int Cam_pin_2 =  33;      // conecctet to cam pin 2

// variables for reading cam pins:
int Cam_pin_1_state = 0;         // variable for reading the cam pin_1
int Cam_pin_2_state = 0;         // variable for reading the cam pin_2

//For Tof:
long TOF_read = 0;  

//For in or Out going
int no_movement = 0;
int go_in = 1;
int go_out = 2;

int move_direktion = 0;

void setup() {
  
  Serial.begin(115200);
  Serial.println("begin");
  //For TOF sesnor:
  Wire.begin(19,18);  // SDL = 18 SDA = 19
  Wire.setClock(400000); // use 400 kHz I2C

  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1);
  }

  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(15000);
  sensor.startContinuous(15);
  Serial.println("new program");

  //CAM pins
  pinMode(Cam_pin_1, INPUT);
  
  pinMode(Cam_pin_2, INPUT);

  //Speaker
  // Init USB serial port for debugging
  MySerial.begin(9600, SERIAL_8N1, 5, 17); // speed, type, RX, TX 5 verbunden mit TX 17 verbunden mit RX vom DFPlayer Rx zu TX und TX zu RX

 
  // Init serial port for DFPlayer Mini
  Serial.begin(115200);

  dfPlayer.begin(MySerial);  //Use softwareSerial to communicate with mp3
  dfPlayer.setTimeOut(500); //Set serial communication time out 500ms
  dfPlayer.volume(volume);  //Set volume value (0~30).
  dfPlayer.EQ(DFPLAYER_EQ_NORMAL); // Equilaziser
  dfPlayer.outputDevice(DFPLAYER_DEVICE_SD); // Source 
  dfPlayer.play(1);  //Play the first mp3

  //Matrix
  strip.begin();           
  strip.show();            
  strip.setBrightness(50);
}

void loop() {
 
  
  //Ceck cam pins
  Cam_pin_1_state = digitalRead(Cam_pin_1);
  Cam_pin_2_state = digitalRead(Cam_pin_2);

  //Check TOF sensor;
  sensor.read();
  Serial.println(String(millis())+","+String(sensor.read()));
  
  if(TOF_read <= 200) //If the TOF sensor outputs a smaller distans than the door frame
  {
        // check if the CAM, for in or out going
    if (Cam_pin_1_state == LOW && Cam_pin_2_state == HIGH) { //means that the camera has registered that the dog has gone into the kitchen
      Serial.println("get in ");
      move_direktion = go_in;
    } 
    else if (Cam_pin_1_state == HIGH && Cam_pin_2_state == LOW) { // means that the camera has registered that the dog has gone out of the kitchen
      Serial.println("get out ");
      move_direktion = go_out;
    }
    else if (Cam_pin_1_state == LOW && Cam_pin_2_state == LOW) { //means that the camera has not registered any movement
      Serial.println("no motion");
      move_direktion = no_movement;
    }
    else { 
      Serial.println("Cam pins read failed");
    }
  }
  else { 
    Serial.println("TOF sensor read no movement");
  } 
  if(move_direktion == go_in)
  {
    Serial.println("The Dog goes in the Kitchen");
    //play the "Raus" command
    dfPlayer.play(1);
    show1(); //color the matrix red
    delay(500);
  }
  else if(move_direktion == go_out)
  {
    Serial.println("The Dog goes out of the Kitchen");
      show2(); //color the matrix green
  }
  else if(move_direktion = no_movement)
  {
    Serial.println("The Dog is not move the Kitchen");
      show3(); //color the matrix green
  }
 
}

void show1 () { 
  int i;
  for (i = 0; i < 64; i++) {  
    strip.show();
    strip.setPixelColor(i, 255, 0, 0);
    delay (40); }
  strip.clear();
}

void show2 () {
  for (int i = 63; i > -1; i--) {  
    strip.show();
    strip.setPixelColor(i, 0, 255, 0);
    delay (40); }  
  strip.clear();

}

void show3 () {
  for (int i = 63; i > -1; i--) {  
    strip.show();
    strip.setPixelColor(i, 0, 255, 0);
    delay (40); }
}
