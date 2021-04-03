/**
 * @file lcd_display_cfg.c
 * @author Mohamed Hassanin
 * @brief Character LCD displays pattern for time-triggered embedded systems.
 * @version 0.1
 * @date 2021-03-31
 */


/******************************************************************************
* Includes
******************************************************************************/
#include "lcd_display_cfg.h"

/******************************************************************************
* Module Variable Definitions
******************************************************************************/
static const LcdDisplayConfig_t LcdDisplayConfig[LCD_DISPLAY_MAX] = {
  {
    .Display = LCD_DISPLAY_0,
    .Width = 20,
    .Height = 2,
    .En = PORTA_0,
    .Rs = PORTA_1,
    .Data =
    {
      PORTA_2,
      PORTA_3,
      PORTA_4,
      PORTA_5
    }
  }
};

/******************************************************************************
* Function Definitions
******************************************************************************/
/**
 * @brief return the configuration structure holding the 
 * configuration table of the module
 */
const LcdDisplayConfig_t* const
LcdDisplay_GetConfig(void)
{
  return LcdDisplayConfig;
}
/***************************** END OF FILE ***********************************/
