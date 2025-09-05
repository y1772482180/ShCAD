#include "ShBlock.h"
#include "Entity/Private/Algorithm/ShVisitor.h"

ShBlock::ShBlock() {
    // 默认构造函数
}

ShBlock::ShBlock(const ShPoint3d& basePoint)
    : basePoint(basePoint) {
    // 使用基点初始化块
}

ShBlock::ShBlock(const ShPoint3d& basePoint, const ShPropertyData& propertyData, ShLayer* layer)
    : ShLeaf(propertyData, layer), basePoint(basePoint) {
    // 使用基点、属性和图层初始化块
}

ShBlock::ShBlock(const ShBlock& other)
    : ShLeaf(other), basePoint(other.basePoint) {
    // 深拷贝所有子实体
    for (ShEntity* entity : other.entities) {
        this->entities.append(entity->clone());
    }
}

ShBlock::~ShBlock() {
    // 释放所有子实体的内存
    qDeleteAll(this->entities);
    this->entities.clear();
}

ShBlock& ShBlock::operator=(const ShBlock& other) {
    if (this != &other) {
        ShLeaf::operator=(other);
        this->basePoint = other.basePoint;

        // 释放现有实体
        qDeleteAll(this->entities);
        this->entities.clear();

        // 深拷贝新实体
        for (ShEntity* entity : other.entities) {
            this->entities.append(entity->clone());
        }
    }
    return *this;
}

ShBlock* ShBlock::clone() {
    return new ShBlock(*this);
}

void ShBlock::accept(ShVisitor* visitor) {
    visitor->visit(this);
}

void ShBlock::addEntity(ShEntity* entity) {
    if (entity != nullptr && !this->entities.contains(entity)) {
        this->entities.append(entity);
    }
}

void ShBlock::removeEntity(ShEntity* entity) {
    if (this->entities.removeOne(entity)) {
        delete entity;
    }
}

void ShBlock::clearEntities() {
    qDeleteAll(this->entities);
    this->entities.clear();
}