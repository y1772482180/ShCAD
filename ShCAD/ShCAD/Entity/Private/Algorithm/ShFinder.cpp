
#include "ShFinder.h"
#include "Base\ShMath.h"
#include "Entity\Leaf\ShLine.h"
#include "Entity\Leaf\ShCircle.h"
#include "Entity\Leaf\ShArc.h"
#include "Entity\Leaf\ShPoint.h"
#include "Entity\Leaf\ShDot.h"
#include "Base\ShPointStyle.h"
#include "Entity\Leaf\ShEllipse.h" 
#include "Entity/Leaf/ShBlock.h"
#include "Entity\Composite\Dim\ShDimLinear.h"
#include "Entity\Composite\Dim\ShDimAligned.h"
#include "Entity\Composite\Dim\ShDimRadius.h"
#include "Entity\Composite\Dim\ShDimDiameter.h"
#include "Entity\Composite\Dim\ShDimArcLength.h"
#include "Entity\Composite\Dim\ShDimAngular.h"
#include "Entity\Leaf\ShConstructionLine.h"

ShFinder::ShFinder(double x, double y, double zoomRate, ShEntity* *foundEntity, double tolerance)
	:x(x), y(y), zoomRate(zoomRate), foundEntity(foundEntity), tolerance(tolerance) {

}

ShFinder::~ShFinder() {

}

void ShFinder::visit(ShLine *line) {

	double tolerance = this->tolerance / this->zoomRate;

	ShLineData data = line->getData();

	if (math::checkPointLiesOnLine(ShPoint3d(this->x, this->y), data.start, data.end, tolerance) == true)
		*this->foundEntity = line;
}

void ShFinder::visit(ShCircle *circle) {

	double tolerance = this->tolerance / this->zoomRate;

	ShCircleData data = circle->getData();

	if (math::checkPointLiesOnCircleBoundary(ShPoint3d(this->x, this->y), data.center, data.radius, tolerance) == true)
		*this->foundEntity = circle;
}

void ShFinder::visit(ShArc *arc) {

	double tolerance = this->tolerance / this->zoomRate;

	ShArcData data = arc->getData();

	if (math::checkPointLiesOnArcBoundary(ShPoint3d(this->x, this->y), data.center, data.radius, data.startAngle, data.endAngle, tolerance) == true)
		*this->foundEntity = arc;
}
void ShFinder::visit(ShEllipse* ellipse) {
	double tolerance = this->tolerance / this->zoomRate;
	ShEllipseData data = ellipse->getData();

	// ʵ����Բ�߽����
	if (math::checkPointLiesOnEllipseBoundary(
		ShPoint3d(this->x, this->y),
		data.center,
		data.majorRadius,
		data.minorRadius,
		data.angle,
		tolerance) == true) {

		*this->foundEntity = ellipse;
	}
}
void ShFinder::visit(ShPoint *point) {

	QList<ShEntity*> list;
	ShPointStyle::getComponentDependsOnStyle(list, point);

	ShEntity *foundEntity = nullptr;
	ShFinder visitor(this->x, this->y, this->zoomRate, &foundEntity, this->tolerance);

	for (int i = 0; i < list.size(); i++) {
		
		list.at(i)->accept(&visitor);

		if (foundEntity != nullptr) {
			*this->foundEntity = point;
			break;
		}
	}


	while (list.isEmpty() == false)
		delete list.takeFirst();
}

void ShFinder::visit(ShDot *dot) {

	double tolerance = this->tolerance / this->zoomRate;

	double dis = math::getDistance(this->x, this->y, dot->getPosition().x, dot->getPosition().y);

	if (dis < tolerance) {
	
		*this->foundEntity = dot;
	}
}

void ShFinder::visit(ShDimLinear *dimLinear) {

	this->visitDim(dimLinear);
}

void ShFinder::visit(ShDimAligned *dimAligned) {

	this->visitDim(dimAligned);
}

void ShFinder::visit(ShDimRadius *dimRadius) {

	this->visitDim(dimRadius);
}

void ShFinder::visit(ShDimDiameter *dimDiameter) {

	this->visitDim(dimDiameter);
}

void ShFinder::visit(ShDimArcLength *dimArcLength) {

	this->visitDim(dimArcLength);
}

void ShFinder::visit(ShDimAngular *dimAngular) {

	this->visitDim(dimAngular);
}

void ShFinder::visit(ShConstructionLine *constructionLine) {

	double tolerance = this->tolerance / this->zoomRate;

	ShLineData data = constructionLine->getData();

	if (math::checkPointLiesOnInfiniteLine(ShPoint3d(this->x, this->y), data.start, data.end, tolerance) == true)
		*this->foundEntity = constructionLine;
}

