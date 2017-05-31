/*
* Code to Connect and send flow sensor and temperature sensor readings to MQTT topic
* By: Inderpreet Singh
*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

volatile int NbTopsFan; //measuring the rising edges of the signal
int Calc;                               
int hallsensor = 2;    //The pin location of the sensor

float tempC;
int reading;
int tempPin = 5;

char message[40];

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 192, 168, 0, 228 };
byte ip[]     = { 192, 168, 0, 230 };

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

void rpm ()     //This is the function that the interupt calls 
{ 
  NbTopsFan++;  //This function measures the rising and falling edge of the hall effect sensors signal
}

void setup()
{
  Ethernet.begin(mac, ip);
  pinMode(hallsensor, INPUT); //initializes digital pin 2 as an input
  attachInterrupt(0, rpm, RISING); //and the interrupt is attached
  analogReference(INTERNAL); // for lm35
  
  if (client.connect("arduinoClient")) {
    client.publish("iot2020/arduino","hello world");
    client.subscribe("inTopic");
  }
}

unsigned int i=65535;
void loop()
{ 
  client.loop();
  if(i==0){ //run every 5 seconds.
    i=40000; //appox 4 seconds or so.
    //Calculate flow
    NbTopsFan = 0;   //Set NbTops to 0 ready for calculations
    sei();      //Enables interrupts
    delay (1000);   //Wait 1 second
    cli();      //Disable interrupts
    Calc = (NbTopsFan * 60 / 7.5); //(Pulse frequency x 60) / 7.5Q, = flow rate in L/hour
    sprintf(message, "%d L/minute", Calc);  
    client.publish("iot2020/arduino", message);
  }
  if(i==20000){
    // Read LM35
    reading = analogRead(tempPin);
    tempC = reading / 9.31;
    sprintf(message, "Temp: %d.%02d", (int)tempC, (int)(tempC*100)%100);
    client.publish("iot2020/arduino", message);
    delay(1000);
  }
  i--;
}

