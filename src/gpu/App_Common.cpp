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
uint8_t GetKeyPressed(void)
{
	uint8_t tempkey[2];
	
	tempkey[0]=Gpu_Hal_Rd8(phost, REG_TOUCH_TAG);
	//delay(10);
	tempkey[1]=Gpu_Hal_Rd8(phost, REG_TOUCH_TAG);

	return (tempkey[0] == tempkey[1])? tempkey[0] : 0;
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

  String KeyboardWithReturn(Gpu_Hal_Context_t *phost, String &buf, String &curtitle, bool numlock, bool caplock)
{
    char curtext[PROFILE_NAME_MAX_LEN];
    char title[100];
    
    // Convert String to C-string
    buf.toCharArray(curtext, PROFILE_NAME_MAX_LEN);
    curtitle.toCharArray(title, 100);
    
    // Call the original Keyboard function
    Keyboard(phost, curtext, title, false);
    
    // Copy result back to buf and return
    buf = String(curtext);
    return buf;
}



void DisplayProfileMenu(uint8_t keypressed, uint8_t curprofnum)//try054
{
	char buf[100]; // a buffer to format your text before printing.

	Gpu_CoCmd_FlashFast(phost, 0);
	Gpu_CoCmd_Dlstart(phost);
	
	App_WrCoCmd_Buffer(phost, CLEAR_TAG(255));
	App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
	App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
	
	App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
	App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
	Gpu_CoCmd_Text(phost, 84, 16, 27, 0, "Profile Selector");
	
	Gpu_CoCmd_FgColor(phost, 0x00A2E8);
	App_WrCoCmd_Buffer(phost, TAG(PROFILEBACK));
	App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
	Gpu_CoCmd_Button(phost, 233, 11, 76, 26, 21, (keypressed==PROFILEBACK)? OPT_FLAT :0 , "Back");
	
	Gpu_CoCmd_FgColor(phost, 0x00A2E8);
	App_WrCoCmd_Buffer(phost, TAG(PROFILELOAD));
	App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
	Gpu_CoCmd_Button(phost, 8, 205, 76, 26, 21, (keypressed==PROFILELOAD)? OPT_FLAT :0 , "Load");
	
	Gpu_CoCmd_FgColor(phost, 0x00A2E8);
	App_WrCoCmd_Buffer(phost, TAG(PROFILEPASS)); //disable advanced button
	App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));		
	Gpu_CoCmd_Button(phost, 180, 205, 132, 26, 21, (keypressed==PROFILEPASS)? OPT_FLAT :0 , "Change Password");
	
	Gpu_CoCmd_FgColor(phost, 0x00A2E8);
	App_WrCoCmd_Buffer(phost, TAG(PROFILEUP));
	//App_WrCoCmd_Buffer(phost, TAG(350)); //disbale Up
	App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
	Gpu_CoCmd_Button(phost, 265, 76, 44, 29, 21, (keypressed==PROFILEUP)? OPT_FLAT :0 , "Up");
	
	Gpu_CoCmd_FgColor(phost, 0x00A2E8);
	App_WrCoCmd_Buffer(phost, TAG(PROFILEDOWN));// disbale Up and down
	//App_WrCoCmd_Buffer(phost, TAG(300)); //disbale Down and down
	App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
	Gpu_CoCmd_Button(phost, 265, 121, 44, 29, 21, (keypressed==PROFILEDOWN)? OPT_FLAT :0 , "Down");
	
	App_WrCoCmd_Buffer(phost, BEGIN(RECTS)); // Profile Name Field
	App_WrCoCmd_Buffer(phost, VERTEX2F(160, 1008));
	App_WrCoCmd_Buffer(phost, VERTEX2F(4944, 704));
	
	App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
	
	Gpu_CoCmd_Text(phost, 160, 54, 27, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, (const char *)SelectProf.profileName);//try054
	App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 127));    

    // Print profileId
    sprintf(buf, "Profile ID:    %d", curprofnum+1);
    Gpu_CoCmd_Text(phost, 8, 65, 21, 0, buf);

    // // Print profileName
    // sprintf(buffer, "Profile Name: %s", curprof.profileName);
    // Gpu_CoCmd_Text(phost, 84, 56, 27, 0, buffer);

    // Print Tube_No_x
    sprintf(buf, "Columns:     %d", SelectProf.Tube_No_x);//try054
    Gpu_CoCmd_Text(phost, 8, 80, 21, 0, buf);

    // Print Tube_No_y
    sprintf(buf, "Rows:         %d", SelectProf.Tube_No_y);//try054
    Gpu_CoCmd_Text(phost, 8, 95, 21, 0, buf);

    // Print pitch_x
    dtostrf(SelectProf.pitch_x, 4, 1, buf);//try054
    Gpu_CoCmd_Text(phost, 8, 110, 21, 0, "Pitch(Col):");
    Gpu_CoCmd_Text(phost, 82, 110, 21, 0, buf);

    // Print pitch_y
    dtostrf(SelectProf.pitch_y, 4, 1, buf);//try054
    Gpu_CoCmd_Text(phost, 8, 125, 21, 0, "Pitch(Ros):");
    Gpu_CoCmd_Text(phost, 83, 125, 21, 0, buf);

    // Print trayOriginX
    dtostrf(SelectProf.trayOriginX, 4, 1, buf);//try054
    Gpu_CoCmd_Text(phost, 8, 140, 21, 0, "OriginX:");
    Gpu_CoCmd_Text(phost, 80, 140, 21, 0, buf);

    // Print trayOriginY
    dtostrf(SelectProf.trayOriginY, 4, 1, buf);//try054
    Gpu_CoCmd_Text(phost, 8, 155, 21, 0, "OriginY:");
    Gpu_CoCmd_Text(phost, 80, 155, 21, 0, buf);

    // Print Cycles
    sprintf(buf, "Cycles:       %d" , SelectProf.Cycles);//try054
    Gpu_CoCmd_Text(phost, 8, 170, 21, 0, buf);

    // Print vibrationEnabled
    //sprintf(buf, "Vibration:    %s", curprof.vibrationEnabled ? "True" : "False");
    //20240903 - erdiongson: Change in Vibration Level
    sprintf(buf, "Vibration Level:    %d", SelectProf.vibrationEnabled);//try054
    Gpu_CoCmd_Text(phost, 8, 185, 21, 0, buf);

    // Print passwordEnabled
    sprintf(buf, "Password: %s" , SelectProf.passwordEnabled ? "True" : "False");//try054
    Gpu_CoCmd_Text(phost, 150, 65, 21, 0, buf);

    // Print vibrationDuration
    //20241001 - erdiongson: Change in Vibration Duration
    sprintf(buf, "Vibration Time: %d", SelectProf.vibrationDuration);//try054
    Gpu_CoCmd_Text(phost, 150, 80, 21, 0, buf);
	
	Disp_End(phost);

}
void Profile_Menu(Gpu_Hal_Context_t *phost)
{
   	//try054 Profile selectprof;
	int8_t selectprofnum;

    uint8_t keypressed = 0;
	bool wait4key=TRUE;

	selectprofnum=CurProfNum;
	ReadProfileMinEEPROM(selectprofnum);//try054
	DisplayProfileMenu(0,selectprofnum);//try054

    do{
        keypressed = GetKeyPressed();
        if (keypressed >0 )
        {
        	DisplayProfileMenu(keypressed,selectprofnum);//try054
			WaitKeyRelease();

			switch(keypressed)
			{
				case PROFILEBACK: 	wait4key=FALSE;
									keypressed=0;
									break;
				case PROFILELOAD: wait4key=FALSE;
									keypressed=0;
									CurProfNum=selectprofnum;
									WriteCurIDEEPROM(selectprofnum);
									ReadProfileEEPROM(CurProfNum);
									ReadProfileMinEEPROM(selectprofnum);//try055
									break;
				case PROFILEUP: keypressed=0;
								if(selectprofnum<MAX_PROFILES-1)
								{
									++selectprofnum;
								}
								else
								{
									selectprofnum=0;
								}
								ReadProfileMinEEPROM(selectprofnum);//try054
								DisplayProfileMenu(keypressed,selectprofnum);//try054
								break;
				case PROFILEDOWN: 	keypressed=0;
								if(selectprofnum>0)
								{
									--selectprofnum;
								}
								else
								{
									selectprofnum=MAX_PROFILES-1;
								}
								ReadProfileMinEEPROM(selectprofnum);//try054
								
								DisplayProfileMenu(keypressed,selectprofnum);//try054
								break;
				case PROFILEPASS: 								
								Password[2][0]=0;
								Keyboard(phost,Password[2],"Enter New Password",FALSE);
								if(Password[2][0]!=0)							
								{
									Password[3][0]=0;
                  //20240226: erdiongson - Prints on the Serial Monitor for Password change Monitoring
									Serial.print("Old Password[1]: ");
                  Serial.print(Password[1]);
                  Serial.println();      
									Keyboard(phost,Password[3],"Enter New Password again",FALSE);
                  
									if(strcmp(Password[2],Password[3])==0)
									{
										strcpy(Password[1],Password[2]);
                    //20240226: erdiongson - Prints on the Serial Monitor for Password change Monitoring
                    Serial.print("New Password[1]: ");
                    Serial.print(Password[1]);
                    Serial.println();
										WritePassEEPROM(Password[1]);
										Keyboard(phost,"Password changed","Press Back to continue", FALSE);
									}
									else
									{
										Keyboard(phost,"Error : Different password entered","Press Back to continue", FALSE);
									}
								}
								else
								{
									Keyboard(phost,"Error : No password entered","Press Back to continue", FALSE);
								}
								//keypressed=0;
								break;
				default : break;
				
			}
        }

		DisplayProfileMenu(keypressed,selectprofnum);//try054

    }while(wait4key);
}
void UpdateConfigSet(Gpu_Hal_Context_t *phost, Profile curprof)
{


}
	

