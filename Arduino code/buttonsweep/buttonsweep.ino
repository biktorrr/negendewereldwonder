// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.


#include <Servo.h> 
const int ledPin =  13;      // the number of the LED pin

const int buttonPin = 2;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status
int currentState = 0; //What are we doing? 0 = waiting for press; 1 = spinning around 

Servo myservo;  // create servo object to control a servo 
// a maximum of eight servo objects can be created 

int pos = 0;    // variable to store the servo position 

void setup() 
{ 


  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);   
  
// attaches the servo on pin 9 to the servo object 
  myservo.attach(9);  

  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);     
  
    // init Serial
  Serial.begin(9600);
  Serial.write('1'); 
} 


void loop() 
{ 
  if (currentState == 0){  
    buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH) {         
      // turn LED on:    
      Serial.write('2'); 
      digitalWrite(ledPin, HIGH);  
      currentState = 1;
      delay(1000);
    } 
  } 

  else if (currentState == 1){
    digitalWrite(ledPin, LOW);  
    
    Serial.write('3'); 

    for(pos = 0; pos < 180; pos += 30)  // goes from 0 degrees to 180 degrees 
    {                                  // in steps of 1 degree 
      myservo.write(pos);              // tell servo to go to position in variable 'pos' 

      // flash led
      digitalWrite(ledPin, HIGH);  
      delay(100); 
      digitalWrite(ledPin, LOW);   
      delay(300); 

      delay(15);                       // waits 15ms for the servo to reach the position 
    } 
        Serial.write('4'); 

    for(pos = 180; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
    {                               
      myservo.write(pos);              // tell servo to go to position in variable 'pos' 
      delay(15);                       // waits 15ms for the servo to reach the position 
    } 

    // flash led twice
    delay(100); 
    digitalWrite(ledPin, HIGH);  
    delay(200);  
    digitalWrite(ledPin, LOW);   
    delay(100);  
    digitalWrite(ledPin, HIGH);  
    delay(200);   
    digitalWrite(ledPin, LOW); 

    currentState = 0;
        Serial.write('5'); 

  }
} 

