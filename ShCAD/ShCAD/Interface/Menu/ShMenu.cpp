

#include "ShMenu.h"
#include <QFile>
#include <qdebug.h>
#include <Manager\ShCADWidgetManager.h>
#include "Entity\Private\ShEntityData.h"
#include "Data\ShPropertyData.h"
#include "Base\ShLayerTable.h"
#include "Entity\Private\ShEntityData.h"
#include <QFileDialog>
#include "Base\ShLayer.h"
#include "Data\ShColor.h"
#include "Entity\Leaf\ShLine.h"
#include "Entity\Leaf\ShCircle.h"
#include <QJsonDocument>
#include <QJsonArray>
#include "Entity\Leaf\ShArc.h"
#include "Interface\Item\ShIcon.h"
#include "Entity\Composite\Dim\ShDimAligned.h"
#include "Entity\Composite\Dim\ShDimAngular.h"
#include "Entity\Composite\Dim\ShDimArcLength.h"
#include "Entity\Composite\Dim\ShDimDiameter.h"
#include "Entity\Composite\Dim\ShDimLinear.h"
#include "Entity\Composite\Dim\ShDimRadius.h"
#include "Interface\ShCADWidget.h"
#include <qdebug.h>
#include <QJsonArray>
#include <QCoreApplication>
#include <QJsonObject>
#include "Manager\ShLanguageManager.h"
#include "Chain of Responsibility\ShRequest.h"
#include "ActionHandler\Private\ShChangeActionStrategy.h"
#include "Interface\Dialog\ShPlotDialog.h"
#include "Manager\ShPlotManager.h"
#include "Interface\Dialog\ShPlotPreviewDialog.h"
#include "Entity\Composite\ShEntityTable.h"
#include <vector>  // Added for std::vector
#include <stdexcept>  // Added for std::invalid_argument
#include <Entity/Leaf/ShBlock.h>
#include <QList>
#include "Data\ShPropertyData.h"
#include "Data\ShLineStyle.h"
#include <QDataStream>
#include "Base/ShDimensionStyleTable.h"
#include "Base/ShDimensionStyle.h"
#include "Entity\Leaf\ShEllipse.h"
// 递归解析实体函数（新增）
QList<ShEntity*> parseEntitiesFromJsonArray(
	const QJsonArray& jsonArray,
	ShLayerTable* layertable,
	ShDimensionStyleTable* styleTable,
	ShCADWidget* widget,
	ShBlock* parentBlock = nullptr)
{
	QList<ShEntity*> entities;
	ShDimensionStyle* style = styleTable->getCurrentStyle();

	for (const QJsonValue& value : jsonArray) {
		bool isDim = false;
		if (!value.isObject()) continue;
		QJsonObject obj = value.toObject();
		QString type = obj["type"].toString();

		// 解析通用属性
		QJsonObject colorObj = obj["color"].toObject();
		ShPropertyData propertyData;
		ShColor color(
			colorObj["r"].toInt(255),
			colorObj["g"].toInt(255),
			colorObj["b"].toInt(255),
			ShColor::Normal
		);

		// 解析线型
		if (obj.contains("lineStyle")) {
			QJsonObject lineStyleObj = obj["lineStyle"].toObject();
			unsigned short pattern = static_cast<unsigned short>(lineStyleObj["pattern"].toInt());
			QString typeStr = lineStyleObj["type"].toString("Normal");
			ShLineStyle::Type lineType = ShLineStyle::Normal;

			if (typeStr == "Invalid") lineType = ShLineStyle::Invalid;
			else if (typeStr == "ByLayer") lineType = ShLineStyle::ByLayer;
			else if (typeStr == "ByBlock") lineType = ShLineStyle::ByBlock;

			ShLineStyle lineStyle(pattern, lineType);
			propertyData.setLineStyle(lineStyle);
		}

		propertyData.setColor(color);

		// 解析图层
		QString layerSet = obj["layer"].toString();
		ShLayer* layerTo = nullptr;
		if (layerSet == "0") layerTo = layertable->getLayer(0);
		else if (layerSet == "temp1") layerTo = layertable->getLayer(1);
		else if (layerSet == "temp2") layerTo = layertable->getLayer(2);

		// 处理 BLOCK 类型（递归解析）
		if (type == "Block") {
			// 解析基点
			QJsonObject basePointObj = obj["basePoint"].toObject();
			ShPoint3d basePoint;
			basePoint.x = basePointObj["x"].toDouble();
			basePoint.y = basePointObj["y"].toDouble();
			basePoint.z = basePointObj["z"].toDouble();

			// 创建新块
			ShBlock* newBlock = new ShBlock(basePoint, propertyData, layerTo);

			// 递归解析块内容
			QJsonArray blockData = obj["data"].toArray();
			QList<ShEntity*> blockEntities = parseEntitiesFromJsonArray(
				blockData, layertable, styleTable, widget, newBlock);

			// 添加块实体到块
			for (ShEntity* entity : blockEntities) {
				newBlock->addEntity(entity);
			}

			// 设置块属性
			newBlock->unSelect();

			// 添加到父块或直接添加到实体表
			if (parentBlock) {
				parentBlock->addEntity(newBlock);
			}
			else {
				widget->getEntityTable().add(newBlock);
				if (layerTo) layerTo->add(newBlock);
			}

			entities.append(newBlock);
			continue;
		}

		// 解析普通实体
		ShEntity* newent = nullptr;

		if (type == "Line") {

			QJsonObject data = obj["data"].toObject();
			QJsonObject start = data["start"].toObject();
			QJsonObject end = data["end"].toObject();
			// 先定义 ShLineData，再填充数据
			ShLineData lineData;
			lineData.start.x = start["x"].toDouble();
			lineData.start.y = start["y"].toDouble();
			lineData.end.x = end["x"].toDouble();
			lineData.end.y = end["y"].toDouble();
			newent = new ShLine(lineData);
			
		}
		else if (type == "Circle") {
			QJsonObject data = obj["data"].toObject();
			QJsonObject center = data["center"].toObject();
			double radius = data["radius"].toDouble();
			// 先定义 ShCircleData，再填充数据
			ShCircleData circleData;
			circleData.center.x = center["x"].toDouble();
			circleData.center.y = center["y"].toDouble();
			circleData.radius = radius;
			newent = new ShCircle(circleData);
			

		}
		else if (type == "Ellipse") {
			QJsonObject data = obj["data"].toObject();
			QJsonObject center = data["center"].toObject();

			// 获取椭圆参数
			double majorRadius = data["majorRadius"].toDouble();
			double minorRadius = data["minorRadius"].toDouble();
			double angle = data["angle"].toDouble();

			// 创建椭圆数据对象并填充数据
			ShEllipseData ellipseData;
			ellipseData.center.x = center["x"].toDouble();
			ellipseData.center.y = center["y"].toDouble();
			ellipseData.center.z = center["z"].toDouble(); // 如果是3D坐标
			ellipseData.majorRadius = majorRadius;
			ellipseData.minorRadius = minorRadius;
			ellipseData.angle = angle;

			// 创建椭圆实体并设置属性
			newent = new ShEllipse(ellipseData);
			
		}

		else if (type == "Arc") {
			QJsonObject data = obj["data"].toObject();
			QJsonObject center = data["center"].toObject();
			double radius = data["radius"].toDouble();
			double startAngle = data["startAngle"].toDouble();
			double endAngle = data["endAngle"].toDouble();
			// 先定义 ShArcData，再填充数据
			ShArcData arcData;
			arcData.center.x = center["x"].toDouble();
			arcData.center.y = center["y"].toDouble();
			arcData.radius = radius;
			arcData.startAngle = startAngle;
			arcData.endAngle = endAngle;
			newent = new ShArc(arcData);
			

		}
		else if (type == "DimLinear") {
			QJsonObject data = obj["data"].toObject();
			QJsonObject firstOrigin = data["firstOrigin"].toObject();
			QJsonObject secondOrigin = data["secondOrigin"].toObject();
			QJsonObject firstDim = data["firstDim"].toObject();
			QJsonObject secondDim = data["secondDim"].toObject();
			QJsonObject text = data["text"].toObject();

			ShDimLinearData dimData;
			// 填充第一个原点坐标
			dimData.firstOrigin.x = firstOrigin["x"].toDouble();
			dimData.firstOrigin.y = firstOrigin["y"].toDouble();
			dimData.firstOrigin.z = firstOrigin["z"].toDouble();

			// 填充第二个原点坐标
			dimData.secondOrigin.x = secondOrigin["x"].toDouble();
			dimData.secondOrigin.y = secondOrigin["y"].toDouble();
			dimData.secondOrigin.z = secondOrigin["z"].toDouble();

			// 填充第一个尺寸线端点坐标
			dimData.firstDim.x = firstDim["x"].toDouble();
			dimData.firstDim.y = firstDim["y"].toDouble();
			dimData.firstDim.z = firstDim["z"].toDouble();

			// 填充第二个尺寸线端点坐标
			dimData.secondDim.x = secondDim["x"].toDouble();
			dimData.secondDim.y = secondDim["y"].toDouble();
			dimData.secondDim.z = secondDim["z"].toDouble();

			// 填充文本位置坐标
			dimData.text.x = text["x"].toDouble();
			dimData.text.y = text["y"].toDouble();
			dimData.text.z = text["z"].toDouble();

			// 创建线性标注实体
			ShDimLinear* addent = new ShDimLinear(dimData, propertyData, layerTo, style);
			newent = addent->clone();
			style->add(addent);

		}
		else if (type == "DimAligned") {
			QJsonObject data = obj["data"].toObject();
			QJsonObject firstOrigin = data["firstOrigin"].toObject();
			QJsonObject secondOrigin = data["secondOrigin"].toObject();
			QJsonObject firstDim = data["firstDim"].toObject();
			QJsonObject secondDim = data["secondDim"].toObject();
			QJsonObject text = data["text"].toObject();

			ShDimAlignedData dimData;
			dimData.firstOrigin.x = firstOrigin["x"].toDouble();
			dimData.firstOrigin.y = firstOrigin["y"].toDouble();
			dimData.firstOrigin.z = firstOrigin["z"].toDouble();

			dimData.secondOrigin.x = secondOrigin["x"].toDouble();
			dimData.secondOrigin.y = secondOrigin["y"].toDouble();
			dimData.secondOrigin.z = secondOrigin["z"].toDouble();

			dimData.firstDim.x = firstDim["x"].toDouble();
			dimData.firstDim.y = firstDim["y"].toDouble();
			dimData.firstDim.z = firstDim["z"].toDouble();

			dimData.secondDim.x = secondDim["x"].toDouble();
			dimData.secondDim.y = secondDim["y"].toDouble();
			dimData.secondDim.z = secondDim["z"].toDouble();

			dimData.text.x = text["x"].toDouble();
			dimData.text.y = text["y"].toDouble();
			dimData.text.z = text["z"].toDouble();

			ShDimAligned* addent = new ShDimAligned(dimData, propertyData, layerTo, style);
			newent = addent->clone();
			style->add(addent);

		}
		else if (type == "DimArcLength") {
			QJsonObject data = obj["data"].toObject();
			QJsonObject center = data["center"].toObject();
			QJsonObject start = data["start"].toObject();
			QJsonObject end = data["end"].toObject();
			QJsonObject boundary = data["boundary"].toObject();
			QJsonObject text = data["text"].toObject();

			ShDimArcLengthData dimData;
			// 填充圆心坐标
			dimData.center.x = center["x"].toDouble();
			dimData.center.y = center["y"].toDouble();
			dimData.center.z = center["z"].toDouble();

			// 填充半径
			dimData.radius = data["radius"].toDouble();

			// 填充起点坐标
			dimData.start.x = start["x"].toDouble();
			dimData.start.y = start["y"].toDouble();
			dimData.start.z = start["z"].toDouble();

			// 填充终点坐标
			dimData.end.x = end["x"].toDouble();
			dimData.end.y = end["y"].toDouble();
			dimData.end.z = end["z"].toDouble();

			// 填充边界点坐标
			dimData.boundary.x = boundary["x"].toDouble();
			dimData.boundary.y = boundary["y"].toDouble();
			dimData.boundary.z = boundary["z"].toDouble();

			// 填充文本位置坐标
			dimData.text.x = text["x"].toDouble();
			dimData.text.y = text["y"].toDouble();
			dimData.text.z = text["z"].toDouble();

			// 创建圆弧长度标注实体
			ShDimArcLength* addent = new ShDimArcLength(dimData, propertyData, layerTo, style);
			newent = addent->clone();
			style->add(addent);
		}
		else if (type == "DimAngular") {
			QJsonObject data = obj["data"].toObject();
			QJsonObject center = data["center"].toObject();
			QJsonObject start = data["start"].toObject();
			QJsonObject end = data["end"].toObject();
			QJsonObject boundary = data["boundary"].toObject();
			QJsonObject text = data["text"].toObject();

			ShDimAngularData dimData;
			dimData.center.x = center["x"].toDouble();
			dimData.center.y = center["y"].toDouble();
			dimData.center.z = center["z"].toDouble();

			dimData.start.x = start["x"].toDouble();
			dimData.start.y = start["y"].toDouble();
			dimData.start.z = start["z"].toDouble();

			dimData.end.x = end["x"].toDouble();
			dimData.end.y = end["y"].toDouble();
			dimData.end.z = end["z"].toDouble();

			dimData.boundary.x = boundary["x"].toDouble();
			dimData.boundary.y = boundary["y"].toDouble();
			dimData.boundary.z = boundary["z"].toDouble();

			dimData.text.x = text["x"].toDouble();
			dimData.text.y = text["y"].toDouble();
			dimData.text.z = text["z"].toDouble();

			ShDimAngular* addent = new ShDimAngular(dimData, propertyData, layerTo, style);
			newent = addent->clone();
			style->add(addent);

		}
		else if (type == "DimRadius") {
			QJsonObject data = obj["data"].toObject();
			QJsonObject center = data["center"].toObject();
			QJsonObject dim = data["dim"].toObject();
			QJsonObject text = data["text"].toObject();

			ShDimRadiusData dimData;
			dimData.center.x = center["x"].toDouble();
			dimData.center.y = center["y"].toDouble();
			dimData.center.z = center["z"].toDouble();

			dimData.dim.x = dim["x"].toDouble();
			dimData.dim.y = dim["y"].toDouble();
			dimData.dim.z = dim["z"].toDouble();

			dimData.text.x = text["x"].toDouble();
			dimData.text.y = text["y"].toDouble();
			dimData.text.z = text["z"].toDouble();

			ShDimRadius* addent = new ShDimRadius(dimData, propertyData, layerTo, style);
			newent = addent->clone();
			style->add(addent);

		}
		else if (type == "DimDiameter") {
			QJsonObject data = obj["data"].toObject();
			QJsonObject center = data["center"].toObject();
			QJsonObject firstDim = data["firstDim"].toObject();
			QJsonObject secondDim = data["secondDim"].toObject();
			QJsonObject text = data["text"].toObject();

			ShDimDiameterData dimData;
			dimData.center.x = center["x"].toDouble();
			dimData.center.y = center["y"].toDouble();
			dimData.center.z = center["z"].toDouble();

			dimData.firstDim.x = firstDim["x"].toDouble();
			dimData.firstDim.y = firstDim["y"].toDouble();
			dimData.firstDim.z = firstDim["z"].toDouble();

			dimData.secondDim.x = secondDim["x"].toDouble();
			dimData.secondDim.y = secondDim["y"].toDouble();
			dimData.secondDim.z = secondDim["z"].toDouble();

			dimData.text.x = text["x"].toDouble();
			dimData.text.y = text["y"].toDouble();
			dimData.text.z = text["z"].toDouble();

			ShDimDiameter* addent = new ShDimDiameter(dimData, propertyData, layerTo, style);
			newent = addent->clone();
			style->add(addent);

		}
		if (!newent) continue;

		// 设置实体属性
		newent->setLayer(layerTo);
		newent->unSelect();
		newent->setPropertyData(propertyData);

		// 添加到父块或直接添加到实体表
		if (parentBlock) {
			parentBlock->addEntity(newent);
		}
		else {
			widget->getEntityTable().add(newent);
			if (layerTo) layerTo->add(newent);
		}

		

		entities.append(newent);
	}

	return entities;
}

