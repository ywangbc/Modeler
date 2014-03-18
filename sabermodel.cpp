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

extern void _dump_current_material(void);
extern void _dump_current_modelview(void);
extern void _setupOpenGl();

void drawCylinderWithoutDisk(double h, double r1, double r2)
{
	ModelerDrawState *mds = ModelerDrawState::Instance();
	int divisions;

	_setupOpenGl();

	switch (mds->m_quality)
	{
	case HIGH:
		divisions = 32; break;
	case MEDIUM:
		divisions = 20; break;
	case LOW:
		divisions = 12; break;
	case POOR:
		divisions = 8; break;
	}

	if (mds->m_rayFile)
	{
		_dump_current_modelview();
		fprintf(mds->m_rayFile,
			"cone { height=%f; bottom_radius=%f; top_radius=%f;\n", h, r1, r2);
		_dump_current_material();
		fprintf(mds->m_rayFile, "})\n");
	}
	else
	{
		GLUquadricObj* gluq;

		/* GLU will again do the work.  draw the sides of the cylinder. */
		gluq = gluNewQuadric();
		gluQuadricDrawStyle(gluq, GLU_FILL);
		gluQuadricTexture(gluq, GL_TRUE);
		gluCylinder(gluq, r1, r2, h, divisions, divisions);
		gluDeleteQuadric(gluq);
	}

}

void QuadFunction(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2, GLdouble x3, GLdouble y3,GLdouble &A,GLdouble &B,GLdouble &C){
	GLdouble a0, a1, a2;
	if (abs(x1 - x2) < esp || abs(x1 - x3) < esp || abs(x2 - x3) < esp)return;
	a0 = y1 / (x1 - x2) / (x1 - x3);
	a1 = y2 / (x2 - x1) / (x2 - x3);
	a2 = y3 / (x3 - x1) / (x3 - x2);
	A = a0 + a1 + a2;
	B = -(a0*(x2 + x3) + a1*(x1 + x3) + a2*(x1 + x2));
	C = a0*x2*x3 + a1*x1*x3 + a2*x1*x2;

}

void drawTorso(GLdouble h,GLdouble r1,GLdouble r2,GLdouble rm,GLdouble mratio){
	GLdouble hup, hlow;
	hlow = mratio * h;
	hup = h - hlow;
	GLdouble A, B, C;

	QuadFunction(0, r1, hlow, rm, h, r2,A,B,C);

	ModelerDrawState *mds = ModelerDrawState::Instance();
	int divisions;

	_setupOpenGl();

	switch (mds->m_quality)
	{
	case HIGH:
		divisions = 32; break;
	case MEDIUM:
		divisions = 20; break;
	case LOW:
		divisions = 12; break;
	case POOR:
		divisions = 8; break;
	}

	if (mds->m_rayFile)
	{
		_dump_current_modelview();
		fprintf(mds->m_rayFile,
			"cone { height=%f; bottom_radius=%f; top_radius=%f;\n", h, r1, r2);
		_dump_current_material();
		fprintf(mds->m_rayFile, "})\n");
	}
	else
	{
		GLUquadricObj* gluq;

		/* GLU will again do the work.  draw the sides of the cylinder. */
		gluq = gluNewQuadric();
		gluQuadricDrawStyle(gluq, GLU_FILL);
		gluQuadricTexture(gluq, GL_TRUE);
		glPushMatrix();
		for (int i = 0; i < divisions; i++){
			GLdouble rfirst, rsecond, hfirst, hsecond;
			hfirst = i*h / divisions;
			hsecond = (i + 1)*h / divisions;
			rfirst = hfirst*hfirst*A + hfirst*B + C;
			rsecond = hsecond*hsecond*A + hsecond*B + C;
			gluCylinder(gluq, rfirst, rsecond, hsecond-hfirst, divisions, 3);
			glTranslated(0.0, 0.0, hsecond - hfirst);
		}
		glPopMatrix();
		gluDeleteQuadric(gluq);

		if (r1 > 0.0)
		{
			/* if the r1 end does not come to a point, draw a flat disk to
			cover it up. */

			gluq = gluNewQuadric();
			gluQuadricDrawStyle(gluq, GLU_FILL);
			gluQuadricTexture(gluq, GL_TRUE);
			gluQuadricOrientation(gluq, GLU_INSIDE);
			gluDisk(gluq, 0.0, r1, divisions, divisions);
			gluDeleteQuadric(gluq);
		}

		if (r2 > 0.0)
		{
			/* if the r2 end does not come to a point, draw a flat disk to
			cover it up. */

			/* save the current matrix mode. */
			int savemode;
			glGetIntegerv(GL_MATRIX_MODE, &savemode);

			/* translate the origin to the other end of the cylinder. */
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glTranslated(0.0, 0.0, h);

			/* draw a disk centered at the new origin. */
			gluq = gluNewQuadric();
			gluQuadricDrawStyle(gluq, GLU_FILL);
			gluQuadricTexture(gluq, GL_TRUE);
			gluQuadricOrientation(gluq, GLU_OUTSIDE);
			gluDisk(gluq, 0.0, r2, divisions, divisions);
			gluDeleteQuadric(gluq);

			/* restore the matrix stack and mode. */
			glPopMatrix();
			glMatrixMode(savemode);
		}
	}

	
}

