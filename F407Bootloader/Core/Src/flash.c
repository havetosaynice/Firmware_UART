#include "flash.h"
#include <string.h>

uint8_t BSP_Flash_IsAddrValid(uint32_t addr)
{
    if (addr < FLASH_APP_START_ADDR || addr > FLASH_END_ADDR)
        return 0;
    return 1;
}


HAL_StatusTypeDef BSP_Flash_Erase(uint32_t startSector, uint32_t sectorCount)
{
    FLASH_EraseInitTypeDef eraseInit;
    uint32_t sectorError = 0;

    HAL_FLASH_Unlock();

    eraseInit.TypeErase    = FLASH_TYPEERASE_SECTORS;
    eraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;   // 2.7V ~ 3.6V
    eraseInit.Sector       = startSector;
    eraseInit.NbSectors    = sectorCount;

    HAL_StatusTypeDef ret = HAL_FLASHEx_Erase(&eraseInit, &sectorError);

    HAL_FLASH_Lock();

    return ret;
}


HAL_StatusTypeDef BSP_Flash_WriteBuffer(uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint32_t i;
    uint32_t data;
    HAL_StatusTypeDef ret = HAL_OK;

    if (!BSP_Flash_IsAddrValid(addr))
        return HAL_ERROR;

    HAL_FLASH_Unlock();

    for (i = 0; i < len; i += 4)
    {
        data =  buf[i] |
               (buf[i + 1] << 8) |
               (buf[i + 2] << 16) |
               (buf[i + 3] << 24);

        ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr + i, data);
        if (ret != HAL_OK)
            break;
    }
    
    HAL_FLASH_Lock();
    return ret;
}


uint8_t WriteFirmwareToFlash(uint8_t *firmwareBuff,
                             uint32_t *writeAddr,
                             uint32_t writeFlashSize)
{
    if ((*writeAddr < APP_FLASH_START) ||
        ((*writeAddr + writeFlashSize) > APP_FLASH_END))
    {
        return 4;   // 地址越界
    }

    if (((*writeAddr % 4) != 0) || ((writeFlashSize % 4) != 0))
    {
        return 3;   // 未对齐
    }

    /* 写 Flash */
    if (BSP_Flash_WriteBuffer(*writeAddr,firmwareBuff, writeFlashSize) != HAL_OK)
    {
        return 2;   // 写失败
    }

    /* 回读校验 */
    if (memcmp((uint8_t *)(*writeAddr), firmwareBuff, writeFlashSize) != 0)
    {
        return 1;   // 校验失败
    }

    /* 写成功，推进写地址 */
    *writeAddr += writeFlashSize;

    return 0;       // 成功
}


