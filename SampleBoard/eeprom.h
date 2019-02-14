/*
	stc89c52的eeprom大小有4KByte

		地址			大小		 扇区
	0x2000~0x21FF      512Byte   	sector1
	0x2200~0x23FF      512Byte   	sector2
	0x2400~0x25FF      512Byte   	sector3
	0x2600~0x27FF      512Byte   	sector4
	0x2800~0x29FF      512Byte   	sector5
	0x2A00~0x2BFF      512Byte   	sector6
	0x2C00~0x2DFF      512Byte   	sector7
	0x2E00~0x2FFF      512Byte   	sector8
*/

#ifndef __EEPROM_H__
#define __EEPROM_H__

typedef unsigned char BYTE;
typedef unsigned int WORD;

/*Declare SFR associated with the IAP */
//sfr IAP_DATA    =   0xE2;           //Flash data register
//sfr IAP_ADDRH   =   0xE3;           //Flash address HIGH
//sfr IAP_ADDRL   =   0xE4;           //Flash address LOW
//sfr IAP_CMD     =   0xE5;           //Flash command register
//sfr IAP_TRIG    =   0xE6;           //Flash command trigger
//sfr IAP_CONTR   =   0xE7;           //Flash control register

/*Define ISP/IAP/EEPROM command*/
#define CMD_IDLE    0               //Stand-By
#define CMD_READ    1               //Byte-Read
#define CMD_PROGRAM 2               //Byte-Program
#define CMD_ERASE   3               //Sector-Erase

/*Define ISP/IAP/EEPROM operation const for IAP_CONTR*/
//#define ENABLE_IAP 0x80           //if SYSCLK<40MHz
#define ENABLE_IAP   0x81           //if SYSCLK<20MHz
//#define ENABLE_IAP x82            //if SYSCLK<10MHz
//#define ENABLE_IAP 0x83           //if SYSCLK<5MHz

//Start address for STC89C52xx EEPROM
#define IAP_ADDRESS 0x2200

void Delay(BYTE n);
void IapIdle();
BYTE IapReadByte(WORD addr);
void IapProgramByte(WORD addr, BYTE dat);
void IapEraseSector(WORD addr);

#endif