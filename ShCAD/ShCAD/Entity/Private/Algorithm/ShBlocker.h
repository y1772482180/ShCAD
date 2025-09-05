#ifndef _ShBlocker_H
#define _ShBlocker_H

#include "entity/ShEntity.h"
#include "Data/ShPoint3d.h"
#include <QLinkedList>

class ShBlocker : public ShEntity {

private:
    ShPoint3d basePoint;
    QLinkedList<ShEntity*> entities;

public:
    ShBlocker(const ShPoint3d& basePoint);
    ~ShBlocker();

    virtual ShBlock* clone() const;
    virtual void accept(ShVisitor* visitor);

    void addEntity(ShEntity* entity);
    const QLinkedList<ShEntity*>& getEntities() const { return entities; }
    ShPoint3d getBasePoint() const { return basePoint; }
    void setBasePoint(const ShPoint3d& point) { basePoint = point; }

    // 其他必要的方法...
};

#endif //_ShBlocker_H