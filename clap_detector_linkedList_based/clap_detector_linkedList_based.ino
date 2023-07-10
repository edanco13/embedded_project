
#define LED_PIN 6
#define MIC_PIN A2

struct Node {
  int data;
  Node* next;
};

Node* head = NULL;
Node* tail = NULL;
Node* firstNodeOfShortSum = NULL;
long longSum;
long shortSum;
unsigned long inClap = 0;
unsigned long waitingForMoreClaps = 0;
int maxShort;
int currentclaps;

void deleteFirstNode() {
  if (head != NULL) {
    Node* temp = head;
    head = head->next;
    delete temp;
  }
}

void addNodeToEnd(int newData) {
  Node* newNode = new Node();
  newNode->data = newData;
  newNode->next = NULL;

  if (head == NULL) {
    head = newNode;
    tail = newNode;
    }
    else {
    tail->next = newNode;
    tail = newNode;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);

  longSum = 77*256;
  shortSum = 77*32;
  maxShort = 77;
  for (int i = 0; i < 256; i++) {
    addNodeToEnd(77);
  }

  int nodePosition = 256 - 32;
  Node* temp = head;
  for (int i = 1; i < nodePosition; i++) {
    temp = temp->next;
  }
  firstNodeOfShortSum = temp;
}

bool clapDetector(){
  int micValue = analogRead(MIC_PIN);
  shortSum += micValue-firstNodeOfShortSum->data;
  longSum += micValue-head->data;

  maxShort =  max(maxShort, micValue);

  if(maxShort == firstNodeOfShortSum->data){
    maxShort = (shortSum >> 5);
  }

  firstNodeOfShortSum = firstNodeOfShortSum->next;
  deleteFirstNode();
  addNodeToEnd(micValue);

  float clapLikeness = float(shortSum >> 5)/float(longSum >> 8);
 

  if ((inClap <= millis()) && (clapLikeness > 1.5) && (maxShort > 600)) {
    inClap = millis() + 250;
    maxShort = (shortSum >> 5);
    Serial.println(micValue);
    return true;
  }
  return false;
  
}


void loop() {
  // put your main code here, to run repeatedly:
  bool didClaped = clapDetector();
  if(didClaped)
  {
    if(currentclaps == 0){
      Serial.println(currentclaps);
      waitingForMoreClaps = millis() + 1000;
    }
    else{
      waitingForMoreClaps + 500;
    }
    currentclaps++;
    Serial.println(currentclaps);
  }


  if(waitingForMoreClaps <= millis()){
    switch(currentclaps){
      case 1: digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        break;
      case 0:
        break;
      default:
        Serial.println(currentclaps);
    }
    currentclaps = 0;
  }
}