// 修改后的读取函数
void ShEmptyDrawingFileMenu::readActionClicked() {
	QString fileName = QFileDialog::getOpenFileName(
		this,
		tr("Read Path"),
		"E:\\Documents\\arc.json", // 默认路径
		tr("JSON(*.json);;all File (*.*)")
	);
	if (fileName.isEmpty()) {
		// 用户取消了操作
		return;
	}
	ShRequestCreateNewCADWidget request;
	this->request(&request);
	ShCADWidget *widget = ShCADWidgetManager::getInstance()->getLastWidget();
	qDebug() << fileName;
	ShLayerTable *layertable = widget->getLayerTable();
	ShDimensionStyleTable *styleTable = widget->getDimensionStyleTable();
	ShDimensionStyle *style = styleTable->getCurrentStyle();
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "无法打开文件：" << fileName;
		return;
	}
	// 2. 读取 JSON 数据
	QByteArray jsonData = file.readAll();
	file.close();
	// 3. 解析 JSON
	QJsonDocument doc = QJsonDocument::fromJson(jsonData);
	if (doc.isNull()) {
		qWarning() << "Invalid JSON data!";
		return;
	}
	// 4. 检查是否是数组
	if (!doc.isArray()) {
		qWarning() << "JSON is not an array!";
		return;
	}
	QJsonArray jsonArray = doc.array();;

	// 使用递归函数解析
	parseEntitiesFromJsonArray(jsonArray, layertable, styleTable, widget);

	widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawAddedEntities));
}

