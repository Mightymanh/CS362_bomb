#include <LiquidCrystal.h>


//puzzle1 globals
const int led1 = 10;
const int led2 = 13;
const int button1 = 6;
const int button2 = 7;
const int button3 = 8;
const int button4 = 9;
const int interval = 25;
unsigned long prevMillis = 0;

//makes an answer array for the user to fill and checks against the correct answer array
int answerArray[4];
int correctAnswer[4] = {3,0,2,1};
int count = 0;

//button variables to debounce with so it works as a toggle
int button1State, button2State,
    button3State, button4State;

int buttonStatus1, buttonStatus2, 
    buttonStatus3, buttonStatus4;

//interval for leds to cycle
int ledCycleCount = 0;
unsigned long ledTime = 0;
const long ledInterval = 1000;





//puzzle2 globals
// Define the LCD dimensions and global variables
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

// Initialize the LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//questions to give to player to answer
char* questionArray[5] = {
"What is 1+1?    ", 
"What year was UIC founded 19XX?    ",
"Two years ago, I was three times as old as my brother was. In three years time I will be twice as old as my brother.” How old is the younger brother in three years?",
"My twin lives at the reverse of my house number. The difference between our house numbers ends in two. What is the lower of the two house numbers?",
"What is the smallest whole number that is equal to seven times the sum of its digits?"};
int answerTens[5] = {0, 8, 1, 1, 2};
int answerOnes[5] = {2, 2, 0, 9, 1};
int questionNumber = 0;

//globals to help manage the text scrolling
char* message;
int length;
char copy[500];
char* temp;
char* end;
char holder;

//initialize analog pins for potentiometers
const int pot1 = A1;
const int pot2 = A5;

//potentiometer inputs 
int tens = 0;
int ones = 0;

//submit button for puzzle2
const int submitButton = A0;
int submitState, submitStatus;

//various timings for certain events from puzzle2 to occur
unsigned long scrollMillis = 0;
const int scrollInterval = 500;
unsigned long buttonMillis = 0;
const int buttonInterval = 25;
unsigned long shiftMillis = 0;
const int shiftInterval = 250;
unsigned long currentMillis = 0;

// puzzle 3
int brown = 13, white = A3, black = A2, green = 10; 
int valBrown, valWhite, valBlack, valGreen;

// STATE, TIME, ARRAY VARIABLES
char signalReceived, signalSent;
int solved1, solved2, solved3; // 0 means not solved, 1 means solved.
int trigger1, trigger2, trigger3;
int triggerHint3;
int gameStatus; // 1 means games on going, 0 means game finished

// DEBUG variable
int DEBUG = 0; 


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

// reset the puzzles
void freshStart() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hi players!");
    solved1 = 0; solved2 = 0; solved3 = 0;
    trigger1 = 0; trigger2 = 0; trigger3 = 0;
    triggerHint3 = 0;
    gameStatus = 0;
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
        if (DEBUG) {
            char output[100];
            sprintf(output, "Signal R: %c", signalReceived);
            writeRow(0, output);
            //Serial.print("Signal received: "); Serial.println(signalReceived);
        }

        if (signalReceived == '1') {
            if (!trigger1 || !trigger3) {
                if (DEBUG) {
                    writeRow(0, "start game");
                    //Serial.println("start the game: trigger puzzle 1 and 3");
                }
                gameStatus = 1;
                trigger1 = 1;   // trigger puzzle 1
                trigger3 = 1;   // trigger puzzle 3
            }
        } 
        else if (signalReceived == '2') {
            if (!trigger2 && solved1) {
                if (DEBUG) {
                    writeRow(0, "trigger 2");
                    //Serial.println("trigger puzzle 2");
                }
                trigger2 = 1; // trigger puzzle 2
            }
        }
        else if (signalReceived == '3') {
            if (!trigger3 && solved2) {
                if (DEBUG) {
                    writeRow(0, "trigger hint 3");
                    // Serial.println("trigger hint for puzzle 3");
                }
                triggerHint3 = 1; // trigger hint for puzzle 3 (puzzle 3 always run from the start)
            }
        }
        else if (signalReceived == '4') {
            if (DEBUG) {
                writeRow(0, "trigger reset");
                //Serial.println("trigger reset");
            }
            freshStart();
        }
    }
}

