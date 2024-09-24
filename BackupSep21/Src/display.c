/*
 * display.c
 *
 *  Created on: Sep 18, 2024
 *      Author: siemens
 */
#include "display.h"
#include "ssd1306.h"
int 	trCheckUS, trCheckTS, 				trCheckUM, trCheckTM, 						trCheckUH, trCheckTH;
int 	drCheckTY, drCheckUY,				drCheckTM, drCheckUM, drCheckWD,			drCheckTD, drCheckUD;
char 	chUniSeconds='0', chTenSeconds='0',	chUniMinutes='0', 	chTenMinutes='0', 			chUniHours='0', chTenHours='0';
char	chTenYears='0', chUniYears='0',		chTenMonths='0', 	chUniMonths='0',chWD='0',		chTenDays='0', chUniDays='0';
int symbolHorizontalShift=6;
int symbolVerticalShift = 8;


extern RTC_HandleTypeDef hrtc;

void displayTime(){
	char timeString[20];
	switch (chWD){
	case 49: 	timeString[9]= 'M';
				timeString[10]='o';
				break;
	case 50: 	timeString[9]= 'T';
				timeString[10]='u';
				break;
	case 51: 	timeString[9]= 'W';
				timeString[10]='e';
				break;
	case 52: 	timeString[9]= 'T';
				timeString[10]='h';
				break;
	case 53: 	timeString[9]= 'F';
				timeString[10]='r';
				break;
	case 54: 	timeString[9]= 'S';
				timeString[10]='a';
				break;
	case 55: 	timeString[9]= 'S';
				timeString[10]='u';
				break;
	}
	timeString[0]=		chTenHours;
	timeString[1]=		chUniHours;
	timeString[2]=		':';
	timeString[3]=		chTenMinutes;
	timeString[4]=		chUniMinutes;
	timeString[5]=		':';
	timeString[6]=		chTenSeconds;
	timeString[7]=		chUniSeconds;
	timeString[8]= 		' ';
	timeString[11]= 	' ';
	timeString[12]=		chTenDays;
	timeString[13]=	 	chUniDays;
	timeString[14]=	 	'/';
	timeString[15]=	 	chTenMonths;
	timeString[16]=	 	chUniMonths;
	timeString[17]=	 	'/';
	timeString[18]=	 	chTenYears;
	timeString[19]=	 	chUniYears;
	timeString[20]=	 	'\0';


	ssd1306_WriteString(timeString, Font_6x8, White);
}
void checkTime(){
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
