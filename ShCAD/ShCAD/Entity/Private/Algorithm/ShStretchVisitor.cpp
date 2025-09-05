
#include "ShStretchVisitor.h"
#include "Base\ShMath.h"
#include "Entity\Leaf\ShLine.h"
#include "Entity\Leaf\ShCircle.h"
#include "Entity\Leaf\ShArc.h"
#include "Entity\Leaf\ShPoint.h"
#include "Entity\Leaf\ShDot.h"
#include "Entity\Leaf/ShEllipse.h"
#include "Entity\Leaf/ShBlock.h"
#include "Entity\Composite\Dim\ShDimLinear.h"
#include "Entity\Composite\Dim\ShDimAligned.h"
#include "Entity\Composite\Dim\ShDimRadius.h"
#include "Entity\Composite\Dim\ShDimDiameter.h"
#include "Entity\Composite\Dim\ShDimArcLength.h"
#include "Entity\Composite\Dim\ShDimAngular.h"
#include "Entity\Leaf\ShConstructionLine.h"

ShStretchData::ShStretchData() {

}

ShStretchData::~ShStretchData() {

}

///////////////////////////////////////////////

ShStretchLeafData::ShStretchLeafData(StretchPoint stretchPoint)
	:stretchPoint(stretchPoint) {

}

ShStretchLeafData::~ShStretchLeafData() {

}

////////////////////////////////////////////////


ShStretchVisitor::ShStretchVisitor(const ShPoint3d &base, const ShPoint3d &current)
	:base(base), current(current), original(nullptr), stretchData(nullptr) {

}

ShStretchVisitor::~ShStretchVisitor() {

}

void ShStretchVisitor::visit(ShLine *line) {

	if (this->original == nullptr || !dynamic_cast<ShLine*>(this->original))
		return;

	if (!dynamic_cast<ShStretchLeafData*>(this->stretchData))
		return;

	ShStretchLeafData *stretchData = dynamic_cast<ShStretchLeafData*>(this->stretchData);

	ShLine *original = dynamic_cast<ShLine*>(this->original);
	ShLineData data = original->getData();

	double disX = this->current.x - this->base.x;
	double disY = this->current.y - this->base.y;

	if (stretchData->stretchPoint == StretchPoint::StretchStart) {
		data.start.move(disX, disY);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchEnd) {
		data.end.move(disX, disY);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchMove) {

		data.start.move(disX, disY);
		data.end.move(disX, disY);
	}


	line->setData(data);
}

void ShStretchVisitor::visit(ShCircle *circle) {

	if (this->original == nullptr || !dynamic_cast<ShCircle*>(this->original))
		return;

	if (!dynamic_cast<ShStretchLeafData*>(this->stretchData))
		return;

	ShStretchLeafData *stretchData = dynamic_cast<ShStretchLeafData*>(this->stretchData);


	ShCircle *original = dynamic_cast<ShCircle*>(this->original);

	ShCircleData data = original->getData();
	ShPoint3d center = data.center;

	double disX = this->current.x - this->base.x;
	double disY = this->current.y - this->base.y;

	if (stretchData->stretchPoint == StretchPoint::StretchMove) {

		data.center.move(disX, disY);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchRight ||
		stretchData->stretchPoint == StretchPoint::StretchBottom ||
		stretchData->stretchPoint == StretchPoint::StretchLeft ||
		stretchData->stretchPoint == StretchPoint::StretchTop) {

		double radius = math::getDistance(center.x, center.y, this->current.x, this->current.y);
		data.radius = radius;
	}

	circle->setData(data);
}

void ShStretchVisitor::visit(ShArc *arc) {


	if (this->original == nullptr || !dynamic_cast<ShArc*>(this->original))
		return;

	if (!dynamic_cast<ShStretchLeafData*>(this->stretchData))
		return;

	ShStretchLeafData *stretchData = dynamic_cast<ShStretchLeafData*>(this->stretchData);

	ShArc *original = dynamic_cast<ShArc*>(this->original);
	ShArcData data = original->getData();

	double disX = this->current.x - this->base.x;
	double disY = this->current.y - this->base.y;

	if (stretchData->stretchPoint == StretchPoint::StretchMove) {

		data.center.move(disX, disY);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchStart ||
		stretchData->stretchPoint == StretchPoint::StretchEnd ||
		stretchData->stretchPoint == StretchPoint::StretchMid) {

		ShPoint3d start = original->getStart();
		ShPoint3d end = original->getEnd();
		ShPoint3d mid = original->getMid();
		ShPoint3d center;

		if (stretchData->stretchPoint == StretchPoint::StretchStart) {

			start.x += disX;
			start.y += disY;
		}
		else if (stretchData->stretchPoint == StretchPoint::StretchEnd) {

			end.x += disX;
			end.y += disY;
		}
		else if (stretchData->stretchPoint == StretchPoint::StretchMid) {

			mid.x += disX;
			mid.y += disY;
		}

		if (math::getCenterWithThreePoint(start, mid, end, center) == false)
			return;

		double startAngle = math::getAbsAngle(center.x, center.y, start.x, start.y);
		double endAngle = math::getAbsAngle(center.x, center.y, end.x, end.y);
		double midAngle = math::getAbsAngle(center.x, center.y, mid.x, mid.y);
		double radius = math::getDistance(center.x, center.y, start.x, start.y);

		data.center = center;
		data.radius = radius;

		if (math::checkAngleLiesOnAngleBetween(startAngle, endAngle, midAngle) == true) {
			data.startAngle = startAngle;
			data.endAngle = endAngle;
		}
		else {
			data.startAngle = endAngle;
			data.endAngle = startAngle;
		}

	}

	arc->setData(data);
}

void ShStretchVisitor::visit(ShPoint *point) {

	if (this->original == nullptr || !dynamic_cast<ShPoint*>(this->original))
		return;

	if (!dynamic_cast<ShStretchLeafData*>(this->stretchData))
		return;

	ShStretchLeafData *stretchData = dynamic_cast<ShStretchLeafData*>(this->stretchData);

	ShPoint *original = dynamic_cast<ShPoint*>(this->original);
	ShPoint3d position = original->getPosition();

	double disX = this->current.x - this->base.x;
	double disY = this->current.y - this->base.y;

	if (stretchData->stretchPoint == StretchPoint::StretchMove) {

		position.move(disX, disY);
		point->setPosition(position);
	}
}

void ShStretchVisitor::visit(ShDot *dot) {

	if (this->original == nullptr || !dynamic_cast<ShDot*>(this->original))
		return;

	if (!dynamic_cast<ShStretchLeafData*>(this->stretchData))
		return;

	ShStretchLeafData *stretchData = dynamic_cast<ShStretchLeafData*>(this->stretchData);

	ShDot *original = dynamic_cast<ShDot*>(this->original);
	ShPoint3d position = original->getPosition();

	double disX = this->current.x - this->base.x;
	double disY = this->current.y - this->base.y;

	if (stretchData->stretchPoint == StretchPoint::StretchMove) {

		position.move(disX, disY);
		dot->setPosition(position);
	}
}

