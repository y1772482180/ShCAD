
#ifndef _SHDIMARCLENGTH_H
#define _SHDIMARCLENGTH_H

#include "ShDim.h"

class ShDimArcLength : public ShDim {

protected:
	ShDimArcLengthData data;

public:
	ShDimArcLength(const ShDimArcLengthData &data, const ShPropertyData &propertyData, ShLayer *layer, ShDimensionStyle *dimensionStyle);
	ShDimArcLength(const ShDimArcLength &other);
	~ShDimArcLength();
	ShDimArcLength& operator=(const ShDimArcLength &other);

	virtual ShDimArcLength* clone();
	virtual void accept(ShVisitor *visitor);

	virtual void updateChild();
	virtual QJsonObject getJsonData() const {
		QJsonObject json;
		QJsonObject detailData;

		// 野割 center
		QJsonObject centerPoint;
		centerPoint["x"] = this->data.center.x;
		centerPoint["y"] = this->data.center.y;
		centerPoint["z"] = this->data.center.z;
		detailData["center"] = centerPoint;

		// 野割 radius
		detailData["radius"] = this->data.radius;

		// 野割 start
		QJsonObject startPoint;
		startPoint["x"] = this->data.start.x;
		startPoint["y"] = this->data.start.y;
		startPoint["z"] = this->data.start.z;
		detailData["start"] = startPoint;

		// 野割 end
		QJsonObject endPoint;
		endPoint["x"] = this->data.end.x;
		endPoint["y"] = this->data.end.y;
		endPoint["z"] = this->data.end.z;
		detailData["end"] = endPoint;

		// 野割 boundary
		QJsonObject boundaryPoint;
		boundaryPoint["x"] = this->data.boundary.x;
		boundaryPoint["y"] = this->data.boundary.y;
		boundaryPoint["z"] = this->data.boundary.z;
		detailData["boundary"] = boundaryPoint;

		// 野割 text
		QJsonObject textPoint;
		textPoint["x"] = this->data.text.x;
		textPoint["y"] = this->data.text.y;
		textPoint["z"] = this->data.text.z;
		detailData["text"] = textPoint;

		// 譜崔窃侏葎 "DimArcLength"
		json["type"] = "DimArcLength";
		json["data"] = detailData;

		return json;
	}
public:
	void setData(const ShDimArcLengthData &data);
	const ShDimArcLengthData& getData() const { return this->data; }

	double getArcRadius();
	ShPoint3d getArcStart();
	ShPoint3d getArcEnd();
	double getStartAngle();
	double getEndAngle();
	double getArcLength();
	void getFirstArrowPoints(ShPoint3d &vertex, ShPoint3d &vertex2, ShPoint3d &vertex3);
	void getSecondArrowPoints(ShPoint3d &vertex, ShPoint3d &vertex2, ShPoint3d &vertex3);

};

#endif //_SHDIMARCLENGTH_H