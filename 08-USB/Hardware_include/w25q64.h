#ifndef __W25Q64_H__
#define __W25Q64_H__

#include "main.h"
#include "stm32f4xx.h"


/*******SPI Flash片选引脚定义*******/
// 控制SPI Flash的片选引脚（GPIOE引脚15），x为BitAction类型（ENABLE/DISABLE）
#define	SPI_FLASH_CS(x)			GPIO_WriteBit(GPIOE, GPIO_Pin_15, (BitAction)x)

/*******W25Q系列Flash芯片ID定义*******/
#define W25Q80 	0XEF13 	// W25Q80芯片ID（容量8Mbit）
#define W25Q16 	0XEF14 	// W25Q16芯片ID（容量16Mbit）
#define W25Q32 	0XEF15 	// W25Q32芯片ID（容量32Mbit）
#define W25Q64 	0XEF16 	// W25Q64芯片ID（容量64Mbit，本驱动默认芯片）

/*******W25Q64芯片容量配置定义*******/
#define W25Q64_SECTOR_SIZE	4096		// 单个扇区大小（4KB）
#define W25Q64_SECTOR_NUM		2048		// 总扇区数（64Mbit = 8MB，8MB/4KB = 2048个扇区）

/*******W25Q64芯片地址存储分配*******/
#define FLASH_ASC16_ADDRESS                 0               // ASC16字模数据存储起始地址
#define FLASH_HZK16_ADDRESS                 0x1000          // HZK16汉字库数据存储起始地址（4KB偏移）

#define FLASH_SYSTEM_CONFIG_ADDRESS         0x43000         // 系统配置参数存储起始地址（276KB偏移）

// 5个位图（Bitmap）的大小信息存储地址（每个间隔160KB）
#define FLASH_BITMAP1_SIZE_ADDRESS	        0x50000         // 位图1大小信息地址（320KB偏移）
#define FLASH_BITMAP2_SIZE_ADDRESS	        FLASH_BITMAP1_SIZE_ADDRESS + 0x28000  // 位图2大小信息地址（+160KB）
#define FLASH_BITMAP3_SIZE_ADDRESS	        FLASH_BITMAP2_SIZE_ADDRESS + 0x28000  // 位图3大小信息地址（+160KB）
#define FLASH_BITMAP4_SIZE_ADDRESS	        FLASH_BITMAP3_SIZE_ADDRESS + 0x28000  // 位图4大小信息地址（+160KB）
#define FLASH_BITMAP5_SIZE_ADDRESS	        FLASH_BITMAP4_SIZE_ADDRESS + 0x28000  // 位图5大小信息地址（+160KB）	
#define FLASH_BITMAP6_SIZE_ADDRESS	        FLASH_BITMAP5_SIZE_ADDRESS + 0x28000  // 位图6大小信息地址（+160KB）

// 功能位图大小信息存储地址（每个间隔160KB）
#define FLASH_BITMAPMAIN_SIZE_ADDRESS       FLASH_BITMAP6_SIZE_ADDRESS + 0x28000  // 主位图大小信息地址（+160KB）
#define FLASH_BITMAPDS1302_SIZE_ADDRESS     FLASH_BITMAPMAIN_SIZE_ADDRESS + 0x28000  // DS1302相关位图大小地址（+160KB）
#define FLASH_BITMAPDS18B20_SIZE_ADDRESS    FLASH_BITMAPDS1302_SIZE_ADDRESS + 0x28000  // DS18B20相关位图大小地址（+160KB）
#define FLASH_BITMAPBLUETOOTH_SIZE_ADDRESS  FLASH_BITMAPDS18B20_SIZE_ADDRESS + 0x28000  // 蓝牙相关位图大小地址（+160KB）