void ShStretchVisitor::visit(ShDimLinear *dimLinear) {

	if (this->original == nullptr || !dynamic_cast<ShDimLinear*>(this->original))
		return;

	if (!dynamic_cast<ShStretchLeafData*>(this->stretchData))
		return;

	ShStretchLeafData *stretchData = dynamic_cast<ShStretchLeafData*>(this->stretchData);

	ShDimLinear *original = dynamic_cast<ShDimLinear*>(this->original);
	ShDimLinearData data = original->getData();

	if (stretchData->stretchPoint == StretchPoint::StretchFirstOrigin) {
	
		double angle = math::getAbsAngle(data.firstOrigin.x, data.firstOrigin.y, data.firstDim.x, data.firstDim.y);
		data.firstOrigin = this->current;

		ShPoint3d rotate;
		math::rotate(angle, data.firstOrigin.x, data.firstOrigin.y, data.firstOrigin.x + 10, data.firstOrigin.y, rotate.x, rotate.y);
		math::checkLineLineIntersect(data.firstOrigin, rotate, data.firstDim, data.secondDim, data.firstDim);

		dimLinear->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchFirstDim) {
	
		double dimDistance = math::getDistance(data.firstDim.x, data.firstDim.y, data.text.x, data.text.y);
		double dimAngle = math::getAbsAngle(data.firstDim.x, data.firstDim.y, data.text.x, data.text.y);

		double angle = math::getAbsAngle(data.firstDim.x, data.firstDim.y, data.secondDim.x, data.secondDim.y);
		ShPoint3d rotate;

		math::rotate(angle, this->current.x, this->current.y, this->current.x + 10, this->current.y, rotate.x, rotate.y);
		math::checkLineLineIntersect(data.firstOrigin, data.firstDim, this->current, rotate, data.firstDim);

		math::rotate(angle, data.firstDim.x, data.firstDim.y, data.firstDim.x + 10, data.firstDim.y, rotate.x, rotate.y);
		math::checkLineLineIntersect(data.firstDim, rotate, data.secondOrigin, data.secondDim, data.secondDim);

		math::rotate(dimAngle, data.firstDim.x, data.firstDim.y, data.firstDim.x + dimDistance, data.firstDim.y, data.text.x, data.text.y);

		dimLinear->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchSecondOrigin) {
	
		double angle = math::getAbsAngle(data.secondOrigin.x, data.secondOrigin.y, data.secondDim.x, data.secondDim.y);
		data.secondOrigin = this->current;

		ShPoint3d rotate;
		math::rotate(angle, data.secondOrigin.x, data.secondOrigin.y, data.secondOrigin.x + 10, data.secondOrigin.y, rotate.x, rotate.y);
		math::checkLineLineIntersect(data.secondOrigin, rotate, data.secondDim, data.firstDim, data.secondDim);

		dimLinear->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchSecondDim) {
	
		double dimDistance = math::getDistance(data.firstDim.x, data.firstDim.y, data.text.x, data.text.y);
		double dimAngle = math::getAbsAngle(data.firstDim.x, data.firstDim.y, data.text.x, data.text.y);

		double angle = math::getAbsAngle(data.secondDim.x, data.secondDim.y, data.firstDim.x, data.firstDim.y);
		ShPoint3d rotate;

		math::rotate(angle, this->current.x, this->current.y, this->current.x + 10, this->current.y, rotate.x, rotate.y);
		math::checkLineLineIntersect(data.secondOrigin, data.secondDim, this->current, rotate, data.secondDim);

		math::rotate(angle, data.secondDim.x, data.secondDim.y, data.secondDim.x + 10, data.secondDim.y, rotate.x, rotate.y);
		math::checkLineLineIntersect(data.secondDim, rotate, data.firstOrigin, data.firstDim, data.firstDim);

		math::rotate(dimAngle, data.firstDim.x, data.firstDim.y, data.firstDim.x + dimDistance, data.firstDim.y, data.text.x, data.text.y);

		dimLinear->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchText) {
	
		data.text = this->current;
		double angle = math::getAbsAngle(data.firstDim.x, data.firstDim.y, data.secondDim.x, data.secondDim.y);
		ShPoint3d rotate;
		math::rotate(angle, this->current.x, this->current.y, this->current.x + 10, this->current.y, rotate.x, rotate.y);
		math::checkLineLineIntersect(this->current, rotate, data.firstOrigin, data.firstDim, data.firstDim);
		math::checkLineLineIntersect(this->current, rotate, data.secondOrigin, data.secondDim, data.secondDim);

		dimLinear->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchMove) {

		double disX = this->current.x - this->base.x;
		double disY = this->current.y - this->base.y;

		data.firstOrigin.move(disX, disY);
		data.firstDim.move(disX, disY);
		data.secondOrigin.move(disX, disY);
		data.secondDim.move(disX, disY);
		data.text.move(disX, disY);

		dimLinear->setData(data);
	}
}

