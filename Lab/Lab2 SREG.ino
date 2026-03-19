uint8_t n1, n2, ans, sreg; //unsinged 8bit=> 0-255.

uint8_t read() { //User input side.
  while (!Serial.available());
  uint8_t value = Serial.readStringUntil('\n').toInt();
  return value; 
}

void setup() {
  Serial.begin(9600); 
}

void loop() {

  Serial.println("\n A (0-255):"); 
  n1 = read();

  Serial.println("B (0-255):");
  n2 = read();

  asm volatile ( 
    "mov r16, %2      \n"
    "mov r17, %3      \n"
    "sub r16, r17     \n" //Subtraction partSreg gets updated.
    "mov %0, r16      \n"
    "in  %1, __SREG__ \n" //Gets SREG values.
    : "=r" (ans), "=r" (sreg)
    : "r" (n1), "r" (n2)
    : "r16", "r17" //For cpu not to mess with it
  );

  Serial.println("\nRESULTS"); //Printing side.
  Serial.print("Result (r16): ");
  Serial.println(ans);

  Serial.print("SREG (binary): ");
  Serial.println(sreg, BIN);

  Serial.print("Zero Flag (Z): ");
  Serial.println((sreg >> 1) & 1);

  Serial.print("Carry Flag (C): ");
  Serial.println(sreg & 1);

  Serial.print("Input A (n1): ");
  Serial.println(n1);

  Serial.print("Input B (n2): ");
  Serial.println(n2);

  delay(2000);  //Delay for myself just togive time for the next promt.
}  
