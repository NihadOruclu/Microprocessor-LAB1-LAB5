#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t count = 0; // My counter variable 
volatile uint8_t is_running = 1; // Just a flag to control timer state (1 = running, 0 = paused).

const uint8_t segments[10] = {
    0b00111111, //0 → segment  for digit 0
    0b00000110, //1 → segment  for digit 1
    0b01011011, //2 → segment  for digit 2
    0b01001111, //3 → segment  for digit 3
    0b01100110, //4 → segment  for digit 4
    0b01101101, //5 → segment  for digit 5
    0b01111101, //6 → segment  for digit 6
    0b00000111, //7 → segment  for digit 7
    0b01111111, //8 → segment  for digit 8
    0b01101111  //9 → segment  for digit 9
};

static inline void displayDigit(uint8_t digit) {
    uint8_t val = segments[digit]; // Fetching segment  from lookup table.

    PORTD = (PORTD & 0x07) | ((val & 0x1F) << 3); //Sending lower 5 bits to PORTD while preserving PD0–PD2.

    PORTB = (PORTB & 0xF8) | ((val >> 5) & 0x07); //Sending upper 3 bits to PORTB while preserving PB3–PB7.
}

static inline void timer1_start(void) {
  
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); //WGM12 = CTC mode, CS12+CS10 = prescaler 1024.
}

static inline void timer1_stop(void) {

    
    TCCR1B = (1 << WGM12); // Only WGM12 set → timer stops counting. CTC active.
}

void setup() {
    cli(); // Disabling interrupts.

    DDRD |= 0xF8; //Setting PD3–PD7 as output for 7-segment.
    
    DDRD &= ~(1 << PD2); //Setting PD2 as input for external interrupt button.
    PORTD |= (1 << PD2); //Enable pull-up resistor on PD2.

    DDRB |= 0x07; //Setting PB0–PB2 as output for remaining  pins.

    PORTD &= 0x07; //Just clearing output bits for safe initial state on PORTD. Same goes down below VV.
    PORTB &= 0xFC; 

    TCCR1A = 0; //Normal mode.
    TCCR1B = 0; //Resetting Timer1 control register.
    TCNT1  = 0; //Timer counter 0.

    OCR1A = 7811; //Compare match value for 0.5s delay (16MHz / 1024 prescaler).

    TIMSK1 |= (1 << OCIE1A); //Enabling Timer1 Compare Match A interrupt.

    EICRA |= (1 << ISC01); //Configuring INT0 for falling edge.
    EIMSK |= (1 << INT0); //Enabling external interrupt INT0.

    displayDigit(count); //Showing  digit (0) on display on start.

    timer1_start(); //Starting Timer1.
    sei(); //Enabling global interrupts.
}

ISR(TIMER1_COMPA_vect) {
    count++; //Incrementing counter every interrupt.
    if (count > 9) {
        count = 0; //Reseting counter after reaching 9.
    }
    displayDigit(count); //Updating 7-segment display output.
}

ISR(INT0_vect) {
    is_running ^= 1; //Toggling running state (pause/resume).

    if (is_running) {
        timer1_start(); // Resuming timer by enabling prescaler.
    } else {
        timer1_stop(); //Pausing timer by disabling clock source.
    }
}

void loop() {
    //Just empty loop because our system is fully interrupt driven.
}
