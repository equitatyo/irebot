#include "render_util.h"

#include "includes.h"

#include <math.h>

#define PI 3.1415926535

Color color_from_bytes(int _r, int _g, int _b, int _a) {
	return (Color) { (float)_r / 255.0, (float)_g / 255.0, (float)_b / 255.0, (float)_a / 255.0};
}

void set_color(Color color) {
	glColor4f(color.r, color.g, color.b, color.a);
}

void draw_rect(float x, float y, float width, float height, int filled, Color color) {
	set_color(color);

	glBegin(filled ? GL_TRIANGLE_FAN : GL_LINES);

	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);

	glEnd();
}

void draw_rounded_rect_custom(float x, float y, float _width, float _height, float radius, Color color, int top_left, int top_right, int bottom_right, int bottom_left) {
		float halfRadius = radius / 2;
		float width = _width - halfRadius;
		float height = _height - halfRadius;

		glColor4f(color.r, color.g, color.b, color.a);

		if (top_left) {

		    glEnable(GL_LINE_SMOOTH);
		    glBegin(GL_TRIANGLE_FAN);

		    for (double i = 180; i <= 270; i++) {
		        float angle = i * (PI * 2) / 360;
		        glVertex2f(x + (halfRadius * cos(angle)) + halfRadius, y + (halfRadius * sin(angle)) + halfRadius);
		    }
		    glVertex2f(x + halfRadius, y + halfRadius);


		    glEnd();

		}
		else {
		    draw_rect(x, y, halfRadius, halfRadius, 1, color);
		}


		if (bottom_right) {

		    glBegin(GL_TRIANGLE_FAN);
		    for (double i = 0; i <= 90; i++) {
		        float angle = i * (PI * 2) / 360;
		        glVertex2f(x + width + (halfRadius * cos(angle)), y + height + (halfRadius * sin(angle)));
		    }

		    glVertex2f(x + width, y + height);
		    glEnd();
		}
		else {
		    draw_rect(x + width, y + height, halfRadius, halfRadius, 1, color);
		}

		if (top_right) {

		    glBegin(GL_TRIANGLE_FAN);
		    for (double i = 270; i <= 360; i++) {
		        float angle = i * (PI * 2) / 360;
		        glVertex2f(x + width + (halfRadius * cos(angle)), y + (halfRadius * sin(angle)) + halfRadius);
		    }

		    glVertex2f(x + width, y + halfRadius);
		    glEnd();
		}
		else {
		    draw_rect(x + width, y, halfRadius, halfRadius, 1, color);
		}

		if (bottom_left) {

		    glBegin(GL_TRIANGLE_FAN);
		    for (double i = 90; i <= 180; i++) {
		        float angle = i * (PI * 2) / 360;
		        glVertex2f(x + (halfRadius * cos(angle)) + halfRadius, y + height + (halfRadius * sin(angle)));
		    }
		    glVertex2f(x + halfRadius, y + height);
		    glEnd();
		    glDisable(GL_LINE_SMOOTH);
		}
		else {
		    draw_rect(x, y + height, halfRadius, halfRadius, 1, color);
		}

		draw_rect(x, y + halfRadius, width + halfRadius, height - halfRadius, 1, color);

		draw_rect(x + halfRadius, y, width - halfRadius, halfRadius, 1, color);
		draw_rect(x + halfRadius, y + height, width - halfRadius, halfRadius, 1, color);
}

void draw_rounded_rect(float x, float y, float width, float height, float radius, Color color) {
	draw_rounded_rect_custom(x, y, width, height, radius, color, 1, 1, 1, 1);
}

void draw_circle(float cx, float cy, float r, float start_angle, float arc_angle, int filled, int num_segments) { //http://slabode.exofire.net/circle_draw.shtml
	float theta = arc_angle / (float)num_segments - 1;

	float tangetial_factor = tanf(theta);

	float radial_factor = cosf(theta);

	
	float x = r * cosf(start_angle);
	float y = r * sinf(start_angle); 
    
	glBegin(filled ? GL_TRIANGLE_FAN : GL_LINE_STRIP);
	for(int ii = 0; ii < num_segments; ii++)
	{ 
		glVertex2f(x + cx, y + cy);

		float tx = -y; 
		float ty = x; 

		x += tx * tangetial_factor; 
		y += ty * tangetial_factor; 

		x *= radial_factor; 
		y *= radial_factor; 
	} 
	glEnd(); 
}

void draw_error_obj(float x, float y, float width, float height) {
	float half_width = width / 2;
	float half_height = height / 2;

	draw_rect(x, y, half_width, half_height, 1, color_from_bytes(255, 0, 255, 255));
	draw_rect(x + half_width, y + half_height, half_width, half_height, 1, color_from_bytes(255, 0, 255, 255));
	draw_rect(x + half_width, y, half_width, half_height, 1, color_from_bytes(0, 0, 0, 255));
	draw_rect(x, y + half_height, half_width, half_height, 1, color_from_bytes(0, 0, 0, 255));
}

int colliding(float x1, float y1, float x, float y, float width, float height) {
	return x1 >= x && x1 <= x + width && y1 >= y && y1 <= y + height;
}