/*

Copyright (c) Bridgetek Pte Ltd
Copyright (c) Riverdi Sp. z o.o. sp. k.
Copyright (c) Lukasz Skalski <contact@lukasz-skalski.com>

THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
BRIDGETEK PTE LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

BRIDGETEK DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON BRIDGETEK PARTS.

BRIDGETEK DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.
F
IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.

Abstract:

Application to demonstrate function of EVE.

Author : Bridgetek
Modified by: XentiQ

Revision History:
0.1 - date 2017.03.24 - Initial version
0.2 - date 2022.12.14 - XentiQ version

* Author: erdiongson
* Date Created: 2024.02.26
* Version 2.04: i. Added some prints on serial monitor to double check the change in password             
*/

#include "Platform.h"
#include "App_Common.h"
#include <EEPROM.h>
#include <AccelStepper.h>
#include <math.h>
#include <string.h>
#include <inttypes.h>
#include "../Utils.h"
#include "../logic/InteractionsHandler.h"
/*************************************************************************************************
 *                      These functions work with FT8XX GPU buffer
 **************************************************************************************************/

uint32_t CmdBuffer_Index;
volatile uint32_t DlBuffer_Index;
volatile uint16_t toggleState;

uint8_t font = 27;
uint8_t Line = 0;
uint16_t line2disp = 0;


const int STARTPOS = 270; // GUI screen coordinates


inline int32_t minimum(int32_t a, int32_t b) { return a < b ? a : b; }

struct
{
    uint8_t Key_Detect : 1;
    uint8_t Caps : 1;
    uint8_t Numeric : 1;
    uint8_t Exit : 1;
} Flag;

//struct Notepad_buffer
//{
//    char8_t *temp;
//    char8_t notepad[MAX_LINES + 1][800];
//} Buffer;

#ifdef BUFFER_OPTIMIZATION
uint8_t DlBuffer[DL_SIZE];
uint8_t CmdBuffer[CMD_FIFO_SIZE];
#endif

void App_WrCoCmd_Buffer(Gpu_Hal_Context_t *phost, uint32_t cmd)
{
#ifdef BUFFER_OPTIMIZATION
    /* Copy the command instruction into buffer */
    uint32_t *pBuffcmd;
    /* Prevent buffer overflow */
    if (CmdBuffer_Index >= CMD_FIFO_SIZE)
    {
        // printf("CmdBuffer overflow\n");

        if (CmdBuffer_Index > 0)
        {
            Gpu_Hal_WrCmdBuf_nowait(phost, CmdBuffer, CmdBuffer_Index);
        }
        CmdBuffer_Index = 0;
    }

    pBuffcmd = (uint32_t *)&CmdBuffer[CmdBuffer_Index];
    *pBuffcmd = cmd;

#endif

#if defined(STM32_PLATFORM) || defined(ARDUINO_PLATFORM)
    Gpu_Hal_WrCmd32(phost, cmd);
#endif
    /* Increment the command index */
    CmdBuffer_Index += CMD_SIZE;
}

void App_WrDl_Buffer(Gpu_Hal_Context_t *phost, uint32_t cmd)
{
#ifdef BUFFER_OPTIMIZATION
    /* Copy the command instruction into buffer */
    uint32_t *pBuffcmd;
    /* Prevent buffer overflow */
    if (DlBuffer_Index < DL_SIZE)
    {
        pBuffcmd = (uint32_t *)&DlBuffer[DlBuffer_Index];
        *pBuffcmd = cmd;
    }
    else
    {
        printf("DlBuffer overflow\n");
    }

#endif

#if defined(STM32_PLATFORM) || defined(ARDUINO_PLATFORM)
    Gpu_Hal_Wr32(phost, (RAM_DL + DlBuffer_Index), cmd);
#endif
    /* Increment the command index */
    DlBuffer_Index += CMD_SIZE;
}

