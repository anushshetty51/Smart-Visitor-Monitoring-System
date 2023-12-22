#define BLYNK_PRINT Serial
#include <Wire.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#define TRIG_PIN_1 12
#define ECHO_PIN_1 13
#define TRIG_PIN_2 27
#define ECHO_PIN_2 26
#define BUZZER_PIN 25

#define BUTTON_PIN V1
#define IN_COUNT_PIN V3
#define OUT_COUNT_PIN V4
#define TOTAL_COUNT_PIN V2

char auth[] = "dezGP2v4uvpGIP1ul8RmvXLHh_wB2EJQ";
char ssid[] = "Realme";
char pass[] = "12345678";

BlynkTimer timer;
int incomingCount = 0;
int outgoingCount = 0;
int insideCount = 0;

int getVisitorCount(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  if (distance < 15) {
    return 1;
  } else {
    return 0;
  }
}



WidgetTerminal terminal(V10);  // Only required to dump data to the Blynk terminal

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, IPAddress(117, 236, 190, 213), 8080);

  pinMode(TRIG_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(TRIG_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

}

BLYNK_WRITE(V1) {
  // Blynk button to reset counts
  if (param.asInt() == 1) {
    resetCounts();
    Blynk.virtualWrite(V1, 0);  // Reset button state to OFF after pressed
  }
}

void loop() {
  Blynk.run();
  int newIncomingCount = getVisitorCount(TRIG_PIN_1, ECHO_PIN_1);
  int newOutgoingCount = getVisitorCount(TRIG_PIN_2, ECHO_PIN_2);
  // Update counts
  incomingCount += newIncomingCount;
  outgoingCount += newOutgoingCount;
  // Make sure outgoing count doesn't exceed total entries
  outgoingCount = min(outgoingCount, incomingCount);
  insideCount = incomingCount - outgoingCount;
  insideCount = max(insideCount, 0);
  Serial.print("Inside Venue: ");
  Serial.println(insideCount);

  if (newIncomingCount > 0 || newOutgoingCount > 0) {
 tone(BUZZER_PIN, 1000);
 delay(50);
 noTone(BUZZER_PIN);
 }

  Blynk.virtualWrite(V3, "In count: " + String(incomingCount));   // Send incoming count to Blynk
  Blynk.virtualWrite(V4, "Out count: " + String(outgoingCount));  // Send outgoing count to Blynk
  Blynk.virtualWrite(V2, "Total count: " + String(insideCount));  // Send total count to Blynk
  delay(200);
}
void resetCounts() {
  // Function to reset all counts
  incomingCount = 0;
  outgoingCount = 0;
  insideCount = 0;
}
