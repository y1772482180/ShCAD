// ShCircleParamDialog.cpp
#include "ShCircleParamDialog.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDoubleSpinBox>

ShCircleParamDialog::ShCircleParamDialog(QWidget* parent)
    : QDialog(parent), currentMode(CenterInput) {
    this->setWindowTitle("Circle Parameters");
    this->setFixedSize(350, 150);
    this->createLayout();
    this->connectSignals();
}

ShCircleParamDialog::~ShCircleParamDialog() {
}

void ShCircleParamDialog::createLayout() {
    QFormLayout* formLayout = new QFormLayout(this);

    // Center point
    this->centerLabel = new QLabel("Center Point:", this);
    formLayout->addRow(this->centerLabel);

    QHBoxLayout* centerLayout = new QHBoxLayout();
    this->centerXSpin = new QDoubleSpinBox(this);
    this->centerXSpin->setRange(-10000, 10000);
    this->centerXSpin->setDecimals(4);
    this->centerXSpin->setSingleStep(0.1);

    this->centerYSpin = new QDoubleSpinBox(this);
    this->centerYSpin->setRange(-10000, 10000);
    this->centerYSpin->setDecimals(4);
    this->centerYSpin->setSingleStep(0.1);

    this->centerConfirmButton = new QPushButton("?", this);
    this->centerConfirmButton->setFixedWidth(30);

    centerLayout->addWidget(new QLabel("X:", this));
    centerLayout->addWidget(this->centerXSpin);
    centerLayout->addWidget(new QLabel("Y:", this));
    centerLayout->addWidget(this->centerYSpin);
    centerLayout->addWidget(this->centerConfirmButton);
    formLayout->addRow(centerLayout);

    // Radius
    this->radiusLabel = new QLabel("Radius:", this);
    this->radiusSpin = new QDoubleSpinBox(this);
    this->radiusSpin->setRange(0.0001, 10000);
    this->radiusSpin->setDecimals(4);
    this->radiusSpin->setSingleStep(0.1);
    formLayout->addRow(this->radiusLabel, this->radiusSpin);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    this->okButton = new QPushButton("OK", this);
    this->cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addWidget(this->okButton);
    buttonLayout->addWidget(this->cancelButton);
    formLayout->addRow(buttonLayout);
}

void ShCircleParamDialog::connectSignals() {
    connect(this->okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(this->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(this->centerConfirmButton, &QPushButton::clicked, this, &ShCircleParamDialog::centerConfirmed);
    connect(this->okButton, &QPushButton::clicked, this, &ShCircleParamDialog::radiusConfirmed);
}

void ShCircleParamDialog::updateCenter(double x, double y) {
    this->centerXSpin->setValue(x);
    this->centerYSpin->setValue(y);
}

void ShCircleParamDialog::updateRadius(double radius) {
    this->radiusSpin->setValue(radius);
}

void ShCircleParamDialog::setCenterEditable(bool editable) {
    this->centerXSpin->setReadOnly(!editable);
    this->centerYSpin->setReadOnly(!editable);
}

void ShCircleParamDialog::setRadiusEditable(bool editable) {
    this->radiusSpin->setReadOnly(!editable);
}

ShPoint3d ShCircleParamDialog::getCenter() const {
    return ShPoint3d(this->centerXSpin->value(), this->centerYSpin->value());
}

double ShCircleParamDialog::getRadius() const {
    return this->radiusSpin->value();
}

void ShCircleParamDialog::setMode(Mode mode) {
    currentMode = mode;
    centerXSpin->setReadOnly(mode != CenterInput);
    centerYSpin->setReadOnly(mode != CenterInput);
    radiusSpin->setReadOnly(mode != RadiusInput);
}

ShCircleParamDialog::Mode ShCircleParamDialog::getCurrentMode() const {
    return currentMode;
}
