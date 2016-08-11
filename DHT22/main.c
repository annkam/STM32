/* ���� ��������� ��������: 6 ������� 2015 ����
��������: ����
�����: STM32VLDISCOVERY
���������������: STM32F100C4T6B
���� ����������������: ��
��������: ������ ������� �����������, trubin@ngs.ru
�������� �� ����� ������ ������� ������ ! http://radiopotok.ru/radio/499/

MPU6050:SDA - PB7, MPU6050:SCL - PB6, MPU6050:VCC +5�, MPU6050:GND �� �����.
�� ����� MPU6050 ���� ������������ �� +3,3 �, ������� SDA, SCL +3,3 � �� ������.
������������� ��������� ��� SDA, SCL ���� �� ����� MPU6050

============ ����� �������� ============
PA0 ������ �� DISCOVERY, 0 = �� ������. 

============ ����� ���������� ============

============ ������ �������� ============
PC8 ����� ��������� �� DISCOVERY, 1 = �����
PC9 ������ ��������� �� DISCOVERY, 1 = �����

============ ��������� ============
PA9 = TxD, PA10 = RxD USART1, ����� � �����������

PB6 - I2C:SCL
PB7 - I2C:SDA

��� ����������� ���������� � ����� stm32f10x.h � ������ 67:
#define STM32F10X_LD_VL  // STM32F10X_LD_VL: STM32 Low density Value Line devices

���������� ������� ����� �� 8 ���, ����������������� ������
#define SYSCLK_FREQ_HSE    HSE_VALUE
� ����� system_stm32f10x.c ������ 107
*/                       
// <...> ���� � ����������� ��������� ��� �������� ������������ �����
//#include <stm32f10x.h>
#include "dht22.h"

#define Blue GPIO_Pin_8 // PORTC
#define Green GPIO_Pin_9 // PORTC
#define ADC1_DR_Address ((uint32_t)0x4001244C)

//============= ���������� ���������� ===============
GPIO_InitTypeDef	Init_PORTB, Init_PORTA, Init_PORTC;
USART_InitTypeDef 	USART_InitStructure;
NVIC_InitTypeDef  	NVIC_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
ErrorStatus         HSEStartUpStatus;
ADC_InitTypeDef     ADC_InitStructure;
DMA_InitTypeDef     DMA_InitStructure;
__IO uint16_t       ADC_ConvertedValueTab[2]; // ���������� ���������

#include "mafunc.c" // ���� � ������� �������� ��-�� ������� �������

/*================== ���������� ���������� =============*/
uint16_t check;
uint8_t mass[6];
uint16_t humidity, temperature;
uint8_t error[] = "Error\r\0";
uint8_t hum[] = "Humidity: \0";
uint8_t temp[] = "Temperature: \0";


// --- !!! �� ������������ � ���� ������ �� !!! ---
//============== TIM2 ==============
void TIM2_IRQHandler(void) { // ���������� ������ ���� ����� ������� !!!
static uint8_t i = 0;
//================= TIM2:������ ====================
if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // Clear TIM2 update interrupt
  if (i) { i = 0; GPIO_ResetBits(GPIOC, Green); }
  else { i = 1; GPIO_SetBits(GPIOC, Green); }
}
}

/*====================== MAIN ====================*/
int main(void)
{
#include "maginit.c"
DHT22_Init();
Pause();
//============== MAIN LOOP ====================
while (1) {
	check = DHT22_GetReadings();
	switch(check) {
		case 0 : Tx('0'); break;
		case 1 : Tx('1'); break;
		case 2 : Tx('2'); break;
		case 3 : Tx('3'); break;
		case 4 : Tx('4'); break;
	}
	check = DHT22_DecodeReadings();

	humidity = DHT22_GetHumidity();
	temperature = DHT22_GetTemperature();
Uint16ToStr(humidity, &mass[0]);
TxStr(&hum[0]);
TxStr(&mass[2]); Tx(' ');
Uint16ToStr(temperature, &mass[0]); 
TxStr(&temp[0]);
TxStr(&mass[2]); Tx('\r');	
Pause();
	
}

}

/* ����� */
