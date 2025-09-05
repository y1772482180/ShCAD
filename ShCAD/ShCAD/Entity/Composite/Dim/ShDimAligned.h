
#ifndef _SHDIMALIGNED_H
#define _SHDIMALIGNED_H

#include "ShDim.h"

class ShLine;

class ShDimAligned : public ShDim {

protected:
	ShDimAlignedData data;

public:
	ShDimAligned(const ShDimAlignedData &data, const ShPropertyData &propertyData, ShLayer *layer, ShDimensionStyle *dimensionStyle);
	ShDimAligned(const ShDimAligned &other);
	~ShDimAligned();
	ShDimAligned& operator=(const ShDimAligned &other);

	virtual ShDimAligned* clone();
	virtual void accept(ShVisitor *visitor);

	virtual void updateChild();
	virtual QJsonObject getJsonData() const {
		QJsonObject json;
		QJsonObject Detaildata;

		// Ìî³ä firstOrigin
		QJsonObject firstOriginPoint;
		firstOriginPoint["x"] = this->data.firstOrigin.x;
		firstOriginPoint["y"] = this->data.firstOrigin.y;
		firstOriginPoint["z"] = this->data.firstOrigin.z;
		Detaildata["firstOrigin"] = firstOriginPoint;

		// Ìî³ä secondOrigin
		QJsonObject secondOriginPoint;
		secondOriginPoint["x"] = this->data.secondOrigin.x;
		secondOriginPoint["y"] = this->data.secondOrigin.y;
		secondOriginPoint["z"] = this->data.secondOrigin.z;
		Detaildata["secondOrigin"] = secondOriginPoint;

		// Ìî³ä firstDim
		QJsonObject firstDimPoint;
		firstDimPoint["x"] = this->data.firstDim.x;
		firstDimPoint["y"] = this->data.firstDim.y;
		firstDimPoint["z"] = this->data.firstDim.z;
		Detaildata["firstDim"] = firstDimPoint;

		// Ìî³ä secondDim
		QJsonObject secondDimPoint;
		secondDimPoint["x"] = this->data.secondDim.x;
		secondDimPoint["y"] = this->data.secondDim.y;
		secondDimPoint["z"] = this->data.secondDim.z;
		Detaildata["secondDim"] = secondDimPoint;

		// Ìî³ä text
		QJsonObject textPoint;
		textPoint["x"] = this->data.text.x;
		textPoint["y"] = this->data.text.y;
		textPoint["z"] = this->data.text.z;
		Detaildata["text"] = textPoint;

		// ÉèÖÃÀàÐÍÎª "DimAligned"
		json["type"] = "DimAligned";
		json["data"] = Detaildata;

		return json;
	}
public:
	void setData(const ShDimAlignedData &data);
	const ShDimAlignedData& getData() { return this->data; }

	double getDistance();
	void getFirstArrowPoints(ShPoint3d &vertex, ShPoint3d &vertex2, ShPoint3d &vertex3);
	void getSecondArrowPoints(ShPoint3d &vertex, ShPoint3d &vertex2, ShPoint3d &vertex3);
};

#endif //_SHDIMALIGNED_H