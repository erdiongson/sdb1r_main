#include "../gpu/App_Common.h"
#include "../gpu/Platform.h"
#include "../Constants.h"

void DisplayProfileMenu(uint8_t keypressed, uint8_t curprofnum, Profile &profile) {
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
	
	Gpu_CoCmd_Text(phost, 160, 54, 27, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, (const char *)profile.profileName);
	App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 127));    

    // Print profileId
    sprintf(buf, "Profile ID:    %d", curprofnum+1);
    Gpu_CoCmd_Text(phost, 8, 65, 21, 0, buf);

    // // Print profileName
    // sprintf(buffer, "Profile Name: %s", curprof.profileName);
    // Gpu_CoCmd_Text(phost, 84, 56, 27, 0, buffer);

    // Print Tube_No_x
    sprintf(buf, "Columns:     %d", profile.Tube_No_x);
    Gpu_CoCmd_Text(phost, 8, 80, 21, 0, buf);

    // Print Tube_No_y
    sprintf(buf, "Rows:         %d", profile.Tube_No_y);
    Gpu_CoCmd_Text(phost, 8, 95, 21, 0, buf);

    // Print pitch_x
    dtostrf(profile.pitch_x, 4, 1, buf);
    Gpu_CoCmd_Text(phost, 8, 110, 21, 0, "Pitch(Col):");
    Gpu_CoCmd_Text(phost, 82, 110, 21, 0, buf);

    // Print pitch_y
    dtostrf(profile.pitch_y, 4, 1, buf);
    Gpu_CoCmd_Text(phost, 8, 125, 21, 0, "Pitch(Ros):");
    Gpu_CoCmd_Text(phost, 83, 125, 21, 0, buf);

    // Print trayOriginX
    dtostrf(profile.trayOriginX, 4, 1, buf);
    Gpu_CoCmd_Text(phost, 8, 140, 21, 0, "OriginX:");
    Gpu_CoCmd_Text(phost, 80, 140, 21, 0, buf);

    // Print trayOriginY
    dtostrf(profile.trayOriginY, 4, 1, buf);
    Gpu_CoCmd_Text(phost, 8, 155, 21, 0, "OriginY:");
    Gpu_CoCmd_Text(phost, 80, 155, 21, 0, buf);

    // Print Cycles
    sprintf(buf, "Cycles:       %d" , profile.Cycles);
    Gpu_CoCmd_Text(phost, 8, 170, 21, 0, buf);

    // Print vibrationEnabled
    //sprintf(buf, "Vibration:    %s", curprof.vibrationEnabled ? "True" : "False");
    //20240903 - erdiongson: Change in Vibration Level
    sprintf(buf, "Vibration Level:    %d", profile.vibrationEnabled);
    Gpu_CoCmd_Text(phost, 8, 185, 21, 0, buf);

    // Print passwordEnabled
    sprintf(buf, "Password: %s" , profile.passwordEnabled ? "True" : "False");
    Gpu_CoCmd_Text(phost, 150, 65, 21, 0, buf);

    // Print vibrationDuration
    //20241001 - erdiongson: Change in Vibration Duration
    sprintf(buf, "Vibration Time: %d", profile.vibrationDuration);
    Gpu_CoCmd_Text(phost, 150, 80, 21, 0, buf);
	
	Disp_End(phost);

}