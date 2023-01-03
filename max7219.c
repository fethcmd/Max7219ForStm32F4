#include "max7219.h"
#include <stdint.h>
#include <string.h>
#include "main.h"

void MAX7219_ShutdownStart (void)
{
  MAX7219_Write(REG_SHUTDOWN, 0);                     
}

void MAX7219_DisplayTestStart (void)
{
  MAX7219_Write(REG_DISPLAY_TEST, 1);                 
}

void Clear(void) {
    
    for(int i=0;i<8;i++) {
        MAX7219_Write(i+1,0x00);
    }
}

void MAX7219_DisplayTestStop (void)
{
  MAX7219_Write(REG_DISPLAY_TEST, 0);                 
}

void MAX7219_SetBrightness (char brightness)
{
  brightness &= 0x0f;                                 
  MAX7219_Write(REG_INTENSITY, brightness);           
}

unsigned char MAX7219_LookupCode (char character, unsigned int dp)
{
  int i;
  unsigned int d=0;
 if (dp) d=1;  
  if (character>=35 && character<=44) {
    character+=13;
    d=1;
  }
  for (i = 0; MAX7219_Font[i].ascii; i++)              
    if (character == MAX7219_Font[i].ascii){
      if (d){
        d=MAX7219_Font[i].segs;
        d |= (1<<7);
        return (d);                                    
        }
      else{
        return MAX7219_Font[i].segs;                    
      }
    }
      
  return 0;                                             
}

void DisplayText(char *text, int justify){
  int decimal[16];
  char trimStr[16] = "";
  int x,y=0;
  int s;
  
  s=strlen(text);
  if (s>16) s=16;
  for (x=0;x<s;x++){
    if (text[x]=='.'){
      decimal[y-1]=1;
      }
    else{
      trimStr[y]=text[x];
      decimal[y]=0;
      y++;
     }
  }
  if (y>8) y=8;
  for (x=0;x<y;x++){
      if (justify==0)
        DisplayChar((int)(8-y+x),trimStr[x], decimal[x]);
      if (justify==1){
        DisplayChar((int)(x),trimStr[x], decimal[x]);
      }
  }
}

void shiftOut(uint8_t val)
{
	uint8_t i;
	for (i = 0; i < 8; i++)  {
		HAL_GPIO_WritePin(SELECTED_GPIO,MAX_DIN,(val & 128) != 0);
		val <<= 1;
		HAL_GPIO_WritePin(SELECTED_GPIO,MAX_CLK,GPIO_PIN_SET);
		HAL_GPIO_WritePin(SELECTED_GPIO,MAX_CLK,GPIO_PIN_RESET);
	}
}

void MAX7219_Write(volatile unsigned char opcode, volatile unsigned char data) {
	HAL_GPIO_WritePin(SELECTED_GPIO,MAX_CS,GPIO_PIN_RESET);
    shiftOut(opcode);
    shiftOut(data);
	HAL_GPIO_WritePin(SELECTED_GPIO,MAX_CS,GPIO_PIN_SET);
}    

void DisplayChar(int digit, char value, unsigned int dp) {
      MAX7219_Write(digit+1,MAX7219_LookupCode(value, dp));
}

void MAX7219_ShutdownStop (void)
{
  MAX7219_Write(REG_SHUTDOWN, 1);   
}
 
void MAX7219_Begin()
{ 
  HAL_GPIO_WritePin(SELECTED_GPIO,MAX_CS,GPIO_PIN_SET);
  MAX7219_Write(REG_SCAN_LIMIT, 7);                   
  MAX7219_Write(REG_DECODE, 0x00);                    
  MAX7219_ShutdownStop();                             
  MAX7219_DisplayTestStop();                          
  Clear();                                    
  MAX7219_SetBrightness(INTENSITY_MAX);               
}