void ShStretchVisitor::visit(ShDimAligned *dimAligned) {

	if (this->original == nullptr || !dynamic_cast<ShDimAligned*>(this->original))
		return;

	if (!dynamic_cast<ShStretchLeafData*>(this->stretchData))
		return;

	ShStretchLeafData *stretchData = dynamic_cast<ShStretchLeafData*>(this->stretchData);

	ShDimAligned *original = dynamic_cast<ShDimAligned*>(this->original);
	ShDimAlignedData data = original->getData();

	if (stretchData->stretchPoint == StretchPoint::StretchFirstOrigin) {

		double angle = math::getAbsAngle(this->current.x, this->current.y, data.secondOrigin.x, data.secondOrigin.y);
		double distance = math::getDistance(this->current.x, this->current.y, data.secondOrigin.x, data.secondOrigin.y);

		ShPoint3d temp, temp2;
		math::rotate(angle + 90, this->current.x, this->current.y, this->current.x + 10, this->current.y, temp.x, temp.y);
		math::rotate(angle, data.text.x, data.text.y, data.text.x + 10, data.text.y, temp2.x, temp2.y);
		math::checkLineLineIntersect(this->current, temp, data.text, temp2, data.firstDim);

		math::rotate(angle + 90, data.secondOrigin.x, data.secondOrigin.y, data.secondOrigin.x + 10, data.secondOrigin.y, temp.x, temp.y);
		math::checkLineLineIntersect(data.secondOrigin, temp, data.text, temp2, data.secondDim);

		data.firstOrigin = this->current;
		dimAligned->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchSecondOrigin) {

		double angle = math::getAbsAngle(this->current.x, this->current.y, data.firstOrigin.x, data.firstOrigin.y);
		double distance = math::getDistance(this->current.x, this->current.y, data.firstOrigin.x, data.firstOrigin.y);

		ShPoint3d temp, temp2;
		math::rotate(angle + 90, this->current.x, this->current.y, this->current.x + 10, this->current.y, temp.x, temp.y);
		math::rotate(angle, data.text.x, data.text.y, data.text.x + 10, data.text.y, temp2.x, temp2.y);
		math::checkLineLineIntersect(this->current, temp, data.text, temp2, data.secondDim);

		math::rotate(angle + 90, data.firstOrigin.x, data.firstOrigin.y, data.firstOrigin.x + 10, data.firstOrigin.y, temp.x, temp.y);
		math::checkLineLineIntersect(data.firstOrigin, temp, data.text, temp2, data.firstDim);

		data.secondOrigin = this->current;
		dimAligned->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchFirstDim) {

		double dimDistance = math::getDistance(data.firstDim.x, data.firstDim.y, data.text.x, data.text.y);
		double dimAngle = math::getAbsAngle(data.firstDim.x, data.firstDim.y, data.text.x, data.text.y);

		double angle = math::getAbsAngle(data.firstDim.x, data.firstDim.y, data.secondDim.x, data.secondDim.y);
		ShPoint3d temp;

		math::rotate(angle, this->current.x, this->current.y, this->current.x + 10, this->current.y, temp.x, temp.y);
		math::checkLineLineIntersect(data.firstOrigin, data.firstDim, this->current, temp, data.firstDim);

		math::rotate(angle, data.firstDim.x, data.firstDim.y, data.firstDim.x + 10, data.firstDim.y, temp.x, temp.y);
		math::checkLineLineIntersect(data.firstDim, temp, data.secondOrigin, data.secondDim, data.secondDim);

		math::rotate(dimAngle, data.firstDim.x, data.firstDim.y, data.firstDim.x + dimDistance, data.firstDim.y, data.text.x, data.text.y);

		dimAligned->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchSecondDim) {

		double dimDistance = math::getDistance(data.firstDim.x, data.firstDim.y, data.text.x, data.text.y);
		double dimAngle = math::getAbsAngle(data.firstDim.x, data.firstDim.y, data.text.x, data.text.y);

		double angle = math::getAbsAngle(data.secondDim.x, data.secondDim.y, data.firstDim.x, data.firstDim.y);
		ShPoint3d temp;

		math::rotate(angle, this->current.x, this->current.y, this->current.x + 10, this->current.y, temp.x, temp.y);
		math::checkLineLineIntersect(data.secondOrigin, data.secondDim, this->current, temp, data.secondDim);

		math::rotate(angle, data.secondDim.x, data.secondDim.y, data.secondDim.x + 10, data.secondDim.y, temp.x, temp.y);
		math::checkLineLineIntersect(data.secondDim, temp, data.firstOrigin, data.firstDim, data.firstDim);

		math::rotate(dimAngle, data.firstDim.x, data.firstDim.y, data.firstDim.x + dimDistance, data.firstDim.y, data.text.x, data.text.y);

		dimAligned->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchText) {

		data.text = this->current;
		double angle = math::getAbsAngle(data.firstDim.x, data.firstDim.y, data.secondDim.x, data.secondDim.y);
		ShPoint3d temp;
		math::rotate(angle, this->current.x, this->current.y, this->current.x + 10, this->current.y, temp.x, temp.y);
		math::checkLineLineIntersect(this->current, temp, data.firstOrigin, data.firstDim, data.firstDim);
		math::checkLineLineIntersect(this->current, temp, data.secondOrigin, data.secondDim, data.secondDim);

		dimAligned->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchMove) {

		double disX = this->current.x - this->base.x;
		double disY = this->current.y - this->base.y;

		data.firstOrigin.move(disX, disY);
		data.firstDim.move(disX, disY);
		data.secondOrigin.move(disX, disY);
		data.secondDim.move(disX, disY);
		data.text.move(disX, disY);

		dimAligned->setData(data);
	}
}

void ShStretchVisitor::visit(ShDimRadius *dimRadius) {

	if (this->original == nullptr || !dynamic_cast<ShDimRadius*>(this->original))
		return;

	if (!dynamic_cast<ShStretchLeafData*>(this->stretchData))
		return;

	ShStretchLeafData *stretchData = dynamic_cast<ShStretchLeafData*>(this->stretchData);

	ShDimRadius *original = dynamic_cast<ShDimRadius*>(this->original);
	ShDimRadiusData data = original->getData();

	if (stretchData->stretchPoint == StretchPoint::StretchCenter) {

		double distanceCenterToDim = math::getDistance(data.center.x, data.center.y, data.dim.x, data.dim.y);
		data.center = this->current;
		double angle = math::getAbsAngle(data.center.x, data.center.y, data.text.x, data.text.y);
		math::rotate(angle, data.center.x, data.center.y, data.center.x + distanceCenterToDim, data.center.y, data.dim.x, data.dim.y);

		dimRadius->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchDim) {
	
		double angle = math::getAbsAngle(data.center.x, data.center.y, this->current.x, this->current.y);
		double radius = original->getRadius();
		double distanceText = math::getDistance(data.center.x, data.center.y, data.text.x, data.text.y);
		math::rotate(angle, data.center.x, data.center.y, data.center.x + radius, data.center.y, data.dim.x, data.dim.y);
		math::rotate(angle, data.center.x, data.center.y, data.center.x + distanceText, data.center.y, data.text.x, data.text.y);

		dimRadius->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchText) {
	
		data.text = this->current;
		double angle = math::getAbsAngle(data.center.x, data.center.y, this->current.x, this->current.y);
		math::rotate(angle, data.center.x, data.center.y, data.center.x + original->getRadius(), data.center.y, data.dim.x, data.dim.y);

		dimRadius->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchMove) {
	
		double disX = this->current.x - this->base.x;
		double disY = this->current.y - this->base.y;

		data.center.move(disX, disY);
		data.dim.move(disX, disY);
		data.text.move(disX, disY);

		dimRadius->setData(data);
	}

}

