const int pin = 13;      // Defining pin 13 for LED output (built-in LED on Arduino Uno).

void setup() { //Initialization part. For other codes I wont be including basic information as it is self explantory.
  pinMode(pin, OUTPUT);   // Setting pin 13 as OUTPUT → allows us to control the LED state.
}

void loop() { //Loop part. 
  digitalWrite(pin, HIGH); // Setting pin 13 HIGH → LED turns ON.
  delay(500);              // Waits for 500 milliseconds (half a second)

  digitalWrite(pin, LOW);  // Set pin 13 LOW → LED turns OFF.
  delay(500);              // Waits for 500 milliseconds.
  // 500 ms ON + 500 ms OFF → 1-second total cycle → 1 Hz blinking.
}
