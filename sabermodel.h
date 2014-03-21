// The sample model.  You should build a file
// very similar to this for when you make your model.
#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include "bitmap.h"
#include <FL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <string>
#include <ctime>
using namespace std;

#include "modelerglobals.h"

#define USE_COLOR_BLUE 37.0/255.0, 69.0/255.0, 130.0/255.0
#define USE_COLOR_LIGHT_BLUE 71.0/255.0,66.0/255.0,157.0/255.0
#define USE_COLOR_SILVER 236.0/255.0, 236.0 / 255.0, 234.0/255.0
#define USE_COLOR_GOLD 252.0/255.0,247.0/255.0,135.0/255.0
#define USE_COLOR_EXCALIBUR 254.0/255.0,245.0/255.0,226.0/255.0
#define USE_COLOR_DARK 50.0/255.0,56.0/255.0,73.0/255.0
#define USE_COLOR_BLACK 12.0/255.0,11.0/255.0,18.0/255.0
#define USE_COLOR_BLACK_CLOTHES 46.0/255.0, 43.0/255.0, 30.0/255.0
#define USE_COLOR_DARK_CLOTHES 52.0/255.0,44.0/255.0,65.0/255.0
#define USE_COLOR_DARK_ARMOR 83.0/255.0, 89.0/255.0, 124.0/255.0
#define USE_COLOR_RED 234.0/255.0,65.0/255.0,43.0/255.0
#define USE_COLOR_WHITE 254.0/255.0,254.0/255.0,254.0/255.0
#define USE_COLOR_BODY 255.0/255.0, 238.0/255.0, 212.0/255.0
#define USE_COLOR_DARK_GOLD 211.0/255.0,197.0/255.0,162.0/255.0
#define USE_COLOR_HAIR_GOLD 255.0/255.0,252.0/255.0,149.0/255.0
#define USE_COLOR_HAIR_DARK_GOLD 243.0/255.0, 233.0/255.0, 210.0/255.0
#define USE_COLOR_EYE_GREEN 117.0/255.0, 150.0/255.0, 142.0/255.0
#define USE_COLOR_EYE_DARK_GREEN 26.0 / 255.0, 63.0 / 255.0, 82.0 / 255.0
#define USE_COLOR_EYE_YELLOW 242.0/255.0, 240.0/255.0, 200.0/255.0
#define USE_COLOR_EYE_DARK_YELLOW 156 / 255.0, 126 / 255.0, 71 / 255.0
#define USE_COLOR_PALE 244.0/255.0, 236.0/255.0, 246.0/255.0

const double esp = 1e-6;
const int STEPMAX = 50;
enum{
	PRIMITIVE_BOX = 1,
	PRIMITIVE_SPHERE,
	PRIMITIVE_CYLINDER,
	PRIMITIVE_CYLINDER_NO_DISK,
	SHAPE_PARTIAL_CYLINDER,
	SHAPE_TORSO,
	SHAPE_TORSO_LINEAR,
	SHAPE_TORSO_HALF_LINEAR,
	SHAPE_PARTIAL_TORSO,
	PRIMITIVE_TRIANGLE,
	SHAPE_BLADE,
	SHAPE_FOOT,
	SHAPE_HEAD,
	SHAPE_GUARD,
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

enum{
	VERTICAL = 1,
	STAND,
	SKEW,
	SINGLE,
	SLASH_TYPE_NUM
};

const GLdouble LOWER_ARM_SLASH=0.8;

class ModelNode{
private:
	ModelNode *childHead, *brotherNext;
	int primitiveType;
	int swordType;
	int headType;
	int haveTorus;
	GLdouble xAngle, yAngle, zAngle, startPosX, startPosY, startPosZ, xScale, yScale, zScale;
	GLdouble upperScale,middleScale,middleRatio;
	GLdouble colorRed, colorGreen, colorBlue;
	GLdouble colorAlpha;
	GLdouble transX, transY, transZ;
	GLdouble startAngle, endAngle;
	bool disabled;
	bool texAvailable;
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
	void setHeadType(int ty);
	void setTorus(int t);
	void setTrans(GLdouble X, GLdouble Y, GLdouble Z);
	void cylinderScale(GLdouble theUpperScale, GLdouble theMiddleScale, GLdouble theMiddleRatio);
	void setStartAndEndAngle(GLdouble theStartAngle, GLdouble theEndAngle);
	void enableNode();
	void disableNode();
	void Render();
	void enableTexture();
	void disableTexture();
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
	void setComplexity(int comp);
private:
	ModelNode *treeRoot;
	ModelNode upperTorso, lowerTorso, leftUpperArm, leftLowerArm, rightUpperArm, rightLowerArm;
	ModelNode excaliburGrip, excaliburGuard, excaliburBlade, head, leftUpperLeg, leftLowerLeg, rightUpperLeg, rightLowerLeg;
	ModelNode leftShoulder, rightShoulder;
	ModelNode lowerArmor[6],lowerFront[2],lowerOuter[6];
	ModelNode leftFoot, rightFoot;
	ModelNode leftHand, rightHand;
};