void App_WrCoStr_Buffer(Gpu_Hal_Context_t *phost, const char8_t *s)
{
#ifdef BUFFER_OPTIMIZATION
    uint16_t length = 0;

    if (CmdBuffer_Index >= CMD_FIFO_SIZE)
    {
        printf("CmdBuffer overflow\n");

        if (CmdBuffer_Index > 0)
        {
            Gpu_Hal_WrCmdBuf(phost, CmdBuffer, CmdBuffer_Index); // This blocking state may be infinite due to Display list overflow
        }
        CmdBuffer_Index = 0;
    }

    length = strlen(s) + 1; // last for the null termination

    strcpy((char *)&CmdBuffer[CmdBuffer_Index], s);

    /* increment the length and align it by 4 bytes */
    CmdBuffer_Index += ((length + 3) & ~3);
#endif
}

void App_Flush_DL_Buffer(Gpu_Hal_Context_t *phost)
{
#ifdef BUFFER_OPTIMIZATION
    if (DlBuffer_Index > 0)
        Gpu_Hal_WrMem(phost, RAM_DL, DlBuffer, DlBuffer_Index);
#endif
    DlBuffer_Index = 0;
}

void App_Flush_Co_Buffer(Gpu_Hal_Context_t *phost)
{
#ifdef BUFFER_OPTIMIZATION
    if (CmdBuffer_Index > 0)
        Gpu_Hal_WrCmdBuf(phost, CmdBuffer, CmdBuffer_Index);
#endif
    CmdBuffer_Index = 0;
}

void App_Flush_Co_Buffer_nowait(Gpu_Hal_Context_t *phost)
{
#ifdef BUFFER_OPTIMIZATION
    if (CmdBuffer_Index > 0)
        Gpu_Hal_WrCmdBuf_nowait(phost, CmdBuffer, CmdBuffer_Index);
#endif
    CmdBuffer_Index = 0;
}

void App_Set_DlBuffer_Index(uint32_t index)
{
    DlBuffer_Index = index;
}

void App_Set_CmdBuffer_Index(uint32_t index)
{
    CmdBuffer_Index = index;
}
/*************************************************************************************************
 *                      Application Utilities
 **************************************************************************************************/

static uint8_t sk = 0;
uint8_t App_Read_Tag(Gpu_Hal_Context_t *phost)
{
    static uint8_t Read_tag = 0, temp_tag = 0, ret_tag = 0;
    Read_tag = Gpu_Hal_Rd8(phost, REG_TOUCH_TAG);
    ret_tag = 0;
    if (Read_tag != 0) // Allow if the Key is released
    {
        if (temp_tag != Read_tag)
        {
            temp_tag = Read_tag;
            sk = Read_tag; // Load the Read tag to temp variable
        }
    }
    else
    {
        if (temp_tag != 0)
        {
            ret_tag = temp_tag;
            temp_tag = 0; // The event will be processed. Clear the tag
        }
        sk = 0;
    }
    return ret_tag;
}

uint8_t App_Touch_Update(Gpu_Hal_Context_t *phost, uint8_t *currTag, uint16_t *x, uint16_t *y)
{
    static uint8_t Read_tag = 0, temp_tag = 0, ret_tag = 0;
    uint32_t touch;
    Read_tag = Gpu_Hal_Rd8(phost, REG_TOUCH_TAG);
    ret_tag = 0;
    if (Read_tag != 0) // Allow if the Key is released
    {
        if (temp_tag != Read_tag)
        {
            temp_tag = Read_tag;
            sk = Read_tag; // Load the Read tag to temp variable
        }
    }
    else
    {
        if (temp_tag != 0)
        {
            ret_tag = temp_tag;
            temp_tag = 0; // The event will be processed. Clear the tag
        }
        sk = 0;
    }
    *currTag = Read_tag;
    touch = Gpu_Hal_Rd32(phost, REG_TOUCH_SCREEN_XY);
    *x = (uint16_t)(touch >> 16);
    *y = (uint16_t)(touch & 0xFFFF);
    return ret_tag;
}
void App_Play_Sound(Gpu_Hal_Context_t *phost, uint8_t sound, uint8_t vol, uint8_t midi)
{
    uint16_t val = (midi << 8) | sound;
    Gpu_Hal_Wr8(phost, REG_VOL_SOUND, vol);
    Gpu_Hal_Wr16(phost, REG_SOUND, val);
    Gpu_Hal_Wr8(phost, REG_PLAY, 1);
}

