#include "Keyboards.h"
#include "../../logic/InteractionsHandler.h"

uint8_t font = 27;

inline int32_t minimum(int32_t a, int32_t b) {
  return a < b ? a : b;
}

// Adjusts scroll offset to keep the last line visible.
// @param text_buffer The text buffer to calculate scroll for.
// @param scroll_offset Reference to the scroll offset to adjust.
void adjustScrollToBottom(const char* text_buffer, uint8_t& scroll_offset) {
  uint8_t text_len = strlen(text_buffer);
  uint8_t total_lines = (text_len + KEYBOARD_MAX_PER_LINE - 1) / KEYBOARD_MAX_PER_LINE;
  if (total_lines == 0) total_lines = 1;
  
  if (total_lines > KEYBOARD_VISIBLE_LINES) {
    uint8_t max_scroll = total_lines - KEYBOARD_VISIBLE_LINES;
    if (scroll_offset < max_scroll) {
      scroll_offset = max_scroll;
    }
  }
}

struct {
  uint8_t Key_Detect : 1;
  uint8_t Caps : 1;
  uint8_t Numeric : 1;
  uint8_t Exit : 1;
} Flag;

void drawKeyboard(Gpu_Hal_Context_t* phost, uint8_t keypressed, char* displaytext, char* displaytitle, bool numlock,
                  bool caplock, const char* errormsg, uint8_t scroll_offset) {

  // Display List start
  Gpu_CoCmd_Dlstart(phost);
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(120, 120, 120));
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));  // enable tagbuffer updation

  Gpu_CoCmd_FgColor(phost, 0x703800);
  Gpu_CoCmd_BgColor(phost, 0x703800);

  Gpu_CoCmd_FgColor(phost, 0xAA0000);

  App_WrCoCmd_Buffer(phost, TAG(CLEAR_KEY));  // Back		 Return to Home
  Gpu_CoCmd_Button(phost, (DispWidth * 0.855), (DispHeight * 0.83), (DispWidth * 0.146), (DispHeight * 0.112), font,
                   (keypressed == CLEAR_KEY) ? OPT_FLAT : 0, "Clear");
  Gpu_CoCmd_FgColor(phost, 0x703800);

  Gpu_CoCmd_FgColor(phost, 0xADAF3C);
  App_WrCoCmd_Buffer(phost, TAG(BACK_SPACE));  // BackSpace
  Gpu_CoCmd_Button(phost, (DispWidth * 0.875), (DispHeight * 0.70), (DispWidth * 0.125), (DispHeight * 0.112), font,
                   (keypressed == BACK_SPACE) ? OPT_FLAT : 0, "<-");
  Gpu_CoCmd_FgColor(phost, 0x703800);

  Gpu_CoCmd_FgColor(phost, 0x00a2e8);
  App_WrCoCmd_Buffer(phost, TAG(KBBACK));  // Back		 Return to Home
  Gpu_CoCmd_Button(phost, (DispWidth * 0.115), (DispHeight * 0.83), (DispWidth * 0.192), (DispHeight * 0.112), font,
                   (keypressed == KBBACK) ? OPT_FLAT : 0, "Back");

  Gpu_CoCmd_FgColor(phost, 0x202020);   // 0x703800);
  App_WrCoCmd_Buffer(phost, TAG(' '));  // Space
  Gpu_CoCmd_Button(phost, (DispWidth * 0.315), (DispHeight * 0.83), (DispWidth * 0.33), (DispHeight * 0.112), font,
                   (keypressed == ' ') ? OPT_FLAT : 0, "Space");

  if (!numlock) {
    Gpu_CoCmd_Keys(phost, 0, (DispHeight * 0.442), DispWidth, (DispHeight * 0.112), font, keypressed,
                   ((caplock) ? "QWERTYUIOP" : "qwertyuiop"));
    Gpu_CoCmd_Keys(phost, (DispWidth * 0.042), (DispHeight * 0.57), (DispWidth * 0.96), (DispHeight * 0.112), font,
                   keypressed, ((caplock) ? "ASDFGHJKL" : "asdfghjkl"));
    Gpu_CoCmd_Keys(phost, (DispWidth * 0.125), (DispHeight * 0.70), (DispWidth * 0.73), (DispHeight * 0.112), font,
                   keypressed, ((caplock) ? "ZXCVBNM" : "zxcvbnm"));

    App_WrCoCmd_Buffer(phost, TAG(CAPS_LOCK));  // Capslock
    Gpu_CoCmd_Button(phost, 0, (DispHeight * 0.70), (DispWidth * 0.10), (DispHeight * 0.112), font,
                     (keypressed == CAPS_LOCK) ? OPT_FLAT : 0, ((caplock) ? "a^" : "A^"));
    App_WrCoCmd_Buffer(phost, TAG(NUMBER_LOCK));  // Numberlock
    Gpu_CoCmd_Button(phost, 0, (DispHeight * 0.83), (DispWidth * 0.10), (DispHeight * 0.112), font,
                     (keypressed == NUMBER_LOCK) ? OPT_FLAT : 0, "12*");
  } else {
    Gpu_CoCmd_Keys(phost, (DispWidth * 0), (DispHeight * 0.442), DispWidth, (DispHeight * 0.112), font, keypressed,
                   "1234567890");
    Gpu_CoCmd_Keys(phost, (DispWidth * 0.042), (DispHeight * 0.57), (DispWidth * 0.96), (DispHeight * 0.112), font,
                   keypressed, "-@#%^&*()");
    Gpu_CoCmd_Keys(phost, (DispWidth * 0.125), (DispHeight * 0.70), (DispWidth * 0.73), (DispHeight * 0.112), font,
                   keypressed, ",_+[]{}");
    App_WrCoCmd_Buffer(phost, TAG(NUMBER_LOCK));  // Numberlock
    Gpu_CoCmd_Button(phost, 0, (DispHeight * 0.83), (DispWidth * 0.10), (DispHeight * 0.112), font,
                     (keypressed == NUMBER_LOCK) ? OPT_FLAT : 0, "AB*");
  }
  Gpu_CoCmd_FgColor(phost, 0x006400);
  App_WrCoCmd_Buffer(phost, TAG(SAVE_KEY));  // Enter
  Gpu_CoCmd_Button(phost, (DispWidth * 0.653), (DispHeight * 0.83), (DispWidth * 0.192), (DispHeight * 0.112), font,
                   (keypressed == SAVE_KEY) ? OPT_FLAT : 0, "Enter");
  Gpu_CoCmd_FgColor(phost, 0x703800);
  
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));  // Disable the tag buffer updates
  App_WrCoCmd_Buffer(phost, SCISSOR_XY(0, 0));
  App_WrCoCmd_Buffer(phost, SCISSOR_SIZE(DispWidth, 100));  // Increased height for wrapped text
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));  // Text Color
  
  // Manual text wrapping - split long text into multiple lines with scrolling support
  uint8_t text_len = strlen(displaytext);
  const uint8_t display_font = KEYBOARD_FONT;
  const uint8_t line_height = KEYBOARD_LINE_HEIGHT;
  const uint8_t max_per_line = KEYBOARD_MAX_PER_LINE;
  const uint8_t max_lines = KEYBOARD_MAX_LINES;
  const uint8_t visible_lines = KEYBOARD_VISIBLE_LINES;
  
  // Split text into all possible lines
  static char lines[KEYBOARD_MAX_LINES][KEYBOARD_MAX_PER_LINE + 1];
  
  // Clear all lines
  for (uint8_t i = 0; i < max_lines; i++) {
    memset(lines[i], 0, KEYBOARD_MAX_PER_LINE + 1);
  }
  
  // Calculate total number of lines needed
  uint8_t total_lines = (text_len + max_per_line - 1) / max_per_line;
  if (total_lines > max_lines) total_lines = max_lines;
  
  // Copy text into lines
  for (uint8_t i = 0; i < total_lines; i++) {
    uint8_t start_pos = i * max_per_line;
    uint8_t remaining = text_len - start_pos;
    uint8_t line_len = (remaining < max_per_line) ? remaining : max_per_line;
    memcpy(lines[i], displaytext + start_pos, line_len);
    lines[i][line_len] = '\0';
  }
  
  // Render visible lines based on scroll offset
  for (uint8_t i = 0; i < visible_lines; i++) {
    uint8_t line_idx = scroll_offset + i;
    if (line_idx < total_lines && lines[line_idx][0] != '\0') {
      Gpu_CoCmd_Text(phost, 0, i * line_height, display_font, 0, lines[line_idx]);
    }
  }
  
  // Draw scroll UI if scrolling is needed
  if (total_lines > visible_lines) {
    App_WrCoCmd_Buffer(phost, TAG_MASK(1));  // Re-enable tag buffer for scroll buttons
    
    bool can_scroll_up = (scroll_offset > 0);
    bool can_scroll_down = (scroll_offset + visible_lines < total_lines);
    
    // Draw scroll up button (disabled if at top)
    App_WrCoCmd_Buffer(phost, TAG(can_scroll_up ? KEYBOARD_SCROLL_UP : 0));
    Gpu_CoCmd_FgColor(phost, can_scroll_up ? 0x505050 : 0x080808);
    Gpu_CoCmd_Button(phost, DispWidth - 35, 2, 33, 20, 20, 
                     (keypressed == KEYBOARD_SCROLL_UP) ? OPT_FLAT : 0, "^");
    Gpu_CoCmd_FgColor(phost, 0x703800);
    
    // Draw scrollbar between buttons
    int16_t scrollbar_top = 24;
    int16_t scrollbar_height = 27;
    int16_t scrollbar_x = (int16_t)DispWidth - 35;
    int16_t scrollbar_width = 33;
    
    // Background track (full button width)
    App_WrCoCmd_Buffer(phost, COLOR_RGB(60, 60, 60));
    App_WrCoCmd_Buffer(phost, BEGIN(RECTS));
    App_WrCoCmd_Buffer(phost, VERTEX2F(scrollbar_x * 16, scrollbar_top * 16));
    App_WrCoCmd_Buffer(phost, VERTEX2F((scrollbar_x + scrollbar_width) * 16, (scrollbar_top + scrollbar_height) * 16));
    App_WrCoCmd_Buffer(phost, END());
    
    // Calculate thumb position and size
    float thumb_ratio = (float)visible_lines / total_lines;
    int16_t thumb_height = (int16_t)(scrollbar_height * thumb_ratio);
    if (thumb_height < 8) thumb_height = 8;  // Minimum thumb size
    
    float scroll_ratio = (float)scroll_offset / (total_lines - visible_lines);
    int16_t thumb_offset = (int16_t)((scrollbar_height - thumb_height) * scroll_ratio);
    int16_t thumb_y = scrollbar_top + thumb_offset;
    
    // Draw thumb as rectangle (no rounded ends)
    App_WrCoCmd_Buffer(phost, COLOR_RGB(180, 180, 180));
    App_WrCoCmd_Buffer(phost, BEGIN(RECTS));
    App_WrCoCmd_Buffer(phost, VERTEX2F((scrollbar_x + 2) * 16, thumb_y * 16));
    App_WrCoCmd_Buffer(phost, VERTEX2F((scrollbar_x + scrollbar_width - 2) * 16, (thumb_y + thumb_height) * 16));
    App_WrCoCmd_Buffer(phost, END());
    
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));  // Reset color
    
    // Draw scroll down button (disabled if at bottom)
    App_WrCoCmd_Buffer(phost, TAG(can_scroll_down ? KEYBOARD_SCROLL_DOWN : 0));
    Gpu_CoCmd_FgColor(phost, can_scroll_down ? 0x505050 : 0x080808);
    Gpu_CoCmd_Button(phost, DispWidth - 35, 55, 33, 20, 20, 
                     (keypressed == KEYBOARD_SCROLL_DOWN) ? OPT_FLAT : 0, "v");
    Gpu_CoCmd_FgColor(phost, 0x703800);
    
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));  // Disable tags again
  }

  App_WrCoCmd_Buffer(phost, SCISSOR_XY(0, 77));
  App_WrCoCmd_Buffer(phost, SCISSOR_SIZE(DispWidth, (uint16_t)(DispHeight * 0.1)));
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(120, 120, 120));
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
  
  // Display title or error message
  bool has_error = (errormsg != NULL && errormsg[0] != '\0');
  if (has_error) {
    // Display error message in darker red
    App_WrCoCmd_Buffer(phost, COLOR_RGB(245, 20, 20));
    Gpu_CoCmd_Text(phost, DispWidth / 2, 82, 27, OPT_CENTERX, errormsg);
  } else {
    // Display normal title in yellow
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 0));
    Gpu_CoCmd_Text(phost, DispWidth / 2, 82, 27, OPT_CENTERX, displaytitle);
  }

  Disp_End(phost);
}

