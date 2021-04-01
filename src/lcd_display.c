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
#define LCD_DISPLAY_INIT_CMD_SIZE 5 /**< number of init commands */


/**
 * Set DDRAM address to "00H"
 */
#define LCD_DISPLAY_CMD_ADDRESS_RESET 0x02

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

/**
 * @brief Clears entire display and sets DDRAM address 0 in
 * address counter
 */
#define LCD_DISPLAY_CMD_CLEAR 0x01 /**< a command to clear display */
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
/**
 * a pointer to the configuration table
 */
static const LcdDisplayConfig_t* gConfig;
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
 * @brief The command that's required for LCD display initialization. 
 * Orders of the commands matter
 */
static const uint8_t LcdDisplayInitCmds[LCD_DISPLAY_INIT_CMD_SIZE] =
{
  LCD_DISPLAY_CMD_ADDRESS_RESET,
  LCD_DISPLAY_CMD_4BIT,
  LCD_DISPLAY_CMD_ON,
  LCD_DISPLAY_CMD_INC,
  LCD_DISPLAY_CMD_CLEAR
};


/**
 * @brief variables to keep track of DDRAM addresses of displays
 */
static uint8_t LcdDisplayLine[LCD_DISPLAY_MAX];

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
  uint8_t cmd;

  //assign the internal config pointer
  gConfig = Config;

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

      LcdDisplayLine[Display] = 0;
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
LcdDisplay_Delay(void)
{
  //dummy delay
  uint8_t x = 0;
  x++;
  x++;
  x++;
}

extern uint8_t LcdDisplay_SetData(const LcdDisplay_t Display,
				  const uint8_t* const Data,
				  const uint8_t DataSize)
{
  if(!(Data != 0x00 && Display < LCD_DISPLAY_MAX))
  {
    //TODO: Handle this error
    return 0;
  }

  if(DataSize == 0) return 0;

  uint8_t res;
  uint8_t i = 0;
  uint8_t ModifiedData;

  do{
    //Make the most significant bit 0 to mark as a data not a command
    ModifiedData = Data[i] & 0x7F;

    res = CircBuff_Enqueue(&LcdDisplayBuff[Display], ModifiedData);

    i = (res == 1) ? (i + 1) : i;
  } while(res == 1 && i < DataSize);

  return i;
}

extern void 
LcdDisplay_Update(void)
{
  LcdDisplay_t Display;
  uint8_t Data;
  uint8_t res;

  for(Display = LCD_DISPLAY_0; Display < LCD_DISPLAY_MAX; Display++)
    {
      // Find the next data/command
      res = CircBuff_Dequeue(&LcdDisplayBuff[Display], &Data);
      if(res == 0) continue;

      //Is it data or command
      if(Data & 0x80)
	{
	  Data = Data & 0x7F;
	  LcdDisplay_SendByte(Display, Data, LCD_DATA_FLAG_CMD);
	}
      else
	{
	  LcdDisplay_SendByte(Display, Data, LCD_DATA_FLAG_DATA);
	}
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
  uint8_t Nibble;

  for(Nibble = 2; Nibble >= 1; Nibble--)
    {
      for(DataCh = 0; DataCh < LCD_DISPLAY_BITLEN; DataCh++)
        {
          if(Data & (1 << (DataCh + (LCD_DISPLAY_BITLEN * (Nibble - 1)))))
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
          Dio_ChannelWrite(gConfig[Display].Rs, DIO_STATE_HIGH);
        }
      else
        {
          Dio_ChannelWrite(gConfig[Display].Rs, DIO_STATE_LOW);
        }

      //latch
      Dio_ChannelWrite(gConfig[Display].En, DIO_STATE_HIGH);
      LcdDisplay_Delay();
      Dio_ChannelWrite(gConfig[Display].En, DIO_STATE_LOW);
      LcdDisplay_Delay();
    }
}

/*****************************End of File ************************************/
