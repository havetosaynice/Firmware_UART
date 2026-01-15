#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"
#include "crc.h"
#include "flash.h"

typedef uint32_t (*pFunction)(void);

uint8_t NeedUpdataFlag = 0;     //更新标志



uint8_t DataAcquired = 0;   //获取到数据

//我来加的
uint32_t FirmwareFlag = 0;//固件标志
//固件大小
uint32_t FirmwareSize = 0;//接收包时候实时变化
uint32_t FirmwareSizeFinally = 0;//最终接收包大小
uint16_t FirmwareCRC = 0; //固件CRC值
uint16_t FirmwareVersion = 0; //固件版本号
uint16_t ReIndex = 0;//这是为索引



uint8_t FirmwareBuff[LENGHTMAX];
uint32_t WriteAddr = 0; 
uint32_t WriteFlashSize = 0;


uint8_t DownloadFlag = 0;

BL_State_t bl_state = BL_STATE_WAIT;
/**
 * @brief  MCU 软件复位函数
 * @note   通过 NVIC 触发系统复位，等效于硬件复位
 *         复位后程序将从复位向量（Reset_Handler）重新执行
 */
void MCU_SoftwareReset(void)
{
    __disable_irq();        // 关闭全局中断，防止复位过程中被中断打断
    NVIC_SystemReset();     // 触发 NVIC 系统复位（写 AIRCR 寄存器）
    while (1);              
}

/**
 * @brief  固件完整性校验函数（CRC 校验）
 *
 * @note   该函数用于对用户 APP 固件进行 CRC 校验，
 *         以判断 Flash 中的固件内容是否完整、未被破坏。
 *
 * @return 1：固件校验通过
 *         0：固件校验失败
 */
uint8_t VerifyFirmware(void)
{
    CRC_USER crc;
    FirmwareHeader *hdr = (FirmwareHeader *)FIRMWAREHEADERADDR;
    
    crc = CalculateCRC((uint8_t *)APP_ADDR,hdr->size);
    if (crc.CRCL == (uint8_t)(hdr->crc & 0xFF) &&
        crc.CRCH == (uint8_t)(hdr->crc >> 8))
    {
        return 1;   // 固件有效
    }
    else
    {
        return 0;   // 固件损坏
    }
}

/**
 * @brief  跳转到用户 APP 程序
 *
 * @note   该函数用于从 Bootloader 跳转到用户 APP，
 *         会完成以下关键操作：
 *         1. 校验 APP 初始栈指针是否合法
 *         2. 关闭全局中断及 SysTick
 *         3. 关闭并清除 NVIC 中所有中断
 *         4. 重定位中断向量表到 APP 区域
 *         5. 设置主栈指针（MSP）
 *         6. 跳转至 APP 的 Reset_Handler
 */
void JumpToApp(void)
{
    uint32_t appStack = *(uint32_t *)APP_ADDR;
    uint32_t appReset = *(uint32_t *)(APP_ADDR + 4);
    if ((appStack & 0x2FFE0000) != 0x20000000)
        return;
    __disable_irq();
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;
    for (uint32_t i = 0; i < 8; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFF;   
        NVIC->ICPR[i] = 0xFFFFFFFF;  
    }
    SCB->VTOR = APP_ADDR;
    __set_MSP(appStack);
    ((pFunction)appReset)();
    while (1);
}

/**
 * @brief  固件升级处理函数                    
 *
 * @note   该函数用于在升级模式下接收上位机发送的固件数据，
 *         并在接收过程中将数据写入 Flash。
 *         通过超时机制判断升级是否结束。
 *
 * @return UPGRADE_OK   ：升级进行中或正常结束
 *         UPGRADE_FAIL  ：升级超时结束
 */
uint8_t UpdataFirmware(void)
{

    static uint32_t startTick;
    while (NeedUpdataFlag)
    {
        if (DataAcquired)
        {
            DataAcquired = 0;
            if(WriteFirmwareToFlash(FirmwareBuff,&WriteAddr,WriteFlashSize) != 0)
            {
                  return UPGRADE_FAIL;
            }
            startTick = HAL_GetTick();
        }
    }
    if (HAL_GetTick() - startTick > 2000)
    {
        NeedUpdataFlag = 0;
        return UPGRADE_END;
    }
    return UPGRADE_OK;
}


uint8_t WaitingForDownloadSignal()
{
  if(DownloadFlag == 1)
  {
      return 1;
  }
  else
  {
      return 0;
  }
}

int main(void)
{
  HAL_Init();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  errON();
  
  //加入的
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, Uart1_RX_Buf, UART1_RX_BUF_LEN);
  __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
  
  uint8_t packageIndex = 0;
  uint8_t errorCount = 0;
  uint32_t power_on_tick = HAL_GetTick();
  uint8_t flag;
  while (1)
  {
      switch (bl_state)
      {
      case BL_STATE_WAIT:
        {
              if (NeedUpdataFlag)
              {     
                  if(WaitingForDownloadSignal())
                  {
                      packageIndex = 0;
                      errorCount = 0;
                      bl_state = BL_STATE_UPGRADING;
                      BSP_Flash_Erase(APP_START_SECTOR, APP_SECTOR_COUNT);
                      //RequestPackage(packageIndex);     //请求第一包数据
                  }
                  else
                  {
                      errLED(3);       //固件校验失败，闪灯3下
                  }
              }
              else
              {
                  if(VerifyFirmware())
                  {
                      errOFF();
                      JumpToApp();    //跳到Firmware
                  }
                  else
                  {
                      NeedUpdataFlag = 1;
                  }
              }
              break;
        }
      case BL_STATE_UPGRADING:
        {
              flag = UpdataFirmware();
              if (flag == UPGRADE_OK)
              {
                  errorCount = 0;
                  packageIndex++;
                  //RequestPackage(packageIndex);
              }
              else if(flag == UPGRADE_END)
              {
                   MCU_SoftwareReset();
              }
              else
              {
                  //RequestPackage(packageIndex);
                  errorCount++;
                  if(errorCount > 3)
                  {
                      bl_state = BL_STATE_ERROR;
                  }
              }
              break;
        }
       
     }
  }

}


