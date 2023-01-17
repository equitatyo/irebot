#ifndef _FONT_RENDERER_H_
#define _FONT_RENDERER_H_

#include "render_util.h"
#include "fontstash.h"

typedef struct _FontRenderer {
	FONScontext* fs;
	int default_font;

} FontRenderer;

FontRenderer* FontRenderer_Init();
void FontRenderer_ClearState(FontRenderer* self);
void FontRenderer_DrawString(FontRenderer* self, char* text, float x, float y, float size, Color color);
void FontRenderer_DrawStringCentered(FontRenderer* self, char* text, float x, float y, float size, Color color);
float FontRenderer_GetHeight(FontRenderer* self, char* text, float size);
float FontRenderer_GetWidth(FontRenderer* self, char* text, float size);

#endif //_FONT_RENDERER_H_