void readActionClicked233() {
	QString fileName = QFileDialog::getOpenFileName(
	//	this,
	//	tr("Read Path"),
	//	"E:\\Documents\\arc.json", // 默认路径
	//	tr("JSON(*.json);;all File (*.*)")
	);
	if (fileName.isEmpty()) {
		// 用户取消了操作
		return;
	}
	ShRequestCreateNewCADWidget request;
	//this->request(&request);
	ShCADWidget *widget = ShCADWidgetManager::getInstance()->getLastWidget();
	qDebug() << fileName;
	ShLayerTable *layertable = widget->getLayerTable();
	ShDimensionStyleTable *styleTable = widget->getDimensionStyleTable();
	ShDimensionStyle *style = styleTable->getCurrentStyle();
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "无法打开文件：" << fileName;
		return;
	}
	// 2. 读取 JSON 数据
	QByteArray jsonData = file.readAll();
	file.close();
	// 3. 解析 JSON
	QJsonDocument doc = QJsonDocument::fromJson(jsonData);
	if (doc.isNull()) {
		qWarning() << "Invalid JSON data!";
		return;
	}
	// 4. 检查是否是数组
	if (!doc.isArray()) {
		qWarning() << "JSON is not an array!";
		return;
	}
	QJsonArray jsonArray = doc.array();
	qDebug() << jsonArray.size();
	QList<ShEntityData*> entities;
	for (const QJsonValue &value : jsonArray) {
		bool isDim = 0;
		if (!value.isObject()) continue;
		QJsonObject obj = value.toObject();
		QString type = obj["type"].toString();
		// 4. 提取颜色（可选）
		QJsonObject colorObj = obj["color"].toObject();
		ShPropertyData propertyData;

		ShColor color(
			colorObj["r"].toInt(255),  // 默认值 255
			colorObj["g"].toInt(255),
			colorObj["b"].toInt(255),
			ShColor::Normal           // 默认类型 Normal
		);
		// 新增线型处理
		if (obj.contains("lineStyle")) {
			QJsonObject lineStyleObj = obj["lineStyle"].toObject();
			unsigned short pattern = static_cast<unsigned short>(lineStyleObj["pattern"].toInt());

			QString typeStr = lineStyleObj["type"].toString("Normal");
			ShLineStyle::Type lineType = ShLineStyle::Normal;

			if (typeStr == "Invalid") lineType = ShLineStyle::Invalid;
			else if (typeStr == "ByLayer") lineType = ShLineStyle::ByLayer;
			else if (typeStr == "ByBlock") lineType = ShLineStyle::ByBlock;

			ShLineStyle lineStyle(pattern, lineType);
			propertyData.setLineStyle(lineStyle);
		}
		propertyData.setColor(color);
		QString layerSet = obj["layer"].toString();
		ShLayer* layerTo;
		if (layerSet == "0") {
			layerTo = layertable->getLayer(0);
		}
		else if (layerSet == "temp1") {
			layerTo = layertable->getLayer(1);
		}
		else if (layerSet == "temp2") {
			layerTo = layertable->getLayer(2);
		}

		// 5. 根据类型解析数据
		

	}
	widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawAddedEntities));

}

