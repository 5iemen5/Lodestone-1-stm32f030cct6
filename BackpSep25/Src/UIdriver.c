/*
 * UIdriver.c
 *
 *  Created on: Sep 18, 2024
 *      Author: siemens
 */
#include "UIdriver.h"
#include "ssd1306.h"
#include "main.h"

extern int symbolHorizontalShift;
extern int symbolVerticalShift;
extern TIM_HandleTypeDef htim7;

int allowNextPage=1, nextOrPrev=1, menuTriggered;

struct uiPage page1, page2, page3;

struct uiPage page1 = {0,0,&page2,&page3, 	"1. Time and Date\0",
						{
						{6, 18},
						{6, 27},
						{6, 36}
						},
						{
						{"Stopwatch\0",	0, 1, 2, 	STATE_RESET, UNSELECTED, SETORRESET,&(page1.buttonMenu[1]), &(page1.buttonMenu[2])	},
						{"Timer\0",	1, 2, 0, 	STATE_RESET, UNSELECTED, SETORRESET,&(page1.buttonMenu[2]), &(page1.buttonMenu[0])	},
						{"Alarm in 5 min\0",	2, 0, 1,	 STATE_RESET, UNSELECTED, SETORRESET,&(page1.buttonMenu[0]), &(page1.buttonMenu[1])	}
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
						{"Light up\0",	0, 1, 2,	 STATE_RESET, UNSELECTED, SETORRESET,&(page1.buttonMenu[1]), &(page1.buttonMenu[2])	},
						{"Open up\0",	1, 2, 0,	 STATE_RESET, UNSELECTED, SETORRESET,&(page1.buttonMenu[2]), &(page1.buttonMenu[0])	},
						{"MEOW!\0",	2, 0, 1,	 STATE_RESET, UNSELECTED, SETORRESET,&(page1.buttonMenu[0]), &(page1.buttonMenu[1])	}
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
						{"Adjust time\0",0, 1, 2,	STATE_RESET, UNSELECTED, SETORRESET,&(page1.buttonMenu[1]), &(page1.buttonMenu[2])	},
						{"Recieve UART\0",			1, 2, 0, 	STATE_RESET, UNSELECTED, SETORRESET,&(page1.buttonMenu[2]), &(page1.buttonMenu[0])	},
						{"Bitmap\0",			2, 0, 1,	STATE_RESET, UNSELECTED, SETORRESET,&(page1.buttonMenu[0]), &(page1.buttonMenu[1])	}
						},
						0,
						};


struct uiPage * currentPage= &page1;

