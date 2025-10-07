/*
 * Keyboards.h
 * 
 * Keyboard and Keypad input handling for touchscreen interface.
 * Provides on-screen keyboard and numeric keypad functionality.
 * 
 * Author: XentiQ
 * Date: 2025-10-07
 */

#ifndef _KEYBOARDS_H_
#define _KEYBOARDS_H_

#include "../gpu/Platform.h"
#include "../gpu/App_Common.h"

// Displays the on-screen keyboard interface.
// @param phost GPU context pointer.
// @param keypressed Currently pressed key code.
// @param displaytext Text to display in the input area.
// @param displaytitle Title text for the keyboard.
// @param numlock Number lock state (true = numeric mode).
// @param caplock Caps lock state (true = uppercase).
// @param errorcode Error state flag.
void DisplayKeyboard(Gpu_Hal_Context_t *phost, uint8_t keypressed, char *displaytext, char *displaytitle, bool numlock, bool caplock, bool errorcode);

// Gets the currently pressed key from touch input.
// @return The key code of the pressed key.
uint8_t GetKeyPressed(void);

// Waits for the current key to be released.
void WaitKeyRelease(void);

// Displays and handles the full keyboard interaction.
// @param phost GPU context pointer.
// @param curtext Current text buffer (input/output).
// @param curtitle Title to display.
// @param password Password mode flag (true = mask input with asterisks).
void Keyboard(Gpu_Hal_Context_t *phost, char *curtext, char *curtitle, bool password);

// Rounds a float value to one decimal place.
// @param x Pointer to the float value to round.
void Round1Dec(float *x);

// Displays the numeric keypad interface.
// @param phost GPU context pointer.
// @param keypressed Currently pressed key code.
// @param displaynum Number string to display.
// @param errorcode Error code (0 = no error).
void DisplayKeypad(Gpu_Hal_Context_t *phost, int32_t keypressed, char *displaynum, int8_t errorcode);

// Loads a float value into a string buffer.
// @param buf Buffer to store the string.
// @param curval Float value to convert.
void LoadBuffer(char *buf, float curval);

// Displays and handles the numeric keypad interaction.
// @param phost GPU context pointer.
// @param curval Current numeric value.
// @param minval Minimum allowed value.
// @param maxval Maximum allowed value.
// @param isfloat Float mode flag (true = allow decimal point).
// @return The entered numeric value.
float Keypad(Gpu_Hal_Context_t *phost, float curval, float minval, float maxval, bool isfloat);

#endif /* _KEYBOARDS_H_ */
