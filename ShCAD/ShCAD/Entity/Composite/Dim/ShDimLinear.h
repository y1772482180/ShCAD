
#ifndef _SHDIMLINEAR_H
#define _SHDIMLINEAR_H

#include "ShDim.h"

class ShLine;

class ShDimLinear : public ShDim {

protected:
	ShDimLinearData data;

public:
	ShDimLinear(const ShDimLinearData &data, const ShPropertyData &propertyData, ShLayer *layer, ShDimensionStyle *dimensionStyle);
	ShDimLinear(const ShDimLinear &other);
	~ShDimLinear();
	ShDimLinear& operator=(const ShDimLinear &other);

	virtual ShDimLinear* clone();
	virtual void accept(ShVisitor *visitor);

	virtual void updateChild();
	virtual QJsonObject getJsonData() const {
		QJsonObject json;
		QJsonObject detailData;

		// Ìî³ä firstOrigin
		QJsonObject firstOriginPoint;
		firstOriginPoint["x"] = this->data.firstOrigin.x;
		firstOriginPoint["y"] = this->data.firstOrigin.y;
		firstOriginPoint["z"] = this->data.firstOrigin.z;
		detailData["firstOrigin"] = firstOriginPoint;

		// Ìî³ä secondOrigin
		QJsonObject secondOriginPoint;
		secondOriginPoint["x"] = this->data.secondOrigin.x;
		secondOriginPoint["y"] = this->data.secondOrigin.y;
		secondOriginPoint["z"] = this->data.secondOrigin.z;
		detailData["secondOrigin"] = secondOriginPoint;

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

		// Ìî³ä text
		QJsonObject textPoint;
		textPoint["x"] = this->data.text.x;
		textPoint["y"] = this->data.text.y;
		textPoint["z"] = this->data.text.z;
		detailData["text"] = textPoint;

		// ÉèÖÃÀàÐÍÎª "DimLinear"
		json["type"] = "DimLinear";
		json["data"] = detailData;

		return json;
	}
public:
	void setData(const ShDimLinearData &data);
	const ShDimLinearData& getData() { return this->data; }

	double getDistance();
	void getFirstArrowPoints(ShPoint3d &vertex, ShPoint3d &vertex2, ShPoint3d &vertex3);
	void getSecondArrowPoints(ShPoint3d &vertex, ShPoint3d &vertex2, ShPoint3d &vertex3);
};

#endif //_SHDIMLINEAR_H