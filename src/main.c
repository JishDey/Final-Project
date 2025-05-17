#include <stm32l432xx.h>
#include "ee14lib.h"
#include <math.h>
#define MAXWAVENUM 4

int _write(int file, char *data, int len){
    serial_write(USART2, data, len);
    return len;
}


uint32_t phase = 0;
uint32_t phase_increment = 0;
int8_t octave = 1;
uint8_t wave_number = 0;


signed int wave_tables[256][MAXWAVENUM];
signed int sqr_table[256];

void SysTick_Handler(void)
{
    phase += phase_increment;
    uint8_t index = (phase >> 24) & 0xFF;  // Take top bits for table index
    DAC1->DHR12R1 = wave_tables[index][wave_number];
}

void config_gpio_interrupt(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    // Port D12
    SYSCFG->EXTICR[1] &= 0x0;
    SYSCFG->EXTICR[1] |= 0x1;
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
    // Port D1
    SYSCFG->EXTICR[1] &= ~(0xF << 4);
    EXTI->FTSR1 |= EXTI_FTSR1_FT9;
    EXTI->IMR1 |= EXTI_IMR1_IM9;
    // Port D0
    SYSCFG->EXTICR[2] &= ~(0xF << 8);
    EXTI->FTSR1 |= EXTI_FTSR1_FT10;
    EXTI->IMR1 |= EXTI_IMR1_IM10;
    // Port D10
    SYSCFG->EXTICR[2] &= ~(0xF << 12);
    EXTI->FTSR1 |= EXTI_FTSR1_FT11;
    EXTI->IMR1 |= EXTI_IMR1_IM11;

    NVIC_SetPriority(EXTI15_10_IRQn, 3);
    NVIC_EnableIRQ(EXTI15_10_IRQn);

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
#define note6 295
#define note5 328
#define note4 349
#define note3 393
#define note2 437
#define note1 491

uint32_t pitch(int freq)
{
    return ((uint64_t)16777216 * freq) / 95;
}


void play(int freq, int length) {
    phase_increment = pitch(freq);
    delay_ms(200);
    phase_increment = 0;
}

void EXTI4_IRQHandler(void)
{
    if (EXTI->PR1 & EXTI_PR1_PIF4) {
        EXTI->PR1 |= EXTI_PR1_PIF4;
        const float base_freq = note7;
        while (!gpio_read(D12)) {
            uint16_t adc_val = adc_read_single();  // 0–63
            float n = ((float)adc_val / 63) * 2.0f - 1.0f;  // n in [-1, +1]
            float multiplier = powf(2.0f, n / 12.0f);
            phase_increment = pitch(base_freq * multiplier) << octave; 
        }
        phase_increment = 0;
    }
}

void EXTI3_IRQHandler(void)
{
    if (EXTI->PR1 & EXTI_PR1_PIF3) {
        EXTI->PR1 |= EXTI_PR1_PIF3;
        const float base_freq = note1;
        while (!gpio_read(A2)) {
            uint16_t adc_val = adc_read_single();  // 0–63
            float n = ((float)adc_val / 63) * 2.0f - 1.0f;  // n in [-1, +1]
            float multiplier = powf(2.0f, n / 12.0f);
            phase_increment = pitch(base_freq * multiplier) << octave; 
        }
        phase_increment = 0;
    }
}

void EXTI2_IRQHandler(void)
{
    if (EXTI->PR1 & EXTI_PR1_PIF2) {
        EXTI->PR1 |= EXTI_PR1_PIF2;
        const float base_freq = note6;
        while (!gpio_read(A7)) {
            uint16_t adc_val = adc_read_single();  // 0–63
            float n = ((float)adc_val / 63) * 2.0f - 1.0f;  // n in [-1, +1]
            float multiplier = powf(2.0f, n / 12.0f);
            phase_increment = pitch(base_freq * multiplier) << octave; 
        }
        phase_increment = 0;
    }
}

void EXTI1_IRQHandler(void)
{
    if (EXTI->PR1 & EXTI_PR1_PIF1) {
        EXTI->PR1 |= EXTI_PR1_PIF1;
        const float base_freq = note5;
        while (!gpio_read(A1)) {
            uint16_t adc_val = adc_read_single();  // 0–63
            float n = ((float)adc_val / 63) * 2.0f - 1.0f;  // n in [-1, +1]
            float multiplier = powf(2.0f, n / 12.0f);
            phase_increment = pitch(base_freq * multiplier) << octave; 
        }
        phase_increment = 0;
    }
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI->PR1 & EXTI_PR1_PIF5) {
        EXTI->PR1 |= EXTI_PR1_PIF5;
        const float base_freq = note4;
        while (!gpio_read(A4)) {
            uint16_t adc_val = adc_read_single();  // 0–63
            float n = ((float)adc_val / 63) * 2.0f - 1.0f;  // n in [-1, +1]
            float multiplier = powf(2.0f, n / 12.0f);
            phase_increment = pitch(base_freq * multiplier) << octave; 
        }
        phase_increment = 0;
    }
    if (EXTI->PR1 & EXTI_PR1_PIF6) {
        EXTI->PR1 |= EXTI_PR1_PIF6;
        const float base_freq = note3;
        while (!gpio_read(A5)) {
            uint16_t adc_val = adc_read_single();  // 0–63
            float n = ((float)adc_val / 63) * 2.0f - 1.0f;  // n in [-1, +1]
            float multiplier = powf(2.0f, n / 12.0f);
            phase_increment = pitch(base_freq * multiplier) << octave; 
        }
        phase_increment = 0;
    }
    if (EXTI->PR1 & EXTI_PR1_PIF7) {
        EXTI->PR1 |= EXTI_PR1_PIF7;
        const float base_freq = note2;
        while (!gpio_read(A6)) {
            uint16_t adc_val = adc_read_single();  // 0–63
            float n = ((float)adc_val / 63) * 2.0f - 1.0f;  // n in [-1, +1]
            float multiplier = powf(2.0f, n / 12.0f);
            phase_increment = pitch(base_freq * multiplier) << octave; 
        }
        phase_increment = 0;
    }

    if (EXTI->PR1 & EXTI_PR1_PIF9) {
        EXTI->PR1 |= EXTI_PR1_PIF9;
        if (!gpio_read(D1)) {
            wave_number++;
            if(wave_number >= MAXWAVENUM){
                wave_number = 0;
            }
        }
        phase_increment = 0;
    }
}

