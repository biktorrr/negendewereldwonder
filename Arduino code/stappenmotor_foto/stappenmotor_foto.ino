

//the number of steppermotors connected to arduino
#define nrOfSteppers 1
//the pins of the steppers that control the direction
const int stepperDirectionPin = 8;
//the pins of the steppers that control the pulse
const int stepperPulsePin = 10;
const int ledPin = 50;
const int ledRood = 48;

#define SHUTTER_PIN 7
int switchPin = 3;
int switchState;

void setup() {                

    pinMode(stepperDirectionPin, OUTPUT);  
    digitalWrite(stepperDirectionPin, LOW);
    pinMode(stepperPulsePin, OUTPUT);
    digitalWrite(stepperPulsePin, LOW);
    pinMode(ledPin, OUTPUT);  
    digitalWrite(ledPin, LOW);
    pinMode(ledRood,OUTPUT);
    
    pinMode(switchPin,INPUT);
    pinMode(SHUTTER_PIN, OUTPUT);
    digitalWrite(SHUTTER_PIN, LOW);  
    
Serial.begin(9600);  
}

void loop(){
  
    digitalWrite(ledPin, LOW);
    digitalWrite(ledRood,HIGH);
    switchState = digitalRead(switchPin);
    while (switchState == HIGH){ 
      digitalWrite(ledRood,LOW);
      stepper(10,0);
      Serial.println("---pieuw---");
      digitalWrite(ledRood,HIGH);
      switchState = LOW;
    
  }
}

void stepper(int steps, boolean stepDirection){
  //first check the direction, if it's not the current direction change it.
    int currentDirection = digitalRead(stepperDirectionPin);
    if(stepDirection != currentDirection){
     digitalWrite(stepperDirectionPin, stepDirection); 
     //this delay makes the motor stop for a while when changing direction.
      delay(100);
    }
  //a for loop to create the pulse
  for(int s=0; s<steps; s++){
    //the LOW, then HIGH creates the pulse the driver is waiting for, no delay needed.
    digitalWrite(SHUTTER_PIN, HIGH);
    delay(100);
    digitalWrite(SHUTTER_PIN, LOW);
    digitalWrite(ledPin, HIGH);
    Serial.println("---pieuwpieuw---");
    delay(250);
    digitalWrite(ledPin, LOW);
     digitalWrite(stepperPulsePin, LOW);  
    digitalWrite(stepperPulsePin, HIGH);
    //delay(5);
    //digitalWrite(stepperPulsePin, LOW);  
    //digitalWrite(stepperPulsePin, HIGH);
    //delay(5);
    //digitalWrite(stepperPulsePin, LOW);  
    //digitalWrite(stepperPulsePin, HIGH);
    //delay(5);
    //digitalWrite(stepperPulsePin, LOW);  
    //digitalWrite(stepperPulsePin, HIGH);
    delay(500);
  }
}
