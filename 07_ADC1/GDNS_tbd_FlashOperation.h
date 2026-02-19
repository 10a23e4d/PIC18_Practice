#define DEBUG
#define USE_FLASH1G     //MT25QL01GBBB:SMF,CF
//#define USE_FLASH128M   //MT25QL128ABA:PICF,GF

//////////////////////////////port select//////////////////////////////////////
#ifdef DEBUG
   #define DEBUG_PORT PORT1
#endif

#ifdef USE_FLASH1G
   #define FLASH1G SPI2//port for 1 Gbit Flash
   #define FLASH1G_CS PIN_B3
#endif

#ifdef USE_FLASH128M
   #define FLASH128M SPI2//port for 128 Mbit Flash
   #define FLASH128M_CS PIN_B3
#endif

////////////////////////////////CMD List///////////////////////////////////////
#define READ_ID                     0x9F
#define READ_STATUS_REGISTER        0x05
#define READ                        0x03
#define READ_4BYTE                  0x13
#define WRITE_ENABLE                0x06
#define PAGE_PROGRAM                0x02
#define PAGE_PROGRAM_4BYTE          0x12
#define SUBSECTOR_4KB_ERASE         0x20
#define SUBSECTOR_4KB_ERASE_4BYTE   0x21
#define SUBSECTOR_32KB_ERASE        0x52
#define SUBSECTOR_32KB_ERASE_4BYTE  0x5C
#define SECTOR_ERASE                0xD8
#define SECTOR_ERASE_4BYTE          0xDC

///////////////////////////function List///////////////////////////////////////
#ifdef USE_FLASH1G
   int8 status_register_flash1g();                                                                       //bit[7:2]status register control bit,bit[1]write enable bit T:ennable F:disable,bit[0]write in progress bit T:busy F:ready
   int8 read_id_flash1g();                                                                               //T:Flash id read successfully(connect OK),F:Flash id read failed
   void sector_erase_flash1g(unsigned int32 sector_address);                                             //64kByte erase
   void subsector_32kByte_erase_flash1g(unsigned int32 subsector_address);                               //32kByte erase
   void subsector_4kByte_erase_flash1g(unsigned int32 subsector_address);                                //4kByte erase
   int8 read_data_byte_flash1g(unsigned int32 read_address);                                             //read 1 Byte data
   void read_data_bytes_flash1g(unsigned int32 read_address,unsigned int32 amount,int8 *read_data);      //read multi Bytes data(max 128 MByte) and store to "read data"
   void write_data_byte_flash1g(unsigned int32 write_address,int8 write_data);                           //write 1 Byte data to flash
   void write_data_bytes_flash1g(unsigned int32 write_address,int8 amount,int8 *write_data);             //write multi Bytes data(max128 Byte) to flash
#endif
   
#ifdef USE_FLASH128M
   int8 status_register_flash128m();                                                                     //bit[7:2]status register control bit,bit[1]write enable bit T:ennable F:disable,bit[0]write in progress bit T:busy F:ready
   int8 read_id_flash128m();                                                                             //T:Flash id read successfully(connect OK),F:Flash id read failed
   void sector_erase_flash128m(unsigned int32 sector_address);                                           //64kByte erase
   void subsector_32kByte_erase_flash128m(unsigned int32 subsector_address);                             //32kByte erase
   void subsector_4kByte_erase_flash128m(unsigned int32 subsector_address);                              //4kByte erase
   int8 read_data_byte_flash128m(unsigned int32 read_address);                                           //read 1 Byte data
   void read_data_bytes_flash128m(unsigned int32 read_address,unsigned int32 amount,int8 *read_data);    //read multi Bytes data(max 128 MByte) and store to "read data"
   void write_data_byte_flash128m(unsigned int32 write_address,int8 write_data);                         //write 1 Byte data to flash
   void write_data_bytes_flash128m(unsigned int32 write_address,int8 amount,int8 *write_data);           //write multi Bytes data(max128 Byte) to flash
#endif
   
