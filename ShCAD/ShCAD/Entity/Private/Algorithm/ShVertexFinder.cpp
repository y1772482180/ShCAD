
#include "ShVertexFinder.h"
#include "Base\ShMath.h"
#include "Entity\Leaf\ShLine.h"
#include "Entity\Leaf\ShCircle.h"
#include "Entity\Leaf\ShArc.h"
#include "Entity\Leaf\ShPoint.h"
#include "Entity\Leaf\ShDot.h"
#include "Entity\Leaf/ShBlock.h"
#include "Entity\Leaf/ShEllipse.h"
#include "Entity\Composite\Dim\ShDimLinear.h"
#include "Entity\Composite\Dim\ShDimAligned.h"
#include "Entity\Composite\Dim\ShDimRadius.h"
#include "Entity\Composite\Dim\ShDimDiameter.h"
#include "Entity\Composite\Dim\ShDimArcLength.h"
#include "Entity\Composite\Dim\ShDimAngular.h"
#include "Entity\Leaf\ShConstructionLine.h"


ShNearestVertexFinder::ShNearestVertexFinder(double x, double y, double zoomRate, VertexType &vertexType, ShPoint3d &vertexPoint, double tolerance)
	:x(x), y(y), zoomRate(zoomRate), vertexType(vertexType), vertexPoint(vertexPoint), tolerance(tolerance) {

}

ShNearestVertexFinder::~ShNearestVertexFinder() {

}

