#define F_CPU 16000000UL
#include <avr/io.h>

#define SLAVE_ADDR 0x08

volatile uint8_t state = 0x00 ;

void setup() {
  TWAR = (SLAVE_ADDR << 1); // set address to the SLAVE_ADDR. Offset by 1 bit because LSM of TWAR is GSE.
  TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT); // Enable TWI, send ACK, write 1 to TWINT to continue

  PORTC |= (1 << PC4) | (1 << PC5);

  DDRB |= (1 << PB5); 
  DDRD &= ~(1 << PD2); 
  PORTD |= (1 << PD2);
}

void loop() {
      if (!(PIND & (1 << PD2))) { // check if button is pressed
          state = 0x02;
      }

      if (TWCR & (1 << TWINT)) {
          uint8_t status = TWSR & 0b11111000; 

          if (status == 0x60) { // Master mənə müraciət etdi və nəsə yazmaq istəyir
              TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
          } else if (status == 0x80) { // Master mənə bir bayt məlumat göndərdi". Biz bu baytı TWDR-dən oxuyuruq. Əgər 0x01-dirsə, LED-i yandırırıq
              uint8_t received = TWDR; // read TWDR
              if (received == 0x01) {  
                  PORTB |= (1 << PB5);
              } else {
                  PORTB &= ~(1 << PB5);
              }
              TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
          } else if (status == 0xA8) { // Master mənə müraciət etdi və məndən nəsə oxumaq (Read) istəyir
              TWDR = state; // send current state
              TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
              state = 0x00; 
          } else { // in any other signal, ignore it, send ACK to continue communication and set TWINT to 1 to proceed
              TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
          }
      }
}
