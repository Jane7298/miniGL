#pragma once

#include <vector>
#include <algorithm>
#include "Graphic.h"
#include "Define.h"

using namespace std;
#include <string>

// 2D点
template<typename T>
struct _Point2D
{
	_Point2D(){ x = y = 0 ;}
	_Point2D( T x, T y ){ this->x = x, this->y = y; }

	T x,y;
};

typedef _Point2D<double> Point2D;

// 2D边界框
template<typename T>
struct _Box2D
{
	_Box2D(){ valid = false; }
	_Box2D( T xmin,T ymin , T xmax, T ymax )
	{
		setBox(xmin, ymin, xmax, ymax);
	}

	T width(){ return _xmax - _xmin; }// 边界框宽度
	T height(){ return _ymax - _ymin; }// 边界框高度
	T centerX(){ return (_xmin + _xmax) /2 ; }// 边界框中心点x坐标
	T centerY(){ return (_ymin + _ymax) /2 ; }// 边界框中心点y坐标
	T xmin(){ return _xmin;}
	T ymin(){ return _ymin;}
	T xmax(){ return _xmax;}
	T ymax(){ return _ymax;}

	void setBox( T xmin,T ymin , T xmax, T ymax )// 设置边界框范围
	{
		this->_xmin = xmin, this->_xmax = xmax, this->_ymin = ymin, this->_ymax = ymax ;
		valid = true;
	}

	void setBox( _Box2D& box )
	{
		setBox( box._xmin, box._ymin, box._xmax, box._ymax );
	}

	// 使边界框无效
	void invalidate()
	{
		valid = false;
	}

	// 根据添加的点扩展边界框
	void expand( T x, T y )
	{
		if( !valid ){
			_xmin = _xmax = x;
			_ymin = _ymax = y;
			valid = true;
		}
		else
		{
			_xmin = min( _xmin, x );
			_xmax = max( _xmax, x );
			_ymin = min( _ymin, y );
			_ymax = max( _ymax, y );
		}
	}

	// 根据添加的边界框扩展边界框
	void expand( _Box2D<T>& box ){
		if (box.valid)
		{
			if ( !valid )
				setBox(box);
			else
			{
				expand(box.xmin(), box.ymin());
				expand(box.xmax(), box.ymax());
			}
		}
	}

protected:
	T _xmin,_ymin;
	T _xmax,_ymax;
	bool valid;// 是否有效
};
typedef _Box2D<double> Box2D;

// 几何对象类型
enum GeomType{ gtUnkown = 0, gtPoint = 1, gtPolyline = 2, gtPolygon = 3 , gtCircle , gtEllipse };

// 几何对象基类，可继承
struct Geometry
{
	virtual ~Geometry(){}
	virtual GeomType getGeomType() =  0; // 获取几何对象类型
	virtual Box2D getEnvelop() = 0;// 获取几何对象边界框
	string label;
	int operationType; // 操作类型，用于区分绘制和填充模式
	
	Geometry() : operationType(0) {} // 构造函数初始化
};

// 点几何对象
struct PointGeometry:Geometry
{
	PointGeometry(  )
	{
		x = y = 0;
	}
	PointGeometry( double x, double y )
	{
		this->x= x;
		this->y = y;
	}

	virtual GeomType getGeomType(){ return gtPoint; }
	virtual Box2D getEnvelop(){ return Box2D( x,y,x,y ); }

	double x,y;
};

// 线几何对象
struct PolylineGeometry:Geometry
{
	virtual GeomType getGeomType(){ return gtPolyline; }

	// 数组重载，获取第i个点
	Point2D& operator[]( int i ){ return pts[i]; }

	// 添加点
	void addPoint( double x, double y )
	{
		pts.push_back( Point2D( x, y ));
		envelop.expand( x, y );
	}

	/// <summary>
	/// 刷新边界框
	/// </summary>
	void refreshEnvelop() 
	{
		envelop.invalidate();
		for (int i = 0; i < pts.size(); ++i)
		{
			envelop.expand(pts[i].x, pts[i].y);
		}
	}

	virtual Box2D getEnvelop(){ return envelop; }
	/*const vector<Point2D>& getPts() const 
	{
		return pts;
	}*/
	vector<Point2D>& getPts()
	{
		return pts;
	}
protected:
	// 所有点
	vector<Point2D> pts;
	Box2D envelop;
};

