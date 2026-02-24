#include <GDNS_tbd_FlashOperation.h>

void flash_setting()
{
    #ifdef USE_FLASH1G
        output_high(FLASH1G_CS);
    #endif
    #ifdef USE_FLASH128M
        output_high(FLASH128M_CS);
    #endif
}

#ifdef USE_FLASH1G
    int8 status_register_flash1g()
    {
        int8 status_reg;
        output_low(FLASH1G_CS);                                              //lower the CS PIN
        spi_xfer(FLASH1G,READ_STATUS_REGISTER);                              //READ STATUS REGISTER COMAND  (0x05)
        status_reg = spi_xfer(FLASH1G);
        output_high(FLASH1G_CS);                                             //take CS PIN higher back
        #ifdef DEBUG
            if((status_reg & 0x01) == true)                                  //masking status bit
            {
                fprintf(DEBUG_PORT,"1Gbit flash busy\n\r");
            }
        #endif
        return status_reg;
    }

    int8 read_id_flash1g()//->success:True,fail:false
    {
        int8 chip_id[21];
        output_low(FLASH1G_CS);
        for (int8 i = 0; i < 21; i++)
        {
            if (i == 0)
            {
                chip_id[i] = spi_xfer(FLASH1G,READ_ID);
                #ifdef DEBUG
                    fprintf(DEBUG_PORT,"Read ID:");
                #endif
            }
            else
            {
                chip_id[i] = spi_xfer(FLASH1G);
                #ifdef DEBUG
                    fprintf(DEBUG_PORT,"%x ",chip_id[i]);
                #endif
            }
        }
        output_high(FLASH1G_CS);
        #ifdef DEBUG
            fprintf(DEBUG_PORT,"\r\n");
        #endif
        //chip id check
        if(chip_id[1] == 0x20)
        {
            #ifdef DEBUG
                fprintf(DEBUG_PORT,"flash connect OK\r\n");
            #endif
            return true;
        }
        else
        {
            #ifdef DEBUG
                fprintf(DEBUG_PORT,"flash not connect\r\n");
            #endif
            return false;
        }
    }

    void sector_erase_flash1g(unsigned int32 sector_address)
    {
        unsigned int8 address[4];
        unsigned int8 timeout_counter = 0;

        address[0]  = (unsigned int8)((sector_address>>24) & 0xff);   // 0x _ _ 00 00 00
        address[1]  = (unsigned int8)((sector_address>>16) & 0xff);   // 0x 00 _ _ 00 00
        address[2]  = (unsigned int8)((sector_address>>8) & 0xff);    // 0x 00 00 _ _ 00
        address[3]  = (unsigned int8)((sector_address) & 0xff);       // 0x 00 00 00 _ _

        //Write enable sequence
        output_low(FLASH1G_CS);
        spi_xfer(FLASH1G,WRITE_ENABLE);
        output_high(FLASH1G_CS);
        //Write enabled

        output_low(FLASH1G_CS);
        ///////////////////////////////////////////////////////////////////
        spi_xfer(FLASH1G,SECTOR_ERASE_4BYTE); //SECTOR ERASE COMAND   (0xDC)

        spi_xfer(FLASH1G,address[0]);
        spi_xfer(FLASH1G,address[1]);
        spi_xfer(FLASH1G,address[2]);
        spi_xfer(FLASH1G,address[3]);
        //////////////////////////////////////////////////////////////////

        output_high(FLASH1G_CS);            //take CS PIN higher back

        while((status_register_flash1g() & 0x01) == 1)                //masking status bit
        {
            if(timeout_counter > 10)
            {
                delay_ms(200);
            }
            else
            {
                delay_ms(10);
            }

            if(timeout_counter > 100)
            {
                #ifdef DEBUG
                    fprintf(DEBUG_PORT,"flash timeout");
                #endif
                break;
            }
            timeout_counter++;
        }
        return;
    }

    void subsector_32kByte_erase_flash1g(unsigned int32 subsector_address)
    {
        unsigned int8 address[4];
        unsigned int8 timeout_counter = 0;

        address[0]  = (unsigned int8)((subsector_address>>24) & 0xff);   // 0x _ _ 00 00 00
        address[1]  = (unsigned int8)((subsector_address>>16) & 0xff);   // 0x 00 _ _ 00 00
        address[2]  = (unsigned int8)((subsector_address>>8) & 0xff);    // 0x 00 00 _ _ 00
        address[3]  = (unsigned int8)((subsector_address) & 0xff);       // 0x 00 00 00 _ _

        //Write enable sequence
        output_low(FLASH1G_CS);
        spi_xfer(FLASH1G,WRITE_ENABLE);
        output_high(FLASH1G_CS);
        //Write enabled

        output_low(FLASH1G_CS);
        ///////////////////////////////////////////////////////////////////
        spi_xfer(FLASH1G,SUBSECTOR_32KB_ERASE_4BYTE);

        spi_xfer(FLASH1G,address[0]);
        spi_xfer(FLASH1G,address[1]);
        spi_xfer(FLASH1G,address[2]);
        spi_xfer(FLASH1G,address[3]);
        //////////////////////////////////////////////////////////////////
        output_high(FLASH1G_CS);            //take CS PIN higher back

        while((status_register_flash1g() & 0x01) == 1)               //masking status bit
        {
            if(timeout_counter > 10)
            {
                delay_ms(200);
            }
            else
            {
                delay_ms(10);
            }

            if(timeout_counter > 100)
            {
                #ifdef DEBUG
                    fprintf(DEBUG_PORT,"flash timeout");
                #endif
                break;
            }
            timeout_counter++;
        }
    }

    void subsector_4kByte_erase_flash1g(unsigned int32 subsector_address)//4kByte erase
    {
        unsigned int8 address[4];
        unsigned int8 timeout_counter = 0;

        address[0]  = (unsigned int8)((subsector_address>>24) & 0xff);   // 0x _ _ 00 00 00
        address[1]  = (unsigned int8)((subsector_address>>16) & 0xff);   // 0x 00 _ _ 00 00
        address[2]  = (unsigned int8)((subsector_address>>8) & 0xff);    // 0x 00 00 _ _ 00
        address[3]  = (unsigned int8)((subsector_address) & 0xff);       // 0x 00 00 00 _ _

        //Write enable sequence
        output_low(FLASH1G_CS);
        spi_xfer(FLASH1G,WRITE_ENABLE);
        output_high(FLASH1G_CS);
        //Write enabled

        output_low(FLASH1G_CS);
        ///////////////////////////////////////////////////////////////////
        spi_xfer(FLASH1G,SUBSECTOR_4KB_ERASE_4BYTE);

        spi_xfer(FLASH1G,address[0]);
        spi_xfer(FLASH1G,address[1]);
        spi_xfer(FLASH1G,address[2]);
        spi_xfer(FLASH1G,address[3]);
        //////////////////////////////////////////////////////////////////

        delay_us(2);
        output_high(FLASH1G_CS);            //take CS PIN higher back

        while((status_register_flash1g() & 0x01) == 1)               //masking status bit
        {
            if(timeout_counter > 10)
            {
                delay_ms(200);
            }
            else
            {
                delay_ms(10);
            }

            if(timeout_counter > 100)
            {
                #ifdef DEBUG
                    fprintf(DEBUG_PORT,"flash timeout");
                #endif
                break;
            }
            timeout_counter++;
        }
    }

    int8 read_data_byte_flash1g(unsigned int32 read_address)
    {
        int8 read_data;
        unsigned int8 address[4];

        address[0]  = (unsigned int8)((read_address>>24) & 0xff);   // 0x _ _ 00 00 00
        address[1]  = (unsigned int8)((read_address>>16) & 0xff);   // 0x 00 _ _ 00 00
        address[2]  = (unsigned int8)((read_address>>8) & 0xff);    // 0x 00 00 _ _ 00
        address[3]  = (unsigned int8)((read_address) & 0xff);       // 0x 00 00 00 _ _

        output_low(FLASH1G_CS);
        ///////////////////////////////////////////////////////////////////
        spi_xfer(FLASH1G,READ_4BYTE);

        spi_xfer(FLASH1G,address[0]);
        spi_xfer(FLASH1G,address[1]);
        spi_xfer(FLASH1G,address[2]);
        spi_xfer(FLASH1G,address[3]);
        read_data = spi_xfer(FLASH1G);
        //////////////////////////////////////////////////////////////////

        delay_us(2);
        output_high(FLASH1G_CS);            //take CS PIN higher back
        return read_data;
    }

    void read_data_bytes_flash1g(unsigned int32 read_address,int8 amount,int8 *read_data)
    {
        unsigned int8 address[4];

        address[0]  = (unsigned int8)((read_address>>24) & 0xff);   // 0x _ _ 00 00 00
        address[1]  = (unsigned int8)((read_address>>16) & 0xff);   // 0x 00 _ _ 00 00
        address[2]  = (unsigned int8)((read_address>>8) & 0xff);    // 0x 00 00 _ _ 00
        address[3]  = (unsigned int8)((read_address) & 0xff);       // 0x 00 00 00 _ _

        output_low(FLASH1G_CS);
        ///////////////////////////////////////////////////////////////////
        spi_xfer(FLASH1G,READ_4BYTE);

        spi_xfer(FLASH1G,address[0]);
        spi_xfer(FLASH1G,address[1]);
        spi_xfer(FLASH1G,address[2]);
        spi_xfer(FLASH1G,address[3]);
        for(int8 i = 0; i < amount; i++)
        {
            read_data[i] = spi_xfer(FLASH1G);
        }
        //////////////////////////////////////////////////////////////////

        delay_us(2);
        output_high(FLASH1G_CS);            //take CS PIN higher back

        return;
    }

    void write_data_byte_flash1g(unsigned int32 write_address,int8 write_data)
    {
        unsigned int8 address[4];
        unsigned int8 timeout_counter = 0;

        //Byte extraction
        address[0]  = (unsigned int8)((write_address>>24) & 0xff);                     // 0x _ _ 00 00 00
        address[1]  = (unsigned int8)((write_address>>16) & 0xff);                     // 0x 00 _ _ 00 00
        address[2]  = (unsigned int8)((write_address>>8) & 0xff);                      // 0x 00 00 _ _ 00
        address[3]  = (unsigned int8)((write_address) & 0xff);                         // 0x 00 00 00 _ _

        //Write enable sequence
        output_low(FLASH1G_CS);
        spi_xfer(FLASH1G,WRITE_ENABLE);
        output_high(FLASH1G_CS);
        //Write enabled

        output_low(FLASH1G_CS);
        ////////////////////////////////////////////////////////////////
        spi_xfer(FLASH1G,PAGE_PROGRAM_4BYTE);                                               //PAGE WRITE COMAND  (0x12)
        spi_xfer(FLASH1G,address[0]);
        spi_xfer(FLASH1G,address[1]);
        spi_xfer(FLASH1G,address[2]);
        spi_xfer(FLASH1G,address[3]);
        spi_xfer(FLASH1G,write_data);
        ////////////////////////////////////////////////////////////////

        output_high(FLASH1G_CS);          //take CS PIN higher back

        while((status_register_flash1g() & 0x01) == 1)                                  //masking status bit
        {
            if(timeout_counter > 10)
            {
                delay_ms(200);
            }
            else
            {
                delay_ms(10);
            }

            if(timeout_counter > 100)
            {
                #ifdef DEBUG
                    fprintf(DEBUG_PORT,"flash timeout");
                #endif
                break;
            }
            timeout_counter++;
        }
        return;
    }

    void write_data_bytes_flash1g(unsigned int32 write_address,int8 amount,int8 *write_data)
    {
        unsigned int8 address[4];
        unsigned int8 timeout_counter = 0;

        //Byte extraction
        address[0]  = (unsigned int8)((write_address>>24) & 0xff);                     // 0x _ _ 00 00 00
        address[1]  = (unsigned int8)((write_address>>16) & 0xff);                     // 0x 00 _ _ 00 00
        address[2]  = (unsigned int8)((write_address>>8) & 0xff);                      // 0x 00 00 _ _ 00
        address[3]  = (unsigned int8)((write_address) & 0xff);                         // 0x 00 00 00 _ _

        //Write enable sequence
        output_low(FLASH1G_CS);
        spi_xfer(FLASH1G,WRITE_ENABLE);
        output_high(FLASH1G_CS);
        //Write enabled

        output_low(FLASH1G_CS);
        ////////////////////////////////////////////////////////////////
        spi_xfer(FLASH1G,PAGE_PROGRAM_4BYTE);                                               //PAGE WRITE COMAND  (0x12)
        spi_xfer(FLASH1G,address[0]);
        spi_xfer(FLASH1G,address[1]);
        spi_xfer(FLASH1G,address[2]);
        spi_xfer(FLASH1G,address[3]);
        for(int8 i = 0; i < amount; i++)
        {
            spi_xfer(FLASH1G,write_data[i]);
        }
        ////////////////////////////////////////////////////////////////
        output_high(FLASH1G_CS);          //take CS PIN higher back

        while((status_register_flash1g() & 0x01) == 1)                                  //masking status bit
        {
            if(timeout_counter > 10)
            {
                delay_ms(200);
            }
            else
            {
                delay_ms(10);
            }

            if(timeout_counter > 100)
            {
                #ifdef DEBUG
                    fprintf(DEBUG_PORT,"flash timeout");
                #endif
                break;
            }
            timeout_counter++;
        }
        return;
    }