void ShStretchVisitor::visit(ShDimDiameter *dimDiameter) {

	if (this->original == nullptr || !dynamic_cast<ShDimDiameter*>(this->original))
		return;

	if (!dynamic_cast<ShStretchLeafData*>(this->stretchData))
		return;

	ShStretchLeafData *stretchData = dynamic_cast<ShStretchLeafData*>(this->stretchData);

	ShDimDiameter *original = dynamic_cast<ShDimDiameter*>(this->original);
	ShDimDiameterData data = original->getData();

	if (stretchData->stretchPoint == StretchPoint::StretchFirstDim) {

		double angle = math::getAbsAngle(data.center.x, data.center.y, this->current.x, this->current.y);
		double radius = original->getRadius();
		double distanceText = math::getDistance(data.center.x, data.center.y, data.text.x, data.text.y);
		math::rotate(angle, data.center.x, data.center.y, data.center.x + radius, data.center.y, data.firstDim.x, data.firstDim.y);
		math::rotate(angle + 180, data.center.x, data.center.y, data.center.x + radius, data.center.y, data.secondDim.x, data.secondDim.y);
		math::rotate(angle, data.center.x, data.center.y, data.center.x + distanceText, data.center.y, data.text.x, data.text.y);

		dimDiameter->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchSecondDim) {

		data.secondDim = this->current;
		double disCenterToText = math::getDistance(data.center.x, data.center.y, data.text.x, data.text.y);
		double angle = math::getAbsAngle(data.center.x, data.center.y, data.secondDim.x, data.secondDim.y);
		double radius = math::getDistance(data.center.x, data.center.y, data.secondDim.x, data.secondDim.y);
		math::rotate(angle + 180, data.center.x, data.center.y, data.center.x + radius, data.center.y, data.firstDim.x, data.firstDim.y);
		math::rotate(angle + 180, data.center.x, data.center.y, data.center.x + disCenterToText, data.center.y, data.text.x, data.text.y);

		dimDiameter->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchText) {

		data.text = this->current;
		double angle = math::getAbsAngle(data.center.x, data.center.y, this->current.x, this->current.y);
		math::rotate(angle, data.center.x, data.center.y, data.center.x + original->getRadius(), data.center.y, data.firstDim.x, data.firstDim.y);
		math::rotate(angle + 180, data.center.x, data.center.y, data.center.x + original->getRadius(), data.center.y, data.secondDim.x, data.secondDim.y);

		dimDiameter->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchMove) {

		double disX = this->current.x - this->base.x;
		double disY = this->current.y - this->base.y;

		data.center.move(disX, disY);
		data.firstDim.move(disX, disY);
		data.secondDim.move(disX, disY);
		data.text.move(disX, disY);

		dimDiameter->setData(data);
	}
}

void ShStretchVisitor::visit(ShDimArcLength *dimArcLength) {

	if (this->original == nullptr || !dynamic_cast<ShDimArcLength*>(this->original))
		return;

	if (!dynamic_cast<ShStretchLeafData*>(this->stretchData))
		return;

	ShStretchLeafData *stretchData = dynamic_cast<ShStretchLeafData*>(this->stretchData);

	ShDimArcLength *original = dynamic_cast<ShDimArcLength*>(this->original);
	ShDimArcLengthData data = original->getData();

	if (stretchData->stretchPoint == StretchPoint::StretchBoundary) {

		data.boundary = this->current;

		double startAngle = original->getStartAngle();
		double endAngle = original->getEndAngle();
		double angle = math::getAbsAngle(data.center.x, data.center.y, data.boundary.x, data.boundary.y);

		if (math::checkAngleLiesOnAngleBetween(startAngle, endAngle, angle) == false) {
		
			math::swap(data.start, data.end);
			math::swap(startAngle, endAngle);
		}

		double distance = math::getDistance(data.center.x, data.center.y, data.boundary.x, data.boundary.y);
		double angleDifference = math::getAngleDifference(startAngle, endAngle);

		math::rotate(startAngle + (angleDifference / 2.0), data.center.x, data.center.y, data.center.x + distance, data.center.y, data.text.x, data.text.y);

		dimArcLength->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchText) {

		data.text = this->current;

		double angleCenterToBoundary = math::getAbsAngle(data.center.x, data.center.y, data.boundary.x, data.boundary.y);
		double disCenterToText = math::getDistance(data.center.x, data.center.y, data.text.x, data.text.y);
		math::rotate(angleCenterToBoundary, data.center.x, data.center.y, data.center.x + disCenterToText, data.center.y, data.boundary.x, data.boundary.y);

		dimArcLength->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchStart) {

		data.start = this->current;
		double startAngle = math::getAbsAngle(data.center.x, data.center.y, data.start.x, data.start.y);
		double endAngle = original->getEndAngle();
		double angle = math::getAbsAngle(data.center.x, data.center.y, data.boundary.x, data.boundary.y);

		if (math::checkAngleLiesOnAngleBetween(startAngle, endAngle, angle) == false) {
		
			math::swap(data.start, data.end);
			math::swap(startAngle, endAngle);
		}

		double angleDifference = math::getAngleDifference(startAngle, endAngle);

		math::rotate(startAngle + (angleDifference / 2.0), data.center.x, data.center.y, 
			data.center.x + original->getArcRadius(), data.center.y, data.text.x, data.text.y);
		math::rotate(startAngle + (angleDifference / 3.0), data.center.x, data.center.y, 
			data.center.x + original->getArcRadius(), data.center.y, data.boundary.x, data.boundary.y);
		dimArcLength->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchEnd) {

		data.end = this->current;
		double startAngle = original->getStartAngle();
		double endAngle = math::getAbsAngle(data.center.x, data.center.y, data.end.x, data.end.y);
		double angle = math::getAbsAngle(data.center.x, data.center.y, data.boundary.x, data.boundary.y);

		if (math::checkAngleLiesOnAngleBetween(startAngle, endAngle, angle) == false) {

			math::swap(data.start, data.end);
			math::swap(startAngle, endAngle);
		}

		double angleDifference = math::getAngleDifference(startAngle, endAngle);

		math::rotate(startAngle + (angleDifference / 2.0), data.center.x, data.center.y,
			data.center.x + original->getArcRadius(), data.center.y, data.text.x, data.text.y);
		math::rotate(startAngle + (angleDifference / 3.0), data.center.x, data.center.y,
			data.center.x + original->getArcRadius(), data.center.y, data.boundary.x, data.boundary.y);
		dimArcLength->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchMove) {
	
		double disX = this->current.x - this->base.x;
		double disY = this->current.y - this->base.y;

		data.center.move(disX, disY);
		data.start.move(disX, disY);
		data.end.move(disX, disY);
		data.text.move(disX, disY);
		data.boundary.move(disX, disY);

		dimArcLength->setData(data);
	}
}

