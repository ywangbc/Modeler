// The sample model.  You should build a file
// very similar to this for when you make your model.
#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include <FL/gl.h>

#include "modelerglobals.h"

enum{
	PRIMITIVE_BOX = 1,
	PRIMITIVE_SPHERE,
	PRIMITIVE_CYLINDER,
	PRIMITIVE_TRIANGLE,
	PRIMITIVE_TYPES
};

class ModelNode{
private:
	ModelNode *childHead, *brotherNext;
	int primitiveType;
	GLdouble xAngle, yAngle, zAngle, startPosX, startPosY, startPosZ, xScale, yScale, zScale;
	GLdouble upperScale;
	GLdouble colorRed, colorGreen, colorBlue;
	GLdouble transX, transY, transZ;
public:
	ModelNode(ModelNode *father, int thePrimitiveType);
	void setAngle(GLdouble XAngle, GLdouble YAngle, GLdouble ZAngle);
	void setStartPos(GLdouble X, GLdouble Y, GLdouble Z);
	void setScale(GLdouble X, GLdouble Y, GLdouble Z);
	void setColor(GLdouble r, GLdouble g, GLdouble b);
	void setTrans(GLdouble X, GLdouble Y, GLdouble Z);
	void cylinderUpperScale(GLdouble theUpperScale);
	void Render();
};

// To make a SaberModel, we inherit off of ModelerView
class SaberModel : public ModelerView
{
public:
	SaberModel(int x, int y, int w, int h, char *label)
		: ModelerView(x, y, w, h, label),
		upperTorso(NULL, PRIMITIVE_BOX),
		lowerTorso(&upperTorso, PRIMITIVE_CYLINDER),
		leftUpperArm(&upperTorso, PRIMITIVE_BOX),
		leftLowerArm(&leftUpperArm, PRIMITIVE_BOX),
		rightUpperArm(&upperTorso, PRIMITIVE_BOX),
		rightLowerArm(&rightUpperArm, PRIMITIVE_BOX),
		excaliburGrip(&rightLowerArm, PRIMITIVE_BOX),
		excaliburGuard(&excaliburGrip, PRIMITIVE_BOX),
		excaliburBlade(&excaliburGuard, PRIMITIVE_BOX),
		head(&upperTorso, PRIMITIVE_BOX),
		leftUpperLeg(&upperTorso, PRIMITIVE_BOX),
		leftLowerLeg(&leftUpperLeg, PRIMITIVE_BOX),
		rightUpperLeg(&upperTorso, PRIMITIVE_BOX),
		rightLowerLeg(&rightUpperLeg, PRIMITIVE_BOX),
		treeRoot(&upperTorso){
		InitializeTree();
	}
	void InitializeTree();
	virtual void draw();
	void RotateLeftUpperArm(GLdouble X, GLdouble Y, GLdouble Z);
	void RotateLeftLowerArm(GLdouble X, GLdouble Y, GLdouble Z);
	void RotateRightUpperArm(GLdouble X, GLdouble Y, GLdouble Z);
	void RotateRightLowerArm(GLdouble X, GLdouble Y, GLdouble Z);
	void RotateExcalibur(GLdouble X, GLdouble Y, GLdouble Z);
private:
	ModelNode *treeRoot;
	ModelNode upperTorso, lowerTorso, leftUpperArm, leftLowerArm, rightUpperArm, rightLowerArm;
	ModelNode excaliburGrip, excaliburGuard, excaliburBlade, head, leftUpperLeg, leftLowerLeg, rightUpperLeg, rightLowerLeg;
};