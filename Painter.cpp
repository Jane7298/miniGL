#include "Painter.h"
#include "Padding.h"
#include "Rasterizer.h"
#include <algorithm>
#include <cmath>

Painter g_Painter;
PixelProcessCallback cb;

Painter::Painter()
{
    //默认模式
    mPainterMode = pmPixel;
    gridSize = 5;
    color = RED;
    padding = new Padding();
}

Painter::~Painter()
{
    delete padding;
}

void Painter::drawLine(int x0, int y0, int x1, int y1)
{
	if( mPainterMode == pmPixel )
		Rasterizer::drawLineDDA(x0, y0, x1, y1, setPixel );
	else
	{
		int g_x0, g_y0, g_x1, g_y1;
		pixelToGrid(x0, y0, g_x0, g_y0 );
		pixelToGrid(x1, y1, g_x1, g_y1);

		Rasterizer::drawLineDDA(g_x0, g_y0, g_x1, g_y1, drawGridCell );
	}
}

void Painter::drawPolygon(PixelPoint* pts, int count) {
	if (pts == nullptr || count < 2) return;
	
	if (mPainterMode == pmPixel)
		Rasterizer::drawPolygon(pts, count);
	else
	{
		std::vector<PixelPoint> gridPts(count);
		for (int i = 0; i < count; i++) {
			pixelToGrid(pts[i].x, pts[i].y, gridPts[i].x, gridPts[i].y);
		}
		
		// 绘制多边形边
		for (int i = 0; i < count - 1; i++) {
			Rasterizer::drawLineDDA(gridPts[i].x, gridPts[i].y, gridPts[i + 1].x, gridPts[i + 1].y, drawGridCell);
		}
		if (count > 2) {
			Rasterizer::drawLineDDA(gridPts[count - 1].x, gridPts[count - 1].y, gridPts[0].x, gridPts[0].y, drawGridCell);
		}
	}
}

void Painter::fillPolygon(PixelPoint* pts, int count) {
	if (pts == nullptr || count < 3) return;
	
	if (mPainterMode == pmPixel) {
		// 像素模式：直接使用Padding类填充
		padding->setPixelCallback(setPixel);
		padding->fillPolygon(pts, count, color);
	}
	else {
		// 网格模式：将像素坐标转换为网格坐标后填充
		std::vector<PixelPoint> gridPts(count);
		for (int i = 0; i < count; i++) {
			pixelToGrid(pts[i].x, pts[i].y, gridPts[i].x, gridPts[i].y);
		}
		
		// 使用Padding类填充网格坐标的多边形，使用drawGridCell作为回调
		padding->setPixelCallback(drawGridCell);
		padding->fillPolygon(gridPts.data(), count, color);
	}
}

void Painter::fillRectangle(int x1, int y1, int x2, int y2) {
	if (mPainterMode == pmPixel) {
		// 像素模式：直接使用Padding类填充
		padding->setPixelCallback(setPixel);
		padding->fillRectangle(x1, y1, x2, y2, color);
	}
	else {
		// 网格模式：将像素坐标转换为网格坐标后填充
		int g_x1, g_y1, g_x2, g_y2;
		pixelToGrid(x1, y1, g_x1, g_y1);
		pixelToGrid(x2, y2, g_x2, g_y2);
		
		// 使用Padding类填充网格坐标的矩形，使用drawGridCell作为回调
		padding->setPixelCallback(drawGridCell);
		padding->fillRectangle(g_x1, g_y1, g_x2, g_y2, color);
	}
}

void Painter::fillCircle(int centerX, int centerY, int radius) {
	if (radius <= 0) return;
	
	if (mPainterMode == pmPixel) {
		// 像素模式：直接使用Padding类填充
		padding->setPixelCallback(setPixel);
		padding->fillCircle(centerX, centerY, radius, color);
	}
	else {
		// 网格模式：将像素坐标转换为网格坐标后填充
		int gridCenterX, gridCenterY, gridRadius;
		pixelToGrid(centerX, centerY, gridCenterX, gridCenterY);
		gridRadius = radius / gridSize;
		if (gridRadius <= 0) gridRadius = 1; // 确保最小半径为1
		
		// 使用Padding类填充网格坐标的圆，使用drawGridCell作为回调
		padding->setPixelCallback(drawGridCell);
		padding->fillCircle(gridCenterX, gridCenterY, gridRadius, color);
	}
}

