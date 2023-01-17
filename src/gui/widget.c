#include "widget.h"

#include "includes.h"

#include "render_util.h"
#include "font_renderer.h"
#include "gui.h"

#define FONT_RENDERER ((GUI*)self->gui)->font_renderer

Widget* Widget_Init_Button(void* gui, char* text, float x, float y, float width, float height, void(*callback)()) {
	Widget* widget = malloc(sizeof(Widget));
	memset(widget, 0, sizeof(Widget));

	widget->gui = gui;

	widget->type = BUTTON;

	widget->x = x;
	widget->y = y;

	widget->width = width;
	widget->height = height;

	widget->text = strdup(text);

	widget->callback = callback;

	return widget;
}

Widget* Widget_Init_Slider(void* gui, char* text, float x, float y, float width, float value, float min, float max, float progress) {
	Widget* widget = malloc(sizeof(Widget));
	memset(widget, 0, sizeof(Widget));

	widget->gui = gui;

	widget->type = SLIDER;

	widget->x = x;
	widget->y = y;

	widget->width = width;
	widget->height = 20;

	widget->text = strdup(text);

	widget->value = value;
	widget->min = min;
	widget->max = max;
	widget->progress = progress;

	return widget;
}

Widget* Widget_Init_Checkbox(void* gui, char* text, float x, float y, float a, int toggled) {
	Widget* widget = malloc(sizeof(Widget));
	memset(widget, 0, sizeof(Widget));

	widget->gui = gui;

	widget->type = CHECKBOX;

	widget->x = x;
	widget->y = y;

	widget->width = a;
	widget->height = a;

	widget->text = strdup(text);

	widget->toggled = toggled;

	return widget;
}

void Widget_OnClick(Widget* self, double mouse_x, double mouse_y, int button, int is_down) {

	if (is_down) {
		switch (self->type) {
			case BUTTON:
				if (colliding(mouse_x, mouse_y, self->x, self->y, self->width, self->height))
					self->holding = true;
				break;
			case SLIDER:
				if (colliding(mouse_x, mouse_y, self->x, self->y, self->width, self->height))
					self->holding = true;
				break;
			case NONE:
			default:
				break;
		}
	} else {
		switch (self->type) {
			case BUTTON:
				if (colliding(mouse_x, mouse_y, self->x, self->y, self->width, self->height) && self->holding)
					self->callback();
				break;
			case CHECKBOX:
				if (colliding(mouse_x, mouse_y, self->x, self->y, self->width, self->height))
					self->toggled = !self->toggled;
			case NONE:
			default:
				break;
		}
		self->holding = false;
	}
}

void Widget_Render(Widget* self, double mouse_x, double mouse_y) {
	switch (self->type) {
		case BUTTON:
			;

			Color color = MAIN_COLOR;
			if (self->holding) color = DARK_MAIN_COLOR;
			else if (colliding(mouse_x, mouse_y, self->x, self->y, self->width, self->height)) color = LIGHT_MAIN_COLOR;

			draw_rounded_rect(self->x, self->y, self->width, self->height, 15, color);
			FontRenderer_DrawStringCentered(FONT_RENDERER, self->text, self->x + self->width / 2, self->y + self->height / 2, 18,  color_from_bytes(255, 255, 255, 255));
			break;
		case SLIDER:

			if (self->holding) {
				self->progress = (mouse_x - self->x) / self->width;
				if (self->progress < 0) self->progress = 0;
				if (self->progress > 1) self->progress = 1;
			}

			self->value = (self->progress * (self->max - self->min)) + self->min;

			float local_progress = self->progress;
			if (local_progress < self->height / 2 / self->width) local_progress = self->height / 2 / self->width;

			char buff[128];

			sprintf_s(buff, 128, "%s: %.2f", self->text, self->value);

			draw_rounded_rect(self->x , self->y, self->width, self->height, self->height / 2, color_from_bytes(75, 75, 75, 255));
			draw_rounded_rect(self->x, self->y, self->width * local_progress, self->height, self->height / 2, MAIN_COLOR);

			FontRenderer_DrawStringCentered(FONT_RENDERER, buff, self->x + self->width / 2, self->y + 10, 16, color_from_bytes(255, 255, 255, 255));
			break;
		case CHECKBOX:
			if (self->toggled) {
				draw_rounded_rect(self->x, self->y, self->width, self->height, self->height / 4, MAIN_COLOR);

				set_color(color_from_bytes(255, 255, 255, 255));
				draw_circle(self->x + self->width / 2, self->y + self->height / 2, self->width / 2.5, 0, 15, true, 24);

			} else {
				draw_rounded_rect(self->x, self->y, self->width, self->height, self->height / 4, color_from_bytes(75, 75, 75, 255));
			}

			FontRenderer_DrawString(FONT_RENDERER, self->text, self->x + self->width + 5, self->y + self->height / 6, self->height / 1.5, color_from_bytes(255, 255, 255, 255));
			break;
		case NONE:
		default:
			draw_error_obj(self->x, self->y, self->width, self->height);
			break;
	}
}