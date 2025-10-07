#include "Keyboards.h"
#include "../logic/InteractionsHandler.h"
#include "../Utils.h"

uint8_t font = 27;
uint8_t Line = 0;
uint16_t line2disp = 0;

const int STARTPOS = 270;  // GUI screen coordinates

inline int32_t minimum(int32_t a, int32_t b) {
  return a < b ? a : b;
}

struct
{
  uint8_t Key_Detect : 1;
  uint8_t Caps : 1;
  uint8_t Numeric : 1;
  uint8_t Exit : 1;
} Flag;

void DisplayKeyboard(Gpu_Hal_Context_t *phost, uint8_t keypressed, char *displaytext, char *displaytitle, bool numlock, bool caplock, bool errorcode) {
  char buf[PROFILE_NAME_MAX_LEN + 8];

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
  Gpu_CoCmd_Button(phost, (DispWidth * 0.855), (DispHeight * 0.83), (DispWidth * 0.146), (DispHeight * 0.112),
                   font, (keypressed == CLEAR_KEY) ? OPT_FLAT : 0, "Clear");
  Gpu_CoCmd_FgColor(phost, 0x703800);


  Gpu_CoCmd_FgColor(phost, 0xADAF3C);
  App_WrCoCmd_Buffer(phost, TAG(BACK_SPACE));  // BackSpace
  Gpu_CoCmd_Button(phost, (DispWidth * 0.875), (DispHeight * 0.70), (DispWidth * 0.125),
                   (DispHeight * 0.112), font, (keypressed == BACK_SPACE) ? OPT_FLAT : 0, "<-");
  Gpu_CoCmd_FgColor(phost, 0x703800);



  Gpu_CoCmd_FgColor(phost, 0x00a2e8);
  App_WrCoCmd_Buffer(phost, TAG(KBBACK));  // Back		 Return to Home
  Gpu_CoCmd_Button(phost, (DispWidth * 0.115), (DispHeight * 0.83), (DispWidth * 0.192), (DispHeight * 0.112),
                   font, (keypressed == KBBACK) ? OPT_FLAT : 0, "Back");

  Gpu_CoCmd_FgColor(phost, 0x202020);   //0x703800);
  App_WrCoCmd_Buffer(phost, TAG(' '));  // Space
  Gpu_CoCmd_Button(phost, (DispWidth * 0.315), (DispHeight * 0.83), (DispWidth * 0.33), (DispHeight * 0.112),
                   font, (keypressed == ' ') ? OPT_FLAT : 0, "Space");

  if (!numlock) {
    Gpu_CoCmd_Keys(phost, 0, (DispHeight * 0.442), DispWidth, (DispHeight * 0.112), font, keypressed,
                   ((caplock) ? "QWERTYUIOP" : "qwertyuiop"));
    Gpu_CoCmd_Keys(phost, (DispWidth * 0.042), (DispHeight * 0.57), (DispWidth * 0.96), (DispHeight * 0.112),
                   font, keypressed, ((caplock) ? "ASDFGHJKL" : "asdfghjkl"));
    Gpu_CoCmd_Keys(phost, (DispWidth * 0.125), (DispHeight * 0.70), (DispWidth * 0.73), (DispHeight * 0.112),
                   font, keypressed, ((caplock) ? "ZXCVBNM" : "zxcvbnm"));


    App_WrCoCmd_Buffer(phost, TAG(CAPS_LOCK));  // Capslock
    Gpu_CoCmd_Button(phost, 0, (DispHeight * 0.70), (DispWidth * 0.10), (DispHeight * 0.112), font,
                     (keypressed == CAPS_LOCK) ? OPT_FLAT : 0,
                     ((caplock) ? "a^" : "A^"));
    App_WrCoCmd_Buffer(phost, TAG(NUMBER_LOCK));  // Numberlock
    Gpu_CoCmd_Button(phost, 0, (DispHeight * 0.83), (DispWidth * 0.10), (DispHeight * 0.112), font,
                     (keypressed == NUMBER_LOCK) ? OPT_FLAT : 0,
                     "12*");
  } else {
    Gpu_CoCmd_Keys(phost, (DispWidth * 0), (DispHeight * 0.442), DispWidth, (DispHeight * 0.112), font,
                   keypressed, "1234567890");
    Gpu_CoCmd_Keys(phost, (DispWidth * 0.042), (DispHeight * 0.57), (DispWidth * 0.96), (DispHeight * 0.112),
                   font, keypressed, "-@#%^&*()");
    Gpu_CoCmd_Keys(phost, (DispWidth * 0.125), (DispHeight * 0.70), (DispWidth * 0.73), (DispHeight * 0.112),
                   font, keypressed, ",_+[]{}");
    App_WrCoCmd_Buffer(phost, TAG(NUMBER_LOCK));  // Numberlock
    Gpu_CoCmd_Button(phost, 0, (DispHeight * 0.83), (DispWidth * 0.10), (DispHeight * 0.112), font,
                     (keypressed == NUMBER_LOCK) ? OPT_FLAT : 0,
                     "AB*");
  }
  Gpu_CoCmd_FgColor(phost, 0x006400);
  App_WrCoCmd_Buffer(phost, TAG(SAVE_KEY));  // Enter
  Gpu_CoCmd_Button(phost, (DispWidth * 0.653), (DispHeight * 0.83), (DispWidth * 0.192), (DispHeight * 0.112), font,
                   (keypressed == SAVE_KEY) ? OPT_FLAT : 0, "Enter");
  Gpu_CoCmd_FgColor(phost, 0x703800);
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));  // Disable the tag buffer updates
  App_WrCoCmd_Buffer(phost, SCISSOR_XY(0, 0));
  App_WrCoCmd_Buffer(phost, SCISSOR_SIZE(DispWidth, (uint16_t)(DispHeight * 0.41)));
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));  // Text Color

  if (errorcode) {
    sprintf(buf, "%s\n%s", displaytext, "error");
    Gpu_CoCmd_Text(phost, 0, 0, font, 0, buf);
  } else
    Gpu_CoCmd_Text(phost, 0, 0, font, 0, displaytext);

  App_WrCoCmd_Buffer(phost, SCISSOR_XY(0, 77));
  App_WrCoCmd_Buffer(phost, SCISSOR_SIZE(DispWidth, (uint16_t)(DispHeight * 0.1)));
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(120, 120, 120));
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 0));
  Gpu_CoCmd_Text(phost, 160 - (strlen(displaytitle) * 5), 78, 27, 0, displaytitle);

  Disp_End(phost);
}
uint8_t GetKeyPressed(void) {
  Interaction interaction;
  do {
    InteractionsHandler::getTouchInteraction(interaction);
  } while (interaction.key_pressed == 0);
  return interaction.key_pressed;
}

