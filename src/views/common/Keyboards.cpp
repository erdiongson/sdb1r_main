#include "Keyboards.h"
#include "../../logic/InteractionsHandler.h"

uint8_t font = 27;

inline int32_t minimum(int32_t a, int32_t b) {
  return a < b ? a : b;
}

struct {
  uint8_t Key_Detect : 1;
  uint8_t Caps : 1;
  uint8_t Numeric : 1;
  uint8_t Exit : 1;
} Flag;

void drawKeyboard(Gpu_Hal_Context_t* phost, uint8_t keypressed, char* displaytext, char* displaytitle, bool numlock,
                  bool caplock, const char* errormsg) {
  char buf[KEYBOARD_MAX_LEN];

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

  // Manual text wrapping - split long text into up to 3 lines
  const uint8_t line_height = 22;  // Vertical spacing between lines
  uint8_t text_len = strlen(displaytext);
  
  // Split text into up to 3 lines (break at any character)
  static char line1[KEYBOARD_MAX_PER_LINE + 1];
  static char line2[KEYBOARD_MAX_PER_LINE + 1];
  static char line3[KEYBOARD_MAX_PER_LINE + 1];
  
  memset(line1, 0, KEYBOARD_MAX_PER_LINE + 1);
  memset(line2, 0, KEYBOARD_MAX_PER_LINE + 1);
  memset(line3, 0, KEYBOARD_MAX_PER_LINE + 1);
  
  // Copy first line
  uint8_t line1_len = (text_len < KEYBOARD_MAX_PER_LINE) ? text_len : KEYBOARD_MAX_PER_LINE;
  memcpy(line1, displaytext, line1_len);
  line1[line1_len] = '\0';
  
  // Copy second line if text is longer than first line
  if (text_len > KEYBOARD_MAX_PER_LINE) {
    uint8_t remaining = text_len - KEYBOARD_MAX_PER_LINE;
    uint8_t line2_len = (remaining < KEYBOARD_MAX_PER_LINE) ? remaining : KEYBOARD_MAX_PER_LINE;
    memcpy(line2, displaytext + KEYBOARD_MAX_PER_LINE, line2_len);
    line2[line2_len] = '\0';
  }
  
  // Copy third line if text is longer than two lines
  if (text_len > KEYBOARD_MAX_PER_LINE * 2) {
    uint8_t remaining = text_len - (KEYBOARD_MAX_PER_LINE * 2);
    uint8_t line3_len = (remaining < KEYBOARD_MAX_PER_LINE) ? remaining : KEYBOARD_MAX_PER_LINE;
    memcpy(line3, displaytext + (KEYBOARD_MAX_PER_LINE * 2), line3_len);
    line3[line3_len] = '\0';
  }
  
  // Render all lines
  Gpu_CoCmd_Text(phost, 0, 0, font, 0, line1);
  if (line2[0] != '\0') Gpu_CoCmd_Text(phost, 0, line_height, font, 0, line2);
  if (line3[0] != '\0') Gpu_CoCmd_Text(phost, 0, line_height * 2, font, 0, line3);

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
  static char buf[KEYBOARD_MAX_LEN];
  memset(buf, 0, KEYBOARD_MAX_LEN);
  
  uint8_t curpos = 0;

  bool numlock = false;
  bool caplock = false;

  strcpy(buf, curtext);

  curpos = strlen(buf);  // starting pos
  buf[curpos] = 0;
  Flag.Numeric = OFF;  // Disable the numbers and spcial charaters

  drawKeyboard(phost, 0, buf, curtitle, numlock, caplock, errormsg);
  InteractionsHandler::waitForTouchRelease();

  if (errormsg != NULL && errormsg[0] != '\0') {
    delay(KEYBOARD_ERROR_DISPLAY_MS);
  }

  while (true) {
    int keypressed = InteractionsHandler::getTouchStateChanged();

    // Only update the keyboard if a key was pressed
    if (keypressed == -1) continue;

    drawKeyboard(phost, keypressed, buf, curtitle, numlock, caplock, NULL);

    switch (keypressed) {
      // No key
      case 0:
        drawKeyboard(phost, 0, buf, curtitle, numlock, caplock, NULL);
        break;

      case BACK_SPACE:
        // check in the line there is any characters are present, cursor not included
        if (curpos > 0) {
          curpos--;  // clear the character in the buffer
          buf[curpos] = 0;
          if (password) curtext[curpos] = 0;
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
        buf[curpos] = 0;
        if (password) curtext[curpos] = 0;
        break;

      case KBBACK:
        return KeyboardResult(ACTION_BACK);

      case SAVE_KEY:
        strcpy(curtext, buf);
        return KeyboardResult(ACTION_ENTER);

      default:
        if (curpos < maxlen - 1 && curpos < KEYBOARD_MAX_LEN - 1) {
          buf[curpos] = keypressed;
          if (password) {
            curtext[curpos] = '*';
            curtext[curpos + 1] = 0;
          }
          buf[++curpos] = 0;
          drawKeyboard(phost, keypressed, buf, curtitle, numlock, caplock, NULL);
        } else {
          drawKeyboard(phost, keypressed, buf, curtitle, numlock, caplock, "Max length reached");
          delay(KEYBOARD_ERROR_DISPLAY_MS);
          drawKeyboard(phost, keypressed, buf, curtitle, numlock, caplock, NULL);
        }
        break;
    }
  }
}

void roundOneDecimal(float* x) {
  char buf[KEYPAD_MAX_LEN];

  dtostrf(*x, 3, 1, buf);
  *x = atof(buf);
}

void drawKeypad(Gpu_Hal_Context_t* phost, int32_t keypressed, char* displaynum, int8_t errorcode) {
  char buf[KEYPAD_MAX_LEN];

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

  sprintf(buf, "%s", displaynum);
  Gpu_CoCmd_Text(phost, 0, 0, 30, 0, buf);

  App_WrCoCmd_Buffer(phost, DISPLAY());
  Gpu_CoCmd_Swap(phost);
  App_Flush_Co_Buffer(phost);
}

float getKeypadValue(Gpu_Hal_Context_t* phost, float curval, float minval, float maxval, bool isfloat) {
  phost = &host;
  char buf[KEYPAD_MAX_LEN] = "";
  int8_t curpos;
  int8_t lastKeyPressed = 0;

  // Load buf with curval
  dtostrf(curval, 2, isfloat ? 1 : 0, buf);
  curpos = strlen(buf);

  InteractionsHandler::waitForTouchRelease();

  drawKeypad(phost, 0, buf, 0);

  while (true) {
    int keypressed = InteractionsHandler::getTouchStateChanged();

    // Only update when key actually changed
    if (keypressed == -1) {
      continue;
    }

    switch (keypressed) {
      // No key
      case 0:
        drawKeypad(phost, 0, buf, 0);
        break;

      case BACK_SPACE:
        if (curpos >= 0) {
          if (curpos > 0) curpos--;
          buf[curpos] = 0;
        }
        break;

      case BACK:
        return curval;

      case NUM_ENTER: {
        float tempval = atof(buf);

        // Check if the value is within the range
        if (tempval > maxval || tempval < minval) {
          // If no, load the max or min value and show it on the keypad
          dtostrf(tempval > maxval ? maxval : minval, 2, isfloat ? 1 : 0, buf);
          curpos = strlen(buf);
          break;
        }

        return isfloat ? tempval : (float)(int)tempval;
      }

      // Any other key
      default:
        if (curpos < KEYPAD_MAX_LEN - 1) {
          buf[curpos] = keypressed;
          buf[++curpos] = 0;
          drawKeypad(phost, keypressed, buf, 0);
        } else {  // max entry
          drawKeypad(phost, keypressed, buf, 1);
        }
        break;
    }
  }

  return curval;
}