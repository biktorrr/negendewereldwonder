// Negende Wereldwonder
// Code by Victor de Boer, Michiel Hildebrand, Pepijn Borgwat
// Apr 2013

#include <LiquidCrystal.h>    //include LCD library

#define SHUTTER_PIN 36    // camera shutter pin
//int switchPin = 3;       // button pin
//int switchState; 

// states
#define STATE_INIT 1
#define STATE_READY 2
#define STATE_BUTTON_PUSHED 3
#define STATE_TURNING_AND_SNAPPING 4
#define STATE_DONE 5
#define STATE_WAITING 6

// timing, shutter, motor configuration
#define MIN_MOTOR_STEP_DELAY 750
#define MAX_MOTOR_STEP_DELAY 1250
#define BIG_WHEEL_TEETH 190
#define SMALL_WHEEL_TEETH 22
#define STEPS_PER_MOTOR_CIRCLE 3200
#define PHOTOS_PER_ROUND 100
#define SHUTTER_DELAY 150
#define SHUTTER_DELAY_AFTER 100

// LCD Connections:
// rs (LCD pin 4) to Arduino pin 12
// rw (LCD pin 5) to Arduino pin 11
// enable (LCD pin 6) to Arduino pin 10
// LCD pin 15 to Arduino pin 13
// LCD pins d4 11, d5 12, d6 13, d7 14 to Arduino pins 5, 4, 3, 2
LiquidCrystal LCD(3, 4, 5, 6, 8, 7, 9);
int backLight = 13;          // pin 13 will control the backlight
static FILE lcdout = {0} ;      // LCD FILE structure

const int buttonPin = 38;     // the number of the pushbutton pin
int buttonState = 0;         // Button state variable for reading the pushbutton status
char currentState = 0;        // State var. What are we doing? 0 = waiting for press; 1 = spinning around 

const int stepperDirectionPin = 34; //the pins of the steppers that control the direction
const int stepperPulsePin = 32; //the pins of the steppers that control the pulse

const int ledPin = 50;      // statusled 2 
const int ledRood = 48;     // statusled 3 (blink when button pushed)

const int L1 = 21;
const int L2 = 22;
const int L3 = 23;
const int L4 = 24;
const int L5 = 25;
const int L6 = 26;
const int L7 = 27;
const int L8 = 28;
const int L9 = 29;
const int L10 = 30;

#define STEPS_PER_TABLE_CIRCLE (double) STEPS_PER_MOTOR_CIRCLE * BIG_WHEEL_TEETH / SMALL_WHEEL_TEETH
#define STEPS_PER_PHOTO (double) STEPS_PER_TABLE_CIRCLE / PHOTOS_PER_ROUND

int photosToGo = 0;

void setup() {
    // init Serial
    Serial.begin(9600);

    setState(STATE_INIT);

    // initialize stepper pins
    pinMode(stepperDirectionPin, OUTPUT);  
    digitalWrite(stepperDirectionPin, LOW);
    pinMode(stepperPulsePin, OUTPUT);
    digitalWrite(stepperPulsePin, LOW);

    // init led pins 
    pinMode(ledPin, OUTPUT);  
    digitalWrite(ledPin, LOW);
    pinMode(ledRood, OUTPUT);

    // init shutter pin    
    pinMode(SHUTTER_PIN, OUTPUT);
    digitalWrite(SHUTTER_PIN, LOW);  

    // init the pushbutton pin as an input:
    pinMode(buttonPin, INPUT);

    //initialize Leds
    pinMode(L1,OUTPUT);
    pinMode(L2,OUTPUT);
    pinMode(L3,OUTPUT);
    pinMode(L4,OUTPUT);
    pinMode(L5,OUTPUT);
    pinMode(L6,OUTPUT);
    pinMode(L7,OUTPUT);
    pinMode(L8,OUTPUT);
    pinMode(L9,OUTPUT);
    pinMode(L10,OUTPUT);

    pinMode(backLight, OUTPUT);
    digitalWrite(backLight, HIGH); // turn backlight on. Replace 'HIGH' with 'LOW' to turn it off.
    LCD.begin(20,4);
    // fill in the LCD FILE structure
    fdev_setup_stream(&lcdout, lcd_putchar, NULL, _FDEV_SETUP_WRITE);

    displayInitMessage();

    //first check the direction, if it's not the current direction change it.
    int currentDirection = digitalRead(stepperDirectionPin);
    if(currentDirection){
        digitalWrite(stepperDirectionPin, false);
        //this delay makes the motor stop for a while when changing direction.
        delay(100);
    }

    delay(3000);

    resetAll();
}

