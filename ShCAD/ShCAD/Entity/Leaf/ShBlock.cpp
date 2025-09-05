#include "ShBlock.h"
#include "Entity/Private/Algorithm/ShVisitor.h"

ShBlock::ShBlock() {
    // Ĭ�Ϲ��캯��
}

ShBlock::ShBlock(const ShPoint3d& basePoint)
    : basePoint(basePoint) {
    // ʹ�û����ʼ����
}

ShBlock::ShBlock(const ShPoint3d& basePoint, const ShPropertyData& propertyData, ShLayer* layer)
    : ShLeaf(propertyData, layer), basePoint(basePoint) {
    // ʹ�û��㡢���Ժ�ͼ���ʼ����
}

ShBlock::ShBlock(const ShBlock& other)
    : ShLeaf(other), basePoint(other.basePoint) {
    // ���������ʵ��
    for (ShEntity* entity : other.entities) {
        this->entities.append(entity->clone());
    }
}

ShBlock::~ShBlock() {
    // �ͷ�������ʵ����ڴ�
    qDeleteAll(this->entities);
    this->entities.clear();
}

ShBlock& ShBlock::operator=(const ShBlock& other) {
    if (this != &other) {
        ShLeaf::operator=(other);
        this->basePoint = other.basePoint;

        // �ͷ�����ʵ��
        qDeleteAll(this->entities);
        this->entities.clear();

        // �����ʵ��
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