QJsonArray forBlock(ShBlock *block){
	QList<ShEntity*> entities = block->getEntities();
	QJsonArray jsonArray;
	for (ShEntity* entity : entities) {
		QJsonObject jsonObj;
		QJsonObject colorJson;

		if (ShLine* ent = dynamic_cast<ShLine*>(entity)) {
			jsonObj = ent->getJsonData();
		}
		else if (ShBlock* ent = dynamic_cast<ShBlock*>(entity)) {
			jsonObj["data"] = forBlock(ent);
			jsonObj["type"] = "Block";
			ShPoint3d basePoint = ent->getBasePoint();
			QJsonObject basePointJson;
			basePointJson["x"] = basePoint.x;
			basePointJson["y"] = basePoint.y;
			basePointJson["z"] = basePoint.z;
			jsonObj["basePoint"] = basePointJson;

		}
		else if (ShCircle* ent = dynamic_cast<ShCircle*>(entity)) {
			jsonObj = ent->getJsonData();
		}
		else if (ShEllipse* ent = dynamic_cast<ShEllipse*>(entity)) {
			jsonObj = ent->getJsonData();
		}
		else if (ShArc* ent = dynamic_cast<ShArc*>(entity)) {
			jsonObj = ent->getJsonData();
		}
		else if (ShDimAligned* ent = dynamic_cast<ShDimAligned*>(entity)) {
			jsonObj = ent->getJsonData();
		}
		else if (ShDimAngular* ent = dynamic_cast<ShDimAngular*>(entity)) {
			jsonObj = ent->getJsonData();
		}
		else if (ShDimArcLength* ent = dynamic_cast<ShDimArcLength*>(entity)) {
			jsonObj = ent->getJsonData();
		}

		else if (ShDimDiameter* ent = dynamic_cast<ShDimDiameter*>(entity)) {
			jsonObj = ent->getJsonData();
		}
		else if (ShDimLinear* ent = dynamic_cast<ShDimLinear*>(entity)) {
			jsonObj = ent->getJsonData();
		}
		else if (ShDimRadius* ent = dynamic_cast<ShDimRadius*>(entity)) {
			jsonObj = ent->getJsonData();
		}
		ShColor color = entity->getPropertyData().getColor();
		colorJson["r"] = color.getRed();
		colorJson["g"] = color.getGreen();
		colorJson["b"] = color.getBlue();
		jsonObj["color"] = colorJson;
		jsonObj["layer"] = entity->getLayer()->getName();

		QJsonObject lineStyleJson;  // 新增线型JSON对象
		// 新增线型处理
		ShLineStyle lineStyle = entity->getPropertyData().getLineStyle();
		lineStyleJson["pattern"] = static_cast<int>(lineStyle.getPattern());

		switch (lineStyle.getType()) {
		case ShLineStyle::Invalid: lineStyleJson["type"] = "Invalid"; break;
		case ShLineStyle::Normal:  lineStyleJson["type"] = "Normal"; break;
		case ShLineStyle::ByLayer: lineStyleJson["type"] = "ByLayer"; break;
		case ShLineStyle::ByBlock: lineStyleJson["type"] = "ByBlock"; break;
		default: lineStyleJson["type"] = "Normal"; break;
		}
		jsonObj["lineStyle"] = lineStyleJson;  // 添加线型到主JSON
		jsonArray.append(jsonObj);
	}
	
	return jsonArray;

}


