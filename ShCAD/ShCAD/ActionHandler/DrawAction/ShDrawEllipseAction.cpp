#include "ShDrawEllipseAction.h"
#include "Entity\Leaf\ShEllipse.h"
#include "Base\ShMath.h"
#include "KeyHandler\ShKeyHandler.h"
#include "Command\ShAvailableCommands.h"
#include "Command\ShCustomCommand.h"

ShDrawEllipseAction::ShDrawEllipseAction(ShCADWidget* widget, SubAction subAction)
    :ShDrawAction(widget), status(PickedNothing), subAction(subAction), subDrawEllipseAction(nullptr) {

    this->keyHandler = ShKeyHandler::ShBuilder(this->widget, this).
        allowInput().
        allowKey(KeyType::EscCancelCurrent).
        allowKey(KeyType::Enter).
        allowKey(KeyType::Return).
        build();

    this->availableCommands = ShAvailableCommands::ShBuilder(this->widget, this).
        addAvailableCommand(CommandType::AbsoluteCoordinate).
        addAvailableCommand(CommandType::Empty_Cancel).
        addAvailableCommand(CommandType::RelativeCoordinate).
        addAvailableCommand(CommandType::PolarCoordinate).
        addAvailableCommand(CommandType::DistanceFromBase).
        build();

    this->changeSubAction(subAction);
}

ShDrawEllipseAction::~ShDrawEllipseAction() {
    if (this->subDrawEllipseAction != nullptr)
        delete this->subDrawEllipseAction;
}

void ShDrawEllipseAction::mouseLeftPressEvent(ShActionData& data) {
    this->subDrawEllipseAction->mouseLeftPressEvent(data);
}

void ShDrawEllipseAction::mouseMoveEvent(ShActionData& data) {
    this->subDrawEllipseAction->mouseMoveEvent(data);
}

ActionType ShDrawEllipseAction::getType() {
    return this->subDrawEllipseAction->getType();
}

QString ShDrawEllipseAction::getHeadTitle() {
    return this->subDrawEllipseAction->getHeadTitle();
}

ShAvailableDraft ShDrawEllipseAction::getAvailableDraft() {
    return this->subDrawEllipseAction->getAvailableDraft();
}

void ShDrawEllipseAction::invalidate(ShPoint3d& point) {
    this->subDrawEllipseAction->invalidate(point);
}

void ShDrawEllipseAction::trigger(const ShPoint3d& point) {
    this->subDrawEllipseAction->trigger(point);
}

void ShDrawEllipseAction::changeSubAction(SubAction subAction) {
    if (this->subDrawEllipseAction != nullptr)
        delete this->subDrawEllipseAction;

    if (subAction == SubAction::CenterAxis)
        this->subDrawEllipseAction = new ShSubDrawEllipseAction_CenterAxis(this, this->widget);
    else if (subAction == SubAction::AxisEnd)
        this->subDrawEllipseAction = new ShSubDrawEllipseAction_AxisEnd(this, this->widget);
}


////////////////////////////////////////////////////////////////////

ShSubDrawEllipseAction::ShSubDrawEllipseAction(ShDrawEllipseAction* drawEllipseAction, ShCADWidget* widget)
    :drawEllipseAction(drawEllipseAction), widget(widget) {
}

ShSubDrawEllipseAction::~ShSubDrawEllipseAction() {
}

void ShSubDrawEllipseAction::mouseLeftPressEvent(ShActionData& data) {
    this->trigger(data.point);
}

void ShSubDrawEllipseAction::mouseMoveEvent(ShActionData& data) {
    this->invalidate(data.point);
}

ShDrawEllipseAction::Status& ShSubDrawEllipseAction::getStatus() {
    return this->drawEllipseAction->status;
}

void ShSubDrawEllipseAction::addEntity(ShEntity* newEntity, const QString& type) {
    this->drawEllipseAction->addEntity(newEntity, type);
}

void ShSubDrawEllipseAction::triggerSucceeded() {
    this->drawEllipseAction->triggerSucceeded();
}