KeyboardResult getKeyboardValue(Gpu_Hal_Context_t* phost, char* curtext, char* curtitle, bool password, uint8_t maxlen, const char* errormsg) {
  delay(KEYBOARD_TRANSITION_DELAY_MS);  // Added to create smooth transition between screen
  uint8_t font = 27;
  
  // Use static buffer to avoid VLA and stack issues
  static char keypad_value_buffer[KEYBOARD_MAX_LEN];
  memset(keypad_value_buffer, 0, KEYBOARD_MAX_LEN);
  
  uint8_t curpos = 0;
  uint8_t scroll_offset = 0;  // Track scroll position

  bool numlock = false;
  bool caplock = false;

  strncpy(keypad_value_buffer, curtext, maxlen);

  curpos = strlen(keypad_value_buffer);  // starting pos
  keypad_value_buffer[curpos] = 0;
  Flag.Numeric = OFF;  // Disable the numbers and spcial charaters

  adjustScrollToBottom(keypad_value_buffer, scroll_offset);
  drawKeyboard(phost, 0, keypad_value_buffer, curtitle, numlock, caplock, errormsg, scroll_offset);
  InteractionsHandler::waitForTouchRelease();

  if (errormsg != NULL && errormsg[0] != '\0') {
    delay(KEYBOARD_ERROR_DISPLAY_MS);
  }

  while (true) {
    int keypressed = InteractionsHandler::getTouchStateChanged();

    // Only update the keyboard if a key was pressed
    if (keypressed == -1) continue;

    drawKeyboard(phost, keypressed, keypad_value_buffer, curtitle, numlock, caplock, NULL, scroll_offset);

    switch (keypressed) {
      // No key
      case 0:
        drawKeyboard(phost, 0, keypad_value_buffer, curtitle, numlock, caplock, NULL, scroll_offset);
        break;

      case BACK_SPACE:
        // check in the line there is any characters are present, cursor not included
        if (curpos > 0) {
          curpos--;  // clear the character in the buffer
          keypad_value_buffer[curpos] = 0;
          if (password) curtext[curpos] = 0;
          
          // Auto-scroll to bottom to show the last line
          adjustScrollToBottom(keypad_value_buffer, scroll_offset);
        }
        break;

      case CAPS_LOCK:
        caplock = !caplock;  // toggle the caps lock on when the key detect
        break;

      case NUMBER_LOCK:
        numlock = (numlock) ? FALSE : TRUE;  // toggle the number lock on when the key detect
        break;
        
      case CLEAR_KEY:
        curpos = 0;
        keypad_value_buffer[curpos] = 0;
        if (password) curtext[curpos] = 0;
        scroll_offset = 0;  // Reset scroll when clearing
        break;

      case KEYBOARD_SCROLL_UP:
        if (scroll_offset > 0) {
          scroll_offset--;
        }
        break;
        
      case KEYBOARD_SCROLL_DOWN: {
        uint8_t text_len = strlen(keypad_value_buffer);
        uint8_t total_lines = (text_len + KEYBOARD_MAX_PER_LINE - 1) / KEYBOARD_MAX_PER_LINE;
        if (total_lines > KEYBOARD_MAX_LINES) total_lines = KEYBOARD_MAX_LINES;
        if (scroll_offset + KEYBOARD_VISIBLE_LINES < total_lines) {
          scroll_offset++;
        }
        break;
      }

      case KBBACK:
        return KeyboardResult(ACTION_BACK);

      case SAVE_KEY:
        strncpy(curtext, keypad_value_buffer, maxlen);
        return KeyboardResult(ACTION_ENTER);

      default:
        if (curpos < maxlen - 1 && curpos < KEYBOARD_MAX_LEN - 1) {
          keypad_value_buffer[curpos] = keypressed;
          if (password) {
            curtext[curpos] = '*';
            curtext[curpos + 1] = 0;
          }
          keypad_value_buffer[++curpos] = 0;
          
          // Auto-scroll to bottom to show the last line
          adjustScrollToBottom(keypad_value_buffer, scroll_offset);
          
          drawKeyboard(phost, keypressed, keypad_value_buffer, curtitle, numlock, caplock, NULL, scroll_offset);
        } else {
          drawKeyboard(phost, keypressed, keypad_value_buffer, curtitle, numlock, caplock, "Max length reached", scroll_offset);
          delay(KEYBOARD_ERROR_DISPLAY_MS);
          drawKeyboard(phost, keypressed, keypad_value_buffer, curtitle, numlock, caplock, NULL, scroll_offset);
        }
        break;
    }
  }
}

