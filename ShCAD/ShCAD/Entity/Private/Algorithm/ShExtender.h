

#ifndef _SHEXTENDER_H
#define _SHEXTENDER_H

#include "ShVisitor.h"
#include "Data\ShPoint3d.h"
#include <qlinkedlist.h>

class ShEntity;

enum ShEntityPartToExtend {
	Start,
	End,
};


class ShEntityPartToExtendFinder : public ShVisitor {

	friend class ShExtender;

private:
	ShEntityPartToExtend &entityPartToExtend;
	ShPoint3d &pointToExtend;
	const ShPoint3d clickPoint;

private:
	ShEntityPartToExtendFinder(ShEntityPartToExtend &entityPartToExtend, ShPoint3d &pointToExtend, const ShPoint3d &clickPoint);
	~ShEntityPartToExtendFinder();

	virtual void visit(ShLine *line);
	virtual void visit(ShArc *arc);
	//virtual void visit(ShEllipse* ellipse);

};

//////////////////////////////////////////////////////


class ShExtender : public ShVisitor {

private:
	QLinkedList<ShEntity*> baseEntities;
	const ShPoint3d clickPoint;

private:
	ShEntity* *extendedEntity;
	bool &valid;

public:
	ShExtender(const QLinkedList<ShEntity*> &baseEntities, const ShPoint3d &clickPoint, ShEntity* *extendedEntity, bool &valid);
	~ShExtender();

	virtual void visit(ShLine *line);
	virtual void visit(ShCircle *circle);
	virtual void visit(ShArc *arc);
	//virtual void visit(ShEllipse* ellipse); // �����Բ֧��

};


/////////////////////////////////////////////////////////


class ShExtensionPointFinder : public ShVisitor {

protected:
	QLinkedList<ShPoint3d> &extensionPointList;

protected:
	ShExtensionPointFinder(QLinkedList<ShPoint3d> &extensionPointList);
	virtual ~ShExtensionPointFinder() = 0;
	
};


//////////////////////////////////////////////////////////



class ShLineExtensionPointFinder : public ShExtensionPointFinder {

	friend class ShExtender;

private:
	ShLine *lineToExtend;
	ShEntityPartToExtend entityPartToExtend;

private:
	ShLineExtensionPointFinder(QLinkedList<ShPoint3d> &extensionPointList, ShLine *lineToExtend, ShEntityPartToExtend entityPartToExtend);
	~ShLineExtensionPointFinder();

	virtual void visit(ShLine *line);
	virtual void visit(ShCircle *circle);
	virtual void visit(ShArc *arc);
	virtual void visit(ShDimLinear *dimLinear);
	virtual void visit(ShDimAligned *dimAligned);
	virtual void visit(ShDimRadius *dimRadius);
	virtual void visit(ShDimDiameter *dimDiameter);
	virtual void visit(ShDimArcLength *dimArcLength);
	virtual void visit(ShDimAngular *dimAngular);
	virtual void visit(ShConstructionLine *constructionLine);
	//virtual void visit(ShEllipse* ellipse); // �����Բ֧��

	bool checkPossibleToExtend(ShLine *lineToExtend, ShEntityPartToExtend entityPartToExtend, const ShPoint3d &extensionPoint);
	bool checkPossibleToExtend(ShLine *lineToExtend, ShEntityPartToExtend entityPartToExtend,
		const ShPoint3d &extensionPoint, const ShPoint3d &extensionPoint2, ShPoint3d &finalExtensionPoint);
};

///////////////////////////////////////////////////////////


class ShArcExtensionPointFinder : public ShExtensionPointFinder {

	friend class ShExtender;

private:
	ShArc *arcToExtend;
	ShEntityPartToExtend entityPartToExtend;

private:
	ShArcExtensionPointFinder(QLinkedList<ShPoint3d> &extensionPointList, ShArc *arcToExtend, ShEntityPartToExtend entityPartToExtend);
	~ShArcExtensionPointFinder();

	virtual void visit(ShLine *line);
	virtual void visit(ShCircle *circle);
	virtual void visit(ShArc *arc);
	virtual void visit(ShDimLinear *dimLinear);
	virtual void visit(ShDimAligned *dimAligned);
	virtual void visit(ShDimRadius *dimRadius);
	virtual void visit(ShDimDiameter *dimDiameter);
	virtual void visit(ShDimArcLength *dimArcLength);
	virtual void visit(ShDimAngular *dimAngular);
	virtual void visit(ShConstructionLine *constructionLine);
	//virtual void visit(ShEllipse* ellipse); // �����Բ֧��

	bool checkPossibleToExtend(ShArc *arcToExtend, ShEntityPartToExtend entityPartToExtend, const ShPoint3d &extensionPoint);
	bool checkPossibleToExtend(ShArc *arcToExtend, ShEntityPartToExtend entityPartToExtend,
		const ShPoint3d &extensionPoint, const ShPoint3d &extensionPoint2, ShPoint3d &finalExtensionPoint);

};

#endif //_SHEXETENDER_H