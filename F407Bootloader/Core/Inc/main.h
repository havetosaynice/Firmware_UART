#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f4xx_hal.h"



#define ERROR_LED_Pin GPIO_PIN_13
#define ERROR_LED_GPIO_Port GPIOC

#define FIRMWAREHEADERADDR 0x08008000  
#define APP_ADDR 0x08008010
  
#define APP_FLASH_START   0x08008010
#define APP_FLASH_END   0x080FFFFF
  
#define APP_SRAM_START 0x20002800  
#define APP_SRAM_END   0x2001FFFF

#define APP_START_SECTOR  FLASH_SECTOR_2
#define APP_SECTOR_COUNT  6
  
#define WaitUpdataTime    1 * 1000
  
  
#define LENGHTMAX         4 * 125
  
extern uint8_t DownloadFlag;


extern uint8_t NeedUpdataFlag;
extern uint8_t DataAcquired;
extern uint8_t FirmwareBuff[];


extern uint32_t FirmwareFlag;//固件标志
//固件大小
extern uint32_t FirmwareSize;//接收包时候实时变化
extern uint32_t FirmwareSizeFinally;//最终接收包大小
extern uint16_t FirmwareCRC;
extern uint16_t FirmwareVersion;
extern uint16_t ReIndex;



typedef enum
{
    UPGRADE_END,
    UPGRADE_OK,
    UPGRADE_FAIL
} UpgradeResult_t;

typedef struct
{
    uint32_t magic;
    uint32_t size;
    uint16_t crc;
    uint16_t version;
} FirmwareHeader;

typedef enum
{
    BL_STATE_WAIT,
    BL_STATE_UPGRADING,
    BL_STATE_VERIFY,
    BL_STATE_JUMP,
    BL_STATE_ERROR
} BL_State_t;


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
