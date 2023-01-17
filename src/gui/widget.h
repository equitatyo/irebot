#ifndef _WIDGET_H_
#define _WIDGET_H_

typedef enum _WidgetType {
	NONE = 0,
	BUTTON = 1,
	//TEXT,
	SLIDER = 2,
	CHECKBOX = 3
} WidgetType;

typedef struct _Widget {
	void* gui;

	WidgetType type;

	float x;
	float y;

	float width;
	float height;

	int holding;

	char* text;

	//Button
	void(*callback)();

	//Slider
	float value;
	float min;
	float max;
	float progress;

	//Checkbox
	int toggled;
} Widget;

Widget* Widget_Init_Button(void* gui, char* text, float x, float y, float width, float height, void(*callback)());
Widget* Widget_Init_Slider(void* gui, char* text, float x, float y, float width, float value, float min, float max, float progress);
Widget* Widget_Init_Checkbox(void* gui, char* text, float x, float y, float a, int toggled);
void Widget_OnClick(Widget* self, double mouse_x, double mouse_y, int button, int is_down);
void Widget_Render(Widget* self, double mouse_x, double mouse_y);
void Widget_GetHeight(Widget* self);

#endif //_WIDGET_H_