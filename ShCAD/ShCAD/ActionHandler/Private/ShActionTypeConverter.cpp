
#include "ShActionTypeConverter.h"

ShActionTypeConverter::ShActionTypeConverter() {

}

ShActionTypeConverter::~ShActionTypeConverter() {


}

QString ShActionTypeConverter::convert(ActionType actionType) {

	QString text;

	switch (actionType)
	{
	case ActionType::ActionDefault:
		text = ""; break;
	case ActionType::ActionDrawLine:
		text = "_Line"; break;
	case ActionType::ActionDrawContructionLine:
		text = "_XLine"; break;
	case ActionType::ActionDrawCircleCenterRadius:
		text = "_Circle"; break;
	case ActionType::ActionDrawCircleCenterDiameter:
		text = "_Circle"; break;
	case ActionType::ActionDrawCircleTwoPoint:
		text = "_Circle"; break;
	case ActionType::ActionDrawCircleThreePoint:
		text = "_Circle"; break;


	case ActionType::ActionDrawArcThreePoint:
		text = "_Arc"; break;



	case ActionType::ActionDrawArcStartCenterEnd:
		text = "_Arc"; break;
	case ActionType::ActionDrawArcStartCenterAngle:
		text = "_Arc"; break;
	case ActionType::ActionDrawArcStartCenterLength:
		text = "_Arc"; break;


	case ActionType::ActionDrawArcStartEndAngle:
		text = "_Arc"; break;
	case ActionType::ActionDrawArcStartEndDirection:
		text = "_Arc"; break;
	case ActionType::ActionDrawArcStartEndRadius:
		text = "_Arc"; break;


	case ActionType::ActionDrawArcCenterStartEnd:
		text = "_Arc"; break;
	case ActionType::ActionDrawArcCenterStartAngle:
		text = "_Arc"; break;
	case ActionType::ActionDrawArcCenterStartLength:
		text = "_Arc"; break;

	case ActionType::ActionDrawEllipseCenterAxis:
	case ActionType::ActionDrawEllipseAxisEnd:
		text = "_Ellipse";
		break;

	case ActionType::ActionModifyMove:
		text = "_Move"; break;
	case ActionType::ActionModifyCopy:
		text = "_Copy"; break;
	case ActionType::ActionModifyRotate:
		text = "_Rotate"; break;
	case ActionType::ActionModifyMirror:
		text = "_Mirror"; break;
	case ActionType::ActionModifyErase:
		text = "_Erase"; break;
	case ActionType::ActionModifyExtend:
		text = "_Extend"; break;
	case ActionType::ActionModifyTrim:
		text = "_Trim"; break;
	case ActionType::ActionModifyStretch:
		text = "_Stretch"; break;
	case ActionType::ActionModifyOffset:
		text = " _Offset"; break;


	case ActionType::ActionDrawDimLinear:
		text = " _DimLinear"; break;
	case ActionType::ActionDrawDimAligned:
		text = " _DimAligned"; break;
	case ActionType::ActionDrawDimRadius:
		text = " _DimRadius"; break;
	case ActionType::ActionDrawDimDiameter:
		text = " _DimDiameter"; break;
	case ActionType::ActionDrawDimArcLength:
		text = " _DimArcLength"; break;
	case ActionType::ActionDrawDimAngular:
		text = " _DimAngular"; break;

	case ActionType::ActionDrawPoint:
		text = " _Point"; break;

	case ActionType::ActionSelectPlotArea:
		text = " _Plot"; break;

	default:
		text = ""; break;
	}

	return text;

}

ActionType ShActionTypeConverter::convert(const QString &text) {

	return ActionType::ActionUnknown;

}