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
#define STATE_ENCODER_DONE 9

/* RUNTIME STATE */

// Button state variable for reading the pushbutton status
int buttonState = 0;

// State var. What are we doing? 0 = waiting for press; 1 = spinning around 
char currentState = 0;

int photosToGo = 0;

unsigned long endTime = 0;
int stepsToTurnBack = 0;

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
int newWaitingLed = 0;

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

    setState(STATE_RESET);
}

void setState(int state) {
    currentState = state;
    Serial.write(state + 48);
}

void resetAll() {
    stepsToTurnBack = 0;
    digitalWrite(MOTOR_DIRECTION_PIN, LOW);

    displayReadyMessage();

    setAllLeds(LOW);

    digitalWrite(BUTTON_LED_PIN, HIGH);

    setState(STATE_READY);
}

void loop() {
    //host command handler
    if(Serial.available() > 0) {
        int hostState = Serial.read() - 48; //ascii table
        setState(hostState);
    }

    //state loop
    switch(currentState) {
        case STATE_RESET:
            if(!stepsToTurnBack) {
                resetAll();
            }
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
            stepsToTurnBack = STEPS_PER_TABLE_CIRCLE;

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
            newWaitingLed = millis() / WAITING_DELAY % N_LEDS;

            if(newWaitingLed != waitingLed) {
                digitalWrite(leds[waitingLed], HIGH);
                digitalWrite(leds[newWaitingLed], LOW);
                waitingLed = newWaitingLed;
            }
        break;
        case STATE_ENCODER_DONE:
            displayEncoderDoneMessage();
            delay(5000);
        break;
    }

    if(stepsToTurnBack > 0) {
        stepsToTurnBack--;
        digitalWrite(MOTOR_PULSE_PIN, LOW);
        digitalWrite(MOTOR_PULSE_PIN, HIGH);

        int n;
        if(stepsToTurnBack < 2000) {
            n = stepsToTurnBack;
        } else if(STEPS_PER_TABLE_CIRCLE - stepsToTurnBack < 2000) {
            n = STEPS_PER_TABLE_CIRCLE - stepsToTurnBack;
        } else {
            n = 2000;
        }

        int d = MAX_MOTOR_STEP_DELAY_BACK - sin((PI * n) / 4000) * (MAX_MOTOR_STEP_DELAY_BACK - MIN_MOTOR_STEP_DELAY_BACK);

        delayMicroseconds(d);
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

void displayReadyMessage() {
    LCD.clear();
    LCD.setCursor(0,0);
    fprintf(&lcdout, "Ik ben WonderBot 9.0");
    LCD.setCursor(0,1);
    fprintf(&lcdout, "Zet je wereldwonder");
    LCD.setCursor(0,2);
    fprintf(&lcdout, "op de draaischijf");
    LCD.setCursor(0,3);
    fprintf(&lcdout, "en druk op de knop.");
}

void displayHereWeGoMessage() {
    LCD.clear();
    LCD.setCursor(0,0);
    fprintf(&lcdout, "Maar let op! Als ik");
    LCD.setCursor(0,1);
    fprintf(&lcdout, "draai mag je er niet");
    LCD.setCursor(0,2);
    fprintf(&lcdout, "aankomen, oke?");
    LCD.setCursor(0,3);
    fprintf(&lcdout, "Daar gaan we dan...");
}

void displayProgressMessage(int photosToGo, boolean full) {
    if(full) {
        LCD.clear();
        LCD.setCursor(0,0);
        fprintf(&lcdout, "WonderBot 9.0 STATUS");
    }

    LCD.setCursor(0,1);
    fprintf(&lcdout, "Foto's: %03d / %i", (int)floor(PHOTOS_PER_ROUND-photosToGo), (int) PHOTOS_PER_ROUND);

    LCD.setCursor(0,2);
    int done = floor((double)(PHOTOS_PER_ROUND - photosToGo) / PHOTOS_PER_ROUND * 20);
    for(int i=0; i < done; i++) {
        fprintf(&lcdout, "*");
    }
    for(int i=0; i < 20-done; i++) {
        fprintf(&lcdout, "-");
    }

    LCD.setCursor(0, 3);
    switch(photosToGo) {
        case 100:
            fprintf(&lcdout, "We zijn onderweg!   ");
        break;
        case 75:
            fprintf(&lcdout, "Film wordt gemaakt..");
        break;
        case 50:
            fprintf(&lcdout, "We zijn op de helft!");
        break;
        case 35:
            fprintf(&lcdout, "Nog heel even geduld");
        break;
        case 20:
            fprintf(&lcdout, "Word je al duizelig?");
        break;
    }
}

void displayDoneMessage() {
    LCD.clear();
    LCD.setCursor(0,0);
    fprintf(&lcdout, "       Klaar!");
    LCD.setCursor(0,1);
    fprintf(&lcdout, "Je filmpje wordt nu");
    LCD.setCursor(0,2);
    fprintf(&lcdout, "gemaakt.");
    LCD.setCursor(0,3);
    fprintf(&lcdout, "Nog even geduld.....");
}

void displayEncoderDoneMessage() {
    LCD.clear();
    LCD.setCursor(0,0);
    fprintf(&lcdout, "Je film is af!");
    LCD.setCursor(0,1);
    fprintf(&lcdout, "Bekijk hem links om");
    LCD.setCursor(0,2);
    fprintf(&lcdout, "de hoek. Vergeet je");
    LCD.setCursor(0,3);
    fprintf(&lcdout, "wereldwonder niet!");
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

