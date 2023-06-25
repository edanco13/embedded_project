#define MIC_PIN A2
#define LED_PIN 6
#define BUTTON_PIN 2

bool recording = false;
unsigned long endRecordingTime;
int micValue;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), startRecording, RISING);
  Serial.begin(115200);
}

void startRecording()
{
  recording = true;
}

void loop() {
  // put your main code here, to run repeatedly:
  if(recording){
    digitalWrite(LED_PIN, HIGH);
    endRecordingTime = millis() + 5000;
    recording = false;
  }
  if(millis() <= endRecordingTime)
  {
    micValue = analogRead(MIC_PIN);
    Serial.print(micValue);
    Serial.print(",");

  }
  else digitalWrite(LED_PIN, LOW);

}
