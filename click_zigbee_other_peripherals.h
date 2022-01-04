#pragma once

#include "click_zigbee_types.h"

//void zigbee_init_uart_isr()
//{
//    RXNEIE_USART1_CR1_bit = 1;
//    NVIC_IntEnable(IVT_INT_USART1);
//    EnableInterrupts();
//}
//
//void UART_RX_ISR() iv IVT_INT_USART1 ics ICS_AUTO
//{
//    char tmp;
//
//    if (RXNE_USART1_SR_bit)
//    {
//        tmp = USART1_DR;
//        zigbee_process(tmp);
//    }
//}
