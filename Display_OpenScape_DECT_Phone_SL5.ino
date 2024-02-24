/*display from OpenScape DECT Phone SL5.
 Controller seems similar to ST7789V or ILI9341 but clock ~20MHz(write cycle 48,5ns) where write cycle for ST7789V min 66ns and ILI9341 min 100ns?
Arduino test sketch hardware SPI
*/
#include <SPI.h>
#define RDDST     0x09// Read Display Status
#define SLPIN 		0x10// Sleep In
#define SLPOUT 		0x11// Sleep Out
#define DISPON 		0x29// Display On
#define CASET 		0x2A// Column Address Set
#define RASET 		0x2B// Row Address Set
#define RAMWR 		0x2C// Memory Write
#define MADCTL 		0x36// Memory Data Access Control
#define COLMOD 		0x3A// Interface Pixel Format
#define PORCTRL 	0xB2// Porch Setting
#define GCTRL 		0xB7// Gate Control
#define VCOMS 		0xBB// VCOM Setting
#define LCMCTRL 	0xC0// LCM Control
#define VDVVRHEN 	0xC2// VDV and VRH Command Enable
#define VRHS 		  0xC3// VRH Set
#define VDVS 		  0xC4// VDV Set
#define FRCTRL2 	0xC6// Frame Rate Control in Normal Mode
#define PWCTRL1 	0xD0// Power Control 1
#define PVGAMCTRL 0xE0// Positive Voltage Gamma Control
#define NVGAMCTRL 0xE1// Negative Voltage Gamma Control

//some colors
#define  BLUE     0x001F 
#define  GREEN    0x07E0 
#define  RED      0xF800 
#define  WHITE    0xFFFF
#define  BLACK    0x0000

//arduino pins
#define CSX   10// select chip 
#define DCX   9// data/command
#define RESX  8// reset

//#define SCL   13//clock
//#define SDA   11//data output

const uint8_t gamma_plus[] PROGMEM = {0xD0,0x00,0x05,0x0E,0x15,0x0D,0x37,0x43,0x47,0x09,0x15,0x12,0x16,0x19};
const uint8_t gamma_minus[] PROGMEM = {0xD0,0x00,0x05,0x0D,0x0C,0x06,0x2D,0x44,0x40,0x0E,0x1C,0x18,0x16,0x19};

void send_data(uint8_t bits)
{
  digitalWrite(DCX, 1);//set D/C 1 = data
  SPI.transfer(bits);//send data bits   
  digitalWrite(DCX, 0);//set D/C 0 = command
}

void send_command(uint8_t bits)
{
  SPI.transfer(bits);//send data bits 
}

void setup()
{
  //init
  pinMode(CSX, OUTPUT);
  pinMode(RESX, OUTPUT); 
  pinMode(DCX, OUTPUT);   
  digitalWrite(RESX, 1);
  digitalWrite(CSX, 1);
  digitalWrite(DCX, 0);

  //reset 
  digitalWrite(RESX, 0);
  delay(1);
  digitalWrite(RESX, 1);
  delay(120); //normal delay after reset if reset and cs in software
  
  digitalWrite(CSX, 0);  
  delay(400); //when reset and cs in hardware so you need this delay
  SPI.begin();
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  
  send_command(SLPOUT);
  //delay(1);
  send_command(PORCTRL);
  send_data(0x0C); send_data(0x0C); send_data(0x00); send_data(0x33); send_data(0x33);//
  
  send_command(GCTRL);
  send_data(0x35);
  
  send_command(VCOMS);
  send_data(0x2B);// 
  
  send_command(LCMCTRL); 
  send_data(0x2C);//
  
  send_command(VDVVRHEN); 
  send_data(0x01);//
  
  send_command(VRHS); 
  send_data(0x11);//
  
  send_command(VDVS); 
  send_data(0x20);//
  
  send_command(FRCTRL2); 
  send_data(0x0F);//
  
  send_command(PWCTRL1); 
  send_data(0xA4); send_data(0xA1);//
    
  send_command(PVGAMCTRL);//Gamma adjustment (+ polarity)
  for(uint8_t i = 0; i < 13; i++)
  {
    send_data(pgm_read_byte_near(gamma_plus + i));
  }

  send_command(NVGAMCTRL);//Gamma adjustment (- polarity)
  for(uint8_t i = 0; i < 13; i++)
  {
    send_data(pgm_read_byte_near(gamma_minus + i));
  }
   
  send_command(MADCTL);
  send_data(0xC0);
  
  send_command(COLMOD);
  send_data(0x05);
  
  send_command(DISPON);
  
  send_command(RASET);  
  send_data(0x00);  send_data(0x00);//0
  send_data(0x01);  send_data(0x3F);//319
  
  send_command(RAMWR);
  //fill display black
  for(long i = 0; i < 76800; i++)//320*240
  {
    send_data(BLACK>>8); send_data(BLACK);//
  }
  
  //print char A
  const uint8_t a[] = {0x18, 0x24, 0x42, 0x81, 0xFF, 0x81, 0x81, 0x81};
  /*  
  00011000
  00100100
  01000010  
  10000001
  11111111
  10000001
  10000001
  10000001
  */
  uint8_t line;

  send_command(CASET);
  send_data(0x00);  send_data(0x00);//0
  send_data(0x00);  send_data(0x07);//7
  
  send_command(RASET);
  send_data(0x00);  send_data(0x00);//0
  send_data(0x00);  send_data(0x07);//7
  
  send_command(RAMWR);
  
  for(uint8_t i = 0; i < 8; i++)
  {
    line = a[i];
    for(uint8_t j = 0; j < 8; j++)
    {
      if((line & 0x80) == 0x80)
      {
        send_data(0xFF); send_data(0xFF);        
      }
      else
      {
        send_data(0x00); send_data(0x00);
      }
      line <<= 1;
    }    
  }
  SPI.endTransaction();
  SPI.end();
}

void loop()
{

}