void ShNearestVertexFinder::visit(ShLine *line) {

	ShLineData data = line->getData();
	ShPoint3d start = data.start;
	ShPoint3d end = data.end;
	ShPoint3d mid = line->getMid();

	if (this->isNear(this->x, this->y, start, this->zoomRate, this->tolerance) == true) {
		this->vertexType = VertexType::VertexStart;
		this->vertexPoint = start;
		return;
	}
	
	if (this->isNear(this->x, this->y, end, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexEnd;
		this->vertexPoint = end;
		return;
	}

	if (this->isNear(this->x, this->y, mid, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexMid;
		this->vertexPoint = mid;
		return;
	}

	if (math::checkPointLiesOnLine(ShPoint3d(this->x, this->y), start, end, this->tolerance) == true)
		this->vertexType = VertexType::VertexOther;
	else
		this->vertexType = VertexType::VertexNothing;
}

void ShNearestVertexFinder::visit(ShCircle *circle) {

	ShPoint3d center = circle->getCenter();
	double radius = circle->getRadius();

	if (this->isNear(this->x, this->y, center, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexCenter;
		this->vertexPoint = center;
		return;
	}

	ShPoint3d right(center.x + radius, center.y);
	if (this->isNear(this->x, this->y, right, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexRight;
		this->vertexPoint = right;
		return;
	}

	ShPoint3d bottom(center.x, center.y - radius);
	if (this->isNear(this->x, this->y, bottom, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexBottom;
		this->vertexPoint = bottom;
		return;
	}

	ShPoint3d left(center.x - radius, center.y);
	if (this->isNear(this->x, this->y, left, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexLeft;
		this->vertexPoint = left;
		return;
	}

	ShPoint3d top(center.x, center.y + radius);
	if (this->isNear(this->x, this->y, top, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexTop;
		this->vertexPoint = top;
		return;
	}

	if (math::checkPointLiesOnCircleBoundary(ShPoint3d(this->x, this->y), center, radius, this->tolerance) == true)
		this->vertexType = VertexType::VertexOther;
	else
		this->vertexType = VertexType::VertexNothing;
}

void ShNearestVertexFinder::visit(ShArc *arc) {

	ShPoint3d center = arc->getCenter();

	if (this->isNear(this->x, this->y, center, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexCenter;
		this->vertexPoint = center;
		return;
	}

	ShPoint3d start = arc->getStart();

	if (this->isNear(this->x, this->y, start, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexStart;
		this->vertexPoint = start;
		return;
	}

	ShPoint3d end = arc->getEnd();

	if (this->isNear(this->x, this->y, end, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexEnd;
		this->vertexPoint = end;
		return;
	}

	ShPoint3d mid = arc->getMid();

	if (this->isNear(this->x, this->y, mid, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexMid;
		this->vertexPoint = mid;
		return;
	}

	if (math::checkPointLiesOnArcBoundary(ShPoint3d(this->x, this->y), center, arc->getRadius(), arc->getStartAngle(),
		arc->getEndAngle(), tolerance) == true)
		this->vertexType = VertexType::VertexOther;
	else
		this->vertexType = VertexType::VertexNothing;
}

void ShNearestVertexFinder::visit(ShPoint *point) {

	ShPoint3d position = point->getPosition();

	if (this->isNear(this->x, this->y, position, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexCenter;
		this->vertexPoint = position;
		return;
	}

	this->vertexType = VertexType::VertexNothing;
}

void ShNearestVertexFinder::visit(ShDot *dot) {

	ShPoint3d position = dot->getPosition();

	if (this->isNear(this->x, this->y, position, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexCenter;
		this->vertexPoint = position;
		return;
	}

	this->vertexType = VertexType::VertexNothing;
}

void ShNearestVertexFinder::visit(ShDimLinear *dimLinear) {

	ShDimLinearData data = dimLinear->getData();

	if (this->isNear(this->x, this->y, data.firstOrigin, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexFirstOrigin;
		this->vertexPoint = data.firstOrigin;
		return;
	}

	if (this->isNear(this->x, this->y, data.secondOrigin, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexSecondOrigin;
		this->vertexPoint = data.secondOrigin;
		return;
	}

	if (this->isNear(this->x, this->y, data.firstDim, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexFirstDim;
		this->vertexPoint = data.firstDim;
		return;
	}

	if (this->isNear(this->x, this->y, data.secondDim, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexSecondDim;
		this->vertexPoint = data.secondDim;
		return;
	}

	if (this->isNear(this->x, this->y, data.text, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexText;
		this->vertexPoint = data.text;
		return;
	}

	this->vertexType = VertexType::VertexNothing;

}

void ShNearestVertexFinder::visit(ShDimAligned *dimAligned) {

	ShDimAlignedData data = dimAligned->getData();

	if (this->isNear(this->x, this->y, data.firstOrigin, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexFirstOrigin;
		this->vertexPoint = data.firstOrigin;
		return;
	}

	if (this->isNear(this->x, this->y, data.secondOrigin, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexSecondOrigin;
		this->vertexPoint = data.secondOrigin;
		return;
	}

	if (this->isNear(this->x, this->y, data.firstDim, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexFirstDim;
		this->vertexPoint = data.firstDim;
		return;
	}

	if (this->isNear(this->x, this->y, data.secondDim, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexSecondDim;
		this->vertexPoint = data.secondDim;
		return;
	}

	if (this->isNear(this->x, this->y, data.text, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexText;
		this->vertexPoint = data.text;
		return;
	}

	this->vertexType = VertexType::VertexNothing;
}

void ShNearestVertexFinder::visit(ShDimRadius *dimRadius) {

	ShDimRadiusData data = dimRadius->getData();

	if (this->isNear(this->x, this->y, data.center, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexCenter;
		this->vertexPoint = data.center;
		return;
	}

	if (this->isNear(this->x, this->y, data.dim, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexDim;
		this->vertexPoint = data.dim;
		return;
	}

	if (this->isNear(this->x, this->y, data.text, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexText;
		this->vertexPoint = data.text;
		return;
	}
}

void ShNearestVertexFinder::visit(ShDimDiameter *dimDiameter) {

	ShDimDiameterData data = dimDiameter->getData();

	if (this->isNear(this->x, this->y, data.center, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexCenter;
		this->vertexPoint = data.center;
		return;
	}

	if (this->isNear(this->x, this->y, data.firstDim, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexFirstDim;
		this->vertexPoint = data.firstDim;
		return;
	}

	if (this->isNear(this->x, this->y, data.secondDim, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexSecondDim;
		this->vertexPoint = data.secondDim;
		return;
	}

	if (this->isNear(this->x, this->y, data.text, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexText;
		this->vertexPoint = data.text;
		return;
	}
}

void ShNearestVertexFinder::visit(ShDimArcLength *dimArcLength) {

	ShDimArcLengthData data = dimArcLength->getData();

	if (this->isNear(this->x, this->y, data.start, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexStart;
		this->vertexPoint = data.start;
		return;
	}

	if (this->isNear(this->x, this->y, data.end, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexEnd;
		this->vertexPoint = data.end;
		return;
	}

	if (this->isNear(this->x, this->y, data.boundary, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexBoundary;
		this->vertexPoint = data.boundary;
		return;
	}

	if (this->isNear(this->x, this->y, data.text, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexText;
		this->vertexPoint = data.text;
		return;
	}
}

void ShNearestVertexFinder::visit(ShDimAngular *dimAngular) {

	ShDimAngularData data = dimAngular->getData();

	if (this->isNear(this->x, this->y, data.center, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexCenter;
		this->vertexPoint = data.center;
		return;
	}

	if (this->isNear(this->x, this->y, data.start, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexStart;
		this->vertexPoint = data.start;
		return;
	}

	if (this->isNear(this->x, this->y, data.end, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexEnd;
		this->vertexPoint = data.end;
		return;
	}

	if (this->isNear(this->x, this->y, data.boundary, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexBoundary;
		this->vertexPoint = data.boundary;
		return;
	}

	if (this->isNear(this->x, this->y, data.text, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexText;
		this->vertexPoint = data.text;
		return;
	}
}

void ShNearestVertexFinder::visit(ShConstructionLine *constructionLine) {

	ShLineData data = constructionLine->getData();
	ShPoint3d start = data.start;
	ShPoint3d end = data.end;
	ShPoint3d mid = constructionLine->getMid();

	if (this->isNear(this->x, this->y, start, this->zoomRate, this->tolerance) == true) {
		this->vertexType = VertexType::VertexStart;
		this->vertexPoint = start;
		return;
	}

	if (this->isNear(this->x, this->y, end, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexEnd;
		this->vertexPoint = end;
		return;
	}

	if (this->isNear(this->x, this->y, mid, this->zoomRate, this->tolerance) == true) {

		this->vertexType = VertexType::VertexMid;
		this->vertexPoint = mid;
		return;
	}

	if (math::checkPointLiesOnInfiniteLine(ShPoint3d(this->x, this->y), start, end, this->tolerance) == true)
		this->vertexType = VertexType::VertexOther;
	else
		this->vertexType = VertexType::VertexNothing;

}

void ShNearestVertexFinder::visit(ShBlock* block) {
	// ��Ķ���������Ļ���
	ShPoint3d basePoint = block->getBasePoint();

	if (this->isNear(this->x, this->y, basePoint, this->zoomRate, this->tolerance)) {
		this->vertexType = VertexType::VertexCenter;  // ʹ��VertexCenter��ʾ��Ļ���
			this->vertexPoint = basePoint;
			return;
	}

	// ����Ƿ��ڿ��е��κ�ʵ����
	const QList<ShEntity*>& entities = block->getEntities();
		for (ShEntity* entity : entities) {
			ShNearestVertexFinder tempFinder(this->x, this->y, this->zoomRate, this->vertexType, this->vertexPoint, this->tolerance);
				entity->accept(&tempFinder);

				if (this->vertexType != VertexType::VertexNothing) {
					return;  // �ҵ�����ͷ���
				}
		}

	// û���ҵ��κζ���
	this->vertexType = VertexType::VertexNothing;
}

// ʵ�� ShNearestVertexFinder ����Բ���ʷ���
void ShNearestVertexFinder::visit(ShEllipse* ellipse) {
	ShPoint3d center = ellipse->getCenter();
	double majorRadius = ellipse->getMajorRadius();
	double minorRadius = ellipse->getMinorRadius();
	double angle = ellipse->getAngle();

	// ������ĵ�
	if (this->isNear(this->x, this->y, center, this->zoomRate, this->tolerance)) {
		this->vertexType = VertexType::VertexCenter;
		this->vertexPoint = center;
		return;
	}

	// ������Բ���ĸ���Ҫ���㣨������ת�Ƕȣ�
	double radAngle = math::degreeToRadian(angle);
	double cosAngle = cos(radAngle);
	double sinAngle = sin(radAngle);

	// �ĸ���Ҫ���㣨δ��תʱ�����꣩
	ShPoint3d points[4] = {
		{center.x + majorRadius, center.y, 0},  // �Ҷ˵�
		{center.x, center.y + minorRadius, 0},  // �϶˵�
		{center.x - majorRadius, center.y, 0},  // ��˵�
		{center.x, center.y - minorRadius, 0}   // �¶˵�
	};

	// Ӧ����ת
	for (int i = 0; i < 4; i++) {
		double x = points[i].x - center.x;
		double y = points[i].y - center.y;
		points[i].x = center.x + x * cosAngle - y * sinAngle;
		points[i].y = center.y + x * sinAngle + y * cosAngle;

		// ���ÿ������
		if (this->isNear(this->x, this->y, points[i], this->zoomRate, this->tolerance)) {
			this->vertexType = static_cast<VertexType>(VertexType::VertexRight << i);
			this->vertexPoint = points[i];
			return;
		}
	}

	// ����Ƿ�����Բ�߽���
	if (math::checkPointLiesOnEllipseBoundary(ShPoint3d(this->x, this->y), center,
		majorRadius, minorRadius, angle, this->tolerance)) {
		this->vertexType = VertexType::VertexOther;
	}
	else {
		this->vertexType = VertexType::VertexNothing;
	}
}


bool ShNearestVertexFinder::isNear(int x, int y, const ShPoint3d &point, double zoomRate, double tolerance) {

	if (x >= point.x - (tolerance / zoomRate) &&
		x <= point.x + (tolerance / zoomRate) &&
		y >= point.y - (tolerance / zoomRate) &&
		y <= point.y + (tolerance / zoomRate)) {

		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////

PointAndVertexTypeMathchedEntityFinder::PointAndVertexTypeMathchedEntityFinder(const ShPoint3d &mustMatchPoint, VertexType vertexType, bool &matched)
	:mustMatchPoint(mustMatchPoint), vertexType(vertexType), matched(matched) {
	
}

PointAndVertexTypeMathchedEntityFinder::~PointAndVertexTypeMathchedEntityFinder() {

}

void PointAndVertexTypeMathchedEntityFinder::visit(ShLine *line) {

	if ((this->vertexType & VertexType::VertexStart) == VertexType::VertexStart) {

		if (this->mustMatchPoint == line->getStart()) {

			this->matched = true;
			return;
		}
	}

	if ((this->vertexType & VertexType::VertexEnd) == VertexType::VertexEnd) {
		
		if (this->mustMatchPoint == line->getEnd()) {
		
			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexMid) == VertexType::VertexMid) {

		if (this->mustMatchPoint == line->getMid()) {

			this->matched = true;
			return;
		}
	}

	this->matched = false;
}

void PointAndVertexTypeMathchedEntityFinder::visit(ShCircle *circle) {

	if ((this->vertexType & VertexType::VertexCenter) == VertexType::VertexCenter) {

		if (this->mustMatchPoint == circle->getCenter()) {

			this->matched = true;
			return;
		}
	}

	this->matched = false;
}

void PointAndVertexTypeMathchedEntityFinder::visit(ShArc *arc) {

	if ((this->vertexType & VertexType::VertexStart) == VertexType::VertexStart) {

		if (this->mustMatchPoint == arc->getStart()) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexEnd) == VertexType::VertexEnd) {

		if (this->mustMatchPoint == arc->getEnd()) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexMid) == VertexType::VertexMid) {

		if (this->mustMatchPoint == arc->getMid()) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexCenter) == VertexType::VertexCenter) {

		if (this->mustMatchPoint == arc->getCenter()) {

			this->matched = true;
			return;
		}
	}

	this->matched = false;
}

void PointAndVertexTypeMathchedEntityFinder::visit(ShPoint *point) {

	if ((this->vertexType & VertexType::VertexCenter) == VertexType::VertexCenter) {

		if (this->mustMatchPoint == point->getPosition()) {

			this->matched = true;
			return;
		}
	}

	this->matched = false;
}

void PointAndVertexTypeMathchedEntityFinder::visit(ShDot *dot) {

	if ((this->vertexType & VertexType::VertexCenter) == VertexType::VertexCenter) {

		if (this->mustMatchPoint == dot->getPosition()) {

			this->matched = true;
			return;
		}
	}

	this->matched = false;
}

void PointAndVertexTypeMathchedEntityFinder::visit(ShDimLinear *dimLinear) {

	ShDimLinearData data = dimLinear->getData();

	if ((this->vertexType & VertexType::VertexFirstOrigin) == VertexType::VertexFirstOrigin) {

		if (this->mustMatchPoint == data.firstOrigin) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexSecondOrigin) == VertexType::VertexSecondOrigin) {

		if (this->mustMatchPoint == data.secondOrigin) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexFirstDim) == VertexType::VertexFirstDim) {

		if (this->mustMatchPoint == data.firstDim) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexSecondDim) == VertexType::VertexSecondDim) {

		if (this->mustMatchPoint == data.secondDim) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexText) == VertexType::VertexText) {

		if (this->mustMatchPoint == data.text) {

			this->matched = true;
			return;
		}
	}

	this->matched = false;
}

void PointAndVertexTypeMathchedEntityFinder::visit(ShDimAligned *dimAligned) {

	ShDimAlignedData data = dimAligned->getData();

	if ((this->vertexType & VertexType::VertexFirstOrigin) == VertexType::VertexFirstOrigin) {

		if (this->mustMatchPoint == data.firstOrigin) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexSecondOrigin) == VertexType::VertexSecondOrigin) {

		if (this->mustMatchPoint == data.secondOrigin) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexFirstDim) == VertexType::VertexFirstDim) {

		if (this->mustMatchPoint == data.firstDim) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexSecondDim) == VertexType::VertexSecondDim) {

		if (this->mustMatchPoint == data.secondDim) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexText) == VertexType::VertexText) {

		if (this->mustMatchPoint == data.text) {

			this->matched = true;
			return;
		}
	}

	this->matched = false;
}

void PointAndVertexTypeMathchedEntityFinder::visit(ShDimRadius *dimRadius) {

	ShDimRadiusData data = dimRadius->getData();

	if ((this->vertexType & VertexType::VertexCenter) == VertexType::VertexCenter) {

		if (this->mustMatchPoint == data.center) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexDim) == VertexType::VertexDim) {

		if (this->mustMatchPoint == data.dim) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexText) == VertexType::VertexText) {

		if (this->mustMatchPoint == data.text) {

			this->matched = true;
			return;
		}
	}

	this->matched = false;
}

void PointAndVertexTypeMathchedEntityFinder::visit(ShDimDiameter *dimDiameter) {

	ShDimDiameterData data = dimDiameter->getData();

	if ((this->vertexType & VertexType::VertexFirstDim) == VertexType::VertexFirstDim) {

		if (this->mustMatchPoint == data.firstDim) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexSecondDim) == VertexType::VertexSecondDim) {

		if (this->mustMatchPoint == data.secondDim) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexText) == VertexType::VertexText) {

		if (this->mustMatchPoint == data.text) {

			this->matched = true;
			return;
		}
	}

	this->matched = false;
}

void PointAndVertexTypeMathchedEntityFinder::visit(ShDimArcLength *dimArcLength) {

	ShDimArcLengthData data = dimArcLength->getData();

	if ((this->vertexType & VertexType::VertexStart) == VertexType::VertexStart) {

		if (this->mustMatchPoint == data.start) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexEnd) == VertexType::VertexEnd) {

		if (this->mustMatchPoint == data.end) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexText) == VertexType::VertexText) {

		if (this->mustMatchPoint == data.text) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexBoundary) == VertexType::VertexBoundary) {

		if (this->mustMatchPoint == data.boundary) {

			this->matched = true;
			return;
		}
	}

	this->matched = false;
}

void PointAndVertexTypeMathchedEntityFinder::visit(ShDimAngular *dimAngular) {

	ShDimAngularData data = dimAngular->getData();

	if ((this->vertexType & VertexType::VertexCenter) == VertexType::VertexCenter) {

		if (this->mustMatchPoint == data.center) {

			this->matched = true;
			return;
		}
	}

	if ((this->vertexType & VertexType::VertexStart) == VertexType::VertexStart) {

		if (this->mustMatchPoint == data.start) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexEnd) == VertexType::VertexEnd) {

		if (this->mustMatchPoint == data.end) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexText) == VertexType::VertexText) {

		if (this->mustMatchPoint == data.text) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexBoundary) == VertexType::VertexBoundary) {

		if (this->mustMatchPoint == data.boundary) {

			this->matched = true;
			return;
		}
	}

	this->matched = false;
}

void PointAndVertexTypeMathchedEntityFinder::visit(ShBlock* block) {
	// ֻƥ���Ļ���
	if ((this->vertexType & VertexType::VertexCenter) == VertexType::VertexCenter) {
		if (this->mustMatchPoint == block->getBasePoint()) {
			this->matched = true;
			return;
		}
	}

	// �����е�ʵ���Ƿ�ƥ��
	const QList<ShEntity*>& entities = block->getEntities();
	for (ShEntity* entity : entities) {
		PointAndVertexTypeMathchedEntityFinder tempFinder(this->mustMatchPoint, this->vertexType, this->matched);
		entity->accept(&tempFinder);

		if (this->matched) {
			return;  // �ҵ�ƥ��ͷ���
		}
	}

	// û���ҵ�ƥ��
	this->matched = false;
}

void PointAndVertexTypeMathchedEntityFinder::visit(ShEllipse* ellipse) {
	ShPoint3d center = ellipse->getCenter();

	// ������ĵ�ƥ��
	if ((this->vertexType & VertexType::VertexCenter) == VertexType::VertexCenter) {
		if (this->mustMatchPoint == center) {
			this->matched = true;
			return;
		}
	}

	// ����ĸ���Ҫ����ƥ��
	double majorRadius = ellipse->getMajorRadius();
	double minorRadius = ellipse->getMinorRadius();
	double angle = ellipse->getAngle();

	double radAngle = math::degreeToRadian(angle);
	double cosAngle = cos(radAngle);
	double sinAngle = sin(radAngle);

	ShPoint3d points[4] = {
		{center.x + majorRadius, center.y, 0},
		{center.x, center.y + minorRadius, 0},
		{center.x - majorRadius, center.y, 0},
		{center.x, center.y - minorRadius, 0}
	};

	for (int i = 0; i < 4; i++) {
		// Ӧ����ת
		double x = points[i].x - center.x;
		double y = points[i].y - center.y;
		points[i].x = center.x + x * cosAngle - y * sinAngle;
		points[i].y = center.y + x * sinAngle + y * cosAngle;

		// ��鶥������ƥ��
		VertexType pointType = static_cast<VertexType>(VertexType::VertexRight << i);
		if ((this->vertexType & pointType) == pointType) {
			if (this->mustMatchPoint == points[i]) {
				this->matched = true;
				return;
			}
		}
	}

	this->matched = false;
}

void PointAndVertexTypeMathchedEntityFinder::visit(ShConstructionLine *constructionLine) {

	if ((this->vertexType & VertexType::VertexStart) == VertexType::VertexStart) {

		if (this->mustMatchPoint == constructionLine->getStart()) {

			this->matched = true;
			return;
		}
	}

	if ((this->vertexType & VertexType::VertexEnd) == VertexType::VertexEnd) {

		if (this->mustMatchPoint == constructionLine->getEnd()) {

			this->matched = true;
			return;
		}
	}
	if ((this->vertexType & VertexType::VertexMid) == VertexType::VertexMid) {

		if (this->mustMatchPoint == constructionLine->getMid()) {

			this->matched = true;
			return;
		}
	}

	this->matched = false;
}