void ShStretchVisitor::visit(ShDimAngular *dimAngular) {

	if (this->original == nullptr || !dynamic_cast<ShDimAngular*>(this->original))
		return;

	if (!dynamic_cast<ShStretchLeafData*>(this->stretchData))
		return;

	ShStretchLeafData *stretchData = dynamic_cast<ShStretchLeafData*>(this->stretchData);

	ShDimAngular *original = dynamic_cast<ShDimAngular*>(this->original);
	ShDimAngularData data = original->getData();

	if (stretchData->stretchPoint == StretchPoint::StretchCenter) {
	
		data.center = this->current;
		double arcRadius = math::getDistance(data.center.x, data.center.y, data.boundary.x, data.boundary.y);
		double startAngle = math::getAbsAngle(data.center.x, data.center.y, data.start.x, data.start.y);
		double endAngle = math::getAbsAngle(data.center.x, data.center.y, data.end.x, data.end.y);
		double boundaryAngle = math::getAbsAngle(data.center.x, data.center.y, data.boundary.x, data.boundary.y);
		
		if (math::checkAngleLiesOnAngleBetween(startAngle, endAngle, boundaryAngle) == false) {

			math::swap(data.start, data.end);
			math::swap(startAngle, endAngle);
		}

		double angleDifference = math::getAngleDifference(startAngle, endAngle);

		math::rotate(startAngle + (angleDifference / 2.0), data.center.x, data.center.y,
			data.center.x + arcRadius, data.center.y, data.text.x, data.text.y);

		dimAngular->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchBoundary) {

		data.boundary = this->current;

		double startAngle = original->getStartAngle();
		double endAngle = original->getEndAngle();
		double angle = math::getAbsAngle(data.center.x, data.center.y, data.boundary.x, data.boundary.y);

		if (math::checkAngleLiesOnAngleBetween(startAngle, endAngle, angle) == false) {

			math::swap(data.start, data.end);
			math::swap(startAngle, endAngle);
		}

		double distance = math::getDistance(data.center.x, data.center.y, data.boundary.x, data.boundary.y);
		double angleDifference = math::getAngleDifference(startAngle, endAngle);

		math::rotate(startAngle + (angleDifference / 2.0), data.center.x, data.center.y, data.center.x + distance, data.center.y, data.text.x, data.text.y);

		dimAngular->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchText) {

		data.text = this->current;

		double angleCenterToBoundary = math::getAbsAngle(data.center.x, data.center.y, data.boundary.x, data.boundary.y);
		double disCenterToText = math::getDistance(data.center.x, data.center.y, data.text.x, data.text.y);
		math::rotate(angleCenterToBoundary, data.center.x, data.center.y, data.center.x + disCenterToText, data.center.y, data.boundary.x, data.boundary.y);

		dimAngular->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchStart) {

		data.start = this->current;
		double startAngle = math::getAbsAngle(data.center.x, data.center.y, data.start.x, data.start.y);
		double endAngle = original->getEndAngle();
		double angle = math::getAbsAngle(data.center.x, data.center.y, data.boundary.x, data.boundary.y);

		if (math::checkAngleLiesOnAngleBetween(startAngle, endAngle, angle) == false) {

			math::swap(data.start, data.end);
			math::swap(startAngle, endAngle);
		}

		double angleDifference = math::getAngleDifference(startAngle, endAngle);

		math::rotate(startAngle + (angleDifference / 2.0), data.center.x, data.center.y,
			data.center.x + original->getArcRadius(), data.center.y, data.text.x, data.text.y);
		math::rotate(startAngle + (angleDifference / 3.0), data.center.x, data.center.y,
			data.center.x + original->getArcRadius(), data.center.y, data.boundary.x, data.boundary.y);
		dimAngular->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchEnd) {

		data.end = this->current;
		double startAngle = original->getStartAngle();
		double endAngle = math::getAbsAngle(data.center.x, data.center.y, data.end.x, data.end.y);
		double angle = math::getAbsAngle(data.center.x, data.center.y, data.boundary.x, data.boundary.y);

		if (math::checkAngleLiesOnAngleBetween(startAngle, endAngle, angle) == false) {

			math::swap(data.start, data.end);
			math::swap(startAngle, endAngle);
		}

		double angleDifference = math::getAngleDifference(startAngle, endAngle);

		math::rotate(startAngle + (angleDifference / 2.0), data.center.x, data.center.y,
			data.center.x + original->getArcRadius(), data.center.y, data.text.x, data.text.y);
		math::rotate(startAngle + (angleDifference / 3.0), data.center.x, data.center.y,
			data.center.x + original->getArcRadius(), data.center.y, data.boundary.x, data.boundary.y);
		dimAngular->setData(data);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchMove) {

		double disX = this->current.x - this->base.x;
		double disY = this->current.y - this->base.y;

		data.center.move(disX, disY);
		data.start.move(disX, disY);
		data.end.move(disX, disY);
		data.text.move(disX, disY);
		data.boundary.move(disX, disY);

		dimAngular->setData(data);
	}
}

void ShStretchVisitor::visit(ShConstructionLine *constructionLine) {

	if (this->original == nullptr || !dynamic_cast<ShConstructionLine*>(this->original))
		return;

	if (!dynamic_cast<ShStretchLeafData*>(this->stretchData))
		return;

	ShStretchLeafData *stretchData = dynamic_cast<ShStretchLeafData*>(this->stretchData);

	ShConstructionLine *original = dynamic_cast<ShConstructionLine*>(this->original);
	ShLineData data = original->getData();

	double disX = this->current.x - this->base.x;
	double disY = this->current.y - this->base.y;

	if (stretchData->stretchPoint == StretchPoint::StretchStart) {
		data.start.move(disX, disY);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchEnd) {
		data.end.move(disX, disY);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchMove) {

		data.start.move(disX, disY);
		data.end.move(disX, disY);
	}


	constructionLine->setData(data);
}

void ShStretchVisitor::visit(ShEllipse* ellipse) {
	if (this->original == nullptr || !dynamic_cast<ShEllipse*>(this->original))
		return;

	if (!dynamic_cast<ShStretchLeafData*>(this->stretchData))
		return;

	ShStretchLeafData* stretchData = dynamic_cast<ShStretchLeafData*>(this->stretchData);
	ShEllipse* original = dynamic_cast<ShEllipse*>(this->original);
	ShEllipseData data = original->getData();

	double disX = this->current.x - this->base.x;
	double disY = this->current.y - this->base.y;

	if (stretchData->stretchPoint == StretchPoint::StretchMove) {
		// 移动整个椭圆
		data.center.move(disX, disY);
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchMajorAxis) {
		// 拉伸长轴
		double newMajorRadius = math::getDistance(data.center.x, data.center.y,
			this->current.x, this->current.y);
		data.majorRadius = newMajorRadius;
	}
	else if (stretchData->stretchPoint == StretchPoint::StretchMinorAxis) {
		// 拉伸短轴
		double newMinorRadius = math::getDistance(data.center.x, data.center.y,
			this->current.x, this->current.y);
		data.minorRadius = newMinorRadius;
	}

	ellipse->setData(data);
}

