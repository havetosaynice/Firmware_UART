
#include "gpio.h"


void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : ERROR_LED_Pin */
  GPIO_InitStruct.Pin = ERROR_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ERROR_LED_GPIO_Port, &GPIO_InitStruct);

}

void errLED(uint8_t count)
{
    for(uint8_t i = 0; i < count; i++)
    {
        HAL_GPIO_TogglePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin);
        HAL_Delay(300);
    }
    HAL_Delay(1000);
}

void errON()
{
    HAL_GPIO_WritePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin, GPIO_PIN_RESET);
}

void errOFF()
{
    HAL_GPIO_WritePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin, GPIO_PIN_SET);
}
