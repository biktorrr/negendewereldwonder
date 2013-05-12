// Negende Wereldwonder
// Code by Victor de Boer, Michiel Hildebrand, Pepijn Borgwat
// Apr 2013

#include <LiquidCrystal.h>    //include LCD library
// LCD Connections:
// rs (LCD pin 4) to Arduino pin 12
// rw (LCD pin 5) to Arduino pin 11
// enable (LCD pin 6) to Arduino pin 10
// LCD pin 15 to Arduino pin 13
// LCD pins d4 11, d5 12, d6 13, d7 14 to Arduino pins 5, 4, 3, 2
LiquidCrystal lcd(3, 4, 5, 6, 8, 7, 9);
int backLight = 13;          // pin 13 will control the backlight

const int buttonPin = 38;     // the number of the pushbutton pin
int buttonState = 0;         // Button state variable for reading the pushbutton status
int currentState = 0;        // State var. What are we doing? 0 = waiting for press; 1 = spinning around 

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


#define SHUTTER_PIN 36    // camera shutter pin
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
  pinMode(ledRood, OUTPUT);
  
  // init shutter pin    
  pinMode(SHUTTER_PIN, OUTPUT);
  digitalWrite(SHUTTER_PIN, LOW);  

  // init the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);     

  // init Serial
  Serial.begin(9600);
  Serial.write('1');  // ser: ready 
  
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
  lcd.begin(20,4);
  lcd.clear();                  // start with a blank screen
  lcd.setCursor(0,0);           // set cursor to column 0, row 0 (the first row)
  lcd.print("9e Wereldwonder");   
  lcd.setCursor(0,1);           // set cursor to column 0, row 1
  lcd.print("versie 1.0");
  lcd.setCursor(0,2);         // set cursor to column 0, row 2
  lcd.print("Synergique");
  lcd.setCursor(0,3);         // set cursor to column 0, row 3
  lcd.print("RMO Leiden");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0);           // set cursor to column 0, row 0 (the first row)
  lcd.print("Ik ben WonderBot 9.0");
  lcd.setCursor(0,1);           // set cursor to column 0, row 0 (the first row)
  lcd.print("Zet je maquette op");
  lcd.setCursor(0,2);           // set cursor to column 0, row 0 (the first row)
  lcd.print("de draaischijf en");
  lcd.setCursor(0,3);           // set cursor to column 0, row 0 (the first row)
  lcd.print("druk op de knop.");
  
} 


void loop() 
{ 
  if (currentState == 0){  
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
    buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH) {         
      // turn LED on:    
      Serial.write('2');
      lcd.clear();
      lcd.setCursor(0,0);           // set cursor to column 0, row 0 (the first row)
      lcd.print("Daar gaan we dan!");
      lcd.setCursor(0,1);           // set cursor to column 0, row 0 (the first row)
      lcd.print("Als ik draai mag je");
      lcd.setCursor(0,2);           // set cursor to column 0, row 0 (the first row)
      lcd.print("niet aan de schijf");
      lcd.setCursor(0,3);           // set cursor to column 0, row 0 (the first row)
      lcd.print("of je maquette komen");
      digitalWrite(ledPin, HIGH);  
      digitalWrite(ledRood, LOW);  
      currentState = 1;
      delay(2000);
    }
  } 

  else if (currentState == 1){
    digitalWrite(ledPin, LOW);  

    Serial.write('3'); //ser: turning and snapping
    int sec=0;
    lcd.clear();
    lcd.setCursor(0,0);           // set cursor to column 0, row 0 (the first row)
    lcd.print("WONDERBOT 9.0 STATUS");
    lcd.setCursor(0,1);           // set cursor to column 0, row 0 (the first row)
    lcd.print("Foto's ");
    lcd.setCursor(10,1);
    lcd.print("van de 100");
    lcd.setCursor(0,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("--------------------");
    lcd.setCursor(0,3);           // set cursor to column 0, row 0 (the first row)
    lcd.print("Nog xx seconden!");

    steppercam(100,21,0,250);  // make 10 'steps', with size 10 in direction 0, with delays 500
    //Serial.write('4'); // ser: going back
    //1766 = 360 graden

    currentState = 0;
    Serial.write('5'); 
    lcd.clear();
    lcd.setCursor(0,0);           // set cursor to column 0, row 0 (the first row)
    lcd.print("Het staat erop.");
    lcd.setCursor(0,1);           // set cursor to column 0, row 0 (the first row)
    lcd.print("Als jij je maquette");
    lcd.setCursor(0,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("pakt, maak ik er een");
    lcd.setCursor(0,3);           // set cursor to column 0, row 0 (the first row)
    lcd.print("mooi filmpje van!");
    

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
    Serial.print(s+1); 
    lcd.setCursor(7,1);
    lcd.print(s+1);
    delay(100);
    digitalWrite(SHUTTER_PIN, LOW);
    digitalWrite(ledPin, LOW);
    delay(250);
    steppermotor(stepsize);
 
    delay(delayDuration);
  
  // Light it up
  
  if(s>steps*0.01){
    digitalWrite(L1,HIGH);
    lcd.setCursor(0,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.05){
    lcd.setCursor(1,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.1){
    digitalWrite(L2,HIGH);
    lcd.setCursor(2,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.15){
    lcd.setCursor(3,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.2){
    digitalWrite(L3,HIGH);
    lcd.setCursor(4,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.25){
    lcd.setCursor(5,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.3){
    digitalWrite(L4,HIGH);
    lcd.setCursor(6,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.35){
    lcd.setCursor(7,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.4){
    digitalWrite(L5,HIGH);
    lcd.setCursor(8,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.45){
    lcd.setCursor(9,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.5){
    digitalWrite(L6,HIGH);
    lcd.setCursor(10,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.55){
    lcd.setCursor(11,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.6){
    digitalWrite(L7,HIGH);
    lcd.setCursor(12,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.65){
    lcd.setCursor(13,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.7){
    digitalWrite(L8,HIGH);
    lcd.setCursor(14,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.75){
    lcd.setCursor(15,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.8){
    digitalWrite(L9,HIGH);
    lcd.setCursor(16,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.85){
    lcd.setCursor(17,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.9){
    digitalWrite(L10,HIGH);
    lcd.setCursor(18,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  if(s>steps*0.95){
    lcd.setCursor(19,2);           // set cursor to column 0, row 0 (the first row)
    lcd.print("*");
  }
  }
}


void steppermotor(int motorsteps){
  for(int i=0; i<motorsteps; i++){ 
    digitalWrite(stepperPulsePin, LOW);  
    digitalWrite(stepperPulsePin, HIGH);
    delay(5);
    //Ia = SIN(Stepnumber*360/32)Imax
    //Ib = COS(Stepnumber*360/32)Imax
    }
  }