void WaitKeyRelease(void) {
  while (Gpu_Hal_Rd8(phost, REG_TOUCH_TAG) > 0) {}
}

void Keyboard(Gpu_Hal_Context_t *phost, char *curtext, char *curtitle, bool password) {
  uint8_t keypressed = 0;

  delay(200);  // Added to create smooth transition between screen
  //phost = &host;
  /*local variables*/
  uint16_t noofchars = 0, line2disp = 0, nextline = 0;
  uint8_t font = 27;
  //char curtext[PROFILE_NAME_MAX_LEN]="hello";
  char buf[PROFILE_NAME_MAX_LEN + 20];
  uint8_t i;
  uint8_t curpos = 0;
  bool wait4key = TRUE;
  bool numlock = FALSE;
  bool caplock = FALSE;


  for (i = 0; i < PROFILE_NAME_MAX_LEN; i++) buf[i] = '\0';
  strcpy(buf, curtext);


  /*initial setup*/
  curpos = strlen(buf);  // starting pos
  buf[curpos] = 0;
  Flag.Numeric = OFF;  // Disable the numbers and spcial charaters

  DisplayKeyboard(phost, keypressed, buf, curtitle, numlock, caplock, 0);
  //for(i=0;i<14;i++) Dprint(buf[i]);
  Dprint("end");

  do {
    keypressed = GetKeyPressed();

    if (keypressed > 0) {
      //DisplayKeyboard(phost, keypressed,buf,numlock,caplock,0);
      WaitKeyRelease();

      switch (keypressed) {
        case BACK_SPACE:
          if (curpos > 0)  // check in the line there is any characters are present, cursor not included
          {
            Dprint("BKSP");
            curpos--;  // clear the character in the buffer
            buf[curpos] = 0;
            if (password) curtext[curpos] = 0;
            Dprint(curpos);
          }
          keypressed = 0;
          break;

        case CAPS_LOCK:
          Dprint("cap");
          keypressed = 0;
          caplock = !caplock;  // toggle the caps lock on when the key detect
          break;

        case NUMBER_LOCK:
          Dprint("num");
          keypressed = 0;
          numlock = (numlock) ? FALSE : TRUE;  // toggle the number lock on when the key detect
          break;
        case CLEAR_KEY:
          keypressed = 0;
          curpos = 0;
          buf[curpos] = 0;
          if (password) curtext[curpos] = 0;
          break;
        case KBBACK:
          keypressed = 0;
          wait4key = FALSE;
          break;

        case SAVE_KEY:
          keypressed = 0;
          wait4key = FALSE;
          strcpy(curtext, buf);
          break;
        default:
          if (curpos < PROFILE_NAME_MAX_LEN) {
            //Dprint(curpos);
            buf[curpos] = keypressed;
            if (password) {
              curtext[curpos] = '*';
              curtext[curpos + 1] = 0;
            }
            buf[++curpos] = 0;
            keypressed = 0;
          } else {
            DisplayKeyboard(phost, keypressed, buf, curtitle, numlock, caplock, TRUE);
            keypressed = 0;
            delay(500);
          }
          break;
      }
      if (wait4key) {
        if (password)
          DisplayKeyboard(phost, keypressed, curtext, curtitle, numlock, caplock, 0);
        else
          DisplayKeyboard(phost, keypressed, buf, curtitle, numlock, caplock, 0);
      }
    }
  } while (wait4key);
}