/*******FLASH相关控制指令定义（遵循W25Q64数据手册）*******/
#define W25X_WriteEnable			0x06 	// 写使能指令（允许后续写/擦除操作）
#define W25X_WriteDisable			0x04 	// 写禁止指令（禁止写/擦除操作，保护数据）
#define W25X_ReadStatusReg		0x05 	// 读状态寄存器指令（获取芯片忙状态、写保护状态等）
#define W25X_WriteStatusReg		0x01 	// 写状态寄存器指令（配置写保护、块保护等）
#define W25X_ReadData					0x03 	// 普通读数据指令（低速，地址后直接读数据）
#define W25X_FastReadData			0x0B 	// 快速读数据指令（高速，地址后加1个空周期）
#define W25X_FastReadDual			0x3B 	// 双输出快速读指令（SPI双路输出，更高速度）
#define W25X_PageProgram			0x02 	// 页编程指令（单次最多写256字节，需先擦除扇区）
#define W25X_SectorErase			0x20 	// 扇区擦除指令（擦除单个4KB扇区，需先写使能）
#define W25X_BlockErase				0xD8 	// 块擦除指令（擦除单个64KB块，需先写使能）
#define W25X_ChipErase				0xC7 	// 整片擦除指令（擦除整个芯片，耗时较长，需先写使能）
#define W25X_PowerDown				0xB9 	// 掉电模式指令（降低功耗，仅上电指令可唤醒）
#define W25X_ReleasePowerDown	0xAB 	// 释放掉电模式指令（唤醒芯片，恢复正常工作）
#define W25X_DeviceID					0xAB 	// 读设备ID指令（获取芯片唯一标识）
#define W25X_ManufactDeviceID	0x90 	// 读厂商+设备ID指令（获取厂商代码和设备代码）
#define W25X_JedecDeviceID		0x9F 	// 读JEDEC标准ID指令（获取厂商+设备+容量信息，常用）

/*******FLASH相关函数声明*******/
void	SpiFlashInit(void);                          // SPI Flash初始化（配置SPI引脚、时钟、模式）
void    spi_set_speed(uint16_t SpeedSet);            // 设置SPI通信速率（根据需求调整传输速度）
u8		SpiWriteRead(u8 d);                          // SPI单字节读写（发送1字节数据，同时接收1字节数据）
u16		SpiFlashReadID(void);                        // 读取Flash芯片ID（判断芯片型号是否为W25Q64）
u8		SpiFlashReadSR(void);                        // 读取Flash状态寄存器（获取忙状态、写保护等信息）
void 	SpiFlashWriteSR(u8 sr);                      // 写入Flash状态寄存器（配置写保护级别等）
void 	SpiFlashWriteEnable(void);                   // 使能Flash写操作（执行写/擦除前必须调用）
void 	SpiFlashWriteDisable(void);                  // 禁止Flash写操作（保护数据，防止误写）
char 	SpiFlashReadChar(u32 ReadAddr);              // 从指定地址读取1个字节数据
void 	SpiFlashRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);  // 批量读取数据（从指定地址读N字节到缓冲区）
void 	SpiFlashWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);  // 批量写入数据（带扇区擦除检查，安全写）
void 	SpiFlashWriteS(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);  // 增强型批量写入（具体逻辑需看实现，通常含额外校验）
void    SpiFlashWriteChar(char tmp, u32 WriteAddr);  // 单字节写入（向指定地址写1个字节，需先擦除扇区）
void 	SpiFlashWritePage(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);  // 页写入（单次最多256字节，需确保地址在同一页）
void 	SpiFlashWriteSector(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);  // 扇区写入（先擦除指定扇区，再写入数据）
void 	SpiFlashWriteNoCheck(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);  // 无检查写入（不擦除扇区，需确保地址已擦除，快速写）
void 	SpiFlashEraseSector(u32 Dst_Addr);           // 扇区擦除（擦除指定地址所在的4KB扇区，需先写使能）
void 	SpiFlashEraseChip(void);                     // 整片擦除（擦除整个芯片所有数据，耗时较长，需谨慎使用）
void 	SpiFlashWaitBusy(void);                      // 等待芯片空闲（循环读取状态寄存器，直到芯片不忙）
void 	SpiFlashPowerDown(void);                     // 进入掉电模式（降低功耗，仅唤醒指令可恢复）
void	SpiFlashWakeUp(void);                        // 唤醒芯片（从掉电模式恢复，唤醒后需等待稳定）
void    setup_w25q64(void);                          // W25Q64存储芯片初始化（高阶初始化，可能含ID校验、参数配置）

/*******其他宏定义*******/
#define PRE_CMD_SIZE 128  // 预命令缓存大小（128字节，具体用途需结合业务逻辑）

/*******Flash操作简化宏定义（封装核心函数，便于调用）*******/
#define w25x_init()          SpiFlashInit()                  // Flash初始化简化宏
#define w25x_readId()        SpiFlashReadID()                // 读Flash ID简化宏
#define w25x_read(buf, addr, len) SpiFlashRead(buf, addr, len)  // 批量读简化宏
#define w25x_write(buf, addr, len) SpiFlashWriteNoCheck(buf, addr, len)  // 无检查写简化宏
#define w25x_writeS(buf, addr, len) SpiFlashWriteS(buf, addr, len)  // 增强型写简化宏
#define w25x_erase_sector(addr) SpiFlashEraseSector(addr)    // 扇区擦除简化宏
#define w25x_wait_busy()     SpiFlashWaitBusy()              // 等待空闲简化宏

#endif
