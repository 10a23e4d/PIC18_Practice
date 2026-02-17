/////LED1/////
//#include <18F67J94.h>
#include "LED2.h"


#pin_select U1TX = PIN_F7
#pin_select U1RX = PIN_F5
#use rs232(baud=9600, UART1, parity=N,bits=8,stream=PORT1)

#pin_select U2TX = PIN_D3
#pin_select U2RX =    PIN_D2
#use rs232(baud=9600, UART2, parity=N,bits=8,stream=PORT2)

#pin_select U3TX = PIN_E5
#pin_select U3RX = PIN_E4
#use rs232(baud=9600, UART3, parity=N,bits=8,stream=PORT3)

#pin_select U4TX = PIN_E3
#pin_select U4RX = PIN_F2
#use rs232(baud=9600, UART4, parity=N,bits=8,stream=PORT4)

#define SPI1_CS PIN_A2
#use spi (MASTER, CLK = PIN_A3, DI = PIN_A0, DO = PIN_A1,BAUD=1000000, MODE=0, BITS=8, STREAM=SPI1)

#define SPI2_CS PIN_B3
#use spi (MASTER, CLK = PIN_B2, DI = PIN_B5, DO = PIN_B4,BAUD=1000000, MODE=0, BITS=8, STREAM=SPI1)

#define SPI3_CS PIN_E2
#use spi (MASTER, CLK = PIN_E1, DI = PIN_E0, DO = PIN_E6,BAUD=1000000, MODE=0, BITS=8, STREAM=SPI1)


#define DIO_3 PIN_G0
#define DIO_4 PIN_G1
#define DIO_5 PIN_G2
#define DIO_6 PIN_G3
#define DIO_8 PIN_G4
#define DIO_12 PIN_F6
#define DIO_27 PIN_A5
#define DIO_28 PIN_A4
#define DIO_29 PIN_C1
#define DIO_30 PIN_C0
#define DIO_31 PIN_C6
#define DIO_32 PIN_C7
#define DIO_33 PIN_C2
#define DIO_34 PIN_C3
#define DIO_35 PIN_C4
#define DIO_36 PIN_C5
#define DIO_47 PIN_B1
#define DIO_48 PIN_B0
#define DIO_49 PIN_D7
#define DIO_50 PIN_D6
#define DIO_51 PIN_D5
#define DIO_52 PIN_D4
#define DIO_55 PIN_D1
#define DIO_58 PIN_D0
#define DIO_59 PIN_E7



void main()
{
    port_b_pullups(TRUE);
    int8 i;
    fprintf(PORT1, "Starting LED2 Program\r\n");
    while (TRUE)
    {
        if(input(DIO_47) == 1)
        {
            fprintf(PORT1, "Switch ON\r\n");
            fprintf(PORT1, "Starting Blinking LED\r\n");
            for (i = 0; i < 5; i++)
            {
                output_high(DIO_3);
                fprintf(PORT1, "LED ON (%d/5)\r\n", i + 1);
                delay_ms(500);
                output_low(DIO_3);
                fprintf(PORT1, "LED OFF (%d/5)\r\n", i + 1);
                delay_ms(500);
            }
            fprintf(PORT1, "Finished Blinking LED\r\n");
        }
    }
    delay_ms(10);
}
