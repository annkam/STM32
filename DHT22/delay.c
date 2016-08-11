#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include "delay.h"

volatile uint32_t Time32, T1, T2;


//============= Функция чтения текущего времени ===============
// Переполнение через 11,93 часа при шаге по времени 10 мкс
uint32_t GetTime32() {
volatile static uint32_t i; 
  do { i = TIM_GetCounter(TIM2); Time32 = TIM_GetCounter(TIM3); 
     } while (TIM_GetCounter(TIM2) != i);
  Time32 &= 0xFFFF; i <<= 16; Time32 |= i;
return(Time32); }
  
// Функция задержки на базе TIMER2
void PauseTim2(uint16_t Tik) { // Tik - требуемая задержка в тиках
static volatile uint32_t X1, X2;
static volatile int32_t dT; // Именно int !!!
  X2 = GetTime32(); X2 += (uint32_t)Tik;
  do { X1 = GetTime32(); dT = X2 - X1; } while (X1 < X2);
}

// SysTick interrupt handler
void SysTick_Handler(void) {
	if (TimingDelay != 0) { TimingDelay--; }
}

void Delay_us(__IO uint32_t uSecs) {
	SysTick_Config(SystemCoreClock / 1000000);
	TimingDelay = uSecs+1;
	while (TimingDelay != 0) continue;
}