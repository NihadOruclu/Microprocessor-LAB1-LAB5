const int pin = 5;       // Defining pin 5 as (PORTB5) for LED output which is actually PIN 13.

void setup() {
  DDRB |= (1 << pin);    // Setting pin 5 of PORTB as output Data Direction Register B || 1 = output, 0 = input
}

void loop() {
  PORTB |= (1 << pin);   // Setting pin 5 HIGH → LED turns ON
  delay(500);            // Waits for 500 milliseconds

  PORTB ^= (1 << 5);     // Toggling pin 5 using (XOR) → LED changes state (ON → OFF or OFF → ON).
  delay(500);            // Waits for 500 milliseconds 
  //500+500 = 1HZ
}
