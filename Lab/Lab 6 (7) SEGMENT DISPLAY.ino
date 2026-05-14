#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t count = 0;
volatile uint8_t is_running = 1;

const uint8_t segments[10] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};

static inline void displayDigit(uint8_t digit) {
    uint8_t val = segments[digit];

    PORTD = (PORTD & 0x07) | ((val & 0x1F) << 3);

    PORTB = (PORTB & 0xF8) | ((val >> 5) & 0x07);
}

static inline void timer1_start(void) {
    //pres
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // Sets WGM12 (CTC mode) and Prescaler 1024 (CS12 & CS10)
}

static inline void timer1_stop(void) {

    TCCR1B = (1 << WGM12);
}

void setup() {
    cli();


    DDRD |= 0xF8;
    
    

    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);

    
    DDRB |= 0x07;

    
    PORTD &= 0x07;
    PORTB &= 0xFC;

    
    TCCR1A = 0; // Normal operation
    TCCR1B = 0; // Reset TCCR1B
    TCNT1  = 0; // Initialize counter to 0


    OCR1A = 7811; // = (16,000,000 / (1024 * 2)) - 1

    TIMSK1 |= (1 << OCIE1A);


    EICRA |= (1 << ISC01);
    EIMSK |= (1 << INT0);


    displayDigit(count);

    timer1_start();
    sei();
}

ISR(TIMER1_COMPA_vect) {
    count++; // Increment the digit
    if (count > 9) {
        count = 0; // Reset to 0 after reaching 9
    }
    displayDigit(count); // Update the physical 7-segment pins
}

ISR(INT0_vect) {
    is_running ^= 1; // Toggle the running state

    if (is_running) {
        timer1_start(); // Resume timer by setting prescaler
    } else {
        timer1_stop(); // Pause timer by clearing prescaler
    }
}

void loop() {
}