// 多边形几何对象
struct PolygonGeometry:PolylineGeometry
{
	virtual GeomType getGeomType(){ return gtPolygon; }
};

// 圆几何对象
struct CircleGeometry :Geometry
{
	CircleGeometry()
	{
		x = y = 0;
		r = 0;
	}
	CircleGeometry(double x, double y, double r )
	{
		this->x = x;
		this->y = y;
		this->r = r;
	}

	virtual GeomType getGeomType() { return gtCircle; }
	virtual Box2D getEnvelop() { return Box2D(x - r, y - r, x + r, y + r ); }

	double x, y;
	double r;
};

// 椭圆几何对象
struct EllipseGeometry :Geometry
{
	EllipseGeometry()
	{
		x1 = y1 = x2 = y2 = 0;
	}
	EllipseGeometry(double x1, double y1, double x2, double y2)
	{
		this->x1 = x1;
		this->y1 = y1;
		this->x2 = x2;
		this->y2 = y2;
	}

	virtual GeomType getGeomType() { return gtEllipse; }
	virtual Box2D getEnvelop() { return Box2D(x1, y1, x2, y2); }

	void getCenter(double& x, double& y) { x = (x1 + x2) * 0.5, y = (y1 + y2) * 0.5; }
	double getWidth() { return x2 - x1; }
	double getHeight() { return y2 - y1; }

	double x1, y1;
	double x2, y2;
};

//struct Symbol;
//struct SymbolGeometry: public Geometry
//{
//	SymbolGeometry( Geometry* pGeometry, Symbol* pSymbol )
//	{ 
//		this->pGeometry = pGeometry;
//		this->pSymbol = pSymbol;
//	}
//	virtual GeomType getGeomType(){ return pGeometry->getGeomType(); }
//
//	Geometry* pGeometry ;
//	Symbol* pSymbol;
//};

// 图层
struct Layer
{
	Layer()
	{
		geomType = gtUnkown;
	}

	Layer( GeomType geomType )
	{ 
		this->geomType = geomType;    
	}

	virtual ~Layer()
	{
		for( size_t i = 0 , size = geometrySet.size(); i < size ; ++i ) delete geometrySet[i];// 析构时删除所有几何对象
	}

	// 数组重载，返回第i个几何对象
	Geometry* operator[]( int i ){ return geometrySet[i]; }

	// 清空图层中的所有几何对象
	void clear()
	{
		for (size_t i = 0, size = geometrySet.size(); i < size; ++i) {
			delete geometrySet[i];
		}
		geometrySet.clear();
		envelop.setBox(0, 0, 0, 0); // 重置边界框
	}

	// 添加几何对象
	void addGeometry(Geometry* pGeometry, bool updateEnvelop = false )
	{
		geometrySet.push_back( pGeometry );
		if(updateEnvelop )
			envelop.expand( (Box2D&) pGeometry->getEnvelop());
	}

	// 设置图层范围
	void setEnvelop( double xmin,double ymin , double xmax, double ymax  )
	{
		envelop.setBox( xmin, ymin,xmax,ymax );
	}

	// 获取图层中几何对象数量
	int getGeometryCount(){ return geometrySet.size(); }

	vector<Geometry*> geometrySet;// 几何对象集合
	Box2D envelop;// 图层范围对应的边界框
	GeomType geomType;// 图层类型
	Color layerColor = BLACK;// 图层颜色
};

// 数据集
struct Dataset
{
	virtual ~Dataset()
	{
		for( size_t i = 0, size = layerSet.size() ; i < size ; ++i ) delete layerSet[i];// 析构时删除图层
	}

	// 数组重载，返回第i个图层
	Layer* operator[]( int i ){ return layerSet[i]; }

	int indexOf( Layer* pLayer ){
		for ( int i = 0 ; i < layerSet.size(); ++i )
		{
			if( pLayer = layerSet[i]) return i;
		}
		return -1;
	}

	// 获取图层数量
	int getLayerCount(){ return layerSet.size(); }	

	// 添加图层
	void addLayer( Layer* pLayer )
	{
		layerSet.push_back( pLayer );
	}

	// 图层集合
	vector<Layer*> layerSet;
};
