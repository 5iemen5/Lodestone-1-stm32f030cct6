/*
 * display.c
 *
 *  Created on: Sep 18, 2024
 *      Author: siemens
 */
#include "display.h"
#include "ssd1306.h"
#include "main.h"
#include "UIdriver.h"
#include <stdio.h>
#include <string.h>

int symbolHorizontalShift=6;
int symbolVerticalShift = 8;
int adjustLimit=7;

extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef sDate;
extern RTC_HandleTypeDef hrtc;
extern struct uiSubPage calibTime;
extern int nextOrPrev;
extern int allowAdjust;

RTC_DateTypeDef dateBuffer={
		0,0,0,0
};
RTC_TimeTypeDef timeBuffer={
		0, 0, 0,
		0, 0, 0, 0, 0,
};

void checkTime(){
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	calibTime.buttonMenu[0].value = sTime.Hours;
	calibTime.buttonMenu[1].value = sTime.Minutes;
	calibTime.buttonMenu[2].value = sTime.Seconds;
	calibTime.buttonMenu[3].value = sDate.WeekDay;
	calibTime.buttonMenu[4].value = sDate.Date;
	calibTime.buttonMenu[5].value = sDate.Month;
	calibTime.buttonMenu[6].value = sDate.Year;
}

void displayTime() {
		char timeString[21];
		char * wd;
	    // Format: YYYY-MM-DD WD HH:MM:SS
		switch (sDate.WeekDay){
		case 1: 	wd="Mo";
					break;
		case 2:  	wd="Tu";
					break;
		case 3:  	wd="We";
					break;
		case 4:  	wd="Th";
					break;
		case 5:  	wd="Fr";
					break;
		case 6:  	wd="Sa";
					break;
		case 7:  	wd="Su";
					break;
		}


	    sprintf(timeString, "%02d:%02d:%02d %s %02d/%02d/%02d",
	    		 // Assuming Year is offset from 2000
				sTime.Hours,
				sTime.Minutes,
				sTime.Seconds,
				wd,
				sDate.Date,
				sDate.Month,
				sDate.Year
);
		ssd1306_WriteString(timeString, Font_6x8, White);
		ssd1306_UpdateScreen();
	}

void adjustTime()
{
	if(allowAdjust==1){
		checkTime();
		timeBuffer=sTime;
		dateBuffer=sDate;

		if(calibTime.currentButton==0)											//IF HOURS (1-23)
		{
			adjustLimit=23;
		}
		if(calibTime.currentButton==1)											//IF MINUTES (1-59)
		{
			adjustLimit=59;
		}
		if(calibTime.currentButton==2)											//IF SECONDS (1-59)
		{
			adjustLimit=59;
		}
		if(calibTime.currentButton==3)											//IF WEEKDAY (1-7)
		{
			adjustLimit=7;
		}
		if(calibTime.currentButton==4)											//IF DAY (1-30)
		{
			adjustLimit=30;
		}
		if(calibTime.currentButton==5)											//IF MONTH (1-12)
		{
			adjustLimit=12;
		}
		if(calibTime.currentButton==6)											//IF YEAR (1-99)
		{
			adjustLimit=99;
		}

	//////////////////////////////////////////////////////////////////////////////////////////////////

		if (nextOrPrev==NEXT)													//INCREMENT
		{
			if(	(int)((calibTime.buttonMenu[calibTime.currentButton].value)+1)	>=	adjustLimit	)
			{}
			else
			{
			calibTime.buttonMenu[calibTime.currentButton].value++;
			}
		}

		else if (nextOrPrev==PREV)												//DECREMENT
		{
			if(	(int)((calibTime.buttonMenu[calibTime.currentButton].value)-1)	<=1	)
			{}
			else
			{
				calibTime.buttonMenu[calibTime.currentButton].value--;
			}
		}

		switch(calibTime.currentButton){													//ASSIGNING THE time/date STRUCTURE
			case 0:																			//VALUES OF SET:
				timeBuffer.Hours=	calibTime.buttonMenu[calibTime.currentButton].value;	//HOURS
				break;
			case 1:
				timeBuffer.Minutes=calibTime.buttonMenu[calibTime.currentButton].value;	//MINUTES
				break;
			case 2:
				timeBuffer.Seconds=calibTime.buttonMenu[calibTime.currentButton].value;	//SECONDS
				break;
			case 3:
				dateBuffer.WeekDay=calibTime.buttonMenu[calibTime.currentButton].value;	//WEEKDAY
				break;
			case 4:
				dateBuffer.Date=calibTime.buttonMenu[calibTime.currentButton].value;		//DAY
				break;
			case 5:
				dateBuffer.Month=calibTime.buttonMenu[calibTime.currentButton].value;		//MONTH
				break;
			case 6:
				dateBuffer.Year=calibTime.buttonMenu[calibTime.currentButton].value;		//YEAR
				break;
		}

		HAL_RTC_SetDate(&hrtc, &dateBuffer, RTC_FORMAT_BIN);
		HAL_RTC_SetTime(&hrtc, &timeBuffer, RTC_FORMAT_BIN);
		checkTime();
		ssd1306_SetCursor(1*symbolHorizontalShift, 1+2*symbolVerticalShift);
		displayTime();
	}
}





