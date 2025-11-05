#pragma once

#include "Graphic.h"
#include <vector>
#include <algorithm>

enum PainterMode{ pmPixel, pmGrid };

// 前向声明
class Padding;

//更高层的绘制工具
class Painter
{
public:
    Painter();
    ~Painter();

    void setPainterMode(PainterMode mode) { mPainterMode = mode; }
    void setGridSize(int gs) { gridSize = gs; }
    void setPenColor(Color col) { color = col; }

    PainterMode getPainterMode() const { return mPainterMode; }
    int getGridSize() const { return gridSize; }
    Color getPenColor() const { return color; }

    void drawLine(int x0, int y0, int x1, int y1);
    void drawPolygon(PixelPoint* pts, int count);
    void fillPolygon(PixelPoint* pts, int count);
    void fillRectangle(int x1, int y1, int x2, int y2);
    void fillCircle(int centerX, int centerY, int radius);
    void fillEllipse(int centerX, int centerY, int radiusX, int radiusY);
    void drawCircle(int centerX, int centerY, int radius);
    void drawEllipse(int centerX, int centerY, int radiusX, int radiusY);

    void pixelToGrid(int p_x0, int p_y0, int& g_x0, int& g_y0);

    void drawGrid();

    PainterMode mPainterMode;
    int gridSize;
    Color color;
    
private:
    Padding* padding; // 填充工具对象
};

extern Painter g_Painter;

void drawGridCell(int x, int y, Color color);