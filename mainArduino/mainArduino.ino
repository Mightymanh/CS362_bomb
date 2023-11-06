#include <TimeLib.h>
#include <LiquidCrystal.h>
#include <Servo.h>

char introMessage[] = "Welcome player! Please pay attention to this screen."
                    "Your job is to defuse a bomb triggered by the CS 362 Riddlers working at Troy's Organization."
                    "Your goal is to cut the correct wire in under 5 minutes."
                    "Good luck with your defusal journey and your timer starts very soon **HAHAHAHAHAHAHAHAHAHAHA** - CS 362 Riddlers";

// components
Servo servoKey, servoReward;
int buzzer = 0;
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// time variable
int timeLimit = 5 * 60000; // 5 minutes = 5 * 60000ms
int startTime, currTime, timeRemaining;

// puzzle variables
int solved1, solved2, solved3; // 0 means not solved, 1 means solved
int bombStatus; // keep track of puzzle 3 whether player cuts correct wire, -1 means explode, 0 means nothing, 1 means defused
int keyOpened, rewardOpened;

// lcd message format

// Signals from puzzle arduino
int signalFrom;

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
int getTime() {
    currTime = millis();
    int diff = (timeLimit - (currTime - startTime)) / 1000;
    if (diff <= 0) return 0;
    return diff;
}

void printTime() {
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
    keyOpened = 0; rewardOpened = 0;
}

// explode when cut wrong wire, stop timer
void explode() {
    bombStatus = -1;

    // print Game over message on lcd
    writeRow(0, "Game Over");

    // stop timer
    timeRemaining = getTime();
    printTime();
    
    // open reward TODO

    // invoke buzzer TODO
    
}

// defuse when cut correct wire, stop timer, get reward
void defused() {
    bombStatus = 1;

    // print Victory message on lcd
    writeRow(0, "Victory!");

    // stop timer
    timeRemaining = getTime();
    printTime();

    // invoke buzzer TODO

}

// check the current timer and decide whether to end the game
void checkTimer() {
    timeRemaining = getTime();
    if (timeRemaining == 0) explode();
    else printTime();
}

// get message from puzzle arduino
/*
    1: puzzle 1 solved
    2: puzzle 2 solved
    3: puzzle 3 solved and defused
    4: puzzle 3 not solved and explode 
*/
void updateFromPuzzler() {
    if (Serial.available()) {
        signalFrom = Serial.read(); // get signals from puzzle arduino
    }
}


// check status of puzzle 3
void check3() {
    if (!solved3) {
        if (signalFrom == -1) {
            solved3 = - 1;
            explode(); // explode
        }
        else if (signalFrom == 1)  {
            solved3 = 1;
            defused(); // defuse
        }
    }
}

// check status of puzzle 2
void check2() {
    if (!solved2 && signalFrom == 2) {
        // print message that puzzle 2 is solved TODO

        // send to puzzle arduino to trigger hint for puzzle 3
        
        writeRow(0, "Puzzle 2 solved!");
        solved2 = 1;
    }
}

// check status of puzzle 1
void check1() {
    if (!solved1 && signalFrom == 1) {
        // print message that puzzle 1 is solved TODO

        // open key chest

        // send to puzzle arduino to trigger hint for puzzle 2

        writeRow(0, "Puzzle 1 solved!");
        solved1 = 1;
    }
}

void setup() {
    // set up components
    Serial.begin(9600);
    lcd.begin(16, 2);
    servoKey.attach(0);
    servoReward.attach(0);
    pinMode(buzzer, OUTPUT);

    // start the bomb
    displayIntroMessage();
    triggerBomb();
}

void loop() {
    checkTimer();   
    updateFromPuzzler();    
    check3();   
    check1();   
    check2();   
}
