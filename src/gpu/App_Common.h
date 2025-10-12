/*

Copyright (c) Bridgetek Pte Ltd

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

IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.

Author : Bridgetek
Modified by: XentiQ

Revision History:
0.1 - date 2017.03.24 - Initial version
0.2 - date 2022.12.14 - XentiQ version
*/

#ifndef _APP_COMMON_H_
#define _APP_COMMON_H_

#if defined(FT80X_ENABLE)
#define RAM_G_END_ADDR 0x40000 // General purpose graphics RAM 256 kB
#elif defined(FT81X_ENABLE)
#define RAM_G_END_ADDR (0x100000) // General purpose graphics RAM 1024 kB
#else
#error "Should select a GPU chip in Platform.h"
#endif

#include "../Constants.h"

#define SIZE_HOME_START_ICON (460)
#define SIZE_LOGO (6703)

#define START_ICON_ADDR (RAM_G_END_ADDR - SIZE_HOME_START_ICON * 6) //*6 to Reserve space for inflate images.
#define LOGO_ADDR (START_ICON_ADDR - SIZE_LOGO)

#define START_ICON_HANDLE 14
#define LOGO_ICON_HANDLE 15

#define SPECIAL_FUN          250
#define BACK_SPACE           251 // Back space
#define CAPS_LOCK            252  // Caps Lock

#define NUMBER_LOCK          253 // Number Lock
#define SAVE_KEY             249    // Number sAVE FROM kEYPAD
#define CLEAR_KEY            254        // Exit
#define BACK                 254
#define KBBACK               248    // Exit Keyboard
#define NUM_ENTER            247   // Enter Numeric Number

/******************************************************************************/

void App_WrCoCmd_Buffer(Gpu_Hal_Context_t *phost, uint32_t cmd);
void App_WrDl_Buffer(Gpu_Hal_Context_t *phost, uint32_t cmd);
void App_WrCoStr_Buffer(Gpu_Hal_Context_t *phost, const char8_t *s);
void App_Flush_DL_Buffer(Gpu_Hal_Context_t *phost);
void App_Flush_Co_Buffer(Gpu_Hal_Context_t *phost);
void App_Flush_Co_Buffer_nowait(Gpu_Hal_Context_t *phost);
void App_Set_DlBuffer_Index(uint32_t index);
void App_Set_CmdBuffer_Index(uint32_t index);

/******************************************************************************/

uint8_t App_Read_Tag(Gpu_Hal_Context_t *phost);
uint8_t App_Touch_Update(Gpu_Hal_Context_t *phost, uint8_t *currTag, uint16_t *x, uint16_t *y);
void App_Play_Sound(Gpu_Hal_Context_t *phost, uint8_t sound, uint8_t vol, uint8_t midi);
void App_Calibrate_Screen(Gpu_Hal_Context_t *phost);
void App_Show_Logo(Gpu_Hal_Context_t *phost);

/******************************************************************************/

void App_Common_Init(Gpu_Hal_Context_t *phost);
void App_Common_Close(Gpu_Hal_Context_t *phost);

/*************************************************************************************************
 *                      XentiQ Header - 23JUN23
 **************************************************************************************************/
#include "../logic/Profile.h"

void Disp_End(Gpu_Hal_Context_t *phost);

#endif /* _APP_COMMON_H_ */
