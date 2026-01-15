#ifndef __FLASH_H
#define __FLASH_H

#include "main.h"

/* APP 起始地址（Bootloader 场景） */
#define FLASH_APP_START_ADDR   0x08008000
#define FLASH_END_ADDR         0x080FFFFF
HAL_StatusTypeDef BSP_Flash_Erase(uint32_t startSector, uint32_t sectorCount);
HAL_StatusTypeDef BSP_Flash_WriteBuffer(uint32_t addr, uint8_t *buf, uint32_t len);
uint8_t WriteFirmwareToFlash(uint8_t *firmwareBuff,uint32_t *writeAddr,uint32_t writeFlashSize);
#endif