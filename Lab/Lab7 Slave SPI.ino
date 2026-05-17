#include <avr/io.h>
#include <Arduino.h>

void SPI_SlaveInit() {
    DDRB |= (1 << PB4);
    DDRB &= ~((1 << PB2) | (1 << PB3) | (1 << PB5));

    SPCR = (1 << SPE); 
    
    SPDR = 0x3C; //60
}

void setup() {
    Serial.begin(9600);
    SPI_SlaveInit();
}

void loop() {
    if (SPSR & (1 << SPIF)) {
        uint8_t received = SPDR; 
        Serial.print("Gelen: ");
        Serial.println(received);

        SPDR = 0x3C; 
    }
}
