#ifndef _GUI_H_
#define _GUI_H_

#include "widget.h"
#include "font_renderer.h"

typedef struct _GUI {
	FontRenderer* font_renderer;

	int width;
	int height;

	double mouse_x;
	double mouse_y;

	Widget** widgets;
	int widget_count;
} GUI;

GUI* GUI_Init(int width, int height);
void GUI_AddWidget(GUI* self, Widget* widget);
void GUI_SetMousePos(GUI* self, double mouse_x, double mouse_y);
void GUI_Render(GUI* self);
void GUI_OnClick(GUI* self, int button, int is_down);
void GUI_OnKey(GUI* self, int key, int is_down);

#endif //_GUI_H_