void drawKeypad(Gpu_Hal_Context_t* phost, int32_t keypressed, char* displaynum, int8_t errorcode) {
  Gpu_CoCmd_Dlstart(phost);
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(120, 120, 120));
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));  // enable tagbuffer updation
  // Gpu_CoCmd_FgColor(phost, 0xB40000);     // 0x703800 0xD00000
  // Gpu_CoCmd_BgColor(phost, 0xB40000);
  Gpu_CoCmd_FgColor(phost, 0x202020);
  Gpu_CoCmd_BgColor(phost, 0x202020);

  Gpu_CoCmd_FgColor(phost, 0x00a2e8);
  App_WrCoCmd_Buffer(phost, TAG(BACK));  // Back		 Return to Home
  Gpu_CoCmd_Button(phost, 207, 37, 67, 46, font, (keypressed == BACK) ? OPT_FLAT : 0, "Back");

  Gpu_CoCmd_FgColor(phost, 0xADAF3C);
  App_WrCoCmd_Buffer(phost, TAG(BACK_SPACE));  // BackSpace
  Gpu_CoCmd_Button(phost, 207, 87, 67, 50, 31, (keypressed == BACK_SPACE) ? OPT_FLAT : 0, "<-");

  Gpu_CoCmd_FgColor(phost, 0x006400);
  App_WrCoCmd_Buffer(phost, TAG(NUM_ENTER));  // Enter
  Gpu_CoCmd_Button(phost, 207, 141, 67, 90, font, (keypressed == NUM_ENTER) ? OPT_FLAT : 0, "Enter");

  Gpu_CoCmd_FgColor(phost, 0x202020);
  Gpu_CoCmd_BgColor(phost, 0x202020);

  Gpu_CoCmd_Keys(phost, 47, 35, 150, 50, 29, keypressed, "789");
  Gpu_CoCmd_Keys(phost, 47, 88, 150, 50, 29, keypressed, "456");
  Gpu_CoCmd_Keys(phost, 47, 140, 150, 50, 29, keypressed, "123");
  Gpu_CoCmd_Keys(phost, 47, 191, 150, 40, 29, keypressed, "0.");

  App_WrCoCmd_Buffer(phost, TAG_MASK(0));  // Disable the tag buffer updates
  App_WrCoCmd_Buffer(phost, SCISSOR_XY(0, 0));
  App_WrCoCmd_Buffer(phost, SCISSOR_SIZE(320, 34));

  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));  // Text Color

  Gpu_CoCmd_Text(phost, 0, 0, 30, 0, displaynum);

  App_WrCoCmd_Buffer(phost, DISPLAY());
  Gpu_CoCmd_Swap(phost);
  App_Flush_Co_Buffer(phost);
}

