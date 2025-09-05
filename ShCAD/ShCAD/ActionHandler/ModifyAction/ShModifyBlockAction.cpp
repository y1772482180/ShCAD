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
    // ������Դ
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
    // ����������ʵ�ֻ���Ķ�̬Ԥ��
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
    // ����Ƿ���ѡ�е�ʵ��
    if (this->widget->getSelectedEntities()->isEmpty()) {
        return;
    }


    // ������ʵ��
    ShBlock* block = new ShBlock(basePoint,this->widget->getPropertyData(), this->widget->getCurrentLayer());

    // ��¡�����ѡ�е�ʵ�嵽����
    QLinkedList<ShEntity*> entitiesToRemove;
    auto itr = this->widget->getSelectedEntities()->begin();
    for (; itr != this->widget->getSelectedEntities()->end(); ++itr) {
        block->addEntity((*itr)->clone());
        entitiesToRemove.append(*itr);  // ��¼Ҫ�Ƴ���ԭʼʵ��
    }
    // ʹ��ShCreateBlockTransaction
    //ShCreateBlockTransaction* transaction = new ShCreateBlockTransaction(
    //    this->widget,
    //    block,
    //    entitiesToRemove
    //);
    // �����µ�����ģ��ShDrawAction�ķ�ʽ��
    if (this->transaction == nullptr) {
        this->transaction = new ShCreateBlockTransaction(this->widget, block, entitiesToRemove);
        ShGlobal::pushNewTransaction(this->widget, this->transaction);
    }
    // ���ѡ��
    this->widget->getSelectedEntities()->unSelectAll();

    // ������ӵ�ʵ���
    //this->widget->getEntityTable().add(block);
    //this->transaction->add(block);
    this->transaction->redo();
    // ��ʵ������Ƴ�ԭʼʵ��
    //for (ShEntity* entity : entitiesToRemove) {
    //    this->widget->getEntityTable().remove(entity);
    //}

    // ������ͼ��ģ��ShDrawAction�ķ�ʽ��
   // this->widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawAddedEntities));
      // ǿ���������»���
    this->widget->update((DrawType)(DrawType::DrawAll
        | DrawType::DrawCaptureImage
        | DrawType::DrawAddedEntities));
    this->widget->captureImage();



    // ���������㲢�����ɹ�
    this->setLastBasePoint(basePoint);
    this->triggerSucceeded();
}