/*
 * UIdriver.c
 *
 *  Created on: Sep 18, 2024
 *      Author: siemens
 */
#include "UIdriver.h"
#include "ssd1306.h"
#include "main.h"
#include "eventRoutine.h"

extern int symbolHorizontalShift;
extern int symbolVerticalShift;
extern TIM_HandleTypeDef htim7;

int allowNextPage=1, allowNextSubPage=0, nextOrPrev=1, menuTriggered,canChooseButton=0;

struct uiPage page1, page2, page3;

struct uiPage page1 = {0,0,&page2,&page3, 	"1. Time and Date\0",
						{
						{6, 18},
						{6, 27},
						{6, 36}
						},
						{
						{"Stopwatch\0",	0,			0, 1, 2, 	STATE_RESET, UNSELECTED, SETORRESET,&(page1.buttonMenu[1]), &(page1.buttonMenu[2])	},
						{"Timer\0",0,				1, 2, 0, 	STATE_RESET, UNSELECTED, SETORRESET,&(page1.buttonMenu[2]), &(page1.buttonMenu[0])	},
						{"Alarm in 5 min\0",0,		2, 0, 1,	 STATE_RESET, UNSELECTED, SETORRESET,&(page1.buttonMenu[0]), &(page1.buttonMenu[1])	}
						},
						0,
						};


struct uiPage page2 = {1,0,&page3,&page1, 	"2. Transmit frames\0",
						{
						{6, 18},
						{6, 27},
						{6, 36}
						},
						{
						{"Test 32 bits\0",0,	0, 1, 2,	 STATE_RESET, UNSELECTED, SINGLEPRESS,&(page2.buttonMenu[1]), &(page2.buttonMenu[2])	},
						{"Test 64 bits\0",	0,	1, 2, 0,	 STATE_RESET, UNSELECTED, SINGLEPRESS,&(page2.buttonMenu[2]), &(page2.buttonMenu[0])	},
						{"Test 128 bits\0",0,	2, 0, 1,	 STATE_RESET, UNSELECTED, SINGLEPRESS,&(page2.buttonMenu[0]), &(page2.buttonMenu[1])	}
						},
						0,
						};

struct uiPage page3 = {2,0,&page1,&page2, 	"3. Settings\0",
						{
						{6, 18},
						{6, 27},
						{6, 36}
						},
						{
						{"Adjust time\0",0,		0, 1, 2,	STATE_RESET, UNSELECTED, SUBMENU,&(page3.buttonMenu[1]), &(page3.buttonMenu[2])	},
						{"Receive UART\0",0,	1, 2, 0, 	STATE_RESET, UNSELECTED, SETORRESET,&(page3.buttonMenu[2]), &(page3.buttonMenu[0])	},
						{"Lock screen\0",0,		2, 0, 1,	STATE_RESET, UNSELECTED, SETORRESET,&(page3.buttonMenu[0]), &(page3.buttonMenu[1])	}
						},
						0,
						};

///////////////////////////////////////// TIME ADJUST SUBPAGE /////////////////////////////

struct uiSubPage  calibTime = {0,0,&calibTime,&calibTime, 	"Adjust time\0",
						{
						{7, 18},			//SECONDS
						{25, 18},			//MINUTES
						{43, 18},			//HOURS
						{61, 18},			//WD
						{79, 18},			//DAYS
						{97, 18},			//MONTH
						{115, 18}			//YEAR
						},
						{
						{"H\0",	0,		0, 1, 6,	STATE_RESET, UNSELECTED, ROLLER,&(calibTime.buttonMenu[1]), &(calibTime.buttonMenu[6])	},
						{"M\0",	0,		1, 2, 0, 	STATE_RESET, UNSELECTED, ROLLER,&(calibTime.buttonMenu[2]), &(calibTime.buttonMenu[0])	},
						{"S\0",	0,		2, 3, 1,	STATE_RESET, UNSELECTED, ROLLER,&(calibTime.buttonMenu[3]), &(calibTime.buttonMenu[1])	},
						{"w\0",	0,		3, 4, 2,	STATE_RESET, UNSELECTED, ROLLER,&(calibTime.buttonMenu[4]), &(calibTime.buttonMenu[2])	},
						{"D\0",	0,		4, 5, 3, 	STATE_RESET, UNSELECTED, ROLLER,&(calibTime.buttonMenu[5]), &(calibTime.buttonMenu[3])	},
						{"M\0",	0,		5, 6, 4,	STATE_RESET, UNSELECTED, ROLLER,&(calibTime.buttonMenu[6]), &(calibTime.buttonMenu[4])	},
						{"Y\0",	0,		6, 0, 5,	STATE_RESET, UNSELECTED, ROLLER,&(calibTime.buttonMenu[0]), &(calibTime.buttonMenu[5])	}
						},
						0,
						};

