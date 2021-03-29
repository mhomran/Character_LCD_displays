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
/******************************************************************************
 * Function prototypes
 ******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

extern void LcdDisplay_Init(const LcdDisplayConfig_t * const Config);
extern void LcdDisplay_Update(void);
extern void LcdDisplay_Clear(LcdDisplay_t Display);
extern uint8_t LcdDisplay_SetData(LcdDisplay_t Display, uint8_t line, uint8_t start,
 uint8_t* Data, uint8_t DataSize);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* end LCD_DISPLAY */
/*****************************End of File ************************************/