void ShFileMenu::saveActionClicked() {
	QString fileName = QFileDialog::getSaveFileName(
		this,
		tr("Save Path"),
		"E:\\Documents\\arc.json", // 默认路径
		tr("JSON(*.json);;所有文件 (*.*)")
	);
	if (fileName.isEmpty()) {
		// 用户取消了操作
		return;
	}
	ShCADWidget *widget = ShCADWidgetManager::getInstance()->getActivatedWidget();
	ShEntityTable& entityTable = widget->getEntityTable();
	QJsonArray jsonArray;

	for (ShEntity* entity : entityTable) {
			QJsonObject jsonObj;
			QJsonObject colorJson;

			if (ShLine* ent = dynamic_cast<ShLine*>(entity)) {
				jsonObj = ent->getJsonData();
			}
			else if (ShBlock* ent = dynamic_cast<ShBlock*>(entity)) {
				jsonObj["data"] = forBlock(ent);
				jsonObj["type"] = "Block";
				ShPoint3d basePoint = ent->getBasePoint();
				QJsonObject basePointJson;
				basePointJson["x"] = basePoint.x;
				basePointJson["y"] = basePoint.y;
				basePointJson["z"] = basePoint.z;
				jsonObj["basePoint"] = basePointJson;

			}
			else if (ShCircle* ent = dynamic_cast<ShCircle*>(entity)) {
				jsonObj = ent->getJsonData();
			}
			else if (ShEllipse* ent = dynamic_cast<ShEllipse*>(entity)) {
				jsonObj = ent->getJsonData();
			}
			else if (ShArc* ent = dynamic_cast<ShArc*>(entity)) {
				jsonObj = ent->getJsonData();
			}
			else if (ShDimAligned* ent = dynamic_cast<ShDimAligned*>(entity)) {
				jsonObj = ent->getJsonData();
			}
			else if (ShDimAngular* ent = dynamic_cast<ShDimAngular*>(entity)) {
				jsonObj = ent->getJsonData();
			}
			else if (ShDimArcLength* ent = dynamic_cast<ShDimArcLength*>(entity)) {
				jsonObj = ent->getJsonData();
			}
			else if (ShDimDiameter* ent = dynamic_cast<ShDimDiameter*>(entity)) {
				jsonObj = ent->getJsonData();
			}
			else if (ShDimLinear* ent = dynamic_cast<ShDimLinear*>(entity)) {
				jsonObj = ent->getJsonData();
			}
			else if (ShDimRadius* ent = dynamic_cast<ShDimRadius*>(entity)) {
				jsonObj = ent->getJsonData();
			}
			ShColor color = entity->getPropertyData().getColor();
			colorJson["r"] = color.getRed();
			colorJson["g"] = color.getGreen();
			colorJson["b"] = color.getBlue();
			jsonObj["color"] = colorJson;
			jsonObj["layer"] = entity->getLayer()->getName();

			QJsonObject lineStyleJson;  // 新增线型JSON对象
			// 新增线型处理
			ShLineStyle lineStyle = entity->getPropertyData().getLineStyle();
			lineStyleJson["pattern"] = static_cast<int>(lineStyle.getPattern());

			switch (lineStyle.getType()) {
			case ShLineStyle::Invalid: lineStyleJson["type"] = "Invalid"; break;
			case ShLineStyle::Normal:  lineStyleJson["type"] = "Normal"; break;
			case ShLineStyle::ByLayer: lineStyleJson["type"] = "ByLayer"; break;
			case ShLineStyle::ByBlock: lineStyleJson["type"] = "ByBlock"; break;
			default: lineStyleJson["type"] = "Normal"; break;
			}
			jsonObj["lineStyle"] = lineStyleJson;  // 添加线型到主JSON

			jsonArray.append(jsonObj);

	}

	QJsonDocument combinedDoc(jsonArray);
	QString combinedJson = combinedDoc.toJson(QJsonDocument::Indented);
	qDebug() << "Combined JSON:\n" << combinedJson;

	QFile file(fileName);

	if (!file.open(QIODevice::WriteOnly)) {
		qDebug() << "Failed to open file for writing:" << file.errorString();

		return;
	}
	file.write(combinedDoc.toJson(QJsonDocument::Indented));
	file.close();
	// 写入文件（自动格式化缩进）
}


ShAbstractMenu::ShAbstractMenu(const QString &title, ShChain *chain, QWidget *parent)
	:QMenu(title, parent), ShChain(chain) {

}

ShAbstractMenu::~ShAbstractMenu() {

}

///////////////////////////////////////////////////////////////////////////

ShEmptyDrawingFileMenu::ShEmptyDrawingFileMenu(const QString &title, ShChain *chain, QWidget *parent)
	:ShAbstractMenu(title, chain, parent) {

	this->newAction = new QAction(ShIcon(":/Image/File/New.png"), shGetLanValue_ui("File/New"), this);

	this->addAction(this->newAction);

	this->readAction = new QAction(ShIcon(":/Image/File/New.png"), shGetLanValue_ui("File/Read"), this);

	this->addAction(this->readAction);

	connect(this->newAction, &QAction::triggered, this, &ShEmptyDrawingFileMenu::newActionClicked);
	connect(this->readAction, &QAction::triggered, this, &ShEmptyDrawingFileMenu::readActionClicked);
}

ShEmptyDrawingFileMenu::~ShEmptyDrawingFileMenu() {

}

void ShEmptyDrawingFileMenu::newActionClicked() {
	
	ShRequestCreateNewCADWidget request;
	this->request(&request);
}

///////////////////////////////////////////////////////////////////////////

ShFileMenu::ShFileMenu(const QString &title, ShChain *chain, QWidget *parent)
	:ShAbstractMenu(title, chain, parent) {

	this->newAction = new QAction(ShIcon(":/Image/File/New.png"), shGetLanValue_ui("File/New"), this);
	this->addAction(this->newAction);

	this->addSeparator();

	this->plotAction = new QAction(ShIcon(":/Image/File/Print.png"), shGetLanValue_ui("File/Plot"), this);
	this->addAction(this->plotAction);

	this->previewAction = new QAction(ShIcon(":/Image/File/Preview.png"), shGetLanValue_ui("File/Preview"), this);
	this->addAction(this->previewAction);

	this->saveAction = new QAction(ShIcon(":/Image/File/Preview.png"), shGetLanValue_ui("File/Save"), this);
	this->addAction(this->saveAction);

	connect(this->newAction, &QAction::triggered, this, &ShFileMenu::newActionClicked);
	connect(this->plotAction, &QAction::triggered, this, &ShFileMenu::plotActionClicked);
	connect(this->previewAction, &QAction::triggered, this, &ShFileMenu::previewActionClicked);
	connect(this->saveAction, &QAction::triggered, this, &ShFileMenu::saveActionClicked);
}

ShFileMenu::~ShFileMenu() {

}

void ShFileMenu::newActionClicked() {
	
	ShRequestCreateNewCADWidget request;
	this->request(&request);
}

void ShFileMenu::plotActionClicked() {

	ShPlotDialog *dialog = new ShPlotDialog(this);
	dialog->exec();
}


