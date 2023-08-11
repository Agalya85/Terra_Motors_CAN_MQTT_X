/*******************************************************************************
* Title                 :   Internal Flash Interface
* Filename              :   user_flash.h
* Author                :
* Origin Date           :   22/04/2022
* Version               :   1.0.0
* Compiler              :
* Target                :   STM32L433
* Notes                 :   None
*
* Copyright (c) by KloudQ Technologies Limited.

  This software is copyrighted by and is the sole property of KloudQ
  Technologies Limited.
  All rights, title, ownership, or other interests in the software remain the
  property of  KloudQ Technologies Limited. This software may only be used in
  accordance with the corresponding license agreement. Any unauthorized use,
  duplication, transmission, distribution, or disclosure of this software is
  expressly forbidden.

  This Copyright notice may not be removed or modified without prior written
  consent of KloudQ Technologies Limited.

  KloudQ Technologies Limited reserves the right to modify this software
  without notice.
*
*
*******************************************************************************/


/*************** FILE REVISION LOG *****************************************
*
*    Date    Version   Author         	   Description
*  22/04/22   1.0.0       Initial Release.
*******************************************************************************/

/** @file user_flash.h
*  @brief Internal flash Implementation
*
*/


#ifndef INC_USER_FLASH_H_
#define INC_USER_FLASH_H_

/******************************************************************************
* Includes
*******************************************************************************/
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_flash.h"


#define page_size 0x800

/* !!! Be careful the user area should be in another bank than the code !!! */
#define FLASH_USER_START_ADDR   ADDR_FLASH_PAGE_32     /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ADDR_FLASH_PAGE_63 + FLASH_PAGE_SIZE - 1   /* End @ of user Flash area */

/* Base address of the Flash sectors */

