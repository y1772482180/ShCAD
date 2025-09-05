
#include "ShSnapPointFinder.h"
#include "Entity\Leaf\ShLine.h"
#include "Base\ShMath.h"
#include "ShFootOfPerpendicularVisitor.h"
#include "Entity\Leaf\ShCircle.h"
#include "Entity\Leaf\ShArc.h"
#include "Entity\Leaf\ShPoint.h"
#include "Entity\Leaf\ShDot.h"
#include "Entity\Leaf\ShConstructionLine.h"
#include "Entity\Leaf\ShEllipse.h"  // ������������Բͷ�ļ�
#include "Entity\Leaf\ShBlock.h"  // ��ӿ��ͷ�ļ�
#include <float.h>  // ����DBL_MAX
#include"stdio.h"
ShSnapPointFinder::ShSnapPointFinder(ObjectSnap objectSnap, double x, double y, double &snapX, double &snapY, bool &isValid)
	:objectSnap(objectSnap), x(x), y(y), snapX(snapX), snapY(snapY), isValid(isValid), mode(Mode::Normal) {

}

ShSnapPointFinder::ShSnapPointFinder(ObjectSnap objectSnap, double x, double y, double &snapX, double &snapY, bool &isValid,
	double perpendicularX, double perpendicularY)
	: objectSnap(objectSnap), x(x), y(y), snapX(snapX), snapY(snapY), isValid(isValid), mode(Mode::FootOfPerpendicular),
	perpendicularX(perpendicularX), perpendicularY(perpendicularY) {


}


ShSnapPointFinder::~ShSnapPointFinder() {

}

void ShSnapPointFinder::visit(ShLine *line) {
	
	if (this->objectSnap == ObjectSnap::ObjectSnapEndPoint) {
	
		ShLineData data = line->getData();
		ShPoint3d start = data.start;
		ShPoint3d end = data.end;

		double disStart = math::getDistance(this->x, this->y, start.x, start.y);
		double disEnd = math::getDistance(this->x, this->y, end.x, end.y);

		if (math::compare(disStart, disEnd) == 1) {
			this->snapX = end.x;
			this->snapY = end.y;
		}
		else {
			this->snapX = start.x;
			this->snapY = start.y;
		}

		this->isValid = true;
		printf("%d,%d", start.x, start.y);
		return;
	
	}
	else if (this->objectSnap == ObjectSnap::ObjectSnapMidPoint) {

		ShPoint3d mid = line->getMid();

		this->snapX = mid.x;
		this->snapY = mid.y;
		this->isValid = true;
		printf("%d,%d", mid.x, mid.y);
		return;
	}
	else if (this->objectSnap == ObjectSnap::ObjectSnapPerpendicular) {

		double x, y;

		if (mode == Mode::Normal) {
			x = this->x;
			y = this->y;
		}
		else {
			x = this->perpendicularX;
			y = this->perpendicularY;
		}

		ShFootOfPerpendicularVisitor visitor(this->snapX, this->snapY, ShPoint3d(x, y));
		line->accept(&visitor);

		this->isValid = true;
		printf("%d,%d", x, y);
		return;

	}

	this->isValid = false;
}

void ShSnapPointFinder::visit(ShCircle *circle) {
	
	if (this->objectSnap == ObjectSnap::ObjectSnapQuadrant) {

		ShPoint3d center = circle->getCenter();
		double radius = circle->getRadius();

		double angle = math::getAbsAngle(center.x, center.y, this->x, this->y);

		//315 degree ~ 45 degree.
		if ((angle >= 315 && angle < 360) || (angle >= 0 && angle < 45)) {
			this->snapX = center.x + radius;
			this->snapY = center.y;
		}
		//45 degree ~ 135 degree.
		else if (angle >= 45 && angle < 135) {
			this->snapX = center.x;
			this->snapY = center.y + radius;
		}
		//135 degree ~ 225 degree
		else if (angle >= 135 && angle < 225) {
			this->snapX = center.x - radius;
			this->snapY = center.y;
		}
		//225 degree ~ 315 degree
		else {
			this->snapX = center.x;
			this->snapY = center.y - radius;
		}

		this->isValid = true;
		return;
	}
	else if (this->objectSnap == ObjectSnap::ObjectSnapCenter) {

		this->snapX = circle->getCenter().x;
		this->snapY = circle->getCenter().y;
		this->isValid = true;
		return;
	}
	else if (this->objectSnap == ObjectSnap::ObjectSnapPerpendicular) {

		double x, y;

		if (mode == Mode::Normal) {
			x = this->x;
			y = this->y;
		}
		else {
			x = this->perpendicularX;
			y = this->perpendicularY;
		}

		ShFootOfPerpendicularVisitor visitor(this->snapX, this->snapY, ShPoint3d(x, y));
		circle->accept(&visitor);

		this->isValid = true;
		return;
	}

	this->isValid = false;
}


