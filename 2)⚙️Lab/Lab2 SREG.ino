uint8_t n1, n2, ans, sreg; //Unsinged 8bit=> 0-255.

uint8_t read() { //Function to read our input from Serial monitor.
  while (!Serial.available());// Waits until user types a value.
  uint8_t value = Serial.readStringUntil('\n').toInt(); // Reads input as string until newline which happens after pressing enter, afterwards it converts to integer.
  return value;
}

void setup() {
  Serial.begin(9600);// Initialize Serial communication at 9600 the standart.
}

void loop() {

  Serial.println("\nA (0-255):");// Prompts user for first number.
  n1 = read();// Stores input in n1.

  Serial.println("B (0-255):");// Prompts user for second number.
  n2 = read();// Stores input in n2.


  asm volatile (  //  Performs subtraction and stores result + status flags.
    "mov r16, %2      \n"// Moves n1 into register r16.
    "mov r17, %3      \n"// Moves n2 into register r17.
    "sub r16, r17     \n"// Subtracts r17 from r16 → r16 = r16 - r17, updates SREG flags (Z, C, N,V).
    "mov %0, r16      \n" // Moves the result from r16 into ans (Answer).
    "in  %1, __SREG__ \n" // Reads current SREG into sreg (Variable).
    : "=r" (ans), "=r" (sreg)  // Outputs operands: ans and sreg.
    : "r" (n1), "r" (n2)// Inputs operands: n1 and n2.
    : "r16", "r17"// //For compiler not to mess with it and tells it r16 and r17 are modified.
  );

  Serial.println("\nRESULTS");//Printing side.         
  Serial.print("Result (r16): ");              
  Serial.println(ans);// Displays subtraction result.

  Serial.print("SREG (binary): ");             
  Serial.println(sreg, BIN);// Displays SREG in binary (all flags).

  Serial.print("Zero Flag (Z): ");             
  Serial.println((sreg >> 1) & 1);//Extracts Z flag (bit 1) → 1 if result is zero.

  Serial.print("Carry Flag (C): ");            
  Serial.println(sreg & 1);// Extracts C flag (bit 0) → 1 if borrow occurred.

  Serial.print("Input A (n1): ");              
  Serial.println(n1);// Displays first input.

  Serial.print("Input B (n2): ");              
  Serial.println(n2);// Displays second input.  Inputs are displayed for myself for verification in case my result mismatches due to mistype.

  delay(2000);//Delay for myself just to give time for the next promt.
}