void App_Calibrate_Screen(Gpu_Hal_Context_t *phost)
{
    Gpu_CoCmd_Dlstart(phost);
    App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    Gpu_CoCmd_Text(phost, DispWidth / 2, DispHeight / 2, 26, OPT_CENTERX | OPT_CENTERY, "Please tap on a dot");

    /* Calibration */
    Gpu_CoCmd_Calibrate(phost, 0);
    App_Flush_Co_Buffer(phost);
    Gpu_Hal_WaitCmdfifo_empty(phost);
}

void App_Common_Init(Gpu_Hal_Context_t *phost)
{
    Gpu_HalInit_t halinit;
    uint8_t chipid;

    Gpu_Hal_Init(&halinit);
    Gpu_Hal_Open(phost);

    Gpu_Hal_Powercycle(phost, TRUE);

    /* FT81x will be in SPI Single channel after POR
    If we are here with FT4222 in multi channel, then
    an explicit switch to single channel is essential
    */
#ifdef FT81X_ENABLE
    Gpu_Hal_SetSPI(phost, GPU_SPI_SINGLE_CHANNEL, GPU_SPI_ONEDUMMY);
#endif

    /* access address 0 to wake up the chip */
    Gpu_HostCommand(phost, GPU_ACTIVE_M);
    Gpu_Hal_Sleep(300);

    // Gpu_HostCommand(phost,GPU_INTERNAL_OSC);
    Gpu_HostCommand(phost, GPU_EXTERNAL_OSC); // Added for ips_35 display
    Gpu_Hal_Sleep(100);

    /* read Register ID to check if chip ID series is correct */
    chipid = Gpu_Hal_Rd8(phost, REG_ID);
    while (chipid != 0x7C)
    {
        chipid = Gpu_Hal_Rd8(phost, REG_ID);
        Gpu_Hal_Sleep(100);
    }

    /* read REG_CPURESET to confirm 0 is returned */
    {
        uint8_t engine_status;

        /* Read REG_CPURESET to check if engines are ready.
             Bit 0 for coprocessor engine,
             Bit 1 for touch engine,
             Bit 2 for audio engine.
        */
        engine_status = Gpu_Hal_Rd8(phost, REG_CPURESET);
        while (engine_status != 0x00)
        {
            engine_status = Gpu_Hal_Rd8(phost, REG_CPURESET);
            Gpu_Hal_Sleep(100);
        }
    }

    /* configuration of LCD display */
    Gpu_Hal_Wr16(phost, REG_HCYCLE, DispHCycle);
    Gpu_Hal_Wr16(phost, REG_HOFFSET, DispHOffset);
    Gpu_Hal_Wr16(phost, REG_HSYNC0, DispHSync0);
    Gpu_Hal_Wr16(phost, REG_HSYNC1, DispHSync1);
    Gpu_Hal_Wr16(phost, REG_VCYCLE, DispVCycle);
    Gpu_Hal_Wr16(phost, REG_VOFFSET, DispVOffset);
    Gpu_Hal_Wr16(phost, REG_VSYNC0, DispVSync0);
    Gpu_Hal_Wr16(phost, REG_VSYNC1, DispVSync1);
    Gpu_Hal_Wr8(phost, REG_SWIZZLE, DispSwizzle);
    Gpu_Hal_Wr8(phost, REG_PCLK_POL, DispPCLKPol);
    Gpu_Hal_Wr16(phost, REG_HSIZE, DispWidth);
    Gpu_Hal_Wr16(phost, REG_VSIZE, DispHeight);
    Gpu_Hal_Wr16(phost, REG_CSPREAD, DispCSpread);
    Gpu_Hal_Wr16(phost, REG_DITHER, DispDither);
    Gpu_Hal_Wr16(phost, REG_TOUCH_RZTHRESH, 1200);

    /* GPIO configuration */
#if defined(FT81X_ENABLE)
    Gpu_Hal_Wr16(phost, REG_GPIOX_DIR, 0xffff);
    Gpu_Hal_Wr16(phost, REG_GPIOX, 0xffff);
#else
    Gpu_Hal_Wr8(phost, REG_GPIO_DIR, 0xff);
    Gpu_Hal_Wr8(phost, REG_GPIO, 0xff);
#endif

    Gpu_ClearScreen(phost);

    /* after this display is visible on the LCD */
    Gpu_Hal_Wr8(phost, REG_PCLK, DispPCLK);

    phost->cmd_fifo_wp = Gpu_Hal_Rd16(phost, REG_CMD_WRITE);
}

