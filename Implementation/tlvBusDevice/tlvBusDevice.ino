/*!
*	\file		tlvBusDevice
*	\author 	Inderpreet Singh (google.com/+InderpreetSingh)
* 	\license	GPL2(see license.txt)
*
*	\section 	Description
*                       TVL Protocol End Device using Arduino
*                       See Protocol.h for Details of communication
*
*	\section	HISTORY
*	v1.0
*/
// ----------------------------------------------------------------------------

#include "protocol.h"

struct sFrame myFrame, buffer;            // Object to store the frame - sFrame is the name of structure.
char *ptr = (char*)&buffer;      // Point to the object
boolean inFrame=false;
boolean FrameReady=false;
unsigned char myIndex = 0;
const int Lamp=13;
const int Pump = 2;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  myFrame.cID='0';
  pinMode(13, OUTPUT);
  pinMode(Pump, OUTPUT);
}

void loop() {
  // print the string when a newline arrives:

  if(FrameReady==true){   //Unprocessed Frame exisits
     if(myFrame.cID=='P'){
        switch(myFrame.cValue){
          case '0':
            //Switch Off Pump Here
            digitalWrite(Pump, LOW);
            break;
          case '1':
            //Switch ON Pump Here
            digitalWrite(Pump, HIGH);
            break;
        }
     } else if(myFrame.cID=='L'){
        switch(myFrame.cValue){
          case '0':
            //Switch Off Pump Here
            digitalWrite(Lamp, LOW);
            break;
          case '1':
            //Switch ON Pump Here
            digitalWrite(Lamp, HIGH);
            break;
        }
    }
    FrameReady=false;
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
      // Read a char
      char inChar = (char)Serial.read(); 
      if(inChar==':'){                         // Start New Frame
          memset(ptr, 0, FRAMESIZE);
          inFrame=true;
          myIndex=0;
      }
      if(inFrame==true  && myIndex<FRAMESIZE){ // Add a byte
            *(ptr+myIndex)=inChar;
            myIndex++;
      }
      if(myIndex==FRAMESIZE){
          inFrame=false;
          FrameReady=true;
          memcpy(&myFrame, ptr, FRAMESIZE);
      }
  }
}

