/*
  Button
*/

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 2;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin
const int rttPin = 3;

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int currentRacerId = 1;

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
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    int header = 16; //00010000
    int Id = currentRacerId%256;
    int checkSum = calculateCheckSum(header)+calculateCheckSum(Id);
    Serial.write(header);
    Serial.write(Id);
    Serial.write(checkSum);

    currentRacerId++;

    //Illuminate LED to show packet has been sent
    digitalWrite(ledPin, HIGH);
    delay(1000);
    }else{
    digitalWrite(ledPin, LOW);
  }

  if(digitalRead(rttPin) == HIGH){
    int header = 1; //00000001
    int checkSum = calculateCheckSum(header);
    Serial.write(header);
    Serial.write(checkSum);
    int delayTimerStart = millis();
  }

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
