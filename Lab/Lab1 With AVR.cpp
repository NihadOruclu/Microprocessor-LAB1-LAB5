const int pin = 5;

void setup() {
  DDRB |= (1 << pin);  
}

void loop() {
  PORTB |= (1 << pin);   
  delay(500);          

  PORTB ^= (1 << 5);  
  delay(500);          
}
