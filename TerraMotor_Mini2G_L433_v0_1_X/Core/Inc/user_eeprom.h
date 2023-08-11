

#ifndef INC_USER_EEPROM_H_
#define INC_USER_EEPROM_H_


#define CONFIG_AT24XX_SIZE 		(32)			/* Configure Used EEPROM memory */

/************************** EEPROM Memory over I2C Defines *************************/
/* EEPROM */
#define EEPROM_SIGNATURE_BYTE 					"&"  // Make sure gu8DeviceSignature[1] = EEPROM_SIGNATURE_BYTE
#define EEPROM_MEMORY_ADDRESS_SIZE				(I2C_MEMADD_SIZE_16BIT)
#define EEPROM_DEVICE_ID_READ					(0b10100001)
#define EEPROM_DEVICE_ID_WRITE					(0b10100000)
#define EEPROM_SIGNATURE_ADDRESS				(0x00)

#define EEPROM_ADDRESS 							(0xA0)
#define EEPROM_OPR_READ 						(0x00)
#define EEPROM_OPR_WRITE						(0x01)
#define EEPROM_OPR_IDLE							(0x02)
#define I2CMEM_MAX_OPRATIONS 					(11)

#define MEM_MOBNUMLEN_ADD 						(0x02)
#define MEM_SERURLLEN_ADD 						(0x04)
#define MEM_NWAPNLEN_ADD 						(0x06)
#define MEM_ONFRQLEN_ADD 						(0x08)
#define MEM_OFFFRQLEN_ADD 						(0x0A)
#define MEM_MOBNUM_ADD 							(0x20)
#define MEM_SERURL_ADD 							(0x40)
#define MEM_NWAPN_ADD 							(0x70)
#define MEM_ONFRQ_ADD 							(0x2C0)
#define MEM_OFFFRQ_ADD 							(0x2C5)
#define MEM_URL_BASE_ADD  						(128)
#define EEPROM_DATA_ADDRESS_SERIAL_FLASH_WRITTEN_TILL		(uint16_t)(0x0600)	// 3 byte data
#define EEPROM_DATA_ADDRESS_SERIAL_FLASH_READ_TILL			(uint16_t)(0x0603)	// 3 Byte data

/* Memory Chip Used - Select Accordingly */
/* Get the part configuration based on the size configuration */
#if CONFIG_AT24XX_SIZE == 2       /* AT24C02: 2Kbits = 256; 16 * 16 =  256 */
#  define AT24XX_NPAGES     16
#  define AT24XX_PAGESIZE   16
#  define AT24XX_ADDRSIZE   1
#elif CONFIG_AT24XX_SIZE == 4     /* AT24C04: 4Kbits = 512B; 32 * 16 = 512 */
#  define AT24XX_NPAGES     32
#  define AT24XX_PAGESIZE   16
#  define AT24XX_ADDRSIZE   1
#elif CONFIG_AT24XX_SIZE == 8     /* AT24C08: 8Kbits = 1KiB; 64 * 16 = 1024 */
#  define AT24XX_NPAGES     64
#  define AT24XX_PAGESIZE   16
#  define AT24XX_ADDRSIZE   1
#elif CONFIG_AT24XX_SIZE == 16    /* AT24C16: 16Kbits = 2KiB; 128 * 16 = 2048 */
#  define AT24XX_NPAGES     128
#  define AT24XX_PAGESIZE   16
#  define AT24XX_ADDRSIZE   1
#elif CONFIG_AT24XX_SIZE == 32    /* AT24C32: 32Kbits = 4KiB; 128 * 32 =  4096 */
#  define AT24XX_NPAGES     128
#  define AT24XX_PAGESIZE   32
#  define AT24XX_ADDRSIZE   2
#elif CONFIG_AT24XX_SIZE == 48    /* AT24C48: 48Kbits = 6KiB; 192 * 32 =  6144 */
#  define AT24XX_NPAGES     192
#  define AT24XX_PAGESIZE   32
#  define AT24XX_ADDRSIZE   2
#elif CONFIG_AT24XX_SIZE == 64    /* AT24C64: 64Kbits = 8KiB; 256 * 32 = 8192 */
#  define AT24XX_NPAGES     256
#  define AT24XX_PAGESIZE   32
#  define AT24XX_ADDRSIZE   2
#elif CONFIG_AT24XX_SIZE == 128   /* AT24C128: 128Kbits = 16KiB; 256 * 64 = 16384 */
#  define AT24XX_NPAGES     256
#  define AT24XX_PAGESIZE   64
#  define AT24XX_ADDRSIZE   2
#elif CONFIG_AT24XX_SIZE == 256   /* AT24C256: 256Kbits = 32KiB; 512 * 64 = 32768 */
#  define AT24XX_NPAGES     512
#  define AT24XX_PAGESIZE   64
#  define AT24XX_ADDRSIZE   2
#elif CONFIG_AT24XX_SIZE == 512   /* AT24C512: 512Kbits = 64KiB; 512 * 128 = 65536 */
#  define AT24XX_NPAGES     512
#  define AT24XX_PAGESIZE   128
#  define AT24XX_ADDRSIZE   2
#endif