void ShSubDrawEllipseAction::actionFinished() {
    this->drawEllipseAction->actionFinished();
}

void ShSubDrawEllipseAction::triggerFailed(ShActionTriggerFailureReason reason) {
    this->drawEllipseAction->triggerFailed(reason);
}

void ShSubDrawEllipseAction::setLastBasePoint(const ShPoint3d& point) {
    this->drawEllipseAction->setLastBasePoint(point);
}


//////////////////////////////////////////////////////////////////////


ShSubDrawEllipseAction_CenterAxis::ShSubDrawEllipseAction_CenterAxis(ShDrawEllipseAction* drawEllipseAction, ShCADWidget* widget)
    :ShSubDrawEllipseAction(drawEllipseAction, widget) {
}

ShSubDrawEllipseAction_CenterAxis::~ShSubDrawEllipseAction_CenterAxis() {
}

ActionType ShSubDrawEllipseAction_CenterAxis::getType() {
    return ActionType::ActionDrawEllipseCenterAxis;
}

QString ShSubDrawEllipseAction_CenterAxis::getHeadTitle() {
    ShDrawEllipseAction::Status status = this->getStatus();
    QString text;

    if (status == ShDrawEllipseAction::Status::PickedNothing)
        text = "Ellipse >> " + shGetLanValue_command("Command/Specify center point") + ": ";
    else if (status == ShDrawEllipseAction::Status::PickedCenter)
        text = "Ellipse >> " + shGetLanValue_command("Command/Specify end point of major axis") + ": ";
    else if (status == ShDrawEllipseAction::Status::PickedMajorAxisEnd)
        text = "Ellipse >> " + shGetLanValue_command("Command/Specify end point of minor axis") + ": ";

    return text;
}

ShAvailableDraft ShSubDrawEllipseAction_CenterAxis::getAvailableDraft() {
    ShAvailableDraft draft;

    if (this->getStatus() == ShDrawEllipseAction::Status::PickedNothing) {
        draft.setAvailableOrthogonal(true);
        draft.setAvailableSnap(true);
        ShPoint3d mouse = this->widget->getMousePoint();
        draft.setOrthogonalBasePoint(mouse);
        draft.setSnapBasePoint(mouse);
    }
    else if (this->getStatus() == ShDrawEllipseAction::Status::PickedCenter ||
        this->getStatus() == ShDrawEllipseAction::Status::PickedMajorAxisEnd) {

        draft.setAvailableOrthogonal(true);
        draft.setAvailableSnap(true);

        ShPoint3d basePoint = (this->getStatus() == ShDrawEllipseAction::Status::PickedCenter) ?
            this->widget->getRubberBand().getStart() :
            dynamic_cast<ShEllipse*>((*this->widget->getPreview().begin()))->getCenter();

        draft.setOrthogonalBasePoint(basePoint);
        draft.setSnapBasePoint(basePoint);
    }

    return draft;
}

void ShSubDrawEllipseAction_CenterAxis::invalidate(ShPoint3d& point) {
    if (this->getStatus() == ShDrawEllipseAction::Status::PickedCenter) {
        this->widget->getRubberBand().setEnd(point);
        this->widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
    }
    else if (this->getStatus() == ShDrawEllipseAction::Status::PickedMajorAxisEnd) {
        ShEllipse* prevEllipse = dynamic_cast<ShEllipse*>((*this->widget->getPreview().begin()));
        ShEllipseData data = prevEllipse->getData();

        ShPoint3d center = data.center;
        ShPoint3d majorAxisEnd = this->widget->getRubberBand().getEnd();
        double majorRadius = math::getDistance(center.x, center.y, majorAxisEnd.x, majorAxisEnd.y);
        double angle = math::getAbsAngle(center.x, center.y, majorAxisEnd.x, majorAxisEnd.y);

        // For preview, we'll use the current mouse position as minor axis end
        ShPoint3d minorAxisEnd;
        math::rotate(angle + 90, center.x, center.y, center.x, center.y + majorRadius / 2, minorAxisEnd.x, minorAxisEnd.y);
        //double minorRadius = math::getDistance(center.x, center.y, minorAxisEnd.x, minorAxisEnd.y);
        double minorRadius = math::getDistance(center.x, center.y, point.x, point.y);
        data.majorRadius = majorRadius;
        data.minorRadius = minorRadius;
        data.angle = angle;
        prevEllipse->setData(data);

       // this->widget->getRubberBand().setEnd(point);
        this->widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
    }
}

