#define MIC_PIN A2 // pin number for the microphone
#define LED_PIN 6 // pin number for the LED
#define BUTTON_PIN 2 // pin number for the button


bool recording = false; // flag to indicate if recording is in progress
bool endRecording = false; // flag to indicate if recording has ended
unsigned long endRecordingTime; // time stamp for the end of recording
int micValue; // variable to store the microphone value

void setup() {
  pinMode(LED_PIN, OUTPUT); // set the LED pin as output
  pinMode(BUTTON_PIN, INPUT); // set the button pin as input
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), startRecording, RISING); // attach an interrupt to the button pin
  Serial.begin(9600);
}


// ISR to start recording
void startRecording()
{
  recording = true; // set the recording flag to true
}

void loop() {
  if (recording) {
    delay(500); // delay for 500 milliseconds so the button noise wont interfere
    digitalWrite(LED_PIN, HIGH); // turn on the LED for visual wile recording
    endRecordingTime = millis() + 5000;  // set the time stamp for the end of recording
    Serial.println("START"); // send "START" to the serial monitor
    recording = false; // reset the recording flag
    endRecording = true;  // set the endRecording flag to true
  }

  if (millis() <= endRecordingTime) {
    micValue = analogRead(MIC_PIN);  // read the microphone value
    Serial.println(micValue);        // send the microphone value to the serial monitor
  } else if (endRecording) {
    digitalWrite(LED_PIN, LOW);    // turn off the LED
    Serial.println("END");         // send "END" to the serial monitor
    endRecording = false;          // reset the endRecording flag
  }
}