////////////////////////////////////////////////////////////////////////////////////////////

struct uiPage * currentPage= &page1;
struct uiSubPage * currentSubPage = &calibTime;

void nextPage(){
if(allowNextPage==1){
	ssd1306_FillRectangle(0, 0, 127, 63, 0x0);//wipeScreen();

	if (nextOrPrev==NEXT)
		currentPage = currentPage->nextoPage;
	else if (nextOrPrev==PREV)
		currentPage = currentPage->prevPage;
	else if (nextOrPrev==CURRENT){currentPage = currentPage;}

	currentSubPage->selected=0;

	ssd1306_SetCursor(1*symbolHorizontalShift, 1+0*symbolVerticalShift);		//Paste name of page
	ssd1306_WriteString(currentPage->nameOfPage, Font_6x8, White);

	if(currentSubPage->selected==0)
			//(	currentPage->numberOfPage!=2)&&(	currentPage->buttonMenu[currentPage->currentButton].selected!=1)
			{	//IF THIS IS NOT A SUBPAGE
		//////////////////////////////////////	BUTTONS ///////////////////////////////////////////////

		ssd1306_SetCursor(1*symbolHorizontalShift, 2+2*symbolVerticalShift);		//Place corresponding buttons
		ssd1306_WriteString(currentPage->buttonMenu[0].title, Font_6x8, White);		//1.
		if ((currentPage->buttonMenu[0].buttonType==SETORRESET)&&(currentPage->buttonMenu[0].state == STATE_SET)){				//IF SET - MARK AS SET
			ssd1306_FillRectangle(	currentPage->cursorAxis[0][0]+115,
									currentPage->cursorAxis[0][1]+2,
									currentPage->cursorAxis[0][0]+117,
									currentPage->cursorAxis[0][1]+4,
									0x01);}
		else {
			ssd1306_FillRectangle(	currentPage->cursorAxis[0][0]+115,				//IF RESET - MARK AS RESET
									currentPage->cursorAxis[0][1]+2,
									currentPage->cursorAxis[0][0]+117,
									currentPage->cursorAxis[0][1]+4,
									0x00);}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////

		ssd1306_SetCursor(1*symbolHorizontalShift, 3+3*symbolVerticalShift);		//2.
		ssd1306_WriteString(currentPage->buttonMenu[1].title, Font_6x8, White);
		if ((currentPage->buttonMenu[0].buttonType==SETORRESET)&&(currentPage->buttonMenu[1].state == STATE_SET)){				//IF SET - MARK AS SET
			ssd1306_FillRectangle(	currentPage->cursorAxis[1][0]+115,
									currentPage->cursorAxis[1][1]+2,
									currentPage->cursorAxis[1][0]+117,
									currentPage->cursorAxis[1][1]+4,
									0x01);}
		else {
			ssd1306_FillRectangle(	currentPage->cursorAxis[1][0]+115,				//IF RESET - MARK AS RESET
									currentPage->cursorAxis[1][1]+2,
									currentPage->cursorAxis[1][0]+117,
									currentPage->cursorAxis[1][1]+4,
									0x00);}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////

		ssd1306_SetCursor(1*symbolHorizontalShift, 4+4*symbolVerticalShift);		//3.
		ssd1306_WriteString(currentPage->buttonMenu[2].title, Font_6x8, White);
		if ((currentPage->buttonMenu[0].buttonType==SETORRESET)&&(currentPage->buttonMenu[2].state == STATE_SET)){				//IF SET - MARK AS SET
			ssd1306_FillRectangle(	currentPage->cursorAxis[2][0]+115,
									currentPage->cursorAxis[2][1]+2,
									currentPage->cursorAxis[2][0]+117,
									currentPage->cursorAxis[2][1]+4,
									0x01);}
		else {
			ssd1306_FillRectangle(	currentPage->cursorAxis[2][0]+115,				//IF RESET - MARK AS RESET
									currentPage->cursorAxis[2][1]+2,
									currentPage->cursorAxis[2][0]+117,
									currentPage->cursorAxis[2][1]+4,
									0x00);}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////
}
	else {											//THE PAGE IS A SUBPAGE ADJ TIME

	}
	ssd1306_UpdateScreen();}
else {}
}

