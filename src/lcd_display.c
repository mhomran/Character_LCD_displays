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
/**
 * Command Identifier. If this byte is found in the buffer, then it's known
 * that the next byte is an instruction.
 */
#define LCD_DISPLAY_CMD_ID 0x80

/**
 * DDRAM Identifier. This is a mask used to set DDRAM address.
 */
#define LCD_DISPLAY_DDRAM_MASK 0x80

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

#define LCD_DISPLAY_DDRAM_LINE_0 0x00 /**< DDRAM Address for line 0 */
#define LCD_DISPLAY_DDRAM_LINE_1 0x40 /**< DDRAM Address for line 1 */
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
static uint8_t gData[LCD_DISPLAY_MAX][LCD_DISPLAY_BUFF_SIZE];

/**
 * @brief the lcd displays data and commands buffers structures
 */
static CircBuff_t gBuff[LCD_DISPLAY_MAX];

/**
 * @brief variables to keep track of cursor position
 */
static uint8_t gCursor[LCD_DISPLAY_MAX];

/**
 * @brief variables to keep track of the current line number of displays
 */
static uint8_t gLine[LCD_DISPLAY_MAX];

/******************************************************************************
 * Functions Prototypes
 ******************************************************************************/
static void LcdDisplay_SendByte(LcdDisplay_t Display, uint8_t Data,
 LcdDataFlag_t Flag);
static void LcdDisplay_Delay(void);
static void LcdDisplay_SetCommand(LcdDisplay_t Display, uint8_t Command);
static uint8_t LcdDisplay_CheckLine(LcdDisplay_t Display);
/******************************************************************************
 * Functions definitions
 ******************************************************************************/
/******************************************************************************
* Function : LcdDisplay_Init()
*//**
* \b Description: Initialization function for LCD Display module<br/>
* \b PRE-CONDITION: Configuration table is populated<br/>
* @param Config a pointer to the configuration table of the displays.
* @return void 
******************************************************************************/
extern void 
LcdDisplay_Init(const LcdDisplayConfig_t * const Config)
{
  if(!(Config != 0x00))
    {
      //TODO: handle this error
      return;
    }

  //The commands that're required for LCD display initialization. 
  //Orders of the commands matter
  const uint8_t InitCmds[] =
  {
    LCD_DISPLAY_CMD_ADDRESS_RESET,
    LCD_DISPLAY_CMD_4BIT,
    LCD_DISPLAY_CMD_ON,
    LCD_DISPLAY_CMD_INC,
    LCD_DISPLAY_CMD_CLEAR
  };

  LcdDisplay_t Display;
  uint8_t cmd;

  //assign the internal config pointer
  gConfig = Config;

  //initialize the buffers
  for(Display = 0; Display < LCD_DISPLAY_MAX; Display++)
    {
      gBuff[Display] = CircBuff_Create(gData[Display],
       LCD_DISPLAY_BUFF_SIZE);
    }
  
  //add init commands
  for(Display = 0; Display < LCD_DISPLAY_MAX; Display++)
    {
      for(cmd = 0; cmd < sizeof(InitCmds); cmd++)
        {
          LcdDisplay_SetCommand(Display, InitCmds[cmd]);
        }

      gCursor[Display] = 0;
      gLine[Display] = 0;
    }
}

/******************************************************************************
* Function : LcdDisplay_Clear()
*//**
* \b Description: Clear the Display and move the cursor to the first char<br/>
* \b PRE-CONDITION: LcdDisplay_Init is called properly <br/>
* @param Display The id of the display.
* @return void 
******************************************************************************/
extern void 
LcdDisplay_Clear(LcdDisplay_t Display)
{
  if(!(Display < LCD_DISPLAY_MAX))
    {
      //TODO: handle this error
      return;
    }

  gCursor[Display] = 0;
  gLine[Display] = 0;
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

  res = CircBuff_Enqueue(&gBuff[Display], LCD_DISPLAY_CMD_ID);
  if(res == 0) 
    {
      //TODO handle this error
      return;
    }

  res = CircBuff_Enqueue(&gBuff[Display], Command);
  if(res == 0)
    {
      //TODO handle this error
      return;
    }
}


/******************************************************************************
* Function : LcdDisplay_Delay()
*//**
* \b Description: Utility function used to make a small software delay for 
* the enable pin cycle of the LCD Display <br/>
* @return void 
******************************************************************************/
static void 
LcdDisplay_Delay(void)
{
  //dummy delay
  uint8_t x = 0;
  x++;
  x++;
  x++;
}

