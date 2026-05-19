#define F_CPU 16000000UL 
#define SLAVE_ADDR 0x08 

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

void TWI_start() {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); //Clearing flag, then making  START condition and enabling TWI module.
    while (!(TWCR & (1 << TWINT))); // Waits until START condition is sent tp bus.
}

void TWI_stop() {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // Stop condition for bus.
}

void TWI_write(uint8_t data) {
    TWDR = data; // Loading data into data register.
    TWCR = (1 << TWINT) | (1 << TWEN); // Clearing flag and starting transmission.
    while (!(TWCR & (1 << TWINT))); // Waits until byte transmission is complete.
}

uint8_t TWI_read_nack() { 
    TWCR = (1 << TWINT) | (1 << TWEN); //Enabling reception without ACK.
    while (!(TWCR & (1 << TWINT))); // Waits until data is received.
    return TWDR; // Returns byte from slave.
}

void send_to_slave(uint8_t data) {
    TWI_start(); //Generating start condition.
    TWI_write((SLAVE_ADDR << 1) | 0); //Sending slave adress.
    TWI_write(data); // Our actual data.
    TWI_stop(); // Ending I2C communication
}

uint8_t request_from_slave() {
    uint8_t data;
    TWI_start(); // Generateing START condition.
    TWI_write((SLAVE_ADDR << 1) | 1); // Sending slave address with READ bit.
    data = TWI_read_nack(); // Reading byte from slave without ACK.
    TWI_stop(); // Ending I2C communication/
    return data; // Returning received data.
}

void setup() {
    TWSR = 0x00; //Just setting prescaler to 1.
    TWBR = 72; // Seting I2C speed ~100kHz for 16MHz clock.
    TWCR = (1 << TWEN); //Enabling TWI hardware module.

    PORTC |= (1 << PC4) | (1 << PC5); // Enabling pull-up resistors on SDA (PC4) and SCL (PC5).

    DDRB |= (1 << PB5); // Setting PB5 as output.
    DDRD &= ~(1 << PD2); // Seting PD2 as input (button).
    PORTD |= (1 << PD2); // Enableign pull-up resistor on button pin.
}

void loop() {
    if (!(PIND & (1 << PD2))) { //Checking if button is pressed.
        send_to_slave(0x01); // Sending ON command to slave LED.
    } else {
        send_to_slave(0x00); // Sending OFF command to slave LED.
    }

    uint8_t received = request_from_slave(); // Reading response from slave.

    if (received == 0x02) { // Checking if slave button was pressed.
        PORTB |= (1 << PB5); // Turning ON master LED.
    } else { 
        PORTB &= ~(1 << PB5); // Turning OFF master LED.
    }

    _delay_ms(10); // Small delay for stability.
}