void Round1Dec(float *x)
{
	char buf[PROFILE_NAME_MAX_LEN];

	dtostrf(*x,3,1,buf);
	*x=atof(buf);
}

void DisplayKeypad(Gpu_Hal_Context_t *phost, int32_t keypressed,char* displaynum,int8_t errorcode)
{
	char buf[KEYPAD_MAX_LEN+20];

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
		sprintf(buf,displaynum);
	Gpu_CoCmd_Text(phost, 0, 0, 30, 0, buf);

    App_WrCoCmd_Buffer(phost, DISPLAY());
    Gpu_CoCmd_Swap(phost);
    App_Flush_Co_Buffer(phost);

}

void LoadBuffer( char *buf, float curval)
{
	float temp;

	temp=(int8_t)curval;
	if(curval-temp>0)
	{
		dtostrf(curval,3,1,buf);
	}
	else
		sprintf(buf,"%d",(int16_t)curval);

}

float Keypad(Gpu_Hal_Context_t *phost, float curval, float minval, float maxval ,bool isfloat)
{
    phost = &host;
    uint8_t keypressed = 0;
	bool wait4key=TRUE;

	char buf[KEYPAD_MAX_LEN+1];
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

void displayTextMatrixName(int startRow, int startCol)
{
    int id;
    
//v205 B
  char matrixName[20];

  id = (CurProf.Tube_No_x % 7) ? 1:0;
  id= (CurProf.Tube_No_x / 7) + id;

    if (startRow >= 1 && startRow <= 7)
    {
        id = (startCol <= 7) ? 1 : (startCol <= 14) ? 2: (startCol <= 21) ? 3: (startCol <= 28) ? 4: 5;
    }
    else if (startRow >= 8 && startRow <= 14)
    {
      id += (startCol <= 7) ? 1 : (startCol <= 14) ? 2: (startCol <= 21) ? 3: (startCol <= 28) ? 4: 5;
    }
    else if (startRow >= 15 && startRow <= 21)
  {
        id = id * 2 + ((startCol <= 7) ? 1 : (startCol <= 14) ? 2: (startCol <= 21) ? 3: (startCol <= 28) ? 4: 5);
  }
  else if (startRow >= 22 && startRow <= 28)
  {
      id = id * 3 + ((startCol <= 7) ? 1 : (startCol <= 14) ? 2: (startCol <= 21) ? 3: (startCol <= 28) ? 4: 5);
  }
  else
  {
    id = id * 4 + ((startCol <= 7) ? 1 : (startCol <= 14) ? 2: (startCol <= 21) ? 3: (startCol <= 28) ? 4: 5);
  }

  sprintf(matrixName,"Matrix %d",id);
//v205 E
    Gpu_CoCmd_Text(phost, 280, 230, 20, OPT_CENTER, matrixName); // Display matrix name
}

void toggleButton(Gpu_Hal_Context_t *phost, int tag, int x, int y, int buttonSize, int row, int col, int i, int j)
{
    bool buttonState = CurProf.buttonStates[col - 1][row - 1];
	char buf[10];

    App_WrCoCmd_Buffer(phost, BEGIN(FTPOINTS));
    App_WrCoCmd_Buffer(phost, POINT_SIZE(buttonSize * 10));

	//Dprint("x=",x);
	//Dprint("y=",y);
	App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 127));
	if (i == 0) // First column on each page
	{
		sprintf(buf, "C%d",col);
		Gpu_CoCmd_Text(phost, x+20, y , 20, OPT_CENTER, buf);
	}
	if (j == 0) // First row on each page
	{
		sprintf(buf, "R%d",row);
		Gpu_CoCmd_Text(phost, x , y-18, 20, OPT_CENTER, buf);
	}

    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    if (buttonState)
    {
        // If button state is ON
        App_WrCoCmd_Buffer(phost, BEGIN(FTPOINTS));
        App_WrCoCmd_Buffer(phost, POINT_SIZE(200));//buttonSize * 10));
        App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 128, 0)); // Display green circle
        App_WrCoCmd_Buffer(phost, TAG(tag));
        Gpu_CoCmd_Track(phost, x + 22 + buttonSize / 2, y - 18 + buttonSize / 2, 0, 0, tag); // Enable touch tracking for TAG
        App_WrCoCmd_Buffer(phost, VERTEX2II(x+22, y - 18, 0, 0));
        App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
        Gpu_CoCmd_Text(phost, x+22, y - 18, 20, OPT_CENTER, "ON");


    }
	else
    {
        // If button state is OFF
        App_WrCoCmd_Buffer(phost, BEGIN(FTPOINTS));
        App_WrCoCmd_Buffer(phost, POINT_SIZE(200));
        App_WrCoCmd_Buffer(phost, COLOR_RGB(200, 0, 0)); // Display red circle
        App_WrCoCmd_Buffer(phost, TAG(tag));
        Gpu_CoCmd_Track(phost, x + 22 + buttonSize / 2, y -18 + buttonSize / 2, 0, 0, tag); // Enable touch tracking for TAG
        App_WrCoCmd_Buffer(phost, VERTEX2II(x+22, y - 18, 0, 0));
        App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
        Gpu_CoCmd_Text(phost, x+22, y - 18, 20, OPT_CENTER, "OFF");
    }



    App_WrCoCmd_Buffer(phost, END());
}

