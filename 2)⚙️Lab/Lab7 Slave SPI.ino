#include <avr/io.h>
#include <Arduino.h>

void SPI_SlaveInit() {
    DDRB |= (1 << PB4); // PB4 (MISO) output for slave response.
    DDRB &= ~((1 << PB2) | (1 << PB3) | (1 << PB5)); //PB2 SS, PB3 MOSI, PB5 SCK set as input.

    SPCR = (1 << SPE); //Enabling SPI in slave mode (SPE = 1, MSTR = 0 by default).
    
    SPDR = 0x3C; //Just preloading SPI data register with 60 decimal to test MISO on osiloscope.
}

void setup() {
    Serial.begin(9600); //Starting serial monitor at 9600.
    SPI_SlaveInit(); // Initializing SPI slave.
}

void loop() {
    if (SPSR & (1 << SPIF)) { //Checking if our SPI transfer complete flag is set.
        uint8_t received = SPDR; //Reading received byte from SPI register.
        Serial.print("Coming: "); // Printing a label just.
        Serial.println(received); //Print received value from master.

        SPDR = 0x3C; // Reloading slave response value  60 for MISO line same as above.
    }
}
