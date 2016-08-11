// Функция преобразования беззнакового 16 битного числа в строку
void Uint16ToStr(uint16_t Number, uint8_t *pStr) {
static int8_t i;
for(i = 4; i != -1; i--) { pStr[i] = (Number % 10) + 0x30; Number /= 10; }
pStr[5] = 0; // конец строки
}

// Функция преобразования знакового 16 битного числа в строку
void Int16ToStr(int16_t Number, uint8_t *pStr) {
static int8_t i;
if (Number >=0) pStr[0] = '+'; 
  else { pStr[0] = '-'; Number = -Number; }
for(i = 5; i != 0; i--) { pStr[i] = (Number % 10) + 0x30; Number /= 10; }
pStr[6] = 0; // конец строки
}

// Функция передачи символа по USART1
void Tx(uint8_t Symbol) {
while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}; 
USART_SendData(USART1, Symbol);
}

// Функция ожидания и приёма символа по USART1
uint8_t Rx() {
while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) {}; 
return USART_ReceiveData(USART1);
}

// Функция передачи массива символов (строки) по USART1
// пока не встретится "0" байт
void TxStr(uint8_t *pStr) {
static uint8_t i;
i = 0; do Tx( pStr[i++] ); while (pStr[i] != 0);
}

// Функция передачи массива символов (строки) CONST по USART1
// пока не встретится "0" байт
void TxStrConst(const uint8_t *pStr) {
static uint8_t i;
i = 0; do Tx( pStr[i++] ); while (pStr[i] != 0);
}

// Функция программной задержки
void Pause() {
static volatile uint32_t i;
for(i = 0; i < 2000000; i++) {};
}

//================== Переключение на внешний генератор =======================
void SetSysClockToHSE(void)
{
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/   
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();
  RCC_HSEConfig(RCC_HSE_ON); /* Enable HSE */
  HSEStartUpStatus = RCC_WaitForHSEStartUp(); /* Wait till HSE is ready */
  if (HSEStartUpStatus == SUCCESS)
  {
    FLASH_SetLatency(FLASH_Latency_0); /* Flash 0 wait state */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); /* HCLK = SYSCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); /* PCLK2 = HCLK */
    RCC_PCLK1Config(RCC_HCLK_Div1); /* PCLK1 = HCLK */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE); /* Select HSE as system clock source */
    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x04)  {}
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock configuration.
       User can add here some code to deal with this error */    
    while (1) { } /* Go to infinite loop */
  }
}

// КОНЕЦ
