#include "gui.h"

#include "includes.h"
#include "widget.h"
#include "render_util.h"
#include "font_renderer.h"
#include "networking.h"

#include "audio/dr_wav.h"
#include "macro.h"
#include "audio/clicks.h"

#define CURRENT_VERSION "0.1"

char* newest_version;

int show_outdated_msg = true;

char macro[MAX_PATH];
char clickpack1[MAX_PATH];
char clickpack2[MAX_PATH];

Widget* download_button; //button

Widget* choose_clickpack1; //button
Widget* choose_clickpack2; //button

Widget* last_soft_time; //slider
Widget* next_soft_time; //slider

Widget* volume1; //slider
Widget* volume2; //slider

Widget* choose_macro; //button
Widget* update_clicks; //button

Widget* render_button; //button

Clicks** player1;
Clicks** player2;

void choose_macro_callback() {
	OPENFILENAMEA ofn;
    memset(&ofn,0,sizeof(ofn));
    ofn.lStructSize     = sizeof(ofn);
    ofn.hwndOwner       = NULL;
    ofn.hInstance       = NULL;
    ofn.lpstrFilter     = "Text files\0*.txt\0\0";
    ofn.lpstrFile 		= macro;
    ofn.nMaxFile        = MAX_PATH;
    ofn.lpstrTitle      = "Please select a plain text macro";
    ofn.Flags           = OFN_NONETWORKBUTTON |
                          OFN_FILEMUSTEXIST;

    if (!GetOpenFileNameA(&ofn)) {
    	printf("GetOpenFileNameA failed (%i)\n", GetLastError());
    }
}

void choose_clickpack1_callback() {
	TODO();
}

void choose_clickpack2_callback() {
	TODO();
}

void render_callback() {
	if (!macro) { printf("No macro specified\n"); return; }

    Macro* parsed_macro = parse_txt(macro);

	//TODO choose output path
    generate_clicks("output.wav", parsed_macro, player1, player2, last_soft_time->value, next_soft_time->value, volume1->value, volume2->value);
    Macro_free(parsed_macro);
}

void update_clicks_callback() {
	//if (player1) unload_clicks(player1); TODO: FIX THIS (!)
	player1 = load_clicks("player1");
	player2 = load_clicks("player2");
}

void download_callback() {
	update_to_latest();
}

GUI* GUI_Init(int width, int height) {
	GUI* gui = malloc(sizeof(GUI));
	memset(gui, 0, sizeof(GUI));

	gui->font_renderer = FontRenderer_Init();

	gui->width = width;
	gui->height = height;

	gui->mouse_x = 0;
	gui->mouse_y = 0;

	gui->widgets = malloc(sizeof(Widget*) * 32);

	float slider_value = 0;

	ZeroMemory(&macro, MAX_PATH);
    //choose_clickpack1 = Widget_Init_Button(gui, "Set Player 1", 65, 95, 90, 35, choose_clickpack1_callback);
    //choose_clickpack2 = Widget_Init_Button(gui, "Set Player 2", 65, 140, 90, 35, choose_clickpack2_callback);

	choose_macro = Widget_Init_Button(gui, "Set Macro", 65, 95, 90, 35, choose_macro_callback);
	update_clicks = Widget_Init_Button(gui, "Update Clicks", 65, 195, 110, 35, update_clicks_callback);

    last_soft_time = Widget_Init_Slider(gui, "Last soft time", 245, 100, 150, 0, 0, 3.0, 0.5 / 3);
    next_soft_time = Widget_Init_Slider(gui, "Next soft time", 245, 145, 150, 0, 0, 3.0, 1.5 / 3);

    volume1 = Widget_Init_Slider(gui, "Player 1", 445, 100, 130, 0, 0, 100, 1);
    volume2 = Widget_Init_Slider(gui, "Player 2", 445, 145, 130, 0, 0, 100, 1);

	//choose_macro = Widget_Init_Button(gui, "Set macro", 65, 225, 90, 35, choose_macro_callback);

    render_button = Widget_Init_Button(gui, "Render", gui->width / 2 - 90 / 2, gui->height - 35 - 15, 90, 35, render_callback);

	//GUI_AddWidget(gui, choose_clickpack1);
    //GUI_AddWidget(gui, choose_clickpack2);
	GUI_AddWidget(gui, choose_macro);
	GUI_AddWidget(gui, update_clicks);

    GUI_AddWidget(gui, last_soft_time);
    GUI_AddWidget(gui, next_soft_time);

    GUI_AddWidget(gui, volume1);
    GUI_AddWidget(gui, volume2);

    GUI_AddWidget(gui, render_button);

	update_clicks_callback();
	
	newest_version = get_version();

	if (newest_version) show_outdated_msg = strcmp(newest_version, CURRENT_VERSION);
	else show_outdated_msg = false;

	return gui;
}

