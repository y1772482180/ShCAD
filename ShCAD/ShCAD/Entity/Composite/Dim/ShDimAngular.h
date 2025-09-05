
#ifndef _SHDIMANGULAR_H
#define _SHDIMANGULAR_H

#include "ShDim.h"

class ShDimAngular : public ShDim {

protected:
	ShDimAngularData data;

public:
	ShDimAngular(const ShDimAngularData &data, const ShPropertyData &propertyData, ShLayer *layer, ShDimensionStyle *dimensionStyle);
	ShDimAngular(const ShDimAngular &other);
	~ShDimAngular();
	ShDimAngular& operator=(const ShDimAngular &other);

	virtual ShDimAngular* clone();
	virtual void accept(ShVisitor *visitor);

	virtual void updateChild();
	virtual QJsonObject getJsonData() const {
		QJsonObject json;
		QJsonObject Detaildata;

		// Ìî³ä center
		QJsonObject centerPoint;
		centerPoint["x"] = this->data.center.x;
		centerPoint["y"] = this->data.center.y;
		centerPoint["z"] = this->data.center.z;
		Detaildata["center"] = centerPoint;

		// Ìî³ä start
		QJsonObject startPoint;
		startPoint["x"] = this->data.start.x;
		startPoint["y"] = this->data.start.y;
		startPoint["z"] = this->data.start.z;
		Detaildata["start"] = startPoint;

		// Ìî³ä end
		QJsonObject endPoint;
		endPoint["x"] = this->data.end.x;
		endPoint["y"] = this->data.end.y;
		endPoint["z"] = this->data.end.z;
		Detaildata["end"] = endPoint;

		// Ìî³ä boundary
		QJsonObject boundaryPoint;
		boundaryPoint["x"] = this->data.boundary.x;
		boundaryPoint["y"] = this->data.boundary.y;
		boundaryPoint["z"] = this->data.boundary.z;
		Detaildata["boundary"] = boundaryPoint;

		// Ìî³ä text
		QJsonObject textPoint;
		textPoint["x"] = this->data.text.x;
		textPoint["y"] = this->data.text.y;
		textPoint["z"] = this->data.text.z;
		Detaildata["text"] = textPoint;

		// ÉèÖÃÀàÐÍÎª "DimAngular"
		json["type"] = "DimAngular";
		json["data"] = Detaildata;

		return json;
	}

public:
	void setData(const ShDimAngularData &data);
	const ShDimAngularData& getData() const { return this->data; }

	double getArcRadius();
	ShPoint3d getArcStart();
	ShPoint3d getArcEnd();
	double getStartAngle();
	double getEndAngle();
	double getAngle();
	void getFirstArrowPoints(ShPoint3d &vertex, ShPoint3d &vertex2, ShPoint3d &vertex3);
	void getSecondArrowPoints(ShPoint3d &vertex, ShPoint3d &vertex2, ShPoint3d &vertex3);

};

#endif //_SHDIMANGULAR_H