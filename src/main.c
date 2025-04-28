#include <stm32l432xx.h>
#include "ee14lib.h"

int _write(int file, char *data, int len){
    serial_write(USART2, data, len);
    return len;
}

int main() {
    host_serial_init();
    gpio_config_pullup(A3, PULL_OFF);
    gpio_config_mode(A3, ANALOG);
    DAC1->CR = 0x0; //reset DAC control register, just in case.
    RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN; //dac clock enable - not sure if we need this
    DAC1->CR |= (0b000 << DAC_CR_TSEL1_Pos); //set trigger source tim6. see 17.4.6 for bit codes
    DAC1->CR |= DAC_CR_TEN1; //enable the trigger source
    DAC1->CR |= (0b10 << DAC_CR_WAVE1_Pos); //enables the triangle wave
    DAC1->CR |= (0b1011 << DAC_CR_MAMP1_Pos); //triangle wave max amplitude ( >= 1011)
    
    
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
