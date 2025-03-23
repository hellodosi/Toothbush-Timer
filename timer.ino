#include <Adafruit_NeoPixel.h>

// Constants for pin assignment and number of LEDs
const int NEOPIXEL_PIN = 2;
const int BUTTON_PIN = 6;
const int NUM_PIXELS = 16;

// Timer constants
const unsigned long TIMER_DURATION_SECONDS = 120; // Timer duration in seconds (2 minutes)
const unsigned long DEBOUNCE_DELAY_MS = 50;    // Debounce time for the button
const unsigned long LONG_PRESS_DURATION_MS = 1000; // Duration for long keystroke in milliseconds
const unsigned long BLINK_INTERVAL_MS = 500;    // Flashing interval in milliseconds
const unsigned long FINISHED_ANIMATION_DURATION_MS = 10000; // Duration of the finished animation

// state variables
enum class TimerState {
  IDLE,
  RUNNING,
  PAUSED,
  FINISHED
};

TimerState currentState = TimerState::IDLE; // Current status of the timer
unsigned long timerStartTime = 0;           // Start time of the timer
unsigned long elapsedTime = 0;
Adafruit_NeoPixel pixels(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
int activePixels = 0; //saves the number of illuminated pixels
uint32_t pausedColor = 0;
uint32_t pausedPixels[NUM_PIXELS]; // Array for saving the colors of the pixels during the pause

// Functional prototypes
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

// Initializes the pin assignment
void setupPins() {
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Activate internal pull-up resistor
}

// Initializes the NeoPixels
void initializeNeoPixels() {
  pixels.begin();
  pixels.setBrightness(32); // 12.5% Brightness
  pixels.show();
}

// Handles button presses and recognizes short/long presses
void handleButtonPress() {
  static unsigned long lastPressTime = 0;
  static bool buttonPressed = false; // Saving the status of the button

  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW) { // Button pressed
    if (!buttonPressed) { // First keystroke
      buttonPressed = true;
      lastPressTime = millis(); // Saving the time the button was pressed
    }
  } else { // Disconnected probe
    if (buttonPressed) { // If button was previously pressed
      buttonPressed = false; // Reset status
      if (millis() - lastPressTime >= LONG_PRESS_DURATION_MS) {
        // Long button press: Reset
        resetTimer();
      } else {
        // Short press: Start/Pause/Resume
        if (currentState == TimerState::IDLE) {
          startTimer();
        } else if (currentState == TimerState::RUNNING || currentState == TimerState::PAUSED) {
          //Pause or resume depending on the status
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

// Updates the timer status based on the elapsed time
void updateTimerState() {
  if (currentState == TimerState::RUNNING) {
    elapsedTime = millis() - timerStartTime;
    if (elapsedTime / 1000 >= TIMER_DURATION_SECONDS) {
      currentState = TimerState::FINISHED;
    }
  }
}

// Displays the idle pattern on the LEDs
void runIdleState() {
  static unsigned long lastBlinkTime = 0;
  static uint8_t rotationIndex = 0; // Index for the rotation

  if (millis() - lastBlinkTime >= BLINK_INTERVAL_MS) {
    lastBlinkTime = millis();
    pixels.clear();
    // Illuminates pixels 1, 5, 9 and 13 (with shift by rotationIndex)
    for (int i = 0; i < 4; i++) {
      int pixelIndex = (i * 4 + rotationIndex) % NUM_PIXELS; // Modulo for circulation
      pixels.setPixelColor(pixelIndex, pixels.Color(0, 0, 255)); // Blue
    }
    pixels.show();
    rotationIndex = (rotationIndex + 1) % NUM_PIXELS; // Increment for rotation
  }
}

// Starts the timer
void startTimer() {
  currentState = TimerState::RUNNING;
  timerStartTime = millis();
  elapsedTime = 0;
  pixels.setPixelColor(0, getTimerColor(elapsedTime)); // Make sure that the first pixel lights up
  pixels.show();
}

void pauseTimer() {
  currentState = TimerState::PAUSED;
  pausedColor = getTimerColor(elapsedTime);
  // Save the current state of the pixels
  for (int i = 0; i < NUM_PIXELS; i++) {
    pausedPixels[i] = pixels.getPixelColor(i);
  }
}

void resumeTimer() {
  currentState = TimerState::RUNNING;
  timerStartTime = millis() - elapsedTime; // Correct timerStartTime
  // Restore the state of the pixels
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, pausedPixels[i]);
  }
}

// Displays the pause pattern on the LEDs
void runPausedState() {
  static unsigned long lastBlinkTime = 0;
  static bool ledsOn = false;

  if (millis() - lastBlinkTime >= BLINK_INTERVAL_MS) {
    lastBlinkTime = millis();
    ledsOn = !ledsOn;
    if (ledsOn) {
      for (int i = 0; i < NUM_PIXELS; i++) {
        pixels.setPixelColor(i, pausedPixels[i]); // Use the saved colors
      }
    } else {
      pixels.clear();
    }
    pixels.show();
  }
}

// Displays the timer progress on the LEDs
void runTimerRunningState() {
  elapsedTime = millis() - timerStartTime;
  // Calculate the active pixel count based on the remaining 15 pixels
  activePixels = map(elapsedTime / 1000, 0, TIMER_DURATION_SECONDS, 0, NUM_PIXELS - 1);
  // Make sure that the first pixel always has the current color
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

// Displays the timer finished animation
void runTimerFinishedState() {
  unsigned long rotationStartTime = millis();
  for (unsigned long i = 0; millis() - rotationStartTime < FINISHED_ANIMATION_DURATION_MS; i++) {
    for (int j = 0; j < NUM_PIXELS; j++) {
      pixels.setPixelColor((j + i) % NUM_PIXELS, getRainbowColor(j));
    }
    pixels.show();
    delay(50);
  }
  resetTimer(); // Reset timer when animation is complete
}

// Resets the timer
void resetTimer() {
  currentState = TimerState::IDLE;
  elapsedTime = 0;
  pixels.clear();
  pixels.show();
}

// Creates a color value for the rainbow effect
uint32_t getRainbowColor(uint16_t pixelIndex) {
  const uint32_t rainbowColors[] = {
    pixels.Color(255, 0, 0, 0), // Red
    pixels.Color(255, 127, 0), // Orange
    pixels.Color(255, 255, 0), // Yellow
    pixels.Color(0, 255, 0), // Green
    pixels.Color(0, 0, 255), // Blue
    pixels.Color(75, 0, 130), // Indigo
    pixels.Color(148, 0, 211) // Purple
  };
  return rainbowColors[pixelIndex % 7];
}

uint32_t getTimerColor(unsigned long elapsed) {
  // Calculates the color gradient from red to orange to yellow to green based on the elapsed time
  float percentage = (float)elapsed / (TIMER_DURATION_SECONDS * 1000);
  if (percentage > 1.0) percentage = 1.0; // Limited to 100%

  if (percentage < 0.33) { // Red
    return pixels.Color(255, 0, 0);
  } else if (percentage < 0.66) { // Orange
    return pixels.Color(255, 127, 0);
  } else if (percentage < 0.90) { // Yellow
    return pixels.Color(255, 255, 0);
  } else { // Green
    return pixels.Color(0, 255, 0);
  }
}