void App_Common_Close(Gpu_Hal_Context_t *phost)
{
    Gpu_Hal_Close(phost);
    Gpu_Hal_DeInit();
}

/*************************************************************************************************
 *                                XentiQ Functions - 23JUN23
 *************************************************************************************************/

void Logo_XQ_trans(Gpu_Hal_Context_t *phost)
{
    Gpu_CoCmd_FlashFast(phost, 0);
    Gpu_CoCmd_Dlstart(phost);

    Gpu_Hal_WrCmd32(phost, CMD_INFLATE);
    Gpu_Hal_WrCmd32(phost, RAM_XQ_LOGO);
    Gpu_Hal_WrCmdBufFromFlash(phost, (uint8_t *)XQ_Logo_trans, sizeof(XQ_Logo_trans));

    App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
    App_WrCoCmd_Buffer(phost, END());
    App_WrCoCmd_Buffer(phost, BITMAP_HANDLE(0));
    Gpu_CoCmd_SetBitmap(phost, 0, RGB565, 100, 82);
    App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
    App_WrCoCmd_Buffer(phost, PALETTE_SOURCE(0));
    App_WrCoCmd_Buffer(phost, VERTEX2F(1856, 1152));
    App_WrCoCmd_Buffer(phost, END());

    Disp_End(phost);
    Gpu_Hal_Sleep(800);
}

void Disp_End(Gpu_Hal_Context_t *phost)
{
    App_WrCoCmd_Buffer(phost, DISPLAY());
    // swap the current display list with the new display list
    Gpu_CoCmd_Swap(phost);
    App_Flush_Co_Buffer(phost);
    Gpu_Hal_WaitCmdfifo_empty(phost);

    /* End of command sequence */
    // This code below will cause flickering on the Tray_Screen
    // Gpu_CoCmd_Dlstart(phost);
    // Gpu_Copro_SendCmd(phost, CMD_STOP);
    // App_Flush_Co_Buffer(phost);
    // Gpu_Hal_WaitCmdfifo_empty(phost);
}

