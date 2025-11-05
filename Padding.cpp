#include "Padding.h"
#include "Rasterizer.h"
#include <algorithm>

Padding::Padding()
{
    // 默认使用setPixel作为像素处理回调
    pixelCallback = setPixel;
}

void Padding::setPixelCallback(PixelProcessCallback callback)
{
    pixelCallback = callback;
}

void Padding::fillPolygon(PixelPoint* pts, int count, Color fillColor)
{
    if (pts == nullptr || count < 3) return;
    
    // 使用扫描线填充算法
    fillPolygonScanline(pts, count, fillColor);
}

void Padding::fillPolygonScanline(PixelPoint* pts, int count, Color fillColor)
{
    if (pts == nullptr || count < 3) return;
    
    // 找到多边形的边界框
    int minY = pts[0].y, maxY = pts[0].y;
    for (int i = 1; i < count; i++) {
        if (pts[i].y < minY) minY = pts[i].y;
        if (pts[i].y > maxY) maxY = pts[i].y;
    }
    
    // 对每条扫描线进行填充
    for (int y = minY; y <= maxY; y++) {
        std::vector<int> intersections;
        getScanlineIntersections(pts, count, y, intersections);
        
        // 对交点进行排序
        std::sort(intersections.begin(), intersections.end());
        
        // 填充扫描线上的像素 - 成对处理交点
        // 如果交点数量为奇数，说明算法有问题，需要调试
        if (intersections.size() % 2 != 0) {
            // 交点数量为奇数，跳过这条扫描线并继续
            // 在实际应用中，这里应该记录错误或使用备用算法
            continue;
        }
        
        for (size_t i = 0; i < intersections.size(); i += 2) {
            if (i + 1 < intersections.size()) {
                int x1 = intersections[i];
                int x2 = intersections[i + 1];
                
                // 确保x1 <= x2
                if (x1 > x2) std::swap(x1, x2);
                
                // 填充从x1到x2的像素（包含端点）
                for (int x = x1; x <= x2; x++) {
                    pixelCallback(x, y, fillColor);
                }
            }
        }
    }
}

void Padding::getScanlineIntersections(PixelPoint* pts, int count, int scanline, std::vector<int>& intersections)
{
    intersections.clear();
    
    for (int i = 0; i < count; i++) {
        int next = (i + 1) % count;
        int y1 = pts[i].y;
        int y2 = pts[next].y;
        int x1 = pts[i].x;
        int x2 = pts[next].x;
        
        // 跳过水平边
        if (y1 == y2) continue;
        
        // 确保y1 < y2
        if (y1 > y2) {
            std::swap(y1, y2);
            std::swap(x1, x2);
        }
        
        // 简化的扫描线填充规则
        if (scanline >= y1 && scanline < y2) {
            // 使用线性插值计算交点
            int x = x1 + (scanline - y1) * (x2 - x1) / (y2 - y1);
            intersections.push_back(x);
        }
    }
}

void Padding::fillRectangle(int x1, int y1, int x2, int y2, Color fillColor)
{
    // 确保x1 <= x2, y1 <= y2
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);
    
    // 创建矩形的四个顶点
    PixelPoint rectPts[4] = {
        {x1, y1},  // 左上角
        {x2, y1},  // 右上角
        {x2, y2},  // 右下角
        {x1, y2}   // 左下角
    };
    
    // 使用多边形填充算法填充矩形
    fillPolygon(rectPts, 4, fillColor);
}

void Padding::fillCircle(int centerX, int centerY, int radius, Color fillColor)
{
    if (radius <= 0) return;
    
    // 使用x-扫描线算法填充圆
    int minY = centerY - radius;
    int maxY = centerY + radius;
    
    for (int y = minY; y <= maxY; y++) {
        // 计算当前扫描线与圆的交点
        int dy = y - centerY;
        int dySquared = dy * dy;
        int radiusSquared = radius * radius;
        
        // 如果 dy² > radius²，则扫描线不与圆相交
        if (dySquared > radiusSquared) continue;
        
        // 计算扫描线与圆的交点x坐标
        // 圆的方程：(x-cx)² + (y-cy)² = r²
        // 当y固定时：(x-cx)² = r² - (y-cy)²
        // x = cx ± sqrt(r² - (y-cy)²)
        int discriminant = radiusSquared - dySquared;
        if (discriminant < 0) continue;
        
        // 使用整数平方根近似计算
        int sqrtDiscriminant = 0;
        for (int i = 0; i * i <= discriminant; i++) {
            sqrtDiscriminant = i;
        }
        
        // 计算扫描线的起始和结束x坐标
        int x1 = centerX - sqrtDiscriminant;
        int x2 = centerX + sqrtDiscriminant;
        
        // 填充扫描线
        for (int x = x1; x <= x2; x++) {
            pixelCallback(x, y, fillColor);
        }
    }
}

void Padding::fillEllipse(int centerX, int centerY, int radiusX, int radiusY, Color fillColor)
{
    if (radiusX <= 0 || radiusY <= 0) return;
    
    // 使用x-扫描线算法填充椭圆
    int minY = centerY - radiusY;
    int maxY = centerY + radiusY;
    
    for (int y = minY; y <= maxY; y++) {
        // 计算当前扫描线与椭圆的交点
        int dy = y - centerY;
        int dySquared = dy * dy;
        int radiusYSquared = radiusY * radiusY;
        
        // 如果 dy² > radiusY²，则扫描线不与椭圆相交
        if (dySquared > radiusYSquared) continue;
        
        // 计算扫描线与椭圆的交点x坐标
        // 椭圆的方程：(x-cx)²/a² + (y-cy)²/b² = 1
        // 当y固定时：(x-cx)²/a² = 1 - (y-cy)²/b²
        // (x-cx)² = a² * (1 - (y-cy)²/b²)
        // x = cx ± a * sqrt(1 - (y-cy)²/b²)
        
        // 使用整数运算：a² * (b² - (y-cy)²) / b²
        long long numerator = (long long)radiusX * radiusX * (radiusYSquared - dySquared);
        long long denominator = radiusYSquared;
        
        if (numerator < 0) continue;
        
        // 计算平方根：sqrt(numerator / denominator)
        // 简化为：sqrt(numerator) / sqrt(denominator)
        // 由于denominator = radiusY²，所以sqrt(denominator) = radiusY
        long long discriminant = numerator;
        
        // 使用整数平方根近似计算
        int sqrtDiscriminant = 0;
        for (int i = 0; (long long)i * i <= discriminant; i++) {
            sqrtDiscriminant = i;
        }
        
        // 计算扫描线的起始和结束x坐标
        // x = cx ± sqrtDiscriminant / radiusY
        // 为了避免除法，我们使用近似：x ≈ cx ± sqrtDiscriminant / radiusY
        int x1 = centerX - sqrtDiscriminant / radiusY;
        int x2 = centerX + sqrtDiscriminant / radiusY;
        
        // 确保x1 <= x2
        if (x1 > x2) std::swap(x1, x2);
        
        // 填充扫描线
        for (int x = x1; x <= x2; x++) {
            pixelCallback(x, y, fillColor);
        }
    }
}