void ShSubDrawEllipseAction_CenterAxis::trigger(const ShPoint3d& point) {
    if (this->getStatus() == ShDrawEllipseAction::Status::PickedNothing) {
        this->getStatus() = ShDrawEllipseAction::Status::PickedCenter;
        this->widget->getRubberBand().create(ShLineData(point, point));
        this->setLastBasePoint(point);
        this->triggerSucceeded();
    }
    else if (this->getStatus() == ShDrawEllipseAction::Status::PickedCenter) {
        this->getStatus() = ShDrawEllipseAction::Status::PickedMajorAxisEnd;

        ShPoint3d center = this->widget->getRubberBand().getStart();
        double majorRadius = math::getDistance(center.x, center.y, point.x, point.y);
        double angle = math::getAbsAngle(center.x, center.y, point.x, point.y);

        ShEllipseData data(center, majorRadius, majorRadius / 2, angle);
        this->widget->getPreview().add(new ShEllipse(this->widget->getPropertyData(), data, this->widget->getCurrentLayer()));

        this->setLastBasePoint(point);
        this->triggerSucceeded();
    }
    else if (this->getStatus() == ShDrawEllipseAction::Status::PickedMajorAxisEnd) {
        ShEllipse* prevEllipse = dynamic_cast<ShEllipse*>((*this->widget->getPreview().begin()));
        ShEllipseData data = prevEllipse->getData();

        ShPoint3d center = data.center;
        double minorRadius = math::getDistance(center.x, center.y, point.x, point.y);

        if (math::compare(minorRadius, 0) == 0 || math::compare(minorRadius, 0) == -1) {
            this->triggerFailed(ShActionTriggerFailureReason::ValueMustBeGreaterThanZero);
            return;
        }

        data.minorRadius = minorRadius;
        prevEllipse->setData(data);

        this->addEntity(prevEllipse->clone(), "Ellipse");
        this->setLastBasePoint(point);
        this->actionFinished();
    }
}


////////////////////////////////////////////////////////////////////////////////////////////

ShSubDrawEllipseAction_AxisEnd::ShSubDrawEllipseAction_AxisEnd(ShDrawEllipseAction* drawEllipseAction, ShCADWidget* widget)
    :ShSubDrawEllipseAction(drawEllipseAction, widget) {
}

ShSubDrawEllipseAction_AxisEnd::~ShSubDrawEllipseAction_AxisEnd() {
}

ActionType ShSubDrawEllipseAction_AxisEnd::getType() {
    return ActionType::ActionDrawEllipseAxisEnd;
}

QString ShSubDrawEllipseAction_AxisEnd::getHeadTitle() {
    ShDrawEllipseAction::Status status = this->getStatus();
    QString text;

    if (status == ShDrawEllipseAction::Status::PickedNothing)
        text = "Ellipse >> " + shGetLanValue_command("Command/Specify first end point of major axis") + ": ";
    else if (status == ShDrawEllipseAction::Status::PickedMajorAxisEnd)
        text = "Ellipse >> " + shGetLanValue_command("Command/Specify second end point of major axis") + ": ";
    else if (status == ShDrawEllipseAction::Status::PickedMinorAxisEnd)
        text = "Ellipse >> " + shGetLanValue_command("Command/Specify distance to other axis") + ": ";

    return text;
}

