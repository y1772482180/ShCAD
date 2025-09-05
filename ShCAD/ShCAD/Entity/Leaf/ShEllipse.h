#ifndef _SHELLIPSE_H
#define _SHELLIPSE_H

#include "ShLeaf.h"

class ShEllipse : public ShLeaf {

protected:
    ShEllipseData data;

public:
    ShEllipse();
    ShEllipse(const ShPropertyData& propertyData, const ShEllipseData& data, ShLayer* layer);
    ShEllipse(const ShEllipseData& data);
    ShEllipse(const ShPoint3d& center, double majorRadius, double minorRadius, double angle);
    ShEllipse(const ShEllipse& other);
    ~ShEllipse();
    ShEllipse& operator=(const ShEllipse& other);

    virtual ShEllipse* clone();
    virtual void accept(ShVisitor* visitor);

public:
    void setCenter(const ShPoint3d& center) { this->data.center = center; }
    void setMajorRadius(double majorRadius) { this->data.majorRadius = majorRadius; }
    void setMinorRadius(double minorRadius) { this->data.minorRadius = minorRadius; }
    void setAngle(double angle) { this->data.angle = angle; }
    void setData(const ShEllipseData& data) { this->data = data; }
	inline const QJsonObject getJsonData() const {
		QJsonObject json;

		// 添加椭圆中心点坐标
		QJsonObject centerPoint;
		centerPoint["x"] = this->data.center.x;
		centerPoint["y"] = this->data.center.y;
		centerPoint["z"] = this->data.center.z;  // 如果是3D坐标
		json["center"] = centerPoint;

		// 添加椭圆参数
		json["majorRadius"] = this->data.majorRadius;
		json["minorRadius"] = this->data.minorRadius;
		json["angle"] = this->data.angle;  // 旋转角度

		QJsonObject data;
		data["type"] = "Ellipse";
		data["data"] = json;

		return data;
	}

public:
    inline const ShPoint3d& getCenter() const { return this->data.center; }
    inline double getMajorRadius() const { return this->data.majorRadius; }
    inline double getMinorRadius() const { return this->data.minorRadius; }
    inline double getAngle() const { return this->data.angle; }
    inline const ShEllipseData& getData() const { return this->data; }
};

#endif //_SHELLIPSE_H