void ShFileMenu::previewActionClicked() {

	if (ShPlotManager::getInstance()->isSavedInfo() == false)
		return;

	ShPlotPreviewDialog dialog(this);
	dialog.exec();
}

/////////////////////////////////////////////////////////////////////////

ShEditMenu::ShEditMenu(const QString &title, ShChain *chain, QWidget *parent)
	:ShAbstractMenu(title, chain, parent) {

}

ShEditMenu::~ShEditMenu() {

}


/////////////////////////////////////////////////////////////////////////

ShDrawMenu::ShDrawMenu(const QString &title, ShChain *chain, QWidget *parent)
	:ShAbstractMenu(title, chain, parent) {

	this->lineAction = new QAction(ShIcon(":/Image/Draw/Line.png"),
		shGetLanValue_ui("Draw/Line"), this);
	this->addAction(this->lineAction);

	this->addSeparator();

	this->polyLineAction = new QAction(ShIcon(":/Image/Draw/PolyLine/PolyLine.png"),
		shGetLanValue_ui("Draw/PolyLine"), this);
	this->addAction(this->polyLineAction);

	this->polygonAction = new QAction(ShIcon(":/Image/Draw/PolyLine/Polygon.png"),
		shGetLanValue_ui("Draw/Polygon"), this);
	this->addAction(this->polygonAction);

	this->rectangleAction = new QAction(ShIcon(":/Image/Draw/PolyLine/Rectangle.png"),
		shGetLanValue_ui("Draw/Rectangle"), this);
	this->addAction(this->rectangleAction);

	this->addSeparator();
	//this->createEllipseMenu();  // 创建椭圆菜单

	this->createCircleMenu();

	this->addSeparator();

	this->createArcMenu();
	
	connect(this->lineAction, &QAction::triggered, this, &ShDrawMenu::lineActionClicked);
}

ShDrawMenu::~ShDrawMenu() {


}

void ShDrawMenu::createCircleMenu() {

	this->circleMenu = new QMenu(shGetLanValue_ui("Draw/Circle"), this);
	this->addMenu(this->circleMenu);

	this->circleCenterRadiusAction = new QAction(ShIcon(":/Image/Draw/Circle/Center-Radius.png"), 
		shGetLanValue_ui("Draw/Circle,Center,Radius"),this->circleMenu);
	this->circleMenu->addAction(this->circleCenterRadiusAction);

	this->circleCenterDiameterAction = new QAction(ShIcon(":/Image/Draw/Circle/Center-Diameter.png"), 
		shGetLanValue_ui("Draw/Circle,Center,Diameter"), this->circleMenu);
	this->circleMenu->addAction(this->circleCenterDiameterAction);


	this->circleMenu->addSeparator();

	this->circleTwoPointAction = new QAction(ShIcon(":/Image/Draw/Circle/2Point.png"), 
		shGetLanValue_ui("Draw/Circle,TwoPoints"), this->circleMenu);
	this->circleMenu->addAction(this->circleTwoPointAction);

	this->circleThreePointAction = new QAction(ShIcon(":/Image/Draw/Circle/3Point.png"), 
		shGetLanValue_ui("Draw/Circle,ThreePoints"), this->circleMenu);
	this->circleMenu->addAction(this->circleThreePointAction);

	connect(this->circleCenterRadiusAction, &QAction::triggered, this, &ShDrawMenu::circleCenterRadiusActionClicked);
	connect(this->circleCenterDiameterAction, &QAction::triggered, this, &ShDrawMenu::circleCenterDiameterActionClicked);
	connect(this->circleTwoPointAction, &QAction::triggered, this, &ShDrawMenu::circleTwoPointActionClicked);
	connect(this->circleThreePointAction, &QAction::triggered, this, &ShDrawMenu::circleThreePointActionClicked);

}

void ShDrawMenu::createArcMenu() {

	this->arcMenu = new QMenu(shGetLanValue_ui("Draw/Arc"), this);
	this->addMenu(this->arcMenu);

	this->arcThreePointAction = new QAction(ShIcon(":/Image/Draw/Arc/3Point.png"), 
		shGetLanValue_ui("Draw/Arc,ThreePoints"), this->arcMenu);
	this->arcMenu->addAction(this->arcThreePointAction);


	this->arcMenu->addSeparator();

	this->arcStartCenterEndAction = new QAction(ShIcon(":/Image/Draw/Arc/Start-Center-End.png"), 
		shGetLanValue_ui("Draw/Arc,Start,Center,End"), this->arcMenu);
	this->arcMenu->addAction(this->arcStartCenterEndAction);

	this->arcStartCenterAngleAction = new QAction(ShIcon(":/Image/Draw/Arc/Start-Center-Angle.png"), 
		shGetLanValue_ui("Draw/Arc,Start,Center,Angle"), this->arcMenu);
	this->arcMenu->addAction(this->arcStartCenterAngleAction);

	this->arcStartCenterLengthAction = new QAction(ShIcon(":/Image/Draw/Arc/Start-Center-Length.png"), 
		shGetLanValue_ui("Draw/Arc,Start,Center,Length"), this->arcMenu);
	this->arcMenu->addAction(this->arcStartCenterLengthAction);


	this->arcMenu->addSeparator();

	this->arcStartEndAngleAction = new QAction(ShIcon(":/Image/Draw/Arc/Start-End-Angle.png"), 
		shGetLanValue_ui("Draw/Arc,Start,End,Angle"), this->arcMenu);
	this->arcMenu->addAction(this->arcStartEndAngleAction);

	this->arcStartEndDirectionAction = new QAction(ShIcon(":/Image/Draw/Arc/Start-End-Direction.png"), 
		shGetLanValue_ui("Draw/Arc,Start,End,Direction"), this->arcMenu);
	this->arcMenu->addAction(this->arcStartEndDirectionAction);

	this->arcStartEndRadiusAction = new QAction(ShIcon(":/Image/Draw/Arc/Start-End-Radius.png"), 
		shGetLanValue_ui("Draw/Arc,Start,End,Radius"), this->arcMenu);
	this->arcMenu->addAction(this->arcStartEndRadiusAction);


	this->arcMenu->addSeparator();

	this->arcCenterStartEndAction = new QAction(ShIcon(":/Image/Draw/Arc/Center-Start-End.png"), 
		shGetLanValue_ui("Draw/Arc,Center,Start,End"), this->arcMenu);
	this->arcMenu->addAction(this->arcCenterStartEndAction);

	this->arcCenterStartAngleAction = new QAction(ShIcon(":/Image/Draw/Arc/Center-Start-Angle.png"), 
		shGetLanValue_ui("Draw/Arc,Center,Start,Angle"), this->arcMenu);
	this->arcMenu->addAction(this->arcCenterStartAngleAction);

	this->arcCenterStartLengthAction = new QAction(ShIcon(":/Image/Draw/Arc/Center-Start-Length.png"), 
		shGetLanValue_ui("Draw/Arc,Center,Start,Length"), this->arcMenu);
	this->arcMenu->addAction(this->arcCenterStartLengthAction);

	connect(this->arcThreePointAction, &QAction::triggered, this, &ShDrawMenu::arcThreePointActionClicked);

	connect(this->arcStartCenterEndAction, &QAction::triggered, this, &ShDrawMenu::arcStartCenterEndActionClicked);
	connect(this->arcStartCenterAngleAction, &QAction::triggered, this, &ShDrawMenu::arcStartCenterAngleActionClicked);
	connect(this->arcStartCenterLengthAction, &QAction::triggered, this, &ShDrawMenu::arcStartCenterLengthActionClicked);

	connect(this->arcStartEndAngleAction, &QAction::triggered, this, &ShDrawMenu::arcStartEndAngleActionClicked);
	connect(this->arcStartEndDirectionAction, &QAction::triggered, this, &ShDrawMenu::arcStartEndDirectionActionClicked);
	connect(this->arcStartEndRadiusAction, &QAction::triggered, this, &ShDrawMenu::arcStartEndRadiusActionClicked);

	connect(this->arcCenterStartEndAction, &QAction::triggered, this, &ShDrawMenu::arcCenterStartEndActionClicked);
	connect(this->arcCenterStartAngleAction, &QAction::triggered, this, &ShDrawMenu::arcCenterStartAngleActionClicked);
	connect(this->arcCenterStartLengthAction, &QAction::triggered, this, &ShDrawMenu::arcCenterStartLengthActionClicked);
}

