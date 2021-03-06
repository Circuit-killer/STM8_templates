/**
  \file timer4.h
   
  \author G. Icking-Konert
  \date 2017-02-19
  \version 0.1
   
  \brief declaration of timer TIM4 (1ms clock) functions/macros
   
  declaration of timer TIM4 functions as 1ms master clock.
  Optional functionality via #define:
    - USE_MILLI_ISR: allow attaching user function to 1ms interrupt
*/

/*-----------------------------------------------------------------------------
    MODULE DEFINITION FOR MULTIPLE INCLUSION
-----------------------------------------------------------------------------*/
#ifndef _TIMER4_H_
#define _TIMER4_H_


/*-----------------------------------------------------------------------------
    INCLUDE FILES
-----------------------------------------------------------------------------*/

#include <stdint.h>
#include "stm8as.h"
#include "config.h"


/*-----------------------------------------------------------------------------
    DECLARATION OF GLOBAL VARIABLES
-----------------------------------------------------------------------------*/

// declare or reference to global variables, depending on '_TIM4_MAIN_'
#if defined(_TIM4_MAIN_)
  volatile uint8_t            g_flagMilli;                 ///< flag for 1ms timer interrupt. Set in TIM4 ISR
  volatile uint32_t           g_millis;                    ///< 1ms counter. Increased in TIM4 ISR
  volatile uint32_t           g_micros;                    ///< 1000us counter. Increased in TIM4 ISR
#else // _TIM4_MAIN_
  extern volatile uint8_t     g_flagMilli;
  extern volatile uint32_t    g_millis;
  extern volatile uint32_t    g_micros;
#endif // _TIM4_MAIN_


/*-----------------------------------------------------------------------------
    DECLARATION OF GLOBAL MACROS
-----------------------------------------------------------------------------*/

#define flagMilli()           g_flagMilli                   ///< 1ms flag. Set in 1ms ISR
#define clearFlagMilli()      g_flagMilli=0                 ///< clear 1ms flag
#define millis()              g_millis                      ///< get milliseconds since start of program

// with attachable user functions 
#if defined(USE_MILLI_ISR)
  #define attachInterruptMillis(fct)   TIM4_UPD_AttachInterrupt(fct)            ///< attach user function to 1ms ISR
  #define detachInterruptMillis()      TIM4_UPD_AttachInterrupt(TIM4_Default)   ///< detach user function from 1ms ISR
#endif

// only with TIM3 update/overflow interrupt
#ifdef USE_TIM4_UPD_ISR

  // for convenience
  #define startClock()     TIM4.IER.reg.UIE = 1;                ///< disable 1ms interrupt
  #define stopClock(fct)   TIM4.IER.reg.UIE = 0;                ///< enable 1ms interrupt

  // for consistency with other timers (same as above)
  #define TIM4_enable_UPD_interrupt()  TIM4.IER.reg.UIE = 1;    ///< enable TIM4UPD interrupt
  #define TIM4_disable_UPD_interrupt() TIM4.IER.reg.UIE = 0;    ///< disable TIM4UPD interrupt

#endif // USE_TIM4_UPD_ISR


/*-----------------------------------------------------------------------------
    DECLARATION OF GLOBAL FUNCTIONS
-----------------------------------------------------------------------------*/

/// init timer 4 (1ms master clock)
void TIM4_init(void);

/// delay code execution for 'ms'
void delay(uint32_t ms);

/// delay code execution for 'us'
void delayMicroseconds(uint32_t us);


// with optional call to user function in 1ms ISR
#if defined(USE_MILLI_ISR)
  
  /// default dummy function for TIM4 ISRs
  void TIM4_Default(void);

  /// attach/detach function call to below ISR
  void TIM4_UPD_AttachInterrupt(void (*pFct)(void));

#endif // USE_MILLI_ISR


/*-----------------------------------------------------------------------------
    DECLARATION OF GLOBAL INLINE FUNCTIONS
-----------------------------------------------------------------------------*/

/**
  \fn uint32_t micros(void)
   
  \brief get microseconds since start of program. Resolution is 4us
  
  \return microseconds from start of program

  Get microseconds from start of program with 4us resolution. 
  Requires TIM4 to be initialized and running, and TIM4 interrupt being active.
  Value overruns every ~1.2 hours.
*/
INLINE uint32_t micros(void) {

  uint8_t   cnt, uif;
  uint32_t  us;
  
  // for consistency of CNT ans SR briefly stop timer
  TIM4.CR1.reg.CEN = 0;

  // get current us value, TIM4 counter, and TIM4 overflow flag
  cnt = TIM4.CNTR.byte;
  uif = TIM4.SR1.byte;
  
  // restart timmer immediately to minimize time gap
  TIM4.CR1.reg.CEN = 1;
  
  // calculate current time [us], including global variable (1000us steps) and counter value (4us steps)
  us  = g_micros;
  #if defined(__CSMC__)     // Cosmic compiler has a re-entrance bug with bitshift
    us += 4 * (uint16_t) cnt;
    /*
    us += cnt;
    us += cnt;
    us += cnt;
    us += cnt;
    */
  #else
    us += ((uint16_t) cnt) << 2;
  #endif
  
  // account for possible overflow of TIM4 --> check UIF (= bit 0)
  if ((uif & 0x01) && (cnt != 250))
    us += 1000L;

  return(us);

} // micros()


/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _TIMER4_H_
