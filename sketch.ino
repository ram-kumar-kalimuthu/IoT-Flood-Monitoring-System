// ============================================================
//  IoT Flood Monitoring & Early Warning System
//  Hardware : ESP32 + HC-SR04 + 3 LEDs + Active Buzzer
//  Simulator: Wokwi
// ============================================================

#define TRIG_PIN     5
#define ECHO_PIN    18
#define LED_GREEN   25   // LOW  risk
#define LED_YELLOW  26   // MEDIUM risk
#define LED_RED     27   // HIGH risk
#define BUZZER_PIN  14

#define MAX_DEPTH_CM  100.0   // full-tank reference depth (cm)
#define THRESH_LOW    40.0    // below this → LOW
#define THRESH_MED    70.0    // between LOW and this → MEDIUM
                               // above this → HIGH

// ── buzzer non-blocking state ──────────────────────────────
unsigned long buzzerPrev    = 0;
bool          buzzerState   = false;
unsigned int  buzzerOnTime  = 0;
unsigned int  buzzerOffTime = 0;
bool          buzzerActive  = false;

// ── helpers ────────────────────────────────────────────────
float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000UL); // 30 ms timeout
  if (duration == 0) return -1;                      // out-of-range guard
  return (duration * 0.0343) / 2.0;                 // cm
}

float distanceToLevel(float dist) {
  if (dist < 0) return -1;
  float level = ((MAX_DEPTH_CM - dist) / MAX_DEPTH_CM) * 100.0;
  return constrain(level, 0.0, 100.0);
}

void setLEDs(bool g, bool y, bool r) {
  digitalWrite(LED_GREEN,  g ? HIGH : LOW);
  digitalWrite(LED_YELLOW, y ? HIGH : LOW);
  digitalWrite(LED_RED,    r ? HIGH : LOW);
}

void setBuzzer(unsigned int onMs, unsigned int offMs) {
  buzzerOnTime  = onMs;
  buzzerOffTime = offMs;
  buzzerActive  = (onMs > 0);
  if (!buzzerActive) {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerState = false;
  }
}

void updateBuzzer() {
  if (!buzzerActive) return;
  unsigned long now = millis();
  unsigned int  interval = buzzerState ? buzzerOnTime : buzzerOffTime;
  if (now - buzzerPrev >= interval) {
    buzzerPrev  = now;
    buzzerState = !buzzerState;
    digitalWrite(BUZZER_PIN, buzzerState ? HIGH : LOW);
  }
}

// ── setup ──────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN,    OUTPUT);
  pinMode(ECHO_PIN,    INPUT);
  pinMode(LED_GREEN,   OUTPUT);
  pinMode(LED_YELLOW,  OUTPUT);
  pinMode(LED_RED,     OUTPUT);
  pinMode(BUZZER_PIN,  OUTPUT);

  Serial.println("=========================================");
  Serial.println("  IoT Flood Monitoring System — READY   ");
  Serial.println("=========================================");
}

// ── main loop ──────────────────────────────────────────────
void loop() {
  float dist  = measureDistance();
  float level = distanceToLevel(dist);

  // ── classify risk ──────────────────────────────────────
  String risk;
  if (dist < 0 || level < 0) {
    risk = "SENSOR ERROR";
    setLEDs(false, false, false);
    setBuzzer(0, 0);
  }
  else if (level < THRESH_LOW) {
    risk = "LOW   ";
    setLEDs(true, false, false);
    setBuzzer(0, 0);                 // silent
  }
  else if (level < THRESH_MED) {
    risk = "MEDIUM";
    setLEDs(false, true, false);
    setBuzzer(500, 500);             // slow beep
  }
  else {
    risk = "HIGH  ";
    setLEDs(false, false, true);
    setBuzzer(100, 100);             // rapid beep
  }

  // ── serial output ──────────────────────────────────────
  String trimmedRisk = risk;
  trimmedRisk.trim();
  Serial.print("{\"distance\":");
  Serial.print(dist, 1);
  Serial.print(",\"level\":");
  Serial.print(level, 1);
  Serial.print(",\"risk\":\"");
  Serial.print(trimmedRisk);
  Serial.println("\"}");

  // ── non-blocking buzzer tick every iteration ───────────
  unsigned long loopEnd = millis() + 500;
  while (millis() < loopEnd) {
    updateBuzzer();
    delay(10);
  }
}
