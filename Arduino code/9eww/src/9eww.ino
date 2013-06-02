// Negende Wereldwonder
// Code by Victor de Boer, Michiel Hildebrand, Pepijn Borgwat
// Apr 2013

#include <LiquidCrystal.h>    //include LCD library

/* STATES */

#define STATE_INIT 1
#define STATE_RESET 2
#define STATE_READY 3
#define STATE_BUTTON_PUSHED 4
#define STATE_TURNING_AND_SNAPPING 5
#define STATE_DONE 6
#define STATE_BLINKING 7
#define STATE_WAITING 8

/* RUNTIME STATE */

// Button state variable for reading the pushbutton status
int buttonState = 0;

// State var. What are we doing? 0 = waiting for press; 1 = spinning around 
char currentState = 0;

int photosToGo = 0;

unsigned long endTime = 0;
int counter = 0;

/* PIN CONFIGURATION */

// camera shutter pin
#define SHUTTER_PIN 31

//the pins of the steppers that control the direction
#define MOTOR_DIRECTION_PIN 28

//the pins of the steppers that control the pulse
#define MOTOR_PULSE_PIN 29

#define BUTTON_PIN 45
#define BUTTON_LED_PIN 42

// status led pins
#define N_LEDS 12
const int leds[N_LEDS] = {30, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 43};
int waitingLed = 0;


// lcd settings
#define LCD_RS 23
#define LCD_EN 22
#define LCD_D4 25
#define LCD_D5 24
#define LCD_D6 27
#define LCD_D7 26

