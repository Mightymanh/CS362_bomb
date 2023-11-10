#include <TimeLib.h>
#include <LiquidCrystal.h>
#include <Servo.h>

char introMessage[] = "Welcome player! Please pay attention to this screen."
                    "Your job is to defuse a bomb triggered by the CS 362 Riddlers working at Troy's Organization."
                    "Your goal is to cut the correct wire in under 5 minutes."
                    "Good luck with your defusal journey and your timer starts very soon **HAHAHAHAHAHAHAHAHAHAHA** - CS 362 Riddlers";

// components
Servo servoKey, servoReward;
int buzzer = 0; // TODO implement buzzer
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// time variable
const long timeLimit = 300000; // 5 minutes = 5 * 60000ms
unsigned long startTime, currTime, timeRemaining;

// puzzle variables
int solved1, solved2, solved3; // 0 means not solved, 1 means solved. For puzzle3 -1 means explode, 0 means nothing, 1 means defused
int bombStatus; // -1 means explode, 0 means nothing, 1 means defused

// Signals received from and sent to puzzle arduino
char signalReceived;
char signalSent;

// lcd functions
void clearRow(int i) { // clear row i (i = 0 or 1)
    lcd.setCursor(0, i);
    lcd.print("                ");
}

void writeRow(int i, char* message) {
    clearRow(i);
    lcd.setCursor(0, i);
    lcd.print(message);
}

// time function
void getTime() { // get the remaining time
    currTime = millis();
    timeRemaining = (timeLimit - (currTime - startTime)) / 1000;
    if (timeRemaining < 0) timeRemaining; // edge case where we have negative time
}

void printTime() { // print timeRemaining
    char timeMessage[17];
    clearRow(1);
    sprintf(timeMessage, "%d seconds", timeRemaining);
    lcd.setCursor(0, 1);
    lcd.print(timeMessage);
}

// display intro message scrolling left on lcd, delay each time scrolling down by 10 seconds
void displayIntroMessage() {
    clearRow(0);
    lcd.setCursor(0, 0);
    lcd.print("Welcome player!");  // right now just a simple message, we will implement scrolling left TODO
} 

// start the timer, initialize all components to 0
void triggerBomb() {
    // init time
    startTime = millis();
    timeRemaining = timeLimit / 1000;

    printTime();

    // init puzzle status
    solved1 = 0; solved2 = 0; solved3 = 0;
    bombStatus = 0;    

    // sent signal to puzzle arduino to open puzzle 1 TODO
    Serial.write('1');
}

// explode when cut wrong wire, stop timer
void explode() {
    bombStatus = -1;

    // print Game over message on lcd
    writeRow(0, "Game Over");

    // stop timer
    getTime();
    printTime();

    // invoke buzzer TODO
    
}

// defuse when cut correct wire, stop timer, get reward
void defused() {
    bombStatus = 1;

    // print Victory message on lcd
    writeRow(0, "Victory!");

    // stop timer
    getTime();
    printTime();

    // open reward
    servoReward.write(90);

    // invoke buzzer TODO

}

// check the current timer and decide whether to end the game
void checkTimer() {
    getTime();
    if (timeRemaining == 0) explode();
    else printTime();
}

// get signal from puzzle arduino
/*
    1: puzzle 1 solved
    2: puzzle 2 solved
    3: puzzle 3 solved and defused
    4: puzzle 3 not solved and explode 
*/
void updateFromPuzzler() {
    if (Serial.available()) {
        signalReceived = Serial.read();

        clearRow(0);
        lcd.setCursor(0, 0);
        lcd.print(signalReceived);
    }
}


// check status of puzzle 3
void check3() {
    if (!solved3) {
        if (signalReceived == '4') {
            solved3 = - 1;
            explode(); // explode
        }
        else if (signalReceived == '3')  {
            solved3 = 1;
            defused(); // defuse
        }
    }
}

// check status of puzzle 2
void check2() {
    if (!solved2 && signalReceived == '2') {
        // send to puzzle arduino to trigger hint for puzzle 3 TODO
        Serial.write('3');
        
        // print message
        writeRow(0, "Puzzle 2 solved!");
        solved2 = 1;
    }
}

// check status of puzzle 1
void check1() {
    if (!solved1 && signalReceived == '1') {

        // open key chest TODO
        servoKey.write(90);

        // send to puzzle arduino to trigger hint for puzzle 2
        Serial.write('2');

        // print message that puzzle 1 is solved
        writeRow(0, "Puzzle 1 solved!");
        solved1 = 1;
    }
}

void setup() {
    // set up components
    Serial.begin(9600);
    lcd.begin(16, 2);
    servoKey.attach(10);
    servoReward.attach(9);
    pinMode(buzzer, OUTPUT);
    servoKey.write(0);
    servoReward.write(0);

    // start the bomb
    displayIntroMessage();
    triggerBomb();
}

void loop() {
    if (bombStatus == 0) {
        checkTimer();   
        updateFromPuzzler();    
        check3();   
        check1();   
        check2();   
    }
}
