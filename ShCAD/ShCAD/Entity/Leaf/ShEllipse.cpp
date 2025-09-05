#include "ShEllipse.h"
#include "Entity\Private\Algorithm\ShVisitor.h"

ShEllipse::ShEllipse() {

}

ShEllipse::ShEllipse(const ShPropertyData& propertyData, const ShEllipseData& data, ShLayer* layer)
    : ShLeaf(propertyData, layer), data(data) {

}

ShEllipse::ShEllipse(const ShEllipseData& data)
    : data(data) {

}

ShEllipse::ShEllipse(const ShPoint3d& center, double majorRadius, double minorRadius, double angle)
    : data(ShEllipseData(center, majorRadius, minorRadius, angle)) {

}

ShEllipse::ShEllipse(const ShEllipse& other)
    : ShLeaf(other), data(other.data) {

}

ShEllipse::~ShEllipse() {

}

ShEllipse& ShEllipse::operator=(const ShEllipse& other) {
    ShLeaf::operator=(other);
    this->data = other.data;
    return *this;
}

ShEllipse* ShEllipse::clone() {
    return new ShEllipse(*this);
}

void ShEllipse::accept(ShVisitor* visitor) {
    visitor->visit(this);
}