void DisplayKeyboard(Gpu_Hal_Context_t *phost, uint8_t keypressed,char *displaytext,char *displaytitle, bool numlock,bool caplock, bool errorcode)
{
	char buf[PROFILE_NAME_MAX_LEN+8];

	// Display List start
	Gpu_CoCmd_Dlstart(phost);
	App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(120, 120, 120));
	App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
	App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
	App_WrCoCmd_Buffer(phost, TAG_MASK(1)); // enable tagbuffer updation

	Gpu_CoCmd_FgColor(phost, 0x703800);
	Gpu_CoCmd_BgColor(phost, 0x703800);
	
	Gpu_CoCmd_FgColor(phost, 0xAA0000);

	App_WrCoCmd_Buffer(phost, TAG(CLEAR_KEY));		 // Back		 Return to Home
	Gpu_CoCmd_Button(phost, (DispWidth * 0.855), (DispHeight * 0.83), (DispWidth * 0.146), (DispHeight * 0.112),
					 font, (keypressed==CLEAR_KEY)? OPT_FLAT : 0, "Clear");
	Gpu_CoCmd_FgColor(phost, 0x703800);


	Gpu_CoCmd_FgColor(phost, 0xADAF3C);
	App_WrCoCmd_Buffer(phost, TAG(BACK_SPACE)); // BackSpace
	Gpu_CoCmd_Button(phost, (DispWidth * 0.875), (DispHeight * 0.70), (DispWidth * 0.125),
					 (DispHeight * 0.112), font, (keypressed==BACK_SPACE)? OPT_FLAT : 0, "<-");
	Gpu_CoCmd_FgColor(phost, 0x703800);



	Gpu_CoCmd_FgColor(phost, 0x00a2e8);
    App_WrCoCmd_Buffer(phost, TAG(KBBACK)); // Back		 Return to Home
    Gpu_CoCmd_Button(phost, (DispWidth * 0.115), (DispHeight * 0.83), (DispWidth * 0.192), (DispHeight * 0.112),
                     font, (keypressed==KBBACK)? OPT_FLAT : 0, "Back");
	
	Gpu_CoCmd_FgColor(phost, 0x202020);//0x703800);
	App_WrCoCmd_Buffer(phost, TAG(' ')); // Space
	Gpu_CoCmd_Button(phost, (DispWidth * 0.315), (DispHeight * 0.83), (DispWidth * 0.33), (DispHeight * 0.112),
					 font, (keypressed==' ')? OPT_FLAT : 0, "Space");

    if (!numlock)
    {
        Gpu_CoCmd_Keys(phost, 0, (DispHeight * 0.442), DispWidth, (DispHeight * 0.112), font, keypressed,
                       ((caplock) ? "QWERTYUIOP" : "qwertyuiop"));
        Gpu_CoCmd_Keys(phost, (DispWidth * 0.042), (DispHeight * 0.57), (DispWidth * 0.96), (DispHeight * 0.112),
                       font, keypressed, ((caplock) ? "ASDFGHJKL" : "asdfghjkl"));
        Gpu_CoCmd_Keys(phost, (DispWidth * 0.125), (DispHeight * 0.70), (DispWidth * 0.73), (DispHeight * 0.112),
                       font, keypressed, ((caplock) ? "ZXCVBNM" : "zxcvbnm"));


        App_WrCoCmd_Buffer(phost, TAG(CAPS_LOCK)); // Capslock
        Gpu_CoCmd_Button(phost, 0, (DispHeight * 0.70), (DispWidth * 0.10), (DispHeight * 0.112), font, 
						(keypressed==CAPS_LOCK)? OPT_FLAT : 0,
                         ((caplock) ?"a^" : "A^"));
        App_WrCoCmd_Buffer(phost, TAG(NUMBER_LOCK)); // Numberlock
        Gpu_CoCmd_Button(phost, 0, (DispHeight * 0.83), (DispWidth * 0.10), (DispHeight * 0.112), font, 
						(keypressed==NUMBER_LOCK)? OPT_FLAT : 0,
                         "12*");
    }
	else
    {
        Gpu_CoCmd_Keys(phost, (DispWidth * 0), (DispHeight * 0.442), DispWidth, (DispHeight * 0.112), font,
                       keypressed, "1234567890");
        Gpu_CoCmd_Keys(phost, (DispWidth * 0.042), (DispHeight * 0.57), (DispWidth * 0.96), (DispHeight * 0.112),
                       font, keypressed, "-@#%^&*()");
        Gpu_CoCmd_Keys(phost, (DispWidth * 0.125), (DispHeight * 0.70), (DispWidth * 0.73), (DispHeight * 0.112),
                       font, keypressed, ",_+[]{}");
        App_WrCoCmd_Buffer(phost, TAG(NUMBER_LOCK)); // Numberlock
        Gpu_CoCmd_Button(phost, 0, (DispHeight * 0.83), (DispWidth * 0.10), (DispHeight * 0.112), font, 
						(keypressed==NUMBER_LOCK)? OPT_FLAT : 0,
                         "AB*");
    }
	Gpu_CoCmd_FgColor(phost, 0x006400);
	App_WrCoCmd_Buffer(phost, TAG(SAVE_KEY)); // Enter
	Gpu_CoCmd_Button(phost, (DispWidth * 0.653), (DispHeight * 0.83), (DispWidth * 0.192), (DispHeight * 0.112), font, 
					(keypressed==SAVE_KEY)? OPT_FLAT : 0, "Enter");
	Gpu_CoCmd_FgColor(phost, 0x703800);
	App_WrCoCmd_Buffer(phost, TAG_MASK(0)); // Disable the tag buffer updates
	App_WrCoCmd_Buffer(phost, SCISSOR_XY(0, 0));
	App_WrCoCmd_Buffer(phost, SCISSOR_SIZE(DispWidth, (uint16_t)(DispHeight * 0.41)));
	App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
	App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
	App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255)); // Text Color

    if(errorcode)
	{
		sprintf(buf,"%s\n%s",displaytext,"error");
		Gpu_CoCmd_Text(phost, 0, 0, font, 0, buf);
    }
	else
		Gpu_CoCmd_Text(phost, 0, 0, font, 0, displaytext);

	App_WrCoCmd_Buffer(phost, SCISSOR_XY(0, 77));
	App_WrCoCmd_Buffer(phost, SCISSOR_SIZE(DispWidth, (uint16_t)(DispHeight * 0.1)));
	App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(120, 120, 120));
	App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
	App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 0));
	Gpu_CoCmd_Text(phost, 160 - (strlen(displaytitle)*5), 78, 27, 0, displaytitle);

	Disp_End(phost);

	
}
uint8_t GetKeyPressed(void) {
    Interaction interaction;
    do {
        InteractionsHandler::getTouchInteraction(interaction);
    } while (interaction.key_pressed == 0);
    return interaction.key_pressed;
}

