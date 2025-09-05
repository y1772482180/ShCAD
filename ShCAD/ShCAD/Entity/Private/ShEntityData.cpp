
#include "ShEntityData.h"

ShEntityData::ShEntityData() {

}

ShEntityData::ShEntityData(const ShEntityData &other) {

}

ShEntityData::~ShEntityData() {}

ShEntityData& ShEntityData::operator=(const ShEntityData &other) {

	return *this;
}

////////////////////////////////////////////////////////////

ShLineData::ShLineData() {

}

ShLineData::ShLineData(const ShPoint3d &start, const ShPoint3d &end)
	:start(start), end(end) {

}

ShLineData::ShLineData(const ShLineData &other)
	: start(other.start), end(other.end) {

}

ShLineData::~ShLineData() {}

ShLineData& ShLineData::operator=(const ShLineData &other) {

	this->start = other.start;
	this->end = other.end;

	return *this;
}

/////////////////////////////////////////////////////////////////

ShCircleData::ShCircleData()
	:radius(0) {

}

ShCircleData::ShCircleData(const ShPoint3d &center, double radius)
	: center(center), radius(radius) {

}

ShCircleData::ShCircleData(const ShCircleData &other)
	: center(other.center), radius(other.radius) {

}

ShCircleData::~ShCircleData() {

}

ShCircleData& ShCircleData::operator=(const ShCircleData &other) {

	this->center = other.center;
	this->radius = other.radius;

	return *this;
}

//////////////////////////////////////////////////////////////////

ShArcData::ShArcData()
	:radius(0), startAngle(0), endAngle(0) {

}

ShArcData::ShArcData(const ShPoint3d &center, double radius, double startAngle, double endAngle)
	: center(center), radius(radius), startAngle(startAngle), endAngle(endAngle) {

}

ShArcData::ShArcData(const ShArcData &other)
	: center(other.center), radius(other.radius), startAngle(other.startAngle), endAngle(other.endAngle) {

}

ShArcData::~ShArcData() {

}

ShArcData& ShArcData::operator=(const ShArcData &other) {

	this->center = other.center;
	this->radius = other.radius;
	this->startAngle = other.startAngle;
	this->endAngle = other.endAngle;

	return *this;
}


////////////////////////////////////////////////////////////////////////

ShDimLinearData::ShDimLinearData() {

}

ShDimLinearData::ShDimLinearData(const ShPoint3d &firstOrigin, const ShPoint3d &secondOrigin, const ShPoint3d &firstDim, const ShPoint3d &secondDim,
	const ShPoint3d &text)
	:firstOrigin(firstOrigin), secondOrigin(secondOrigin), firstDim(firstDim), secondDim(secondDim), text(text) {

}

ShDimLinearData::ShDimLinearData(const ShDimLinearData &other)
	: firstOrigin(other.firstOrigin), secondOrigin(other.secondOrigin), firstDim(other.firstDim), secondDim(other.secondDim), text(other.text) {

}

ShDimLinearData::~ShDimLinearData() {

}

ShDimLinearData& ShDimLinearData::operator=(const ShDimLinearData &other) {

	this->firstOrigin = other.firstOrigin;
	this->secondOrigin = other.secondOrigin;
	this->firstDim = other.firstDim;
	this->secondDim = other.secondDim;
	this->text = other.text;

	return *this;
}
//////////////////////////////////////////////////////////////////

ShEllipseData::ShEllipseData()
	: majorRadius(0), minorRadius(0), angle(0) {

}

ShEllipseData::ShEllipseData(const ShPoint3d& center, double majorRadius, double minorRadius, double angle)
	: center(center), majorRadius(majorRadius), minorRadius(minorRadius), angle(angle) {

}

ShEllipseData::ShEllipseData(const ShEllipseData& other)
	: center(other.center), majorRadius(other.majorRadius),
	minorRadius(other.minorRadius), angle(other.angle) {

}

ShEllipseData::~ShEllipseData() {

}

ShEllipseData& ShEllipseData::operator=(const ShEllipseData& other) {
	this->center = other.center;
	this->majorRadius = other.majorRadius;
	this->minorRadius = other.minorRadius;
	this->angle = other.angle;

	return *this;
}

//////////////////////////////////////////////////////////////////////////////

ShDimAlignedData::ShDimAlignedData() {

}

