#include "draw_line.h"


#define _USE_MATH_DEFINES
#include <math.h>

#include <algorithm>


cg::LineDrawing::LineDrawing(unsigned short width, unsigned short height): cg::ClearRenderTarget(width, height)
{
}

cg::LineDrawing::~LineDrawing()
{
}

void cg::LineDrawing::DrawLine(unsigned x_begin, unsigned y_begin, unsigned x_end, unsigned y_end, color color)
{
	const bool steep = abs((int)y_end - (int)y_begin) > abs((int)x_end - (int)x_begin);

	if (steep) {
		std::swap(x_begin, y_begin);
		std::swap(x_end, y_end);
	}

	if (x_begin > x_end) {
		std::swap(x_begin, x_end);
		std::swap(y_begin, y_end);
	}

	const float dx = (float)x_end - (float)x_begin;
	const float dy = abs((float)y_end - (float)y_begin);

	float error = dx / 2.f;
	const int ystep = (y_begin < y_end) ? 1 : -1;
	unsigned y = y_begin;

	for (unsigned x = x_begin; x <= x_end; x++) {
		if (steep) {
			SetPixel(y, x, color);
		} else {
			SetPixel(x, y, color);
		}

		error -= dy;
		if (error < 0) {
			y += ystep;
			error += dx;
		}
	}

	/*double dy = y_begin - y_end;
	double dx = x_begin - x_end;
	double slope = dy / dx;

	for (unsigned x = x_begin; x <= x_end; x++) {
		double y = y_begin + slope * (x - x_begin);
		SetPixel(x, round(y), color);
	}*/
}

void cg::LineDrawing::DrawScene()
{
    unsigned x_center = width / 2;
    unsigned y_center = height / 2;
	unsigned radius = std::min(x_center, y_center) - 1;
	for (double angle = 0.0; angle < 360.0; angle += 5.0)
	{
		DrawLine(x_center, y_center, static_cast<unsigned>(x_center + radius * cos(angle * M_PI / 180)), static_cast<unsigned>(y_center + radius * sin(angle * M_PI / 180)),
            color(static_cast<unsigned char>(255 * sin(angle * M_PI / 180)), static_cast<unsigned char>(255 * cos(angle * M_PI / 180)), 255));
	}

}

