#include <Adafruit_NeoPixel.h>

// Konstanten für Pinbelegung und LED-Anzahl
const int NEOPIXEL_PIN = 2;
const int BUTTON_PIN = 6;
const int NUM_PIXELS = 16;

// Timer-Konstanten
const unsigned long TIMER_DURATION_SECONDS = 120; // Timerdauer in Sekunden (2 Minuten)
const unsigned long DEBOUNCE_DELAY_MS = 50;    // Entprellzeit für den Taster
const unsigned long LONG_PRESS_DURATION_MS = 1000; // Dauer für langen Tastendruck in Millisekunden
const unsigned long BLINK_INTERVAL_MS = 500;    // Blinkintervall in Millisekunden
const unsigned long FINISHED_ANIMATION_DURATION_MS = 10000; // Dauer der Finished-Animation

// Zustandsvariablen
enum class TimerState {
  IDLE,
  RUNNING,
  PAUSED,
  FINISHED
};

TimerState currentState = TimerState::IDLE; // Aktueller Zustand des Timers
unsigned long timerStartTime = 0;           // Startzeit des Timers
unsigned long elapsedTime = 0;
Adafruit_NeoPixel pixels(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
int activePixels = 0; //speichert die Anzahl der leuchtenden Pixel
uint32_t pausedColor = 0;
uint32_t pausedPixels[NUM_PIXELS]; // Array zum Speichern der Farben der Pixel während der Pause

// Funktionsprototypen
void setupPins();
void initializeNeoPixels();
void handleButtonPress();
void updateTimerState();
void runIdleState();
void startTimer();
void pauseTimer();
void resumeTimer();
void runPausedState();
void runTimerRunningState();
void runTimerFinishedState();
void resetTimer();
uint32_t getRainbowColor(uint16_t pixelIndex);
uint32_t getTimerColor(unsigned long elapsed);

void setup() {
  setupPins();
  initializeNeoPixels();
}

void loop() {
  handleButtonPress();
  updateTimerState();

  switch (currentState) {
    case TimerState::IDLE:
      runIdleState();
      break;
    case TimerState::RUNNING:
      runTimerRunningState();
      break;
    case TimerState::PAUSED:
      runPausedState();
      break;
    case TimerState::FINISHED:
      runTimerFinishedState();
      break;
  }
}

// Initialisiert die Pinbelegung
void setupPins() {
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Internen Pull-Up-Widerstand aktivieren
}

// Initialisiert die NeoPixels
void initializeNeoPixels() {
  pixels.begin();
  pixels.setBrightness(32); // 12.5% Helligkeit
  pixels.show();
}

// Behandelt Tastendrücke und erkennt kurze/lange Drücke
void handleButtonPress() {
  static unsigned long lastPressTime = 0;
  static bool buttonPressed = false; // Zustand des Tasters speichern

  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW) { // Taster gedrückt
    if (!buttonPressed) { // Erster Tastendruck
      buttonPressed = true;
      lastPressTime = millis(); // Zeit des Tastendrucks speichern
    }
  } else { // Taster losgelassen
    if (buttonPressed) { // Wenn Taster vorher gedrückt war
      buttonPressed = false; // Zustand zurücksetzen
      if (millis() - lastPressTime >= LONG_PRESS_DURATION_MS) {
        // Langer Tastendruck: Reset
        resetTimer();
      } else {
        // Kurzer Tastendruck: Start/Pause/Resume
        if (currentState == TimerState::IDLE) {
          startTimer();
        } else if (currentState == TimerState::RUNNING || currentState == TimerState::PAUSED) {
          //Pause oder Resume je nach Zustand
          if (currentState == TimerState::RUNNING) {
            pauseTimer();
          } else {
            resumeTimer();
          }
        }
      }
    }
  }
}

// Aktualisiert den Timer-Zustand basierend auf der verstrichenen Zeit
void updateTimerState() {
  if (currentState == TimerState::RUNNING) {
    elapsedTime = millis() - timerStartTime;
    if (elapsedTime / 1000 >= TIMER_DURATION_SECONDS) {
      currentState = TimerState::FINISHED;
    }
  }
}

// Zeigt das Idle-Muster auf den LEDs an
void runIdleState() {
  static unsigned long lastBlinkTime = 0;
  static uint8_t rotationIndex = 0; // Index für die Drehung

  if (millis() - lastBlinkTime >= BLINK_INTERVAL_MS) {
    lastBlinkTime = millis();
    pixels.clear();
    // Leuchtet die Pixel 1, 5, 9 und 13 (mit Verschiebung durch rotationIndex)
    for (int i = 0; i < 4; i++) {
      int pixelIndex = (i * 4 + rotationIndex) % NUM_PIXELS; // Modulo für Umlauf
      pixels.setPixelColor(pixelIndex, pixels.Color(0, 0, 255)); // Blau
    }
    pixels.show();
    rotationIndex = (rotationIndex + 1) % NUM_PIXELS; // Inkrementiere für Drehung
  }
}

