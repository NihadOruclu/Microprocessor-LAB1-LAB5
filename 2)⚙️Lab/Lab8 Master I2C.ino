#define F_CPU 16000000UL 
#define SLAVE_ADDR 0x08 

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

void TWI_start() {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Bayrağı təmizləyir , Start vəziyyəti yaradır, TWI modulu aktiv edir
    while (!(TWCR & (1 << TWINT))); // Start siqnalı xəttə göndərilənə qədər gözləyir.
}

void TWI_stop() {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void TWI_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);// Bayrağı təmizləyir,TWI modulu aktiv edir 
    while (!(TWCR & (1 << TWINT)));
}

uint8_t TWI_read_nack() { 
    TWCR = (1 << TWINT) | (1 << TWEN); 
    while (!(TWCR & (1 << TWINT))); 
    return TWDR; 
}

void send_to_slave(uint8_t data) {
    TWI_start();
    TWI_write((SLAVE_ADDR << 1) | 0); // Burada ünvan 1 bit sola sürüşdürülür, axırıncı bit 0 olur
    TWI_write(data); 
    TWI_stop();
}

uint8_t request_from_slave() {
    uint8_t data;
    TWI_start();
    TWI_write((SLAVE_ADDR << 1) | 1); 
    data = TWI_read_nack(); // read data from the slave
    TWI_stop();
    return data;
}

void setup() {
    TWSR = 0x00; 
    TWBR = 72; // suret
    TWCR = (1 << TWEN); // enable I2C

    PORTC |= (1 << PC4) | (1 << PC5); // SDA və SCL xətlərində daxili pull-up rezistorları aktiv edir., Xətt boşda olanda 'High' qalsın deyə

    DDRB |= (1 << PB5); // 13 cu pin
    DDRD &= ~(1 << PD2); // Düymə pini üçün pull-up aktiv edir. Düyməyə basanda 0 (GND) oxunacaq.
    PORTD |= (1 << PD2);
}

void loop() {
    if (!(PIND & (1 << PD2))) {
        send_to_slave(0x01); //slave led
    } else {
        send_to_slave(0x00);
    }

    uint8_t received = request_from_slave(); // listen to the slave

    if (received == 0x02) { // if received value from the slave is 2, then light up LED
        PORTB |= (1 << PB5);
    } else { 
        PORTB &= ~(1 << PB5);
    }

    _delay_ms(10);
}
