#ifndef _SHMODIFYBLOCKACTION_H
#define _SHMODIFYBLOCKACTION_H

#include "ShModifyAction.h"
#include"ActionHandler/ShActionHandler.h"
class ShCreateBlockTransaction;
class ShModifyBlockAction : public ShModifyAction {

public:
    enum Status {
        SelectingEntities,
        PickingBasePoint,
        CreatingBlock,
    };

private:
    ShCreateBlockTransaction* transaction;
    Status status;
    ShPoint3d basePoint;

public:
    ShModifyBlockAction(ShCADWidget* widget);
    ~ShModifyBlockAction();

    virtual void mouseLeftPressEvent(ShActionData& data);
    virtual void mouseRightPressEvent(ShActionData& data);
    virtual void mouseMoveEvent(ShActionData& data);

    virtual ActionType getType();
    virtual QString getHeadTitle();
    virtual QCursor getCursorShape();
    virtual ShAvailableDraft getAvailableDraft();

    virtual void invalidate(ShPoint3d& point);
    virtual void finishSelectingEntities();

protected:
    virtual void trigger(const ShPoint3d& point);

private:
    void createBlock(const ShPoint3d& basePoint);
};

#endif //_SHMODIFYBLOCKACTION_H