void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR1 & EXTI_PR1_PIF10) {
        EXTI->PR1 |= EXTI_PR1_PIF10;
        for (volatile int i = 0; i < 20000; i++) { } //debounce
        if(!gpio_read(D0)){
            octave--;
            if(octave < 0){
                octave = 3;
            }
        }
    }

    if (EXTI->PR1 & EXTI_PR1_PIF11) {
        EXTI->PR1 |= EXTI_PR1_PIF11;
        for (volatile int i = 0; i < 20000; i++) { } //debounce
        if(!gpio_read(D10)){
            octave++;
            if(octave >= 4){
                octave = 0;
            }
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

void sin_table_init() {
    float sf;
    for (int i = 0; i < 256; i++){
        sf = cos(3.1415926 * ((float)i / 128));
        wave_tables[i][0] = (1+sf) * 2000;
        if(wave_tables[i][0] >= 0x1000){
            wave_tables[i][0] = 0xFFF;
        }
    }
}

void sqr_table_init() {
    for (int i = 0; i < 256; i++){
        if(i < 128)
            wave_tables[i][1] = 0;
        else
            wave_tables[i][1] = 1400;
    }
}

void tng_table_init() {
    for (int i = 0; i < 128; i++){
        wave_tables[i][2] = i*15;
    }
    for (int i = 127; i >= 0; i--){
        wave_tables[i+128][2] = 1920-i*15;
    }
}

void czy_table_init() {
    for (int i = 0; i < 256; i++){
        if(i < 128)
            wave_tables[i][3] = 0;
        else
            wave_tables[i][3] = 700;
    }
    float sf;
    for (int i = 0; i < 256; i++){
        sf = cos(3.1415926 * ((float)i / 64));
        wave_tables[i][3] += (1+sf) * 1200;
        if(wave_tables[i][3] >= 0x1000){
            wave_tables[i][3] = 0xFFF;
        }
    }
}

int main() {
    host_serial_init();
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
    gpio_config_pullup(D1, PULL_UP);
    gpio_config_mode(D1, INPUT);
    gpio_config_pullup(D0, PULL_UP);
    gpio_config_mode(D0, INPUT);
    gpio_config_pullup(D10, PULL_UP);
    gpio_config_mode(D10, INPUT);
    config_gpio_interrupt();
    SysTick_initialize();
    RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN; //dac clock enable
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
    sin_table_init();
    sqr_table_init();
    tng_table_init();
    czy_table_init();
    adc_config_single(D3); //placed here so that we can measure the read later
    DAC_init();
    //adc_config_single(D6);
    // gpio_write(D4, 0);
    // while(1){
    //     adc_read_single(); //commented out to measure only the write times
    //                        //replaced with adc_config_single to measure the config time
    //     printf("pause\n"); // this is the easiest operation that takes 
    //                        //a long time, which makes it easier for the oscilliscope to parse
    // }
    while(1){ }

}