void nextPage(){
if(allowNextPage==1){
	ssd1306_FillRectangle(0, 0, 127, 63, 0x0);//wipeScreen();

	if (nextOrPrev==NEXT)
		currentPage = currentPage->nextoPage;
	else if (nextOrPrev==PREV)
		currentPage = currentPage->prevPage;
	else {}

	//ssd1306_Line(0, 9, 127, 9, 0x01);

	ssd1306_SetCursor(1*symbolHorizontalShift, 1+0*symbolVerticalShift);		//Paste name of page
	ssd1306_WriteString(currentPage->nameOfPage, Font_6x8, White);

	//////////////////////////////////////	BUTTONS ///////////////////////////////////////////////

	ssd1306_SetCursor(1*symbolHorizontalShift, 2+2*symbolVerticalShift);		//Place corresponding buttons
	ssd1306_WriteString(currentPage->buttonMenu[0].title, Font_6x8, White);		//1.
	if (	currentPage->buttonMenu[0].state == STATE_SET){				//IF SET - MARK AS SET
		ssd1306_FillRectangle(	currentPage->cursorAxis[0][0]+115,
								currentPage->cursorAxis[0][1]+2,
								currentPage->cursorAxis[0][0]+117,
								currentPage->cursorAxis[0][1]+4,
								0x01);}
	else {
		ssd1306_FillRectangle(	currentPage->cursorAxis[0][0]+115,		//IF RESET - MARK AS RESET
								currentPage->cursorAxis[0][1]+2,
								currentPage->cursorAxis[0][0]+117,
								currentPage->cursorAxis[0][1]+4,
								0x00);}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	ssd1306_SetCursor(1*symbolHorizontalShift, 3+3*symbolVerticalShift);		//2.
	ssd1306_WriteString(currentPage->buttonMenu[1].title, Font_6x8, White);
	if (	currentPage->buttonMenu[1].state == STATE_SET){				//IF SET - MARK AS SET
		ssd1306_FillRectangle(	currentPage->cursorAxis[1][0]+115,
								currentPage->cursorAxis[1][1]+2,
								currentPage->cursorAxis[1][0]+117,
								currentPage->cursorAxis[1][1]+4,
								0x01);}
	else {
		ssd1306_FillRectangle(	currentPage->cursorAxis[1][0]+115,		//IF RESET - MARK AS RESET
								currentPage->cursorAxis[1][1]+2,
								currentPage->cursorAxis[1][0]+117,
								currentPage->cursorAxis[1][1]+4,
								0x00);}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	ssd1306_SetCursor(1*symbolHorizontalShift, 4+4*symbolVerticalShift);		//3.
	ssd1306_WriteString(currentPage->buttonMenu[2].title, Font_6x8, White);
	if (	currentPage->buttonMenu[2].state == STATE_SET){				//IF SET - MARK AS SET
		ssd1306_FillRectangle(	currentPage->cursorAxis[2][0]+115,
								currentPage->cursorAxis[2][1]+2,
								currentPage->cursorAxis[2][0]+117,
								currentPage->cursorAxis[2][1]+4,
								0x01);}
	else {
		ssd1306_FillRectangle(	currentPage->cursorAxis[2][0]+115,		//IF RESET - MARK AS RESET
								currentPage->cursorAxis[2][1]+2,
								currentPage->cursorAxis[2][0]+117,
								currentPage->cursorAxis[2][1]+4,
								0x00);}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	ssd1306_UpdateScreen();}
else {}
}

void moveCursor(){
if (currentPage->selected==1){
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

void selectPage(){
	ssd1306_DrawRectangle(0, 0, 127, 9, 0x01);

	ssd1306_FillRectangle(0, 10, 5, 54, 0x00);				//CLEAR BUTTON MARKERS

	allowNextPage=0;

	page1.selected=0;
	page2.selected=0;
	page3.selected=0;

	currentPage->selected=1;
	currentPage->buttonMenu[0].selected=1;
	currentPage->currentButton=0;

	markButton();//MARK THE BUTTON

	ssd1306_UpdateScreen();
}

void unselectPage(){

	ssd1306_DrawRectangle(0, 0, 127, 9, 0x00);				//CLEAR SELECT RECTANGLE
	ssd1306_FillRectangle(0, 9, 5, 54, 0x00);				//CLEAR BUTTON MARKERS
	//ssd1306_Line(0, 9, 127, 9, 0x01);						//GET BACK TITLE UNDERLINE

	currentPage->currentButton=0;
	currentPage->buttonMenu[0].selected=0;
	currentPage->buttonMenu[1].selected=0;
	currentPage->buttonMenu[2].selected=0;
	currentPage->selected=0;
	allowNextPage=1;

	page1.selected=0;
	page2.selected=0;
	page3.selected=0;

	ssd1306_UpdateScreen();
}
void markButton(){
	ssd1306_DrawRectangle(	currentPage->cursorAxis[currentPage->currentButton][0]-5,
							currentPage->cursorAxis[currentPage->currentButton][1]+2,
							currentPage->cursorAxis[currentPage->currentButton][0]-3,
							currentPage->cursorAxis[currentPage->currentButton][1]+4,
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

		}
		///////////////////////// SUBMENU /////////////////////////////////
		else if ((	currentPage->buttonMenu[currentPage->currentButton].selected==1)&&			//CHECKIN IF BUTTON IS SELECTED
				(currentPage->buttonMenu[currentPage->currentButton].buttonType == SUBMENU))
		{}





	}
}

