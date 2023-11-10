#include <LiquidCrystal.h>

// puzzle 1
int led1, led2 = 0;
int b1, b2, b3, b4;
int b1State, b2State, b3State, b4State, b1State2, b2State2, b3State2, b4State2;
int answerArray1[4];
int correctAnswer[] = {3, 0, 2, 1};
int ledCycleCount = 0;
unsigned long ledTime = 0;
const long ledInterval = 1000;

// puzzle 2
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2; 
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int pot1, pot2; // put to any analog pin
int confirmButton;

// puzzle 3
int brown = 12, white = 8, black = 7, green = 4; 
int valBrown, valWhite, valBlack, valGreen;

// STATE, TIME, ARRAY VARIABLES
char signalReceived, signalSent;
int solved1, solved2, solved3; // 0 means not solved, 1 means solved.
int trigger1, trigger2, trigger3;
int triggerHint3;
int gameStatus; // 1 means games on going, 0 means game finished

// led TESTING
int ledR = 2, ledG = 4, ledY = 7;
int stateR, stateG, stateY;

// DEBUG variable
int DEBUG = 1; 

// reset the puzzles
void freshStart() {
    solved1 = 0; solved2 = 0; solved3 = 0;
    trigger1 = 0; trigger2 = 0; trigger3 = 0;
    triggerHint3 = 0;
    gameStatus = 0;
    stateR = LOW, stateG = LOW, stateY = LOW;
}

/*
    1: start the game, intia, trigger puzzle 1 and 3
    2: trigger puzzle 2
    3: trigger hint for puzzle 3 (puzzle 3 always run from the start)
    4: stop the game and reset
*/
// get signals from main Arduino
void updateFromMain() {
    if (Serial.available()) {
        signalReceived = Serial.read();
        if (DEBUG) {Serial.print("Signal received: "); Serial.println(signalReceived);}

        if (signalReceived == '1') {
            if (!trigger1 || !trigger3) {
                if (DEBUG) {Serial.println("start the game: trigger puzzle 1 and 3");}
                gameStatus = 1;
                trigger1 = 1;   // trigger puzzle 1
                trigger3 = 1;   // trigger puzzle 3
            }
        } 
        else if (signalReceived == '2') {
            if (!trigger2 && solved1) {
                if (DEBUG) {Serial.println("trigger puzzle 2");}
                trigger2 = 1; // trigger puzzle 2
            }
        }
        else if (signalReceived == '3') {
            if (!trigger3 && solved2) {
                if (DEBUG) {Serial.println("trigger hint for puzzle 3");}
                triggerHint3 = 1; // trigger hint for puzzle 3 (puzzle 3 always run from the start)
            }
        }
        else if (signalReceived == '4') {
            if (DEBUG) {Serial.println("trigger reset");}
            freshStart();
        }
    }
}

// blinking led for puzzle 1
void ledCycle() {

}

// PUZZLE FUNCTIONS
void puzzle1() {

}

void puzzle2() {
    
}

void puzzle3() {
    valBrown = digitalRead(brown);
    valWhite = digitalRead(white);
    valBlack = digitalRead(black);
    valGreen = digitalRead(green);
    if (DEBUG) {
        char output[100];
        sprintf(output, "valBrown: %d, valWhite: %d, valBlack: %d, valGreen: %d", valBrown, valWhite, valBlack, valGreen);
        Serial.println(output);
    }
    if (valBrown == 1 && valWhite == 1 && valBlack == 1 && valGreen == 1) {
        if (DEBUG) Serial.println("bomb ticking");
    }
    else if (valBrown == 0 || valWhite == 0 || valGreen == 0) {
        if (DEBUG) Serial.println("exploded!");
        Serial.write('4');
        gameStatus = 0;
    }
    else if (valBlack == 0) {
        if (DEBUG) Serial.println("correct!");
        Serial.write('3');
        solved3 = 1;
        gameStatus = 0;
    }    
}

void setup() {
    Serial.begin(9600);
    // puzzle 1
    pinMode(led1, OUTPUT); pinMode(led2, OUTPUT);
    pinMode(b1, INPUT); pinMode(b2, INPUT); pinMode(b3, INPUT); pinMode(b4, INPUT);
    // // puzzle 2
    // lcd.begin(16, 2);
    // pinMode(pot1, INPUT); pinMode(pot2, INPUT);
    // pinMode(confirmButton, INPUT);
    // puzzle 3
    pinMode(brown, INPUT); pinMode(white, INPUT); pinMode(black, INPUT); pinMode(green, INPUT);

    // TESTING
    pinMode(ledR, INPUT); pinMode(ledG, INPUT); pinMode(ledY, INPUT);

    // fresh start the puzzle Arduino's state variables
    freshStart();
}

void loop() {
    //ledCycle();
    updateFromMain();
    //Serial.print("Game status "); Serial.println(gameStatus);
    if (gameStatus) {
        if (trigger1 && !solved1) puzzle1();
        if (trigger2 && !solved2) puzzle2();
        if (trigger3 && !solved3) puzzle3();
    }
}