void GUI_AddWidget(GUI* self, Widget* widget) {
	if (self->widget_count >= 32) return;

	self->widgets[self->widget_count] = widget;
	self->widget_count++;
}

void GUI_SetMousePos(GUI* self, double mouse_x, double mouse_y) {
	self->mouse_x = mouse_x;
	self->mouse_y = mouse_y;
}

void GUI_Render(GUI* self) {
	FontRenderer_ClearState(self->font_renderer);
	draw_rect(0, 0, self->width, self->height, true, color_from_bytes(35, 35, 35, 255));

	FontRenderer_DrawStringCentered(self->font_renderer, "IreBot", self->width / 2, 30, 32, WHITE);

	for (int i = 0; i < self->widget_count; i++)
		Widget_Render(self->widgets[i], self->mouse_x, self->mouse_y);

    // -- MAIN RENDER --

    //FontRenderer_DrawString(self->font_renderer, "Clickpack(s)", 45, 50, 20, WHITE);
	FontRenderer_DrawString(self->font_renderer, "Macro", 45, 60, 20, WHITE);

	FontRenderer_DrawString(self->font_renderer, "Clickpack", 45, 150, 20, WHITE);

    FontRenderer_DrawString(self->font_renderer, "Soft clicks", 225, 60, 20, WHITE);

    FontRenderer_DrawString(self->font_renderer, "Volume", 425, 60, 20, WHITE);

    //FontRenderer_DrawString(self->font_renderer, "Replay", 45, 180, 20, WHITE);

	// -- END MAIN RENDER --

	if (show_outdated_msg) {
		draw_rect(0, 0, self->width, self->height, true, color_from_bytes(20, 20, 20, 200));

		float msg_width = 350;
		float msg_height = 200;

		draw_rounded_rect(self->width / 2 - msg_width / 2, self->height / 2 - msg_height / 2, msg_width, msg_height, 50, color_from_bytes(50, 50, 50, 255));

		FontRenderer_DrawStringCentered(self->font_renderer, "Outdated version", self->width / 2, self->height / 2 - msg_height / 2 + 30, 32, WHITE);

		FontRenderer_DrawStringCentered(self->font_renderer, "There is a new version", self->width / 2, self->height / 2 - msg_height / 2 + 60, 20, WHITE);
		FontRenderer_DrawStringCentered(self->font_renderer, "of IreBot available", self->width / 2, self->height / 2 - msg_height / 2 + 60 + FontRenderer_GetHeight(self->font_renderer, "T", 20) + 5, 20, WHITE);

		if (!download_button) {
			char download_text[32];
			sprintf(download_text, "Download %s", newest_version);
			download_button = Widget_Init_Button(self, download_text, self->width / 2 - 175 / 2, self->height / 2 + msg_height / 2 - 50 - 10, 175, 50, download_callback);
		}

		Widget_Render(download_button, self->mouse_x, self->mouse_y);
	}
}

void GUI_OnClick(GUI* self, int button, int is_down) {
	if (show_outdated_msg) {
		float msg_width = 350;
		float msg_height = 200;

		if ((colliding(self->mouse_x, self->mouse_y, self->width / 2 - 175 / 2, self->height / 2 + msg_height / 2 - 50 - 10, 175, 50) ||
			!colliding(self->mouse_x, self->mouse_y, self->width / 2 - msg_width / 2, self->height / 2 - msg_height / 2, msg_width, msg_height)) && !is_down)
			show_outdated_msg = false;

		Widget_OnClick(download_button, self->mouse_x, self->mouse_y, button, is_down);
		return;
	}
	for (int i = 0; i < self->widget_count; i++)
		Widget_OnClick(self->widgets[i], self->mouse_x, self->mouse_y, button, is_down);
}

void GUI_OnKey(GUI* self, int key, int is_down) {
	if (show_outdated_msg && key == GLFW_KEY_ESCAPE) show_outdated_msg = false; 
}