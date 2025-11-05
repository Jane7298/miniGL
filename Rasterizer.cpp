#include "Graphic.h"
#include "Rasterizer.h"
#include "math.h"
#include <algorithm>


/// 使用DDA算法绘制直线
void Rasterizer::drawLineDDA(int x0, int y0, int x1, int y1, PixelProcessCallback cb )
{
	// 如果起点和终点相同，只画一个点
	if (x0 == x1 && y0 == y1)
	{
		Color color = getPenColor();
		cb(x0, y0, color);
		return;
	}

	int dx = x1 - x0;
	int dy = y1 - y0;
	int steps = std::max(abs(dx), abs(dy));

	if (steps == 0) return;

	float xIncrement = (float)dx / steps;
	float yIncrement = (float)dy / steps; 
	float x = (float)x0;
	float y = (float)y0;

	Color color = getPenColor();

	for (int i = 0; i <= steps; i++)
	{
		cb((int)(x + 0.5), (int)(y + 0.5), color);
		x += xIncrement;
		y += yIncrement;
	}
}

/// 使用中点Bresenham算法绘制直线
void Rasterizer::drawLineBresenham(int x0, int y0, int x1, int y1)
{
	if (x0 == x1 && y0 == y1)
	{
		Color color = getPenColor();
		setPixel(x0, y0, color);
		return;
	}

	Color color = getPenColor();
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;

	// 根据斜率选择主方向
	if (dx > dy) {
		int e = -dx;//选取x方向为主方向
		int x = x0;
		int y = y0;

		while (x < x1) {
			setPixel(x, y, color);
			x += sx;
			e = e + 2 * dy;//更新误差，避免小数
			if (e > 0) {
				y += sy;
				e = e - 2 * dx;
			}
		}
		setPixel(x, y, color);
	}
	else
	{
		int e = -dy;//选取y方向为主方向
		int x = x0;
		int y = y0;

		while (y < y1) {
			setPixel(x, y, color);
			y += sy;
			e = e + 2 * dx;//更新误差，避免小数
			if (e > 0) {
				x += sx;
				e = e - 2 * dy;
			}
		}
		setPixel(x, y, color);
	}
	
}

/// 绘制多边形
void Rasterizer::drawPolygon(PixelPoint* pts, int count)
{
	if (pts == nullptr || count < 2) return;

	for (int i = 0; i < count - 1; ++i)
	{
		drawLineDDA(pts[i].x, pts[i].y, pts[i + 1].x, pts[i + 1].y, setPixel);
	}
	if (count > 2) {
		drawLineDDA(pts[count - 1].x, pts[count - 1].y, pts[0].x, pts[0].y, setPixel);
	}
}

/// 绘制矩形
void Rasterizer::drawRectangle(int x0, int y0, int x1, int y1)
{
	if (x0 > x1) std::swap(x0, x1);
	if (y0 > y1) std::swap(y0, y1);//调整坐标

	//// 绘制矩形的四条边
	drawLineDDA(x0, y0, x1, y0, setPixel); // 上边
	drawLineDDA(x1, y0, x1, y1, setPixel); // 右边
	drawLineDDA(x1, y1, x0, y1, setPixel); // 下边
	drawLineDDA(x0, y1, x0, y0, setPixel); // 左边
}

/// 绘制圆（使用中点Bresenham画圆算法）
void Rasterizer::drawCircle(int cX, int cY, int radius, PixelProcessCallback cb) {
	if (radius <= 0) return;

	Color color = getPenColor();
	int x = 0;
	int y = radius;
	int d = 1 - radius;//设置误差，避免浮点数

	auto drawCirclePoints = [&](int x, int y) {
		cb(cX + x, cY + y, color);
		cb(cX - x, cY + y, color);
		cb(cX + x, cY - y, color);
		cb(cX - x, cY - y, color);
		cb(cX + y, cY + x, color);
		cb(cX - y, cY + x, color);
		cb(cX + y, cY - x, color);
		cb(cX - y, cY - x, color);
		};

	drawCirclePoints(x, y);


	while (x < y)
	{
		if (d < 0)
		{
			d += 2 * x + 3;
		}
		else
		{
			d += 2 * (x - y) + 5;
			y--;
		}
		x++;
		drawCirclePoints(x, y);
	}

}

/// 绘制椭圆（使用中点椭圆算法）
void Rasterizer::drawEllipse(int centerX, int centerY, int radiusX, int radiusY, PixelProcessCallback cb)
{
	if (radiusX <= 0 || radiusY <= 0) return;
	
	Color color = getPenColor();
	int a = radiusX;
	int b = radiusY; 
	

	auto drawEllipsePoints = [&](int x, int y) {
		cb(centerX + x, centerY + y, color);
		cb(centerX - x, centerY + y, color);
		cb(centerX + x, centerY - y, color);
		cb(centerX - x, centerY - y, color);
	};

	// 第一区域：斜率 < 1
	int x = 0;
	int y = b;
	int a2 = a * a;
	int b2 = b * b;
	int d1 = b2 - a2 * b + a2 / 4;

	drawEllipsePoints(x, y);

	while (b2 * x < a2 * y)
	{
		if (d1 < 0)
		{
			d1 += b2 * (2 * x + 3);
		}
		else
		{
			d1 += b2 * (2 * x + 3) + a2 * (-2 * y + 2);
			y--;
		}
		x++;
		drawEllipsePoints(x, y);
	}

	// 第二区域：斜率 >= 1
	int d2 = b2 * (x + 0.5) * (x + 0.5) + a2 * (y - 1) * (y - 1) - a2 * b2;

	while (y > 0)
	{
		if (d2 < 0)
		{
			d2 += b2 * (2 * x + 2) + a2 * (-2 * y + 3);
			x++;
		}
		else
		{
			d2 += a2 * (-2 * y + 3);
		}
		y--;
		drawEllipsePoints(x, y);
	}
}