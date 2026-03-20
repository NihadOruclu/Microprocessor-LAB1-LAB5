#include <Arduino.h>
#include <avr/io.h>

#define LED_PIN 13// Connecting LED with pin 13.
#define BUTTON_PIN 8// And button with pin 8.

extern "C" void modeA();// Mode A function (assembly-safe) had conflicts without this.
extern "C" void modeB();// Mode B function
extern "C" void modeC();// Mode C function

extern "C" void modeA_entry(); // Enry points for JMP
extern "C" void modeB_entry();
extern "C" void modeC_entry();

volatile uint8_t action = 0; // Current action index (0–3).

void action0() {
  digitalWrite(LED_PIN, HIGH); // LED ON.
  delay(200);// Waiting 200 ms.
  digitalWrite(LED_PIN, LOW);  // LED OFF.
  delay(200);// Waiting 200 ms.
}

void action1() {
  digitalWrite(LED_PIN, LOW);  // LED OFF.
  delay(200);// Waiting 200 ms.
  digitalWrite(LED_PIN, HIGH); // LED ON.
  delay(200);// Waiting 200 ms.
}

void action2() {
  digitalWrite(LED_PIN, HIGH); // LED ON
  delay(200);// wait 200 ms
  digitalWrite(LED_PIN, LOW);  // LED OFF
  delay(400);// Longer OFF delay.
}

void action3() {
  action = 0;// Reseting action back to 0.
}


void (*actionTable[4])() = {
  action0,// Index 0 → action0.
  action1,// Index 1 → action1.
  action2,// Index 2 → action2.
  action3// Index 3 → reset action.
};

void actionselect() {
  void (*handler)() = actionTable[action]; // Selecting function based on action.

  asm volatile(
    "movw r30,%A0\n" // Moving function address into Z register (r30:r31).
    "ijmp\n"// Jumping to address stored in Z register.
    :
    : "r"(handler)
  );
}
void waitForFirstPress() {
  asm volatile(
    "boot_wait:\n"// This will be the label for loop start.
    "sbic %[pin],0\n"// Skipping next if button is not pressed meaning HIGH.
    "rjmp boot_wait\n"// lOOPING UNTIL it becomes LOW meaning button is pressed.
    :
    : [pin] "I" (_SFR_IO_ADDR(PINB))
  );
}

int chooseMode() {
  int presses = 0;// Counting button presses.
  unsigned long start = millis();// Start time.

  while (millis() - start < 2000) { // Will run for 2 seconds.
    if (digitalRead(BUTTON_PIN) == LOW) { // If button is pressed...

      presses++;//..then increment press count.

      while (digitalRead(BUTTON_PIN) == LOW); // Waiting for release.
      delay(50);// Debounce.
    }
  }

  if (presses < 1) presses = 1;  //  Minimum mode = 1
  if (presses > 3) presses = 3;  // Maximum mode = 3

  return presses;// Returning selected mode.
}

extern "C" void modeA() {

  while (1) { // Infinite looping.
    if (digitalRead(BUTTON_PIN) == LOW) {

      action++; // Next action.
      if (action > 3) action = 0; // Wraping around.

      while (digitalRead(BUTTON_PIN) == LOW); // Waitting for release.
      delay(50);
    }

    actionselect();// Executing  action via IJMP.
  }
}

extern "C" void modeB() {

  while (1) { // Infinite looping.
    if (digitalRead(BUTTON_PIN) == LOW) {

      action++;// Changing action.
      if (action > 3) action = 0; // Wrapping around.

      while (digitalRead(BUTTON_PIN) == LOW);
      delay(50);
    }

    actionselect();// Executing action once.
    actionselect();// Executing action again wich gives double blink affect.
    delay(400); //Delaying between repetitions.
  }
}

extern "C" void modeC() {

  while (1) { // Infinite looping.
    digitalWrite(LED_PIN, HIGH); // LED ON.
    delay(40);// Short delay.
    digitalWrite(LED_PIN, LOW);  // LED OFF.
    delay(40);// Short delay, causes fast blinking affect.
  }
}

void setup() {

  pinMode(LED_PIN, OUTPUT);// Setting LED as output.
  pinMode(BUTTON_PIN, INPUT_PULLUP);// Button with pull-up.
  waitForFirstPress();//Waiting until first button gets pressed.

  digitalWrite(LED_PIN, HIGH);// Starting confirmation ON.
  delay(200);
  digitalWrite(LED_PIN, LOW);// Starting confirmation OFF.

  int mode = chooseMode();// Selecting mode from 1 to 3.

  if (mode == 1) {
    asm volatile("jmp modeA_entry"); // Jumping to Mode A.
  }

  if (mode == 2) {
    asm volatile("jmp modeB_entry"); // Jumping to Mode B.
  }

  if (mode == 3) {
    asm volatile("jmp modeC_entry"); // Jumping to Mode C.
  }

  while (1); // Stopping execution here so never return.
}


extern "C" __attribute__((used)) void modeA_entry() {
  modeA(); // Calling Mode A.
}

extern "C" __attribute__((used)) void modeB_entry() {
  modeB(); //Calling Mode B.
}

extern "C" __attribute__((used)) void modeC_entry() {
  modeC(); //Calling Mode C.
}

void loop() {} //Empty no need for using.
