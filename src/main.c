#include "main.h"
#include <stdio.h>

typedef enum
{
    false = 0,
    true = 1
} bool;

bool servoTurned = false;

int main(void) {

    Delay_Init();

    RCC->CTLR = ((uint32_t)0x00);
    RCC->CTLR |= ((uint32_t)0x01) | (((uint32_t)0x01)<<24); // HSI & PLL ON
    RCC->CFGR0 = ((uint32_t)0x00);
    RCC->APB2PCENR = ((uint32_t)0x00);
    RCC->APB2PCENR |= ((uint32_t)0x01) | (((uint32_t)0x01)<<4) | ((uint32_t)0x01<<5) | (((uint32_t)0x01)<<9) 
                    | (((uint32_t)0x01)<<11) | (((uint32_t)0x01)<<14); // Clock for AFIO | PORTC | PORTD | ADC1 | TIM1 | USART1
	RCC->APB1PRSTR |= RCC_APB1Periph_TIM2;
	RCC->APB1PRSTR &= ~RCC_APB1Periph_TIM2;
    RCC->APB1PCENR |= (((uint32_t)0x01)<<28); // Clock for PVD, important to activate before configuring the PWR register
    RCC->APB1PCENR |= ((uint32_t)0x01); // Clock for TIM2
    RCC->RSTSCKR = ((uint32_t)0x01); // Only the first bit of the register is set, LSI enabled

    PWR->CTLR = ((uint32_t)0x00);
    PWR->CTLR |= (((uint32_t)0x07)<<5) | ((uint16_t)0x0010); // PLS: 111, PVD enabled

    GPIOC->CFGLR = ((uint32_t)0x00);
    GPIOC->CFGLR |= (((uint32_t)0x03)<<16); // LED
    GPIOC->CFGLR |= (((uint32_t)0x02)<<14); // pushbutton


    GPIOD->CFGLR &= ~(0xf<<(4*4));
	GPIOD->CFGLR |= (((uint32_t)0x0B)<<16); // PP_AF @ 30MHz

    AFIO->PCFR1 = ((uint32_t)0x00); // Default pin mapping

    TIM2->CTLR1 = ((uint16_t)0x00);
    TIM2->CTLR1 |= ((uint16_t)0x01) | ((uint16_t)0x01<<7);
    TIM2->CTLR2 = ((uint16_t)0x00);
    TIM2->CTLR2 |= (((uint16_t)0x04)<<4); // Questionable, I don't know how this should be set up yet
    TIM2->SMCFGR = ((uint16_t)0x00);
    TIM2->SWEVGR |= TIM_UG;
    TIM2->CHCTLR1 = ((uint16_t)0x00);
    TIM2->CHCTLR1 |= TIM_OC1M_2 | TIM_OC1M_1 | TIM_OC1PE;
    TIM2->CCER |= TIM_CC1E | (TIM_CC1P & 0xff);
    TIM2->PSC = ((uint16_t)0x00);
    TIM2->ATRLR = (uint32_t)479999; // 20ms
    TIM2->CH1CVR = (uint32_t)455999; // 1ms high at the end of every 20ms cycle
    
    

    while(1) 
    {
        if ((GPIOC->INDR & ((uint32_t)0x08))) 
        {
            GPIOC->BSHR = GPIO_Pin_4;
            toggle_servo_pos();
            Delay_Ms(100);
        }
        else GPIOC->BCR = GPIO_Pin_4;

        if ((PWR->CSR & ((uint16_t)0x0004)) != 0) warning_blink(); // if PVD0 flag is set - blink the LED*/

    }
}
// 1 tick takes 4.1666666667e-8 seconds
//480000 ticks for timer2 to have a period of 20ms (Hz)

void toggle_servo_pos(void)
{
    if (servoTurned == false) 
    {
        TIM2->CH1CVR = 23999u;
        servoTurned = true;
    }
    else 
    {
        TIM2->CH1CVR = 47999u;
        servoTurned = false;
    }
    return;
}


void warning_blink(void)
{
    GPIOC->BSHR = GPIO_Pin_4;
    Delay_Ms(250);
    GPIOC->BCR = GPIO_Pin_4;
    Delay_Ms(250);
    GPIOC->BSHR = GPIO_Pin_4;
    Delay_Ms(250);
    GPIOC->BCR = GPIO_Pin_4;
    Delay_Ms(250);
    GPIOC->BSHR = GPIO_Pin_4;
    Delay_Ms(250);
    GPIOC->BCR = GPIO_Pin_4;
    Delay_Ms(500);


}