float getKeypadValue(Gpu_Hal_Context_t* phost, float curval, float minval, float maxval, bool isfloat) {
  phost = &host;
  // Use static buffer to avoid stack issues
  static char keypad_render_buffer[KEYPAD_MAX_LEN];
  memset(keypad_render_buffer, 0, KEYPAD_MAX_LEN);
  int8_t curpos;
  int8_t lastKeyPressed = 0;

  // Load keypad_render_buffer with curval
  dtostrf(curval, 2, isfloat ? 1 : 0, keypad_render_buffer);
  curpos = strlen(keypad_render_buffer);

  InteractionsHandler::waitForTouchRelease();

  drawKeypad(phost, 0, keypad_render_buffer, 0);

  while (true) {
    int keypressed = InteractionsHandler::getTouchStateChanged();

    // Only update when key actually changed
    if (keypressed == -1) {
      continue;
    }

    switch (keypressed) {
      // No key
      case 0:
        drawKeypad(phost, 0, keypad_render_buffer, 0);
        break;

      case BACK_SPACE:
        if (curpos >= 0) {
          if (curpos > 0) curpos--;
          keypad_render_buffer[curpos] = 0;
        }
        break;

      case BACK:
        return curval;

      case NUM_ENTER: {
        float tempval = atof(keypad_render_buffer);

        // Check if the value is within the range
        if (tempval > maxval || tempval < minval) {
          // If no, load the max or min value and show it on the keypad
          dtostrf(tempval > maxval ? maxval : minval, 2, isfloat ? 1 : 0, keypad_render_buffer);
          curpos = strlen(keypad_render_buffer);
          break;
        }

        return isfloat ? tempval : (float)(int)tempval;
      }

      // Any other key
      default:
        if (curpos < KEYPAD_MAX_LEN - 1) {
          keypad_render_buffer[curpos] = keypressed;
          keypad_render_buffer[++curpos] = 0;
          drawKeypad(phost, keypressed, keypad_render_buffer, 0);
        } else {  // max entry
          drawKeypad(phost, keypressed, keypad_render_buffer, 1);
        }
        break;
    }
  }

  return curval;
}