
#include "ShDrawCircleAction.h"
#include "Entity\Leaf\ShCircle.h"
#include "Base\ShMath.h"
#include "KeyHandler\ShKeyHandler.h"
#include "Command\ShAvailableCommands.h"
#include "Command\ShCustomCommand.h"
#include"qobject.h"
ShDrawCircleAction::ShDrawCircleAction(ShCADWidget *widget, SubAction subAction)
	:ShDrawAction(widget), status(PickedNothing), subAction(subAction), subDrawCircleAction(nullptr) {

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

ShDrawCircleAction::~ShDrawCircleAction() {

	if (this->subDrawCircleAction != nullptr)
		delete this->subDrawCircleAction;
}


void ShDrawCircleAction::mouseLeftPressEvent(ShActionData &data) {

	this->subDrawCircleAction->mouseLeftPressEvent(data);
}

void ShDrawCircleAction::mouseMoveEvent(ShActionData &data) {

	this->subDrawCircleAction->mouseMoveEvent(data);
}

ActionType ShDrawCircleAction::getType() {

	return this->subDrawCircleAction->getType();
}

QString ShDrawCircleAction::getHeadTitle() {

	return this->subDrawCircleAction->getHeadTitle();
}

ShAvailableDraft ShDrawCircleAction::getAvailableDraft() {

	return this->subDrawCircleAction->getAvailableDraft();
}

void ShDrawCircleAction::invalidate(ShPoint3d &point) {

	this->subDrawCircleAction->invalidate(point);
}

void ShDrawCircleAction::trigger(const ShPoint3d &point) {

	this->subDrawCircleAction->trigger(point);
}


void ShDrawCircleAction::changeSubAction(SubAction subAction) {

	if (this->subDrawCircleAction != nullptr)
		delete this->subDrawCircleAction;

	if (subAction == SubAction::CenterRadius)
		this->subDrawCircleAction = new ShSubDrawCircleAction_CenterRadius(this, this->widget);
	else if (subAction == SubAction::CenterDiameter)
		this->subDrawCircleAction = new ShSubDrawCircleAction_CenterDiameter(this, this->widget);
	else if (subAction == SubAction::TwoPoint)
		this->subDrawCircleAction = new ShSubDrawCircleAction_TwoPoint(this, this->widget);
	else if (subAction == SubAction::ThreePoint)
		this->subDrawCircleAction = new ShSubDrawCircleAction_ThreePoint(this, this->widget);
}


////////////////////////////////////////////////////////////////////

ShSubDrawCircleAction::ShSubDrawCircleAction(ShDrawCircleAction *drawCircleAction, ShCADWidget *widget)
	:drawCircleAction(drawCircleAction), widget(widget) {

}

ShSubDrawCircleAction::~ShSubDrawCircleAction() {

}

void ShSubDrawCircleAction::mouseLeftPressEvent(ShActionData &data) {

	this->trigger(data.point);
}

void ShSubDrawCircleAction::mouseMoveEvent(ShActionData &data) {

	this->invalidate(data.point);
}

ShDrawCircleAction::Status& ShSubDrawCircleAction::getStatus() {

	return this->drawCircleAction->status;
}

void ShSubDrawCircleAction::addEntity(ShEntity *newEntity, const QString &type) {

	this->drawCircleAction->addEntity(newEntity, type);
}

void ShSubDrawCircleAction::triggerSucceeded() {

	this->drawCircleAction->triggerSucceeded();
}

void ShSubDrawCircleAction::actionFinished() {

	this->drawCircleAction->actionFinished();
}

void ShSubDrawCircleAction::triggerFailed(ShActionTriggerFailureReason reason) {

	this->drawCircleAction->triggerFailed(reason);
}

void ShSubDrawCircleAction::setLastBasePoint(const ShPoint3d &point) {

	this->drawCircleAction->setLastBasePoint(point);
}


//////////////////////////////////////////////////////////////////////


// 在构造函数中创建并显示对话框
ShSubDrawCircleAction_CenterRadius::ShSubDrawCircleAction_CenterRadius(ShDrawCircleAction* drawCircleAction, ShCADWidget* widget)
	: ShSubDrawCircleAction(drawCircleAction, widget), dialog(new ShCircleParamDialog(widget)), useDialog(true)
{
	// 初始化对话框连接
	connect(dialog, &ShCircleParamDialog::centerConfirmed,
		this, &ShSubDrawCircleAction_CenterRadius::handleDialogCenterConfirmed);
	connect(dialog, &QDialog::finished,
		this, &ShSubDrawCircleAction_CenterRadius::handleDialogFinished);

	// 初始设置为中心点输入模式
	dialog->setMode(ShCircleParamDialog::CenterInput);
	dialog->setRadiusEditable(false);

	// 立即显示对话框
	dialog->show();
}


ShSubDrawCircleAction_CenterRadius::~ShSubDrawCircleAction_CenterRadius() {
	if (dialog != nullptr) {
		delete dialog;
	}
}

ActionType ShSubDrawCircleAction_CenterRadius::getType() {

	return ActionType::ActionDrawCircleCenterRadius;
}

QString ShSubDrawCircleAction_CenterRadius::getHeadTitle() {

	ShDrawCircleAction::Status status = this->getStatus();
	QString text;

	if (status == ShDrawCircleAction::Status::PickedNothing)
		text = "Circle >> " + shGetLanValue_command("Command/Specify center point") + ": ";
	else if(status== ShDrawCircleAction::Status::PickedCenter)
		text = "Circle >> " + shGetLanValue_command("Command/Specify radius of circle") + ": ";

	return text;
}

ShAvailableDraft ShSubDrawCircleAction_CenterRadius::getAvailableDraft() {

	ShAvailableDraft draft;

	if (this->getStatus() == ShDrawCircleAction::Status::PickedNothing) {
		draft.setAvailableOrthogonal(true);
		draft.setAvailableSnap(true);
		ShPoint3d mouse = this->widget->getMousePoint();
		draft.setOrthogonalBasePoint(mouse);
		draft.setSnapBasePoint(mouse);
	}
	else if (this->getStatus() == ShDrawCircleAction::Status::PickedCenter) {
	
		draft.setAvailableOrthogonal(true);
		draft.setAvailableSnap(true);

		ShCircle *prevCircle = dynamic_cast<ShCircle*>((*this->widget->getPreview().begin()));
		ShPoint3d center = prevCircle->getCenter();

		draft.setOrthogonalBasePoint(center);
		draft.setSnapBasePoint(center);
	}

	return draft;
}

void ShSubDrawCircleAction_CenterRadius::invalidate(ShPoint3d &point) {

	//if (this->getStatus() == ShDrawCircleAction::Status::PickedCenter) {
	//
	//	ShCircle *prevCircle = dynamic_cast<ShCircle*>((*this->widget->getPreview().begin()));
	//	ShPoint3d center = prevCircle->getCenter();

	//	double radius = math::getDistance(center.x, center.y, point.x, point.y);

	//	prevCircle->setRadius(radius);

	//	this->widget->getRubberBand().setEnd(point);

	//	this->widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	//}
	if (this->getStatus() == ShDrawCircleAction::Status::PickedNothing) {
		// 在未确定圆心前，实时更新对话框中的坐标
		if (dialog->isVisible() && dialog->getCurrentMode() == ShCircleParamDialog::CenterInput) {
			dialog->updateCenter(point.x, point.y);
		}
	}
	else if (this->getStatus() == ShDrawCircleAction::Status::PickedCenter) {
		ShCircle* prevCircle = dynamic_cast<ShCircle*>((*this->widget->getPreview().begin()));
		if (prevCircle) {
			ShPoint3d center = prevCircle->getCenter();
			double radius = math::getDistance(center.x, center.y, point.x, point.y);

			// 实时更新对话框中的半径值
			if (dialog->isVisible() && dialog->getCurrentMode() == ShCircleParamDialog::RadiusInput) {
				dialog->updateRadius(radius);
			}

			// 更新图形显示
			prevCircle->setRadius(radius);
			this->widget->getRubberBand().setEnd(point);
			this->widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
		}
	}

}
void ShSubDrawCircleAction_CenterRadius::handleDialogCenterConfirmed() {
	ShPoint3d center = dialog->getCenter();

	// 更新状态
	this->getStatus() = ShDrawCircleAction::Status::PickedCenter;

	// 创建/更新预览图形
	this->widget->getPreview().clear();
	this->widget->getPreview().add(new ShCircle(this->widget->getPropertyData(),
		ShCircleData(center, 0),
		this->widget->getCurrentLayer()));
	this->widget->getRubberBand().create(ShLineData(center, center));

	// 切换到半径输入模式
	dialog->setMode(ShCircleParamDialog::RadiusInput);
	dialog->setCenterEditable(false);

	this->setLastBasePoint(center);
	this->triggerSucceeded();
}
void ShSubDrawCircleAction_CenterRadius::handleDialogFinished(int result) {
	if (result == QDialog::Accepted && this->getStatus() == ShDrawCircleAction::Status::PickedCenter) {
		ShCircle* prevCircle = dynamic_cast<ShCircle*>((*this->widget->getPreview().begin()));
		if (prevCircle) {
			prevCircle->setRadius(dialog->getRadius());
			this->addEntity(prevCircle->clone(), "Circle");
		}
	}
	this->actionFinished();
	//dialog->hide();
}

void ShSubDrawCircleAction_CenterRadius::trigger(const ShPoint3d &point) {

	//if (this->getStatus() == ShDrawCircleAction::Status::PickedNothing) {
	//	// 创建并显示对话框
	//	if (dialog == nullptr) {
	//		dialog = new ShCircleParamDialog(this->widget);
	//		QObject::connect(dialog, &ShCircleParamDialog::centerConfirmed,
	//			this, &ShSubDrawCircleAction_CenterRadius::handleDialogCenterConfirmed);
	//		QObject::connect(dialog, &QDialog::finished,
	//			this, &ShSubDrawCircleAction_CenterRadius::handleDialogFinished);
	//		QObject::connect(dialog, &ShCircleParamDialog::radiusConfirmed,
	//			[this]() {
	//				if (dialog->getCurrentMode() == ShCircleParamDialog::RadiusInput) {
	//					double radius = dialog->getRadius();
	//					ShCircle* prevCircle = dynamic_cast<ShCircle*>((*this->widget->getPreview().begin()));
	//					if (prevCircle) {
	//						prevCircle->setRadius(radius);
	//						this->widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	//					}
	//				}
	//			});
	//	}

	//	dialog->setMode(ShCircleParamDialog::CenterInput);
	//	dialog->setRadiusEditable(false);
	//	dialog->updateCenter(point.x, point.y);
	//	dialog->show();
	//	useDialog = true;
	//	this->getStatus() = ShDrawCircleAction::Status::PickedCenter;

	//	this->widget->getPreview().add(new ShCircle(this->widget->getPropertyData(), ShCircleData(point, 0), this->widget->getCurrentLayer()));
	//	this->widget->getRubberBand().create(ShLineData(point, point));

	//	this->setLastBasePoint(point);
	//	this->triggerSucceeded();
	//}
	//else if (this->getStatus() == ShDrawCircleAction::PickedCenter) {
	//	if (useDialog && dialog != nullptr && dialog->isVisible()) {
	//		// 如果对话框处于中心点输入模式，更新中心点坐标
	//		if (dialog->getCurrentMode() == ShCircleParamDialog::CenterInput) {
	//			dialog->updateCenter(point.x, point.y);
	//		}
	//		// 如果对话框处于半径输入模式，更新半径
	//		else if (dialog->getCurrentMode() == ShCircleParamDialog::RadiusInput) {
	//			ShCircle* prevCircle = dynamic_cast<ShCircle*>((*this->widget->getPreview().begin()));
	//			if (prevCircle) {
	//				ShPoint3d center = prevCircle->getCenter();
	//				double radius = math::getDistance(center.x, center.y, point.x, point.y);
	//				dialog->updateRadius(radius);
	//				prevCircle->setRadius(radius);
	//				this->widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	//			}
	//		}
	//	}
	//	else {
	//		ShCircle* prevCircle = dynamic_cast<ShCircle*>((*this->widget->getPreview().begin()));
	//		if (prevCircle) {
	//			ShCircleData data = prevCircle->getData();
	//			data.radius = math::getDistance(data.center.x, data.center.y, point.x, point.y);
	//			if (math::compare(data.radius, 0) <= 0) {
	//				this->triggerFailed(ShActionTriggerFailureReason::ValueMustBeGreaterThanZero);
	//				return;
	//			}
	//			prevCircle->setData(data);
	//			this->addEntity(prevCircle->clone(), "Circle");
	//			this->setLastBasePoint(point);
	//			this->actionFinished();
	//		}
	//	}
	//}
	////else if (this->getStatus() == ShDrawCircleAction::PickedCenter) {
	//	if (useDialog) {
	//		// 如果使用对话框，半径已经在对话框处理中设置
	//		return;
	//	}
	//	ShCircle *prevCircle = dynamic_cast<ShCircle*>((*this->widget->getPreview().begin()));
	//	ShCircleData data = prevCircle->getData();

	//	data.radius = math::getDistance(data.center.x, data.center.y, point.x, point.y);
	//	
	//	if (math::compare(data.radius, 0) == 0 || math::compare(data.radius, 0) == -1) {
	//		this->triggerFailed(ShActionTriggerFailureReason::ValueMustBeGreaterThanZero);
	//		return;
	//	}

	//	prevCircle->setData(data);

	//	this->addEntity(prevCircle->clone(), "Circle");

	//	this->setLastBasePoint(point);
	//	this->actionFinished();
	//}
	if (this->getStatus() == ShDrawCircleAction::Status::PickedNothing) {
		// 确认圆心
		this->getStatus() = ShDrawCircleAction::Status::PickedCenter;

		// 创建预览图形
		this->widget->getPreview().add(new ShCircle(this->widget->getPropertyData(),
			ShCircleData(point, 0),
			this->widget->getCurrentLayer()));
		this->widget->getRubberBand().create(ShLineData(point, point));

		// 切换到半径输入模式
		dialog->setMode(ShCircleParamDialog::RadiusInput);
		dialog->setCenterEditable(false);

		this->setLastBasePoint(point);
		this->triggerSucceeded();
	}
	else if (this->getStatus() == ShDrawCircleAction::PickedCenter) {
		// 完成绘制
		ShCircle* prevCircle = dynamic_cast<ShCircle*>((*this->widget->getPreview().begin()));
		if (prevCircle) {
			ShCircleData data = prevCircle->getData();
			data.radius = math::getDistance(data.center.x, data.center.y, point.x, point.y);

			if (math::compare(data.radius, 0) <= 0) {
				this->triggerFailed(ShActionTriggerFailureReason::ValueMustBeGreaterThanZero);
				return;
			}

			this->addEntity(prevCircle->clone(), "Circle");
			this->setLastBasePoint(point);
			this->actionFinished();

			// 隐藏对话框
		//	dialog->hide();
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////




ShSubDrawCircleAction_CenterDiameter::ShSubDrawCircleAction_CenterDiameter(ShDrawCircleAction *drawCircleAction, ShCADWidget *widget)
	:ShSubDrawCircleAction(drawCircleAction, widget) {

}

ShSubDrawCircleAction_CenterDiameter::~ShSubDrawCircleAction_CenterDiameter() {

}


ActionType ShSubDrawCircleAction_CenterDiameter::getType() {

	return ActionType::ActionDrawCircleCenterDiameter;
}

QString ShSubDrawCircleAction_CenterDiameter::getHeadTitle() {

	ShDrawCircleAction::Status status = this->getStatus();
	QString text;

	if (status == ShDrawCircleAction::Status::PickedNothing)
		text = "Circle >> " + shGetLanValue_command("Command/Specify center point") + ": ";
	else if (status == ShDrawCircleAction::Status::PickedCenter)
		text = "Circle >> " + shGetLanValue_command("Command/Specify diameter of circle") + ": ";

	return text;
}

ShAvailableDraft ShSubDrawCircleAction_CenterDiameter::getAvailableDraft() {

	ShAvailableDraft draft;

	if (this->getStatus() == ShDrawCircleAction::Status::PickedNothing) {
		draft.setAvailableOrthogonal(true);
		draft.setAvailableSnap(true);
		ShPoint3d mouse = this->widget->getMousePoint();
		draft.setOrthogonalBasePoint(mouse);
		draft.setSnapBasePoint(mouse);
	}
	else if (this->getStatus() == ShDrawCircleAction::Status::PickedCenter) {

		draft.setAvailableOrthogonal(true);
		draft.setAvailableSnap(true);

		ShCircle *prevCircle = dynamic_cast<ShCircle*>((*this->widget->getPreview().begin()));
		ShPoint3d center = prevCircle->getCenter();

		draft.setOrthogonalBasePoint(center);
		draft.setSnapBasePoint(center);
	}

	return draft;
}

void ShSubDrawCircleAction_CenterDiameter::invalidate(ShPoint3d &point) {

	if (this->getStatus() == ShDrawCircleAction::Status::PickedCenter) {

		ShCircle *prevCircle = dynamic_cast<ShCircle*>((*this->widget->getPreview().begin()));
		ShPoint3d center = prevCircle->getCenter();

		double radius = math::getDistance(center.x, center.y, point.x, point.y) / 2.0;

		prevCircle->setRadius(radius);

		this->widget->getRubberBand().setEnd(point);

		this->widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}
}

void ShSubDrawCircleAction_CenterDiameter::trigger(const ShPoint3d &point) {

	if (this->getStatus() == ShDrawCircleAction::Status::PickedNothing) {

		this->getStatus() = ShDrawCircleAction::Status::PickedCenter;

		this->widget->getPreview().add(new ShCircle(this->widget->getPropertyData(), ShCircleData(point, 0), this->widget->getCurrentLayer()));
		this->widget->getRubberBand().create(ShLineData(point, point));

		this->setLastBasePoint(point);
		this->triggerSucceeded();
	}
	else if (this->getStatus() == ShDrawCircleAction::PickedCenter) {

		ShCircle *prevCircle = dynamic_cast<ShCircle*>((*this->widget->getPreview().begin()));
		ShCircleData data = prevCircle->getData();

		data.radius = math::getDistance(data.center.x, data.center.y, point.x, point.y) / 2.0;

		if (math::compare(data.radius, 0) == 0 || math::compare(data.radius, 0) == -1) {

			this->triggerFailed(ValueMustBeGreaterThanZero);
			return;
		}

		prevCircle->setData(data);

		this->addEntity(prevCircle->clone(), "Circle");

		this->setLastBasePoint(point);
		this->actionFinished();
	}
}

//////////////////////////////////////////////////////////////////////////////////



ShSubDrawCircleAction_TwoPoint::ShSubDrawCircleAction_TwoPoint(ShDrawCircleAction *drawCircleAction, ShCADWidget *widget)
	:ShSubDrawCircleAction(drawCircleAction, widget) {

	
}

ShSubDrawCircleAction_TwoPoint::~ShSubDrawCircleAction_TwoPoint() {

}


ActionType ShSubDrawCircleAction_TwoPoint::getType() {

	return ActionType::ActionDrawCircleTwoPoint;
}

QString ShSubDrawCircleAction_TwoPoint::getHeadTitle() {

	ShDrawCircleAction::Status status = this->getStatus();
	QString text;

	if (status == ShDrawCircleAction::Status::PickedNothing)
		text = "Circle >> " + shGetLanValue_command("Command/Specify first end point of circle's diameter") + ": ";
	else if (status == ShDrawCircleAction::Status::PickedFirstPoint)
		text = "Circle >> " + shGetLanValue_command("Command/Specify second end point of circle's diameter") + ": ";

	return text;
}

ShAvailableDraft ShSubDrawCircleAction_TwoPoint::getAvailableDraft() {

	ShAvailableDraft draft;

	if (this->getStatus() == ShDrawCircleAction::Status::PickedNothing) {
		draft.setAvailableOrthogonal(true);
		draft.setAvailableSnap(true);
		ShPoint3d mouse = this->widget->getMousePoint();
		draft.setOrthogonalBasePoint(mouse);
		draft.setSnapBasePoint(mouse);
	}
	else if (this->getStatus() == ShDrawCircleAction::Status::PickedFirstPoint) {

		draft.setAvailableOrthogonal(true);
		draft.setAvailableSnap(true);

		ShPoint3d first = this->widget->getRubberBand().getStart();

		draft.setOrthogonalBasePoint(first);
		draft.setSnapBasePoint(first);
	}

	return draft;
}

void ShSubDrawCircleAction_TwoPoint::invalidate(ShPoint3d &point) {

	if (this->getStatus() == ShDrawCircleAction::Status::PickedFirstPoint) {

		ShPoint3d first = this->widget->getRubberBand().getStart();

		ShCircle *prevCircle = dynamic_cast<ShCircle*>((*this->widget->getPreview().begin()));
		
		ShPoint3d center = this->getCenter(first, point);
		double radius = math::getDistance(first.x, first.y, point.x, point.y) / 2.0;

		prevCircle->setCenter(center);
		prevCircle->setRadius(radius);

		this->widget->getRubberBand().setEnd(point);

		this->widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}
}

void ShSubDrawCircleAction_TwoPoint::trigger(const ShPoint3d &point) {

	if (this->getStatus() == ShDrawCircleAction::Status::PickedNothing) {

		this->getStatus() = ShDrawCircleAction::Status::PickedFirstPoint;

		ShPoint3d center = this->getCenter(point, point);

		this->widget->getPreview().add(new ShCircle(this->widget->getPropertyData(), ShCircleData(center, 0), this->widget->getCurrentLayer()));
		this->widget->getRubberBand().create(ShLineData(point, point));

		this->setLastBasePoint(point);
		this->triggerSucceeded();
	}
	else if (this->getStatus() == ShDrawCircleAction::PickedFirstPoint) {

		ShPoint3d first = this->widget->getRubberBand().getStart();

		if (math::compare(first.x, point.x) == 0 &&
			math::compare(first.y, point.y) == 0) {

			this->triggerFailed(InvalidPoint);
			return;
		}

		ShCircle *prevCircle = dynamic_cast<ShCircle*>((*this->widget->getPreview().begin()));
		ShCircleData data = prevCircle->getData();

		data.center = this->getCenter(first, point);
		data.radius = math::getDistance(first.x, first.y, point.x, point.y) / 2.0;

		prevCircle->setData(data);

		this->addEntity(prevCircle->clone(), "Circle");

		this->setLastBasePoint(point);
		this->actionFinished();
	}
}

ShPoint3d ShSubDrawCircleAction_TwoPoint::getCenter(const ShPoint3d &first, const ShPoint3d &second) {

	double angle = math::getAbsAngle(first.x, first.y, second.x, second.y);
	double dis = math::getDistance(first.x, first.y, second.x, second.y);

	ShPoint3d center;
	math::rotate(angle, first.x, first.y, first.x + (dis / 2), first.y, center.x, center.y);

	return center;
}


/////////////////////////////////////////////////////////////////////////////////


ShSubDrawCircleAction_ThreePoint::ShSubDrawCircleAction_ThreePoint(ShDrawCircleAction *drawCircleAction, ShCADWidget *widget)
	:ShSubDrawCircleAction(drawCircleAction, widget) {

	
}

ShSubDrawCircleAction_ThreePoint::~ShSubDrawCircleAction_ThreePoint() {

}


ActionType ShSubDrawCircleAction_ThreePoint::getType() {

	return ActionType::ActionDrawCircleThreePoint;
}

QString ShSubDrawCircleAction_ThreePoint::getHeadTitle() {

	ShDrawCircleAction::Status status = this->getStatus();
	QString text;

	if (status == ShDrawCircleAction::Status::PickedNothing)
		text = "Circle >> " + shGetLanValue_command("Command/Specify first point on circle") + ": ";
	else if (status == ShDrawCircleAction::Status::PickedFirstPoint)
		text = "Circle >> " + shGetLanValue_command("Command/Specify second point on circle") + ": ";
	else if(status==ShDrawCircleAction::Status::PickedSecondPoint)
		text= "Circle >> " + shGetLanValue_command("Command/Specify third point on circle") + ": ";

	return text;
}

ShAvailableDraft ShSubDrawCircleAction_ThreePoint::getAvailableDraft() {

	ShAvailableDraft draft;

	if (this->getStatus() == ShDrawCircleAction::Status::PickedNothing) {
		draft.setAvailableOrthogonal(true);
		draft.setAvailableSnap(true);
		ShPoint3d mouse = this->widget->getMousePoint();
		draft.setOrthogonalBasePoint(mouse);
		draft.setSnapBasePoint(mouse);
	}
	else if (this->getStatus() == ShDrawCircleAction::Status::PickedFirstPoint ||
		this->getStatus() == ShDrawCircleAction::Status::PickedSecondPoint) {

		draft.setAvailableOrthogonal(true);
		draft.setAvailableSnap(true);

		ShPoint3d first = this->widget->getRubberBand().getStart();

		draft.setOrthogonalBasePoint(first);
		draft.setSnapBasePoint(first);
	}

	return draft;
}

void ShSubDrawCircleAction_ThreePoint::invalidate(ShPoint3d &point) {

	if (this->getStatus() == ShDrawCircleAction::Status::PickedFirstPoint) {
	
		this->widget->getRubberBand().setEnd(point);
		this->widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}
	else if (this->getStatus() == ShDrawCircleAction::Status::PickedSecondPoint) {
	
		this->widget->getRubberBand().setEnd(point);
		ShCircleData data;
		if (math::getCenterWithThreePoint(this->first, this->second, point, data.center) == false) {
		
			data = ShCircleData(ShPoint3d(0, 0), 0);
		}
		else {
			data.radius = math::getDistance(data.center.x, data.center.y, point.x, point.y);
		}

		ShCircle *prevCircle = dynamic_cast<ShCircle*>((*this->widget->getPreview().begin()));
		prevCircle->setData(data);

		this->widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}
}

void ShSubDrawCircleAction_ThreePoint::trigger(const ShPoint3d &point) {

	if (this->getStatus() == ShDrawCircleAction::Status::PickedNothing) {

		this->getStatus() = ShDrawCircleAction::Status::PickedFirstPoint;

		this->first = point;
		this->widget->getRubberBand().create(ShLineData(point, point));

		this->setLastBasePoint(point);
		this->triggerSucceeded();
	}
	else if (this->getStatus() == ShDrawCircleAction::PickedFirstPoint) {

		this->getStatus() = ShDrawCircleAction::Status::PickedSecondPoint;

		this->second = point;
		this->widget->getRubberBand().setData(ShLineData(this->second, this->second));
		
		ShCircleData data;
		math::getCenterWithThreePoint(this->first, this->second, this->second, data.center);

		this->widget->getPreview().add(new ShCircle(this->widget->getPropertyData(), data, this->widget->getCurrentLayer()));

		this->setLastBasePoint(point);
		this->triggerSucceeded();

	}
	else if (this->getStatus() == ShDrawCircleAction::Status::PickedSecondPoint) {
	
		ShCircleData data;
		if (math::getCenterWithThreePoint(this->first, this->second, point, data.center) == false) {
			this->triggerFailed(InvalidPoint);
			return;
		}
		else {
		
			data.radius = math::getDistance(data.center.x, data.center.y, point.x, point.y);
		}
	
		ShCircle *prevCircle = dynamic_cast<ShCircle*>((*this->widget->getPreview().begin()));
		prevCircle->setData(data);

		this->addEntity(prevCircle->clone(), "Circle");

		this->setLastBasePoint(point);
		this->actionFinished();
	}
}