// ���ShFinder��visit(ShBlock*)ʵ��
void ShFinder::visit(ShBlock* block) {
	ShEntity* foundEntity = nullptr;
	ShFinder visitor(this->x, this->y, this->zoomRate, &foundEntity, this->tolerance);

	// �ݹ�����е�ÿ��ʵ��
	for (ShEntity* entity : block->getEntities()) {
		entity->accept(&visitor);
		if (foundEntity != nullptr) {
			*this->foundEntity = block;
			break;
		}
	}
}

void ShFinder::visitDim(ShDim *dim) {

	ShEntity *foundEntity = nullptr;
	ShFinder visitor(this->x, this->y, this->zoomRate, &foundEntity, this->tolerance);

	auto itr = dim->begin();
	for (itr; itr != dim->end(); ++itr) {

		(*itr)->accept(&visitor);

		if (foundEntity != nullptr) {
			*this->foundEntity = dim;
			break;
		}
	}
}


//////////////////////////////////////////

ShRectFinder::ShRectFinder(const ShPoint3d &topLeft, const ShPoint3d &bottomRight, ShEntity* *foundEntity, FindMethod findMethod)
	:topLeft(topLeft), bottomRight(bottomRight), foundEntity(foundEntity), findMethod(findMethod) {

}

ShRectFinder::~ShRectFinder() {

}

void ShRectFinder::visit(ShLine *line) {

	ShLineData data = line->getData();

	if (math::checkPointLiesInsideRect(data.start, this->topLeft, this->bottomRight, 0) == true &&
		math::checkPointLiesInsideRect(data.end, this->topLeft, this->bottomRight, 0) == true) {
		*this->foundEntity = line;
		return;
	}

	if (findMethod == FindMethod::OnePartLiesInsideRect) {

		//Rectangle has four line-segment.
		ShPoint3d intersect;
		if (math::checkTwoLineSegmentsIntersect(data.start, data.end, this->topLeft, ShPoint3d(this->topLeft.x, this->bottomRight.y),
			intersect) == true) {
			*this->foundEntity = line;
			return;
		}
		if (math::checkTwoLineSegmentsIntersect(data.start, data.end, this->topLeft, ShPoint3d(this->bottomRight.x, this->topLeft.y),
			intersect) == true) {
			*this->foundEntity = line;
			return;
		}
		if (math::checkTwoLineSegmentsIntersect(data.start, data.end, ShPoint3d(this->topLeft.x, this->bottomRight.y), this->bottomRight,
			intersect) == true) {
			*this->foundEntity = line;
			return;
		}
		if (math::checkTwoLineSegmentsIntersect(data.start, data.end, ShPoint3d(this->bottomRight.x, this->topLeft.y), this->bottomRight,
			intersect) == true) {
			*this->foundEntity = line;
			return;
		}
	}
}
void ShRectFinder::visit(ShEllipse* ellipse) {
	ShEllipseData data = ellipse->getData();

	if (this->findMethod == FindMethod::AllPartLiesInsideRect) {
		// �����Բ�Ƿ���ȫ�ھ�����
		if (math::checkEllipseFullyInsideRect(
			data.center,
			data.majorRadius,
			data.minorRadius,
			data.angle,
			this->topLeft,
			this->bottomRight)) {

			*this->foundEntity = ellipse;
		}
	}
	else {
		// �����Բ�Ƿ�������ཻ
		if (math::checkEllipseIntersectsRect(
			data.center,
			data.majorRadius,
			data.minorRadius,
			data.angle,
			this->topLeft,
			this->bottomRight)) {

			*this->foundEntity = ellipse;
		}
	}
}
// ���ShRectFinder��visit(ShBlock*)ʵ��
void ShRectFinder::visit(ShBlock* block) {
	ShEntity* foundEntity = nullptr;
	int count = 0;
	ShRectFinder visitor(this->topLeft, this->bottomRight, &foundEntity, this->findMethod);

	if (this->findMethod == FindMethod::AllPartLiesInsideRect) {
		// ����������ʵ���Ƿ��ھ�����
		for (ShEntity* entity : block->getEntities()) {
			entity->accept(&visitor);
			if (foundEntity != nullptr) {
				count++;
				foundEntity = nullptr;
			}
		}
		if (count == block->getEntityCount()) {
			*this->foundEntity = block;
		}
	}
	else {
		// �������Ƿ�������ʵ��������ཻ
		for (ShEntity* entity : block->getEntities()) {
			entity->accept(&visitor);
			if (foundEntity != nullptr) {
				*this->foundEntity = block;
				break;
			}
		}
	}
}
void ShRectFinder::visit(ShCircle *circle) {

	ShCircleData data = circle->getData();

	if (data.center.x - data.radius >= this->topLeft.x &&
		data.center.x + data.radius <= this->bottomRight.x &&
		data.center.y - data.radius >= this->bottomRight.y &&
		data.center.y + data.radius <= this->topLeft.y) {
		*this->foundEntity = circle;
		return;
	}

	if (this->findMethod == FindMethod::OnePartLiesInsideRect) {

		if (math::checkCircleLineSegmentIntersect(data.center, data.radius, this->topLeft,
			ShPoint3d(this->topLeft.x, this->bottomRight.y)) == true) {
			*this->foundEntity = circle;
			return;
		}
		if (math::checkCircleLineSegmentIntersect(data.center, data.radius, this->topLeft,
			ShPoint3d(this->bottomRight.x, this->topLeft.y)) == true) {
			*this->foundEntity = circle;
			return;
		}
		if (math::checkCircleLineSegmentIntersect(data.center, data.radius,
			ShPoint3d(this->topLeft.x, this->bottomRight.y), this->bottomRight) == true) {
			*this->foundEntity = circle;
			return;
		}
		if (math::checkCircleLineSegmentIntersect(data.center, data.radius,
			ShPoint3d(this->bottomRight.x, this->topLeft.y), this->bottomRight) == true) {
			*this->foundEntity = circle;
			return;
		}
	}
}

