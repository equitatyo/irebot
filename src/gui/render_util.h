#ifndef _RENDER_UTIL_H
#define _RENDER_UTIL_H

#define MAIN_COLOR color_from_bytes(0, 120, 255, 255)
#define LIGHT_MAIN_COLOR color_from_bytes(0, 150, 255, 255)
#define DARK_MAIN_COLOR color_from_bytes(0, 80, 255, 255)

#define WHITE color_from_bytes(255, 255, 255, 255)

typedef struct _Color {
	float r,g,b,a;
} Color;

Color color_from_bytes(int r, int g, int b, int a);
void set_color(Color color);
void draw_rect(float x, float y, float width, float height, int filled, Color color);
void draw_rounded_rect(float x, float y, float width, float height, float radius, Color color);
void draw_circle(float cx, float cy, float r, float start_angle, float arc_angle, int filled, int num_segments);
void draw_error_obj(float x, float y, float width, float height);
int colliding(float x1, float y1, float x, float y, float width, float height);

#endif //_RENDER_UTIL_H