void WaitKeyRelease(void)
{
	while(Gpu_Hal_Rd8(phost, REG_TOUCH_TAG)>0);
}

void Keyboard(Gpu_Hal_Context_t *phost,char *curtext,char *curtitle,bool password)
{
	uint8_t keypressed = 0;

    delay(200); // Added to create smooth transition between screen
    //phost = &host;
    /*local variables*/
    uint16_t noofchars = 0, line2disp = 0, nextline = 0;
    uint8_t font = 27;
	//char curtext[PROFILE_NAME_MAX_LEN]="hello";
	char buf[PROFILE_NAME_MAX_LEN+20];
	uint8_t i;
	uint8_t curpos=0;
	bool wait4key=TRUE;
	bool numlock=FALSE;
	bool caplock=FALSE;


	for(i=0;i<PROFILE_NAME_MAX_LEN;i++) buf[i]='\0';
    strcpy(buf, curtext);

	
    /*initial setup*/
    curpos = strlen(buf);                                   // starting pos
   	buf[curpos]=0;
    Flag.Numeric = OFF;                                         // Disable the numbers and spcial charaters

	DisplayKeyboard(phost, keypressed,buf,curtitle,numlock,caplock,0);
	//for(i=0;i<14;i++) Dprint(buf[i]);
	Dprint("end");

	do{
        keypressed = GetKeyPressed();
	
		if(keypressed>0)
		{
			//DisplayKeyboard(phost, keypressed,buf,numlock,caplock,0);
			WaitKeyRelease();

            switch (keypressed)
            {
                case BACK_SPACE:
                    if (curpos>0) // check in the line there is any characters are present, cursor not included
                    {
	                    Dprint("BKSP");
                        curpos--;                                                             // clear the character in the buffer
                        buf[curpos]=0;
						if(password) curtext[curpos]=0;
						Dprint(curpos);      
                    }
					keypressed=0;
                    break;

                case CAPS_LOCK:
					Dprint("cap");
					keypressed=0;
                    caplock=!caplock; // toggle the caps lock on when the key detect
                    break;

                case NUMBER_LOCK:
					Dprint("num");
					keypressed=0;
                    numlock=(numlock)? FALSE:TRUE; // toggle the number lock on when the key detect
                    break;
				case CLEAR_KEY: keypressed=0;
							curpos=0;
							buf[curpos]=0;
							if(password) curtext[curpos]=0;
							break;
				case KBBACK: 	keypressed=0;
								wait4key=FALSE;
								break;
					
				case SAVE_KEY:	keypressed=0;
								wait4key=FALSE;
								strcpy(curtext,buf);
								break;
				default:	if(curpos<PROFILE_NAME_MAX_LEN)
							{
								//Dprint(curpos);
								buf[curpos]=keypressed;
								if(password)
								{
									curtext[curpos]='*';
									curtext[curpos+1]=0;
								}
								buf[++curpos]=0;
								keypressed=0;
							}
							else 
							{
								DisplayKeyboard(phost, keypressed,buf,curtitle,numlock,caplock,TRUE);
								keypressed=0;
								delay(500);
							}
							break;
								
            }
			if(wait4key)
			{
				if(password)
					DisplayKeyboard(phost, keypressed,curtext,curtitle,numlock,caplock,0);
				else
					DisplayKeyboard(phost, keypressed,buf,curtitle,numlock,caplock,0);
			}
		}
   }while (wait4key);

}

