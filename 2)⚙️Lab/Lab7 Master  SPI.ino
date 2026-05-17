#include <avr/io.h>
#include <util/delay.h>

uint8_t dataSequence[3] = {85, 170, 255}; //Our values that will be transmitted via SPI.
uint8_t indexTx = 0; //Index  used to cycle through the values within our array.

void SPI_MasterInit() {
    DDRB |= (1 << PB2) | (1 << PB3) | (1 << PB5); //Setting PB2(SS), PB3(MOSI), PB5(SCK) as OUTPUT for SPI Master.
    DDRB &= ~(1 << PB4); //Making sure MISO is input since Master receives data only.
    PORTB |= (1 << PB2); //Setting SS HIGH initially (no slave selected at start).

    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0); // Enabling SPI, setting Master mode, and setting clock prescaler fosc/128.
}

void SPI_MasterTransmit(uint8_t data) {
    PORTB &= ~(1 << PB2); //Pulling SS LOW to select slave and start SPI transaction.
    _delay_us(5); // Just a small delay to stabilize slave selection before transmission.

    SPDR = data; //Loading data into SPI Data Register.
    while (!(SPSR & (1 << SPIF))); //Waiting until SPI transmission is complete (SPIF flag set).

    _delay_us(5); //Simply a small delay making sure  slave has time to process received byte.
    PORTB |= (1 << PB2); //Pulling SS HIGH to end SPI communication session.
}

void setup() {
    SPI_MasterInit(); //Initializing SPI in Master mode with configured pins and clock.
}

void loop() {
    SPI_MasterTransmit(dataSequence[indexTx]); // ending current value from array via SPI.

    indexTx++; //Moving to next value in sequence.
    if (indexTx >= 3) indexTx = 0; //Resetting index to loop through values 85 → 170 → 255 continuously.

    _delay_ms(500); //Another delay just this one is in between transmissions for visible output on oscilloscope and our serial monitor.
}
