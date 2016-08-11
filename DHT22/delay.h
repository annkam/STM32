
static __IO uint32_t TimingDelay; // __IO -- volatile


/*
 *   Declare Functions
 */
uint32_t GetTime32();
void PauseTim2(uint16_t Tik);
void Delay_us(__IO uint32_t uSecs);