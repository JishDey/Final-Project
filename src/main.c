#include <stm32l432xx.h>
#include "ee14lib.h"
#include <math.h>

int _write(int file, char *data, int len){
    serial_write(USART2, data, len);
    return len;
}

void DAC_init(void) {
    DAC1->CR = 0x0; //reset, just in case.
    DAC1->CR |= 0x1; // Enabling DAC1
    DAC1->CR &= ~DAC_CR_TEN1; //disable the trigger source
    DAC1->CR |= (0b00 << DAC_CR_WAVE1_Pos); //disables wave gen
    DAC1->CR |= (0b0111 << DAC_CR_MAMP1_Pos); //triangle wave max amplitude ( >= 1011)
}

int main() {
    host_serial_init();
    SysTick_initialize();
    gpio_config_pullup(A3, PULL_OFF);
    gpio_config_mode(A3, ANALOG);
    RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN; //dac clock enable
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
    signed int sin_table[180];
    float sf;
    DAC_init();
    for (int i = 0; i < 180; i++){
        sf = sin(3.1415926 * (i + 90)/180);
        sin_table[i] = (1+sf) * 2048;
        if(sin_table[i] == 0x1000){
            sin_table[i] = 0xFFF;
        }
    }
    int i = 0;
    while (true) {
        i++;
        DAC1->DHR12R1 = sin_table[i];
        for (volatile int j = 0; j < 5; j++){}
        if (i == 180) {
            while (i > 0) {
                DAC1->DHR12R1 = sin_table[i];
                i--;
                for (volatile int j = 0; j < 5; j++){}
            }
        }
    }
    // DAC1->CR &= ~(DAC_CR_WAVE1 | DAC_CR_MAMP1 | DAC_CR_TSEL1); // Clear WAVE, MAMP, TSEL
    // DAC1->CR |= DAC_CR_WAVE1_1;          // Enable triangle wave generation (WAVE1 = 0b10)
    // DAC1->CR |= (0b1000 << DAC_CR_MAMP1_Pos); // Max amplitude (MAMP1 = 0x7 → 4095 steps)
    
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