void ShSnapPointFinder::visit(ShEllipse* ellipse) {
	if (this->objectSnap == ObjectSnap::ObjectSnapCenter) {
		// ��׽��Բ���ĵ�
		this->snapX = ellipse->getCenter().x;
		this->snapY = ellipse->getCenter().y;
		this->isValid = true;
		return;
	}
	//else if (this->objectSnap == ObjectSnap::ObjectSnapQuadrant) {
	//	// ��׽��Բ���޵㣨����Ͷ���Ķ˵㣩
	//	ShPoint3d center = ellipse->getCenter();
	//	double majorRadius = ellipse->getMajorRadius();
	//	double minorRadius = ellipse->getMinorRadius();
	//	double angle = ellipse->getAngle();

	//	// ������Բ���ĸ����޵㣨������ת�Ƕȣ�
	//	double radAngle = math::degreeToRadian(angle);
	//	double cosAngle = cos(radAngle);
	//	double sinAngle = sin(radAngle);

	//	// �ĸ����޵㣨δ��תʱ�����꣩
	//	ShPoint3d points[4] = {
	//		{center.x + majorRadius, center.y, 0},  // �Ҷ˵�
	//		{center.x, center.y + minorRadius, 0},   // �϶˵�
	//		{center.x - majorRadius, center.y, 0},  // ��˵�
	//		{center.x, center.y - minorRadius, 0}    // �¶˵�
	//	};

	//	// ��ת�������
	//	for (int i = 0; i < 4; i++) {
	//		double x = points[i].x - center.x;
	//		double y = points[i].y - center.y;
	//		points[i].x = center.x + x * cosAngle - y * sinAngle;
	//		points[i].y = center.y + x * sinAngle + y * cosAngle;
	//	}

	//	// �ҵ������������ĵ�
	//	double minDist = DBL_MAX;
	//	int closestIndex = 0;
	//	for (int i = 0; i < 4; i++) {
	//		double dist = math::getDistance(this->x, this->y, points[i].x, points[i].y);
	//		if (dist < minDist) {
	//			minDist = dist;
	//			closestIndex = i;
	//		}
	//	}

	//	this->snapX = points[closestIndex].x;
	//	this->snapY = points[closestIndex].y;
	//	this->isValid = true;
	//	return;
	//}
	//else if (this->objectSnap == ObjectSnap::ObjectSnapPerpendicular) {
	//	// ��׽����㣨���ô�����㹤�ߣ�
	//	double x, y;
	//	if (mode == Mode::Normal) {
	//		x = this->x;
	//		y = this->y;
	//	}
	//	else {
	//		x = this->perpendicularX;
	//		y = this->perpendicularY;
	//	}

	//	ShFootOfPerpendicularVisitor visitor(this->snapX, this->snapY, ShPoint3d(x, y));
	//	ellipse->accept(&visitor);

	//	this->isValid = true;
	//	return;
	//}

	this->isValid = false;  // ��֧��������׽ģʽ
}