void Painter::fillEllipse(int centerX, int centerY, int radiusX, int radiusY) {
	if (radiusX <= 0 || radiusY <= 0) return;
	
	if (mPainterMode == pmPixel) {
		// 像素模式：直接使用Padding类填充
		padding->setPixelCallback(setPixel);
		padding->fillEllipse(centerX, centerY, radiusX, radiusY, color);
	}
	else {
		// 网格模式：将像素坐标转换为网格坐标后填充
		int gridCenterX, gridCenterY, gridRadiusX, gridRadiusY;
		pixelToGrid(centerX, centerY, gridCenterX, gridCenterY);
		gridRadiusX = radiusX / gridSize;
		gridRadiusY = radiusY / gridSize;
		if (gridRadiusX <= 0) gridRadiusX = 1; 
		if (gridRadiusY <= 0) gridRadiusY = 1;
		
		// 使用Padding类填充网格坐标的椭圆，使用drawGridCell作为回调
		padding->setPixelCallback(drawGridCell);
		padding->fillEllipse(gridCenterX, gridCenterY, gridRadiusX, gridRadiusY, color);
	}
}

void Painter::drawCircle(int centerX, int centerY, int radius) {
	if (radius <= 0) return;
	
	if (mPainterMode == pmPixel) {
		Rasterizer::drawCircle(centerX, centerY, radius, setPixel);
	}
	else {
		int gridCenterX, gridCenterY, gridRadius;
		pixelToGrid(centerX, centerY, gridCenterX, gridCenterY);
		gridRadius = radius / gridSize;
		if (gridRadius <= 0) gridRadius = 1; // 确保最小半径为1
		
		Rasterizer::drawCircle(gridCenterX, gridCenterY, gridRadius, drawGridCell);
	}
}

void Painter::drawEllipse(int centerX, int centerY, int radiusX, int radiusY) {
	if (radiusX <= 0 || radiusY <= 0) return;

	if (mPainterMode == pmPixel) {
		Rasterizer::drawEllipse(centerX, centerY, radiusX, radiusY, setPixel);
	}
	else {
		int gridCenterX, gridCenterY, gridRadiusX, gridRadiusY;
		pixelToGrid(centerX, centerY, gridCenterX, gridCenterY);
		gridRadiusX = radiusX / gridSize;
		gridRadiusY = radiusY / gridSize;
		if (gridRadiusX <= 0) gridRadiusX = 1; 
		if (gridRadiusY <= 0) gridRadiusY = 1;
		
		Rasterizer::drawEllipse(gridCenterX, gridCenterY, gridRadiusX, gridRadiusY, drawGridCell);
	}
}


void Painter::pixelToGrid(int p_x0, int p_y0, int& g_x0, int& g_y0)
{
	g_x0 = p_x0 / gridSize;
	g_y0 = p_y0 / gridSize;
}
		
void Painter::drawGrid()
{
	if (mPainterMode != pmGrid) {
		return;
	}

	int windowWidth = getWindowWidth();
	int windowHeight = getWindowHeight();
	Color gridColor = _RGB(200, 200, 200);

	// 绘制水平网格线
	for (int y = 0; y <= windowHeight; y += gridSize) {
		for (int x = 0; x <= windowWidth; x++) {
			setPixel(x, y, gridColor);
		}
	}
	
	// 绘制垂直网格线
	for (int x = 0; x <= windowWidth; x += gridSize) {
		for (int y = 0; y <= windowHeight; y++) {
				setPixel(x, y, gridColor);
		}
	}
}

void drawGridCell(int x, int y, Color color)
{
	int x0 = x * g_Painter.gridSize;
	int y0 = y * g_Painter.gridSize;

	for (int i = 0; i < g_Painter.gridSize; ++i)
	{
		for (int j = 0; j < g_Painter.gridSize; ++j)
		{
			 setPixel(x0 + i, y0 + j, color);
		}
	}
}