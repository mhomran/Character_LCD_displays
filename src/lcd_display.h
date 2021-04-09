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

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* end LCD_DISPLAY */
/*****************************End of File ************************************/
