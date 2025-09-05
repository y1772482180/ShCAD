
#ifndef _SHCIRCLE_H
#define _SHCIRCLE_H

#include "ShLeaf.h"

class ShCircle : public ShLeaf {

protected:
	ShCircleData data;

public:
	ShCircle();
	ShCircle(const ShPropertyData &propertyData, const ShCircleData &data, ShLayer *layer);
	ShCircle(const ShCircleData &data);
	ShCircle(const ShPoint3d &center, double radius);
	ShCircle(const ShCircle &other);
	~ShCircle();
	ShCircle& operator=(const ShCircle &other);

	virtual ShCircle* clone();
	virtual void accept(ShVisitor *visitor);
	inline const QJsonObject getJsonData() const {
		QJsonObject json;

		// 添加圆心坐标
		QJsonObject centerPoint;
		centerPoint["x"] = this->data.center.x;
		centerPoint["y"] = this->data.center.y;
		json["center"] = centerPoint;

		// 添加半径
		json["radius"] = this->data.radius;
		QJsonObject data;
		data["type"] = "Circle";
		data["data"] = json;
		// 转换为紧凑格式的 JSON 字符串
		return data;
	}
public:
	void setCenter(const ShPoint3d &center) { this->data.center = center; }
	void setRadius(double radius) { this->data.radius = radius; }
	void setData(const ShCircleData &data) { this->data = data; }

public:
	inline const ShPoint3d& getCenter() const { return this->data.center; }
	inline double getRadius() const { return this->data.radius; }
	inline const ShCircleData& getData() const { return this->data; }
};

#endif //_SHCIRCLE_H