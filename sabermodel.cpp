#include "sabermodel.h"

void setDiffuseColorAlpha(float r, float g, float b, float alpha)
{
	ModelerDrawState *mds = ModelerDrawState::Instance();

	mds->m_diffuseColor[0] = (GLfloat)r;
	mds->m_diffuseColor[1] = (GLfloat)g;
	mds->m_diffuseColor[2] = (GLfloat)b;
	mds->m_diffuseColor[3] = (GLfloat)alpha;

	if (mds->m_drawMode == NORMAL)
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mds->m_diffuseColor);
	else
		glColor3f(r, g, b);
}

ModelNode::ModelNode(){
	childHead = brotherNext = NULL;
}
void ModelNode::nodeCreate(ModelNode *father, int thePrimitiveType){
	primitiveType = thePrimitiveType;
	if (father){
		if (father->childHead)brotherNext = father->childHead;
		father->childHead = this;
	}
}

void ModelNode::setAngle(GLdouble XAngle, GLdouble YAngle, GLdouble ZAngle, char theRotateOrder[]){
	xAngle = XAngle;
	yAngle = YAngle;
	zAngle = ZAngle;
	rotateOrder = string(theRotateOrder);
}
void ModelNode::setStartPos(GLdouble X, GLdouble Y, GLdouble Z){
	startPosX = X;
	startPosY = Y;
	startPosZ = Z;
}
void ModelNode::setScale(GLdouble X, GLdouble Y, GLdouble Z){
	xScale = X;
	yScale = Y;
	zScale = Z;
}
void ModelNode::setColor(GLdouble r, GLdouble g, GLdouble b){
	colorRed = r;
	colorGreen = g;
	colorBlue = b;
	colorAlpha = 1.0;
}

void ModelNode::setColorAlpha(GLdouble r, GLdouble g, GLdouble b, GLdouble alpha){
	colorRed = r;
	colorGreen = g;
	colorBlue = b;
	colorAlpha = alpha;
}

void ModelNode::setTrans(GLdouble X, GLdouble Y, GLdouble Z){
	transX = X;
	transY = Y;
	transZ = Z;
}

void ModelNode::cylinderUpperScale(GLdouble theUpperScale){
	upperScale = theUpperScale;
}

void ModelNode::Render(){
	setAmbientColor(.1f, .1f, .1f);
	if (abs(colorAlpha - 1.0) < esp){
		setDiffuseColor(colorRed, colorGreen, colorBlue);//set color
	}
	else {
		setDiffuseColorAlpha(colorRed, colorGreen, colorBlue, colorAlpha);
	}
	glPushMatrix();
	glTranslated(startPosX, startPosY, startPosZ);
	for (int i = 2; i>=0 ; i--){
		if (rotateOrder[i] == 'x')glRotated(xAngle, 1.0, 0.0, 0.0); 
		if (rotateOrder[i] == 'y')glRotated(yAngle, 0.0, 1.0, 0.0);
		if (rotateOrder[i] == 'z')glRotated(zAngle, 0.0, 0.0, 1.0);
	}
	glPushMatrix();
	if (primitiveType == PRIMITIVE_CYLINDER)glRotated(90.0, 1.0, 0.0, 0.0);
	glTranslated(transX, transY, transZ);//for this primitive only
	glScaled(xScale, yScale, zScale);//for this primitive only
	switch (primitiveType){
	case PRIMITIVE_BOX:
		drawBox(1.0, 1.0, 1.0);
		break;
	case PRIMITIVE_SPHERE:
		drawSphere(1.0);
		break;
	case PRIMITIVE_CYLINDER:
		drawCylinder(1.0, 1.0, upperScale);
		break;
	case PRIMITIVE_TRIANGLE:
		//not used yet
		break;
	}
	glPopMatrix();
	//recursive draw children
	for (ModelNode* mn = childHead; mn; mn = mn->brotherNext){
		mn->Render();
	}
	glPopMatrix();
}





