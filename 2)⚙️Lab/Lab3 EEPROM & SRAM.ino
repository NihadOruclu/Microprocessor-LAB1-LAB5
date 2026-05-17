#include <Arduino.h>
#include <avr/io.h>

volatile uint8_t counter = 0; // Unsigned 8-bit variable  (0–255).

void setup() {
  Serial.begin(9600); // Initialize Serial communication at 9600 (Standart).
  
  while (EECR & (1 << EEPE)) {}   // Waits if EEPROM is busy.
  EEAR = 0x00;// Seting EEPROM Address Register to 0x00. You may alter it but others must be chagned as well.
  EECR |= (1 << EERE);// Starts EEPROM read → value at EEAR copied to EEDR.
  counter = EEDR;// Loading EEPROM value into counter.

  Serial.println("Commands: S/s = save, R/r = reset"); //For user interface.
}

void loop() {
  // Incrementation part.
  asm volatile(
    "lds r24, counter  \n\t"// Loading counter from SRAM into r24.
    "inc r24           \n\t" // Incrementing r24 by 1.
    "sts counter, r24  \n\t"// Storingincremented value back to SRAM variable counter.
    :
    :
    : "r24"// Telling compiler r24 is modified not to mess with it.
  );

  Serial.print("Counter = ");// Displaying counter.
  Serial.println(counter);// Printing current counter value.

  if (Serial.available()) { //Checking if user typed anything in Serial Monitor promt.
    char command = Serial.read(); // Reads the command character.

    // SAVE 
    if (command == 'S' || command == 's') {
      while (EECR & (1 << EEPE)) {} // Waits if EEPROM is busy.
      EEAR = 0x00;//Seting EEPROM address to 0x00. 
      EEDR = counter;// Loading counter value into EEPROM Data Register.
      EECR |= (1 << EEMPE);// This is Master Write Enable: allows writing.
      EECR |= (1 << EEPE);// Starts EEPROM write.
      Serial.println("Stored in EEPROM[0]."); // Informs user.
    }

    // RESET 
    if (command == 'R' || command == 'r') {
      counter = 0;// Resettin counter to 0.

      while (EECR & (1 << EEPE)) {}   // Waits if EEPROM is busy.
      EEAR = 0x00;// Same EEPROM address as stated previously.
      EEDR = 0;// Loading 0 into EEPROM Data Register.
      EECR |= (1 << EEMPE);// Master Write Enable tuning on.
      EECR |= (1 << EEPE);// Starting EEPROM write.
      Serial.println("Reset to 0 and stored in EEPROM[0]."); // Informing user.
    }
  }

  delay(1000); // Again this is for myself to give time on serial monitor visually.
}