#define ADDR_FLASH_PAGE_0     ((uint32_t)0x08000000) /* Base @ of Page 0, 2 Kbytes */
#define ADDR_FLASH_PAGE_1     ((uint32_t)0x08000800) /* Base @ of Page 1, 2 Kbytes */
#define ADDR_FLASH_PAGE_2     ((uint32_t)0x08001000) /* Base @ of Page 2, 2 Kbytes */
#define ADDR_FLASH_PAGE_3     ((uint32_t)0x08001800) /* Base @ of Page 3, 2 Kbytes */
#define ADDR_FLASH_PAGE_4     ((uint32_t)0x08002000) /* Base @ of Page 4, 2 Kbytes */
#define ADDR_FLASH_PAGE_5     ((uint32_t)0x08002800) /* Base @ of Page 5, 2 Kbytes */
#define ADDR_FLASH_PAGE_6     ((uint32_t)0x08003000) /* Base @ of Page 6, 2 Kbytes */
#define ADDR_FLASH_PAGE_7     ((uint32_t)0x08003800) /* Base @ of Page 7, 2 Kbytes */
#define ADDR_FLASH_PAGE_8     ((uint32_t)0x08004000) /* Base @ of Page 8, 2 Kbytes */
#define ADDR_FLASH_PAGE_9     ((uint32_t)0x08004800) /* Base @ of Page 9, 2 Kbytes */
#define ADDR_FLASH_PAGE_10    ((uint32_t)0x08005000) /* Base @ of Page 10, 2 Kbytes */
#define ADDR_FLASH_PAGE_11    ((uint32_t)0x08005800) /* Base @ of Page 11, 2 Kbytes */
#define ADDR_FLASH_PAGE_12    ((uint32_t)0x08006000) /* Base @ of Page 12, 2 Kbytes */
#define ADDR_FLASH_PAGE_13    ((uint32_t)0x08006800) /* Base @ of Page 13, 2 Kbytes */
#define ADDR_FLASH_PAGE_14    ((uint32_t)0x08007000) /* Base @ of Page 14, 2 Kbytes */
#define ADDR_FLASH_PAGE_15    ((uint32_t)0x08007800) /* Base @ of Page 15, 2 Kbytes */
#define ADDR_FLASH_PAGE_16    ((uint32_t)0x08008000) /* Base @ of Page 16, 2 Kbytes */
#define ADDR_FLASH_PAGE_17    ((uint32_t)0x08008800) /* Base @ of Page 17, 2 Kbytes */
#define ADDR_FLASH_PAGE_18    ((uint32_t)0x08009000) /* Base @ of Page 18, 2 Kbytes */
#define ADDR_FLASH_PAGE_19    ((uint32_t)0x08009800) /* Base @ of Page 19, 2 Kbytes */
#define ADDR_FLASH_PAGE_20    ((uint32_t)0x0800A000) /* Base @ of Page 20, 2 Kbytes */
#define ADDR_FLASH_PAGE_21    ((uint32_t)0x0800A800) /* Base @ of Page 21, 2 Kbytes */
#define ADDR_FLASH_PAGE_22    ((uint32_t)0x0800B000) /* Base @ of Page 22, 2 Kbytes */
#define ADDR_FLASH_PAGE_23    ((uint32_t)0x0800B800) /* Base @ of Page 23, 2 Kbytes */
#define ADDR_FLASH_PAGE_24    ((uint32_t)0x0800C000) /* Base @ of Page 24, 2 Kbytes */
#define ADDR_FLASH_PAGE_25    ((uint32_t)0x0800C800) /* Base @ of Page 25, 2 Kbytes */
#define ADDR_FLASH_PAGE_26    ((uint32_t)0x0800D000) /* Base @ of Page 26, 2 Kbytes */
#define ADDR_FLASH_PAGE_27    ((uint32_t)0x0800D800) /* Base @ of Page 27, 2 Kbytes */
#define ADDR_FLASH_PAGE_28    ((uint32_t)0x0800E000) /* Base @ of Page 28, 2 Kbytes */
#define ADDR_FLASH_PAGE_29    ((uint32_t)0x0800E800) /* Base @ of Page 29, 2 Kbytes */
#define ADDR_FLASH_PAGE_30    ((uint32_t)0x0800F000) /* Base @ of Page 30, 2 Kbytes */
#define ADDR_FLASH_PAGE_31    ((uint32_t)0x0800F800) /* Base @ of Page 31, 2 Kbytes */
#define ADDR_FLASH_PAGE_32    ((uint32_t)0x08010000) /* Base @ of Page 32, 2 Kbytes */
#define ADDR_FLASH_PAGE_33    ((uint32_t)0x08010800) /* Base @ of Page 33, 2 Kbytes */
#define ADDR_FLASH_PAGE_34    ((uint32_t)0x08011000) /* Base @ of Page 34, 2 Kbytes */
#define ADDR_FLASH_PAGE_35    ((uint32_t)0x08011800) /* Base @ of Page 35, 2 Kbytes */
#define ADDR_FLASH_PAGE_36    ((uint32_t)0x08012000) /* Base @ of Page 36, 2 Kbytes */
#define ADDR_FLASH_PAGE_37    ((uint32_t)0x08012800) /* Base @ of Page 37, 2 Kbytes */
#define ADDR_FLASH_PAGE_38    ((uint32_t)0x08013000) /* Base @ of Page 38, 2 Kbytes */
#define ADDR_FLASH_PAGE_39    ((uint32_t)0x08013800) /* Base @ of Page 39, 2 Kbytes */
#define ADDR_FLASH_PAGE_40    ((uint32_t)0x08014000) /* Base @ of Page 40, 2 Kbytes */
#define ADDR_FLASH_PAGE_41    ((uint32_t)0x08014800) /* Base @ of Page 41, 2 Kbytes */
#define ADDR_FLASH_PAGE_42    ((uint32_t)0x08015000) /* Base @ of Page 42, 2 Kbytes */
#define ADDR_FLASH_PAGE_43    ((uint32_t)0x08015800) /* Base @ of Page 43, 2 Kbytes */
#define ADDR_FLASH_PAGE_44    ((uint32_t)0x08016000) /* Base @ of Page 44, 2 Kbytes */
#define ADDR_FLASH_PAGE_45    ((uint32_t)0x08016800) /* Base @ of Page 45, 2 Kbytes */
#define ADDR_FLASH_PAGE_46    ((uint32_t)0x08017000) /* Base @ of Page 46, 2 Kbytes */
#define ADDR_FLASH_PAGE_47    ((uint32_t)0x08017800) /* Base @ of Page 47, 2 Kbytes */
#define ADDR_FLASH_PAGE_48    ((uint32_t)0x08018000) /* Base @ of Page 48, 2 Kbytes */
#define ADDR_FLASH_PAGE_49    ((uint32_t)0x08018800) /* Base @ of Page 49, 2 Kbytes */
#define ADDR_FLASH_PAGE_50    ((uint32_t)0x08019000) /* Base @ of Page 50, 2 Kbytes */
#define ADDR_FLASH_PAGE_51    ((uint32_t)0x08019800) /* Base @ of Page 51, 2 Kbytes */
#define ADDR_FLASH_PAGE_52    ((uint32_t)0x0801A000) /* Base @ of Page 52, 2 Kbytes */
#define ADDR_FLASH_PAGE_53    ((uint32_t)0x0801A800) /* Base @ of Page 53, 2 Kbytes */
#define ADDR_FLASH_PAGE_54    ((uint32_t)0x0801B000) /* Base @ of Page 54, 2 Kbytes */
#define ADDR_FLASH_PAGE_55    ((uint32_t)0x0801B800) /* Base @ of Page 55, 2 Kbytes */
#define ADDR_FLASH_PAGE_56    ((uint32_t)0x0801C000) /* Base @ of Page 56, 2 Kbytes */
#define ADDR_FLASH_PAGE_57    ((uint32_t)0x0801C800) /* Base @ of Page 57, 2 Kbytes */
#define ADDR_FLASH_PAGE_58    ((uint32_t)0x0801D000) /* Base @ of Page 58, 2 Kbytes */
#define ADDR_FLASH_PAGE_59    ((uint32_t)0x0801D800) /* Base @ of Page 59, 2 Kbytes */
#define ADDR_FLASH_PAGE_60    ((uint32_t)0x0801E000) /* Base @ of Page 60, 2 Kbytes */
#define ADDR_FLASH_PAGE_61    ((uint32_t)0x0801E800) /* Base @ of Page 61, 2 Kbytes */
#define ADDR_FLASH_PAGE_62    ((uint32_t)0x0801F000) /* Base @ of Page 62, 2 Kbytes */
#define ADDR_FLASH_PAGE_63    ((uint32_t)0x0801F800) /* Base @ of Page 63, 2 Kbytes */

