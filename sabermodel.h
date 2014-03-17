// The sample model.  You should build a file
// very similar to this for when you make your model.
#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include <FL/gl.h>
#include <cmath>
#include <string>
using namespace std;

#include "modelerglobals.h"

#define USE_COLOR_BLUE 135.0/256.0, 196.0/256.0, 250.0/256.0
#define USE_COLOR_SILVER 0.851f, 0.851f, 0.851f

const double esp = 1e-6;
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
	GLdouble colorAlpha;
	GLdouble transX, transY, transZ;
	string rotateOrder;
public:
	ModelNode();
	void nodeCreate(ModelNode *father, int thePrimitiveType);
	void setAngle(GLdouble XAngle, GLdouble YAngle, GLdouble ZAngle, char theRotateOrder[] = "xyz");
	void setStartPos(GLdouble X, GLdouble Y, GLdouble Z);
	void setScale(GLdouble X, GLdouble Y, GLdouble Z);
	void setColor(GLdouble r, GLdouble g, GLdouble b);
	void setColorAlpha(GLdouble r, GLdouble g, GLdouble b,GLdouble alpha);
	void setTrans(GLdouble X, GLdouble Y, GLdouble Z);
	void cylinderUpperScale(GLdouble theUpperScale);
	void Render();
};

// To make a SaberModel, we inherit off of ModelerView
class SaberModel : public ModelerView
{
public:
	SaberModel(int x, int y, int w, int h, char *label)
		: ModelerView(x, y, w, h, label)
		{
		InitializeTree();
	}
	void InitializeTree();
	virtual void draw();
	void LocateBody(GLdouble xPos, GLdouble yPos, GLdouble zPos, GLdouble rotateAngle);
	void RotateLeftUpperArm(GLdouble X, GLdouble Y, GLdouble Z, char theRotateOrder[] = "xyz");
	void RotateLeftLowerArm(GLdouble X, GLdouble Y, GLdouble Z, char theRotateOrder[] = "xyz");
	void RotateRightUpperArm(GLdouble X, GLdouble Y, GLdouble Z, char theRotateOrder[] = "xyz");
	void RotateRightLowerArm(GLdouble X, GLdouble Y, GLdouble Z, char theRotateOrder[] = "xyz");
	void RotateExcalibur(GLdouble X, GLdouble Y, GLdouble Z, char theRotateOrder[] = "xyz");
	void setExcaliburTransparency(GLdouble alpha);
private:
	ModelNode *treeRoot;
	ModelNode upperTorso, lowerTorso, leftUpperArm, leftLowerArm, rightUpperArm, rightLowerArm;
	ModelNode excaliburGrip, excaliburGuard, excaliburBlade, head, leftUpperLeg, leftLowerLeg, rightUpperLeg, rightLowerLeg;
};