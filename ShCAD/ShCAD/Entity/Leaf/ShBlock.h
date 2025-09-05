#ifndef _ShBlock_H
#define _ShBlock_H

#include "ShLeaf.h"
#include "Data/ShPoint3d.h"
#include <QList>

class ShEntity;

class ShBlock : public ShLeaf {
protected:
    ShPoint3d basePoint;  // ��Ļ���
    QList<ShEntity*> entities;  // ���а�����ʵ��

public:
    ShBlock();
    ShBlock(const ShPoint3d& basePoint);
    ShBlock(const ShPoint3d& basePoint, const ShPropertyData& propertyData, ShLayer* layer);
    ShBlock(const ShBlock& other);
    ~ShBlock();

    ShBlock& operator=(const ShBlock& other);

    virtual ShBlock* clone() override;
    virtual void accept(ShVisitor* visitor) override;

public:
    // ���úͻ�ȡ����
    void setBasePoint(const ShPoint3d& basePoint) { this->basePoint = basePoint; }
    const ShPoint3d& getBasePoint() const { return this->basePoint; }

    // ������е�ʵ��
    void addEntity(ShEntity* entity);
    void removeEntity(ShEntity* entity);
    void clearEntities();

    // ��ȡ���е�ʵ���б�
    const QList<ShEntity*>& getEntities() const { return this->entities; }
    int getEntityCount() const { return this->entities.size(); }
};

#endif //_ShBlock_H