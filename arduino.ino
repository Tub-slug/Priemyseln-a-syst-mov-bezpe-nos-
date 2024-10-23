#include <DHT.h>

// Pin definitions
const int soundSensorPin = A0;          // Audio sensor pin
const int waterSensorPin = A1;          // Water sensor pin
const int DHTPIN = 8;                   // DHT11 sensor pin (Temperature/Humidity)
const int motionPin1 = 2;               // First motion sensor (HC-SR501)
const int motionPin2 = 3;               // Second motion sensor (HC-SR505)
const int vibrationSensorPin = 4;       // Vibration sensor pin (SW-520D)
const int buzzerPin = 9;                // Buzzer pin (using tone functions)
const int buttonAPin = 7;               // Button A for incrementing the digit
const int buttonBPin = 6;               // Button B for storing the digit

// Threshold values
const int soundThreshold = 60;          // Threshold for sound sensor
const int waterThreshold = 600;         // Threshold for water sensor

// Initialize DHT sensor
#define DHTTYPE DHT11                   // DHT11 sensor type
DHT dht(DHTPIN, DHTTYPE);

// Variables for buttons and code input
int buttonAState = 0;
int buttonBState = 0;
int lastButtonAState = 0;
int lastButtonBState = 0;
int currentNumber = 0;                  // Current number for Button A
int enteredCode[4];                     // Array to store 4 digits entered via Button B
int currentIndex = 0;
int predefinedCode[4] = {1, 2, 3, 4};   // Predefined code to compare
bool codeMatched = false;               // To track if the code matches
bool buzzerEnabled = true;              // Buzzer state flag

void setup() {
  Serial.begin(9600);                   // Initialize serial communication

  // Pin modes for sensors, buttons, and buzzer
  pinMode(motionPin1, INPUT);
  pinMode(motionPin2, INPUT);
  pinMode(vibrationSensorPin, INPUT);
  pinMode(waterSensorPin, INPUT);
  pinMode(buttonAPin, INPUT);
  pinMode(buttonBPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  // Initialize DHT sensor
  dht.begin();
  

}

void loop() {
  // Sound Sensor
  int soundValue = analogRead(soundSensorPin);
  
  // DHT11 Temperature and Humidity Sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // Check for valid temperature/humidity readings
  if (isnan(temperature) || isnan(humidity)) {
    return;
  }

  // Water Sensor
  int waterValue = analogRead(waterSensorPin);
  
  // Motion Sensors (2 infrared motion sensors)
  int motionState1 = digitalRead(motionPin1);   // First motion sensor
  int motionState2 = digitalRead(motionPin2);   // Second motion sensor

  // Vibration Sensor
  int vibrationState = digitalRead(vibrationSensorPin);


  buttonAState = digitalRead(buttonAPin);
  if (buttonAState == HIGH && lastButtonAState == LOW) {
    currentNumber = (currentNumber + 1) % 10;  

    delay(200);  // Simple debounce
  }
  lastButtonAState = buttonAState;

 
  buttonBState = digitalRead(buttonBPin);
  if (buttonBState == HIGH && lastButtonBState == LOW) {
    enteredCode[currentIndex] = currentNumber;  

    
    currentIndex++; 
    if (currentIndex >= 4) {
      currentIndex = 0;  
      codeMatched = checkCode();  
      if (codeMatched) {
        buzzerEnabled = !buzzerEnabled;  

        noTone(buzzerPin);      
      } else {
        
      }
    }
    delay(200);  // Simple debounce
  }
  lastButtonBState = buttonBState;

  
  if (buzzerEnabled && (vibrationState == 0 || waterValue > waterThreshold || motionState1 == HIGH || motionState2 == HIGH || temperature>40)) {
    tone(buzzerPin, 1000);  
  } else {
    noTone(buzzerPin);      
  }


  Serial.print(soundValue);
  Serial.print(",");
  Serial.print(temperature);
  Serial.print(",");
  Serial.print(humidity);
  Serial.print(",");
  Serial.print(waterValue);
  Serial.print(",");
  Serial.print(motionState1);
  Serial.print(",");
  Serial.print(motionState2);
  Serial.print(",");
  Serial.print(vibrationState);
  Serial.print(",");
  Serial.print(codeMatched ? "Code Matched" : "Code Not Matched");
  Serial.print(",");
  Serial.print(currentNumber);
  Serial.print(",");
  Serial.print(currentIndex);
  Serial.println();
  
  delay(1000);  // 1-second delay between readings
}

bool checkCode() {
  for (int i = 0; i < 4; i++) {
    if (enteredCode[i] != predefinedCode[i]) {
      return false;  
    }
  }
  return true; 
}
