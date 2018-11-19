#include <Key.h>
#include <Keypad.h>

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 2;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin
const int rttPin = 3;

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int currentRacerId = 1;
int delayTimerStart = 0;
String nextRacerId = "";

//-----------------
//set up keypad
//-----------------
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'#','0','*'}
};
// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 9, 8, 7, 6 };
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { 12, 11, 10 }; 
// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  pinMode(rttPin, INPUT);

  // initialize serial communication:
  Serial.begin(9600);
}

void loop() {
  char key = kpd.getKey();

  if(key && key!='*' && key!='#'){  // Check for a valid key.
    nextRacerId = nextRacerId + key;
    if (nextRacerId.length()>3){
      nextRacerId = nextRacerId.substring(1, 4);
    }
  }
  if(key && key =='*' || key =='#'){
    nextRacerId = "";
  }

  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  //send start signal
  if (buttonState == HIGH) {
    int header = 16; //00010000
    int Id = nextRacerId.toInt%256;
    int checkSum = calculateCheckSum(header)+calculateCheckSum(Id);
    Serial.write(header);
    Serial.write(Id);
    Serial.write(checkSum);

    //Illuminate LED to show packet has been sent
    digitalWrite(ledPin, HIGH);
    delay(1000);
    }else{
    digitalWrite(ledPin, LOW);
  }

  //send first rtt test packet
  if(digitalRead(rttPin) == HIGH){
    int header = 1; //00000001
    int checkSum = calculateCheckSum(header);
    Serial.write(header);
    Serial.write(checkSum);
    int delayTimerStart = millis();
  }

  //send rtt test result back to finish line
  if (Serial.available()>0){
    int header = 19; //00010011
    int delayTime = millis()-delayTimerStart;
    int checkSum = calculateCheckSum(header)+calculateCheckSum(delayTime);
    Serial.write(header);
    Serial.write(delayTime);
    Serial.write(checkSum);
    while(Serial.available()>0){
      Serial.read();
    }
  }
  
}

int calculateCheckSum(byte input){
  int tot = 0;
  for (int i = 0; i<8; i++){
    tot += input>>i & 1;
  }
  return tot;
}
