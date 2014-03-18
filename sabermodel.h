// The sample model.  You should build a file
// very similar to this for when you make your model.
#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include <FL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <string>
using namespace std;

#include "modelerglobals.h"

#define USE_COLOR_BLUE 54.0/255.0, 86.0/255.0, 128.0/255.0
#define USE_COLOR_SILVER 238.0/255.0, 238.0 / 255.0, 236.0/255.0
#define USE_COLOR_GOLD 252.0/255.0,247.0/255.0,135.0/255.0
#define USE_COLOR_EXCALIBUR 254.0/255.0,245.0/255.0,226.0/255.0
#define USE_COLOR_DARK 50.0/255.0,56.0/255.0,73.0/255.0
#define USE_COLOR_BLACK 12.0/255.0,11.0/255.0,18.0/255.0
#define USE_COLOR_RED 234.0/255.0,65.0/255.0,43.0/255.0
#define USE_COLOR_WHITE 252.0/255.0,248.0/255.0,246.0/255.0

const double esp = 1e-6;
enum{
	PRIMITIVE_BOX = 1,
	PRIMITIVE_SPHERE,
	PRIMITIVE_CYLINDER,
	PRIMITIVE_CYLINDER_NO_DISK,
	SHAPE_TORSO,
	SHAPE_TORSO_LINEAR,
	PRIMITIVE_TRIANGLE,
	SHAPE_BLADE,
	PRIMITIVE_TYPES
};

enum{
	TYPE_EXCALIBUR = 1,
	TYPE_CALIBURN,
	TYPE_EXCALIBUR_MORGAN,
	TYPE_SWORDS
};

enum{
	COSTUME_SABER = 1,
	COSTUME_SABER_ALTER,
	COSTUME_SABER_LILY
};

const GLdouble LOWER_ARM_SLASH=0.8;

class ModelNode{
private:
	ModelNode *childHead, *brotherNext;
	int primitiveType;
	int swordType;
	GLdouble xAngle, yAngle, zAngle, startPosX, startPosY, startPosZ, xScale, yScale, zScale;
	GLdouble upperScale,middleScale,middleRatio;
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
	void setSwordType(int ty);
	void setTrans(GLdouble X, GLdouble Y, GLdouble Z);
	void cylinderScale(GLdouble theUpperScale, GLdouble theMiddleScale, GLdouble theMiddleRatio);
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
		CostumeSaber();
	}
	void InitializeTree();
	void CostumeSaber();
	void CostumeSaberAlter();
	void CostumeSaberLily();
	void ChooseCostume(int cost);
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
	ModelNode leftShoulder, rightShoulder;
};