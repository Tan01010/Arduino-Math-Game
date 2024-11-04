#include <U8glib.h>
#include <TM1637Display.h>
#include <Keypad.h>

#define CLK 10
#define DIO 11
TM1637Display display7Segment(CLK, DIO);

const byte ROWS = 4; 
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {' ', '3', '2', '1'},
  {'-', '6', '5', '4'},
  {'C', '9', '8', '7'},
  {'S', ' ', '0', ' '}
};
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE); // Initialize U8glib for the OLED

int score = 0;
int currentAnswer = 0;
String userAnswer = ""; // Change to String to accumulate input

const int greenLEDPin = 12;
const int redLEDPin = 13;

void setup() {
  Serial.begin(9600);
  display7Segment.setBrightness(0x0f); 
  display7Segment.showNumberDec(score);
  
  pinMode(greenLEDPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);
  
  generateQuestion();
}

void loop() {
  char key = keypad.getKey();
  
  if (key) {
    Serial.print(key);
    if (key >= '0' && key <= '9') {
      userAnswer += key; // Append the pressed key to the userAnswer string
      Serial.print(userAnswer);
      displayAnswer(userAnswer.toInt()); // Convert to int for display
    } else if (key == 'S') { 
      checkAnswer();
      userAnswer = ""; // Reset the userAnswer
      generateQuestion();
    } else if (key == 'C') { 
      userAnswer = ""; // Reset the userAnswer
      displayAnswer(0); // Display 0 when cleared
    } else if (key == '-') { // Handle the negative button
      // Negate the current input if the first character is not already negative
      if (userAnswer.length() > 0 && userAnswer.charAt(0) != '-') {
        userAnswer = "-" + userAnswer; // Add negative sign
      }
      displayAnswer(userAnswer.toInt()); // Update the display
    }
  }
}


void generateQuestion() {
  int num1 = random(1, 1000);
  int num2 = random(1, 1000);
  int operation = random(3); 

  switch (operation) {
    case 0:
      currentAnswer = num1 + num2;
      displayQuestion(num1, '+', num2);
      break;
    case 1:
      currentAnswer = num1 - num2;
      displayQuestion(num1, '-', num2);
      break;
    case 2:
      currentAnswer = num1 * num2;
      displayQuestion(num1, '*', num2);
      break;
  }
}

void displayQuestion(int num1, char op, int num2) {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_6x10); // Set font size
    u8g.setPrintPos(0, 15);
    u8g.print("Q: ");
    u8g.print(num1);
    u8g.print(" ");
    u8g.print(op);
    u8g.print(" ");
    u8g.print(num2);
  } while (u8g.nextPage());
}

void displayAnswer(int answer) {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_6x10);
    u8g.setPrintPos(0, 40);
    u8g.print("A: ");
    u8g.print(answer);
  } while (u8g.nextPage());
}

void checkAnswer() {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_6x10);
    // Compare userAnswer as integer only once
    if (userAnswer.toInt() == currentAnswer) {
      score++; // Increment score by 1 if the answer is correct
      digitalWrite(greenLEDPin, HIGH);
      delay(500);
      digitalWrite(greenLEDPin, LOW);
      u8g.setPrintPos(0, 15);
      u8g.print("Correct!");
    } else {
      digitalWrite(redLEDPin, HIGH);
      delay(500);
      digitalWrite(redLEDPin, LOW);
      u8g.setPrintPos(0, 15);
      u8g.print("Wrong!");
    }
  } while (u8g.nextPage());
  
  // Reset userAnswer after checking
  userAnswer = ""; // Clear userAnswer to avoid repeated checks
  display7Segment.showNumberDec(score);
  delay(1000);
}

