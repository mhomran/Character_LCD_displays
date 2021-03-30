/**
 * @file lcd_display.c
 * @author Mohamed Hassanin
 * @brief Character LCD displays pattern for time-triggered embedded systems.
 * @version 0.1
 * @date 2021-03-30
 */
/******************************************************************************
 * Includes
 ******************************************************************************/
#include "lcd_display.h"
/******************************************************************************
 * Typedefs
 ******************************************************************************/
typedef enum
{
  LCD_DATA_FLAG_DATA,
  LCD_DATA_FLAG_INST,
  LCD_DATA_FLAG_MAX
} LcdDataFlag_t;
/******************************************************************************
 * Module variable definitions
 ******************************************************************************/
const LcdDisplayConfig_t* gConfig;
/******************************************************************************
 * Functions Prototypes
 ******************************************************************************/
static void LcdDisplay_SendByte(LcdDisplay_t Display, uint8_t Data,
 LcdDataFlag_t Flag);
static void LcdDisplay_Delay(void);
/******************************************************************************
 * Functions definitions
 ******************************************************************************/
static void 
LcdDisplay_SendByte(LcdDisplay_t Display, uint8_t Data, LcdDataFlag_t Flag)
{
  uint8_t DataCh;
  int8_t Nibble;
  for(Nibble = 1; Nibble >= 0; Nibble--)
    {
      for(DataCh = 0; DataCh < LCD_DISPLAY_BITLEN; DataCh++)
        {
          if(Data & (1 << (DataCh + (LCD_DISPLAY_BITLEN * Nibble))))
            {
              Dio_ChannelWrite(gConfig[Display].Data[DataCh], DIO_STATE_HIGH);
            }
          else
            {
              Dio_ChannelWrite(gConfig[Display].Data[DataCh], DIO_STATE_LOW);
            }
        }
      
      if (Flag == LCD_DATA_FLAG_DATA)
        {
          Dio_ChannelWrite(gConfig[Display].Rs, DIO_STATE_LOW);
        }
      else
        {
          Dio_ChannelWrite(gConfig[Display].Rs, DIO_STATE_HIGH);
        }

      //latch
      Dio_ChannelWrite(gConfig[Display].En, DIO_STATE_HIGH);
      LcdDisplay_Delay();
      Dio_ChannelWrite(gConfig[Display].En, DIO_STATE_LOW);
      LcdDisplay_Delay();
    }
}

static void 
LcdDisplay_Delay(void)
{
  //dummy delay
  uint8_t x = 0;
  x++;
  x++;
  x++;
}
/*****************************End of File ************************************/