void ShSnapPointFinder::visit(ShArc *arc) {

	if (this->objectSnap == ObjectSnap::ObjectSnapEndPoint) {

		double dis = math::getDistance(this->x, this->y, arc->getStart().x, arc->getStart().y);
		double dis2 = math::getDistance(this->x, this->y, arc->getEnd().x, arc->getEnd().y);

		if (math::compare(dis, dis2) == 1) {
			this->snapX = arc->getEnd().x;
			this->snapY = arc->getEnd().y;
		}
		else {
			this->snapX = arc->getStart().x;
			this->snapY = arc->getStart().y;
		}
		this->isValid = true;
		return;
	}
	else if (this->objectSnap == ObjectSnap::ObjectSnapMidPoint) {

		this->snapX = arc->getMid().x;
		this->snapY = arc->getMid().y;
		this->isValid = true;
		return;
	}
	else if (this->objectSnap == ObjectSnap::ObjectSnapCenter) {

		this->snapX = arc->getCenter().x;
		this->snapY = arc->getCenter().y;
		this->isValid = true;
		return;
	}
	else if (this->objectSnap == ObjectSnap::ObjectSnapQuadrant) {

		ShPoint3d center = arc->getCenter();
		double radius = arc->getRadius();

		double angle = math::getAbsAngle(center.x, center.y, this->x, this->y);
		double startAngle = arc->getStartAngle();
		double endAngle = arc->getEndAngle();

		//315 degree ~ 45 degree.
		if ((angle >= 315 && angle < 360) || (angle >= 0 && angle < 45)) {

			if (math::checkAngleLiesOnAngleBetween(startAngle, endAngle, 0) == true) {
				this->snapX = center.x + radius;
				this->snapY = center.y;
				this->isValid = true;
				return;
			}
		}
		//45 degree ~ 135 degree.
		else if (angle >= 45 && angle < 135) {
			if (math::checkAngleLiesOnAngleBetween(startAngle, endAngle, 90) == true) {
				this->snapX = center.x;
				this->snapY = center.y + radius;
				this->isValid = true;
				return;
			}
		}
		//135 degree ~ 225 degree
		else if (angle >= 135 && angle < 225) {
			if (math::checkAngleLiesOnAngleBetween(startAngle, endAngle, 180) == true) {
				this->snapX = center.x - radius;
				this->snapY = center.y;
				this->isValid = true;
				return;
			}
		}
		//225 degree ~ 315 degree
		else {
			if (math::checkAngleLiesOnAngleBetween(startAngle, endAngle, 270) == true) {
				this->snapX = center.x;
				this->snapY = center.y - radius;
				this->isValid = true;
				return;
			}
		}
	}
	else if (this->objectSnap == ObjectSnap::ObjectSnapPerpendicular) {

		double x, y;

		if (mode == Mode::Normal) {
			x = this->x;
			y = this->y;
		}
		else {
			x = this->perpendicularX;
			y = this->perpendicularY;
		}

		ShFootOfPerpendicularVisitor visitor(this->snapX, this->snapY, ShPoint3d(x, y));
		arc->accept(&visitor);

		this->isValid = true;
		return;
	}

	this->isValid = false;
}
void ShSnapPointFinder::visit(ShBlock* block) {
	if (this->objectSnap == ObjectSnap::ObjectSnapEndPoint ||
		this->objectSnap == ObjectSnap::ObjectSnapNode) {

		// ��ȡ��Ļ���
		const ShPoint3d& basePoint = block->getBasePoint();

		// ������λ���Ƿ�ӽ�����
		double distance = math::getDistance(this->x, this->y, basePoint.x, basePoint.y);
		if (distance < 10.0) {  // 10���ط�Χ�ڵĲ�׽��ֵ
			this->snapX = basePoint.x;
			this->snapY = basePoint.y;
			this->isValid = true;
			return;
		}
	}
	//else if (this->objectSnap == ObjectSnap::ObjectSnapCenter) {
	//	// �����������ʵ������ĵ�
	//	const QList<ShEntity*>& entities = block->getEntities();
	//	if (entities.isEmpty()) {
	//		this->isValid = false;
	//		return;
	//	}

	//	double minX = DBL_MAX, minY = DBL_MAX;
	//	double maxX = -DBL_MAX, maxY = -DBL_MAX;

	//	for (ShEntity* entity : entities) {
	//		ShBoundingBox3d bbox = entity->getBoundingBox();
	//		minX = qMin(minX, bbox.minX);
	//		minY = qMin(minY, bbox.minY);
	//		maxX = qMax(maxX, bbox.maxX);
	//		maxY = qMax(maxY, bbox.maxY);
	//	}

	//	this->snapX = (minX + maxX) / 2.0;
	//	this->snapY = (minY + maxY) / 2.0;
	//	this->isValid = true;
	//	return;
	//}
	//else if (this->objectSnap == ObjectSnap::ObjectSnapPerpendicular) {
	//	// �����㲶׽
	//	double x, y;

	//	if (mode == Mode::Normal) {
	//		x = this->x;
	//		y = this->y;
	//	}
	//	else {
	//		x = this->perpendicularX;
	//		y = this->perpendicularY;
	//	}

	//	// �ڿ���Ѱ�������ʵ����д������
	//	const QList<ShEntity*>& entities = block->getEntities();
	//	double minDistance = DBL_MAX;
	//	bool found = false;

	//	for (ShEntity* entity : entities) {
	//		ShFootOfPerpendicularVisitor visitor(this->snapX, this->snapY, ShPoint3d(x, y));
	//		entity->accept(&visitor);

	//		if (visitor.isValid()) {
	//			double dist = math::getDistance(x, y, visitor.getX(), visitor.getY());
	//			if (dist < minDistance) {
	//				minDistance = dist;
	//				this->snapX = visitor.getX();
	//				this->snapY = visitor.getY();
	//				found = true;
	//			}
	//		}
	//	}

	//	if (found) {
	//		this->isValid = true;
	//		return;
	//	}
	//}

	// ���û���ҵ����ʵĲ�׽��
	this->isValid = false;
}
void ShSnapPointFinder::visit(ShPoint *point) {

	if (this->objectSnap == ObjectSnap::ObjectSnapNode) {
	
		this->snapX = point->getPosition().x;
		this->snapY = point->getPosition().y;
		this->isValid = true;
		return;
	}

	this->isValid = false;
}

void ShSnapPointFinder::visit(ShDot *dot) {

	if (this->objectSnap == ObjectSnap::ObjectSnapNode) {

		this->snapX = dot->getPosition().x;
		this->snapY = dot->getPosition().y;
		this->isValid = true;
		return;
	}

	this->isValid = false;
}

void ShSnapPointFinder::visit(ShConstructionLine *constructionLine) {

	
	if (this->objectSnap == ObjectSnap::ObjectSnapMidPoint) {

		ShPoint3d mid = constructionLine->getMid();

		this->snapX = mid.x;
		this->snapY = mid.y;
		this->isValid = true;
		return;
	}
	else if (this->objectSnap == ObjectSnap::ObjectSnapPerpendicular) {

		double x, y;

		if (mode == Mode::Normal) {
			x = this->x;
			y = this->y;
		}
		else {
			x = this->perpendicularX;
			y = this->perpendicularY;
		}

		ShFootOfPerpendicularVisitor visitor(this->snapX, this->snapY, ShPoint3d(x, y));
		constructionLine->accept(&visitor);

		this->isValid = true;
		return;

	}

	this->isValid = false;
}