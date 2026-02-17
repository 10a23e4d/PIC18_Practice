#include "FLASH1.h"

//=============================================================================
//  Pin & Communication Settings
//=============================================================================

// --- UART1 Settings (PPS) ---
#pin_select U1TX = PIN_F7
#pin_select U1RX = PIN_F5
#use rs232(baud=9600, UART1, parity=N, bits=8, stream=PORT1)

// --- SPI1 Settings (PPS & Port A) ---
// 【重要】PIC18F67J94はPPS設定が必須です。これがないと信号が出ません。
#pin_select SDI1    = PIN_A0  // MISO: Flash DO -> PIC DI
#pin_select SDO1    = PIN_A1  // MOSI: PIC DO -> Flash DI
#pin_select SCK1OUT = PIN_A3  // CLK : Clock

// 基板の配線定義
#define SPI1_CS  PIN_A2

// SPI設定
#use spi(MASTER, CLK=PIN_A3, DI=PIN_A0, DO=PIN_A1, BAUD=1000000, MODE=0, BITS=8, STREAM=SPI1)


//=============================================================================
//  Flash Memory Commands (MT25QL128ABA)
//=============================================================================
#define FLASH_CMD_WREN      0x06
#define FLASH_CMD_RDID      0x9F
#define FLASH_CMD_RDSR      0x05
#define FLASH_CMD_READ      0x03
#define FLASH_CMD_PP        0x02
#define FLASH_CMD_SE        0x20

//=============================================================================
//  Low Level Functions
//=============================================================================

void Flash_Write_Enable() {
    output_low(SPI1_CS);
    spi_xfer(SPI1, FLASH_CMD_WREN);
    output_high(SPI1_CS);
}

void Flash_Wait_Busy() {
    int8 status;
    do {
        output_low(SPI1_CS);
        spi_xfer(SPI1, FLASH_CMD_RDSR);
        status = spi_xfer(SPI1, 0x00);
        output_high(SPI1_CS);
    } while (status & 0x01);
}

void Flash_Read_ID(int8 *mfg_id, int16 *dev_id) {
    int8 d1, d2;

    output_low(SPI1_CS);
    spi_xfer(SPI1, FLASH_CMD_RDID);

    *mfg_id = spi_xfer(SPI1, 0x00);
    d1 = spi_xfer(SPI1, 0x00);
    d2 = spi_xfer(SPI1, 0x00);

    output_high(SPI1_CS);

    *dev_id = make16(d1, d2);
}

void Flash_Sector_Erase(int32 address) {
    Flash_Wait_Busy();
    Flash_Write_Enable();

    output_low(SPI1_CS);
    spi_xfer(SPI1, FLASH_CMD_SE);
    spi_xfer(SPI1, (address >> 16) & 0xFF);
    spi_xfer(SPI1, (address >> 8)  & 0xFF);
    spi_xfer(SPI1,  address        & 0xFF);
    output_high(SPI1_CS);

    Flash_Wait_Busy();
}

void Flash_Write_Byte(int32 address, int8 data) {
    Flash_Wait_Busy();
    Flash_Write_Enable();

    output_low(SPI1_CS);
    spi_xfer(SPI1, FLASH_CMD_PP);
    spi_xfer(SPI1, (address >> 16) & 0xFF);
    spi_xfer(SPI1, (address >> 8)  & 0xFF);
    spi_xfer(SPI1,  address        & 0xFF);
    spi_xfer(SPI1, data);
    output_high(SPI1_CS);

    Flash_Wait_Busy();
}

void Flash_Read_Data(int32 address, int8 *buffer, int16 length) {
    int16 i;
    Flash_Wait_Busy();

    output_low(SPI1_CS);
    spi_xfer(SPI1, FLASH_CMD_READ);
    spi_xfer(SPI1, (address >> 16) & 0xFF);
    spi_xfer(SPI1, (address >> 8)  & 0xFF);
    spi_xfer(SPI1,  address        & 0xFF);

    for(i=0; i<length; i++) {
        buffer[i] = spi_xfer(SPI1, 0x00);
    }
    output_high(SPI1_CS);
}

//=============================================================================
//  Main Function
//=============================================================================
void main() {
    int8 mfg_id;
    int16 dev_id;
    int32 target_addr = 0x001000;
    int8 write_val = 0x55;
    int8 read_val = 0;

    // --- Init ---
    setup_adc_ports(NO_ANALOGS); // アナログ入力無効化
    output_drive(SPI1_CS);
    output_high(SPI1_CS);

    delay_ms(100);

    fprintf(PORT1, "\r\n\r\n=== Flash Test (PPS Added) ===\r\n");

    // 1. Read Chip ID
    Flash_Read_ID(&mfg_id, &dev_id);
    fprintf(PORT1, "ID Read: Mfg=0x%X, Dev=0x%LX\r\n", mfg_id, dev_id);

    if(mfg_id == 0x20) {
        fprintf(PORT1, "[OK] Micron Flash Detected.\r\n");
    } else {
        fprintf(PORT1, "[ERR] ID Read Failed. Check connections.\r\n");
    }

    // 2. Erase/Write/Read
    Flash_Sector_Erase(target_addr);
    Flash_Write_Byte(target_addr, write_val);
    Flash_Read_Data(target_addr, &read_val, 1);

    fprintf(PORT1, "W:0x%X, R:0x%X -> ", write_val, read_val);

    if(write_val == read_val) {
        fprintf(PORT1, "SUCCESS!\r\n");
    } else {
        fprintf(PORT1, "FAIL\r\n");
    }

    while(TRUE) {
        delay_ms(1000);
    }
}