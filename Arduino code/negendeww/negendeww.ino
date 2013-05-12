// Negende Wereldwonder
// Code by Victor de Boer, Michiel Hildebrand, Pepijn Borgwat
// Apr 2013

const int buttonPin = 3;     // the number of the pushbutton pin
int buttonState = 0;         // Button state variable for reading the pushbutton status
int currentState = 0;        // State var. What are we doing? 0 = waiting for press; 1 = spinning around 

const int stepperDirectionPin = 8; //the pins of the steppers that control the direction
const int stepperPulsePin = 10; //the pins of the steppers that control the pulse

const int ledPin = 50;      // statusled 2 
const int ledRood = 48;     // statusled 3 (blink when button pushed)

#define SHUTTER_PIN 7    // camera shutter pin
//int switchPin = 3;       // button pin
//int switchState; 

void setup() 
{ 

  // initialize stepper pins
  pinMode(stepperDirectionPin, OUTPUT);  
  digitalWrite(stepperDirectionPin, LOW);
  pinMode(stepperPulsePin, OUTPUT);
  digitalWrite(stepperPulsePin, LOW);

  // init led pins 
  pinMode(ledPin, OUTPUT);  
  digitalWrite(ledPin, LOW);
  pinMode(ledRood,OUTPUT);

  // init shutter pin    
  pinMode(SHUTTER_PIN, OUTPUT);
  digitalWrite(SHUTTER_PIN, LOW);  

  // init the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);     

  // init Serial
  Serial.begin(9600);
  Serial.write('1');  // ser: ready 
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

    Serial.write('3'); //ser: turning and snapping

    steppercam(10,100,0,500);  // make 10 'steps', with size 10 in direction 0, with delays 500
    //Serial.write('4'); // ser: going back

    currentState = 0;
    Serial.write('5'); 

  }
} 


void steppercam(int steps, int stepsize, boolean stepDirection, int delayDuration){
  //first check the direction, if it's not the current direction change it.
  int currentDirection = digitalRead(stepperDirectionPin);
  if(stepDirection != currentDirection){
    digitalWrite(stepperDirectionPin, stepDirection); 
    //this delay makes the motor stop for a while when changing direction.
    delay(100);
  }
  //a for loop to create the pulse
  for(int s=0; s<steps; s++){
    // make foto + flash led
    digitalWrite(SHUTTER_PIN, HIGH);
    digitalWrite(ledPin, HIGH);
    delay(250);
    digitalWrite(SHUTTER_PIN, LOW);
    digitalWrite(ledPin, LOW);

    steppermotor(stepsize);
 
    delay(delayDuration);
  }
}


void steppermotor(int motorsteps){
  for(int i=0; i<motorsteps; i++){ 
    digitalWrite(stepperPulsePin, LOW);  
    digitalWrite(stepperPulsePin, HIGH);
    delay(5); 
    }
  }


