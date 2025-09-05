

#ifndef _SHDIMDIAMETER_H
#define _SHDIMDIAMETER_H

#include "ShDim.h"

class ShDimDiameter : public ShDim {

protected:
	ShDimDiameterData data;

public:
	ShDimDiameter(const ShDimDiameterData &data, const ShPropertyData &propertyData, ShLayer *layer, ShDimensionStyle *dimensionStyle);
	ShDimDiameter(const ShDimDiameter &other);
	~ShDimDiameter();
	ShDimDiameter& operator=(const ShDimDiameter &other);

	virtual ShDimDiameter* clone();
	virtual void accept(ShVisitor *visitor);

	virtual void updateChild();
	virtual QJsonObject getJsonData() const {
		QJsonObject json;
		QJsonObject detailData;

		// Ìî³ä center
		QJsonObject centerPoint;
		centerPoint["x"] = this->data.center.x;
		centerPoint["y"] = this->data.center.y;
		centerPoint["z"] = this->data.center.z;
		detailData["center"] = centerPoint;

		// Ìî³ä text
		QJsonObject textPoint;
		textPoint["x"] = this->data.text.x;
		textPoint["y"] = this->data.text.y;
		textPoint["z"] = this->data.text.z;
		detailData["text"] = textPoint;

		// Ìî³ä firstDim
		QJsonObject firstDimPoint;
		firstDimPoint["x"] = this->data.firstDim.x;
		firstDimPoint["y"] = this->data.firstDim.y;
		firstDimPoint["z"] = this->data.firstDim.z;
		detailData["firstDim"] = firstDimPoint;

		// Ìî³ä secondDim
		QJsonObject secondDimPoint;
		secondDimPoint["x"] = this->data.secondDim.x;
		secondDimPoint["y"] = this->data.secondDim.y;
		secondDimPoint["z"] = this->data.secondDim.z;
		detailData["secondDim"] = secondDimPoint;

		// ÉèÖÃÀàÐÍÎª "DimDiameter"
		json["type"] = "DimDiameter";
		json["data"] = detailData;

		return json;
	}
public:
	void setData(const ShDimDiameterData &data);
	const ShDimDiameterData& getData() const { return this->data; }

	double getRadius();
	double getDiameter();
	void getFirstArrowPoints(ShPoint3d &vertex, ShPoint3d &vertex2, ShPoint3d &vertex3);
	void getSecondArrowPoints(ShPoint3d &vertex, ShPoint3d &vertex2, ShPoint3d &vertex3);
	bool isCenterToTextDistanceShorterThanRadius();
};

#endif //_SHDIMDIAMETER_H