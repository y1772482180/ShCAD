#include "Entity/Private/Algorithm/ShBlocker.h"
#include "Entity/Private/Algorithm/ShVisitor.h"
#include"entity/ShEntity.h"
#include "Entity/Leaf/ShBlock.h"
ShBlocker::ShBlocker(const ShPoint3d& basePoint)
    : basePoint(basePoint) {
}

ShBlocker::~ShBlocker() {
    qDeleteAll(entities);
};

//ShBlock* ShBlocker::clone() const {
//    ShBlock* block = new ShBlock(this->basePoint);
//    for (ShEntity* entity : entities) {
//        block->addEntity(entity->clone());
//    }
//    return block;
//}

void ShBlocker::accept(ShVisitor* visitor) {
    //visitor->visit(this);
}

void ShBlocker::addEntity(ShEntity* entity) {
    entities.append(entity);
}