void ShStretchVisitor::visit(ShBlock* block) {
	if (this->original == nullptr || !dynamic_cast<ShBlock*>(this->original))
		return;

	if (!dynamic_cast<ShStretchLeafData*>(this->stretchData))
		return;

	ShStretchLeafData* stretchData = dynamic_cast<ShStretchLeafData*>(this->stretchData);
	ShBlock* original = dynamic_cast<ShBlock*>(this->original);

	if (stretchData->stretchPoint == StretchPoint::StretchMove) {
		// 计算鼠标移动的偏移量
		double disX = this->current.x - this->base.x;
		double disY = this->current.y - this->base.y;

		// 计算新的基点位置
		ShPoint3d originalBase = original->getBasePoint();
		ShPoint3d newBasePoint = originalBase;
		newBasePoint.move(disX, disY);
		block->setBasePoint(newBasePoint);

		// 移动块中的所有实体（保持相对位置不变）
		const QList<ShEntity*>& entities = original->getEntities();
		for (int i = 0; i < entities.size(); ++i) {
			ShEntity* originalEntity = entities[i];
			ShEntity* previewEntity = block->getEntities()[i];

			ShStretchVisitor stretchVisitor(this->base, this->current);
			stretchVisitor.setOriginal(originalEntity);
			stretchVisitor.setStretchData(new ShStretchLeafData(StretchPoint::StretchMove));
			previewEntity->accept(&stretchVisitor);
		}
	}
}
///////////////////////////////////////////////////

ShPossibleEntityToStretchFinder::ShPossibleEntityToStretchFinder(const ShPoint3d &point, bool &possible, ShStretchData* *stretchData)
	:point(point), possible(possible), stretchData(stretchData) {

}

ShPossibleEntityToStretchFinder::~ShPossibleEntityToStretchFinder() {

}

void ShPossibleEntityToStretchFinder::visit(ShLine *line) {

	if (this->point.isEqual(line->getStart()) == true) {

		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchStart);
		this->possible = true;
	}

	else if (this->point.isEqual(line->getEnd()) == true) {

		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchEnd);
		this->possible = true;
	}
	else if (this->point.isEqual(line->getMid()) == true) {

		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
		this->possible = true;
	}
}

void ShPossibleEntityToStretchFinder::visit(ShCircle *circle) {

	ShPoint3d center = circle->getCenter();
	double radius = circle->getRadius();

	if (this->point.isEqual(center) == true) {

		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
		this->possible = true;
		return;
	}

	ShPoint3d temp;
	temp.x = center.x + radius;
	temp.y = center.y;

	if (this->point.isEqual(temp) == true) {

		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchRight);
		this->possible = true;
		return;
	}

	temp.x = center.x;
	temp.y = center.y - radius;

	if (this->point.isEqual(temp) == true) {

		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchBottom);
		this->possible = true;
		return;
	}

	temp.x = center.x - radius;
	temp.y = center.y;

	if (this->point.isEqual(temp) == true) {

		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchLeft);
		this->possible = true;
		return;
	}

	temp.x = center.x;
	temp.y = center.y + radius;

	if (this->point.isEqual(temp) == true) {

		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchTop);
		this->possible = true;
		return;
	}
}

void ShPossibleEntityToStretchFinder::visit(ShArc *arc) {

	if (this->point.isEqual(arc->getCenter()) == true) {

		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
		this->possible = true;
	}
	else if (this->point.isEqual(arc->getStart()) == true) {

		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchStart);
		this->possible = true;
	}
	else if (this->point.isEqual(arc->getEnd()) == true) {

		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchEnd);
		this->possible = true;
	}
	else if (this->point.isEqual(arc->getMid()) == true) {

		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMid);
		this->possible = true;
	}
}

void ShPossibleEntityToStretchFinder::visit(ShPoint *point) {

	if (this->point.isEqual(point->getPosition()) == true) {

		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
		this->possible = true;
	}
}

void ShPossibleEntityToStretchFinder::visit(ShDot *dot) {

	if (this->point.isEqual(dot->getPosition()) == true) {

		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
		this->possible = true;
	}
}

void ShPossibleEntityToStretchFinder::visit(ShDimLinear *dimLinear) {

	ShDimLinearData data = dimLinear->getData();

	if (this->point.isEqual(data.firstOrigin) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchFirstOrigin);
		this->possible = true;
	}
	else if (this->point.isEqual(data.secondOrigin) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchSecondOrigin);
		this->possible = true;
	}
	else if (this->point.isEqual(data.firstDim) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchFirstDim);
		this->possible = true;
	}
	else if (this->point.isEqual(data.secondDim) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchSecondDim);
		this->possible = true;
	}
	else if (this->point.isEqual(data.text) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchText);
		this->possible = true;
	}
}

void ShPossibleEntityToStretchFinder::visit(ShDimAligned *dimAligned) {

	ShDimAlignedData data = dimAligned->getData();

	if (this->point.isEqual(data.firstOrigin) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchFirstOrigin);
		this->possible = true;
	}
	else if (this->point.isEqual(data.secondOrigin) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchSecondOrigin);
		this->possible = true;
	}
	else if (this->point.isEqual(data.firstDim) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchFirstDim);
		this->possible = true;
	}
	else if (this->point.isEqual(data.secondDim) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchSecondDim);
		this->possible = true;
	}
	else if (this->point.isEqual(data.text) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchText);
		this->possible = true;
	}
}

void ShPossibleEntityToStretchFinder::visit(ShDimRadius *dimRadius) {

	ShDimRadiusData data = dimRadius->getData();

	if (this->point.isEqual(data.center) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchCenter);
		this->possible = true;
	}
	else if (this->point.isEqual(data.dim) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchDim);
		this->possible = true;
	}
	else if (this->point.isEqual(data.text) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchText);
		this->possible = true;
	}
}

void ShPossibleEntityToStretchFinder::visit(ShDimDiameter *dimDiameter) {

	ShDimDiameterData data = dimDiameter->getData();

	if (this->point.isEqual(data.firstDim) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchFirstDim);
		this->possible = true;
	}
	else if (this->point.isEqual(data.secondDim) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchSecondDim);
		this->possible = true;
	}
	else if (this->point.isEqual(data.text) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchText);
		this->possible = true;
	}
}

void ShPossibleEntityToStretchFinder::visit(ShDimArcLength *dimArcLength) {

	ShDimArcLengthData data = dimArcLength->getData();

	if (this->point.isEqual(data.start) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchStart);
		this->possible = true;
	}
	else if (this->point.isEqual(data.end) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchEnd);
		this->possible = true;
	}
	else if (this->point.isEqual(data.text) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchText);
		this->possible = true;
	}
	else if (this->point.isEqual(data.boundary) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchBoundary);
		this->possible = true;
	}
}

