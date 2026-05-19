#define F_CPU 16000000UL
#include <avr/io.h>

#define SLAVE_ADDR 0x08 

volatile uint8_t state = 0x00 ; //Value to send back to Master. OX08 will be our slave adress.

void setup() {
  TWAR = (SLAVE_ADDR << 1); //Setting slave address (shift because last bit reserved).
  TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT); //Enabling TWI + ACK + clear flag.

  PORTC |= (1 << PC4) | (1 << PC5); //Pullups for SDA and SCL.

  DDRB |= (1 << PB5); //LED output.
  DDRD &= ~(1 << PD2); //Button input.
  PORTD |= (1 << PD2); //Pull-up for button.
}

void loop() {
      if (!(PIND & (1 << PD2))) { //If button pressed (LOW).
          state = 0x02; //Preparing value to send to Master.
      }

      if (TWCR & (1 << TWINT)) { //Checking if I2C event happened.
          uint8_t status = TWSR & 0b11111000; //Getting status code only.

          if (status == 0x60) { //Master wants to send data.
              TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT); //Ready to receive.
          } else if (status == 0x80) { //Data received from Master.
              uint8_t received = TWDR; //Read data.
              if (received == 0x01) {  
                  PORTB |= (1 << PB5); //LED ON.
              } else {
                  PORTB &= ~(1 << PB5); //LED OFF.
              }
              TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT); //Continue.
          } else if (status == 0xA8) { //Master wants to read.
              TWDR = state; //Send state.
              TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
              state = 0x00; //Reset after sending.
          } else { //Anything else.
              TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT); //Ignore and continue.
          }
      }
}
