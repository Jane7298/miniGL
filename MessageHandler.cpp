#include <windows.h>
#include "MessageHandler.h"
#include "resource.h"
#include "Graphic.h"
#include "GeoDefine.h"
#include "GeometryFactory.h"
#include "Rasterizer.h"
#include "Painter.h"

// 确保包含所有必要的头文件
#include <vector>
#include <algorithm>

enum OperationType {
	otNone, otDrawRectangle, otDrawRectangleOutline,
	otDrawLine, otDrawPolyline, otDrawPolygon, otDrawPolygonOutline,
	otFillPolygon, // 新增填充多边形操作类型
	otFillRectangle, // 新增填充矩形操作类型
	otFillCircle, // 新增填充圆操作类型
	otFillEllipse, // 新增填充椭圆操作类型
	otDrawCircle, otDrawEllipse,
	otDrawHoriLine, otDrawVertLine,
	otDrawLineDDA, otDrawLineBresenham,
	otClear
};

OperationType g_OperationType = otNone;//当前操作类型
Layer* g_pLayer = NULL;

///处理菜单消息
void handleMenuMessage(HWND hWnd, int menuID)
{
	switch (menuID)
	{
	case IDM_EXIT:
		PostQuitMessage(0);
		break;
	case ID_2D_DRAW_RECT:
		g_OperationType = otDrawRectangle;
		setRubberMode(rmRectangle);
		setCursor(csArrow);
		break;
	case ID_2D_DRAW_HORILINE:
		g_OperationType = otDrawHoriLine;
		setRubberMode(rmLine);
		setCursor(csArrow);
		break;
	case ID_2D_DRAW_VERTLINE:
		g_OperationType = otDrawVertLine;
		setRubberMode(rmLine);
		setCursor(csArrow);
		break;
	case ID_2D_DRAW_LINE_DDA:
		g_OperationType = otDrawLineDDA;
		setRubberMode(rmLine);
		setCursor(csArrow);
		break;
	case ID_2D_DRAW_LINE_BRESENHAM:
		g_OperationType = otDrawLineBresenham;
		setRubberMode(rmLine);
		setCursor(csArrow);
		break;
	case ID_2D_CLEAR:
		g_OperationType = otClear;
		if (g_pLayer != NULL) {
			g_pLayer->clear();
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	case ID_2D_DRAW_POLYLINE:
		g_OperationType = otDrawPolyline;
		setRubberMode(rmPolyline);
		setCursor(csCross);
		break;
	case ID_2D_DRAW_POLYGON:
		g_OperationType = otDrawPolygon;
		setRubberMode(rmPolygon);
		setCursor(csCross);
		break;
	case ID_2D_FILL_POLYGON:
		g_OperationType = otFillPolygon;
		setRubberMode(rmPolygon);
		setCursor(csCross);
		break;
	case ID_2D_FILL_RECTANGLE:
		g_OperationType = otFillRectangle;
		setRubberMode(rmRectangle);
		setCursor(csArrow);
		break;
	case ID_2D_FILL_CIRCLE:
		g_OperationType = otFillCircle;
		setRubberMode(rmLine);
		setCursor(csCross);
		break;
	case ID_2D_FILL_ELLIPSE:
		g_OperationType = otFillEllipse;
		setRubberMode(rmLine);
		setCursor(csCross);
		break;
	case ID_2D_DRAW_CIRCLE:
		g_OperationType = otDrawCircle;
		setRubberMode(rmLine);
		setCursor(csCross);
		break;
	case ID_2D_DRAW_ELLIPSE:
		g_OperationType = otDrawEllipse;
		setRubberMode(rmLine);
		setCursor(csCross);
		break;
	case ID_SET_PIXEL_MODE:
		g_Painter.setPainterMode(pmPixel);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case ID_SET_GRID_MODE:
		g_Painter.setPainterMode(pmGrid);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case ID_SET_GRID_SIZE_3:
		g_Painter.setGridSize(3);
		if (g_Painter.getPainterMode() == pmGrid) {
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	case ID_SET_GRID_SIZE_5:
		g_Painter.setGridSize(5);
		if (g_Painter.getPainterMode() == pmGrid) {
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	case ID_SET_GRID_SIZE_8:
		g_Painter.setGridSize(8);
		if (g_Painter.getPainterMode() == pmGrid) {
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	case ID_SET_GRID_SIZE_10:
		g_Painter.setGridSize(10);
		if (g_Painter.getPainterMode() == pmGrid) {
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	case ID_SET_COLOR_RED:
		g_Painter.setPenColor(RED);
		break;
	case ID_SET_COLOR_GREEN:
		g_Painter.setPenColor(GREEN);
		break;
	case ID_SET_COLOR_BLUE:
		g_Painter.setPenColor(BLUE);
		break;
	case ID_SET_COLOR_YELLOW:
		g_Painter.setPenColor(YELLOW);
		break;

	}
}

///处理键盘消息
void handleKeyMessage(int key)
{
	switch (key)
	{
	case VK_UP: // 上一行，上箭头			
	case VK_DOWN:
	case VK_LEFT:
	case VK_RIGHT:
	{
		//refreshWindow();
	}
	}
}

Geometry* createGeometry(OperationType operationType, vector<PixelPoint>& pts);

///处理鼠标消息
void handleMouseMessage(int message, int x, int y, int det)
{
	DPtToLPt(x, y, x, y);//将设备坐标转换为逻辑坐标

	switch (message)
	{
		case WM_LBUTTONDOWN:
		case WM_MOUSEMOVE:
		case WM_MOUSEWHEEL:
		{
			//refreshWindow();
		}
		break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		{
			if (getRubberMode() == rmNone) return ;//非橡皮筋模式，退出

			int c = getRubberPointCount();
			if (c < 2) return;//橡皮筋点数<2 ,无法形成有效图形，退出
			

			//橡皮筋操作完成时才会创建几何对象，判断橡皮筋操作是否完成
			switch (g_OperationType)
			{
			case otDrawLine:
			case otDrawHoriLine:
			case otDrawVertLine:
			case otDrawLineDDA:
			case otDrawLineBresenham:
			case otDrawCircle:
			case otDrawRectangle:
			case otDrawRectangleOutline:
			case otFillRectangle:
			case otFillCircle:
			case otFillEllipse:
			case otDrawEllipse:
				if (message == WM_RBUTTONUP)return;//右键完成，Line和Rectangle模式需要标志
				break;
			case otDrawPolyline:
			case otDrawPolygon:
			case otDrawPolygonOutline:
			case otFillPolygon:
				if (message == WM_LBUTTONUP)return;//左键完成，Polygon和Polyline模式需要标志
				break;
			default:
				return;
			}

			vector<PixelPoint> pts(c);//pts为动态数组，数组大小为c
			getRubberPoints(pts.data());//pts存储橡皮筋点集合

			//橡皮筋操作完成，根据橡皮筋点创建几何对象
			Geometry* pGeometry = createGeometry(g_OperationType, pts);
			if (pGeometry)
			{
				// 设置操作类型
				pGeometry->operationType = g_OperationType;
				g_pLayer->addGeometry(pGeometry);
				refreshWindow();
			}
		}
		break;
	}
}


void renderLayer(Layer* pLayer);
void display()
{
	setYUp(true);//y轴向上
	setOrig(0, getWindowHeight());// 原点为窗口左下角

	////画矩形
	//int cx = getWindowWidth() / 2;//窗口中心x坐标
	//int cy = getWindowHeight() / 2;//窗口中心y坐标
	//int gap = 200;//矩形边界到窗口中心的距离

	//drawLine(cx - gap, cy - gap, cx + gap, cy - gap);
	//drawLine(cx + gap, cy - gap, cx + gap, cy + gap);
	//drawLine(cx + gap, cy + gap, cx - gap, cy + gap);
	//drawLine(cx - gap, cy + gap, cx - gap, cy - gap);

	if (g_Painter.mPainterMode == pmGrid) {
		g_Painter.drawGrid();
	}

	renderLayer(g_pLayer);
}

///根据操作类型operationType和点集合pts创建对应的几何对象
Geometry* createGeometry(OperationType operationType, vector<PixelPoint>& pts)
{
	int count = pts.size();
	if (count < 2) return NULL;

	PixelPoint* pData = pts.data();

	///不同操作类型可以创建同一类型的图形，比如Rectange模式创建的图形也是多边形
	switch (operationType)
	{
	case otDrawHoriLine:
		return GeometryFactory::createPolylineGeometry(pData, count );
	case otDrawVertLine:
		return GeometryFactory::createPolylineGeometry(pData, count );
	case otDrawLineDDA:
		return GeometryFactory::createPolylineGeometry(pData, count );
	case otDrawLineBresenham:
		return GeometryFactory::createPolylineGeometry(pData, count );
	case otDrawRectangle:
		return GeometryFactory::creatRectangleGeometry(pData, count);
	case otDrawRectangleOutline:
		return GeometryFactory::creatRectangleOutlineGeometry(pData, count);
	case otDrawPolyline:
		return GeometryFactory::createPolylineGeometry(pData, count);
	case otDrawPolygon:
		return GeometryFactory::createPolygonGeometry(pData, count);
	case otDrawPolygonOutline:
		return GeometryFactory::createPolygonOutlineGeometry(pData, count);
	case otFillPolygon:
		return GeometryFactory::createPolygonGeometry(pData, count);
	case otFillRectangle:
		return GeometryFactory::creatRectangleGeometry(pData, count);
	case otFillCircle:
		return GeometryFactory::createCircleGeometry(pData, count);
	case otFillEllipse:
		return GeometryFactory::createEllipseGeometry(pData, count);
	case otDrawCircle:
		return GeometryFactory::createCircleGeometry(pData, count);
	case otDrawEllipse:
		return GeometryFactory::createEllipseGeometry(pData, count);
	}
	return NULL;
}

///绘制单个几何对象
void renderGeometry(Geometry* pGeometryDef)
{
	switch (pGeometryDef->getGeomType())
	{
	case gtPolyline:
	{
		PolylineGeometry* pGeometry = (PolylineGeometry*)pGeometryDef;
		const vector<Point2D>& pts = pGeometry->getPts();
		int opType = pGeometryDef->operationType;
		
		if (opType == otDrawHoriLine && pts.size() >= 2) {
			// 绘制水平线：保持y坐标不变，x坐标从起点到终点
			int y = pts[0].y;
			int x1 = pts[0].x;
			int x2 = pts[pts.size() - 1].x;
			if (x1 > x2) std::swap(x1, x2);
			g_Painter.drawLine(x1, y, x2, y);
		}
		else if (opType == otDrawVertLine && pts.size() >= 2) {
			// 绘制垂直线：保持x坐标不变，y坐标从起点到终点
			int x = pts[0].x;
			int y1 = pts[0].y;
			int y2 = pts[pts.size() - 1].y;
			if (y1 > y2) std::swap(y1, y2);
			g_Painter.drawLine(x, y1, x, y2);
		}
		else if (opType == otDrawLineDDA) {
			// 使用DDA算法绘制直线
			for (int i = 0, ptsCount = pts.size(); i < ptsCount - 1; ++i)
			{
				g_Painter.drawLine(pts[i].x, pts[i].y, pts[(i + 1)].x, pts[(i + 1)].y);
			}
		}
		/*else if (opType == otDrawLineBresenham) {
			// 使用Bresenham算法绘制直线
			for (int i = 0, ptsCount = pts.size(); i < ptsCount - 1; ++i)
			{
				g_Painter.drawLine(pts[i].x, pts[i].y, pts[(i + 1)].x, pts[(i + 1)].y);
			}
		}*/
		else if (opType == otDrawPolyline) {
			// 绘制折线：连接所有相邻的点
			for (int i = 0, ptsCount = pts.size(); i < ptsCount - 1; ++i)
			{
				g_Painter.drawLine(pts[i].x, pts[i].y, pts[(i + 1)].x, pts[(i + 1)].y);
			}
		}
	}
	break;
	case gtPolygon:
	{
		PolygonGeometry* pGeometry = (PolygonGeometry*)pGeometryDef;
		const vector<Point2D>& pts = pGeometry->getPts();
		size_t ptsCount = pts.size();
		vector <PixelPoint> _pts(ptsCount);
		for (int i = 0; i < ptsCount; ++i)
		{
			_pts[i].x = pts[i].x;
			_pts[i].y = pts[i].y;
		}
		
		// 根据操作类型决定是绘制多边形轮廓还是填充多边形
		if (pGeometryDef->operationType == otFillPolygon) {
			g_Painter.fillPolygon(_pts.data(), ptsCount);
		} else if (pGeometryDef->operationType == otFillRectangle) {
			// 填充矩形：从4个顶点中提取对角点
			if (ptsCount >= 4) {
				// 找到最小和最大的x、y坐标作为对角点
				int minX = _pts[0].x, maxX = _pts[0].x;
				int minY = _pts[0].y, maxY = _pts[0].y;
				
				for (int i = 1; i < ptsCount; i++) {
					if (_pts[i].x < minX) minX = _pts[i].x;
					if (_pts[i].x > maxX) maxX = _pts[i].x;
					if (_pts[i].y < minY) minY = _pts[i].y;
					if (_pts[i].y > maxY) maxY = _pts[i].y;
				}
				
				g_Painter.fillRectangle(minX, minY, maxX, maxY);
			}
		} else {
			g_Painter.drawPolygon(_pts.data(), ptsCount);
		}
	}
	break;
	case gtCircle:
	{
		CircleGeometry* pGeometry = (CircleGeometry*)pGeometryDef;

		// 根据操作类型决定是绘制圆轮廓还是填充圆
		if (pGeometryDef->operationType == otFillCircle) {
			g_Painter.fillCircle(pGeometry->x, pGeometry->y, pGeometry->r);
		} else {
			g_Painter.drawCircle(pGeometry->x, pGeometry->y, pGeometry->r);
		}
	}
	break;
	case gtEllipse:
	{
		EllipseGeometry* pGeometry = (EllipseGeometry*)pGeometryDef;

		double centerX = (pGeometry->x1 + pGeometry->x2) * 0.5;
		double centerY = (pGeometry->y1 + pGeometry->y2) * 0.5;
		int radiusX = abs(pGeometry->x2 - pGeometry->x1) / 2;
		int radiusY = abs(pGeometry->y2 - pGeometry->y1) / 2;
		
		// 根据操作类型决定是绘制椭圆轮廓还是填充椭圆
		if (pGeometryDef->operationType == otFillEllipse) {
			g_Painter.fillEllipse(centerX, centerY, radiusX, radiusY);
		} else {
			g_Painter.drawEllipse(centerX, centerY, radiusX, radiusY);
		}
	}
	break;
	}
}

void renderLayer(Layer* pLayer)
{
	setPenColor(pLayer->layerColor);
	for (int i = 0, size = pLayer->getGeometryCount(); i < size; ++i)
	{
		renderGeometry((*pLayer)[i]);
	}
}

#pragma region less-used 

///处理窗口大小变化消息
void sized(int cx, int cy)
{

}

///初始化
void initialize()
{
	g_pLayer = new Layer();
}

///程序退出时清理资源
void destroy()
{
	delete g_pLayer;
}

#pragma endregion

#pragma region handleMessage

///消息处理入口点
LRESULT handleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND://菜单消息
	{
		int menuID = LOWORD(wParam);
		handleMenuMessage(hWnd, menuID);
		return FALSE;
	}
	case WM_PAINT://绘制消息
		display();
		return FALSE;
	case WM_CREATE:
		init((unsigned)hWnd);
		initialize();
		return TRUE;
	case WM_DESTROY:
		destroy();
		PostQuitMessage(0);
		return TRUE;
	case WM_KEYDOWN://键盘消息
	case WM_SYSKEYDOWN:
	{
		int key = wParam;//获取按键码
		handleKeyMessage(key);
		return FALSE;
	}
	case WM_MOUSEMOVE://鼠标移动消息
	case WM_LBUTTONDOWN://鼠标左键按下消息	
	case WM_LBUTTONUP://鼠标左键抬起消息
	case WM_RBUTTONUP://鼠标右键抬起消息
	case WM_RBUTTONDOWN://鼠标右键按下消息
	case WM_LBUTTONDBLCLK://鼠标双击消息
	case WM_MOUSEWHEEL://鼠标滚轮消息
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		int det = message == WM_MOUSEWHEEL ? (short)HIWORD(wParam) : 0;
		handleMouseMessage(message, x, y, det);
		return FALSE;
	}
	case WM_SIZE://窗口大小变化消息
	{
		UINT cx = LOWORD(lParam);
		UINT cy = HIWORD(lParam);
		sized(cx, cy);
		return TRUE;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

#pragma endregion