void nextSubPage(){
	if(allowNextSubPage==1){
		ssd1306_FillRectangle(0, 0, 127, 63, 0x0);//wipeScreen();

			checkTime();
			if (nextOrPrev==NEXT)
				currentSubPage = currentSubPage->nextoPage;
			else if (nextOrPrev==PREV)
				currentSubPage = currentSubPage->prevPage;
			else if (nextOrPrev==CURRENT){currentSubPage = currentSubPage;}

			ssd1306_SetCursor(1*symbolHorizontalShift, 1+0*symbolVerticalShift);

			ssd1306_WriteString(currentSubPage->nameOfPage, Font_6x8, White);		//Paste name of page

			ssd1306_SetCursor(1*symbolHorizontalShift, 1+2*symbolVerticalShift);
			displayTime();

			ssd1306_UpdateScreen();
	}
}

void moveCursor(){
if (currentPage->selected==1&&currentSubPage->selected==0){
		ssd1306_FillRectangle(0, 10, 5, 54, 0x00);				//CLEAR BUTTON MARKERS

		currentPage->buttonMenu[0].selected=0;
		currentPage->buttonMenu[1].selected=0;
		currentPage->buttonMenu[2].selected=0;


		if 			(nextOrPrev==PREV)
		{currentPage->currentButton = currentPage->buttonMenu[currentPage->currentButton].prevObject;
		currentPage->buttonMenu[currentPage->currentButton].selected=1;}

		else if 	(nextOrPrev==NEXT)
		{currentPage->currentButton = currentPage->buttonMenu[currentPage->currentButton].nextObject;
		currentPage->buttonMenu[currentPage->currentButton].selected=1;}

		markButton();					//MARK THE BUTTON

		ssd1306_UpdateScreen();
	}
}

void moveSPCursor(){
if ((currentSubPage->selected==1)&&(allowNextSubPage==0)){
		ssd1306_Line(			currentSubPage->cursorAxis[currentPage->currentButton][0]-1, //CLEAR ALL SubPage CURSORS
								currentSubPage->cursorAxis[currentSubPage->currentButton][1]+9,
								currentSubPage->cursorAxis[currentSubPage->currentButton][0]+127,
								currentSubPage->cursorAxis[currentSubPage->currentButton][1]+9,
								0x00);

		if 	(nextOrPrev==PREV)
		{
			currentSubPage->buttonMenu[currentSubPage->currentButton].selected=0;
			currentSubPage->currentButton = currentSubPage->buttonMenu[currentSubPage->currentButton].prevObject;
			currentSubPage->buttonMenu[currentSubPage->currentButton].selected=1;
		}

		if 	(nextOrPrev==NEXT)
		{
			currentSubPage->buttonMenu[currentSubPage->currentButton].selected=0;
			currentSubPage->currentButton = currentSubPage->buttonMenu[currentSubPage->currentButton].nextObject;
			currentSubPage->buttonMenu[currentSubPage->currentButton].selected=1;
		}

		markSubPageButton();																	//MARK THE SP BUTTON

		ssd1306_UpdateScreen();
	}
}