//iterates the character one at a time per call of this function to scroll the text accross the top line of the lcd
void rollMessage() {
    
    int sizeDisplay;

    if (end - temp >= 16) sizeDisplay = 16;
    else sizeDisplay = end - temp;
    holder = temp[sizeDisplay];
    temp[sizeDisplay] = '\0';

    clearRow(0);
    lcd.setCursor(0,0);
    lcd.print(temp);

    temp[sizeDisplay] = holder;
    temp++;
}


//shows the question from the array depending on what question number the player is on
void displayQuestion() {
    
    if (temp != end) {
        rollMessage();
    } else {
        temp = copy;
        rollMessage();
    }

}


//displays the input of the players turning of the potentiometers
void questionInput() {
    int POTValue1 = analogRead(pot1);
    int POTValue2 = analogRead(pot2);

    tens = map(POTValue1,0,1000,0,9);
    ones = map(POTValue2,0,1000,0,9);

    lcd.setCursor(0,1);
    char inputString[16];
    snprintf(inputString, 16, "Input: %d%d", tens, ones);
    lcd.print(inputString);
}

//used to make sure that the text is properly aligned when the question changes
void updateGlobals() {
    memset(message, 0, sizeof(message));
    message = questionArray[questionNumber];
    length = strlen(message);
    memset(copy, 0, sizeof(copy));
    strcpy(copy, message);
    temp = copy;
    end = copy + length;
}

//checks the tens and ones to ensure that it is of the correct number when 
void validateAnswer() {
    Serial.println("Submitting!");
    if (tens == answerTens[questionNumber] &&
        ones == answerOnes[questionNumber]) {
        questionNumber++;
        if (questionNumber < 5) {
            updateGlobals();
        }
        Serial.println("correct");
    } else {
        //wrong answer lose time
        Serial.println("wrong");
    }
    if (questionNumber >= 5) {
        //puzzle is solved!
        solved2 = 1;
        Serial.println("solved!");
        lcd.clear();
        writeRow(0, "Hint: 25225");
        writeRow(1, "Use your phone!");
    }
}


//debounces the submit button so it doesn't accidentally penalize the player
void brooklynDebouncePuzzle2(int buttonState, int *buttonStatus, int button) {
    buttonState = digitalRead(button);

    if (buttonState != *buttonStatus) {
        if (buttonState == HIGH) {
            validateAnswer();
        }
    }
    *buttonStatus = buttonState;
}



void ledCycle () {

    int cycleNum = ledCycleCount % 6;

    //int correctAnswer[4] = {3,0,1,2};
    if (cycleNum == 0) {

        //display 3
        digitalWrite(led1, HIGH);
        digitalWrite(led2, HIGH);
    } else if (cycleNum == 1) {

        //display 0
        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
    } else if (cycleNum == 2) {

        //display 1
        digitalWrite(led1, HIGH);
        digitalWrite(led2, LOW);
    } else if (cycleNum == 3) {

        //display 2
        digitalWrite(led1, LOW);
        digitalWrite(led2, HIGH);
    } else if (cycleNum == 4) {
        
        // empty to reset and not confuse player
        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
    } else if (cycleNum == 5) {
        
        //empty to reset and not confuse player
        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
    }
    //Serial.println(cycleNum);
    ledCycleCount++;
}

void checkAnswer () {
    for (int i = 0; i < 4; i++) {
        if (answerArray[i] == correctAnswer[i]) {
            continue;
        } else {
            //serial communicate failure
            writeRow(1, "P1 Failed");
            memset(answerArray, -1, sizeof(answerArray));
            return;
        }
    }
    //serial communicate success and open box
    solved1 = 1;
    writeRow(1, "P1 solved");
    Serial.write('1'); // send to main arduino to indicate puzzle 1 is solved
    return;
}

void checkButtonInput (int button) {
    int index = count % 4;

    switch (button) {
        case 6:
            answerArray[index] = 0;
            break;
        case 7:
            answerArray[index] = 1;
            break;
        case 8:
            answerArray[index] = 2;
            break;
        case 9:
            answerArray[index] = 3;
            break;
    }
    char currentInput[20];
    sprintf(currentInput, "[%d,%d,%d,%d]", answerArray[0], answerArray[1], answerArray[2], answerArray[3]);
    writeRow(0, currentInput);
    count++;
    if (index == 3) {
        checkAnswer();
    }
}