#define BOOTMEMORYINFO			(*((uint32_t *)  0x0801F800))  /* Store info related to FW Boot point for FW Jump */
#define BOOTMEMORYADDRESS		((uint32_t)0x0801F800)

/* Notable Flash addresses */
#define FLASH_START		        ADDR_FLASH_PAGE_1           /*Start addr of Flash Memory after boot loader*/
#define USER_FLASH_END_ADDRESS  ADDR_FLASH_PAGE_62 			/*End addr of Flash Memory as STM32L433 is of 128KB, 2kb each*/
#define FLASH_BASE_ADDR 		(0x08000000UL)              /*Start addr of Flash Memory*/

#define FLASH_X_START_ADDR   	(ADDR_FLASH_PAGE_1)		   	/* Start @ of X Flash area */
#define FLASH_X_END_ADDR     	(ADDR_FLASH_PAGE_30) 	    /* End @ of X Flash area */

//#define FLASH_START_ADDR      	((uint32_t)0x0803F800)

#define FLASH_Y_START_ADDR	 	(ADDR_FLASH_PAGE_33) 	   /* Start @ of Y Flash area */
#define FLASH_Y_END_ADDR      	(ADDR_FLASH_PAGE_62)	   /* End @ of Y Flash area */

#define BOOT_MEM_X				((uint32_t)0x7E)
#define BOOT_MEM_X_JUMP			((uint32_t)0x58)
#define BOOT_MEM_Y				((uint32_t)0x7F)
#define BOOT_MEM_Y_JUMP			((uint32_t)0x59)


/* Boot Memory Region */
typedef enum
{
	enmBOOTSEGMENT_X = 0,
	enmBOOTSEGMENT_Y,
	enmBOOTSEGMENT_X_JUMP,
	enmBOOTSEGMENT_Y_JUMP,
	enmBOOTSEGMENT_UNKNOWN
}enmBootMemorySegment;

/* Error code */
enum
{
  FLASHIF_OK = 0,
  FLASHIF_ERASE_OK,
  FLASHIF_ERASE_ERROR,
  FLASHIF_WRITINGCTRL_ERROR,
  FLASHIF_WRITING_ERROR,
  FLASHIF_WRITEVERIFY_ERROR
};

/* Function Pointers */
typedef  void (*pFunction)(void);

void initVectorTable(enmBootMemorySegment memorytouse);
void initFWSelection(void);

void FLASH_If_Init(void);

uint32_t WriteDatatoFlash(uint8_t *data,uint32_t writeCycle);
uint32_t FLASH_If_Write(uint64_t* Data ,uint32_t DataSize,enmBootMemorySegment BootMemoryRegion, uint32_t writeCycle);
uint32_t GetPage(uint32_t Addr);
uint32_t FLASH_If_Erase(uint32_t state, enmBootMemorySegment BootMemoryRegion);
enmBootMemorySegment getNewFirmwareStorageLocation(void);
uint32_t flashWriteBootSection(enmBootMemorySegment memorySegment);
uint32_t EraseSinglePage(uint32_t EraseAddr);

//static uint32_t GetBank(uint32_t Addr);

#endif /* INC_USER_FLASH_H_ */
