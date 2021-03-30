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
#include "circ_buffer.h"
/******************************************************************************
 * Definitions
 ******************************************************************************/
#define LCD_DISPLAY_INIT_CMD_SIZE 4 /**< number of init commands */

/**
 * @brief Clears entire display and sets DDRAM address 0 in
 * address counter
 */
#define LCD_DISPLAY_CMD_CLEAR 0x01 /**< a command to clear display */

/**
 * @brief a command to choose 4-bit interface 
 * 
 * @details There are two ways of interfacing the LCD. The first one is the 
 * 8-bit. The other one is the 4-bit which is the used in this module.
 * The 4-bit is better in using fewer numbers of IO pins than the 8-bit.
 * This command is for configuring the LCD for 4-bit
 */
#define LCD_DISPLAY_CMD_4BIT 0x28 

/**
 * @brief a command to turn the display on and the cursor off.
 */
#define LCD_DISPLAY_CMD_ON 0x0C

/**
 * @brief Increment display address for each character but do not scroll
 */
#define LCD_DISPLAY_CMD_INC 0x06
/******************************************************************************
 * Typedefs
 ******************************************************************************/
/**
 * @brief enumeration for the the data type that can be sent to LCD. It can 
 *be either data or command 
 */
typedef enum
{
  LCD_DATA_FLAG_DATA,
  LCD_DATA_FLAG_CMD,
  LCD_DATA_FLAG_MAX
} LcdDataFlag_t;
/******************************************************************************
 * Module variable definitions
 ******************************************************************************/
const LcdDisplayConfig_t* gConfig; /**< a pointer to the configuration table */

/**
 * @brief the Lcd displays data and commands buffers.
 * The most significant bit determines if the byte is a command or data. 
 * If it's 1, then the byte is a command.
 */
static uint8_t LcdDisplayData[LCD_DISPLAY_MAX][LCD_DISPLAY_BUFF_SIZE];

/**
 * @brief the lcd displays data and commands buffers structures
 */
static CircBuff_t LcdDisplayBuff[LCD_DISPLAY_MAX];

/**
 * @brief The command that's required for LCD display initialization
 */
static const uint8_t LcdDisplayInitCmds[LCD_DISPLAY_INIT_CMD_SIZE] =
{
  LCD_DISPLAY_CMD_CLEAR,
  LCD_DISPLAY_CMD_4BIT,
  LCD_DISPLAY_CMD_ON,
  LCD_DISPLAY_CMD_INC
};

/******************************************************************************
 * Functions Prototypes
 ******************************************************************************/
static void LcdDisplay_SendByte(LcdDisplay_t Display, uint8_t Data,
 LcdDataFlag_t Flag);
static void LcdDisplay_Delay(void);
static void LcdDisplay_SetCommand(LcdDisplay_t Display, uint8_t Command);
/******************************************************************************
 * Functions definitions
 ******************************************************************************/
extern void 
LcdDisplay_Init(const LcdDisplayConfig_t * const Config)
{
  if(!(Config != 0x00))
    {
      //TODO: handle this error
      return;
    }

  LcdDisplay_t Display;
  uint8_t res;
  uint8_t cmd;

  //initialize the buffers
  for(Display = 0; Display < LCD_DISPLAY_MAX; Display++)
    {
      LcdDisplayBuff[Display] = CircBuff_Create(LcdDisplayData[Display],
       LCD_DISPLAY_BUFF_SIZE);
    }
  
  //add init commands
  for(Display = 0; Display < LCD_DISPLAY_MAX; Display++)
    {
      for(cmd = 0; cmd < LCD_DISPLAY_INIT_CMD_SIZE; cmd++)
        {
          LcdDisplay_SetCommand(Display, LcdDisplayInitCmds[cmd]);
        }
    }
}

extern void 
LcdDisplay_Clear(LcdDisplay_t Display)
{
  if(!(Display < LCD_DISPLAY_MAX))
    {
      //TODO: handle this error
      return;
    }

  LcdDisplay_SetCommand(Display, LCD_DISPLAY_CMD_CLEAR);
}

static void 
LcdDisplay_SetCommand(LcdDisplay_t Display, uint8_t Command)
{
 if(!(Display < LCD_DISPLAY_MAX))
    {
      //TODO: handle this error
      return;
    }
  
  uint8_t res;

  Command = Command | 0x80;
  res = CircBuff_Enqueue(&LcdDisplayBuff[Display], Command);
  if(res == 0) 
    {
      //TODO handle this error
      return;
    }
}

static void 
LcdDisplay_SendByte(LcdDisplay_t Display, uint8_t Data, LcdDataFlag_t Flag)
{
  if(!(Display < LCD_DISPLAY_MAX && Flag < LCD_DATA_FLAG_MAX))
    {
      //TODO: handle this error
      return;
    }
  
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