void setState(int state) {
    currentState = state;
    Serial.write(state + 48);
}

void resetAll() {
    displayReadyMessage();

    digitalWrite(ledRood,HIGH);
    digitalWrite(L1,LOW);
    digitalWrite(L2,LOW);
    digitalWrite(L3,LOW);
    digitalWrite(L4,LOW);
    digitalWrite(L5,LOW);
    digitalWrite(L6,LOW);
    digitalWrite(L7,LOW);
    digitalWrite(L8,LOW);
    digitalWrite(L9,LOW);
    digitalWrite(L10,LOW);

    setState(STATE_READY);
}

void loop() {
    //host command handler
    if (Serial.available() > 0) {
        int hostState = Serial.read() - 48; //ascii table
        switch(hostState) {
            case 6:
                resetAll();
            break;
        }
    }

    //state loop
    switch(currentState) {
        case STATE_READY:
            //watch for button push
            buttonState = digitalRead(buttonPin);
            if (buttonState == HIGH) {         
                displayHereWeGoMessage();
                digitalWrite(ledPin, HIGH);  
                digitalWrite(ledRood, LOW);
                delay(2000);
                setState(STATE_BUTTON_PUSHED);
            }
        break;
        case STATE_BUTTON_PUSHED:
            //start the process
            photosToGo = PHOTOS_PER_ROUND;
            displayProgressMessage(photosToGo, true);
            setState(STATE_TURNING_AND_SNAPPING);
        break;
        case STATE_TURNING_AND_SNAPPING:
            //take a photo
            takePhoto();

            //turn stepper motor
            moveStepperMotor(STEPS_PER_PHOTO);

            photosToGo--;

            //update status message
            displayProgressMessage(photosToGo, false);

            //update leds

            if(photosToGo == 0) {
                //we're done
                setState(STATE_DONE);
            }
        break;
        case STATE_DONE:
            //wait for the host to say he's done..
            displayDoneMessage();
            setState(STATE_WAITING);
        break;
        case STATE_WAITING:
            delay(500);
        break;
    }
}

/*
void steppercam(int steps, int stepsize, boolean stepDirection){  
  if(s>steps*0.01){
    digitalWrite(L1,HIGH);
    LCD.setCursor(0,2);
    LCD.print("*");
  }
  if(s>steps*0.05){
    LCD.setCursor(1,2);
    LCD.print("*");
  }
  if(s>steps*0.1){
    digitalWrite(L2,HIGH);
    LCD.setCursor(2,2);
    LCD.print("*");
  }
  if(s>steps*0.15){
    LCD.setCursor(3,2);
    LCD.print("*");
  }
  if(s>steps*0.2){
    digitalWrite(L3,HIGH);
    LCD.setCursor(4,2);
    LCD.print("*");
  }
  if(s>steps*0.25){
    LCD.setCursor(5,2);
    LCD.print("*");
  }
  if(s>steps*0.3){
    digitalWrite(L4,HIGH);
    LCD.setCursor(6,2);
    LCD.print("*");
  }
  if(s>steps*0.35){
    LCD.setCursor(7,2);
    LCD.print("*");
  }
  if(s>steps*0.4){
    digitalWrite(L5,HIGH);
    LCD.setCursor(8,2);
    LCD.print("*");
  }
  if(s>steps*0.45){
    LCD.setCursor(9,2);
    LCD.print("*");
  }
  if(s>steps*0.5){
    digitalWrite(L6,HIGH);
    LCD.setCursor(10,2);
    LCD.print("*");
  }
  if(s>steps*0.55){
    LCD.setCursor(11,2);
    LCD.print("*");
  }
  if(s>steps*0.6){
    digitalWrite(L7,HIGH);
    LCD.setCursor(12,2);
    LCD.print("*");
  }
  if(s>steps*0.65){
    LCD.setCursor(13,2);
    LCD.print("*");
  }
  if(s>steps*0.7){
    digitalWrite(L8,HIGH);
    LCD.setCursor(14,2);
    LCD.print("*");
  }
  if(s>steps*0.75){
    LCD.setCursor(15,2);
    LCD.print("*");
  }
  if(s>steps*0.8){
    digitalWrite(L9,HIGH);
    LCD.setCursor(16,2);
    LCD.print("*");
  }
  if(s>steps*0.85){
    LCD.setCursor(17,2);
    LCD.print("*");
  }
  if(s>steps*0.9){
    digitalWrite(L10,HIGH);
    LCD.setCursor(18,2);
    LCD.print("*");
  }
  if(s>steps*0.95){
    LCD.setCursor(19,2);
    LCD.print("*");
  }
  }
}
*/

