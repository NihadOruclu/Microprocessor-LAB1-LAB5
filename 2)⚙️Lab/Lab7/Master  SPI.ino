#include <avr/io.h>
#include <util/delay.h>

uint8_t dataSequence[3] = {85, 170, 255};
uint8_t indexTx = 0;

void SPI_MasterInit() {
    DDRB |= (1 << PB2) | (1 << PB3) | (1 << PB5);
    // PB4(MISO) -> Input
    DDRB &= ~(1 << PB4);
    PORTB |= (1 << PB2);

    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
}

void SPI_MasterTransmit(uint8_t data) {
    PORTB &= ~(1 << PB2); // SS LOW (Slave-i seç)
    _delay_us(5);         // Qısa sinxronizasiya boşluğu

    SPDR = data;          // Məlumatı göndər
    while (!(SPSR & (1 << SPIF))); // Bitməsini gözlə

    _delay_us(5);         // Slave-in qəbul etməsinə vaxt tanı
    PORTB |= (1 << PB2);  // SS HIGH (Bitir)
}

void setup() {
    SPI_MasterInit();
}

void loop() {
    SPI_MasterTransmit(dataSequence[indexTx]);
    
    indexTx++;
    if (indexTx >= 3) indexTx = 0;

    _delay_ms(500); // 1 saniyə çoxdur, 500ms daha dinamik görünür
}
