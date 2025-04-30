#include "stm32l432xx.h"
#include "ee14lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Seven segment constants
// #define A A0
// #define B A1
// #define C A2
// #define D A3
// #define E A4
// #define F A5
// #define G A6
// #define ONES D4
// #define TENS D5
// #define INPUT D12

volatile int tick = 0;
// This function MUST be named SysTick_Handler for the CMSIS framework
// code to link to it correctly.


void SysTick_initialize(void) {
    // TODO: figure out what each line of code in this function does
    SysTick->CTRL = 0;
    SysTick->LOAD = 39; // This sets the rate of interupts for this function
                          // based on the internal cpu clock. The value 3999
                          // triggers an interupt every millisecond.

    // This sets the priority of the interrupt to 15 (2^4 - 1), which is the
    // largest supported value (aka lowest priority)
    NVIC_SetPriority (SysTick_IRQn, 1);
    // This sets the initial value of the SysTick register to 0, so that it will
    // start at the LOAD value on the next tick. 
    SysTick->VAL = 0;
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;    // Selects the CPU clk as 
                                                    // the clock source.
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;      // Makes it so the SysTick
                                                    // exception request is 
                                                    // enabled.
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;       // Actually enables the
                                                    // counter to count down to
                                                    // zero and reset to the 
                                                    // LOAD value. 
}

// void EXTI4_IRQHandler(void)
// {
//     if (EXTI->PR1 & EXTI_PR1_PIF4) {
//         EXTI->PR1 |= EXTI_PR1_PIF4;
//         tick = 0;
//     }
// }

// void config_gpio_interrupt(void)
// {
//     RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
//     SYSCFG->EXTICR[1] = 0b001;
//     EXTI->FTSR1 |= EXTI_FTSR1_FT4;
//     EXTI->IMR1 |= EXTI_IMR1_IM4;
//     NVIC_SetPriority(EXTI4_IRQn, 2);
//     NVIC_EnableIRQ(EXTI4_IRQn);
// }

void delay_ms(int ms)
{
    int startTime = tick;
    while ((tick - startTime) < ms) {
        if (abs(tick - startTime) > ms)
            break;
    }
}

// #define HIGH 1
// #define LOW 0
// volatile bool segments[70] = {
//     LOW, LOW, LOW, LOW, LOW, LOW, HIGH, // 0
//     HIGH, LOW, LOW, HIGH, HIGH, HIGH, HIGH, // 1
//     LOW, LOW, HIGH, LOW, LOW, HIGH, LOW, // 2
//     LOW, LOW, LOW, LOW, HIGH, HIGH, LOW, // 3
//     HIGH, LOW, LOW, HIGH, HIGH, LOW, LOW, // 4
//     LOW, HIGH, LOW, LOW, HIGH, LOW, LOW, // 5
//     LOW, HIGH, LOW, LOW, LOW, LOW, LOW, // 6
//     LOW, LOW, LOW, HIGH, HIGH, HIGH, HIGH, // 7
//     LOW, LOW, LOW, LOW, LOW, LOW, LOW, // 8
//     LOW, LOW, LOW, HIGH, HIGH, LOW, LOW  // 9
// };

// void setDigit(int place, int number) 
// {    
    
//     if (place) {
//         gpio_write(ONES, 0);
//         gpio_write(TENS, 1);
//     } else {
//         gpio_write(ONES, 1);
//         gpio_write(TENS, 0);
//     }
//     int offset = number * 7;
//     gpio_write(A, segments[offset]);
//     gpio_write(B, segments[offset + 1]);
//     gpio_write(C, segments[offset + 2]);
//     gpio_write(D, segments[offset + 3]);
//     gpio_write(E, segments[offset + 4]);
//     gpio_write(F, segments[offset + 5]);
//     gpio_write(G, segments[offset + 6]);


// }

// int getPlace(int place, int number)
// {
//     int div = pow(10, place);
    
//     return (number / div) % 10;
// }