void Tray_Screen(Gpu_Hal_Context_t *phost)
{
	char buf[10];
	bool showonce=FALSE;
	uint8_t touch_tag;


	int32_t startRow = 1;
	int32_t startCol = 1;


    CurProf.Tube_No_x = min(CurProf.Tube_No_x, MAX_TUBES_X);
    CurProf.Tube_No_y = min(CurProf.Tube_No_y, MAX_TUBES_Y);

    int32_t totalNumButtons = CurProf.Tube_No_x * CurProf.Tube_No_y;

    const int maxRowsPerPage = 7;
    const int maxColsPerPage = 7;


    int numRows = minimum(minimum(CurProf.Tube_No_y, maxRowsPerPage), MAX_TUBES_Y);
    int numCols = minimum(minimum(CurProf.Tube_No_x, maxColsPerPage), MAX_TUBES_X);

    const int numButtonsPerPage = numRows * numCols;

    int32_t numPages = (totalNumButtons + numButtonsPerPage - 1) / numButtonsPerPage;
    int32_t currentPage = 0;

    int currentRowPage = 0;
    int currentColPage = 0;

    // Calculate button size and gaps once and use them on subsequent pages to mantain the same buttonsize across all pages
    int32_t buttonSize = 20;//min((240 - ((numCols + 1) * 16)) / numCols, (240 - ((numRows + 1) * 16)) / numRows);
    int32_t rowGap = 12;//(240 - (numRows * buttonSize)) / (numRows + 1);
    int32_t colGap = 12;//(240 - (numCols * buttonSize)) / (numCols + 1);

	
	
    // Calculate starting position for buttons
    int32_t startX = 10;//26;//(STARTPOS - ((numCols * (buttonSize + colGap)) - colGap)) / 2;
    int32_t startY = 230;//26;// (STARTPOS - ((numRows * (buttonSize + rowGap)) - rowGap)) / 2;



    while (1)
    {
        touch_tag = GetKeyPressed();

        Gpu_CoCmd_FlashFast(phost, 0);
        Gpu_CoCmd_Dlstart(phost);

        App_WrCoCmd_Buffer(phost, CLEAR_TAG(255));
        App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
        App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
        App_WrCoCmd_Buffer(phost, TAG_MASK(1)); // enable tagbuffer updation

        // Draw a rectangle for the button area
        Gpu_CoCmd_FgColor(phost, 0xE5DCD9);
        Gpu_CoCmd_BgColor(phost, 0xE5DCD9);
        App_WrCoCmd_Buffer(phost, LINE_WIDTH(16));
        App_WrCoCmd_Buffer(phost, BEGIN(RECTS));
        App_WrCoCmd_Buffer(phost, VERTEX2F(0, 0));
        //App_WrCoCmd_Buffer(phost, VERTEX2F(3840, 3840));
        App_WrCoCmd_Buffer(phost, END());

        //Gpu_CoCmd_FgColor(phost, 0x00A2E8);
        //App_WrCoCmd_Buffer(phost, TAG(11));
        //App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
        //Gpu_CoCmd_Button(phost, 248, 90, 62, 26, 21, 0, "Load"); // Tray Load button

        //Gpu_CoCmd_FgColor(phost, 0x00A2E8);
        //App_WrCoCmd_Buffer(phost, TAG(21));
        //App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
        //Gpu_CoCmd_Button(phost, 248, 46, 62, 26, 21, 0, "Save"); // Tray Save button

        Gpu_CoCmd_FgColor(phost, 0x00A2E8);
        App_WrCoCmd_Buffer(phost, TAG(22));
        App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
        Gpu_CoCmd_Button(phost, 248, 0, 62, 26, 21, 0, "Back"); // Tray Home button

        // Draw Left navigation buttons
        Gpu_CoCmd_FgColor(phost, 0x00A2E8);
        App_WrCoCmd_Buffer(phost, TAG(23));
        App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
        Gpu_CoCmd_Button(phost, 248, 160, 25, 25, 24, (touch_tag==23)? OPT_FLAT :0 , "<"); // Previous page button

        // Draw right navigation buttons
        Gpu_CoCmd_FgColor(phost, 0x00A2E8);
        App_WrCoCmd_Buffer(phost, TAG(24));
        App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
        Gpu_CoCmd_Button(phost, 288, 160, 25, 25, 24, (touch_tag==24)? OPT_FLAT :0, ">"); // Next page button

        // Draw up navigation button
        Gpu_CoCmd_FgColor(phost, 0x00A2E8);
        App_WrCoCmd_Buffer(phost, TAG(26));
        App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
        Gpu_CoCmd_Button(phost, 268, 130, 25, 25, 24, (touch_tag==26)? OPT_FLAT :0, "^");

        // Draw down navigation button
        Gpu_CoCmd_FgColor(phost, 0x00A2E8);
        App_WrCoCmd_Buffer(phost, TAG(27));
        App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
        Gpu_CoCmd_Button(phost, 268, 190, 25, 25, 24, (touch_tag==27)? OPT_FLAT :0, "v");

        // Calculate starting index and ending index for current page
        int startIndex = currentPage * numButtonsPerPage;
        // int endIndex = min(startIndex + numButtonsPerPage, totalNumButtons);// (optional to detect the last index)

        int currentPageRows = minimum(CurProf.Tube_No_y, numRows);
        int currentPageCols = minimum(CurProf.Tube_No_x, numCols);

        // Draw buttons for current page
        int x =startX;
        int y =startY;

        int tag = TAG_START + startIndex; // Start tag at 28 (currentPage * numButtonsPerPage)
        for (int i = 0; i < currentPageRows; i++)
        {
            for (int j = 0; j < currentPageCols; j++, tag++)
            {
                int row = startRow + i;
                int col = startCol + j;
                if (col <= CurProf.Tube_No_x && row <= CurProf.Tube_No_y) // Add this line to prevent drawing buttons beyond column 13
                {
                    if (touch_tag == tag)
                    {
                        CurProf.buttonStates[col - 1][row - 1] = !CurProf.buttonStates[col - 1][row - 1];
                        Serial.println("Button at (" + String(row) + ", " + String(col) + ") is now " + (CurProf.buttonStates[col - 1][row - 1] ? "OFF" : "ON"));
                    }
                    toggleButton(phost, tag, x, y, buttonSize, row, col, i, j);
                }
                x += buttonSize + colGap;
            }
            x = startX;
            y -= buttonSize + rowGap;
        }
        displayTextMatrixName(startRow,startCol);

 		if (touch_tag == 22) // Home button
        {
            //break;
            return;
        }

        else if (touch_tag == 24) // Next page button
        {
        	//Dprint(">","\n");


            if (currentColPage < ((CurProf.Tube_No_x / maxColsPerPage) - ((CurProf.Tube_No_x % maxColsPerPage) ? 0 : 1)))
			{
				Dprint("cin");
                currentColPage++;
                startCol = currentColPage * numCols + 1;
            }
			delay(50);
			//Dprint("curcolpage=",currentColPage);
        }

        else if (touch_tag == 23) // Previous page button
        {
        	//Dprint("<","\n");

            if (currentColPage > 0)
            {
                currentColPage--;
                startCol = currentColPage * numCols + 1;
            }
			delay(50);
			//Dprint("curcolpage=",currentColPage);
        }

        else if (touch_tag == 27) // Up button
        {
        	//Dprint("^","\n");


            if (currentRowPage > 0)
            {
                currentRowPage--;
                startRow = currentRowPage * numRows + 1;
            }
			delay(50);
			//Dprint("currowpage=",currentRowPage);
        }

        else if (touch_tag == 26) // Down button
        {
        	//Dprint("v","\n");

            if (currentRowPage < ((CurProf.Tube_No_y / maxRowsPerPage) - ((CurProf.Tube_No_y % maxRowsPerPage) ? 0 : 1))) // Change here
            {
        				Dprint("rin");
                currentRowPage++;
                startRow = currentRowPage * numRows + 1;
            }
			delay(50);
			//Dprint("currowpage=",currentRowPage);
        }

        if (currentPage == numPages - 1)
        { // last page
            currentPageRows = totalNumButtons % numRows;
            currentPageCols = totalNumButtons % numCols;
        }
        else
        {
            currentPageRows = numRows;
            currentPageCols = numCols;
        }

        Disp_End(phost);
		if(touch_tag !=0) WaitKeyRelease();//delay(100);
    }
}