void selectPage(){
	if(currentSubPage->selected==0){
		ssd1306_DrawRectangle(0, 0, 127, 9, 0x01);

		ssd1306_FillRectangle(0, 10, 5, 54, 0x00);				//CLEAR BUTTON MARKERS
		if ((currentPage->buttonMenu[0].selected==0)&&(currentPage->selected==0)&&(currentSubPage->selected==0))
			{
			allowNextPage=0;

			page1.selected=0;
			page2.selected=0;
			page3.selected=0;
			calibTime.selected=0;

			currentPage->selected=1;
			currentPage->buttonMenu[0].selected=1;
			currentPage->buttonMenu[1].selected=0;
			currentPage->buttonMenu[2].selected=0;
			currentPage->currentButton=0;

			markButton();//MARK THE BUTTON

			if (currentPage->numberOfPage==0){
				 checkTime();					//DISPLAY TIME AT THE BOTTOM OF THE SCREEN
				 ssd1306_SetCursor(0, 55);
				 displayTime();
			}

			ssd1306_UpdateScreen();
			for(int i=0; i<350000; i++){};	//BUTTON DEBOUNCE DELAY
		}
	}
}

void selectSubPage(){
	if ((currentPage->selected==0)){
			ssd1306_DrawRectangle(0, 0, 127, 9, 0x01);

			allowNextSubPage=0;

			page1.selected=0;
			page2.selected=0;
			page3.selected=0;

			currentSubPage->selected=1;
			currentSubPage->buttonMenu[0].selected=1;
			currentSubPage->buttonMenu[1].selected=0;
			currentSubPage->buttonMenu[2].selected=0;
			currentSubPage->buttonMenu[3].selected=0;
			currentSubPage->buttonMenu[4].selected=0;
			currentSubPage->buttonMenu[5].selected=0;
			currentSubPage->buttonMenu[6].selected=0;
			currentSubPage->currentButton=0;

			markSubPageButton();//MARK THE BUTTON

			ssd1306_UpdateScreen();
			for(int i=0; i<350000; i++){};	//BUTTON DEBOUNCE DELAY
		}
}

void unselectPage(){
	if (currentSubPage->selected!=1){							//IF WE'RE NOT BUSY WITH A SUBPAGE
		ssd1306_DrawRectangle(0, 0, 127, 9, 0x00);				//CLEAR SELECT RECTANGLE
		ssd1306_FillRectangle(0, 9, 5, 54, 0x00);				//CLEAR BUTTON MARKERS
		//ssd1306_Line(0, 9, 127, 9, 0x01);						//GET BACK TITLE UNDERLINE

		currentPage->currentButton=0;
		currentPage->buttonMenu[0].selected=0;
		currentPage->buttonMenu[1].selected=0;
		currentPage->buttonMenu[2].selected=0;

		currentPage->selected=0;
		allowNextPage=1;
		//allowNextSubPage=0;

		page1.selected=0;
		page2.selected=0;
		page3.selected=0;
		calibTime.selected=0;
		ssd1306_UpdateScreen();
	}
}

void unselectSubPage(){

	ssd1306_DrawRectangle(0, 0, 127, 9, 0x00);				//CLEAR SELECT RECTANGLE

	currentSubPage->currentButton=0;
	currentSubPage->buttonMenu[0].selected=0;
	currentSubPage->buttonMenu[1].selected=0;
	currentSubPage->buttonMenu[2].selected=0;
	currentSubPage->buttonMenu[3].selected=0;
	currentSubPage->buttonMenu[4].selected=0;
	currentSubPage->buttonMenu[5].selected=0;
	currentSubPage->buttonMenu[6].selected=0;

	currentSubPage->selected=0;
	//allowNextSubPage=1;
	nextOrPrev=CURRENT;
	nextPage();
	ssd1306_UpdateScreen();
}