void ShRectFinder::visit(ShArc *arc) {

	ShArcData data = arc->getData();

	if (math::checkPointLiesInsideRect(arc->getStart(), this->topLeft, this->bottomRight, 0) == true &&
		math::checkPointLiesInsideRect(arc->getEnd(), this->topLeft, this->bottomRight, 0) == true &&
		math::checkPointLiesInsideRect(arc->getMid(), this->topLeft, this->bottomRight, 0) == true) {

		ShPoint3d top(data.center.x, data.center.y + data.radius);
		ShPoint3d left(data.center.x + data.radius, data.center.y);
		ShPoint3d bottom(data.center.x, data.center.y - data.radius);
		ShPoint3d right(data.center.x - data.radius, data.center.y);


		bool flag = true;
		if (math::checkAngleLiesOnAngleBetween(data.startAngle, data.endAngle, 90.0) == true) {
			if (math::checkPointLiesInsideRect(top, this->topLeft, this->bottomRight, 0) == false)
				flag = false;
		}
		if (math::checkAngleLiesOnAngleBetween(data.startAngle, data.endAngle, 0) == true) {
			if (flag == true && math::checkPointLiesInsideRect(left, this->topLeft, this->bottomRight, 0) == false)
				flag = false;
		}
		if (math::checkAngleLiesOnAngleBetween(data.startAngle, data.endAngle, 270) == true) {
			if (flag == true && math::checkPointLiesInsideRect(bottom, this->topLeft, this->bottomRight, 0) == false)
				flag = false;
		}
		if (math::checkAngleLiesOnAngleBetween(data.startAngle, data.endAngle, 180) == true) {
			if (flag == true && math::checkPointLiesInsideRect(right, this->topLeft, this->bottomRight, 0) == false)
				flag = false;
		}

		if (flag == true) {
			*this->foundEntity = arc;
			return;
		}
	}

	if (this->findMethod == FindMethod::OnePartLiesInsideRect) {

		if (math::checkArcLineSegmentIntersect(data.center, data.radius, data.startAngle, data.endAngle,
			this->topLeft, ShPoint3d(this->topLeft.x, this->bottomRight.y)) == true) {
			*this->foundEntity = arc;
			return;
		}
		if (math::checkArcLineSegmentIntersect(data.center, data.radius, data.startAngle, data.endAngle,
			this->topLeft, ShPoint3d(this->bottomRight.x, this->topLeft.y)) == true) {
			*this->foundEntity = arc;
			return;
		}
		if (math::checkArcLineSegmentIntersect(data.center, data.radius, data.startAngle, data.endAngle,
			ShPoint3d(this->topLeft.x, this->bottomRight.y), this->bottomRight) == true) {
			*this->foundEntity = arc;
			return;
		}
		if (math::checkArcLineSegmentIntersect(data.center, data.radius, data.startAngle, data.endAngle,
			ShPoint3d(this->bottomRight.x, this->topLeft.y), this->bottomRight) == true) {
			*this->foundEntity = arc;
			return;
		}
	}
}

