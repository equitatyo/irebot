#include "font_renderer.h"

#include "includes.h"
#include <unistd.h>

#include "render_util.h"
#include "fontstash.h"
#include "glfontstash.h"

FontRenderer* FontRenderer_Init() {
	FontRenderer* font_renderer = malloc(sizeof(FontRenderer));
	memset(font_renderer, 0, sizeof(FontRenderer));

	font_renderer->fs = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT);

	if (font_renderer->fs == NULL) printf("Error creating fs\n");

	font_renderer->default_font = fonsAddFont(font_renderer->fs, "Arial-Rounded", "assets/arial_rounded.ttf");

	if (font_renderer->default_font == FONS_INVALID) printf("Error adding font\n");

	return font_renderer;
}

void FontRenderer_ClearState(FontRenderer* self) {
	fonsClearState(self->fs);
}

void FontRenderer_DrawString(FontRenderer* self, char* text, float x, float y, float size, Color color) {
	fonsSetSize(self->fs, size);
	fonsSetFont(self->fs, self->default_font);
	fonsSetColor(self->fs, glfonsRGBA(color.r * 255, color.g * 255, color.b * 255, color.a * 255));
	fonsDrawText(self->fs, x, y + FontRenderer_GetHeight(self, text, size), text, NULL);
}

void FontRenderer_DrawStringCentered(FontRenderer* self, char* text, float x, float y, float size, Color color) {
	float _x = x - FontRenderer_GetWidth(self, text, size) / 2;
	float _y = y - FontRenderer_GetHeight(self, text, size) / 1.5;
	FontRenderer_DrawString(self, text, _x, _y, size, color);
}

float FontRenderer_GetHeight(FontRenderer* self, char* text, float size) {
	float bounds[4];
	fonsSetSize(self->fs, size);
	fonsSetFont(self->fs, self->default_font);
	fonsTextBounds(self->fs, 0, 0, text, NULL, bounds);
	return bounds[3] - bounds[1];
}

float FontRenderer_GetWidth(FontRenderer* self, char* text, float size) {
	float bounds[4];
	fonsSetSize(self->fs, size);
	fonsSetFont(self->fs, self->default_font);
	fonsTextBounds(self->fs, 0, 0, text, NULL, bounds);
	return bounds[2] - bounds[0];
}