void markButton(){
	ssd1306_DrawRectangle(	currentPage->cursorAxis[currentPage->currentButton][0]-5,
							currentPage->cursorAxis[currentPage->currentButton][1]+2,
							currentPage->cursorAxis[currentPage->currentButton][0]-3,
							currentPage->cursorAxis[currentPage->currentButton][1]+4,
							0x01);
}

void markSubPageButton(){
	ssd1306_Line(			currentSubPage->cursorAxis[currentSubPage->currentButton][0]-1,
							currentSubPage->cursorAxis[currentSubPage->currentButton][1]+9,
							currentSubPage->cursorAxis[currentSubPage->currentButton][0]+7,
							currentSubPage->cursorAxis[currentSubPage->currentButton][1]+9,
							0x01);
}

void markSet(){
	ssd1306_FillRectangle(	currentPage->cursorAxis[currentPage->currentButton][0]+115,
							currentPage->cursorAxis[currentPage->currentButton][1]+2,
							currentPage->cursorAxis[currentPage->currentButton][0]+117,
							currentPage->cursorAxis[currentPage->currentButton][1]+4,
							0x01);
}

void unmarkSet(){
	ssd1306_FillRectangle(	currentPage->cursorAxis[currentPage->currentButton][0]+115,
							currentPage->cursorAxis[currentPage->currentButton][1]+2,
							currentPage->cursorAxis[currentPage->currentButton][0]+117,
							currentPage->cursorAxis[currentPage->currentButton][1]+4,
							0x00);
}

void markSetSPButton(){
	ssd1306_DrawRectangle(	currentPage->cursorAxis[currentSubPage->currentButton][0]-1,
							currentPage->cursorAxis[currentPage->currentButton][1]-1,
							currentPage->cursorAxis[currentPage->currentButton][0]+7,
							currentPage->cursorAxis[currentPage->currentButton][1]+9,
							0x01);
	ssd1306_UpdateScreen();
}

void pressButton(){
	if (currentPage->selected==1){
		////////////////////////// SETORRESET  /////////////////////////
		if (	(currentPage->buttonMenu[currentPage->currentButton].selected==1)&&				//CHECKIN IF BUTTON IS SELECTED
				(currentPage->buttonMenu[currentPage->currentButton].buttonType == SETORRESET))	//CHECKING BUTTON TYPE
		{
			if (	currentPage->buttonMenu[currentPage->currentButton].state == STATE_RESET){		//IF RESET - SET
					currentPage->buttonMenu[currentPage->currentButton].state = STATE_SET;
					markSet();
					ssd1306_UpdateScreen();}

			else {	currentPage->buttonMenu[currentPage->currentButton].state = STATE_RESET;
					unmarkSet();
					ssd1306_UpdateScreen();}		//IF SET - RESET
		}
		////////////////////////// SINGLEPRESS /////////////////////////
		else if ((	currentPage->buttonMenu[currentPage->currentButton].selected==1)&&			//CHECKIN IF BUTTON IS SELECTED
					(currentPage->buttonMenu[currentPage->currentButton].buttonType == SINGLEPRESS)){
					event();
		}
		///////////////////////// SUBMENU /////////////////////////////////
		else if ((	currentPage->buttonMenu[currentPage->currentButton].selected==1)&&			//CHECKIN IF BUTTON IS SELECTED
				(currentPage->buttonMenu[currentPage->currentButton].buttonType == SUBMENU))	//AND IF IT'S SUBMENU
		{
			currentSubPage->selected=0;
			//allowNextSubPage=0;
			event();
			currentPage->buttonMenu[currentPage->currentButton].selected=1;
			currentPage->selected=1;
		}
	}
}

void pageCalibTime(){
	if (currentPage->numberOfPage==3){

	}
}