void drawTorsoLinear(GLdouble h, GLdouble r1, GLdouble r2, GLdouble rm, GLdouble mratio){
	GLdouble hup, hlow;
	hlow = mratio * h;
	hup = h - hlow;

	ModelerDrawState *mds = ModelerDrawState::Instance();
	int divisions;

	_setupOpenGl();

	switch (mds->m_quality)
	{
	case HIGH:
		divisions = 32; break;
	case MEDIUM:
		divisions = 20; break;
	case LOW:
		divisions = 12; break;
	case POOR:
		divisions = 8; break;
	}

	if (mds->m_rayFile)
	{
		_dump_current_modelview();
		fprintf(mds->m_rayFile,
			"cone { height=%f; bottom_radius=%f; top_radius=%f;\n", h, r1, r2);
		_dump_current_material();
		fprintf(mds->m_rayFile, "})\n");
	}
	else
	{
		GLUquadricObj* gluq;

		/* GLU will again do the work.  draw the sides of the cylinder. */
		gluq = gluNewQuadric();
		gluQuadricDrawStyle(gluq, GLU_FILL);
		gluQuadricTexture(gluq, GL_TRUE);
		glPushMatrix();
			gluCylinder(gluq, r1, rm, hlow, divisions, divisions);
			glTranslated(0.0, 0.0, hlow);
			gluCylinder(gluq, rm, r2, hup, divisions, divisions);
		glPopMatrix();
		gluDeleteQuadric(gluq);

		if (r1 > 0.0)
		{
			/* if the r1 end does not come to a point, draw a flat disk to
			cover it up. */

			gluq = gluNewQuadric();
			gluQuadricDrawStyle(gluq, GLU_FILL);
			gluQuadricTexture(gluq, GL_TRUE);
			gluQuadricOrientation(gluq, GLU_INSIDE);
			gluDisk(gluq, 0.0, r1, divisions, divisions);
			gluDeleteQuadric(gluq);
		}

		if (r2 > 0.0)
		{
			/* if the r2 end does not come to a point, draw a flat disk to
			cover it up. */

			/* save the current matrix mode. */
			int savemode;
			glGetIntegerv(GL_MATRIX_MODE, &savemode);

			/* translate the origin to the other end of the cylinder. */
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glTranslated(0.0, 0.0, h);

			/* draw a disk centered at the new origin. */
			gluq = gluNewQuadric();
			gluQuadricDrawStyle(gluq, GLU_FILL);
			gluQuadricTexture(gluq, GL_TRUE);
			gluQuadricOrientation(gluq, GLU_OUTSIDE);
			gluDisk(gluq, 0.0, r2, divisions, divisions);
			gluDeleteQuadric(gluq);

			/* restore the matrix stack and mode. */
			glPopMatrix();
			glMatrixMode(savemode);
		}
	}


}

