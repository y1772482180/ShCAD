
#include "ShMover.h"
#include "Entity\Leaf\ShLine.h"
#include "Entity\Leaf\ShCircle.h"
#include "Entity\Leaf\ShArc.h"
#include "Entity\Leaf\ShPoint.h"
#include "Entity\Leaf\ShDot.h"
#include "Entity\Leaf\ShEllipse.h" // �����Բͷ�ļ�
#include "Entity\Composite\Dim\ShDimLinear.h"
#include "Entity\Composite\Dim\ShDimAligned.h"
#include "Entity\Composite\Dim\ShDimRadius.h"
#include "Entity\Composite\Dim\ShDimDiameter.h"
#include "Entity\Composite\Dim\ShDimArcLength.h"
#include "Entity\Composite\Dim\ShDimAngular.h"
#include "Entity\Leaf\ShConstructionLine.h"
#include "Entity\leaf/ShBlock.h"  // ��ӿ��ͷ�ļ�
ShMover::ShMover(double disX, double disY)
	:disX(disX), disY(disY) {

}

ShMover::~ShMover() {

}

void ShMover::visit(ShLine *line) {

	ShPoint3d start = line->getStart();
	ShPoint3d end = line->getEnd();

	start.move(this->disX, this->disY);
	end.move(this->disX, this->disY);

	line->setStart(start);
	line->setEnd(end);
}

void ShMover::visit(ShCircle *circle) {

	ShPoint3d center = circle->getCenter();

	center.move(this->disX, this->disY);

	circle->setCenter(center);
}

void ShMover::visit(ShArc *arc) {

	ShPoint3d center = arc->getCenter();

	center.move(this->disX, this->disY);

	arc->setCenter(center);
}

void ShMover::visit(ShPoint *point) {

	ShPoint3d position = point->getPosition();

	position.move(this->disX, this->disY);

	point->setPosition(position);
}

void ShMover::visit(ShDot *dot) {

	ShPoint3d position = dot->getPosition();

	position.move(this->disX, this->disY);

	dot->setPosition(position);
}

void ShMover::visit(ShEllipse* ellipse) {
	ShEllipseData data = ellipse->getData();
	data.center.move(this->disX, this->disY);
	ellipse->setData(data);
}
void ShMover::visit(ShDimLinear *dimLinear) {

	ShDimLinearData data = dimLinear->getData();

	data.firstOrigin.move(this->disX, this->disY);
	data.firstDim.move(this->disX, this->disY);
	data.secondOrigin.move(this->disX, this->disY);
	data.secondDim.move(this->disX, this->disY);
	data.text.move(this->disX, this->disY);

	dimLinear->setData(data);
}

void ShMover::visit(ShDimAligned *dimAligned) {

	ShDimAlignedData data = dimAligned->getData();

	data.firstOrigin.move(this->disX, this->disY);
	data.firstDim.move(this->disX, this->disY);
	data.secondOrigin.move(this->disX, this->disY);
	data.secondDim.move(this->disX, this->disY);
	data.text.move(this->disX, this->disY);

	dimAligned->setData(data);
}

void ShMover::visit(ShDimRadius *dimRadius) {

	ShDimRadiusData data = dimRadius->getData();

	data.center.move(this->disX, this->disY);
	data.dim.move(this->disX, this->disY);
	data.text.move(this->disX, this->disY);

	dimRadius->setData(data);
}

void ShMover::visit(ShDimDiameter *dimDiameter) {

	ShDimDiameterData data = dimDiameter->getData();

	data.center.move(this->disX, this->disY);
	data.firstDim.move(this->disX, this->disY);
	data.secondDim.move(this->disX, this->disY);
	data.text.move(this->disX, this->disY);

	dimDiameter->setData(data);
}

void ShMover::visit(ShDimArcLength *dimArcLength) {

	ShDimArcLengthData data = dimArcLength->getData();

	data.center.move(this->disX, this->disY);
	data.start.move(this->disX, this->disY);
	data.end.move(this->disX, this->disY);
	data.text.move(this->disX, this->disY);
	data.boundary.move(this->disX, this->disY);

	dimArcLength->setData(data);
}

void ShMover::visit(ShDimAngular *dimAngular) {

	ShDimAngularData data = dimAngular->getData();

	data.center.move(this->disX, this->disY);
	data.start.move(this->disX, this->disY);
	data.end.move(this->disX, this->disY);
	data.text.move(this->disX, this->disY);
	data.boundary.move(this->disX, this->disY);

	dimAngular->setData(data);
}

void ShMover::visit(ShConstructionLine *constructionLine) {

	ShPoint3d start = constructionLine->getStart();
	ShPoint3d end = constructionLine->getEnd();

	start.move(this->disX, this->disY);
	end.move(this->disX, this->disY);

	constructionLine->setStart(start);
	constructionLine->setEnd(end);
}

void ShMover::visit(ShBlock* block) {
	// �ƶ���Ļ���
	ShPoint3d basePoint = block->getBasePoint();
	basePoint.move(this->disX, this->disY);
	block->setBasePoint(basePoint);

	// �ƶ����е�����ʵ��
	const QList<ShEntity*>& entities = block->getEntities();
	for (ShEntity* entity : entities) {
		entity->accept(this);
	}
}

