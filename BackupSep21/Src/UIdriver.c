/*
 * UIdriver.c
 *
 *  Created on: Sep 18, 2024
 *      Author: siemens
 */
#include "UIdriver.h"
#include "ssd1306.h"

extern int symbolHorizontalShift;
extern int symbolVerticalShift;

int allowNextPage=1, nextOrPrev=1;

struct clickableObject{
	char title[22];
	unsigned int numberOfObject, nextObject, prevObject;
	unsigned int state, selected;
	struct clickableObject * pNextObject;
	struct clickableObject * pPrevObject;
};

struct uiPage{
	unsigned int numberOfPage, selected;
	struct uiPage * nextoPage;
	struct uiPage * prevPage;
	char nameOfPage[22];
	int cursorAxis[3][2];
	struct clickableObject buttonMenu[3];
	int currentButton;
};

struct uiPage page1, page2, page3;

struct uiPage page1 = {0,0,&page2,&page3, 	"1. Time and Date\0",
						{
						{6, 18},
						{6, 27},
						{6, 36}
						},
						{
						{"1.\0",	0, 1, 2, 	STATE_RESET, UNSELECTED, &(page1.buttonMenu[1]), &(page1.buttonMenu[2])	},
						{"2.\0",	1, 2, 0, 	STATE_RESET, UNSELECTED, &(page1.buttonMenu[2]), &(page1.buttonMenu[0])	},
						{"3.\0",	2, 0, 1,	 STATE_RESET, UNSELECTED, &(page1.buttonMenu[0]), &(page1.buttonMenu[1])	}
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
						{"4.\0",	0, 1, 2,	 STATE_RESET, UNSELECTED, &(page1.buttonMenu[1]), &(page1.buttonMenu[2])	},
						{"5.\0",	1, 2, 0,	 STATE_RESET, UNSELECTED, &(page1.buttonMenu[2]), &(page1.buttonMenu[0])	},
						{"6.\0",	2, 0, 1,	 STATE_RESET, UNSELECTED, &(page1.buttonMenu[0]), &(page1.buttonMenu[1])	}
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
						{"7. Adjust time\0",0, 1, 2,	STATE_RESET, UNSELECTED, &(page1.buttonMenu[1]), &(page1.buttonMenu[2])	},
						{"8.\0",			1, 2, 0, 	STATE_RESET, UNSELECTED, &(page1.buttonMenu[2]), &(page1.buttonMenu[0])	},
						{"9.\0",			2, 0, 1,	STATE_RESET, UNSELECTED, &(page1.buttonMenu[0]), &(page1.buttonMenu[1])	}
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

	ssd1306_Line(0, 9, 127, 9, 0x01);

	ssd1306_SetCursor(1*symbolHorizontalShift, 1+0*symbolVerticalShift);		//Paste name of page
	ssd1306_WriteString(currentPage->nameOfPage, Font_6x8, White);

	ssd1306_SetCursor(1*symbolHorizontalShift, 2+2*symbolVerticalShift);		//Place corresponding buttons
	ssd1306_WriteString(currentPage->buttonMenu[0].title, Font_6x8, White);		//1.

	ssd1306_SetCursor(1*symbolHorizontalShift, 3+3*symbolVerticalShift);		//2.
	ssd1306_WriteString(currentPage->buttonMenu[1].title, Font_6x8, White);

	ssd1306_SetCursor(1*symbolHorizontalShift, 4+4*symbolVerticalShift);		//3.
	ssd1306_WriteString(currentPage->buttonMenu[2].title, Font_6x8, White);

	ssd1306_UpdateScreen();}
else {}
}

void moveCursor(){
if (currentPage->selected==1){
		ssd1306_FillRectangle(0, 10, 5, 63, 0x00);				//CLEAR BUTTON MARKERS

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

	ssd1306_FillRectangle(0, 10, 5, 63, 0x00);				//CLEAR BUTTON MARKERS

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
	ssd1306_FillRectangle(0, 9, 5, 63, 0x00);				//CLEAR BUTTON MARKERS
	ssd1306_Line(0, 9, 127, 9, 0x01);						//GET BACK TITLE UNDERLINE

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

