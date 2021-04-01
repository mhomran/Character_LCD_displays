/**
 * @file lcd_display.h
 * @author Mohamed Hassanin Mohamed
 * @brief Character LCD displays pattern for time-triggered embedded systems.
 * @version 0.1
 * @date 2021-03-29
 */
#ifndef LCD_DISPLAY_CFG
#define LCD_DISPLAY_CFG

/******************************************************************************
 * Definitions
 ******************************************************************************/
/**
 * @brief The interface number of bits. Either 4 or 8. In this module, 4-bit
 * interface is the only available option since it's so common.
 */ 
#define LCD_DISPLAY_BITLEN 4

//TODO: change as required
/**
 * @brief the buffer size of the buffer used for holding data or commands 
 */
#define LCD_DISPLAY_BUFF_SIZE 40 

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "../dio/dio.h"
/******************************************************************************
 * Typedefs
 ******************************************************************************/
/**
* Defines an enumerated list of all the lcd displays on the system
*/
typedef enum 
{
  LCD_DISPLAY_0,
  LCD_DISPLAY_MAX
} LcdDisplay_t;


/**
* A structure for the display configuration
*/
typedef struct
{
  LcdDisplay_t Display; /**< The Display Id*/

  DioChannel_t Rs; /**< the channel used to choose data or instruction */
  DioChannel_t En; /**< the channel used to start writing */
  DioChannel_t Data[LCD_DISPLAY_BITLEN]; /**< the data channels */
} LcdDisplayConfig_t;

/******************************************************************************
 * Function prototypes
 ******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

const LcdDisplayConfig_t* const LcdDisplay_GetConfig(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* end LCD_DISPLAY_CFG */
/*****************************End of File ************************************/
