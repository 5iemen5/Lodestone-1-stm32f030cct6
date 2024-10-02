/*
 * eventRoutine.c
 *
 *  Created on: Sep 25, 2024
 *      Author: siemens
 */
#include "main.h"
#include "UIdriver.h"
#include "display.h"

extern int nextOrPrev;
extern int menuTriggered;
extern int allowNextPage;
extern int allowNextSubPage;
extern int canChooseButton;
extern int nextOrPrev;
extern struct uiPage * currentPage;
extern struct uiSubPage * currentSubPage;
extern struct uiPage page1, page2, page3;
extern struct uiSubPage calibTime;
extern TIM_HandleTypeDef htim14;

int 	allowCalibTime=0,
		allowSelectCalibTime=0,
		allowCursorCalibTime=0,
		allowSelectSPButton=0,
		allowMove=1,
		allowAdjust=0;

char * buf32="01010101010101010101010101010101";
char * buf64="1001100110011001100110011001100110011001100110011001100110011001";
char * buf128="10000001100000011000000110000001100000011000000110000001100000011000000110000001100000011000000110000001100000011000000110000001";
char * frameToSend;

void transmitFrame();

void event(){
	switch (currentPage->numberOfPage){
	case 0:{							//PAGE1

		 checkTime();					//DISPLAY TIME AT THE BOTTOM OF THE SCREEN
		 ssd1306_SetCursor(0, 55);
		 displayTime();

	break;}

////////////////////////////////////////////////////////////////////////////////////////
if (currentPage->selected==1){
	case 1:{							//PAGE2
				if (currentPage->currentButton==0){			//TEST 32
					frameToSend=buf32;
					transmitFrame();
				}
		else 	if(currentPage->currentButton==1){			//TEST 64
					frameToSend=buf64;
					transmitFrame();
		}
		else 	if(currentPage->currentButton==2){			//TEST 128
					frameToSend=buf128;
					transmitFrame();
		}



	break;}
}
////////////////////////////////////////////////////////////////////////////////////////

	case 2:{							//PAGE3

/*		if(canChooseButton==1){											//ADJUST TIME SECTIONS
			switch(calibTime.currentButton){
			case 0:
				markSetSPButton();
				adjustTime();
				break;
			case 1:
				markSetSPButton();
				adjustTime();
				break;
			case 2:
				markSetSPButton();
				adjustTime();
				break;
			case 3:
				markSetSPButton();
				adjustTime();
				break;
			case 4:
				markSetSPButton();
				adjustTime();
				break;
			case 5:
				markSetSPButton();
				adjustTime();
				break;
			case 6:
				markSetSPButton();
				adjustTime();
				break;
			}
		}*/

/*		if(allowAdjust==1)				//ADJUST FUNCTION
		{
			allowSelectSPButton=0;
			//adjustTime();
		}*/

		if(allowSelectSPButton==1)		//MARK THE BUTTON TO ADJUST
		{
			markSetSPButton();
			allowMove=0;
			allowCursorCalibTime=0;
			allowAdjust=1;
		}

		if(allowCursorCalibTime==1)
		{								//
			nextOrPrev=CURRENT;
			allowMove=1;
			moveSPCursor();				//CURSOR APPEARS AND CAN BE MOVED
			allowSelectSPButton=1;
			allowSelectCalibTime=0;
		}

		if((allowSelectCalibTime==1))
		{								//SELECT THE SUBPAGE
			allowCalibTime=0;
			nextSubPage();
			allowNextSubPage=0;
			selectSubPage();
			if(currentSubPage->selected==1)
			{
				allowCursorCalibTime=1;
			}
		}

		if (
			(allowCalibTime==1))			//ENTER THE SUBPAGE
		{
			unselectPage();
			allowNextPage=0;
			allowNextSubPage=1;
			nextSubPage();
			allowSelectCalibTime=1;

		}
		else {

		}
	break;}

////////////////////////////////////////////////////////////////////////////////////////

	case 3:{
		if((currentPage->selected=0)&&(currentSubPage->selected==1)){

		}
	break;}
////////////////////////////////////////////////////////////////////////////////////////
	default:{}
	}
}

void transmitFrame(){

		for (int i=0; i<256; i++){
			htim14.Instance->CNT=0;
			HAL_TIM_Base_Start(&htim14);
			HAL_TIM_Base_Stop(&htim14);

			if (frameToSend[i]=='1'){
				HAL_GPIO_WritePin(GPIOA, SYN115_DATA_Pin, GPIO_PIN_SET);
				for(int i=0; i<150; i++){};}
			else{
				HAL_GPIO_WritePin(GPIOA, SYN115_DATA_Pin, GPIO_PIN_RESET);
				for(int i=0; i<150; i++){};}
		}

}