void takePhoto() {
    digitalWrite(SHUTTER_PIN, HIGH);
    digitalWrite(ledPin, HIGH);

    delay(SHUTTER_DELAY);

    digitalWrite(SHUTTER_PIN, LOW);
    digitalWrite(ledPin, LOW);
    
    delay(SHUTTER_DELAY_AFTER);
}

void moveStepperMotor(int steps) {
    for(float i=0; i < steps; i++) { 
        digitalWrite(stepperPulsePin, LOW);
        digitalWrite(stepperPulsePin, HIGH);
        int delay = MAX_MOTOR_STEP_DELAY - sin((PI * i) / steps) * (MAX_MOTOR_STEP_DELAY - MIN_MOTOR_STEP_DELAY);
        delayMicroseconds(delay);
    }
}

//----- LCD stuff -----

// LCD character writer
static int lcd_putchar(char ch, FILE* stream) {
    LCD.write(ch) ;
    return (0) ;
}

void displayInitMessage() {
    LCD.clear();
    LCD.setCursor(0,0);
    fprintf(&lcdout, "9e Wereldwonder");
    LCD.setCursor(0,1);
    fprintf(&lcdout, "versie 1.0");
    LCD.setCursor(0,2);
    fprintf(&lcdout, "Synergique");
    LCD.setCursor(0,3);
    fprintf(&lcdout, "RMO Leiden");
}

void displayReadyMessage() {
    LCD.clear();
    LCD.setCursor(0,0);
    fprintf(&lcdout, "Ik ben WonderBot 9.0");
    LCD.setCursor(0,1);
    fprintf(&lcdout, "Zet je maquette op");
    LCD.setCursor(0,2);
    fprintf(&lcdout, "de draaischijf en");
    LCD.setCursor(0,3);
    fprintf(&lcdout, "druk op de knop.");
}

void displayHereWeGoMessage() {
    LCD.clear();
    LCD.setCursor(0,0);
    fprintf(&lcdout, "Daar gaan we dan!");
    LCD.setCursor(0,1);
    fprintf(&lcdout, "Als ik draai mag je");
    LCD.setCursor(0,2);
    fprintf(&lcdout, "niet aan de schijf");
    LCD.setCursor(0,3);
    fprintf(&lcdout, "of je maquette komen");
}

void displayProgressMessage(int photosToGo, boolean full) {
    if(full) {
        LCD.clear();
        LCD.setCursor(0,0);
        fprintf(&lcdout, "WONDERBOT 9.0 STATUS");
    }

    LCD.setCursor(0,1);
    fprintf(&lcdout, "Foto %u van de %u", floor(PHOTOS_PER_ROUND-photosToGo), PHOTOS_PER_ROUND);
    LCD.setCursor(0,2);
    int done = ceil((double)(PHOTOS_PER_ROUND - photosToGo) / PHOTOS_PER_ROUND * 20);
    for(int i=0; i < done; i++) {
        fprintf(&lcdout, "*");
    }
    for(int i=0; i < 20-done; i++) {
        fprintf(&lcdout, "-");
    }
}

void displayDoneMessage() {
    LCD.clear();
    LCD.setCursor(0,0);
    fprintf(&lcdout, "Het staat erop.");
    LCD.setCursor(0,1);
    fprintf(&lcdout, "Als jij je maquette");
    LCD.setCursor(0,2);
    fprintf(&lcdout, "pakt, maak ik er een");
    LCD.setCursor(0,3);
    fprintf(&lcdout, "mooi filmpje van!");
}
