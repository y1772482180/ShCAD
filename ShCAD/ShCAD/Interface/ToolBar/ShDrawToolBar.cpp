

#include "ShDrawToolBar.h"
#include "Interface\Item\ShIcon.h"
#include "ActionHandler\Private\ShChangeActionStrategy.h"
#include "Chain of Responsibility\ShRequest.h"

ShDrawToolBar::ShDrawToolBar(const QString &title, ShChain *chain, QWidget *parent)
	:ShAbstractToolBar(title, chain, parent) {

	this->setObjectName("DrawToolBar");
	this->setIconSize(QSize(20, 20));

	QAction *action = this->addAction(ShIcon(":/Image/Draw/Line.png"), "Line");
	action->setToolTip("Line \nCreates straight line segments");
	connect(action, &QAction::triggered, this, &ShDrawToolBar::lineActionClicked);

	action = this->addAction(ShIcon(":/Image/Draw/ConstructionLine.png"), "ConstructionLine");
	connect(action, &QAction::triggered, this, &ShDrawToolBar::constructionLineActionClicked);

	action = this->addAction(ShIcon(":/Image/Draw//PolyLine/PolyLine.png"), "PolyLine");
	action->setToolTip("PolyLine \nCreates a polyline");
	connect(action, &QAction::triggered, this, &ShDrawToolBar::polyLineActionClicked);

	action = this->addAction(ShIcon(":/Image/Draw/Circle/Center-Radius.png"), "Circle");
	action->setToolTip("Circle \nCreates a circle using a center point and a radius");
	connect(action, &QAction::triggered, this, &ShDrawToolBar::circleActionClicked);

	action = this->addAction(ShIcon(":/Image/Draw/Arc/3Point.png"), "Arc");
	action->setToolTip("Arc \nCreates an arc using three points");
	connect(action, &QAction::triggered, this, &ShDrawToolBar::arcActionClicked);

	action = this->addAction(ShIcon(":/Image/Draw/Arc/3Point.png"), "Ellipse");
	action->setToolTip("Arc \nCreates an arc using three points");
	connect(action, &QAction::triggered, this, &ShDrawToolBar::arcActionClicked);

	action = this->addAction(ShIcon(":/Image/Draw/Point.png"), "Point");
	connect(action, &QAction::triggered, this, &ShDrawToolBar::pointActionClicked);

}

ShDrawToolBar::~ShDrawToolBar() {


}


void ShDrawToolBar::lineActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawLine);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDrawToolBar::constructionLineActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawContructionLine);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDrawToolBar::polyLineActionClicked() {


}

void ShDrawToolBar::circleActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawCircleCenterRadius);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}
// ������Բ��ť�������
void ShDrawToolBar::ellipseActionClicked() {
	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawEllipseCenterAxis);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}
void ShDrawToolBar::arcActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawArcThreePoint);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDrawToolBar::pointActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawPoint);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}