LiquidCrystal LCD(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
static FILE lcdout = {0} ;      // LCD FILE structure

/* timing, shutter, motor configuration */
#define MIN_MOTOR_STEP_DELAY 750L
#define MAX_MOTOR_STEP_DELAY 1250L
#define MIN_MOTOR_STEP_DELAY_BACK 250L
#define MAX_MOTOR_STEP_DELAY_BACK 1250L

#define BIG_WHEEL_TEETH 190L
#define SMALL_WHEEL_TEETH 22L
#define STEPS_PER_MOTOR_CIRCLE 3200L
#define PHOTOS_PER_ROUND 100
#define SHUTTER_DELAY 150L
#define SHUTTER_DELAY_AFTER 100L

#define STEPS_PER_TABLE_CIRCLE STEPS_PER_MOTOR_CIRCLE * BIG_WHEEL_TEETH / SMALL_WHEEL_TEETH
#define STEPS_PER_PHOTO STEPS_PER_TABLE_CIRCLE / PHOTOS_PER_ROUND

#define N_BLINKS 5
#define BLINKING_DELAY 300

#define WAITING_DELAY 100

void setup() {
    // init Serial
    Serial.begin(9600);

    setState(STATE_INIT);

    //lcd
    pinMode(LCD_RS, OUTPUT);
    pinMode(LCD_EN, OUTPUT);
    pinMode(LCD_D4, OUTPUT);
    pinMode(LCD_D5, OUTPUT);
    pinMode(LCD_D6, OUTPUT);
    pinMode(LCD_D7, OUTPUT);

    // initialize stepper pins
    pinMode(MOTOR_DIRECTION_PIN, OUTPUT);  
    digitalWrite(MOTOR_DIRECTION_PIN, LOW);
    pinMode(MOTOR_PULSE_PIN, OUTPUT);
    digitalWrite(MOTOR_PULSE_PIN, LOW);

    // init led pins 

    // init shutter pin    
    pinMode(SHUTTER_PIN, OUTPUT);
    digitalWrite(SHUTTER_PIN, LOW);  

    // init the pushbutton pin as an input:
    pinMode(BUTTON_PIN, INPUT);
    pinMode(BUTTON_LED_PIN, OUTPUT);
    digitalWrite(BUTTON_LED_PIN, HIGH);

    //initialize Leds
    for(int i=0; i < sizeof(leds)/sizeof(int); i++) {
        pinMode(leds[i], OUTPUT);
    }

    // pinMode(backLight, OUTPUT);
    // digitalWrite(backLight, HIGH); // turn backlight on. Replace 'HIGH' with 'LOW' to turn it off.
    LCD.begin(20,4);
    // fill in the LCD FILE structure
    fdev_setup_stream(&lcdout, lcd_putchar, NULL, _FDEV_SETUP_WRITE);

    displayInitMessage();

    setState(STATE_RESET);
}

void setState(int state) {
    currentState = state;
    Serial.write(state + 48);
}

void resetAll() {
    counter = 0;
    digitalWrite(MOTOR_DIRECTION_PIN, LOW);

    displayReadyMessage();

    setAllLeds(LOW);

    digitalWrite(BUTTON_LED_PIN, HIGH);

    setState(STATE_READY);
}

void loop() {
    //host command handler
    if(!counter && Serial.available() > 0) {
        int hostState = Serial.read() - 48; //ascii table
        setState(hostState);
    }

    //state loop
    switch(currentState) {
        case STATE_RESET:
            resetAll();
        break;
        case STATE_READY:
            //watch for button push
            buttonState = digitalRead(BUTTON_PIN);
            if(buttonState == HIGH) {
                displayHereWeGoMessage();
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

            digitalWrite(BUTTON_LED_PIN, LOW);
            takePhoto();
            digitalWrite(BUTTON_LED_PIN, HIGH);

            //turn stepper motor
            moveStepperMotor(STEPS_PER_PHOTO);

            photosToGo--;

            //update status message
            displayProgressMessage(photosToGo, false);

            //update leds
            updateLeds(photosToGo);

            if(photosToGo == 0) {
                //we're done
                setState(STATE_DONE);
            }
        break;
        case STATE_DONE:
            //we're turning back
            digitalWrite(MOTOR_DIRECTION_PIN, HIGH);
            counter = STEPS_PER_TABLE_CIRCLE;

            //wait for the host to say he's done..
            digitalWrite(BUTTON_LED_PIN, LOW);

            displayDoneMessage();
            setState(STATE_BLINKING);

            endTime = millis() + (N_BLINKS * BLINKING_DELAY);
        break;
        case STATE_BLINKING:
            setAllLeds(millis() / BLINKING_DELAY % 2);

            if(millis() > endTime) {
                setAllLeds(HIGH);
                setState(STATE_WAITING);
            }
        break;
        case STATE_WAITING:
            int newWaitingLed = millis() / WAITING_DELAY % N_LEDS;

            if(newWaitingLed != waitingLed) {
                digitalWrite(leds[waitingLed], HIGH);
                digitalWrite(leds[newWaitingLed], LOW);
                waitingLed = newWaitingLed;
            }
        break;
    }

    if(currentState > STATE_DONE) {
        if(counter > 0) {
            counter--;
            digitalWrite(MOTOR_PULSE_PIN, LOW);
            digitalWrite(MOTOR_PULSE_PIN, HIGH);

            int n;
            if(counter < 2000) {
                n = counter;
            } else if(STEPS_PER_TABLE_CIRCLE - counter < 2000) {
                n = STEPS_PER_TABLE_CIRCLE - counter;
            } else {
                n = 2000;
            }

            int d = MAX_MOTOR_STEP_DELAY_BACK - sin((PI * n) / 4000) * (MAX_MOTOR_STEP_DELAY_BACK - MIN_MOTOR_STEP_DELAY_BACK);

            delayMicroseconds(d);
        }
    }
}

void takePhoto() {
    digitalWrite(SHUTTER_PIN, HIGH);

    delay(SHUTTER_DELAY);

    digitalWrite(SHUTTER_PIN, LOW);
    
    delay(SHUTTER_DELAY_AFTER);
}

void moveStepperMotor(int steps) {
    for(float i=0; i < steps; i++) { 
        digitalWrite(MOTOR_PULSE_PIN, LOW);
        digitalWrite(MOTOR_PULSE_PIN, HIGH);
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


#define NMSG 5
const char *msgs[NMSG] = {
    "msg1",
    "msg2",
    "msg3",
    "msg4",
    "msg5",
};

void displayProgressMessage(int photosToGo, boolean full) {
    if(full) {
        LCD.clear();
        LCD.setCursor(0,0);
        fprintf(&lcdout, "WONDERBOT 9.0 STATUS");
    }

    LCD.setCursor(0,1);
    fprintf(&lcdout, "Foto %i van de %i", (int)floor(PHOTOS_PER_ROUND-photosToGo), (int) PHOTOS_PER_ROUND);
    LCD.setCursor(0,2);
    int done = floor((double)(PHOTOS_PER_ROUND - photosToGo) / PHOTOS_PER_ROUND * 20);
    for(int i=0; i < done; i++) {
        fprintf(&lcdout, "*");
    }
    for(int i=0; i < 20-done; i++) {
        fprintf(&lcdout, "-");
    }

    double perc = (PHOTOS_PER_ROUND - (double)photosToGo) / PHOTOS_PER_ROUND;
    LCD.setCursor(0, 3);

    int displaymsg = floor((double)(PHOTOS_PER_ROUND - photosToGo) / PHOTOS_PER_ROUND * NMSG);
    fprintf(&lcdout, msgs[displaymsg]);
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

void setAllLeds(int state) {
    for(int i=0; i < N_LEDS; i++) {
        digitalWrite(leds[i], state);
    }
}

void updateLeds(int photosToGo) {
    int lightupled = floor((double)(PHOTOS_PER_ROUND - photosToGo) / PHOTOS_PER_ROUND * N_LEDS);
    digitalWrite(leds[lightupled], HIGH);
}

