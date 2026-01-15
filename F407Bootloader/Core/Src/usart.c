/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "crc.h"
#include <string.h>
#include <stdlib.h>

/* USER CODE BEGIN 0 */
//����
uint8_t  Uart1_RX_Buf[UART1_RX_BUF_LEN];
uint16_t Uart1_RX_Len = 0;  // �������ݳ���    ��û�ã�
uint8_t  Uart1_RX_Flag = 0; // ������ɱ�־λ			��û�ã�
//����
uint8_t  Uart1_TX_Flag = 0; // ������ɱ�־λ
uint8_t  Uart1_TX_Busy = 0; // ����æ��־λ

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 19200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {

  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA2_Stream2;
    hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {

    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA2_Stream7;
    hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {

    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}


void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//������ȫ���ͻص�����
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart == &huart1)
    {
        uart1_tx_flag = 1;  // ��λ������ɱ�־λ��main�����ɼ��
        uart1_tx_busy = 0;  // ������ɣ��������æ��־λ
    }
}
//���ڿ��лص��������ӽ���ת������ʱ��
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	uitn8_t Crcl = 0,Crch = 0;
	CRC_USER Crc;
    if(huart == &huart1)
    {
        if(huart->RxEventType == HAL_UART_RXEVENT_IDLE)
        {
        	Crc = CalculateCRC(Uart1_RX_Buf,Size-2);
        /����������������������������������������
            Uart1_RX_Len = Size;
            Uart1_RX_Flag = 1; //�������ݳɹ�
		/����������������������������������������
			if(Uart1_RX_Buf[0] == 0x02)
			{
				Crcl = Uart1_RX_Buf[Size - 2];
				Crch = Uart1_RX_Buf[Size - 1];
				if(Crc.CRCH == Crch && Crc.CRCL == Crcl)
				{
					if(Uart1_RX_Buf[2] == (Size-4))
					{
						memcpy(&FirmwareBuff[ReIndex],&Uart1_RX_Buf[2],Size-4);
						FirmwareSize += (Size-4);
						ReIndex = FirmwareSize-1;
						DataAcquired = 1;
					}
				}
			}
			else if (uart1_rx_buf[0] == 0x03)
			{
				Crcl = Uart1_RX_Buf[Size - 2];
				Crch = Uart1_RX_Buf[Size - 1];
				FirmwareSizeFinally = *(uint32_t *)&Uart1_RX_Buf[5];;
				if(Crc.CRCH == Crch && Crc.CRCL == Crcl)
				{
					if(FirmwareSizeFinally == FirmwareSize)
					{
						FirmwareFlag = *(uint32_t *)&Uart1_RX_Buf[1];
						FirmwareCRC = *(uint16_t *)&Uart1_RX_Buf[9];
						FirmwareVersion = *(uint16_t *)&Uart1_RX_Buf[11];
						DataAcquired = 1;
					}

				}
			}
			hdma_usart1_rx.Instance->NDTR = UART1_RX_BUF_LEN;
			
            HAL_UARTEx_ReceiveToIdle_DMA(&huart1, Uart1_RX_Buf, UART1_RX_BUF_LEN);
            
            __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
        }
    }

}


uint8_t RequestPackage(uint8_t PackageIndex)
{
	CRC_USER Crc;
	const uint8_t  RequesData[4] = {0x01, PackageIndex,}; // DMA���ͻ�����
	Crc = CalculateCRC(RequesData,2);
	RequesData[2] = Crc.CRCL;
	RequesData[3] = Crc.CRCH;
	HAL_UART_Transmit_DMA(&huart1, RequesData, sizeof(RequesData));
	Uart1_TX_Flag = 0;               // ������ɱ�־λ
	Uart1_TX_Busy = 1;               // ����æ��־λ
}

/* USER CODE END 1 */
