#include "../gpu/App_Common.h"
#include "../gpu/Platform.h"
#include "../Constants.h"

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