//Manually initialize tree nodes
void SaberModel::InitializeTree(){
	upperTorso.nodeCreate(NULL, PRIMITIVE_CYLINDER);
	rightUpperArm.nodeCreate(&upperTorso, PRIMITIVE_BOX);
	rightLowerArm.nodeCreate(&rightUpperArm, PRIMITIVE_BOX);
	leftUpperArm.nodeCreate(&upperTorso, PRIMITIVE_BOX);
	leftLowerArm.nodeCreate(&leftUpperArm, PRIMITIVE_BOX);
	excaliburGrip.nodeCreate(&rightLowerArm, PRIMITIVE_BOX);
	excaliburGuard.nodeCreate(&excaliburGrip, PRIMITIVE_BOX);
	excaliburBlade.nodeCreate(&excaliburGuard, PRIMITIVE_BOX);
	lowerTorso.nodeCreate(&upperTorso, PRIMITIVE_CYLINDER);
	head.nodeCreate(&upperTorso, PRIMITIVE_SPHERE);
	leftUpperLeg.nodeCreate(&upperTorso, PRIMITIVE_BOX);
	leftLowerLeg.nodeCreate(&leftUpperLeg, PRIMITIVE_BOX);
	rightUpperLeg.nodeCreate(&upperTorso, PRIMITIVE_BOX);
	rightLowerLeg.nodeCreate(&rightUpperLeg, PRIMITIVE_BOX);
	treeRoot = &upperTorso;

	upperTorso.setAngle(0.0, 0.0, 0.0);
	upperTorso.setColor(USE_COLOR_SILVER);
	upperTorso.setScale(1.3, 0.8, 4.0);
	upperTorso.setStartPos(0.0, 0.0, 0.0);
	upperTorso.setTrans(0.0, 0, 0.0);//center at neck
	upperTorso.cylinderUpperScale(1.0);

	lowerTorso.setAngle(0.0, 0.0, 0.0);
	lowerTorso.setColor(USE_COLOR_BLUE);
	lowerTorso.setScale(1.3, -0.8, 2.0);
	lowerTorso.setStartPos(0.0, -4.0, 0.0);
	lowerTorso.setTrans(0.0, 0.0, 0.0);//center at weist
	lowerTorso.cylinderUpperScale(2.0);

	leftUpperArm.setAngle(0.0, 0.0, 0.0);
	leftUpperArm.setColor(USE_COLOR_BLUE);
	leftUpperArm.setScale(1.0, -2.0, 1.0);
	leftUpperArm.setStartPos(-1.7, 0.0, 0.0);
	leftUpperArm.setTrans(-0.5, 0.0, -0.5);//center at shoulder

	leftLowerArm.setAngle(0.0, 0.0, 0.0);
	leftLowerArm.setColor(USE_COLOR_SILVER);
	leftLowerArm.setScale(1.0, -2.0, 1.0);
	leftLowerArm.setStartPos(0.0, -2.2, 0.0);
	leftLowerArm.setTrans(-0.5, 0.0, -0.5);

	rightUpperArm.setAngle(0.0, 0.0, 0.0);
	rightUpperArm.setColor(USE_COLOR_BLUE);
	rightUpperArm.setScale(1.0, -2.0, 1.0);
	rightUpperArm.setStartPos(1.7, 0.0, 0.0);
	rightUpperArm.setTrans(-0.5, 0.0, -0.5);//center at shoulder

	rightLowerArm.setAngle(0.0, 0.0, 0.0);
	rightLowerArm.setColor(USE_COLOR_SILVER);
	rightLowerArm.setScale(1.0, -2.0, 1.0);
	rightLowerArm.setStartPos(0.0, -2.2, 0.0);
	rightLowerArm.setTrans(-0.5, 0.0, -0.5);

	excaliburGuard.setAngle(0.0, 0.0, 0.0);
	excaliburGuard.setColor(USE_COLOR_BLUE);
	excaliburGuard.setScale(2.0, 0.5, 0.4);
	excaliburGuard.setStartPos(0.0, -0.25, 0.0);
	excaliburGuard.setTrans(-1.0, -0.25, -0.2);

	excaliburBlade.setAngle(0.0, 0.0, 0.0);
	excaliburBlade.setColor(USE_COLOR_SILVER);
	excaliburBlade.setScale(0.8, -3.0, 0.1);
	excaliburBlade.setStartPos(0.0, -0.25, 0.0);
	excaliburBlade.setTrans(-0.5, 0.0, -0.05);

	excaliburGrip.setAngle(0.0, 0.0, 0.0);
	excaliburGrip.setColor(USE_COLOR_BLUE);
	excaliburGrip.setScale(0.4, 1.0, 0.4);
	excaliburGrip.setStartPos(0.0, -2.5, 0.0);
	excaliburGrip.setTrans(-0.2, 0.0, -0.2);

	head.setAngle(0.0, 0.0, 0.0);
	head.setColor(1.0f, 0.8f, 0.7f);
	head.setScale(1.0, 1.0, 1.0);
	head.setStartPos(0.0, 0.7, 0.0);
	head.setTrans(0.0, 0.0, 0.0);

	leftUpperLeg.setAngle(0.0, 0.0, 0.0);
	leftUpperLeg.setColor(USE_COLOR_BLUE);
	leftUpperLeg.setScale(1.0, -2.0, 1.0);
	leftUpperLeg.setStartPos(-0.7, -4.2, 0.0);
	leftUpperLeg.setTrans(-0.5, 0.0, -0.5);

	rightUpperLeg.setAngle(0.0, 0.0, 0.0);
	rightUpperLeg.setColor(USE_COLOR_BLUE);
	rightUpperLeg.setScale(1.0, -2.0, 1.0);
	rightUpperLeg.setStartPos(0.7, -4.2, 0.0);
	rightUpperLeg.setTrans(-0.5, 0.0, -0.5);

	leftLowerLeg.setAngle(0.0, 0.0, 0.0);
	leftLowerLeg.setColor(USE_COLOR_SILVER);
	leftLowerLeg.setScale(1.0, -2.0, 1.0);
	leftLowerLeg.setStartPos(0.0, -2.2, 0.0);
	leftLowerLeg.setTrans(-0.5, 0.0, -0.5);

	rightLowerLeg.setAngle(0.0, 0.0, 0.0);
	rightLowerLeg.setColor(USE_COLOR_SILVER);
	rightLowerLeg.setScale(1.0, -2.0, 1.0);
	rightLowerLeg.setStartPos(0.0, -2.2, 0.0);
	rightLowerLeg.setTrans(-0.5, 0.0, -0.5);
}

