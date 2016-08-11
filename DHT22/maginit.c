/*================ Это не функция, идёт как вставка ТЕКСТА !!! =================*/
//SetSysClockToHSE();
/* Разрешаем тактирование, эта строка должна быть первой, т.к. сбрасываются настройки */
RCC_ADCCLKConfig(RCC_PCLK2_Div2);
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // для PA15 (JTAG)
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);

/*RCC_ClocksTypeDef RCC_Clocks;
RCC_GetClocksFreq(&RCC_Clocks);
SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000000);*/


/* JTAG-DP Disabled and SW-DP Enabled */
GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

Init_PORTA.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
Init_PORTA.GPIO_Speed = GPIO_Speed_10MHz;
Init_PORTA.GPIO_Mode = GPIO_Mode_IN_FLOATING;
GPIO_Init(GPIOA, &Init_PORTA);

/*====================== USART1 ======================*/
Init_PORTA.GPIO_Pin = GPIO_Pin_10; // USART1 Rx PA10
Init_PORTA.GPIO_Speed = GPIO_Speed_10MHz;
Init_PORTA.GPIO_Mode = GPIO_Mode_IPU; // GPIO_Mode_IN_FLOATING; - было
GPIO_Init(GPIOA, &Init_PORTA);

Init_PORTA.GPIO_Pin = GPIO_Pin_9; // USART1 Tx PA9
Init_PORTA.GPIO_Speed = GPIO_Speed_10MHz;
Init_PORTA.GPIO_Mode = GPIO_Mode_AF_PP; // Alternate function push-pull
GPIO_Init(GPIOA, &Init_PORTA);

USART_InitStructure.USART_BaudRate = 9600;
USART_InitStructure.USART_WordLength = USART_WordLength_8b;
USART_InitStructure.USART_StopBits = USART_StopBits_1;
USART_InitStructure.USART_Parity = USART_Parity_No;
USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

USART_Init(USART1, &USART_InitStructure);

/*==================== TIM2-TIM3 ===================*/
TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
TIM_TimeBaseStructure.TIM_Prescaler = 80 - 1; // квант = 10 мкс
TIM_TimeBaseStructure.TIM_ClockDivision = 0;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
TIM_ARRPreloadConfig(TIM3, ENABLE);
/* Выбираем вход триггера от TIM3 (ITR2) */
TIM_SelectInputTrigger(TIM2, TIM_TS_ITR2);
/* Включаем тактирование от внешнего источника. 
Теперь TIM2 тактируется по ITR2. */
TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);
TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_External1);
/* Выходной триггер будет срабатывать по переполнению */
TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
/* Обнуляем оба счётчика ( или один 32-битный - кому как :) */
TIM_SetCounter(TIM2, 0); TIM_SetCounter(TIM3, 0);

/*====================== DMA1 ======================*/
DMA_DeInit(DMA1_Channel1);
DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &ADC_ConvertedValueTab[0];
DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
DMA_InitStructure.DMA_BufferSize = 2;
DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
DMA_InitStructure.DMA_Priority = DMA_Priority_High; // Medium
DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
DMA_Init(DMA1_Channel1, &DMA_InitStructure);
DMA_Cmd(DMA1_Channel1, ENABLE);

/*====================== ADC1 ======================*/
ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
ADC_InitStructure.ADC_ScanConvMode = ENABLE; // Многоканальный режим
ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // Авт.перезапуск
ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
ADC_InitStructure.ADC_NbrOfChannel = 2;
ADC_Init(ADC1, &ADC_InitStructure);

ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);
ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);

ADC_DMACmd(ADC1, ENABLE); // Enable ADC1 DMA
ADC_Cmd(ADC1, ENABLE); // Enable ADC1
ADC_ResetCalibration(ADC1); // Enable ADC1 reset calibration register
// Check the end of ADC1 reset calibration register
while(ADC_GetResetCalibrationStatus(ADC1));
ADC_StartCalibration(ADC1); // Start ADC1 calibration
while(ADC_GetCalibrationStatus(ADC1)); // Check the end of ADC1 calibration
ADC_SoftwareStartConvCmd(ADC1, ENABLE); // Start ADC1 Software Conversion

/*====================== NVIC ======================*/
NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);

//TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
TIM_Cmd(TIM2, ENABLE); /* TIM enable counter */
TIM_Cmd(TIM3, ENABLE);

USART_Cmd(USART1, ENABLE);
// КОНЕЦ
