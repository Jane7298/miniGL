#pragma once

#include "Graphic.h"
#include <vector>
#include <algorithm>

// 像素处理回调函数类型
typedef void (*PixelProcessCallback)(int x, int y, Color color);

// 多边形填充工具类
class Padding
{
public:
    Padding();
    
    // 填充多边形
    void fillPolygon(PixelPoint* pts, int count, Color fillColor);
    
    // 填充矩形
    void fillRectangle(int x1, int y1, int x2, int y2, Color fillColor);
    
    // 填充圆
    void fillCircle(int centerX, int centerY, int radius, Color fillColor);
    
    // 填充椭圆
    void fillEllipse(int centerX, int centerY, int radiusX, int radiusY, Color fillColor);
    
    // 设置像素处理回调函数
    void setPixelCallback(PixelProcessCallback callback);
    
private:
    // 扫描线填充算法
    void fillPolygonScanline(PixelPoint* pts, int count, Color fillColor);
    
    // 获取扫描线与多边形的交点
    void getScanlineIntersections(PixelPoint* pts, int count, int scanline, std::vector<int>& intersections);
    
    // 设置像素的函数指针
    PixelProcessCallback pixelCallback;
};