#endif

#ifdef USE_FLASH128M
    int8 status_register_flash128m()
    {
        unsigned int8 status_reg;
        output_low(FLASH128M_CS);                                              //lower the CS PIN
        spi_xfer(FLASH128M,READ_STATUS_REGISTER);                              //READ STATUS REGISTER COMAND (0x05)
        status_reg = spi_xfer(FLASH128M);
        output_high(FLASH128M_CS);                                             //take CS PIN higher back
        #ifdef DEBUG
            if((status_reg & 0x01) == true)                                    //masking status bit
            {
                fprintf(DEBUG_PORT,"128Mbit flash busy\n\r");
            }
        #endif
        return status_reg;
    }

    int8 read_id_flash128m()//->success:True,fail:false
    {
        int8 chip_id[21];
        output_low(FLASH128M_CS);
        for (int8 i = 0; i <= 20; i++)
        {
            if (i == 0)
            {
                chip_id[i] = spi_xfer(FLASH128M,READ_ID);
                #ifdef DEBUG
                    fprintf(DEBUG_PORT,"Read ID:");
                #endif
            }
            else
            {
                chip_id[i] = spi_xfer(FLASH128M,0x00);
                #ifdef DEBUG
                    fprintf(DEBUG_PORT,"%x ",chip_id[i]);
                #endif
            }
        }
        output_high(FLASH128M_CS);
        #ifdef DEBUG
            fprintf(DEBUG_PORT,"\r\n");
        #endif
        //chip id check
        if(chip_id[1] == 0x20)
        {
            #ifdef DEBUG
                fprintf(DEBUG_PORT,"flash connect OK\r\n");
            #endif
            return true;
        }
        else
        {
            #ifdef DEBUG
                fprintf(DEBUG_PORT,"flash not connect\r\n");
            #endif
            return false;
        }
    }

    void sector_erase_flash128m(unsigned int32 sector_address)
    {
        unsigned int8 address[3];
        unsigned int8 timeout_counter = 0;

        address[0]  = (unsigned int8)((sector_address>>16) & 0xff);   // 0x 00 _ _ 00 00
        address[1]  = (unsigned int8)((sector_address>>8) & 0xff);    // 0x 00 00 _ _ 00
        address[2]  = (unsigned int8)((sector_address) & 0xff);       // 0x 00 00 00 _ _

        //Write enable sequence
        output_low(FLASH128M_CS);
        spi_xfer(FLASH128M,WRITE_ENABLE);
        output_high(FLASH128M_CS);
        //Write enabled

        delay_us(2);

        output_low(FLASH128M_CS);
        ///////////////////////////////////////////////////////////////////
        spi_xfer(FLASH128M,SECTOR_ERASE); //SECTOR ERASE COMAND   (0xD8)

        spi_xfer(FLASH128M,address[0]);
        spi_xfer(FLASH128M,address[1]);
        spi_xfer(FLASH128M,address[2]);
        //////////////////////////////////////////////////////////////////

        output_high(FLASH128M_CS);            //take CS PIN higher back

        while((status_register_flash128m() & 0x01) == 1)             //masking status bit
        {
            if(timeout_counter > 10)
            {
                delay_ms(200);
            }
            else
            {
                delay_ms(10);
            }

            if(timeout_counter > 100)
            {
                #ifdef DEBUG
                    fprintf(DEBUG_PORT,"flash timeout");
                #endif
                break;
            }
            timeout_counter++;
        }
        return;
    }

    void subsector_32kByte_erase_flash128m(unsigned int32 subsector_address)
    {
        unsigned int8 address[3];
        unsigned int8 timeout_counter = 0;

        address[0]  = (unsigned int8)((subsector_address>>16) & 0xff);   // 0x 00 _ _ 00 00
        address[1]  = (unsigned int8)((subsector_address>>8) & 0xff);    // 0x 00 00 _ _ 00
        address[2]  = (unsigned int8)((subsector_address) & 0xff);       // 0x 00 00 00 _ _

        //Write enable sequence
        output_low(FLASH128M_CS);
        spi_xfer(FLASH128M,WRITE_ENABLE);
        output_high(FLASH128M_CS);
        //Write enabled

        output_low(FLASH128M_CS);
        ///////////////////////////////////////////////////////////////////
        spi_xfer(FLASH128M,SUBSECTOR_32KB_ERASE);

        spi_xfer(FLASH128M,address[0]);
        spi_xfer(FLASH128M,address[1]);
        spi_xfer(FLASH128M,address[2]);
        //////////////////////////////////////////////////////////////////

        output_high(FLASH128M_CS);            //take CS PIN higher back

        while((status_register_flash128m() & 0x01) == 1)             //masking status bit
        {
            if(timeout_counter > 10)
            {
                delay_ms(200);
            }
            else
            {
                delay_ms(10);
            }

            if(timeout_counter > 100)
            {
                #ifdef DEBUG
                    fprintf(DEBUG_PORT,"flash timeout");
                #endif
                break;
            }
            timeout_counter++;
        }

    }

    void subsector_4kByte_erase_flash128m(unsigned int32 subsector_address)
    {
        unsigned int8 address[3];
        unsigned int8 timeout_counter = 0;

        address[0]  = (unsigned int8)((subsector_address>>16) & 0xff);   // 0x 00 _ _ 00 00
        address[1]  = (unsigned int8)((subsector_address>>8) & 0xff);    // 0x 00 00 _ _ 00
        address[2]  = (unsigned int8)((subsector_address) & 0xff);       // 0x 00 00 00 _ _

        //Write enable sequence
        output_low(FLASH128M_CS);
        //delay_ms(2);
        spi_xfer(FLASH128M,WRITE_ENABLE);
        output_high(FLASH128M_CS);
        //Write enabled

        delay_us(2);

        output_low(FLASH128M_CS);
        ///////////////////////////////////////////////////////////////////
        spi_xfer(FLASH128M,SUBSECTOR_4KB_ERASE);

        spi_xfer(FLASH128M,address[0]);
        spi_xfer(FLASH128M,address[1]);
        spi_xfer(FLASH128M,address[2]);
        //////////////////////////////////////////////////////////////////

        output_high(FLASH128M_CS);            //take CS PIN higher back

        while((status_register_flash128m() & 0x01) == 1)             //masking status bit
        {
            if(timeout_counter > 10)
            {
                delay_ms(200);
            }
            else
            {
                delay_ms(10);
            }

            if(timeout_counter > 100)
            {
                #ifdef DEBUG
                    fprintf(DEBUG_PORT,"flash timeout");
                #endif
                break;
            }
            timeout_counter++;
        }
    }

    int8 read_data_byte_flash128m(unsigned int32 read_address)
    {
        int8 read_data;
        unsigned int8 address[3];

        address[0]  = (unsigned int8)((read_address>>16) & 0xff);   // 0x 00 _ _ 00 00
        address[1]  = (unsigned int8)((read_address>>8) & 0xff);    // 0x 00 00 _ _ 00
        address[2]  = (unsigned int8)((read_address) & 0xff);       // 0x 00 00 00 _ _

        output_low(FLASH128M_CS);
        ///////////////////////////////////////////////////////////////////
        spi_xfer(FLASH128M,READ);

        spi_xfer(FLASH128M,address[0]);
        spi_xfer(FLASH128M,address[1]);
        spi_xfer(FLASH128M,address[2]);
        read_data = spi_xfer(FLASH128M);
        //////////////////////////////////////////////////////////////////

        delay_us(2);
        output_high(FLASH128M_CS);            //take CS PIN higher back

        return read_data;
    }

    void read_data_bytes_flash128m(unsigned int32 read_address, int8 amount, int8 *read_data)
    {
        unsigned int8 address[3];

        address[0]  = (unsigned int8)((read_address>>16) & 0xff);   // 0x 00 _ _ 00 00
        address[1]  = (unsigned int8)((read_address>>8) & 0xff);    // 0x 00 00 _ _ 00
        address[2]  = (unsigned int8)((read_address) & 0xff);       // 0x 00 00 00 _ _

        output_low(FLASH128M_CS);
        ///////////////////////////////////////////////////////////////////
        spi_xfer(FLASH128M,READ);

        spi_xfer(FLASH128M,address[0]);
        spi_xfer(FLASH128M,address[1]);
        spi_xfer(FLASH128M,address[2]);
        for(int8 i = 0; i < amount; i++)
        {
            read_data[i] = spi_xfer(FLASH128M);
        }
        //////////////////////////////////////////////////////////////////

        delay_us(2);
        output_high(FLASH128M_CS);            //take CS PIN higher back

        return;
    }

    void write_data_byte_flash128m(unsigned int32 write_address, int8 write_data)
    {
        unsigned int8 address[3];
        unsigned int8 timeout_counter = 0;
        //Byte extraction
        address[0]  = (unsigned int8)((write_address>>16) & 0xff);                     // 0x 00 _ _ 00 00
        address[1]  = (unsigned int8)((write_address>>8) & 0xff);                      // 0x 00 00 _ _ 00
        address[2]  = (unsigned int8)((write_address) & 0xff);                         // 0x 00 00 00 _ _

        //Write enable sequence
        output_low(FLASH128M_CS);
        spi_xfer(FLASH128M,WRITE_ENABLE);
        output_high(FLASH128M_CS);
        //Write enabled

        output_low(FLASH128M_CS);
        ////////////////////////////////////////////////////////////////
        spi_xfer(FLASH128M,PAGE_PROGRAM);                                              //PAGE WRITE COMAND  (0x02)
        spi_xfer(FLASH128M,address[0]);
        spi_xfer(FLASH128M,address[1]);
        spi_xfer(FLASH128M,address[2]);
        spi_xfer(FLASH128M,write_data);
        ////////////////////////////////////////////////////////////////

        output_high(FLASH128M_CS);          //take CS PIN higher back

        while((status_register_flash128m() & 0x01) == 1)                               //masking status bit
        {
            if(timeout_counter > 10)
            {
                delay_ms(200);
            }
            else
            {
                delay_ms(10);
            }

            if(timeout_counter > 100)
            {
                #ifdef DEBUG
                    fprintf(DEBUG_PORT,"flash timeout");
                #endif
                break;
            }
            timeout_counter++;
        }
        //delay_us(10);

        return;
    }

    void write_data_bytes_flash128m(unsigned int32 write_address, int8 amount, int8 *write_data)
    {
        unsigned int8 address[3];
        unsigned int8 timeout_counter = 0;
        //Byte extraction
        address[0]  = (unsigned int8)((write_address>>16) & 0xff);                     // 0x 00 _ _ 00 00
        address[1]  = (unsigned int8)((write_address>>8) & 0xff);                      // 0x 00 00 _ _ 00
        address[2]  = (unsigned int8)((write_address) & 0xff);                         // 0x 00 00 00 _ _

        //Write enable sequence
        output_low(FLASH128M_CS);
        spi_xfer(FLASH128M,WRITE_ENABLE);
        output_high(FLASH128M_CS);
        //Write enabled

        output_low(FLASH128M_CS);
        ////////////////////////////////////////////////////////////////
        spi_xfer(FLASH128M,PAGE_PROGRAM);                                              //PAGE WRITE COMAND  (0x02)
        spi_xfer(FLASH128M,address[0]);
        spi_xfer(FLASH128M,address[1]);
        spi_xfer(FLASH128M,address[2]);
        for(int8 i = 0; i < amount; i++)
        {
            spi_xfer(FLASH128M,write_data[i]);
        }
        ////////////////////////////////////////////////////////////////
        output_high(FLASH128M_CS);          //take CS PIN higher back

        while((status_register_flash128m() & 0x01) == 1)                               //masking status bit
        {
            if(timeout_counter > 10)
            {
                delay_ms(200);
            }
            else
            {
                delay_ms(10);
            }

            if(timeout_counter > 100)
            {
                #ifdef DEBUG
                    fprintf(DEBUG_PORT,"flash timeout");
                #endif
                break;
            }
            timeout_counter++;
        }
        return;
    }

#endif