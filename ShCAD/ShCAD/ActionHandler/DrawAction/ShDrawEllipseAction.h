
#ifndef _SHDRAWELLIPSEACTION_H
#define _SHDRAWELLIPSEACTION_H

#include "ShDrawAction.h"

class ShDrawEllipseAction : public ShDrawAction {

    friend class ShSubDrawEllipseAction;

public:
    enum Status {
        PickedNothing,
        PickedCenter,
        PickedMajorAxisEnd,
        PickedMinorAxisEnd,
    };

    enum SubAction {
        CenterAxis,
        AxisEnd,
    };

private:
    Status status;
    SubAction subAction;
    ShSubDrawEllipseAction* subDrawEllipseAction;

public:
    ShDrawEllipseAction(ShCADWidget* widget, SubAction subAction = SubAction::CenterAxis);
    ~ShDrawEllipseAction();

    virtual void mouseLeftPressEvent(ShActionData& data);
    virtual void mouseMoveEvent(ShActionData& data);

    virtual ActionType getType();
    virtual QString getHeadTitle();
    virtual ShAvailableDraft getAvailableDraft();

    virtual void invalidate(ShPoint3d& point);

protected:
    virtual void trigger(const ShPoint3d& point);

private:
    void changeSubAction(SubAction subAction);
};


////////////////////////////////////////////////////////////////////////////


class ShSubDrawEllipseAction {

protected:
    ShDrawEllipseAction* drawEllipseAction;
    ShCADWidget* widget;

public:
    ShSubDrawEllipseAction(ShDrawEllipseAction* drawEllipseAction, ShCADWidget* widget);
    virtual ~ShSubDrawEllipseAction() = 0;

    void mouseLeftPressEvent(ShActionData& data);
    void mouseMoveEvent(ShActionData& data);

    virtual ActionType getType() = 0;
    virtual QString getHeadTitle() = 0;
    virtual ShAvailableDraft getAvailableDraft() = 0;

    virtual void invalidate(ShPoint3d& point) = 0;
    virtual void trigger(const ShPoint3d& point) = 0;

protected:
    ShDrawEllipseAction::Status& getStatus();
    void addEntity(ShEntity* newEntity, const QString& type);
    void triggerSucceeded();
    void actionFinished();
    void setLastBasePoint(const ShPoint3d& point);
    void triggerFailed(ShActionTriggerFailureReason reason);
};


///////////////////////////////////////////////////////////////////////////////


class ShSubDrawEllipseAction_CenterAxis : public ShSubDrawEllipseAction {

public:
    ShSubDrawEllipseAction_CenterAxis(ShDrawEllipseAction* drawEllipseAction, ShCADWidget* widget);
    ~ShSubDrawEllipseAction_CenterAxis();

    virtual ActionType getType();
    virtual QString getHeadTitle();
    virtual ShAvailableDraft getAvailableDraft();

    virtual void invalidate(ShPoint3d& point);
    virtual void trigger(const ShPoint3d& point);
};


//////////////////////////////////////////////////////////////////////////////////

class ShSubDrawEllipseAction_AxisEnd : public ShSubDrawEllipseAction {

public:
    ShSubDrawEllipseAction_AxisEnd(ShDrawEllipseAction* drawEllipseAction, ShCADWidget* widget);
    ~ShSubDrawEllipseAction_AxisEnd();

    virtual ActionType getType();
    virtual QString getHeadTitle();
    virtual ShAvailableDraft getAvailableDraft();

    virtual void invalidate(ShPoint3d& point);
    virtual void trigger(const ShPoint3d& point);
};

#endif //_SHDRAWELLIPSEACTION_H
