#include <stm32l432xx.h>
#include "ee14lib.h"
#include <math.h>

// #define 220 A2
// #define 349 F3
// #define 329 E3
// #define 293 D3

int _write(int file, char *data, int len){
    serial_write(USART2, data, len);
    return len;
}

void config_gpio_interrupt(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    SYSCFG->EXTICR[1] = 0b001;
    SYSCFG->EXTICR[0] &= ~(0xF << 12);
    SYSCFG->EXTICR[0] |= (0x1 << 12);
    EXTI->FTSR1 |= EXTI_FTSR1_FT4;
    EXTI->IMR1 |= EXTI_IMR1_IM4;
    NVIC_SetPriority(EXTI4_IRQn, 4);
    NVIC_EnableIRQ(EXTI4_IRQn);
    EXTI->FTSR1 |= EXTI_FTSR1_FT3;
    EXTI->IMR1 |= EXTI_IMR1_IM3;
    NVIC_SetPriority(EXTI3_IRQn, 3);
    NVIC_EnableIRQ(EXTI3_IRQn);
}

void DAC_init(void) {
    DAC1->CR = 0x0; //reset, just in case.
    DAC1->CR |= 0x1; // Enabling DAC1
    DAC1->CR &= ~DAC_CR_TEN1; //disable the trigger source
    DAC1->CR |= (0b00 << DAC_CR_WAVE1_Pos); //disables wave gen
    DAC1->CR |= (0b0111 << DAC_CR_MAMP1_Pos); //triangle wave max amplitude ( >= 1011)
}

uint32_t pitch(int freq)
{
    return ((uint64_t)16777216 * freq) / 2210;
}

signed int sin_table[256];

void play(int freq, int length) {
    uint32_t phase = 0;
    uint32_t phase_increment = pitch(freq);
    for (int i = 0; i < length; i++) {
        phase += phase_increment;
        uint8_t index = (phase >> 24) & 0xFF;  // Take top bits for table index
        DAC1->DHR12R1 = sin_table[index];
    }
}

void EXTI4_IRQHandler(void)
{
    if (EXTI->PR1 & EXTI_PR1_PIF4) {
        EXTI->PR1 |= EXTI_PR1_PIF4;
        uint32_t phase = 0;
        uint32_t phase_increment = pitch(440);
        while (!gpio_read(D12)) {
            phase += phase_increment;
            uint8_t index = (phase >> 24) & 0xFF;  // Take top bits for table index
            DAC1->DHR12R1 = sin_table[index];
        }
    }
}

void EXTI3_IRQHandler(void)
{
    if (EXTI->PR1 & EXTI_PR1_PIF3) {
        EXTI->PR1 |= EXTI_PR1_PIF3;
        uint32_t phase = 0;
        uint32_t phase_increment = pitch(220);
        while (!gpio_read(D13)) {
            phase += phase_increment;
            uint8_t index = (phase >> 24) & 0xFF;  // Take top bits for table index
            DAC1->DHR12R1 = sin_table[index];
        }
    }
}

void classical() {
    int tempo = 90000;
    play(220, tempo);     //A
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(349, tempo);     //F
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(329, tempo);     //E
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(293, tempo);     //D
    play(220, tempo);     //A
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(349, tempo);     //F
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(329, tempo);     //E
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(293, tempo);     //D
    play(220, tempo);     //A
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(349, tempo);     //F
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(329, tempo);     //E
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(293, tempo);     //D
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(220, tempo);
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(349, tempo);  
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(293, tempo);
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(329, tempo);
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(220, tempo); 
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(196, tempo); //G
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(349, tempo); 
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(329, tempo); 
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(220, tempo); 
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(196, tempo); //G
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(349, tempo); 
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(329, tempo); 
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(220, tempo); 
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(196, tempo); //G
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(349, tempo); 
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(329, tempo);  
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(220, tempo); 
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(196, tempo);
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(329, tempo);
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(293, tempo);
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(440, tempo); 
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(593, tempo); 
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(493, tempo); 
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(246, tempo); 
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(293, tempo);
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(493, tempo); 
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(392, tempo);
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(196, tempo);
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(246, tempo);
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(392, tempo);
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(329, tempo);
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(165, tempo);
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(220, tempo);
    for (volatile int i = 0; i < tempo / 5; i++) { }
    play(329, tempo);
}


int main() {
    host_serial_init();
    gpio_config_pullup(A3, PULL_OFF);
    gpio_config_mode(A3, ANALOG);
    gpio_config_pullup(D12, PULL_UP);
    gpio_config_mode(D12, INPUT);
    gpio_config_pullup(D13, PULL_UP);
    gpio_config_mode(D13, INPUT);
    config_gpio_interrupt();
    RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN; //dac clock enable
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
    float sf;
    DAC_init();
    for (int i = 0; i < 256; i++){
        sf = cos(3.1415926 * ((float)i / 128));
        sin_table[i] = (1+sf) * 2048;
        if(sin_table[i] >= 0x1000){
            sin_table[i] = 0xFFF;
        }
    }

    // DAC1->CR &= ~(DAC_CR_WAVE1 | DAC_CR_MAMP1 | DAC_CR_TSEL1); // Clear WAVE, MAMP, TSEL
    // DAC1->CR |= DAC_CR_WAVE1_1;          // Enable triangle wave generation (WAVE1 = 0b10)
    
    // DAC1->CR |= DAC_CR_TEN1;             // Enable trigger (TEN1)
    // DAC1->CR |= DAC_CR_EN1;              // Enable DAC Channel 1
    
    /*
    host_serial_init();
    gpio_config_mode(D4, 1);
    adc_config_single(A0); //placed here so that we can measure the read later
    gpio_write(D4, 0);
    while(1){
        gpio_write(D4, 1);
        adc_read_single(); //commented out to measure only the write times
                           //replaced with adc_config_single to measure the config time
        gpio_write(D4, 0);
        printf("pause\n"); // this is the easiest operation that takes 
                           //a long time, which makes it easier for the oscilliscope to parse
    }
    */
    while(1){ }

}