void ShPossibleEntityToStretchFinder::visit(ShDimAngular *dimAngular) {

	ShDimAngularData data = dimAngular->getData();

	if (this->point.isEqual(data.center) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchCenter);
		this->possible = true;
	}
	else if (this->point.isEqual(data.start) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchStart);
		this->possible = true;
	}
	else if (this->point.isEqual(data.end) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchEnd);
		this->possible = true;
	}
	else if (this->point.isEqual(data.text) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchText);
		this->possible = true;
	}
	else if (this->point.isEqual(data.boundary) == true) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchBoundary);
		this->possible = true;
	}
}

void ShPossibleEntityToStretchFinder::visit(ShConstructionLine *constructionLine) {

	if (this->point.isEqual(constructionLine->getStart()) == true) {

		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchStart);
		this->possible = true;
	}

	else if (this->point.isEqual(constructionLine->getEnd()) == true) {

		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchEnd);
		this->possible = true;
	}
	else if (this->point.isEqual(constructionLine->getMid()) == true) {

		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
		this->possible = true;
	}
}

void ShPossibleEntityToStretchFinder::visit(ShEllipse* ellipse) {
	ShPoint3d center = ellipse->getCenter();
	double majorRadius = ellipse->getMajorRadius();
	double minorRadius = ellipse->getMinorRadius();
	double angle = ellipse->getAngle();

	// 计算旋转后的长轴和短轴端点
	double radAngle = angle * 3.1415926535897 / 180.0;
	double cosAngle = cos(radAngle);
	double sinAngle = sin(radAngle);

	// 长轴端点
	ShPoint3d majorAxisPoint = {
		center.x + majorRadius * cosAngle,
		center.y + majorRadius * sinAngle
	};

	// 短轴端点
	ShPoint3d minorAxisPoint = {
		center.x - minorRadius * sinAngle,
		center.y + minorRadius * cosAngle
	};

	if (this->point.isEqual(center)) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
		this->possible = true;
	}
	else if (this->point.isEqual(majorAxisPoint)) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMajorAxis);
		this->possible = true;
	}
	else if (this->point.isEqual(minorAxisPoint)) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMinorAxis);
		this->possible = true;
	}
}

void ShPossibleEntityToStretchFinder::visit(ShBlock* block) {
	// 检查是否点击了块的基点
	if (this->point.isEqual(block->getBasePoint())) {
		*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
		this->possible = true;
	}
	else {
		// 检查块中的每个实体
		for (ShEntity* entity : block->getEntities()) {
			ShPossibleEntityToStretchFinder finder(this->point, this->possible, this->stretchData);
			entity->accept(&finder);
			if (this->possible) {
				break;
			}
		}
	}
}
////////////////////////////////////////////////////////////////


ShStretchDataForMoveCreator::ShStretchDataForMoveCreator(ShStretchData* *stretchData)
	:stretchData(stretchData) {

}

ShStretchDataForMoveCreator::~ShStretchDataForMoveCreator() {

}

void ShStretchDataForMoveCreator::visit(ShLine *line) {

	*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
}

void ShStretchDataForMoveCreator::visit(ShCircle *circle) {

	*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
}

void ShStretchDataForMoveCreator::visit(ShArc *arc) {

	*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
}

void ShStretchDataForMoveCreator::visit(ShPoint *point) {

	*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
}

void ShStretchDataForMoveCreator::visit(ShDot *dot) {

	*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
}

void ShStretchDataForMoveCreator::visit(ShDimLinear *dimLinear) {

	*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
}

void ShStretchDataForMoveCreator::visit(ShDimAligned *dimAligned) {

	*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
}

void ShStretchDataForMoveCreator::visit(ShDimRadius *dimRadius) {

	*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
}

void ShStretchDataForMoveCreator::visit(ShDimDiameter *dimDiameter) {

	*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
}

void ShStretchDataForMoveCreator::visit(ShDimArcLength *dimArcLength) {

	*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
}

void ShStretchDataForMoveCreator::visit(ShDimAngular *dimAngular) {

	*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
}

void ShStretchDataForMoveCreator::visit(ShConstructionLine *constructionLine) {

	*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
}

void ShStretchDataForMoveCreator::visit(ShEllipse* ellipse) {
	*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
}

void ShStretchDataForMoveCreator::visit(ShBlock* block) {
	*this->stretchData = new ShStretchLeafData(StretchPoint::StretchMove);
}

///////////////////////////////////////////////////////////////////


ShStretchPointRectFinder::ShStretchPointRectFinder(const ShPoint3d &topLeft, const ShPoint3d &bottomRight, ShStretchData* *stretchData)
	:topLeft(topLeft), bottomRight(bottomRight), stretchData(stretchData) {

}

ShStretchPointRectFinder::~ShStretchPointRectFinder() {

}

void ShStretchPointRectFinder::visit(ShLine *line) {

	int insideCount = 0;
	StretchPoint stretchPoint = StretchPoint::StretchNothing;

	if (this->checkPointLiesInsideRect(line->getStart()) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchStart;
	}

	if (this->checkPointLiesInsideRect(line->getEnd()) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchEnd;
	}

	if (insideCount == 2)
		stretchPoint = StretchPoint::StretchMove;

	*this->stretchData = new ShStretchLeafData(stretchPoint);
}

void ShStretchPointRectFinder::visit(ShCircle *circle) {

	StretchPoint stretchPoint = StretchPoint::StretchNothing;

	if (this->checkPointLiesInsideRect(circle->getCenter()) == true)
		stretchPoint = StretchPoint::StretchMove;

	*this->stretchData = new ShStretchLeafData(stretchPoint);
}

void ShStretchPointRectFinder::visit(ShArc *arc) {

	int insideCount = 0;
	StretchPoint stretchPoint = StretchPoint::StretchNothing;

	if (this->checkPointLiesInsideRect(arc->getStart()) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchStart;
	}

	if (this->checkPointLiesInsideRect(arc->getEnd()) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchEnd;
	}

	if (insideCount == 2)
		stretchPoint = StretchPoint::StretchMove;

	*this->stretchData = new ShStretchLeafData(stretchPoint);
}

void ShStretchPointRectFinder::visit(ShPoint *point) {

	StretchPoint stretchPoint = StretchPoint::StretchNothing;

	if (this->checkPointLiesInsideRect(point->getPosition()) == true) {

		stretchPoint = StretchPoint::StretchMove;
	}

	*this->stretchData = new ShStretchLeafData(stretchPoint);
}

void ShStretchPointRectFinder::visit(ShDot *dot) {

	StretchPoint stretchPoint = StretchPoint::StretchNothing;

	if (this->checkPointLiesInsideRect(dot->getPosition()) == true) {

		stretchPoint = StretchPoint::StretchMove;
	}

	*this->stretchData = new ShStretchLeafData(stretchPoint);
}