/*int 	trCheckUS, trCheckTS, 				trCheckUM, trCheckTM, 						trCheckUH, trCheckTH;
int 	drCheckTY, drCheckUY,				drCheckTM, drCheckUM, drCheckWD,			drCheckTD, drCheckUD;
char 	chUniSeconds='0', chTenSeconds='0',	chUniMinutes='0', 	chTenMinutes='0', 			chUniHours='0', chTenHours='0';
char	chTenYears='0', chUniYears='0',		chTenMonths='0', 	chUniMonths='0',chWD='0',		chTenDays='0', chUniDays='0';*/

/*
void checkTime2(){
	uint32_t a=hrtc.Instance->TR;
	for (int i=4; i<32; i++){ 		//Erasing each bit but 0-3 (units seconds)
		a = a & ~(1 << i);			//Refer RM0360 pg.502/775
	}
	trCheckUS=a;

	a=hrtc.Instance->TR;
	for (int i=7; i<32; i++){ 		//Erasing each bit but 4-6 (Tens seconds)
		a = a & ~(1 << i);
	}
	a=a>>4;
	trCheckTS=a;

	a=hrtc.Instance->TR;
	for (int i=12; i<32; i++){
		a = a & ~(1 << i);
	}
	a=a>>8;
	trCheckUM=a;

	a=hrtc.Instance->TR;
	for (int i=15; i<32; i++){
		a = a & ~(1 << i);
	}
	a=a>>12;
	trCheckTM=a;

	a=hrtc.Instance->TR;
	for (int i=20; i<32; i++){
		a = a & ~(1 << i);
	}
	a=a>>16;
	trCheckUH=a;

	a=hrtc.Instance->TR;
	for (int i=22; i<32; i++){
		a = a & ~(1 << i);
	}
	a=a>>20;
	trCheckTH=a;

	  chUniSeconds=trCheckUS + '0';	//Units and tens to char
	  chTenSeconds=trCheckTS + '0';
	  chUniMinutes = trCheckUM + '0';
	  chTenMinutes = trCheckTM + '0';
	  chUniHours = trCheckUH + '0';
	  chTenHours = trCheckTH + '0';

///////////////// DATE /////////////////

	  a=hrtc.Instance->DR;
	  for (int i=24; i<32; i++){
		  a = a & ~(1 << i);
}
	  a=a>>20;
	  drCheckTY=a;


	a=hrtc.Instance->DR;
	for (int i=20; i<32; i++){
		a = a & ~(1 << i);
}
	a=a>>16;
	drCheckUY=a;

	a=hrtc.Instance->DR;
	for (int i=13; i<32; i++){
		a = a & ~(1 << i);
}
	a=a>>12;
	drCheckTM=a;

	a=hrtc.Instance->DR;
	for (int i=12; i<32; i++){
		a = a & ~(1 << i);
}
	a=a>>8;
	drCheckUM=a;

	a=hrtc.Instance->DR;
	for (int i=6; i<32; i++){
		a = a & ~(1 << i);
}
	a=a>>4;
	drCheckTD=a;

	a=hrtc.Instance->DR;
	for (int i=4; i<32; i++){
		a = a & ~(1 << i);
}
	drCheckUD=a;

	a=hrtc.Instance->DR;
	for (int i=16; i<32; i++){
		a = a & ~(1 << i);
}
	a=a>>13;
	drCheckWD=a;

	  chWD=drCheckWD + '0';
	  chTenDays=drCheckTD + '0';
	  chUniDays=drCheckUD + '0';
	  chTenMonths=drCheckTM + '0';
	  chUniMonths=drCheckUM + '0';
	  chTenYears=drCheckTY + '0';
	  chUniYears=drCheckUY + '0';
	}
*/
