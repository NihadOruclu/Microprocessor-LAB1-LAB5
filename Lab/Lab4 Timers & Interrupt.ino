#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t countdown = 0;

ISR(INT0_vect) {
    if (countdown == 0) { // Checking if pulse is not already running.
        PORTB |= (1 << PB5); // Setting PB5 HIGH to start 10ms pulse.
        countdown = 10; // Initializing 10 ms countdown.
    }
}
ISR(TIMER1_COMPA_vect)
{
    PINB |= (1 << PB0);  // Toggling PB0 for oscilloscope, debuging.

    if (countdown > 0)  // Checking if pulse is active.
    {
        countdown--; // Decrementng countdown by 1 ms.
        if (countdown == 0)  // When countdown finishes....
        {
        PORTB &= ~(1 << PB5); // ...set PB5 LOW to end pulse.
        }
    }
}
void setup(void)
{
    cli();  // This allows to disable global interrupts during setup.
    DDRB |= (1 << PB5);   // Seting PB5 as output (pulse pin).
    PORTB &= ~(1 << PB5);   // Initializing PB5 LOW.
    DDRB |= (1 << PB0);   // Setting PB0 as output (for ossiloscope toogle).
    DDRD &= ~(1 << PD2);   // Setting PD2 as input (button).
    PORTD |= (1 << PD2);   // Enabling pull-up on PD2.
    EICRA |= (1 << ISC01);   // ISC01=1 Trigerring Falling edge.
    EICRA &= ~(1 << ISC00);  // ISC00=0 -> Clearing to complete Falling Edge.
    EIMSK |= (1 << INT0);   // Enabling INT0 interrupt.

    TCCR1A = 0;  // Clearing Timer1 A register.
    TCCR1B = 0;  // Clearing Timer1 B register.
    TCCR1B |= (1 << WGM12);  // This is CTC mode, clearing timer on compare match.
    OCR1A = 249;  // I set compare value for 1 ms interval.
    TIMSK1 |= (1 << OCIE1A);   // Enabling Timer1 Compare Match interrupt.
    TCCR1B |= (1 << CS11) | (1 << CS10); // Setting prescaler to 64 -> timer counts slower.
    sei(); // This will simply enable the interrupts from this point on.                    
}
void loop(void)
{
}
