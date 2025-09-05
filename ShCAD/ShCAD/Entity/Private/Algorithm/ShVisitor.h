
#ifndef _SHVISITOR_H
#define _SHVISITOR_H

// Visitor Pattern

class ShLine;
class ShRubberBand;
class ShCircle;
class ShArc;
class ShDimLinear;
class ShPoint;
class ShDot;
class ShDimAligned;
class ShDimRadius;
class ShDimDiameter;
class ShDimArcLength;
class ShDimAngular;
class ShConstructionLine;
class ShEllipse;
class ShBlock;  // 新增块类的前向声明

class ShVisitor {

public:
	ShVisitor();
	virtual ~ShVisitor() = 0;

	virtual void visit(ShLine *line) {}
	virtual void visit(ShRubberBand *rubberBand) {}
	virtual void visit(ShCircle *circle) {}
	virtual void visit(ShArc *arc) {}
	virtual void visit(ShPoint *point) {}
	virtual void visit(ShDot *dot) {}
	virtual void visit(ShDimLinear *dimLinear) {}
	virtual void visit(ShDimAligned *dimAligned) {}
	virtual void visit(ShDimRadius *dimRadius) {}
	virtual void visit(ShDimDiameter *dimDiameter) {}
	virtual void visit(ShDimArcLength *dimArcLength) {}
	virtual void visit(ShDimAngular *dimAngular) {}
	virtual void visit(ShConstructionLine *constructionLine) {}
	virtual void visit(ShEllipse* ellipse) {}
	virtual void visit(ShBlock* block) {}  // 新增块访问方法

};


#endif //_SHVISITOR_H