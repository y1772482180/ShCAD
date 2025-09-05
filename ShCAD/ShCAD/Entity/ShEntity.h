

#ifndef _SHENTITY_H
#define _SHENTITY_H
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "Private\ShEntityData.h"
#include "Data\ShPropertyData.h"


//Composite Pattern, Visitor Pattern, Prototype Pattern

class ShLayer;
class ShVisitor;


class ShEntity {

protected:
	ShPropertyData propertyData;
	bool selected;
	ShLayer *layer;
	//ShEntity* farther = nullptr;
	//vector<ShEntity*>father;
	//unsigned int shEId = 0;

public:
	ShEntity();
	ShEntity(const ShPropertyData &propertyData, ShLayer *layer);
	ShEntity(const ShEntity &other);
	virtual ~ShEntity() = 0;
	ShEntity& operator=(const ShEntity &other);


	virtual ShEntity* clone() = 0;
	virtual void accept(ShVisitor *visitor) = 0;

	virtual void select();
	virtual void unSelect();

	void setPropertyData(const ShPropertyData &propertyData) { this->propertyData = propertyData; }
	void setLayer(ShLayer *layer) { this->layer = layer; }

	inline const ShPropertyData& getPropertyData() const { return this->propertyData; }
	inline bool isSelected() const { return this->selected; }
	inline ShLayer* getLayer() const { return this->layer; }

};

#endif //_SHENTITY_H