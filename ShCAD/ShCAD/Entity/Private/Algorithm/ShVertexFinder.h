

#ifndef _SHVERTEXFINDER_H
#define _SHVERTEXFINDER_H

#include "ShVisitor.h"
#include "Base\ShVariable.h"
#include "Data\ShPoint3d.h"

class ShNearestVertexFinder : public ShVisitor {

private:
	double x;
	double y;
	double zoomRate;
	VertexType &vertexType;
	ShPoint3d &vertexPoint;
	double tolerance;

public:
	ShNearestVertexFinder(double x, double y, double zoomRate, VertexType &vertexType, ShPoint3d &vertexPoint, double tolerance = 6.0);
	~ShNearestVertexFinder();

	virtual void visit(ShLine *line);
	virtual void visit(ShCircle *circle);
	virtual void visit(ShArc *arc);
	virtual void visit(ShPoint *point);
	virtual void visit(ShDot *dot);
	virtual void visit(ShDimLinear *dimLinear);
	virtual void visit(ShDimAligned *dimAligned);
	virtual void visit(ShDimRadius *dimRadius);
	virtual void visit(ShDimDiameter *dimDiameter);
	virtual void visit(ShDimArcLength *dimArcLength);
	virtual void visit(ShDimAngular *dimAngular);
	virtual void visit(ShConstructionLine *constructionLine);
	virtual void visit(ShBlock* block);  // 添加对块的访问方法
	virtual void visit(ShEllipse* ellipse);  // 新增：椭圆支持

private:
	bool isNear(int x, int y, const ShPoint3d &point, double zoomRate, double tolerance);

};

/////////////////////////////////////////////

class PointAndVertexTypeMathchedEntityFinder : public ShVisitor {

private:
	ShPoint3d mustMatchPoint;
	VertexType vertexType;
	bool &matched;

public:
	PointAndVertexTypeMathchedEntityFinder(const ShPoint3d &mustMatchPoint, VertexType vertexType, bool &matched);
	~PointAndVertexTypeMathchedEntityFinder();

	virtual void visit(ShLine *line);
	virtual void visit(ShCircle *circle);
	virtual void visit(ShArc *arc);
	virtual void visit(ShPoint *point);
	virtual void visit(ShDot *dot);
	virtual void visit(ShDimLinear *dimLinear);
	virtual void visit(ShDimAligned *dimAligned);
	virtual void visit(ShDimRadius *dimRadius);
	virtual void visit(ShDimDiameter *dimDiameter);
	virtual void visit(ShDimArcLength *dimArcLength);
	virtual void visit(ShDimAngular *dimAngular);
	virtual void visit(ShConstructionLine *constructionLine);
	virtual void visit(ShBlock* block);  // 添加对块的访问方法
	virtual void visit(ShEllipse* ellipse);  // 新增：椭圆支持

};


#endif //_SHVERTEXFINDER_H