void drawBlade(int swordType){
	if(swordType==TYPE_EXCALIBUR_MORGAN)setDiffuseColor(USE_COLOR_DARK);
	else setDiffuseColor(USE_COLOR_EXCALIBUR);
	glScaled(1.0/13.5, 1.0 / 320.0, 1.0);
	//Blade top:(0,320,0)
	//First part:blade
	//Left upper
	drawTriangle(0.0, 280.0, 1.0, 0.0, 320.0, 0.0, -9.0, 300.0, 0.0);
	drawTriangle(0.0, 280.0, 1.0, -9.0, 300.0, 0.0, -4.0,275.0,1.0);
	drawTriangle(-4.0, 275.0, 1.0, -9.0, 300.0, 0.0, -13.5, 23.0, 0.0);
	drawTriangle(-4.0, 275.0, 1.0, -13.5, 23.0, 0.0, -8.0, 37.0, 1.0);
	drawTriangle(-8.0, 37.0, 1.0, -13.5, 23.0, 0.0, -8.0, 18.0, 1.0);
	//Right upper
	drawTriangle(0.0, 280.0, 1.0, 9.0, 300.0, 0.0, 0.0, 320.0, 0.0);
	drawTriangle(0.0, 280.0, 1.0, 4.0, 275.0, 1.0, 9.0, 300.0, 0.0);
	drawTriangle(4.0, 275.0, 1.0, 13.5, 23.0, 0.0, 9.0, 300.0, 0.0);
	drawTriangle(4.0, 275.0, 1.0, 8.0, 37.0, 1.0, 13.5, 23.0, 0.0);
	drawTriangle(8.0, 37.0, 1.0, 8.0, 18.0, 1.0, 13.5, 23.0, 0.0);
	//Left lower
	drawTriangle(0.0, 280.0, -1.0, -9.0, 300.0, 0.0, 0.0, 320.0, 0.0);
	drawTriangle(0.0, 280.0, -1.0, -4.0, 275.0, -1.0, -9.0, 300.0, 0.0);
	drawTriangle(-4.0, 275.0, -1.0, -13.5, 23.0, 0.0, -9.0, 300.0, 0.0);
	drawTriangle(-4.0, 275.0, -1.0, -8.0, 37.0, -1.0, -13.5, 23.0, 0.0);
	drawTriangle(-8.0, 37.0, -1.0, -8.0, 18.0, -1.0, -13.5, 23.0, 0.0);
	//Right lower
	drawTriangle(0.0, 280.0, -1.0, 0.0, 320.0, 0.0, 9.0, 300.0, 0.0);
	drawTriangle(0.0, 280.0, -1.0, 9.0, 300.0, 0.0, 4.0, 275.0, -1.0);
	drawTriangle(4.0, 275.0, -1.0, 9.0, 300.0, 0.0, 13.5, 23.0, 0.0);
	drawTriangle(4.0, 275.0, -1.0, 13.5, 23.0, 0.0, 8.0, 37.0, -1.0);
	drawTriangle(8.0, 37.0, -1.0, 13.5, 23.0, 0.0, 8.0, 18.0, -1.0);
	//Second part:spine
	switch (swordType){
	case TYPE_EXCALIBUR:
		setDiffuseColor(USE_COLOR_EXCALIBUR);
		break;
	case TYPE_EXCALIBUR_MORGAN:
		setDiffuseColor(USE_COLOR_BLACK);
		break;
	case TYPE_CALIBURN:
		setDiffuseColor(USE_COLOR_GOLD);
		break;
	}
	//Up
	drawTriangle(-4.0, 275.0, 1.0, 4.0, 275.0, 1.0, 0.0, 280.0, 1.0);
	drawTriangle(0.0, 55.0, 1.0, 4.0, 275.0, 1.0, 0.5, 275.0, 1.0);
	drawTriangle(0.0, 55.0, 1.0, -0.5, 275.0, 1.0, -4.0, 275.0, 1.0);
	drawTriangle(0.0, 55.0, 1.0, 8.0, 46.0, 1.0, 4.0, 275.0, 1.0);
	drawTriangle(0.0, 55.0, 1.0, -4.0, 275.0, 1.0, -8.0, 46.0, 1.0);
	//Down
	drawTriangle(-4.0, 275.0, -1.0, 0.0, 280.0, -1.0, 4.0, 275.0, -1.0);
	drawTriangle(0.0, 55.0, -1.0, -4.0, 275.0, -1.0, -0.5, 275.0, -1.0);
	drawTriangle(0.0, 55.0, -1.0, 0.5, 275.0, -1.0, 4.0, 275.0, -1.0);
	drawTriangle(0.0, 55.0, -1.0, 4.0, 275.0, -1.0, 8.0, 46.0, -1.0);
	drawTriangle(0.0, 55.0, -1.0, -8.0, 46.0, -1.0, -4.0, 275.0, -1.0);
	if (swordType == TYPE_EXCALIBUR_MORGAN)setDiffuseColor(USE_COLOR_RED);
	drawTriangle(0.0, 55.0, 1.0, 0.5, 275.0, 1.0, -0.5, 275.0, 1.0);
	drawTriangle(0.0, 55.0, -1.0, -0.5, 275.0, -1.0, 0.5, 275.0, -1.0);

	//Third part:decoration
	switch (swordType){
	case TYPE_EXCALIBUR:
		setDiffuseColor(USE_COLOR_BLUE);
		break;
	case TYPE_EXCALIBUR_MORGAN:
		setDiffuseColor(USE_COLOR_BLACK);
		break;
	case TYPE_CALIBURN:
		setDiffuseColor(USE_COLOR_BLUE);
		break;
	}
	//up
	drawTriangle(-8.0, 46.0, 1.0, 8.0, 46.0, 1.0, 0.0, 53.0, 1.0);
	
	drawTriangle(-8.0, 0.0, 1.0, 8.0, 46.0, 1.0, -8.0, 46.0, 1.0);
	drawTriangle(-8.0, 0.0, 1.0, 8.0, 0.0, 1.0, 8.0, 46.0, 1.0);
	//down
	drawTriangle(-8.0, 46.0, -1.0, 0.0, 53.0, -1.0, 8.0, 46.0, -1.0);

	drawTriangle(-8.0, 0.0, -1.0, -8.0, 46.0, -1.0, 8.0, 46.0, -1.0);
	drawTriangle(-8.0, 0.0, -1.0, 8.0, 46.0, -1.0, 8.0, 0.0, -1.0);
	switch (swordType){
	case TYPE_EXCALIBUR:
		setDiffuseColor(USE_COLOR_GOLD);
		break;
	case TYPE_EXCALIBUR_MORGAN:
		setDiffuseColor(USE_COLOR_RED);
		break;
	case TYPE_CALIBURN:
		setDiffuseColor(USE_COLOR_BLUE);
		break;
	}
	drawTriangle(-8.0, 46.0, 1.0, 0.0, 53.0, 1.0, 0.0, 55.0, 1.0);
	drawTriangle(0.0, 55.0, 1.0, 0.0, 53.0, 1.0, 8.0, 46.0, 1.0);

	drawTriangle(-8.0, 46.0, -1.0, 0.0, 55.0, -1.0, 0.0, 53.0, -1.0);
	drawTriangle(0.0, 55.0, -1.0, 8.0, 46.0, -1.0, 0.0, 53.0, -1.0);

	//Fourth part:Side
	switch (swordType){
	case TYPE_EXCALIBUR:
		setDiffuseColor(USE_COLOR_GOLD);
		break;
	case TYPE_EXCALIBUR_MORGAN:
		setDiffuseColor(USE_COLOR_DARK);
		break;
	case TYPE_CALIBURN:
		setDiffuseColor(USE_COLOR_BLUE);
		break;
	}
	//left up
	drawTriangle(-8.0, 18.0, 1.0, -13.5, 23.0, 0.0, -13.5, 0.0, 0.0);
	drawTriangle(-8.0, 18.0, 1.0, -13.5, 0.0, 0.0, -8.0, 0.0, 1.0);

	//right up
	drawTriangle(8.0, 18.0, 1.0, 13.5, 0.0, 0.0, 13.5, 23.0, 0.0);
	drawTriangle(8.0, 18.0, 1.0, 8.0, 0.0, 1.0, 13.5, 0.0, 0.0);
	//left down

	drawTriangle(-8.0, 18.0, -1.0, -13.5, 0.0, 0.0, -13.5, 23.0, 0.0);
	drawTriangle(-8.0, 18.0, -1.0, -8.0, 0.0, -1.0, -13.5, 0.0, 0.0);
	//right down

	drawTriangle(8.0, 18.0, -1.0, 13.5, 23.0, 0.0, 13.5, 0.0, 0.0);
	drawTriangle(8.0, 18.0, -1.0, 13.5, 0.0, 0.0, 8.0, 0.0, -1.0);
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

void ModelNode::cylinderScale(GLdouble theUpperScale, GLdouble theMiddleScale, GLdouble theMiddleRatio){
	upperScale = theUpperScale;
	middleScale = theMiddleScale;
	middleRatio = theMiddleRatio;
}

void ModelNode::setSwordType(int ty){
	swordType = ty;
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
	switch (primitiveType){
	case PRIMITIVE_BOX:
		glTranslated(transX, transY, transZ);
		drawBox(xScale, yScale, zScale);
		break;
	case PRIMITIVE_SPHERE:
		glTranslated(transX, transY, transZ);
		glScaled(xScale, yScale, zScale);
		drawSphere(1.0);
		break;
	case PRIMITIVE_CYLINDER:
		glRotated(90.0, 1.0, 0.0, 0.0);
		glTranslated(transX, transZ, transY);
		glScaled(xScale, zScale, yScale / abs(yScale));
		drawCylinder(abs(yScale), 1.0, upperScale);
		break;
	case PRIMITIVE_CYLINDER_NO_DISK:
		glRotated(90.0, 1.0, 0.0, 0.0);
		glTranslated(transX, transZ, transY);
		glScaled(xScale, zScale, yScale/abs(yScale));
		drawCylinderWithoutDisk(abs(yScale), 1.0, upperScale);
		break;
	case SHAPE_TORSO:
		glRotated(90.0, 1.0, 0.0, 0.0);
		glTranslated(transX, transZ, transY);
		glScaled(xScale, zScale, yScale / abs(yScale));
		drawTorso(abs(yScale),1.0,upperScale,middleScale,middleRatio);
		break;
	case SHAPE_TORSO_LINEAR:
		glRotated(90.0, 1.0, 0.0, 0.0);
		glTranslated(transX, transZ, transY);
		glScaled(xScale, zScale, yScale / abs(yScale));
		drawTorsoLinear(abs(yScale), 1.0, upperScale, middleScale,middleRatio);
		break;
	case SHAPE_BLADE:
		glTranslated(transX, transY, transZ);
		glScaled(xScale, yScale, zScale);
		drawBlade(swordType);
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
	upperTorso.nodeCreate(NULL, SHAPE_TORSO);
	rightUpperArm.nodeCreate(&upperTorso, SHAPE_TORSO);
	rightLowerArm.nodeCreate(&rightUpperArm, PRIMITIVE_CYLINDER);
	rightShoulder.nodeCreate(&rightUpperArm, SHAPE_TORSO_LINEAR);
	leftUpperArm.nodeCreate(&upperTorso, SHAPE_TORSO);
	leftLowerArm.nodeCreate(&leftUpperArm, PRIMITIVE_CYLINDER);
	leftShoulder.nodeCreate(&leftUpperArm, SHAPE_TORSO_LINEAR);
	excaliburGrip.nodeCreate(&rightLowerArm, SHAPE_TORSO);
	excaliburGuard.nodeCreate(&excaliburGrip, PRIMITIVE_BOX);
	excaliburBlade.nodeCreate(&excaliburGuard, SHAPE_BLADE);
	lowerTorso.nodeCreate(&upperTorso, PRIMITIVE_CYLINDER_NO_DISK);
	head.nodeCreate(&upperTorso, PRIMITIVE_SPHERE);
	leftUpperLeg.nodeCreate(&upperTorso, PRIMITIVE_BOX);
	leftLowerLeg.nodeCreate(&leftUpperLeg, PRIMITIVE_BOX);
	rightUpperLeg.nodeCreate(&upperTorso, PRIMITIVE_BOX);
	rightLowerLeg.nodeCreate(&rightUpperLeg, PRIMITIVE_BOX);
	treeRoot = &upperTorso;
}

void SaberModel::CostumeSaber(){

	leftShoulder.nodeCreate(NULL, SHAPE_TORSO_LINEAR);
	rightShoulder.nodeCreate(NULL, SHAPE_TORSO_LINEAR);

	upperTorso.setAngle(0.0, 0.0, 0.0);
	upperTorso.setColor(USE_COLOR_SILVER);
	upperTorso.setScale(0.32, 2.5, 0.2);
	upperTorso.setStartPos(0.0, 0.0, 0.0);
	upperTorso.setTrans(0.0, 0, 0.0);//center at neck
	upperTorso.cylinderScale(2.5,3.0,0.4);

	lowerTorso.setAngle(0.0, 0.0, 0.0);
	lowerTorso.setColor(USE_COLOR_BLUE);
	lowerTorso.setScale(0.8, 4.5, 0.5);
	lowerTorso.setStartPos(0.0, -2.5, 0.0);
	lowerTorso.setTrans(0.0, 0.0, 0.0);//center at weist
	lowerTorso.cylinderScale(4.0, 1.0, 0.5);

	leftUpperArm.setAngle(0.0, 0.0, 0.0);
	leftUpperArm.setColor(USE_COLOR_BLUE);
	leftUpperArm.setScale(0.35, 2.2, 0.35);
	leftUpperArm.setStartPos(-1.5, -1.0, 0.0);
	leftUpperArm.setTrans(0.0,0.0,0.0);//center at shoulder
	leftUpperArm.cylinderScale(0.8, 1.2, 0.5);

	leftLowerArm.setAngle(0.0, 0.0, 0.0);
	leftLowerArm.setColor(USE_COLOR_SILVER);
	leftLowerArm.setScale(0.5, 2.3, 0.5);
	leftLowerArm.setStartPos(0.0, -2.0, 0.0);
	leftLowerArm.setTrans(0.0, 0.0, 0.0);
	leftLowerArm.cylinderScale(0.6, 1.0, 0.5);

	leftShoulder.setAngle(0.0, 0.0, -15.0);
	leftShoulder.setColor(USE_COLOR_BLUE);
	leftShoulder.setScale(0.4, 1.0, 0.4);
	leftShoulder.setStartPos(0.0, 0.0, 0.0);
	leftShoulder.setTrans(0.0, -0.8, 0.0);
	leftShoulder.cylinderScale(1.0, 1.5, 0.4);

	rightUpperArm.setAngle(0.0, 0.0, 0.0);
	rightUpperArm.setColor(USE_COLOR_BLUE);
	rightUpperArm.setScale(0.35, 2.2, 0.35);
	rightUpperArm.setStartPos(1.5, -1.0, 0.0);
	rightUpperArm.setTrans(0.0, 0.0, 0.0);//center at shoulder
	rightUpperArm.cylinderScale(0.8, 1.2, 0.5);

	rightLowerArm.setAngle(0.0, 0.0, 0.0);
	rightLowerArm.setColor(USE_COLOR_SILVER);
	rightLowerArm.setScale(0.5, 2.3, 0.5);
	rightLowerArm.setStartPos(0.0, -2.0, 0.0);
	rightLowerArm.setTrans(0.0, 0.0, 0.0);
	rightLowerArm.cylinderScale(0.6, 1.0, 0.5);

	rightShoulder.setAngle(0.0, 0.0, 15.0);
	rightShoulder.setColor(USE_COLOR_BLUE);
	rightShoulder.setScale(0.4, 1.0, 0.4);
	rightShoulder.setStartPos(0.0, 0.0, 0.0);
	rightShoulder.setTrans(0.0, -0.8, 0.0);
	rightShoulder.cylinderScale(1.0, 1.5, 0.4);

	excaliburGuard.setAngle(0.0, 0.0, 0.0);
	excaliburGuard.setColor(USE_COLOR_GOLD);
	excaliburGuard.setScale(2.0, 0.5, 0.4);
	excaliburGuard.setStartPos(0.0, -0.25, 0.0);
	excaliburGuard.setTrans(-1.0, -0.25, -0.2);

	excaliburBlade.setAngle(0.0, 0.0, 0.0);
	excaliburBlade.setColor(USE_COLOR_EXCALIBUR);
	excaliburBlade.setScale(0.4, -4.0, 0.1);
	excaliburBlade.setStartPos(0.0, -0.25, 0.0);
	excaliburBlade.setTrans(0.0, 0.0, 0.0);
	excaliburBlade.setSwordType(TYPE_EXCALIBUR);

	excaliburGrip.setAngle(0.0, 0.0, 0.0);
	excaliburGrip.setColor(USE_COLOR_BLUE);
	excaliburGrip.setScale(0.2, -1.0, 0.2);
	excaliburGrip.setStartPos(0.0, -2.5, 0.0);
	excaliburGrip.setTrans(0.0, 0.0, -0.0);
	excaliburGrip.cylinderScale(0.8, 0.7, 0.5);

	head.setAngle(0.0, 0.0, 0.0);
	head.setColor(USE_COLOR_GOLD);
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
void SaberModel::CostumeSaberAlter(){
	CostumeSaber();
	excaliburBlade.setSwordType(TYPE_EXCALIBUR_MORGAN);

	upperTorso.setColor(USE_COLOR_DARK);
	lowerTorso.setColor(USE_COLOR_BLACK);
	leftUpperArm.setColor(USE_COLOR_BLACK);
	leftLowerArm.setColor(USE_COLOR_DARK);
	rightUpperArm.setColor(USE_COLOR_BLACK);
	rightLowerArm.setColor(USE_COLOR_DARK);
	leftUpperLeg.setColor(USE_COLOR_BLACK);
	leftLowerLeg.setColor(USE_COLOR_DARK);
	rightUpperLeg.setColor(USE_COLOR_BLACK);
	rightLowerLeg.setColor(USE_COLOR_DARK);
	leftShoulder.setColor(USE_COLOR_DARK);
	rightShoulder.setColor(USE_COLOR_DARK);
	leftShoulder.cylinderScale(1.0, 1.8, 0.4);
	rightShoulder.cylinderScale(1.0, 1.8, 0.4);
}
void SaberModel::CostumeSaberLily(){
	CostumeSaber();
	excaliburBlade.setSwordType(TYPE_CALIBURN);
	
	leftShoulder.nodeCreate(NULL, SHAPE_TORSO);
	rightShoulder.nodeCreate(NULL, SHAPE_TORSO);

	upperTorso.setColor(USE_COLOR_SILVER);
	lowerTorso.setColor(USE_COLOR_WHITE);
	leftUpperArm.setColor(USE_COLOR_WHITE);
	leftLowerArm.setColor(USE_COLOR_SILVER);
	rightUpperArm.setColor(USE_COLOR_WHITE);
	rightLowerArm.setColor(USE_COLOR_SILVER);
	leftUpperLeg.setColor(USE_COLOR_WHITE);
	leftLowerLeg.setColor(USE_COLOR_SILVER);
	rightUpperLeg.setColor(USE_COLOR_WHITE);
	rightLowerLeg.setColor(USE_COLOR_SILVER);
	leftShoulder.setColor(USE_COLOR_WHITE);
	rightShoulder.setColor(USE_COLOR_WHITE);
	leftShoulder.cylinderScale(1.0, 1.2, 0.4);
	rightShoulder.cylinderScale(1.0, 1.2, 0.4);

}

void SaberModel::ChooseCostume(int cost){
	switch (cost){
	case COSTUME_SABER:
		CostumeSaber();
		break;
	case COSTUME_SABER_ALTER:
		CostumeSaberAlter();
		break;
	case COSTUME_SABER_LILY:
		CostumeSaberLily();
		break;
	}
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