////////////////////////////////////////////////////////////////

ShMoverByAxis::ShMoverByAxis(const ShScrollPosition &scroll, const ShPoint3d &prevCenter, const ShPoint3d &currentCenter, double zoomRate)
	:scroll(scroll), prevCenter(prevCenter), currentCenter(currentCenter), zoomRate(zoomRate) {

}

ShMoverByAxis::~ShMoverByAxis() {

}

void ShMoverByAxis::visit(ShLine *line) {

	ShLineData data = line->getData();

	this->convert(data.start, data.start);
	this->convert(data.end, data.end);

	line->setData(data);
}

void ShMoverByAxis::visit(ShCircle *circle) {

	ShCircleData data = circle->getData();

	this->convert(data.center, data.center);

	circle->setData(data);
}

void ShMoverByAxis::visit(ShArc *arc) {

	ShArcData data = arc->getData();

	this->convert(data.center, data.center);

	arc->setData(data);
}

void ShMoverByAxis::visit(ShPoint *point) {

	ShPoint3d position = point->getPosition();

	this->convert(position, position);

	point->setPosition(position);
}

void ShMoverByAxis::visit(ShDot *dot) {

	ShPoint3d position = dot->getPosition();

	this->convert(position, position);

	dot->setPosition(position);
}

void ShMoverByAxis::visit(ShEllipse* ellipse) {
	ShEllipseData data = ellipse->getData();
	this->convert(data.center, data.center);
	ellipse->setData(data);
}

void ShMoverByAxis::visit(ShDimLinear *dimLinear) {

	ShDimLinearData data = dimLinear->getData();

	this->convert(data.firstOrigin, data.firstOrigin);
	this->convert(data.firstDim, data.firstDim);
	this->convert(data.secondOrigin, data.secondOrigin);
	this->convert(data.secondDim, data.secondDim);
	this->convert(data.text, data.text);

	dimLinear->setData(data);
}

void ShMoverByAxis::visit(ShDimAligned *dimAligned) {

	ShDimAlignedData data = dimAligned->getData();

	this->convert(data.firstOrigin, data.firstOrigin);
	this->convert(data.firstDim, data.firstDim);
	this->convert(data.secondOrigin, data.secondOrigin);
	this->convert(data.secondDim, data.secondDim);
	this->convert(data.text, data.text);

	dimAligned->setData(data);
}

void ShMoverByAxis::visit(ShDimRadius *dimRadius) {

	ShDimRadiusData data = dimRadius->getData();

	this->convert(data.center, data.center);
	this->convert(data.dim, data.dim);
	this->convert(data.text, data.text);

	dimRadius->setData(data);
}

void ShMoverByAxis::visit(ShDimDiameter *dimDiameter) {

	ShDimDiameterData data = dimDiameter->getData();

	this->convert(data.center, data.center);
	this->convert(data.firstDim, data.firstDim);
	this->convert(data.secondDim, data.secondDim);
	this->convert(data.text, data.text);

	dimDiameter->setData(data);
}

void ShMoverByAxis::visit(ShDimArcLength *dimArcLength) {

	ShDimArcLengthData data = dimArcLength->getData();

	this->convert(data.center, data.center);
	this->convert(data.start, data.start);
	this->convert(data.end, data.end);
	this->convert(data.text, data.text);
	this->convert(data.boundary, data.boundary);

	dimArcLength->setData(data);
}

void ShMoverByAxis::visit(ShDimAngular *dimAngular) {

	ShDimAngularData data = dimAngular->getData();

	this->convert(data.center, data.center);
	this->convert(data.start, data.start);
	this->convert(data.end, data.end);
	this->convert(data.text, data.text);
	this->convert(data.boundary, data.boundary);

	dimAngular->setData(data);
}

void ShMoverByAxis::visit(ShConstructionLine *constructionLine) {

	ShLineData data = constructionLine->getData();

	this->convert(data.start, data.start);
	this->convert(data.end, data.end);

	constructionLine->setData(data);
}

void ShMoverByAxis::convert(const ShPoint3d &point, ShPoint3d &converted) {

	ShPoint3d temp;

	temp.x = (point.x*this->zoomRate) - this->scroll.horizontal + (this->prevCenter.x*this->zoomRate);
	temp.y = -1 * ((point.y*this->zoomRate) + this->scroll.vertical - (this->prevCenter.y*this->zoomRate));

	converted.x = (temp.x + this->scroll.horizontal - (this->currentCenter.x*this->zoomRate)) / this->zoomRate;
	converted.y = (-1 * (temp.y + this->scroll.vertical - (this->currentCenter.y*this->zoomRate))) / this->zoomRate;
}

void ShMoverByAxis::visit(ShBlock* block) {
	// ת����Ļ���
	ShPoint3d basePoint = block->getBasePoint();
	this->convert(basePoint, basePoint);
	block->setBasePoint(basePoint);

	// ת�����е�����ʵ��
	const QList<ShEntity*>& entities = block->getEntities();
	for (ShEntity* entity : entities) {
		entity->accept(this);
	}
}