void ShDrawMenu::lineActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawLine);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDrawMenu::circleCenterRadiusActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawCircleCenterRadius);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDrawMenu::circleCenterDiameterActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawCircleCenterDiameter);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDrawMenu::circleTwoPointActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawCircleTwoPoint);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDrawMenu::circleThreePointActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawCircleThreePoint);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDrawMenu::arcThreePointActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawArcThreePoint);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDrawMenu::arcStartCenterEndActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawArcStartCenterEnd);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDrawMenu::arcStartCenterAngleActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawArcStartCenterAngle);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDrawMenu::arcStartCenterLengthActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawArcStartCenterLength);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDrawMenu::arcStartEndAngleActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawArcStartEndAngle);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDrawMenu::arcStartEndDirectionActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawArcStartEndDirection);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDrawMenu::arcStartEndRadiusActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawArcStartEndRadius);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDrawMenu::arcCenterStartEndActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawArcCenterStartEnd);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDrawMenu::arcCenterStartAngleActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawArcCenterStartAngle);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDrawMenu::arcCenterStartLengthActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawArcCenterStartLength);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

/////////////////////////////////////////////////////////////////////////

ShModifyMenu::ShModifyMenu(const QString &title, ShChain *chain, QWidget *parent)
	:ShAbstractMenu(title, chain, parent) {

	this->eraseAction = new QAction(ShIcon(":/Image/Modify/Erase.png"), shGetLanValue_ui("Modify/Erase"), this);
	this->addAction(this->eraseAction);

	this->copyAction = new QAction(ShIcon(":/Image/Modify/Copy.png"), shGetLanValue_ui("Modify/Copy"), this);
	this->addAction(this->copyAction);

	this->mirrorAction = new QAction(ShIcon(":/Image/Modify/Mirror.png"), shGetLanValue_ui("Modify/Mirror"), this);
	this->addAction(this->mirrorAction);

	this->offsetAction = new QAction(ShIcon(":/Image/Modify/Offset.png"), shGetLanValue_ui("Modify/Offset"), this);
	this->addAction(this->offsetAction);

	this->addSeparator();

	this->moveAction = new QAction(ShIcon(":/Image/Modify/Move.png"), shGetLanValue_ui("Modify/Move"), this);
	this->addAction(this->moveAction);

	this->rotateAction = new QAction(ShIcon(":/Image/Modify/Rotate.png"), shGetLanValue_ui("Modify/Rotate"), this);
	this->addAction(this->rotateAction);

	this->scaleAction = new QAction(ShIcon(":/Image/Modify/Scale.png"), shGetLanValue_ui("Modify/Scale"), this);
	this->addAction(this->scaleAction);

	this->stretchAction = new QAction(ShIcon(":/Image/Modify/Stretch.png"), shGetLanValue_ui("Modify/Stretch"), this);
	this->addAction(this->stretchAction);

	this->addSeparator();

	this->trimAction = new QAction(ShIcon(":/Image/Modify/Trim.png"), shGetLanValue_ui("Modify/Trim"), this);
	this->addAction(this->trimAction);

	this->extendAction = new QAction(ShIcon(":/Image/Modify/Extend.png"), shGetLanValue_ui("Modify/Extend"), this);
	this->addAction(this->extendAction);

	connect(this->moveAction, &QAction::triggered, this, &ShModifyMenu::moveActionClicked);
	connect(this->copyAction, &QAction::triggered, this, &ShModifyMenu::copyActionClicked);
	connect(this->mirrorAction, &QAction::triggered, this, &ShModifyMenu::mirrorActionClicked);
	connect(this->offsetAction, &QAction::triggered, this, &ShModifyMenu::offsetActionClicked);
	connect(this->rotateAction, &QAction::triggered, this, &ShModifyMenu::rotateActionClicked);
	connect(this->eraseAction, &QAction::triggered, this, &ShModifyMenu::eraseActionClicked);
	connect(this->extendAction, &QAction::triggered, this, &ShModifyMenu::extendActionClicked);
	connect(this->trimAction, &QAction::triggered, this, &ShModifyMenu::trimActionClicked);
	connect(this->stretchAction, &QAction::triggered, this, &ShModifyMenu::stretchActionClicked);

}

