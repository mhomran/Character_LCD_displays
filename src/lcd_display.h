/**
 * @file lcd_display.h
 * @author Mohamed Hassanin Mohamed
 * @brief Character LCD displays pattern for time-triggered embedded systems.
 * @version 0.1
 * @date 2021-03-29
 */
#ifndef LCD_DISPLAY
#define LCD_DISPLAY
/******************************************************************************
 * Includes
 ******************************************************************************/
#include <inttypes.h>
#include "lcd_display_cfg.h"
/******************************************************************************
 * Definitions
 ******************************************************************************/
#define LCD_DISPLAY_CGRAM_CHAR_0 0x00 /**< Character 0 of CGRAM code */ 
#define LCD_DISPLAY_CGRAM_CHAR_1 0x01 /**< Character 1 of CGRAM code */ 
#define LCD_DISPLAY_CGRAM_CHAR_2 0x02 /**< Character 2 of CGRAM code */ 
#define LCD_DISPLAY_CGRAM_CHAR_3 0x03 /**< Character 3 of CGRAM code */ 
#define LCD_DISPLAY_CGRAM_CHAR_4 0x04 /**< Character 4 of CGRAM code */ 
#define LCD_DISPLAY_CGRAM_CHAR_5 0x05 /**< Character 5 of CGRAM code */ 
#define LCD_DISPLAY_CGRAM_CHAR_6 0x06 /**< Character 6 of CGRAM code */ 
#define LCD_DISPLAY_CGRAM_CHAR_7 0x07 /**< Character 7 of CGRAM code */ 
/******************************************************************************
 * Function prototypes
 ******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

extern void LcdDisplay_Init(const LcdDisplayConfig_t * const Config);
extern void LcdDisplay_Update(void);
extern void LcdDisplay_Clear(LcdDisplay_t Display);
extern uint8_t LcdDisplay_SetData(const LcdDisplay_t Display,
                                  const uint8_t* const Data,
                                  const uint8_t DataSize);
extern uint8_t LcdDisplay_SetCursor(LcdDisplay_t Display,
                                    uint8_t Row, 
                                    uint8_t Col);

extern void LcdDisplay_CreateChar(const LcdDisplay_t Display,
                                  const uint8_t CharIndex,
                                  const uint8_t* const Data);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* end LCD_DISPLAY */
/*****************************End of File ************************************/