ShAvailableDraft ShSubDrawEllipseAction_AxisEnd::getAvailableDraft() {
    ShAvailableDraft draft;

    if (this->getStatus() == ShDrawEllipseAction::Status::PickedNothing) {
        draft.setAvailableOrthogonal(true);
        draft.setAvailableSnap(true);
        ShPoint3d mouse = this->widget->getMousePoint();
        draft.setOrthogonalBasePoint(mouse);
        draft.setSnapBasePoint(mouse);
    }
    else if (this->getStatus() == ShDrawEllipseAction::Status::PickedMajorAxisEnd ||
        this->getStatus() == ShDrawEllipseAction::Status::PickedMinorAxisEnd) {

        draft.setAvailableOrthogonal(true);
        draft.setAvailableSnap(true);

        ShPoint3d first = this->widget->getRubberBand().getStart();
        draft.setOrthogonalBasePoint(first);
        draft.setSnapBasePoint(first);
    }

    return draft;
}

void ShSubDrawEllipseAction_AxisEnd::invalidate(ShPoint3d& point) {
    if (this->getStatus() == ShDrawEllipseAction::Status::PickedMajorAxisEnd) {
        this->widget->getRubberBand().setEnd(point);
        this->widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
    }
    else if (this->getStatus() == ShDrawEllipseAction::Status::PickedMinorAxisEnd) {
        ShPoint3d first = this->widget->getRubberBand().getStart();
        ShPoint3d second = this->widget->getRubberBand().getEnd();

        ShPoint3d center((first.x + second.x) / 2, (first.y + second.y) / 2, 0);
        double majorRadius = math::getDistance(center.x, center.y, second.x, second.y);
        double angle = math::getAbsAngle(center.x, center.y, second.x, second.y);

        // Calculate minor radius based on distance from center to current mouse position
        double minorRadius = math::getDistance(center.x, center.y, point.x, point.y);

        ShEllipse* prevEllipse = dynamic_cast<ShEllipse*>((*this->widget->getPreview().begin()));
        ShEllipseData data(center, majorRadius, minorRadius, angle);
        prevEllipse->setData(data);

        this->widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
    }
}

void ShSubDrawEllipseAction_AxisEnd::trigger(const ShPoint3d& point) {
    if (this->getStatus() == ShDrawEllipseAction::Status::PickedNothing) {
        this->getStatus() = ShDrawEllipseAction::Status::PickedMajorAxisEnd;
        this->widget->getRubberBand().create(ShLineData(point, point));
        this->setLastBasePoint(point);
        this->triggerSucceeded();
    }
    else if (this->getStatus() == ShDrawEllipseAction::Status::PickedMajorAxisEnd) {
        ShPoint3d first = this->widget->getRubberBand().getStart();

        if (math::compare(first.x, point.x) == 0 && math::compare(first.y, point.y) == 0) {
            this->triggerFailed(InvalidPoint);
            return;
        }

        this->getStatus() = ShDrawEllipseAction::Status::PickedMinorAxisEnd;
        this->widget->getRubberBand().setEnd(point);

        ShPoint3d center((first.x + point.x) / 2, (first.y + point.y) / 2, 0);
        double majorRadius = math::getDistance(center.x, center.y, point.x, point.y);
        double angle = math::getAbsAngle(center.x, center.y, point.x, point.y);

        // For preview, initialize with minor radius equal to half of major radius
        ShEllipseData data(center, majorRadius, majorRadius / 2, angle);
        this->widget->getPreview().add(new ShEllipse(this->widget->getPropertyData(), data, this->widget->getCurrentLayer()));

        this->setLastBasePoint(point);
        this->triggerSucceeded();
    }
    else if (this->getStatus() == ShDrawEllipseAction::Status::PickedMinorAxisEnd) {
        ShEllipse* prevEllipse = dynamic_cast<ShEllipse*>((*this->widget->getPreview().begin()));
        ShEllipseData data = prevEllipse->getData();

        double minorRadius = math::getDistance(data.center.x, data.center.y, point.x, point.y);

        if (math::compare(minorRadius, 0) == 0 || math::compare(minorRadius, 0) == -1) {
            this->triggerFailed(ShActionTriggerFailureReason::ValueMustBeGreaterThanZero);
            return;
        }

        data.minorRadius = minorRadius;
        prevEllipse->setData(data);

        this->addEntity(prevEllipse->clone(), "Ellipse");
        this->setLastBasePoint(point);
        this->actionFinished();
    }
}