ShModifyMenu::~ShModifyMenu() {

}

void ShModifyMenu::moveActionClicked() {

	ShChangeModifyAfterCancelingCurrentStrategy strategy(ActionType::ActionModifyMove);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShModifyMenu::copyActionClicked() {

	ShChangeModifyAfterCancelingCurrentStrategy strategy(ActionType::ActionModifyCopy);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShModifyMenu::mirrorActionClicked() {

	ShChangeModifyAfterCancelingCurrentStrategy strategy(ActionType::ActionModifyMirror);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShModifyMenu::offsetActionClicked() {

	ShChangeModifyAfterCancelingCurrentStrategy strategy(ActionType::ActionModifyOffset);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShModifyMenu::rotateActionClicked() {

	ShChangeModifyAfterCancelingCurrentStrategy strategy(ActionType::ActionModifyRotate);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShModifyMenu::eraseActionClicked() {

	ShChangeModifyAfterCancelingCurrentStrategy strategy(ActionType::ActionModifyErase);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShModifyMenu::extendActionClicked() {

	ShChangeModifyAfterCancelingCurrentStrategy strategy(ActionType::ActionModifyExtend);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShModifyMenu::trimActionClicked() {

	ShChangeModifyAfterCancelingCurrentStrategy strategy(ActionType::ActionModifyTrim);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShModifyMenu::stretchActionClicked() {

	ShChangeModifyAfterCancelingCurrentStrategy strategy(ActionType::ActionModifyStretch);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

////////////////////////////////////////////////////////////////

ShWindowMenu::ShWindowMenu(const QString &title, ShChain *chain, QWidget *parent)
	:ShAbstractMenu(title, chain, parent) {

	this->tabbedViewAction = new QAction(shGetLanValue_ui("Window/TabbedView"), this);
	this->addAction(this->tabbedViewAction);

	this->subWindowViewAction = new QAction(shGetLanValue_ui("Window/SubWindowView"), this);
	this->addAction(this->subWindowViewAction);

	this->cascadeAction = new QAction(shGetLanValue_ui("Window/Cascade"), this);
	this->addAction(this->cascadeAction);

	this->tileAction = new QAction(shGetLanValue_ui("Window/Tile"), this);
	this->addAction(this->tileAction);

	connect(this->tabbedViewAction, &QAction::triggered, this, &ShWindowMenu::tabbedViewActionClicked);
	connect(this->subWindowViewAction, &QAction::triggered, this, &ShWindowMenu::subWindowViewActionClicked);
	connect(this->cascadeAction, &QAction::triggered, this, &ShWindowMenu::cascadeActionClicked);
	connect(this->tileAction, &QAction::triggered, this, &ShWindowMenu::tileActionClicked);

}

ShWindowMenu::~ShWindowMenu() {

}

void ShWindowMenu::tabbedViewActionClicked() {

	ShRequestChangeViewMode request(ShRequestChangeViewMode::ViewMode::TabbedView);
	this->request(&request);
}

void ShWindowMenu::subWindowViewActionClicked() {

	ShRequestChangeViewMode request(ShRequestChangeViewMode::ViewMode::SubWindowView);
	this->request(&request);
}

void ShWindowMenu::cascadeActionClicked() {

	ShRequestChangeViewMode request(ShRequestChangeViewMode::ViewMode::Cascade);
	this->request(&request);
}

void ShWindowMenu::tileActionClicked() {

	ShRequestChangeViewMode request(ShRequestChangeViewMode::ViewMode::Tile);
	this->request(&request);
}


/////////////////////////////////////////////////////////////////////

ShDimMenu::ShDimMenu(const QString &title, ShChain *chain, QWidget *parent)
	:ShAbstractMenu(title, chain, parent) {

	this->dimLinearAcion = new QAction(ShIcon(":/Image/Dimension/DimLinear.png"), shGetLanValue_ui("Dim/Linear"), this);
	this->addAction(this->dimLinearAcion);

	this->dimAlignedAction = new QAction(ShIcon(":/Image/Dimension/DimAligned.png"), shGetLanValue_ui("Dim/Aligned"), this);
	this->addAction(this->dimAlignedAction);

	this->dimAngularAction = new QAction(ShIcon(":/Image/Dimension/DimAngular.png"), shGetLanValue_ui("Dim/Angular"), this);
	this->addAction(this->dimAngularAction);

	this->dimRadiusAction = new QAction(ShIcon(":/Image/Dimension/DimRadius.png"), shGetLanValue_ui("Dim/Radius"), this);
	this->addAction(this->dimRadiusAction);

	this->dimDiameterAction = new QAction(ShIcon(":/Image/Dimension/DimDiameter.png"), shGetLanValue_ui("Dim/Diameter"), this);
	this->addAction(this->dimDiameterAction);

	this->dimArcLengthAction = new QAction(ShIcon(":/Image/Dimension/DimArcLength.png"), shGetLanValue_ui("Dim/ArcLength"), this);
	this->addAction(this->dimArcLengthAction);

	connect(this->dimLinearAcion, &QAction::triggered, this, &ShDimMenu::dimLinearActionClicked);
	connect(this->dimAlignedAction, &QAction::triggered, this, &ShDimMenu::dimAlignedActionClicked);
	connect(this->dimRadiusAction, &QAction::triggered, this, &ShDimMenu::dimRadiusActionClicked);
	connect(this->dimDiameterAction, &QAction::triggered, this, &ShDimMenu::dimDiameterActionClicked);
	connect(this->dimArcLengthAction, &QAction::triggered, this, &ShDimMenu::dimArcLengthActionClicked);
	connect(this->dimAngularAction, &QAction::triggered, this, &ShDimMenu::dimAngularActionClicked);
}

ShDimMenu::~ShDimMenu() {

}

void ShDimMenu::dimLinearActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawDimLinear);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDimMenu::dimAlignedActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawDimAligned);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDimMenu::dimAngularActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawDimAngular);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDimMenu::dimRadiusActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawDimRadius);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDimMenu::dimDiameterActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawDimDiameter);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}

void ShDimMenu::dimArcLengthActionClicked() {

	ShChangeActionAfterCancelingCurrentStrategy strategy(ActionType::ActionDrawDimArcLength);
	ShRequestChangeActionHandler request(&strategy);
	this->request(&request);
}