//debounces the submit button so it doesn't accidentally penalize the player
void brooklynDebouncePuzzle1(int buttonState, int *buttonStatus, int button) {
    buttonState = digitalRead(button);

    if (buttonState != *buttonStatus) {
        if (buttonState == HIGH) {
            checkButtonInput(button);
        }
    }
    *buttonStatus = buttonState;
}

// PUZZLE FUNCTIONS
void puzzle1() {
    if (currentMillis - prevMillis > interval) {
        brooklynDebouncePuzzle1(button1State, &buttonStatus1, button1);
        brooklynDebouncePuzzle1(button2State, &buttonStatus2, button2);
        brooklynDebouncePuzzle1(button3State, &buttonStatus3, button3);
        brooklynDebouncePuzzle1(button4State, &buttonStatus4, button4);
        prevMillis = currentMillis;
    }
    if (currentMillis - ledTime > ledInterval) {
        ledCycle();
        ledTime = currentMillis;
        //Serial.println(ledTime);
    }
}

void puzzle2() {
    if (currentMillis - scrollMillis > scrollInterval) {
        //displayQuestion();
        questionInput();
        scrollMillis = currentMillis;
    }
    if (currentMillis - buttonMillis > buttonInterval) {
        brooklynDebouncePuzzle2(submitState, &submitStatus, submitButton);
        buttonMillis = currentMillis;
    }
    if (currentMillis - shiftMillis > shiftInterval) {
        displayQuestion();
        //Serial.println(temp);
        shiftMillis = currentMillis;
    }
}

void puzzle3() {
    valBrown = digitalRead(brown);
    valWhite = digitalRead(white);
    valBlack = digitalRead(black);
    valGreen = digitalRead(green);
    if (DEBUG) {
        char output[100];
        sprintf(output, "br%d wh%d bl%d gr%d", valBrown, valWhite, valBlack, valGreen);
        //Serial.println(output);
        writeRow(1, output);
    }
    if (valBrown == 1 && valWhite == 1 && valBlack == 1 && valGreen == 1) {
        //if (DEBUG) lcd.print("bomb ticking");
    }
    else if (valBrown == 0 || valWhite == 0 || valGreen == 0) {
        lcd.clear();
        writeRow(0, "Game over!");


        Serial.write('4');
        gameStatus = 0;
    }
    else if (valBlack == 0) {

        lcd.clear();
        writeRow(0, "Congratulations");
        //Serial.println("correct!");

        Serial.write('3');
        solved3 = 1;
        gameStatus = 0;
    }    
}

void setup() {
    Serial.begin(9600);
    // // puzzle 1
    // pinMode(led1, OUTPUT); pinMode(led2, OUTPUT);
    // pinMode(b1, INPUT); pinMode(b2, INPUT); pinMode(b3, INPUT); pinMode(b4, INPUT);
    // // puzzle 2
    lcd.begin(16, 2);
    // pinMode(pot1, INPUT); pinMode(pot2, INPUT);
    // pinMode(confirmButton, INPUT);
    // puzzle 3
    pinMode(brown, INPUT); pinMode(white, INPUT); pinMode(black, INPUT); pinMode(green, INPUT);

    // fresh start the puzzle Arduino's state variables
    freshStart();
    // Initialize the serial port
    Serial.begin(9600);
    pinMode(pot1, INPUT);
    pinMode(pot2, INPUT);
  

    // Initialize the LCD
    lcd.setCursor(0,0);
    pinMode(submitButton, INPUT);
    submitStatus = digitalRead(submitButton);

    updateGlobals();

    //puzzle1 pins
    pinMode(button1, INPUT);
    pinMode(button2, INPUT);
    pinMode(button3, INPUT);
    pinMode(button4, INPUT);

    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);

    Serial.begin(9600);
    buttonStatus1 = digitalRead(button1);
    buttonStatus2 = digitalRead(button2);
    buttonStatus3 = digitalRead(button3);
    buttonStatus4 = digitalRead(button4);
}

void loop() {
    //ledCycle();
    currentMillis = millis();
    updateFromMain();
    //Serial.print("Game status "); Serial.println(gameStatus);
    if (gameStatus) {
        if (trigger1 && !solved1) puzzle1();
        if (trigger2 && !solved2) puzzle2();
        if (trigger3 && !solved3) puzzle3();
    }
}