void Round1Dec(float *x)
{
	char buf[PROFILE_NAME_MAX_LEN];

	dtostrf(*x,3,1,buf);
	*x=atof(buf);
}

void DisplayKeypad(Gpu_Hal_Context_t *phost, int32_t keypressed,char* displaynum,int8_t errorcode)
{
	char buf[KEYPAD_MAX_LEN];

    Gpu_CoCmd_Dlstart(phost);
    App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(120, 120, 120));
    App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    App_WrCoCmd_Buffer(phost, TAG_MASK(1)); // enable tagbuffer updation
	//Gpu_CoCmd_FgColor(phost, 0xB40000);     // 0x703800 0xD00000
  	//Gpu_CoCmd_BgColor(phost, 0xB40000);
	Gpu_CoCmd_FgColor(phost, 0x202020);
	Gpu_CoCmd_BgColor(phost, 0x202020);

	Gpu_CoCmd_FgColor(phost, 0x00a2e8);
    App_WrCoCmd_Buffer(phost, TAG(BACK)); // Back		 Return to Home
    Gpu_CoCmd_Button(phost, 207, 37, 67, 46, font, (keypressed==BACK)? OPT_FLAT :0 , "Back");

	Gpu_CoCmd_FgColor(phost, 0xADAF3C);
    App_WrCoCmd_Buffer(phost, TAG(BACK_SPACE)); // BackSpace
    Gpu_CoCmd_Button(phost, 207, 87, 67, 50, 31, (keypressed==BACK_SPACE)? OPT_FLAT :0 , "<-");

	Gpu_CoCmd_FgColor(phost, 0x006400);
    App_WrCoCmd_Buffer(phost, TAG(NUM_ENTER)); // Enter
    Gpu_CoCmd_Button(phost, 207, 141, 67, 90, font, (keypressed==NUM_ENTER)? OPT_FLAT :0 , "Enter");

	Gpu_CoCmd_FgColor(phost, 0x202020);
	Gpu_CoCmd_BgColor(phost, 0x202020);

    Gpu_CoCmd_Keys(phost, 47, 35, 150, 50, 29, keypressed, "789");
    Gpu_CoCmd_Keys(phost, 47, 88, 150, 50, 29, keypressed, "456");
    Gpu_CoCmd_Keys(phost, 47, 140, 150, 50, 29, keypressed, "123");
    Gpu_CoCmd_Keys(phost, 47, 191, 150, 40, 29, keypressed, "0.");

    
	App_WrCoCmd_Buffer(phost, TAG_MASK(0)); // Disable the tag buffer updates
	App_WrCoCmd_Buffer(phost, SCISSOR_XY(0, 0));
	App_WrCoCmd_Buffer(phost, SCISSOR_SIZE(320, 34));
	
	App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0,0,0));
	App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
	App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255)); // Text Color

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
    temp=(int8_t)curval;

    if(curval-temp>0) {
        dtostrf(curval,3,1,buf);
        buf[KEYPAD_MAX_LEN-1] = '\0'; // Force null termination
    } else {
        sprintf(buf,"%d",(int16_t)curval);
        buf[KEYPAD_MAX_LEN-1] = '\0'; // Force null termination
    }
}

