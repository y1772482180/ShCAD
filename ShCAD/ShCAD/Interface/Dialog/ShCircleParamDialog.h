// ShCircleParamDialog.h
#ifndef _SHCIRCLEPARAMDIALOG_H
#define _SHCIRCLEPARAMDIALOG_H

#include <QDialog>
#include "Data\ShPoint3d.h"

class QDoubleSpinBox;
class QLabel;
class QPushButton;

class ShCircleParamDialog : public QDialog {
    Q_OBJECT

public:
    enum Mode {
        CenterInput,
        RadiusInput
    };

    ShCircleParamDialog(QWidget* parent = nullptr);
    ~ShCircleParamDialog();

    void updateCenter(double x, double y);
    void updateRadius(double radius);
    ShPoint3d getCenter() const;
    double getRadius() const;
    void setCenterEditable(bool editable);
    void setRadiusEditable(bool editable);
    void setMode(Mode mode);
    Mode getCurrentMode() const;

signals:
    void centerConfirmed();
    void radiusConfirmed();

private:
    void createLayout();
    void connectSignals();

private:
    QLabel* centerLabel;
    QDoubleSpinBox* centerXSpin;
    QDoubleSpinBox* centerYSpin;
    QPushButton* centerConfirmButton;
    QLabel* radiusLabel;
    QDoubleSpinBox* radiusSpin;
    QPushButton* okButton;
    QPushButton* cancelButton;
    Mode currentMode;
};

#endif // _SHCIRCLEPARAMDIALOG_H