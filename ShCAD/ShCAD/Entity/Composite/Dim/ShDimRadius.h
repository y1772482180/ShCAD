
#ifndef _SHDIMRADIUS_H
#define _SHDIMRADIUS_H

#include "ShDim.h"

class ShDimRadius : public ShDim {

protected:
	ShDimRadiusData data;

public:
	ShDimRadius(const ShDimRadiusData &data, const ShPropertyData &propertyData, ShLayer *layer, ShDimensionStyle *dimensionStyle);
	ShDimRadius(const ShDimRadius &other);
	~ShDimRadius();
	ShDimRadius& operator=(const ShDimRadius &other);

	virtual ShDimRadius* clone();
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

		// Ìî³ä dim
		QJsonObject dimPoint;
		dimPoint["x"] = this->data.dim.x;
		dimPoint["y"] = this->data.dim.y;
		dimPoint["z"] = this->data.dim.z;
		detailData["dim"] = dimPoint;

		// ÉèÖÃÀàÐÍÎª "DimRadius"
		json["type"] = "DimRadius";
		json["data"] = detailData;

		return json;
	}
public:
	void setData(const ShDimRadiusData &data);
	const ShDimRadiusData& getData() const { return this->data; }

	double getRadius();
	void getArrowPoints(ShPoint3d &vertex, ShPoint3d &vertex2, ShPoint3d &vertex3);
};

#endif //_SHDIMRADIUS_H