float Keypad(Gpu_Hal_Context_t *phost, float curval, float minval, float maxval ,bool isfloat)
{
    phost = &host;
    uint8_t keypressed = 0;
	bool wait4key=TRUE;

	char buf[KEYPAD_MAX_LEN];
	int8_t curpos;
	float tempval;

	for(int i=0;i<KEYPAD_MAX_LEN;i++) buf[i]=0;
	LoadBuffer(buf, curval);
	curpos=strlen(buf);
	buf[curpos]=0;

	Dprint("T112");
	Dprint(buf[0]);
	Dprint(buf[1]);
	Dprint(buf[2]);
	Dprint(buf[3]);
	Dprint(buf[4]);

        // Display List start
	DisplayKeypad(phost,keypressed,buf,0);
	Dprint(curpos);

	do{
		keypressed = GetKeyPressed();

		if(keypressed>0)
		{
			//Dprintln(keypressed);
			DisplayKeypad(phost,keypressed,buf,0);
			WaitKeyRelease();
			Dprint("T10");

			switch(keypressed)
			{
				case BACK_SPACE:    if(curpos>=0)
									{
										if(curpos>0) curpos--;
										buf[curpos]=0;									
										Dprint(curpos);
									}
									keypressed=0;
									Dprint("T11");
									break;
				case NUM_ENTER: keypressed=0;
								wait4key=FALSE;
								break;
				case BACK: 	keypressed=0;
							wait4key=FALSE;
							LoadBuffer(buf,curval);
							break;
				default: 	if(curpos<KEYPAD_MAX_LEN-1)
							{
								Dprint(curpos);
								buf[curpos]=keypressed;
								buf[++curpos]=0;
								keypressed=0;
							}
							else
							{//max entry
								DisplayKeypad(phost,keypressed,buf,1);
							}
							break;
									
									
			}
			Dprint(buf[0]);
			Dprint(buf[1]);
			Dprint(buf[2]);
			Dprint(buf[3]);
			Dprint(buf[4]);
			if(!wait4key)
			{
				tempval=atof(buf);
				Dprint("tempval=",tempval);
				if(tempval<minval || tempval>maxval)
				{//out of range error
          //v204 DisplayKeypad(phost,keypressed,buf,1);
          //v204 delay(2000);
					if(tempval>maxval) LoadBuffer(buf,maxval);
					else LoadBuffer(buf,minval);
					curpos=strlen(buf);
					DisplayKeypad(phost,keypressed,buf,0);
					wait4key=TRUE;//error re enter again
					
				}
				else
				{
					curval=tempval;//accept entry
					LoadBuffer(buf,curval);
					DisplayKeypad(phost,keypressed,buf,0);
				}
			}
			else 
			{
				DisplayKeypad(phost,keypressed,buf,0);
			}
			if(wait4key) WaitKeyRelease();
		}
		
	}while(wait4key);

	return curval;
}

// Renders a toggle button with customizable colors based on enabled state.
// @param phost The GPU HAL context.
// @param enabled The toggle state (true = ON/enabled, false = OFF/disabled).
// @param tag The tag ID for the toggle button.
// @param x The x-coordinate of the toggle button.
// @param y The y-coordinate of the toggle button.
// @param enabledLabel The label to display when enabled.
// @param disabledLabel The label to display when disabled.
void Toggle_Button(Gpu_Hal_Context_t *phost, bool enabled, uint8_t tag, int16_t x, int16_t y, const char *enabledLabel, const char *disabledLabel) {
  char labels[50];
  snprintf(labels, sizeof(labels), "%s\xFF%s", enabledLabel, disabledLabel);
  
  int16_t toggleState = enabled ? 0 : 65535;
  
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_BgColor(phost, 0x00A2E8);
  
  // Check the enabled state and set the foreground color accordingly
  if (enabled) {
    Gpu_CoCmd_FgColor(phost, 0x007300);               // Green when ON
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 128, 0));  // text color
  } else {
    Gpu_CoCmd_FgColor(phost, 0xFF0000);               // Red when OFF
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 0, 0));  // text color
  }

  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(tag));
  Gpu_CoCmd_Toggle(phost, x, y, 40, 21, OPT_FLAT | OPT_FORMAT, toggleState, labels);
  Gpu_CoCmd_BgColor(phost, 0x00A2E8);
  Gpu_CoCmd_FgColor(phost, 0x007300);
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
}