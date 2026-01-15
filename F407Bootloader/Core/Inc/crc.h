#ifndef __CRC_H
#define __CRC_H
#include "main.h"

typedef struct
{
  uint8_t CRCH;
  uint8_t CRCL;
}CRC_USER;

extern uint8_t aucCRCHi[];
extern uint8_t aucCRCLo[];

// CRC¼ÆËãº¯Êý
CRC_USER CalculateCRC(uint8_t* pucFrame,uint16_t usLen);

#endif