void ShStretchPointRectFinder::visit(ShDimLinear *dimLinear) {

	int insideCount = 0;
	StretchPoint stretchPoint = StretchPoint::StretchNothing;
	ShDimLinearData data = dimLinear->getData();

	if (this->checkPointLiesInsideRect(data.firstOrigin) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchFirstOrigin;
	}
	if (this->checkPointLiesInsideRect(data.secondOrigin) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchSecondOrigin;
	}
	if (this->checkPointLiesInsideRect(data.firstDim) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchFirstDim;
	}
	if (this->checkPointLiesInsideRect(data.secondDim) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchSecondDim;
	}
	if (this->checkPointLiesInsideRect(data.text) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchText;
	}

	if (insideCount > 1)
		stretchPoint = StretchPoint::StretchMove;

	*this->stretchData = new ShStretchLeafData(stretchPoint);
}

void ShStretchPointRectFinder::visit(ShDimAligned *dimAligned) {

	int insideCount = 0;
	StretchPoint stretchPoint = StretchPoint::StretchNothing;
	ShDimAlignedData data = dimAligned->getData();

	if (this->checkPointLiesInsideRect(data.firstOrigin) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchFirstOrigin;
	}
	if (this->checkPointLiesInsideRect(data.secondOrigin) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchSecondOrigin;
	}
	if (this->checkPointLiesInsideRect(data.firstDim) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchFirstDim;
	}
	if (this->checkPointLiesInsideRect(data.secondDim) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchSecondDim;
	}
	if (this->checkPointLiesInsideRect(data.text) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchText;
	}

	if (insideCount > 1)
		stretchPoint = StretchPoint::StretchMove;

	*this->stretchData = new ShStretchLeafData(stretchPoint);
}

void ShStretchPointRectFinder::visit(ShDimRadius *dimRadius) {

	int insideCount = 0;
	StretchPoint stretchPoint = StretchPoint::StretchNothing;
	ShDimRadiusData data = dimRadius->getData();

	if (this->checkPointLiesInsideRect(data.center) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchCenter;
	}
	if (this->checkPointLiesInsideRect(data.dim) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchDim;
	}
	if (this->checkPointLiesInsideRect(data.text) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchText;
	}

	if (insideCount > 1)
		stretchPoint = StretchPoint::StretchMove;

	*this->stretchData = new ShStretchLeafData(stretchPoint);
}

void ShStretchPointRectFinder::visit(ShDimDiameter *dimDiameter) {

	int insideCount = 0;
	StretchPoint stretchPoint = StretchPoint::StretchNothing;
	ShDimDiameterData data = dimDiameter->getData();

	if (this->checkPointLiesInsideRect(data.firstDim) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchFirstDim;
	}
	if (this->checkPointLiesInsideRect(data.secondDim) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchSecondDim;
	}
	if (this->checkPointLiesInsideRect(data.text) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchText;
	}

	if (insideCount > 1)
		stretchPoint = StretchPoint::StretchMove;

	*this->stretchData = new ShStretchLeafData(stretchPoint);
}

void ShStretchPointRectFinder::visit(ShDimArcLength *dimArcLength) {

	int insideCount = 0;
	StretchPoint stretchPoint = StretchPoint::StretchNothing;
	ShDimArcLengthData data = dimArcLength->getData();

	if (this->checkPointLiesInsideRect(data.start) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchStart;
	}
	if (this->checkPointLiesInsideRect(data.end) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchEnd;
	}
	if (this->checkPointLiesInsideRect(data.text) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchText;
	}
	if (this->checkPointLiesInsideRect(data.boundary) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchBoundary;
	}

	if (insideCount > 1)
		stretchPoint = StretchPoint::StretchMove;

	*this->stretchData = new ShStretchLeafData(stretchPoint);
}

void ShStretchPointRectFinder::visit(ShDimAngular *dimAngular) {

	int insideCount = 0;
	StretchPoint stretchPoint = StretchPoint::StretchNothing;
	ShDimAngularData data = dimAngular->getData();

	if (this->checkPointLiesInsideRect(data.center) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchCenter;
	}
	if (this->checkPointLiesInsideRect(data.start) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchStart;
	}
	if (this->checkPointLiesInsideRect(data.end) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchEnd;
	}
	if (this->checkPointLiesInsideRect(data.text) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchText;
	}
	if (this->checkPointLiesInsideRect(data.boundary) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchBoundary;
	}

	if (insideCount > 1)
		stretchPoint = StretchPoint::StretchMove;

	*this->stretchData = new ShStretchLeafData(stretchPoint);
}

void ShStretchPointRectFinder::visit(ShConstructionLine *constructionLine) {

	int insideCount = 0;
	StretchPoint stretchPoint = StretchPoint::StretchNothing;

	if (this->checkPointLiesInsideRect(constructionLine->getStart()) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchStart;
	}

	if (this->checkPointLiesInsideRect(constructionLine->getEnd()) == true) {
		insideCount++;
		stretchPoint = StretchPoint::StretchEnd;
	}

	if (insideCount == 2)
		stretchPoint = StretchPoint::StretchMove;

	*this->stretchData = new ShStretchLeafData(stretchPoint);
}

void ShStretchPointRectFinder::visit(ShEllipse* ellipse) {
	ShPoint3d center = ellipse->getCenter();
	StretchPoint stretchPoint = StretchPoint::StretchNothing;

	if (this->checkPointLiesInsideRect(center)) {
		stretchPoint = StretchPoint::StretchMove;
	}

	*this->stretchData = new ShStretchLeafData(stretchPoint);
}

void ShStretchPointRectFinder::visit(ShBlock* block) {
	StretchPoint stretchPoint = StretchPoint::StretchNothing;
	int insideCount = 0;

	// 检查基点是否在矩形内
	if (this->checkPointLiesInsideRect(block->getBasePoint())) {
		insideCount++;
		stretchPoint = StretchPoint::StretchMove;
	}

	// 检查块中的每个实体
	for (ShEntity* entity : block->getEntities()) {
		ShStretchPointRectFinder finder(this->topLeft, this->bottomRight, this->stretchData);
		entity->accept(&finder);

		ShStretchLeafData* data = dynamic_cast<ShStretchLeafData*>(*this->stretchData);
		if (data && data->stretchPoint != StretchPoint::StretchNothing) {
			insideCount++;
			stretchPoint = StretchPoint::StretchMove;
		}

		delete* this->stretchData;
		*this->stretchData = nullptr;
	}

	*this->stretchData = new ShStretchLeafData(stretchPoint);
}

bool ShStretchPointRectFinder::checkPointLiesInsideRect(const ShPoint3d &point) {

	return math::checkPointLiesInsideRect(point, this->topLeft, this->bottomRight, 0);
}