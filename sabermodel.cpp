#include "sabermodel.h"


ModelNode::ModelNode(ModelNode *father, int thePrimitiveType){
	primitiveType = thePrimitiveType;
	childHead = brotherNext = NULL;
	if (father){
		if (father->childHead)brotherNext = father->childHead;
		father->childHead = this;
	}
}

void ModelNode::setAngle(GLdouble XAngle, GLdouble YAngle, GLdouble ZAngle){
	xAngle = XAngle;
	yAngle = YAngle;
	zAngle = ZAngle;
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
	setDiffuseColor(colorRed, colorGreen, colorBlue);//set color
	glPushMatrix();
	glTranslated(startPosX, startPosY, startPosZ);
	glRotated(xAngle, 1.0, 0.0, 0.0);
	glRotated(yAngle, 0.0, 1.0, 0.0);
	glRotated(zAngle, 0.0, 0.0, 1.0);
	glPushMatrix();
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

	upperTorso.setAngle(0.0, 0.0, 0.0);
	upperTorso.setColor(0.0f, 0.0f, 1.0f);
	upperTorso.setScale(2.0, -4.0, 1.0);
	upperTorso.setStartPos(0.0, 0.0, 0.0);
	upperTorso.setTrans(-1.0, 0, -0.5);//center at neck

	lowerTorso.setAngle(90.0, 0.0, 0.0);
	lowerTorso.setColor(1.0f, 1.0f, 1.0f);
	lowerTorso.setScale(1.3, -1.3, 2.0);
	lowerTorso.setStartPos(0.0, -4.0, 0.0);
	lowerTorso.setTrans(0.0, 0.0, 0.0);//center at weist
	lowerTorso.cylinderUpperScale(1.5);

	leftUpperArm.setAngle(0.0, 0.0, 0.0);
	leftUpperArm.setColor(0.0f, 1.0f, 0.0f);
	leftUpperArm.setScale(1.0, -2.0, 1.0);
	leftUpperArm.setStartPos(-1.7, 0.0, 0.0);
	leftUpperArm.setTrans(-0.5, 0.0, -0.5);//center at shoulder

	leftLowerArm.setAngle(0.0, 0.0, 0.0);
	leftLowerArm.setColor(1.0f, 0.0f, 0.0f);
	leftLowerArm.setScale(1.0, -2.0, 1.0);
	leftLowerArm.setStartPos(0.0, -2.2, 0.0);
	leftLowerArm.setTrans(-0.5, 0.0, -0.5);

	rightUpperArm.setAngle(0.0, 0.0, 0.0);
	rightUpperArm.setColor(0.0f, 1.0f, 0.0f);
	rightUpperArm.setScale(1.0, -2.0, 1.0);
	rightUpperArm.setStartPos(1.7, 0.0, 0.0);
	rightUpperArm.setTrans(-0.5, 0.0, -0.5);//center at shoulder

	rightLowerArm.setAngle(0.0, 0.0, 0.0);
	rightLowerArm.setColor(1.0f, 0.0f, 0.0f);
	rightLowerArm.setScale(1.0, -2.0, 1.0);
	rightLowerArm.setStartPos(0.0, -2.2, 0.0);
	rightLowerArm.setTrans(-0.5, 0.0, -0.5);

	excaliburGuard.setAngle(0.0, 0.0, 0.0);
	excaliburGuard.setColor(0.0f, 1.0f, 1.0f);
	excaliburGuard.setScale(2.0, 0.5, 0.2);
	excaliburGuard.setStartPos(0.0, -0.25, 0.0);
	excaliburGuard.setTrans(-1.0, -0.25, -0.1);

	excaliburBlade.setAngle(0.0, 0.0, 0.0);
	excaliburBlade.setColor(1.0f, 1.0f, 1.0f);
	excaliburBlade.setScale(1.0, -3.0, 0.1);
	excaliburBlade.setStartPos(0.0, -0.25, 0.0);
	excaliburBlade.setTrans(-0.5, 0.0, -0.05);

	excaliburGrip.setAngle(0.0, 0.0, 0.0);
	excaliburGrip.setColor(0.5f, 0.5f, 0.3f);
	excaliburGrip.setScale(0.3, 0.8, 0.3);
	excaliburGrip.setStartPos(0.0, -2.0, 0.0);
	excaliburGrip.setTrans(-0.15, 0.0, -0.15);

	head.setAngle(0.0, 0.0, 0.0);
	head.setColor(0.0f, 1.0f, 1.0f);
	head.setScale(1.0, 1.0, 1.0);
	head.setStartPos(0.0, 0.2, 0.0);
	head.setTrans(-0.5, 0.0, -0.5);

	leftUpperLeg.setAngle(0.0, 0.0, 0.0);
	leftUpperLeg.setColor(0.0f, 0.0f, 1.0f);
	leftUpperLeg.setScale(1.0, -2.0, 1.0);
	leftUpperLeg.setStartPos(-0.7, -4.2, 0.0);
	leftUpperLeg.setTrans(-0.5, 0.0, -0.5);

	rightUpperLeg.setAngle(0.0, 0.0, 0.0);
	rightUpperLeg.setColor(0.0f, 0.0f, 1.0f);
	rightUpperLeg.setScale(1.0, -2.0, 1.0);
	rightUpperLeg.setStartPos(0.7, -4.2, 0.0);
	rightUpperLeg.setTrans(-0.5, 0.0, -0.5);

	leftLowerLeg.setAngle(0.0, 0.0, 0.0);
	leftLowerLeg.setColor(1.0f, 1.0f, 1.0f);
	leftLowerLeg.setScale(1.0, -2.0, 1.0);
	leftLowerLeg.setStartPos(0.0, -2.2, 0.0);
	leftLowerLeg.setTrans(-0.5, 0.0, -0.5);

	rightLowerLeg.setAngle(0.0, 0.0, 0.0);
	rightLowerLeg.setColor(1.0f, 1.0f, 1.0f);
	rightLowerLeg.setScale(1.0, -2.0, 1.0);
	rightLowerLeg.setStartPos(0.0, -2.2, 0.0);
	rightLowerLeg.setTrans(-0.5, 0.0, -0.5);
}

void SaberModel::RotateLeftUpperArm(GLdouble X, GLdouble Y, GLdouble Z){
	leftUpperArm.setAngle(X, Y, Z);
}
void SaberModel::RotateLeftLowerArm(GLdouble X, GLdouble Y, GLdouble Z){
	leftLowerArm.setAngle(X, Y, Z);
}
void SaberModel::RotateRightUpperArm(GLdouble X, GLdouble Y, GLdouble Z){
	rightUpperArm.setAngle(X, Y, Z);
}
void SaberModel::RotateRightLowerArm(GLdouble X, GLdouble Y, GLdouble Z){
	rightLowerArm.setAngle(X, Y, Z);
}

void SaberModel::RotateExcalibur(GLdouble X, GLdouble Y, GLdouble Z){
	excaliburGrip.setAngle(X, Y, Z);
}