void ShRectFinder::visit(ShPoint *point) {

	QList<ShEntity*> list;
	ShPointStyle::getComponentDependsOnStyle(list, point);

	ShEntity *foundEntity = nullptr;
	int count = 0;
	ShRectFinder visitor(this->topLeft, this->bottomRight, &foundEntity, this->findMethod);


	if (this->findMethod == FindMethod::AllPartLiesInsideRect) {

		for (int i = 0; i < list.size(); i++) {

			list.at(i)->accept(&visitor);

			if (foundEntity != nullptr) {
				count++;
				foundEntity = nullptr;
				
			}
		}

		if (count == list.size())
			*this->foundEntity = point;
	}
	else {

		for (int i = 0; i < list.size(); i++) {

			list.at(i)->accept(&visitor);

			if (foundEntity != nullptr) {
				*this->foundEntity = point;
				break;
			}
		}
	}

	while (list.isEmpty() == false)
		delete list.takeFirst();
}

void ShRectFinder::visit(ShDot *dot) {

	if (math::checkPointLiesInsideRect(dot->getPosition(), this->topLeft, this->bottomRight, 0) == true)
		*this->foundEntity = dot;

}

void ShRectFinder::visit(ShDimLinear *dimLinear) {

	this->visitDim(dimLinear);
}

void ShRectFinder::visit(ShDimAligned *dimAligned) {

	this->visitDim(dimAligned);
}

void ShRectFinder::visit(ShDimRadius *dimRadius) {

	this->visitDim(dimRadius);
}

void ShRectFinder::visit(ShDimDiameter *dimDiameter) {

	this->visitDim(dimDiameter);
}

void ShRectFinder::visit(ShDimArcLength *dimArcLength) {

	this->visitDim(dimArcLength);
}

void ShRectFinder::visit(ShDimAngular *dimAngular) {

	this->visitDim(dimAngular);
}

void ShRectFinder::visit(ShConstructionLine *constructionLine) {

	if (this->findMethod == FindMethod::OnePartLiesInsideRect) {

		ShLineData data = constructionLine->getData();

		//Rectangle has four line-segment.
		ShPoint3d intersect;
		if (math::checkLineLineIntersect(data.start, data.end, this->topLeft, ShPoint3d(this->topLeft.x, this->bottomRight.y),
			intersect) == true) {

			if (math::checkPointLiesOnLine(intersect, this->topLeft, ShPoint3d(this->topLeft.x, this->bottomRight.y), 1) == true) {
				*this->foundEntity = constructionLine;
				return;
			}
		}
		if (math::checkLineLineIntersect(data.start, data.end, this->topLeft, ShPoint3d(this->bottomRight.x, this->topLeft.y),
			intersect) == true) {

			if (math::checkPointLiesOnLine(intersect, this->topLeft, ShPoint3d(this->bottomRight.x, this->topLeft.y), 1) == true) {
				*this->foundEntity = constructionLine;
				return;
			}
		}
		if (math::checkLineLineIntersect(data.start, data.end, ShPoint3d(this->topLeft.x, this->bottomRight.y), this->bottomRight,
			intersect) == true) {

			if (math::checkPointLiesOnLine(intersect, ShPoint3d(this->topLeft.x, this->bottomRight.y), this->bottomRight, 1) == true) {
				*this->foundEntity = constructionLine;
				return;
			}
		}
		if (math::checkLineLineIntersect(data.start, data.end, ShPoint3d(this->bottomRight.x, this->topLeft.y), this->bottomRight,
			intersect) == true) {
			
			if (math::checkPointLiesOnLine(intersect, ShPoint3d(this->bottomRight.x, this->topLeft.y), this->bottomRight, 1) == true) {
				*this->foundEntity = constructionLine;
				return;
			}
		}
	}
}

void ShRectFinder::visitDim(ShDim *dim) {

	ShEntity *foundEntity = nullptr;
	int count = 0;
	ShRectFinder visitor(this->topLeft, this->bottomRight, &foundEntity, this->findMethod);
	auto itr = dim->begin();

	if (this->findMethod == FindMethod::AllPartLiesInsideRect) {

		for (itr; itr != dim->end(); ++itr) {

			(*itr)->accept(&visitor);

			if (foundEntity != nullptr) {
				count++;
				foundEntity = nullptr;
			}
		}

		if (count == dim->getSize())
			*this->foundEntity = dim;

	}
	else {

		for (itr; itr != dim->end(); ++itr) {

			(*itr)->accept(&visitor);

			if (foundEntity != nullptr) {
				*this->foundEntity = dim;
				return;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////