void Round1Dec(float *x) {
  char buf[PROFILE_NAME_MAX_LEN];

  dtostrf(*x, 3, 1, buf);
  *x = atof(buf);
}

void DisplayKeypad(Gpu_Hal_Context_t *phost, int32_t keypressed, char *displaynum, int8_t errorcode) {
  char buf[KEYPAD_MAX_LEN];

  Gpu_CoCmd_Dlstart(phost);
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(120, 120, 120));
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));  // enable tagbuffer updation
  //Gpu_CoCmd_FgColor(phost, 0xB40000);     // 0x703800 0xD00000
  //Gpu_CoCmd_BgColor(phost, 0xB40000);
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

  //App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  //v204 if(errorcode)
  //v204  sprintf(buf,"%s  %s",displaynum,"out of range");
  //v204 else
  sprintf(buf, "%s", displaynum);
  Gpu_CoCmd_Text(phost, 0, 0, 30, 0, buf);

  App_WrCoCmd_Buffer(phost, DISPLAY());
  Gpu_CoCmd_Swap(phost);
  App_Flush_Co_Buffer(phost);
}


void LoadBuffer(char *buf, float curval) {
  float temp;
  temp = (int8_t)curval;

  if (curval - temp > 0) {
    dtostrf(curval, 3, 1, buf);
    buf[KEYPAD_MAX_LEN - 1] = '\0';  // Force null termination
  } else {
    sprintf(buf, "%d", (int16_t)curval);
    buf[KEYPAD_MAX_LEN - 1] = '\0';  // Force null termination
  }
}

float Keypad(Gpu_Hal_Context_t *phost, float curval, float minval, float maxval, bool isfloat) {
  phost = &host;
  uint8_t keypressed = 0;
  bool wait4key = TRUE;

  char buf[KEYPAD_MAX_LEN];
  int8_t curpos;
  float tempval;

  for (int i = 0; i < KEYPAD_MAX_LEN; i++) buf[i] = 0;
  LoadBuffer(buf, curval);
  curpos = strlen(buf);
  buf[curpos] = 0;

  Dprint("T112");
  Dprint(buf[0]);
  Dprint(buf[1]);
  Dprint(buf[2]);
  Dprint(buf[3]);
  Dprint(buf[4]);

  // Display List start
  DisplayKeypad(phost, keypressed, buf, 0);
  Dprint(curpos);

  do {
    keypressed = GetKeyPressed();

    if (keypressed > 0) {
      //Dprintln(keypressed);
      DisplayKeypad(phost, keypressed, buf, 0);
      WaitKeyRelease();
      Dprint("T10");

      switch (keypressed) {
        case BACK_SPACE:
          if (curpos >= 0) {
            if (curpos > 0) curpos--;
            buf[curpos] = 0;
            Dprint(curpos);
          }
          keypressed = 0;
          Dprint("T11");
          break;
        case NUM_ENTER:
          keypressed = 0;
          wait4key = FALSE;
          break;
        case BACK:
          keypressed = 0;
          wait4key = FALSE;
          LoadBuffer(buf, curval);
          break;
        default:
          if (curpos < KEYPAD_MAX_LEN - 1) {
            Dprint(curpos);
            buf[curpos] = keypressed;
            buf[++curpos] = 0;
            keypressed = 0;
          } else {  //max entry
            DisplayKeypad(phost, keypressed, buf, 1);
          }
          break;
      }
      Dprint(buf[0]);
      Dprint(buf[1]);
      Dprint(buf[2]);
      Dprint(buf[3]);
      Dprint(buf[4]);
      if (!wait4key) {
        tempval = atof(buf);
        Dprint("tempval=", tempval);
        if (tempval < minval || tempval > maxval) {  //out of range error
                                                     //v204 DisplayKeypad(phost,keypressed,buf,1);
                                                     //v204 delay(2000);
          if (tempval > maxval) LoadBuffer(buf, maxval);
          else LoadBuffer(buf, minval);
          curpos = strlen(buf);
          DisplayKeypad(phost, keypressed, buf, 0);
          wait4key = TRUE;  //error re enter again

        } else {
          curval = tempval;  //accept entry
          LoadBuffer(buf, curval);
          DisplayKeypad(phost, keypressed, buf, 0);
        }
      } else {
        DisplayKeypad(phost, keypressed, buf, 0);
      }
      if (wait4key) WaitKeyRelease();
    }

  } while (wait4key);

  return curval;
}