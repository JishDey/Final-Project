#include <stm32l432xx.h>
#include "ee14lib.h"
#include <math.h>

int _write(int file, char *data, int len){
    serial_write(USART2, data, len);
    return len;
}

uint32_t phase = 0;
uint32_t phase_increment = 0;


signed int sin_table[256];
signed int sqr_table[256];

void SysTick_Handler(void)
{
    phase += phase_increment;
    uint8_t index = (phase >> 24) & 0xFF;  // Take top bits for table index
    DAC1->DHR12R1 = sin_table[index];
}

void config_gpio_interrupt(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    // Port D12
    SYSCFG->EXTICR[1] = 0b001;
    EXTI->FTSR1 |= EXTI_FTSR1_FT4;
    EXTI->IMR1 |= EXTI_IMR1_IM4;
    NVIC_SetPriority(EXTI4_IRQn, 4);
    NVIC_EnableIRQ(EXTI4_IRQn);
    // Port A2
    SYSCFG->EXTICR[0] &= ~(0xF << 12);
    EXTI->FTSR1 |= EXTI_FTSR1_FT3;
    EXTI->IMR1 |= EXTI_IMR1_IM3;
    NVIC_SetPriority(EXTI3_IRQn, 4);
    NVIC_EnableIRQ(EXTI3_IRQn);
    // Port A7
    SYSCFG->EXTICR[0] &= ~(0xF << 8);
    EXTI->FTSR1 |= EXTI_FTSR1_FT2;
    EXTI->IMR1 |= EXTI_IMR1_IM2;
    NVIC_SetPriority(EXTI2_IRQn, 4);
    NVIC_EnableIRQ(EXTI2_IRQn);
    // Port A1
    SYSCFG->EXTICR[0] &= ~(0xF << 4);
    EXTI->FTSR1 |= EXTI_FTSR1_FT1;
    EXTI->IMR1 |= EXTI_IMR1_IM1;
    NVIC_SetPriority(EXTI1_IRQn, 4);
    NVIC_EnableIRQ(EXTI1_IRQn);
    // Port A4
    SYSCFG->EXTICR[1] &= ~(0xF << 4);
    EXTI->FTSR1 |= EXTI_FTSR1_FT5;
    EXTI->IMR1 |= EXTI_IMR1_IM5;
    // Port A5
    SYSCFG->EXTICR[1] &= ~(0xF << 8);
    EXTI->FTSR1 |= EXTI_FTSR1_FT6;
    EXTI->IMR1 |= EXTI_IMR1_IM6;
    // Port A6
    SYSCFG->EXTICR[1] &= ~(0xF << 12);
    EXTI->FTSR1 |= EXTI_FTSR1_FT7;
    EXTI->IMR1 |= EXTI_IMR1_IM7;

    // Interupt enable for A4, A5 and A6
    NVIC_SetPriority(EXTI9_5_IRQn, 4);
    NVIC_EnableIRQ(EXTI9_5_IRQn);

}

void DAC_init(void) {
    DAC1->CR = 0x0; //reset, just in case.
    DAC1->CR |= 0x1; // Enabling DAC1
    DAC1->CR &= ~DAC_CR_TEN1; //disable the trigger source
    DAC1->CR |= (0b00 << DAC_CR_WAVE1_Pos); //disables wave gen
    DAC1->CR |= (0b0111 << DAC_CR_MAMP1_Pos); //triangle wave max amplitude ( >= 1011)
}

#define note7 262
#define note6 294
#define note5 330
#define note4 349
#define note3 392
#define note2 440
#define note1 494

uint32_t pitch(int freq)
{
    return ((uint64_t)16777216 * freq) / 380;
}


void play(int freq, int length) {
    phase_increment = pitch(freq);
    delay_ms(500);
    phase_increment = 0;
}

void EXTI4_IRQHandler(void)
{
    if (EXTI->PR1 & EXTI_PR1_PIF4) {
        EXTI->PR1 |= EXTI_PR1_PIF4;
        phase_increment = pitch(note7);
        while (!gpio_read(D12)) { }
        phase_increment = 0;
    }
}

void EXTI3_IRQHandler(void)
{
    if (EXTI->PR1 & EXTI_PR1_PIF3) {
        EXTI->PR1 |= EXTI_PR1_PIF3;
        phase_increment = pitch(note1);
        while (!gpio_read(A2)) { }
        phase_increment = 0;
    }
}

void EXTI2_IRQHandler(void)
{
    if (EXTI->PR1 & EXTI_PR1_PIF2) {
        EXTI->PR1 |= EXTI_PR1_PIF2;
        phase_increment = pitch(note6);
        while (!gpio_read(A7)) { }
        phase_increment = 0;
    }
}

void EXTI1_IRQHandler(void)
{
    if (EXTI->PR1 & EXTI_PR1_PIF1) {
        EXTI->PR1 |= EXTI_PR1_PIF1;
        phase_increment = pitch(note5);
        while (!gpio_read(A1)) { }
        phase_increment = 0;
    }
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI->PR1 & EXTI_PR1_PIF5) {
        EXTI->PR1 |= EXTI_PR1_PIF5;
        phase_increment = pitch(note4);
        while (!gpio_read(A4)) { }
        phase_increment = 0;
    }
    if (EXTI->PR1 & EXTI_PR1_PIF6) {
        EXTI->PR1 |= EXTI_PR1_PIF6;
        phase_increment = pitch(note3);
        while (!gpio_read(A5)) { }
        phase_increment = 0;
    }
    if (EXTI->PR1 & EXTI_PR1_PIF7) {
        EXTI->PR1 |= EXTI_PR1_PIF7;
        phase_increment = pitch(note2);
        while (!gpio_read(A6)) { }
        phase_increment = 0;
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

void sin_table_init() {
    float sf;
    for (int i = 0; i < 256; i++){
        sf = cos(3.1415926 * ((float)i / 128));
        sin_table[i] = (1+sf) * 2048;
        if(sin_table[i] >= 0x1000){
            sin_table[i] = 0xFFF;
        }
    }
}

void sqr_table_init() {
    float sf;
    for (int i = 0; i < 256; i++){
        if(i < 128)
            sqr_table[i] = 0;
        else
            sqr_table[i] = 2048;
    }
}

int main() {
    //host_serial_init();
    gpio_config_pullup(A3, PULL_OFF);
    gpio_config_mode(A3, ANALOG);
    gpio_config_pullup(D12, PULL_UP);
    gpio_config_mode(D12, INPUT);
    gpio_config_pullup(A2, PULL_UP);
    gpio_config_mode(A2, INPUT);
    gpio_config_pullup(A7, PULL_UP);
    gpio_config_mode(A7, INPUT);
    gpio_config_pullup(A1, PULL_UP);
    gpio_config_mode(A1, INPUT);
    gpio_config_pullup(A4, PULL_UP);
    gpio_config_mode(A4, INPUT);
    gpio_config_pullup(A5, PULL_UP);
    gpio_config_mode(A5, INPUT);
    gpio_config_pullup(A6, PULL_UP);
    gpio_config_mode(A6, INPUT);
    config_gpio_interrupt();
    SysTick_initialize();
    RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN; //dac clock enable
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
    sin_table_init();
    sqr_table_init();
    DAC_init();
    classical();
    //phase_increment = 1 << 24;
    // while (true) {
    //     phase += phase_increment;
    //     uint8_t index = (phase >> 24) & 0xFF;  // Take top bits for table index
    //     DAC1->DHR12R1 = sin_table[index];
    // }

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
