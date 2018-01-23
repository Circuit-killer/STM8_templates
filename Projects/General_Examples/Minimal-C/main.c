/**********************
  simple C-project without Arduino-like initialization and setup()/loop(). 
  No assumption is made, no interrupts used. Can be used as starting point 
  for projects with extreme memory limitations.
  Functionality:
  - init FCPU to 16MHz
  - configure pin as output
  - toggle pin every 500ms with delay (->blocking)
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "stm8as.h"     // STM8 peripheral registers
#include "gpio.h"       // pin access routines
#include "sw_delay.h"   // SW pause without timer

// define board LED pin
#if defined(STM8S105)
  #define LED   pinSet(PORT_D, pin0)   // STM8S Discovery Board
#else
  #define LED   pinSet(PORT_H, pin3)   // muBoard LED
#endif


// main routine
void main(void) {
  
  uint8_t  i;
  
  // switch to 16MHz (default is 2MHz) 
  CLK.CKDIVR.byte = 0x00;  

  // configure LED pin as output
  #if defined(STM8S105)
    pinMode(PORT_D, pin0, OUTPUT);    // STM8S Discovery Board
  #else
    pinMode(PORT_H, pin3, OUTPUT);    // muBoard LED
  #endif
  
  // main loop
  while (1) {

    // toggle LED
    LED ^= 1;
    
    // wait some time
    sw_delay(500);
    
  } // main loop

} // main

