#pragma once

#include "Graphic.h"

typedef void (*PixelProcessCallback)(int x, int y, Color color);

/// 光栅化器类，用于实现基本的图形绘制功能
class Rasterizer
{
public:

	/// 使用DDA算法绘制直线
	/// @param x0 起点x坐标
	/// @param y0 起点y坐标
	/// @param x1 终点x坐标
	/// @param y1 终点y坐标
	static void drawLineDDA(int x0, int y0, int x1, int y1, PixelProcessCallback cb );

	/// 使用中点Bresenham算法绘制直线
	/// @param x0 起点x坐标
	/// @param y0 起点y坐标
	/// @param x1 终点x坐标
	/// @param y1 终点y坐标
	static void drawLineBresenham(int x0, int y0, int x1, int y1);

	/// 绘制多边形
	/// @param pts 顶点数组指针
	/// @param count 顶点数量
	static void drawPolygon(PixelPoint* pts, int count);

	/// 绘制矩形
	/// @param x0 左上角x坐标
	/// @param y0 左上角y坐标
	/// @param x1 右下角x坐标
	/// @param y1 右下角y坐标
	static void drawRectangle(int x0, int y0, int x1, int y1);

	

	/// 绘制圆
	/// @param centerX 圆心x坐标
	/// @param centerY 圆心y坐标
	/// @param radius 半径
	static void drawCircle(int cX, int cY, int radius, PixelProcessCallback cb);

	/// 绘制椭圆
	/// @param centerX 椭圆中心x坐标
	/// @param centerY 椭圆中心y坐标
	/// @param radiusX x轴半径
	/// @param radiusY y轴半径
	static void drawEllipse(int centerX, int centerY, int radiusX, int radiusY, PixelProcessCallback cb);
};