void SaberModel::LocateBody(GLdouble xPos, GLdouble yPos, GLdouble zPos, GLdouble rotateAngle){
	upperTorso.setStartPos(xPos, yPos, zPos);
	upperTorso.setAngle(0.0, rotateAngle, 0.0);
}

void SaberModel::RotateLeftUpperArm(GLdouble X, GLdouble Y, GLdouble Z, char theRotateOrder[]){
	leftUpperArm.setAngle(X, Y, Z, theRotateOrder);
}
void SaberModel::RotateLeftLowerArm(GLdouble X, GLdouble Y, GLdouble Z, char theRotateOrder[]){
	leftLowerArm.setAngle(X, Y, Z, theRotateOrder);
}
void SaberModel::RotateRightUpperArm(GLdouble X, GLdouble Y, GLdouble Z, char theRotateOrder[]){
	rightUpperArm.setAngle(X, Y, Z, theRotateOrder);
}
void SaberModel::RotateRightLowerArm(GLdouble X, GLdouble Y, GLdouble Z, char theRotateOrder[]){
	rightLowerArm.setAngle(X, Y, Z, theRotateOrder);
}

void SaberModel::RotateExcalibur(GLdouble X, GLdouble Y, GLdouble Z, char theRotateOrder[]){
	excaliburGrip.setAngle(X, Y, Z, theRotateOrder);
}

void SaberModel::setExcaliburTransparency(GLdouble alpha){
	excaliburBlade.setColorAlpha(1.0f, 1.0f, 1.0f, alpha);
}