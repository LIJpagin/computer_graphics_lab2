#include "Object2D.h"
#define PI 3.14159
unsigned int Object2D::action_object_ = 0;
unsigned int Object2D::number_objects_ = 0;

Object2D::Object2D(double coord, ...) {
	va_list vl;
	points_.resize(coord);
	va_start(vl, coord);
	object_number_ = number_objects_++;
	for (auto i = 0; i < coord * 2; i++)
		if (i % 2) points_[i / 2].y = va_arg(vl, double);
		else points_[i / 2].x = va_arg(vl, double);
	va_end(vl);
}

void Object2D::installPen(COLORREF color_pen, int width_pen) {
	width_pen_ = width_pen;
	color_pen_ = color_pen;
}

void Object2D::points(double coord, ...) {
	points_.clear();
	points_.resize(coord);
	va_list vl;
	va_start(vl, coord);
	for (auto i = 0; i < coord; i++) {
		points_[i / 2].x = va_arg(vl, double);
		points_[i / 2].y = va_arg(vl, double);
	}
	va_end(vl);
}

void Object2D::moving(double Dx, double Dy) {
	for (auto point = 0; point < points_.size(); point++) {
		points_[point].x = points_[point].x + Dx;
		points_[point].y = points_[point].y + Dy;
	}
}

void Object2D::scalingByAxes(double Sx, double Sy) {
	for (auto point = 0; point < points_.size(); point++) {
		points_[point].x = points_[action_point_].x + (points_[point].x - points_[action_point_].x) * Sx;
		points_[point].y = points_[action_point_].y + (points_[point].y - points_[action_point_].y) * Sy;
	}
}

void Object2D::rotation(double angle) {
	for (auto point = 0; point < points_.size(); point++) {
		points_[point].x = points_[action_point_].x +
			(points_[point].x - points_[action_point_].x) * cos(angle * PI / 180) -
			(points_[point].y - points_[action_point_].y) * sin(angle * PI / 180);
		points_[point].y = points_[action_point_].y +
			(points_[point].x - points_[action_point_].x) * sin(angle * PI / 180) +
			(points_[point].y - points_[action_point_].y) * cos(angle * PI / 180);
	}
}

void Object2D::draw(HDC hdc) const {
	HPEN hPen1;
	hPen1 = CreatePen(PS_DASHDOT, width_pen_, object_number_ == action_object_ ? 0xFFFFFF : color_pen_);
	SelectObject(hdc, hPen1);
	MoveToEx(hdc, (int)round(points_[0].x), (int)round(points_[0].y), NULL);
	for (auto i = 1; i < points_.size(); i++)
		LineTo(hdc, (int)round(points_[i].x), (int)round(points_[i].y));
	DeleteObject(hPen1);

	HPEN hPen2;
	hPen2 = CreatePen(PS_DASHDOT, width_pen_, 0x0000FF);
	SelectObject(hdc, hPen2);
	Ellipse(hdc, (int)round(points_[action_point_].x - width_pen_ / 2),
		(int)round(points_[action_point_].y - width_pen_ / 2),
		(int)round(points_[action_point_].x + width_pen_ / 2),
		(int)round(points_[action_point_].y + width_pen_ / 2));
	DeleteObject(hPen2);
}

void Object2D::lineBresenham(HDC hdc, int x0, int y0, int x1, int y1, COLORREF color) const {
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2;
	int e2 = err;
	for (;;) {
		SetPixel(hdc, x0, y0, color);
		SetPixel(hdc, x0 + 1, y0 + 1, color);
		SetPixel(hdc, x0 - 1, y0 - 1, color);
		SetPixel(hdc, x0 - 1, y0 + 1, color);
		SetPixel(hdc, x0 + 1, y0 - 1, color);
		if (x0 == x1 && y0 == y1) break;
		e2 = err;
		if (e2 > -dx) err -= dy, x0 += sx;
		if (e2 < dy) err += dx, y0 += sy;
	}
}

void Object2D::drawBresenham(HDC hdc) const {
	for (auto i = 0; i < points_.size() - 1; i++)
		lineBresenham(hdc, (int)round(points_[i].x), (int)round(points_[i].y),
			(int)round(points_[i + 1].x), (int)round(points_[i + 1].y), object_number_ == action_object_ ? 0xFFFFFF : color_pen_);

	HPEN hPen;
	hPen = CreatePen(PS_DASHDOT, width_pen_, 0x0000FF);
	SelectObject(hdc, hPen);
	Ellipse(hdc, (int)round(points_[action_point_].x - width_pen_ / 2),
		(int)round(points_[action_point_].y - width_pen_ / 2),
		(int)round(points_[action_point_].x + width_pen_ / 2),
		(int)round(points_[action_point_].y + width_pen_ / 2));
	DeleteObject(hPen);
}