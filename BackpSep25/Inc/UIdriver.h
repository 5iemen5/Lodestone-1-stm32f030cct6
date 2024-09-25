#include "main.h"

struct clickableObject{
	char title[22];
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

void nextPage();
void selectPage();
void unselectPage();
void moveCursor();
void markButton();
void showTime();
