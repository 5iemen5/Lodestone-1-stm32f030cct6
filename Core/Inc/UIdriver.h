/*
 * UIdriver.h
 *
 *  Created on: Sep 18, 2024
 *      Author: siemens
 */
#include "main.h"

struct clickableObject{
	char title[22];
	char value;
	unsigned int numberOfObject, nextObject, prevObject;
	unsigned int state, selected, buttonType;
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

struct uiSubPage{
	unsigned int numberOfPage, selected;
	struct uiSubPage * nextoPage;
	struct uiSubPage * prevPage;
	char nameOfPage[22];
	int cursorAxis[7][2];
	struct clickableObject buttonMenu[7];
	int currentButton;
};

void nextPage();
void nextSubPage();
void selectPage();
void selectSubPage();
void unselectPage();
void unselectSubPage();
void moveCursor();
void moveSPCursor();
void markButton();
void markSubPageButton();
void showTime();
void markSet();
void unmarkSet();
void pressButton();
void pageCalibTime();