// Startet den Timer
void startTimer() {
  currentState = TimerState::RUNNING;
  timerStartTime = millis();
  elapsedTime = 0;
  pixels.setPixelColor(0, getTimerColor(elapsedTime)); // Stelle sicher, dass der erste Pixel leuchtet
  pixels.show();
}

void pauseTimer() {
  currentState = TimerState::PAUSED;
  pausedColor = getTimerColor(elapsedTime);
  // Speichere den aktuellen Zustand der Pixel
  for (int i = 0; i < NUM_PIXELS; i++) {
    pausedPixels[i] = pixels.getPixelColor(i);
  }
}

void resumeTimer() {
  currentState = TimerState::RUNNING;
  timerStartTime = millis() - elapsedTime; // Korrigiere timerStartTime
  // Stelle den Zustand der Pixel wieder her
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, pausedPixels[i]);
  }
}

// Zeigt das Pausenmuster auf den LEDs an
void runPausedState() {
  static unsigned long lastBlinkTime = 0;
  static bool ledsOn = false;

  if (millis() - lastBlinkTime >= BLINK_INTERVAL_MS) {
    lastBlinkTime = millis();
    ledsOn = !ledsOn;
    if (ledsOn) {
      for (int i = 0; i < NUM_PIXELS; i++) {
        pixels.setPixelColor(i, pausedPixels[i]); // Verwende die gespeicherten Farben
      }
    } else {
      pixels.clear();
    }
    pixels.show();
  }
}

// Zeigt den Timer-Fortschritt auf den LEDs an
void runTimerRunningState() {
  elapsedTime = millis() - timerStartTime;
  // Berechne die aktive Pixelanzahl basierend auf den verbleibenden 15 Pixeln
  activePixels = map(elapsedTime / 1000, 0, TIMER_DURATION_SECONDS, 0, NUM_PIXELS - 1);
  // Stelle sicher, dass der erste Pixel immer die aktuelle Farbe hat
  pixels.setPixelColor(0, getTimerColor(elapsedTime));
  for (int i = 1; i <= activePixels; i++) {
    pixels.setPixelColor(i, getTimerColor(elapsedTime));
  }
  // Clear any extra pixels
  for (int i = activePixels + 1; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
}

// Zeigt die Timer-Finished-Animation an
void runTimerFinishedState() {
  unsigned long rotationStartTime = millis();
  for (unsigned long i = 0; millis() - rotationStartTime < FINISHED_ANIMATION_DURATION_MS; i++) {
    for (int j = 0; j < NUM_PIXELS; j++) {
      pixels.setPixelColor((j + i) % NUM_PIXELS, getRainbowColor(j));
    }
    pixels.show();
    delay(50);
  }
  resetTimer(); // Timer zurücksetzen, wenn Animation abgeschlossen ist
}

// Setzt den Timer zurück
void resetTimer() {
  currentState = TimerState::IDLE;
  elapsedTime = 0;
  pixels.clear();
  pixels.show();
}

// Erzeugt einen Farbwert für den Regenbogeneffekt
uint32_t getRainbowColor(uint16_t pixelIndex) {
  const uint32_t rainbowColors[] = {
    pixels.Color(255, 0, 0),    // Rot
    pixels.Color(255, 127, 0),  // Orange
    pixels.Color(255, 255, 0),  // Gelb
    pixels.Color(0, 255, 0),    // Grün
    pixels.Color(0, 0, 255),    // Blau
    pixels.Color(75, 0, 130),   // Indigo
    pixels.Color(148, 0, 211)   // Violett
  };
  return rainbowColors[pixelIndex % 7];
}

uint32_t getTimerColor(unsigned long elapsed) {
  // Berechnet den Farbverlauf von Rot nach Orange nach Gelb nach Grün basierend auf der verstrichenen Zeit
  float percentage = (float)elapsed / (TIMER_DURATION_SECONDS * 1000);
  if (percentage > 1.0) percentage = 1.0; // Begrenzt auf 100%

  if (percentage < 0.33) { // Rot
    return pixels.Color(255, 0, 0);
  } else if (percentage < 0.66) { // Orange
    return pixels.Color(255, 127, 0);
  } else if (percentage < 0.90) { // Gelb
    return pixels.Color(255, 255, 0);
  } else { // Grün
    return pixels.Color(0, 255, 0);
  }
}