ShDimAlignedData::ShDimAlignedData(const ShPoint3d &firstOrigin, const ShPoint3d &secondOrigin, const ShPoint3d &firstDim, const ShPoint3d &secondDim,
	const ShPoint3d &text)
	:firstOrigin(firstOrigin), secondOrigin(secondOrigin), firstDim(firstDim), secondDim(secondDim), text(text) {

}

ShDimAlignedData::ShDimAlignedData(const ShDimAlignedData &other)
	: firstOrigin(other.firstOrigin), secondOrigin(other.secondOrigin), firstDim(other.firstDim), secondDim(other.secondDim), text(other.text) {

}

ShDimAlignedData::~ShDimAlignedData() {

}

ShDimAlignedData& ShDimAlignedData::operator=(const ShDimAlignedData &other) {

	this->firstOrigin = other.firstOrigin;
	this->secondOrigin = other.secondOrigin;
	this->firstDim = other.firstDim;
	this->secondDim = other.secondDim;
	this->text = other.text;

	return *this;
}

////////////////////////////////////////////////////////////////

ShDimRadiusData::ShDimRadiusData() {

}

ShDimRadiusData::ShDimRadiusData(const ShPoint3d &center, const ShPoint3d &dim, const ShPoint3d &text)
	:center(center), dim(dim), text(text) {

}

ShDimRadiusData::ShDimRadiusData(const ShDimRadiusData &other)
	: center(other.center), dim(other.dim), text(other.text) {

}

ShDimRadiusData::~ShDimRadiusData() {

}

ShDimRadiusData& ShDimRadiusData::operator=(const ShDimRadiusData &other) {

	this->center = other.center;
	this->dim = other.dim;
	this->text = other.text;

	return *this;
}

//////////////////////////////////////////////////////////////

ShDimDiameterData::ShDimDiameterData() {

}

ShDimDiameterData::ShDimDiameterData(const ShPoint3d &center, const ShPoint3d &firstDim, const ShPoint3d &secondDim, const ShPoint3d &text)
	:center(center), firstDim(firstDim), secondDim(secondDim), text(text) {

}

ShDimDiameterData::ShDimDiameterData(const ShDimDiameterData &other)
	: center(other.center), firstDim(other.firstDim), secondDim(other.secondDim), text(other.text) {

}

ShDimDiameterData::~ShDimDiameterData() {

}

ShDimDiameterData& ShDimDiameterData::operator=(const ShDimDiameterData &other) {

	this->center = other.center;
	this->firstDim = other.firstDim;
	this->secondDim = other.secondDim;
	this->text = other.text;
	
	return *this;
}

/////////////////////////////////////////////////////////////////

ShDimArcLengthData::ShDimArcLengthData() {

}

ShDimArcLengthData::ShDimArcLengthData(const ShPoint3d &center, double radius, const ShPoint3d &start, const ShPoint3d &end, const ShPoint3d &boundary, const ShPoint3d &text)
	:center(center),radius(radius), start(start), end(end), boundary(boundary), text(text) {

}

ShDimArcLengthData::ShDimArcLengthData(const ShDimArcLengthData &other)
	: center(other.center), radius(other.radius), start(other.start), end(other.end), boundary(other.boundary), text(other.text) {

}

ShDimArcLengthData::~ShDimArcLengthData() {

}

ShDimArcLengthData& ShDimArcLengthData::operator=(const ShDimArcLengthData &other) {

	this->center = other.center;
	this->radius = other.radius;
	this->start = other.start;
	this->end = other.end;
	this->boundary = other.boundary;
	this->text = other.text;
	
	return *this;
}

//////////////////////////////////////////////////////////////////////////

ShDimAngularData::ShDimAngularData() {

}

ShDimAngularData::ShDimAngularData(const ShPoint3d &center, const ShPoint3d &start, const ShPoint3d &end, const ShPoint3d &boundary, const ShPoint3d &text)
	:center(center), start(start), end(end), boundary(boundary), text(text) {

}

ShDimAngularData::ShDimAngularData(const ShDimAngularData &other)
	: center(other.center), start(other.start), end(other.end), boundary(other.boundary), text(other.text) {

}

ShDimAngularData::~ShDimAngularData() {

}

ShDimAngularData& ShDimAngularData::operator=(const ShDimAngularData &other) {

	this->center = other.center;
	this->start = other.start;
	this->end = other.end;
	this->boundary = other.boundary;
	this->text = other.text;

	return *this;
}