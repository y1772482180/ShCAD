#include "ShModifyBlockAction.h"
#include "Entity/Composite/ShSelectedEntities.h"
#include "Entity/Leaf/ShBlock.h"
#include "UnRedo/ShEntityTransaction.h"
#include "Base/ShGlobal.h"
#include "Command/ShAvailableCommands.h"
#include "Base/ShCursorShape.h"
#include "KeyHandler/ShKeyHandler.h"

ShModifyBlockAction::ShModifyBlockAction(ShCADWidget* widget)
    : ShModifyAction(widget), transaction(nullptr), status(Status::SelectingEntities) {

    this->keyHandler = ShKeyHandler::ShBuilder(this->widget, this)
        .allowKey(KeyType::Enter)
        .allowKey(KeyType::Return)
        .allowKey(KeyType::Control_A)
        .allowKey(KeyType::EscCancelCurrent)
        .allowInput()
        .build();
}

ShModifyBlockAction::~ShModifyBlockAction() {
    // 清理资源
}

void ShModifyBlockAction::mouseLeftPressEvent(ShActionData& data) {
    if (this->status == Status::SelectingEntities) {
        this->triggerSelectingEntities(data.mouseEvent);
    }
    else if (this->status == Status::PickingBasePoint) {
        this->trigger(data.point);
    }
}

void ShModifyBlockAction::mouseRightPressEvent(ShActionData& data) {
    if (this->status == Status::SelectingEntities) {
        this->finishSelectingEntities();
    }
}

void ShModifyBlockAction::mouseMoveEvent(ShActionData& data) {
    if (this->status == Status::PickingBasePoint) {
        this->invalidate(data.point);
    }
}

ActionType ShModifyBlockAction::getType() {
    return ActionType::ActionModifyBlock;
}

QString ShModifyBlockAction::getHeadTitle() {
    QString text;

    if (this->status == Status::SelectingEntities) {
        text = "Block >> " + shGetLanValue_command("Command/Select objects") + ": ";
    }
    else if (this->status == Status::PickingBasePoint) {
        text = "Block >> " + shGetLanValue_command("Command/Specify base point") + ": ";
    }

    return text;
}

QCursor ShModifyBlockAction::getCursorShape() {
    QCursor cursor;

    if (this->status == Status::SelectingEntities) {
        cursor = ShCursorShape::getCursor(ShCursorShape::CursorType::Selecting);
    }
    else if (this->status == Status::PickingBasePoint) {
        cursor = ShCursorShape::getCursor(ShCursorShape::CursorType::Drawing);
    }

    return cursor;
}

ShAvailableDraft ShModifyBlockAction::getAvailableDraft() {
    ShAvailableDraft draft;

    if (this->status == Status::PickingBasePoint) {
        draft.setAvailableOrthogonal(true);
        draft.setAvailableSnap(true);
        draft.setOrthogonalBasePoint(this->widget->getMousePoint());
        draft.setSnapBasePoint(this->widget->getMousePoint());
    }

    return draft;
}

void ShModifyBlockAction::trigger(const ShPoint3d& point) {
    if (this->status == Status::PickingBasePoint) {
        this->basePoint = point;
        this->createBlock(point);
        this->actionFinished();
    }
}

void ShModifyBlockAction::invalidate(ShPoint3d& point) {
    // 可以在这里实现基点的动态预览
    if (this->status == Status::PickingBasePoint) {
        this->widget->update(DrawType::DrawCaptureImage);
    }
}

void ShModifyBlockAction::finishSelectingEntities() {
    if (this->widget->getSelectedEntities()->getSize() != 0) {
        this->status = Status::PickingBasePoint;

        shCommandLogManager->appendListEditTextWith("");
        this->updateCommandEditHeadTitle();

        this->widget->setCursor(this->getCursorShape());

        this->availableCommands = ShAvailableCommands::ShBuilder(this->widget, this)
            .addAvailableCommand(CommandType::AbsoluteCoordinate)
            .addAvailableCommand(CommandType::Empty_Cancel)
            .addAvailableCommand(CommandType::RelativeCoordinate)
            .addAvailableCommand(CommandType::PolarCoordinate)
            .addAvailableCommand(CommandType::DistanceFromBase)
            .build();

        this->keyHandler->disAllowKey(KeyType::Control_A);
    }
    else {
        this->actionCanceled();
    }
}

void ShModifyBlockAction::createBlock(const ShPoint3d& basePoint) {
    // 检查是否有选中的实体
    if (this->widget->getSelectedEntities()->isEmpty()) {
        return;
    }


    // 创建块实体
    ShBlock* block = new ShBlock(basePoint,this->widget->getPropertyData(), this->widget->getCurrentLayer());

    // 克隆并添加选中的实体到块中
    QLinkedList<ShEntity*> entitiesToRemove;
    auto itr = this->widget->getSelectedEntities()->begin();
    for (; itr != this->widget->getSelectedEntities()->end(); ++itr) {
        block->addEntity((*itr)->clone());
        entitiesToRemove.append(*itr);  // 记录要移除的原始实体
    }
    // 使用ShCreateBlockTransaction
    //ShCreateBlockTransaction* transaction = new ShCreateBlockTransaction(
    //    this->widget,
    //    block,
    //    entitiesToRemove
    //);
    // 创建新的事务（模仿ShDrawAction的方式）
    if (this->transaction == nullptr) {
        this->transaction = new ShCreateBlockTransaction(this->widget, block, entitiesToRemove);
        ShGlobal::pushNewTransaction(this->widget, this->transaction);
    }
    // 清除选择集
    this->widget->getSelectedEntities()->unSelectAll();

    // 将块添加到实体表
    //this->widget->getEntityTable().add(block);
    //this->transaction->add(block);
    this->transaction->redo();
    // 从实体表中移除原始实体
    //for (ShEntity* entity : entitiesToRemove) {
    //    this->widget->getEntityTable().remove(entity);
    //}

    // 更新视图（模仿ShDrawAction的方式）
   // this->widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawAddedEntities));
      // 强制完整更新绘制
    this->widget->update((DrawType)(DrawType::DrawAll
        | DrawType::DrawCaptureImage
        | DrawType::DrawAddedEntities));
    this->widget->captureImage();



    // 设置最后基点并触发成功
    this->setLastBasePoint(basePoint);
    this->triggerSucceeded();
}