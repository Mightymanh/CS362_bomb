#include <LiquidCrystal.h>

// Define the LCD dimensions and global variables
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

// Initialize the LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//questions to give to player to answer
char* questionArray[5] = {"What is 1+1?    ", "What year was UIC founded 19XX?    "};
int answerTens[5] = {0, 8};
int answerOnes[5] = {2, 2};
int questionNumber = 0;

char* message;
int length;
char copy[100];
char* temp;
char* end;
char holder;

//initialize analog pins for potentiometers
const int pot1 = A1;
const int pot2 = A5;

int tens = 0;
int ones = 0;

const int submitButton = A0;
int submitState, submitStatus;

unsigned long scrollMillis = 0;
const int scrollInterval = 500;
unsigned long buttonMillis = 0;
const int buttonInterval = 25;
unsigned long shiftMillis = 0;
const int shiftInterval = 250;
unsigned long currentMillis;


//puzzle1 globals
const int led1 = 10;
const int led2 = 13;
const int button1 = 6;
const int button2 = 7;
const int button3 = 8;
const int button4 = 9;
const int interval = 25;
unsigned long prevMillis = 0;

int answerArray[4];
int correctAnswer[4] = {3,0,2,1};
int count = 0;

int button1State, button2State,
    button3State, button4State;

int buttonStatus1, buttonStatus2, 
    buttonStatus3, buttonStatus4;

int ledCycleCount = 0;
unsigned long ledTime = 0;
const long ledInterval = 1000;


void clearTop() {
    lcd.setCursor(0,0);
    lcd.print("                ");
}


void rollMessage() {
    
    int sizeDisplay;

    if (end - temp >= 16) sizeDisplay = 16;
    else sizeDisplay = end - temp;
    holder = temp[sizeDisplay];
    temp[sizeDisplay] = '\0';

    clearTop();
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
    // char testString[30];
    // snprintf(testString, 30, "tens = <%d> ones = <%d>", tens, ones);
    // Serial.println(testString);


    lcd.setCursor(0,1);
    char inputString[16];
    snprintf(inputString, 16, "Input: %d%d", tens, ones);
    lcd.print(inputString);

}

void updateGlobals() {
    memset(message, 0, sizeof(message));
    message = questionArray[questionNumber];
    length = strlen(message);
    memset(copy, 0, sizeof(copy));
    strcpy(copy, message);
    temp = copy;
    end = copy + length;
}


void validateAnswer() {
    Serial.println("Submitting!");
    if (tens == answerTens[questionNumber] &&
        ones == answerOnes[questionNumber]) {
        questionNumber++;
        updateGlobals();
        Serial.println("correct");
    } else {
        //wrong answer lose time
        Serial.println("wrong");
    }
    if (questionNumber >= 5) {
        //puzzle is solved!
        Serial.println("solved!");
    }
}


//debounces the submit button so it doesn't accidentally penalize the player
void brooklynDebounce(int buttonState, int *buttonStatus, int button) {
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
            Serial.println("Failed");
            return;
        }
    }
    //serial communicate success and open box
    Serial.println("Success");
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

    count++;
    if (index == 3) {
        checkAnswer();
    }
}



void setup() {
    // Initialize the serial port
    Serial.begin(9600);
    pinMode(pot1, INPUT);
    pinMode(pot2, INPUT);
  

    // Initialize the LCD
    lcd.begin(LCD_COLS, LCD_ROWS);
    lcd.clear();
    lcd.setCursor(0,0);
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
  // put your main code here, to run repeatedly:
    currentMillis = millis();
    if (currentMillis - scrollMillis > scrollInterval) {
        //displayQuestion();
        questionInput();
        scrollMillis = currentMillis;
    }
    if (currentMillis - buttonMillis > buttonInterval) {
        brooklynDebounce(submitState, &submitStatus, submitButton);
        buttonMillis = currentMillis;
    }
    if (currentMillis - shiftMillis > shiftInterval) {
        displayQuestion();
        //Serial.println(temp);
        shiftMillis = currentMillis;
    }
    if (currentMillis - prevMillis > interval) {
        brooklynDebounce(button1State, &buttonStatus1, button1);
        brooklynDebounce(button2State, &buttonStatus2, button2);
        brooklynDebounce(button3State, &buttonStatus3, button3);
        brooklynDebounce(button4State, &buttonStatus4, button4);
        prevMillis = currentMillis;
    }
    if (currentMillis - ledTime > ledInterval) {
        ledCycle();
        ledTime = currentMillis;
        //Serial.println(ledTime);
    }
}