typedef enum
{
	enmMEMORY_WRITE,
	enmMEMORY_READ
}enmI2CMemoryOperation;

typedef struct
{
	uint32_t u32WrMobileNumberLength;
	uint32_t u32WrNetworkAPNLength;
	uint32_t u32WrServerURLLength;
	uint32_t u32WrOnFrequencyLength;    // in Sec 1 to 10000 Sec
	uint32_t u32WrOffFrequencyLength;   // in Sec 1 to 10000 Sec

	uint32_t u32RdMobileNumberLength;
	uint32_t u32RdNetworkAPNLength;
	uint32_t u32RdServerURLLength;
	uint32_t u32RdOnFrequencyLength;    // in Sec 1 to 10000 Sec
	uint32_t u32RdOffFrequencyLength;   // in Sec 1 to 10000 Sec

	char pu8Wr8UploadOnFreq[5];	    	// Upload On Freq
	char pu8WrUploadOffFreq[5];	   		// Upload On Freq
	char pu8WrMobileNumber[15];	    	// SMS Send to : 10 or 13 Digit

	char pu8DeviceSignature[1];	    	// Device Signature
	char pu8MobileNumber[15];	    	// SMS Send to : 10 or 13 Digit
	char pu8NetworkAPN[20];		    	// APN : Max 20 characters
	char pu8RdServerURL[150];		    // URL : Max 150 Characters
	char pu8RdUploadOnFreq[5];		    // Upload On Freq
	char pu8RdUploadOffFreq[5];	    	// Upload Off Freq
	uint32_t test;
	uint8_t SerialFlashWrittenTill[4];
	uint8_t SerialFlashReadTill[4];
	/* Flash memory variables */

	/* Flash memory variables */
	char pu8SFlashWriteMemLocAddress[5]; // added on 15-12-2020 Serial flash current write address
	char pu8SFlashReadMemLocAddress[5];  // added on 15-12-2020 Serial flash current read address

}strctMemoryLayout;

extern strctMemoryLayout strI2cEeprom;

typedef enum
{
	I2C_MEM_DEVSIGNATURE = 0,
	I2C_MEM_MOBILELENGTH,
	I2C_MEM_APNSTRLENGTH,
	I2C_MEM_URLSTRLENGTH,
	I2C_MEM_ONFREQLENGTH,
	I2C_MEM_OFFFREQLENGTH,
	I2C_MEM_MOBILENOSTR,
	I2C_MEM_APNSTR,
	I2C_MEM_URLSTR,
	I2C_MEM_ONFREQ,
	I2C_MEM_OFFFREQ,
	I2C_MEM_FLASH_READ,
	I2C_MEM_FLASH_WRITE
}enmMemoryOperation;

void initMemoryRead(void);
void initDefaultParameters(void);
void initMemoryWrite(void);
void getDeviceSignatureFromMemory(void);
void writeToMemory(void);
void readFromMemory(void);
void initSignatureWrite(void);
void writeDefaultParamtoMemory(void);
void writeParametertoMemory(enmMemoryOperation memOpr);
void initSystemDefaultsfromMemory(void);
uint32_t getAvailableSpaceInMemoryPage(uint32_t MemAddress);
uint32_t getFirstPageWriteSize(uint32_t pageSpace);
uint32_t getLastPageWriteSize(uint32_t firstPageWriteSize , uint32_t dataLength);
uint32_t getPageWriteCyclesRequired(uint32_t firstPageWriteSize , uint32_t dataLength);
/************************** End of EEPROM Memory Over I2C Defines *************************************/


#endif /* INC_USER_EEPROM_H_ */
