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

// puzzle variables
int solved1, solved2, solved3;
int status3; // keep track of puzzle 3 whether player cuts correct wire
int keyOpened, rewardOpened;

// display intro message scrolling down on lcd, delay each time scrolling down by 10 seconds
void displayIntroMessage() {
    

} 

// start the timer, initialize all components to 0
void triggerBomb() {

}

// explode when cut wrong wire, stop timer
void explode() {

}

// defuse when cut correct wire, stop timer, get reward
void defused() {

}

// check the current timer and decide whether to end the game
void checkTimer() {

}

// get message from puzzle arduino
void updateFromPuzzler() {

}

// check status of puzzle 3
void check3() {

}

// check status of puzzle 2
void check2() {

}

// check status of puzzle 1
void check1() {

}

void setup() {
    // set up components
    Serial.begin(9600);
    lcd.begin(16, 2);
    servoKey.attach(0);
    servoReward.attach(0);
    pinMode(buzzer, OUTPUT);

    // start up the bomb
    displayIntroMessage()
    triggerBomb();
}

void loop() {
    
    checkTimer();   
    updateFromPuzzler();    
    check3();   
    check1();   
    check2();   
}