/******************************************************************************
* Function : LcdDisplay_SetData()
*//**
* \b Description: Set data in Lcd buffer to show it<br/>
* \b PRE-CONDITION: LcdDisplay_Init is called properly <br/>
* @param Display The id of the display.
* @param Data A pointer to the data to show.
* @param DataSize The number of characters to send
* @return uint8_t how many characters are sent
******************************************************************************/
extern uint8_t 
LcdDisplay_SetData(const LcdDisplay_t Display,
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
    ModifiedData = Data[i] & (~LCD_DISPLAY_CMD_ID);

    res = CircBuff_Enqueue(&gBuff[Display], ModifiedData);

    if(res == 1)
      {
        i++;
      }
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
      res = LcdDisplay_CheckLine(Display);
      if(res == 0) continue;

      // Find the next data/command
      res = CircBuff_Dequeue(&gBuff[Display], &Data);
      if(res == 0) continue;

      //Is it data or command
      if(Data == LCD_DISPLAY_CMD_ID)
        {
          res = CircBuff_Dequeue(&gBuff[Display], &Data);
          if(res == 0)
            {
              //TODO: handle this error.
              return;
            }

          LcdDisplay_SendByte(Display, Data, LCD_DATA_FLAG_CMD);
        }
      else
        {
          LcdDisplay_SendByte(Display, Data, LCD_DATA_FLAG_DATA);
          gCursor[Display]++;
        }
    }
}

/******************************************************************************
* Function : LcdDisplay_SendByte()
*//**
* \b Description: Utility function to send a char to show or a command to
* execute on the lcd display<br/>
* \b PRE-CONDITION: LcdDisplay_Init is called properly <br/>
* @param Display The id of the display.
* @param Data the command/char
* @param Flag A flag to differentiate between commands and data
* @return void 
******************************************************************************/
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

/******************************************************************************
* Function : LcdDisplay_CheckLine()
*//**
* \b Description: Utility function to check if the cursor position should 
* be updated. It updates the cursor postion if it reached the end of the 
* current line. If it the cursos reached the last line, the new cursor 
* position is at the first of the first line of the display.
* execute on the lcd display<br/>
* \b PRE-CONDITION: LcdDisplay_Init is called properly <br/>
* @param Display The id of the display.
* @return uint8_t 1 if there's no update, 0 otherwise
******************************************************************************/
static uint8_t
LcdDisplay_CheckLine(LcdDisplay_t Display)
{
  uint8_t res = 0;
  uint8_t NewAddress;
  if(gCursor[Display] == gConfig[Display].Width)
    {
      gLine[Display]++;
      gLine[Display] = gLine[Display] %
        gConfig[Display].Height;
      gCursor[Display] = 0;
      switch(gLine[Display])
      {
        case 0:
        NewAddress = LCD_DISPLAY_DDRAM_LINE_0;
        break;

        case 1:
        NewAddress = LCD_DISPLAY_DDRAM_LINE_1;
        break;

        case 2:
        NewAddress = LCD_DISPLAY_DDRAM_LINE_0 + gConfig[Display].Width;
        break;

        case 3:
        NewAddress = LCD_DISPLAY_DDRAM_LINE_1 + gConfig[Display].Width;
        break;

        default:
        //DO NOTHING
        break;
      }

      NewAddress = NewAddress | LCD_DISPLAY_DDRAM_MASK;
      LcdDisplay_SendByte(Display, NewAddress, LCD_DATA_FLAG_CMD);
    }
  else
    {
      res = 1;
    }
  return res;
}

/******************************************************************************
* Function : LcdDisplay_SetCursor()
*//**
* \b Description: function to set the position of the cursor <br/>
* \b PRE-CONDITION: LcdDisplay_Init is called properly <br/>
* @param Display The id of the display.
* @param Row The row of the cursor in the display. It starts from zero.
* @param Row The column of the cursor in the display. It starts from zero.
* @return uint8_t 1 if the cursor set properly, 0 otherwise
******************************************************************************/
extern uint8_t 
LcdDisplay_SetCursor(LcdDisplay_t Display, uint8_t Row, uint8_t Col)
{
  if(!(Display < LCD_DISPLAY_MAX &&
       Row < gConfig[Display].Height &&
       Col < gConfig[Display].Width))
    {
      return 0;
    }
  
  uint8_t NewAddress;

  switch(Row)
  {
    case 0:
    NewAddress = LCD_DISPLAY_DDRAM_LINE_0;
    break;

    case 1:
    NewAddress = LCD_DISPLAY_DDRAM_LINE_1;
    break;

    case 2:
    NewAddress = LCD_DISPLAY_DDRAM_LINE_0 + gConfig[Display].Width;
    break;

    case 3:
    NewAddress = LCD_DISPLAY_DDRAM_LINE_1 + gConfig[Display].Width;
    break;

    default:
    //DO NOTHING
    break;
  }

  NewAddress = NewAddress + Col;
  NewAddress = NewAddress | LCD_DISPLAY_DDRAM_MASK;
  LcdDisplay_SetCommand(Display, NewAddress); 
  
  return 1;
}                                 
/*****************************End of File ************************************/
