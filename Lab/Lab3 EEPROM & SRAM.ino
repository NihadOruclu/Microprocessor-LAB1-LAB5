#include <Arduino.h>
#include <avr/io.h>

volatile uint8_t counter = 0; //Unsinged 8 bit 0-255.

void setup() {
  Serial.begin(9600);

  // Read EEPROM from adress [0x00] at boot.
  while (EECR & (1 << EEPE)) {}   // Wait if eeprom is busy.
  EEAR = 0x00; //The adress I assinged.
  EECR |= (1 << EERE);
  counter = EEDR;

  Serial.println("Commands: S/s = save, R/r = reset"); //For user interface.
}

void loop() {
  // Incrementation part.
  asm volatile(
    "lds r24, counter  \n\t"
    "inc r24           \n\t"
    "sts counter, r24  \n\t" //Storing in SRAM.
    :
    :
    : "r24"
  );

  Serial.print("Counter = ");
  Serial.println(counter);

  if (Serial.available()) {
    char command = Serial.read();

    // SAVE 
    if (command == 'S' || command == 's') {
      while (EECR & (1 << EEPE)) {} // Wait if busy.
      EEAR = 0x00; // Save to same address.
      EEDR = counter;
      EECR |= (1 << EEMPE);
      EECR |= (1 << EEPE);
      Serial.println("Stored in EEPROM[0].");
    }

    // RESET 
    if (command == 'R' || command == 'r') {
      counter = 0;

      while (EECR & (1 << EEPE)) {} // Wait if busy.
      EEAR = 0x00; // Same adress.
      EEDR = 0;
      EECR |= (1 << EEMPE);
      EECR |= (1 << EEPE);
      Serial.println("Reset to 0 and stored in EEPROM[0].");
    }
  }

  delay(1000); 
}
