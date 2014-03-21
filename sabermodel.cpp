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

const GLdouble DIVIDE_POINT = 1.0/4.0;

void drawTorsoHalfLinear(GLdouble h,GLdouble r1,GLdouble r2,GLdouble rm,GLdouble mratio,GLdouble ACME_DELTA){
	GLdouble hup, hlow;
	hlow = mratio * h;
	hup = h - hlow;
	GLdouble A, B, C;

	QuadFunction(0+hlow*(1-DIVIDE_POINT), r1*(DIVIDE_POINT)+rm*(1-DIVIDE_POINT), hlow, rm*(1-ACME_DELTA), hup*DIVIDE_POINT+hlow, r2*DIVIDE_POINT+rm*(1-DIVIDE_POINT),A,B,C);

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
		gluCylinder(gluq, r1, r1*(DIVIDE_POINT)+rm*(1 - DIVIDE_POINT), hlow*(1 - DIVIDE_POINT), divisions, divisions);
		glTranslated(0.0, 0.0, hlow*(1 - DIVIDE_POINT));
		for (int i = 0; i < divisions; i++){
			GLdouble rfirst, rsecond, hfirst, hsecond;
			hfirst = i*h*DIVIDE_POINT / divisions + hlow*(1-DIVIDE_POINT);
			hsecond = (i + 1)*h*DIVIDE_POINT / divisions + hlow*(1 - DIVIDE_POINT);
			rfirst = hfirst*hfirst*A + hfirst*B + C;
			rsecond = hsecond*hsecond*A + hsecond*B + C;
			gluCylinder(gluq, rfirst, rsecond, hsecond-hfirst, divisions, 3);
			glTranslated(0.0, 0.0, hsecond - hfirst);
		}
		gluCylinder(gluq, r2*DIVIDE_POINT + rm*(1 - DIVIDE_POINT), r2, hup*(1-DIVIDE_POINT), divisions, divisions);
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


void drawTorso(GLdouble h, GLdouble r1, GLdouble r2, GLdouble rm, GLdouble mratio){
	GLdouble hup, hlow;
	hlow = mratio * h;
	hup = h - hlow;
	GLdouble A, B, C;

	QuadFunction(0, r1, hlow, rm, hup + hlow, r2, A, B, C);

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
			gluCylinder(gluq, rfirst, rsecond, hsecond - hfirst, divisions, 3);
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

void drawPartialCylinder(GLdouble h, GLdouble r1, GLdouble r2, GLdouble startAngle, GLdouble endAngle){
	ModelerDrawState *mds = ModelerDrawState::Instance();
	int divisions;
	startAngle *= 3.1415926 / 180.0;
	endAngle *= 3.1415926 / 180.0;

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
		divisions *= 3;
		GLdouble rangeAngle = endAngle - startAngle;
		GLdouble upthis[2], uplast[2], downthis[2], downlast[2];
		upthis[0] = r1*cos(startAngle);
		upthis[1] = r1*sin(startAngle);
		downthis[0] = r2*cos(startAngle);
		downthis[1] = r2*sin(startAngle);
		for (int i = 1; i <= divisions; i++){
			GLdouble curAngle = (rangeAngle / divisions)*i + startAngle;
			uplast[0] = upthis[0];
			uplast[1] = upthis[1];
			downlast[0] = downthis[0];
			downlast[1] = downthis[1];
			upthis[0] = r1*cos(curAngle);
			upthis[1] = r1*sin(curAngle);
			downthis[0] = r2*cos(curAngle);
			downthis[1] = r2*sin(curAngle);
			drawTriangle(uplast[0],uplast[1],0.0, downlast[0],downlast[1],h, upthis[0],upthis[1],0.0);
			drawTriangle(upthis[0],upthis[1],0.0, downlast[0],downlast[1],h, downthis[0],downthis[1],h);
		}


	}

}

void drawPartialTorso(GLdouble h, GLdouble r1, GLdouble r2, GLdouble rm,GLdouble mratio,GLdouble startAngle, GLdouble endAngle){
	ModelerDrawState *mds = ModelerDrawState::Instance();
	int divisions;
	
	GLdouble hup, hlow;
	hlow = mratio * h;
	hup = h - hlow;
	GLdouble A, B, C;

	QuadFunction(0, r1, hlow, rm, hup + hlow, r2, A, B, C);

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
		glPushMatrix();
		for (int i = 0; i < divisions; i++){
			GLdouble rfirst, rsecond, hfirst, hsecond;
			hfirst = i*h / divisions;
			hsecond = (i + 1)*h / divisions;
			rfirst = hfirst*hfirst*A + hfirst*B + C;
			rsecond = hsecond*hsecond*A + hsecond*B + C;
			drawPartialCylinder(hsecond - hfirst, rfirst, rsecond,startAngle,endAngle);
			glTranslated(0.0, 0.0, hsecond - hfirst);
		}
		glPopMatrix();

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
		setDiffuseColor(USE_COLOR_DARK_GOLD);
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
		setDiffuseColor(USE_COLOR_GOLD);
		break;
	}
	//up
	drawTriangle(-8.0, 46.0, 1.0, 8.0, 46.0, 1.0, 0.0, 51.0, 1.0);
	
	drawTriangle(-8.0, 0.0, 1.0, 8.0, 46.0, 1.0, -8.0, 46.0, 1.0);
	drawTriangle(-8.0, 0.0, 1.0, 8.0, 0.0, 1.0, 8.0, 46.0, 1.0);
	//down
	drawTriangle(-8.0, 46.0, -1.0, 0.0, 51.0, -1.0, 8.0, 46.0, -1.0);

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
		setDiffuseColor(USE_COLOR_GOLD);
		break;
	}
	drawTriangle(-8.0, 46.0, 1.0, 0.0, 51.0, 1.0, 0.0, 55.0, 1.0);
	drawTriangle(0.0, 55.0, 1.0, 0.0, 51.0, 1.0, 8.0, 46.0, 1.0);

	drawTriangle(-8.0, 46.0, -1.0, 0.0, 55.0, -1.0, 0.0, 51.0, -1.0);
	drawTriangle(0.0, 55.0, -1.0, 8.0, 46.0, -1.0, 0.0, 51.0, -1.0);

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

void drawFoot(){
	glPushMatrix();
	glScaled(1.0, 1.5, 1.0);
	drawPartialCylinder(1.5, 0.4, 0, 0, 180);
	glPopMatrix();
	drawTriangle(-0.5, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 1.5);
	glPushMatrix();
	glRotated(-90.0, 1.0, 0.0, 0.0);
	drawTorso(1.5, 0.52, 0.35, 0.4, 0.4);
	glPopMatrix();
}

void drawEye(int li,int ty){
	setDiffuseColor(USE_COLOR_WHITE);
	drawTriangle(4.0, 3.0, 0.0, 3.0, 3.0, 0.0, 3.0, -1.0, 0.0);
	drawTriangle(-3.0, 3.0, 0.0, -4.0, 3.0, 0.0, -3.0, -1.0, 0.0);

	if(ty!=COSTUME_SABER_ALTER)setDiffuseColor(USE_COLOR_EYE_DARK_GREEN);
	else setDiffuseColor(USE_COLOR_EYE_DARK_YELLOW);
	drawTriangle(3.0, 3.0, 0.0, -3.0, 3.0, 0.0, -3.0, 1.0, 0.0);
	drawTriangle(3.0, 3.0, 0.0, -3.0, 1.0, 0.0, 3.0, 1.0, 0.0);
	drawTriangle(1.0, 1.0, 0.0, -1.0, 1.0, 0.0, -1.0, -1.0, 0.0);
	drawTriangle(1.0, 1.0, 0.0, -1.0, -1.0, 0.0, 1.0, -1.0, 0.0);

	setDiffuseColor(USE_COLOR_WHITE);
	drawTriangle(-1.0, 1.0, 0.0, -3.0, 1.0, 0.0, -3.0, 0.0, 0.0);
	drawTriangle(-1.0, 1.0, 0.0, -3.0, 0.0, 0.0, -1.0, 0.0, 0.0);

	if (ty != COSTUME_SABER_ALTER)setDiffuseColor(USE_COLOR_EYE_GREEN);
	else setDiffuseColor(USE_COLOR_EYE_YELLOW);
	drawTriangle(1.0, 1.0, 0.0, 3.0, 0.0, 0.0, 3.0, 1.0, 0.0);
	drawTriangle(1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 3.0, 0.0, 0.0);
	drawTriangle(-1.0, 0.0, 0.0, -3.0, 0.0, 0.0, -3.0, -1.0, 0.0);
	drawTriangle(-1.0, 0.0, 0.0, -3.0, -1.0, 0.0, -1.0, -1.0, 0.0);
	drawTriangle(1.0, 0.0, 0.0, 3.0, -1.0, 0.0, 3.0, 0.0, 0.0);
	drawTriangle(1.0, 0.0, 0.0, 1.0, -1.0, 0.0, 3.0, -1.0, 0.0);
	drawTriangle(2.0, -3.0, 0.0, -3.0, -1.0, 0.0, -2.0, -3.0, 0.0);
	drawTriangle(2.0, -3.0, 0.0, 3.0,-1.0, 0.0, -3.0, -1.0, 0.0);
	
	setDiffuseColor(USE_COLOR_BLACK);
	if (li){
		drawTriangle(-4.0, 4.5, 0.0, 4.0, 4.0, 0.0, 4.0, 4.2, 0.0);
		drawTriangle(-4.0, 4.8, 0.0, -4.0, 4.5, 0.0, 4.0, 4.0, 0.0);
	}
	else {
		drawTriangle(-4.0, 4.0, 0.0, 4.0, 4.5, 0.0, 4.0, 4.8, 0.0);
		drawTriangle(-4.0, 4.2, 0.0, -4.0, 4.0, 0.0, 4.0, 4.5, 0.0);
	}

}

void drawNose(){
	setDiffuseColor(USE_COLOR_BODY);
	drawTriangle(0.0, 3.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	drawTriangle(0.0, 3.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0);
	drawTriangle(0.0, 0.0, 1.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
}
void drawMouth(){
	setDiffuseColor(USE_COLOR_BODY);
	drawTriangle(0.0, 1.0, 0.0, -3.0, 0.0, 0.0, 0.0, 0.2, 1.0);
	drawTriangle(0.0, 1.0, 0.0, 0.0, 0.2, 1.0, 3.0, 0.0, 0.0);
	drawTriangle(0.0, 0.2, 1.0, -3.0, 0.0, 0.0, 3.0, 0.0, 0.0);
	drawTriangle(0.0, -1.0, 0.0, 0.0, -0.2, 1.0, -3.0, 0.0, 0.0);
	drawTriangle(0.0, -1.0, 0.0, 3.0, 0.0, 0.0, 0.0, -0.2, 1.0);
	drawTriangle(0.0, -0.2, 1.0, 3.0, 0.0, 0.0, -3.0, 0.0, 0.0);
}

void drawHair(GLdouble mp,GLdouble ep){
	drawTriangle(0.5, 0.0, 0.0, mp - 0.25, -0.5, 0.0, -0.5, 0.0, 0.0);
	drawTriangle(0.5, 0.0, 0.0, mp + 0.25, -0.5, 0.0, mp - 0.25, -0.5, 0.0);
	drawTriangle(mp + 0.25, -0.5, 0.0, mp + ep, -1.0, 0.0, mp - 0.25, -0.5, 0.0);
}

#define UP_LINE_HEIGHT(x) (x*x/5.0)
#define LOW_LINE_HEIGHT(x) (x*x/10.0)*0.3
#define FACE_START_POINT 0.5
#define FACE_INDENT 0.2
#define FACE_BOTTOM 0.0, -0.05, 0.6
#define LOWER_RADIUS 0.6
#define LOWER_HEIGHT 0.25
#define BOTTOM_RADIUS 0.1
void drawHead(int ty){
	srand(time(0));
	glPushMatrix();
	if(ty!=COSTUME_SABER_ALTER)setDiffuseColor(USE_COLOR_HAIR_GOLD);
	else setDiffuseColor(USE_COLOR_HAIR_DARK_GOLD);
	glTranslated(0.0, 1.0, 0.0);
	glRotated(90.0, 1.0, 0.0, 0.0);
	drawCylinder(1 - LOWER_HEIGHT, 1.0, LOWER_RADIUS);
	glPopMatrix();
	glPushMatrix();
	if (ty != COSTUME_SABER_ALTER)setDiffuseColor(USE_COLOR_BODY);
	else setDiffuseColor(USE_COLOR_PALE);
	glTranslated(0.0, LOWER_HEIGHT, 0.0);
	glRotated(90.0, 1.0, 0.0, 0.0);
	drawCylinder(LOWER_HEIGHT+0.01, LOWER_RADIUS, BOTTOM_RADIUS);
	glPopMatrix();

	if (ty != COSTUME_SABER_ALTER)setDiffuseColor(USE_COLOR_BODY);
	else setDiffuseColor(USE_COLOR_PALE);

	drawTriangle(cos(FACE_START_POINT), 1.0, sin(FACE_START_POINT), cos(FACE_START_POINT) - FACE_INDENT, 1.0, sin(FACE_START_POINT) + FACE_INDENT / sin(FACE_START_POINT) * cos(FACE_START_POINT), LOWER_RADIUS*cos(FACE_START_POINT) - FACE_INDENT, LOWER_HEIGHT, LOWER_RADIUS*sin(FACE_START_POINT) + FACE_INDENT / sin(FACE_START_POINT) * cos(FACE_START_POINT));
	drawTriangle(cos(FACE_START_POINT), 1.0, sin(FACE_START_POINT), LOWER_RADIUS*cos(FACE_START_POINT) - FACE_INDENT, LOWER_HEIGHT, LOWER_RADIUS*sin(FACE_START_POINT) + FACE_INDENT / sin(FACE_START_POINT) * cos(FACE_START_POINT), cos(FACE_START_POINT)* LOWER_RADIUS, LOWER_HEIGHT, sin(FACE_START_POINT)*LOWER_RADIUS);
	drawTriangle(-cos(FACE_START_POINT), 1.0, sin(FACE_START_POINT), -LOWER_RADIUS*cos(FACE_START_POINT) + FACE_INDENT, LOWER_HEIGHT, LOWER_RADIUS*sin(FACE_START_POINT) + FACE_INDENT / sin(FACE_START_POINT) * cos(FACE_START_POINT), -cos(FACE_START_POINT) + FACE_INDENT, 1.0, sin(FACE_START_POINT) + FACE_INDENT / sin(FACE_START_POINT) * cos(FACE_START_POINT));
	drawTriangle(-cos(FACE_START_POINT), 1.0, sin(FACE_START_POINT), -cos(FACE_START_POINT)* LOWER_RADIUS, LOWER_HEIGHT, sin(FACE_START_POINT)*LOWER_RADIUS, -LOWER_RADIUS*cos(FACE_START_POINT) + FACE_INDENT, LOWER_HEIGHT, LOWER_RADIUS*sin(FACE_START_POINT) + FACE_INDENT / sin(FACE_START_POINT) * cos(FACE_START_POINT));
	
	
	drawTriangle(cos(FACE_START_POINT), 1.0, sin(FACE_START_POINT), cos(FACE_START_POINT) - FACE_INDENT, 1.0, sin(FACE_START_POINT) + FACE_INDENT / sin(FACE_START_POINT) * cos(FACE_START_POINT), LOWER_RADIUS*cos(FACE_START_POINT) - FACE_INDENT, LOWER_HEIGHT, LOWER_RADIUS*sin(FACE_START_POINT) + FACE_INDENT / sin(FACE_START_POINT) * cos(FACE_START_POINT));
	drawTriangle(cos(FACE_START_POINT), 1.0, sin(FACE_START_POINT), LOWER_RADIUS*cos(FACE_START_POINT) - FACE_INDENT, LOWER_HEIGHT, LOWER_RADIUS*sin(FACE_START_POINT) + FACE_INDENT / sin(FACE_START_POINT) * cos(FACE_START_POINT), cos(FACE_START_POINT)* LOWER_RADIUS, LOWER_HEIGHT, sin(FACE_START_POINT)*LOWER_RADIUS);
	
	if (ty != COSTUME_SABER_ALTER)setDiffuseColor(USE_COLOR_BODY);
	else setDiffuseColor(USE_COLOR_DARK_ARMOR);
	drawTriangle(0.0, 0.0, 0.0, FACE_BOTTOM, BOTTOM_RADIUS*cos(FACE_START_POINT), 0.0, BOTTOM_RADIUS*sin(FACE_START_POINT));
	drawTriangle(FACE_BOTTOM, BOTTOM_RADIUS*cos(FACE_START_POINT), 0.0, BOTTOM_RADIUS*sin(FACE_START_POINT), LOWER_RADIUS*cos(FACE_START_POINT), LOWER_HEIGHT, LOWER_RADIUS*sin(FACE_START_POINT));
	drawTriangle(cos(FACE_START_POINT)* LOWER_RADIUS, LOWER_HEIGHT, sin(FACE_START_POINT)*LOWER_RADIUS, LOWER_RADIUS*cos(FACE_START_POINT) - FACE_INDENT, LOWER_HEIGHT, LOWER_RADIUS*sin(FACE_START_POINT) + FACE_INDENT / sin(FACE_START_POINT) * cos(FACE_START_POINT), FACE_BOTTOM);
	drawTriangle(0.0, 0.0, 0.0, -BOTTOM_RADIUS*cos(FACE_START_POINT), 0.0, BOTTOM_RADIUS*sin(FACE_START_POINT), FACE_BOTTOM);
	drawTriangle(FACE_BOTTOM, -LOWER_RADIUS*cos(FACE_START_POINT), LOWER_HEIGHT, LOWER_RADIUS*sin(FACE_START_POINT), -BOTTOM_RADIUS*cos(FACE_START_POINT), 0.0, BOTTOM_RADIUS*sin(FACE_START_POINT));
	drawTriangle(-cos(FACE_START_POINT)* LOWER_RADIUS, LOWER_HEIGHT, sin(FACE_START_POINT)*LOWER_RADIUS, FACE_BOTTOM, -LOWER_RADIUS*cos(FACE_START_POINT) + FACE_INDENT, LOWER_HEIGHT, LOWER_RADIUS*sin(FACE_START_POINT) + FACE_INDENT / sin(FACE_START_POINT) * cos(FACE_START_POINT));

	if (ty != COSTUME_SABER_ALTER)setDiffuseColor(USE_COLOR_BODY);
	else setDiffuseColor(USE_COLOR_PALE);
	drawTriangle(-cos(FACE_START_POINT), 1.0, sin(FACE_START_POINT), -LOWER_RADIUS*cos(FACE_START_POINT) + FACE_INDENT, LOWER_HEIGHT, LOWER_RADIUS*sin(FACE_START_POINT) + FACE_INDENT / sin(FACE_START_POINT) * cos(FACE_START_POINT), -cos(FACE_START_POINT) + FACE_INDENT, 1.0, sin(FACE_START_POINT) + FACE_INDENT / sin(FACE_START_POINT) * cos(FACE_START_POINT));
	drawTriangle(-cos(FACE_START_POINT), 1.0, sin(FACE_START_POINT), -cos(FACE_START_POINT)* LOWER_RADIUS, LOWER_HEIGHT, sin(FACE_START_POINT)*LOWER_RADIUS, -LOWER_RADIUS*cos(FACE_START_POINT) + FACE_INDENT, LOWER_HEIGHT, LOWER_RADIUS*sin(FACE_START_POINT) + FACE_INDENT / sin(FACE_START_POINT) * cos(FACE_START_POINT));
	
	
	
	GLdouble upidZ = sin(FACE_START_POINT) + FACE_INDENT / sin(FACE_START_POINT) * cos(FACE_START_POINT);
	GLdouble lowidZ = LOWER_RADIUS*sin(FACE_START_POINT) + FACE_INDENT / sin(FACE_START_POINT) * cos(FACE_START_POINT);
	int divisions = 30;
	GLdouble upthis[2], lowthis[2], uplast[2], lowlast[2];
	upthis[0]=0.0;
	upthis[1] =  upidZ+ 0.2;
	lowthis[0] = 0.0;
	lowthis[1] = lowidZ+0.03 ;
	for (int i = 1; i <= divisions; i++){
		uplast[0] = upthis[0];
		uplast[1] = upthis[1];
		lowlast[0] = lowthis[0];
		lowlast[1] = lowthis[1];
		upthis[0] = (cos(FACE_START_POINT) - FACE_INDENT) / divisions*i;
		upthis[1] = upidZ + 0.2 - UP_LINE_HEIGHT(upthis[0] / (cos(FACE_START_POINT) - FACE_INDENT));
		lowthis[0] = (LOWER_RADIUS*cos(FACE_START_POINT) - FACE_INDENT) / divisions*i;
		lowthis[1] = lowidZ  +0.03 - LOW_LINE_HEIGHT(lowthis[0] / (LOWER_RADIUS*cos(FACE_START_POINT) - FACE_INDENT));

		if (ty != COSTUME_SABER_ALTER)setDiffuseColor(USE_COLOR_BODY);
		else setDiffuseColor(USE_COLOR_PALE);
		drawTriangle(uplast[0], 1.0, uplast[1], lowlast[0], LOWER_HEIGHT, lowlast[1], upthis[0], 1.0, upthis[1]);
		drawTriangle(upthis[0], 1.0, upthis[1], lowlast[0], LOWER_HEIGHT, lowlast[1], lowthis[0], LOWER_HEIGHT, lowthis[1]);
		drawTriangle(lowlast[0], LOWER_HEIGHT, lowlast[1], lowthis[0], LOWER_HEIGHT, lowthis[1], FACE_BOTTOM);
		drawTriangle(-uplast[0], 1.0, uplast[1], -upthis[0], 1.0, upthis[1], -lowlast[0], LOWER_HEIGHT, lowlast[1]);
		drawTriangle(-upthis[0], 1.0, upthis[1], -lowthis[0], LOWER_HEIGHT, lowthis[1], -lowlast[0], LOWER_HEIGHT, lowlast[1]);
		drawTriangle(-lowlast[0], LOWER_HEIGHT, lowlast[1], FACE_BOTTOM, -lowthis[0], LOWER_HEIGHT, lowthis[1]);

		if (ty != COSTUME_SABER_ALTER)setDiffuseColor(USE_COLOR_HAIR_GOLD);
		else setDiffuseColor(USE_COLOR_HAIR_DARK_GOLD);
		drawTriangle(-uplast[0], 1.0, uplast[1], 0.0, 1.0, 0.0, -upthis[0], 1.0, upthis[1]);
		drawTriangle(uplast[0], 1.0, uplast[1], upthis[0], 1.0, upthis[1], 0.0, 1.0, 0.0);

		if (!(i % 2)){
			double mp = (double)rand() / RAND_MAX *2.0 - 1.0;
			double ep = (double)rand() / RAND_MAX *4.0 - 2.0;
			if (ty != COSTUME_SABER_ALTER)setDiffuseColor(USE_COLOR_HAIR_GOLD);
			else setDiffuseColor(USE_COLOR_HAIR_DARK_GOLD);
			glPushMatrix();
			glTranslated(upthis[0], 1.05, upthis[1]+0.02);
			glScaled(-0.15, 0.4, 0.0);
			drawHair(mp, ep);
			glPopMatrix();
			mp = (double)rand() / RAND_MAX *2.0 - 1.0;
			ep = (double)rand() / RAND_MAX *4.0 - 2.0;
			glPushMatrix();
			glTranslated(-upthis[0], 1.05, upthis[1] + 0.02);
			glScaled(-0.1, 0.4, 0.1);
			drawHair(mp, ep);
			glPopMatrix();
		}
	}
	if (ty != COSTUME_SABER_ALTER)setDiffuseColor(USE_COLOR_HAIR_GOLD);
	else setDiffuseColor(USE_COLOR_HAIR_DARK_GOLD);
	drawTriangle(upthis[0], 1.0, upthis[1], cos(FACE_START_POINT), 1.0, sin(FACE_START_POINT), 0.0, 1.0, 0.0);
	drawTriangle(-upthis[0], 1.0, upthis[1], 0.0, 1.0, 0.0, -cos(FACE_START_POINT), 1.0, sin(FACE_START_POINT));

	if (ty != COSTUME_SABER_ALTER)setDiffuseColor(USE_COLOR_HAIR_GOLD);
	else setDiffuseColor(USE_COLOR_HAIR_DARK_GOLD);
	divisions = 80;
	for (int i = -divisions/10; i <= divisions+divisions/10; i++){
		GLdouble curAngle = (3.14159 + FACE_START_POINT * 2.0) / divisions * i + 3.14159 - FACE_START_POINT;
		GLdouble curX, curZ;
		curX = cos(curAngle);
		curZ = sin(curAngle);
		GLdouble rotY = (-curAngle*180.0 / 3.14159 + 90.0);
		glPushMatrix();
		glTranslated(curX*1.05, 1.05, curZ*1.05);
		glRotated(rotY, 0.0, 1.0, 0.0);
		if(curZ<0.3&&curZ>esp)glScaled(-0.15, 1.2, 0.1);
		else glScaled(-0.1, 0.7, 0.1);
		double mp = (double)rand() / RAND_MAX *1.0 - 0.5;
		double ep = (double)rand() / RAND_MAX *2.0 - 1.0;
		drawHair(mp, ep);
		glPopMatrix();
	}
	//Top hair
	divisions = 80;
	for (int i = 0; i <= divisions; i++){
		GLdouble curAngle = (3.14159 *2) / divisions * i;
		glPushMatrix(); 
		GLdouble curX, curZ;
		curX = cos(curAngle);
		curZ = sin(curAngle);
		glTranslated(curZ*1.05, 1.05, curX*1.05);
		glRotated(curAngle*180.0 / 3.14159, 0.0, 1.0, 0.0);
		glRotated(105.0, 1.0, 0.0, 0.0);
		glScaled(0.1, 1.2, 0.1);
		double mp = (double)rand() / RAND_MAX *1.0 - 0.5;
		double ep = (double)rand() / RAND_MAX *2.0 - 1.0;
		drawHair(mp, ep);
		glPopMatrix();
	}
	
	glPushMatrix();
	glTranslated(0.0, 1.0, 0.0);
	glRotated(-90, 1.0, 0.0, 0.0);
	drawCylinder(0.3, 1.0, 0);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.23, 0.5, 0.87);
	glRotated(10.0, 1.0, 0.0, 0.0);
	glRotated(-10.0,0.0, 1.0, 0.0);
	glScaled(0.04, 0.03, 1.0);
	drawEye(1,ty);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.23, 0.5, 0.87);
	glRotated(10.0, 1.0, 0.0, 0.0);
	glRotated(10.0, 0.0, 1.0, 0.0);
	glScaled(0.04, 0.03, 1.0);
	drawEye(0,ty);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.0, 0.3, 0.77);
	glScaled(0.07, 0.05, 0.1);
	glRotated(10.0, 1.0, 0.0, 0.0);
	drawNose();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.0, 0.2, 0.67);
	glScaled(0.05, 0.01, 0.05);
	drawMouth();
	glPopMatrix();

	//VERY IMPORTANT!
	if (ty != COSTUME_SABER_ALTER){
		setDiffuseColor(USE_COLOR_HAIR_GOLD);
		double mp = (double)rand() / RAND_MAX *1.0 - 0.5;
		double ep = (double)rand() / RAND_MAX *2.0 - 1.0;
		glPushMatrix();
		glTranslated(0.0, 1.01, 0.8);
		glRotated(120, 0.0, 0.0, 1.0);
		glScaled(-0.1, 0.8, 0.1);
		drawHair(mp,ep);
		glPopMatrix();
	}
}

void drawSwordGuard(){
	glPushMatrix();
	glTranslated(-1.0, 0.0, -1.0);
	glRotated(-30.0, 0.0, 0.0, 1.0);
	drawBox(-2.0, 1.0, 2.0);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1.0, 0.0, -1.0);
	glRotated(30.0, 0.0, 0.0, 1.0);
	drawBox(2.0, 1.0, 2.0);
	glPopMatrix();

	drawTriangle(-1.0, 0.0, -1.0, -0.5, 0.866, -1.0, 1.0, 0.0, -1.0);
	drawTriangle(1.0, 0.0, -1.0, -0.5, 0.866, -1.0, 0.5, 0.866, -1.0);

	drawTriangle(-1.0, 0.0, 1.0, 1.0, 0.0, 1.0, -0.5, 0.866, 1.0);
	drawTriangle(1.0, 0.0, 1.0, 0.5, 0.866, 1.0, -0.5, 0.866, 1.0);

	drawTriangle(0.5, 0.866, 1.0, -0.5, 0.866, -1.0, -0.5, 0.866, 1.0);
	drawTriangle(0.5, 0.866, 1.0, 0.5, 0.866, -1.0, -0.5, 0.866, -1.0);

	drawTriangle(1.0, 0.0, 1.0, -1.0, 0.0, 1.0, -1.0, 0.0, -1.0);
	drawTriangle(1.0, 0.0, 1.0, -1.0, 0.0, -1.0, 1.0, 0.0, -1.0);
}

ModelNode::ModelNode(){
	childHead = brotherNext = NULL;
	disabled = false;
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

void ModelNode::setHeadType(int ty){
	headType = ty;
}

void ModelNode::setStartAndEndAngle(GLdouble theStartAngle, GLdouble theEndAngle){
	startAngle = theStartAngle;
	endAngle = theEndAngle;
}
void ModelNode::enableNode(){
	disabled = false;
}
void ModelNode::disableNode(){
	disabled = true;
}
void ModelNode::Render(){
	if (disabled)return;
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
	case SHAPE_TORSO_HALF_LINEAR:
		glRotated(90.0, 1.0, 0.0, 0.0);
		glTranslated(transX, transZ, transY);
		glScaled(xScale, zScale, yScale / abs(yScale));
		drawTorsoHalfLinear(abs(yScale), 1.0, upperScale, middleScale, middleRatio,0.1);
		break;
	case SHAPE_BLADE:
		glTranslated(transX, transY, transZ);
		glScaled(xScale, yScale, zScale);
		drawBlade(swordType);
		break;
	case SHAPE_PARTIAL_CYLINDER:
		glRotated(90.0, 1.0, 0.0, 0.0);
		glTranslated(transX, transZ, transY);
		glScaled(xScale, zScale, yScale / abs(yScale));
		drawPartialCylinder(abs(yScale), 1.0, upperScale,startAngle,endAngle);
		break;
	case SHAPE_FOOT:
		glTranslated(transX, transY, transZ);
		glScaled(xScale, yScale, zScale);
		drawFoot();
		break;
	case SHAPE_HEAD:
		glTranslated(transX, transY, transZ);
		glScaled(xScale, yScale, zScale);
		drawHead(headType);
		break;
	case SHAPE_GUARD:
		glTranslated(transX, transY, transZ);
		glScaled(xScale, yScale, zScale);
		drawSwordGuard();
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
	upperTorso.nodeCreate(NULL, SHAPE_TORSO_HALF_LINEAR);
	rightUpperArm.nodeCreate(&upperTorso, SHAPE_TORSO);
	rightLowerArm.nodeCreate(&rightUpperArm, PRIMITIVE_CYLINDER);
	rightShoulder.nodeCreate(&rightUpperArm, SHAPE_TORSO_LINEAR);
	leftUpperArm.nodeCreate(&upperTorso, SHAPE_TORSO);
	leftLowerArm.nodeCreate(&leftUpperArm, PRIMITIVE_CYLINDER);
	leftShoulder.nodeCreate(&leftUpperArm, SHAPE_TORSO_LINEAR);
	excaliburGrip.nodeCreate(&rightHand, SHAPE_TORSO);
	excaliburGuard.nodeCreate(&excaliburGrip, SHAPE_GUARD);
	excaliburBlade.nodeCreate(&excaliburGuard, SHAPE_BLADE);
	lowerTorso.nodeCreate(&upperTorso, PRIMITIVE_CYLINDER_NO_DISK);
	head.nodeCreate(&upperTorso, SHAPE_HEAD);
	leftUpperLeg.nodeCreate(&lowerTorso, SHAPE_TORSO);
	leftLowerLeg.nodeCreate(&leftUpperLeg, SHAPE_TORSO);
	rightUpperLeg.nodeCreate(&lowerTorso, SHAPE_TORSO);
	rightLowerLeg.nodeCreate(&rightUpperLeg, SHAPE_TORSO);
	leftFoot.nodeCreate(&leftLowerLeg, SHAPE_FOOT);
	rightFoot.nodeCreate(&rightLowerLeg, SHAPE_FOOT);
	leftHand.nodeCreate(&leftLowerArm, SHAPE_TORSO);
	rightHand.nodeCreate(&rightLowerArm, SHAPE_TORSO);

	lowerArmor[0].nodeCreate(&lowerTorso, SHAPE_PARTIAL_CYLINDER);
	lowerArmor[1].nodeCreate(&lowerArmor[0], SHAPE_PARTIAL_CYLINDER);
	lowerArmor[2].nodeCreate(&lowerArmor[1], SHAPE_PARTIAL_CYLINDER); 
	lowerArmor[3].nodeCreate(&lowerTorso, SHAPE_PARTIAL_CYLINDER);
	lowerArmor[4].nodeCreate(&lowerArmor[3], SHAPE_PARTIAL_CYLINDER);
	lowerArmor[5].nodeCreate(&lowerArmor[4], SHAPE_PARTIAL_CYLINDER);
	lowerFront[0].nodeCreate(&lowerTorso, SHAPE_PARTIAL_CYLINDER);
	lowerFront[1].nodeCreate(&lowerTorso, SHAPE_PARTIAL_CYLINDER);
	lowerOuter[0].nodeCreate(&lowerTorso, SHAPE_PARTIAL_CYLINDER);
	lowerOuter[1].nodeCreate(&lowerTorso, SHAPE_PARTIAL_CYLINDER);
	lowerOuter[2].nodeCreate(&lowerTorso, SHAPE_PARTIAL_CYLINDER);
	lowerOuter[3].nodeCreate(&lowerTorso, SHAPE_PARTIAL_CYLINDER);
	lowerOuter[4].nodeCreate(&lowerTorso, SHAPE_PARTIAL_CYLINDER);
	lowerOuter[5].nodeCreate(&lowerTorso, SHAPE_PARTIAL_CYLINDER);
	treeRoot = &upperTorso;

	animation = 0;
	step = 0;
	redLight = false;
}

void SaberModel::CostumeSaber(){

	leftShoulder.nodeCreate(NULL, SHAPE_TORSO_LINEAR);
	rightShoulder.nodeCreate(NULL, SHAPE_TORSO_LINEAR);

	upperTorso.setAngle(0.0, 0.0, 0.0);
	upperTorso.setColor(USE_COLOR_SILVER);
	upperTorso.setScale(0.32, 2.5, 0.2);
	upperTorso.setStartPos(0.0, 0.0, 0.0);
	upperTorso.setTrans(0.0, 0, 0.0);//center at neck
	upperTorso.cylinderScale(2.5,3.5,0.4);

	lowerTorso.setAngle(0.0, 0.0, 0.0);
	lowerTorso.setColor(USE_COLOR_WHITE);
	lowerTorso.setScale(0.75, 4.5, 0.47);
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
	leftLowerArm.setScale(0.5, 1.8, 0.5);
	leftLowerArm.setStartPos(0.0, -2.0, 0.0);
	leftLowerArm.setTrans(0.0, 0.0, 0.0);
	leftLowerArm.cylinderScale(0.6, 1.0, 0.5);

	leftHand.setAngle(0.0, 0.0, 0.0);
	leftHand.setColor(USE_COLOR_SILVER);
	leftHand.setScale(0.3, 0.5, 0.3);
	leftHand.setStartPos(0.0, -1.8, 0.0);
	leftHand.setTrans(0.0, 0.0, 0.0);
	leftHand.cylinderScale(0.6, 0.8, 0.7);

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
	rightLowerArm.setScale(0.5, 1.8, 0.5);
	rightLowerArm.setStartPos(0.0, -2.0, 0.0);
	rightLowerArm.setTrans(0.0, 0.0, 0.0);
	rightLowerArm.cylinderScale(0.6, 1.0, 0.5); 
	
	rightHand.setAngle(0.0, 0.0, 0.0);
	rightHand.setColor(USE_COLOR_SILVER);
	rightHand.setScale(0.3, 0.5, 0.3);
	rightHand.setStartPos(0.0, -1.8, 0.0);
	rightHand.setTrans(0.0, 0.0, 0.0);
	rightHand.cylinderScale(0.6, 0.8, 0.7);


	rightShoulder.setAngle(0.0, 0.0, 15.0);
	rightShoulder.setColor(USE_COLOR_BLUE);
	rightShoulder.setScale(0.4, 1.0, 0.4);
	rightShoulder.setStartPos(0.0, 0.0, 0.0);
	rightShoulder.setTrans(0.0, -0.8, 0.0);
	rightShoulder.cylinderScale(1.0, 1.5, 0.4);

	excaliburGuard.setAngle(0.0, 0.0, 0.0);
	excaliburGuard.setColor(USE_COLOR_GOLD);
	excaliburGuard.setScale(0.3, -0.2, 0.06);
	excaliburGuard.setStartPos(0.0, 0.0, 0.0);
	excaliburGuard.setTrans(0.0, 0.0, 0.0);

	excaliburBlade.setAngle(0.0, 0.0, 0.0);
	excaliburBlade.setColor(USE_COLOR_EXCALIBUR);
	excaliburBlade.setScale(0.3, -5.0, 0.04);
	excaliburBlade.setStartPos(0.0, -0.17, 0.0);
	excaliburBlade.setTrans(0.0, 0.0, 0.0);
	excaliburBlade.setSwordType(TYPE_EXCALIBUR);

	excaliburGrip.setAngle(0.0, 0.0, 0.0);
	excaliburGrip.setColor(USE_COLOR_BLUE);
	excaliburGrip.setScale(0.07, -1.0, 0.07);
	excaliburGrip.setStartPos(0.0, -0.5, 0.0);
	excaliburGrip.setTrans(0.0, 0.0, 0.0);
	excaliburGrip.cylinderScale(0.8, 0.5, 0.5);

	head.setAngle(0.0, 215.0, 0.0);
	head.setColor(USE_COLOR_BODY);
	head.setScale(0.8, 1.4, 0.8);
	head.setStartPos(0.0, 0.0, 0.0);
	head.setTrans(0.0, 0.1, 0.0);
	head.setHeadType(COSTUME_SABER);

	leftUpperLeg.setAngle(0.0, 0.0, -10.0);
	leftUpperLeg.setColor(USE_COLOR_BLUE);
	leftUpperLeg.setScale(0.4, 2.8, 0.4);
	leftUpperLeg.setStartPos(-0.4, -0.4, 0.0);
	leftUpperLeg.setTrans(0.0, 0.0, 0.0);
	leftUpperLeg.cylinderScale(0.75, 1.0, 0.6);

	rightUpperLeg.setAngle(0.0, 0.0, 10.0);
	rightUpperLeg.setColor(USE_COLOR_BLUE);
	rightUpperLeg.setScale(0.4, 2.8, 0.4);
	rightUpperLeg.setStartPos(0.4, -0.4, 0.0);
	rightUpperLeg.setTrans(0.0, 0.0, 0.0);
	rightUpperLeg.cylinderScale(0.75, 1.0, 0.6);

	leftLowerLeg.setAngle(0.0, 0.0, 10.0);
	leftLowerLeg.setColor(USE_COLOR_SILVER);
	leftLowerLeg.setScale(0.35, 1.5, 0.35);
	leftLowerLeg.setStartPos(0.0, -2.8, 0.0);
	leftLowerLeg.setTrans(0.0, 0.0, 0.0);
	leftLowerLeg.cylinderScale(0.8, 0.9, 0.6);

	rightLowerLeg.setAngle(0.0, 0.0, -10.0);
	rightLowerLeg.setColor(USE_COLOR_SILVER);
	rightLowerLeg.setScale(0.35, 1.5, 0.35);
	rightLowerLeg.setStartPos(0.0, -2.8, 0.0);
	rightLowerLeg.setTrans(0.0, 0.0, 0.0);
	rightLowerLeg.cylinderScale(0.8, 0.9, 0.6);

	leftFoot.setAngle(0.0, 180.0, 0.0);
	leftFoot.setColor(USE_COLOR_SILVER);
	leftFoot.setScale(0.8, 0.7, 0.8);
	leftFoot.setStartPos(0.0, -2.5, 0.0);
	leftFoot.setTrans(0.0, 0.0, 0.0);

	rightFoot.setAngle(0.0, 180.0, 0.0);
	rightFoot.setColor(USE_COLOR_SILVER);
	rightFoot.setScale(0.8, 0.7, 0.8);
	rightFoot.setStartPos(0.0, -2.5, 0.0);
	rightFoot.setTrans(0.0, 0.0, 0.0);


	
	lowerArmor[0].setAngle(0.0, 0.0, 0.0);
	lowerArmor[0].setColor(USE_COLOR_SILVER);
	lowerArmor[0].setScale(0.8, 1.0, 0.5);
	lowerArmor[0].setStartPos(0.0, 0.0, 0.0);
	lowerArmor[0].setTrans(0.0, 0.0, 0.0);//center at weist
	lowerArmor[0].cylinderScale(1.8, 1.0, 0.5);
	lowerArmor[0].setStartAndEndAngle(-70, 70);

	lowerArmor[1].setAngle(0.0, 0.0, 0.0);
	lowerArmor[1].setColor(USE_COLOR_SILVER);
	lowerArmor[1].setScale(1.5, 1.0, 0.9);
	lowerArmor[1].setStartPos(0.0, -1.05, 0.0);
	lowerArmor[1].setTrans(0.0, 0.0, 0.0);
	lowerArmor[1].cylinderScale(1.4, 1.0, 0.5);
	lowerArmor[1].setStartAndEndAngle(-70, 70);

	lowerArmor[2].setAngle(0.0, 0.0, 0.0);
	lowerArmor[2].setColor(USE_COLOR_SILVER);
	lowerArmor[2].setScale(2.1, 1.0, 1.26);
	lowerArmor[2].setStartPos(0.0, -1.05, 0.0);
	lowerArmor[2].setTrans(0.0, 0.0, 0.0);
	lowerArmor[2].cylinderScale(1.3, 1.0, 0.5);
	lowerArmor[2].setStartAndEndAngle(-70, 70);

	lowerArmor[3].setAngle(0.0, 0.0, 0.0);
	lowerArmor[3].setColor(USE_COLOR_SILVER);
	lowerArmor[3].setScale(0.8, 1.0, 0.5);
	lowerArmor[3].setStartPos(0.0, 0.0, 0.0);
	lowerArmor[3].setTrans(0.0, 0.0, 0.0);//center at weist
	lowerArmor[3].cylinderScale(1.8, 1.0, 0.5);
	lowerArmor[3].setStartAndEndAngle(110, 250);

	lowerArmor[4].setAngle(0.0, 0.0, 0.0);
	lowerArmor[4].setColor(USE_COLOR_SILVER);
	lowerArmor[4].setScale(1.5, 1.0, 0.9);
	lowerArmor[4].setStartPos(0.0, -1.05, 0.0);
	lowerArmor[4].setTrans(0.0, 0.0, 0.0);
	lowerArmor[4].cylinderScale(1.4, 1.0, 0.5);
	lowerArmor[4].setStartAndEndAngle(110, 250);

	lowerArmor[5].setAngle(0.0, 0.0, 0.0);
	lowerArmor[5].setColor(USE_COLOR_SILVER);
	lowerArmor[5].setScale(2.1, 1.0, 1.26);
	lowerArmor[5].setStartPos(0.0, -1.05, 0.0);
	lowerArmor[5].setTrans(0.0, 0.0, 0.0);
	lowerArmor[5].cylinderScale(1.3, 1.0, 0.5);
	lowerArmor[5].setStartAndEndAngle(110, 250);

	lowerFront[0].enableNode();

	lowerFront[0].setAngle(0.0, 0.0, 0.0);
	lowerFront[0].setColor(USE_COLOR_LIGHT_BLUE);
	lowerFront[0].setScale(0.8, 3.5, 0.5);
	lowerFront[0].setStartPos(0.0, 0.0, 0.0);
	lowerFront[0].setTrans(0.0, 0.0, 0.0);
	lowerFront[0].cylinderScale(3.5, 1.0, 0.5);
	lowerFront[0].setStartAndEndAngle(250, 290);
	
	lowerFront[1].disableNode();

	lowerOuter[0].setAngle(0.0, 0.0, 0.0);
	lowerOuter[0].setColor(USE_COLOR_BLUE);
	lowerOuter[0].setScale(0.8, 4.3, 0.5);
	lowerOuter[0].setStartPos(0.0, 0.0, 0.0);
	lowerOuter[0].setTrans(0.0, 0.0, 0.0);
	lowerOuter[0].cylinderScale(3.7, 1.0, 0.5);
	lowerOuter[0].setStartAndEndAngle(-75, 85);

	lowerOuter[1].enableNode();
	lowerOuter[2].enableNode();

	lowerOuter[1].setAngle(0.0, 0.0, 0.0);
	lowerOuter[1].setColor(USE_COLOR_BLUE);
	lowerOuter[1].setScale(0.8, 3.8, 0.5);
	lowerOuter[1].setStartPos(0.0, 0.0, 0.0);
	lowerOuter[1].setTrans(0.0, 0.0, 0.0);
	lowerOuter[1].cylinderScale(3.4, 1.0, 0.5);
	lowerOuter[1].setStartAndEndAngle(80,100);

	lowerOuter[2].setAngle(0.0, 0.0, 0.0);
	lowerOuter[2].setColor(USE_COLOR_BLUE);
	lowerOuter[2].setScale(0.8, 4.3, 0.5);
	lowerOuter[2].setStartPos(0.0, 0.0, 0.0);
	lowerOuter[2].setTrans(0.0, 0.0, 0.0);
	lowerOuter[2].cylinderScale(3.7, 1.0, 0.5);
	lowerOuter[2].setStartAndEndAngle(95, 255);

	lowerOuter[3].disableNode();
	lowerOuter[4].disableNode();
	lowerOuter[5].disableNode();
	
}
void SaberModel::CostumeSaberAlter(){
	CostumeSaber();
	excaliburBlade.setSwordType(TYPE_EXCALIBUR_MORGAN);
	head.setHeadType(COSTUME_SABER_ALTER);

	upperTorso.setColor(USE_COLOR_DARK_ARMOR);
	leftUpperArm.setColor(USE_COLOR_DARK_CLOTHES);
	leftLowerArm.setColor(USE_COLOR_DARK_ARMOR);
	rightUpperArm.setColor(USE_COLOR_DARK_CLOTHES);
	rightLowerArm.setColor(USE_COLOR_DARK_ARMOR);
	leftUpperLeg.setColor(USE_COLOR_DARK_CLOTHES);
	leftLowerLeg.setColor(USE_COLOR_DARK_ARMOR);
	rightUpperLeg.setColor(USE_COLOR_DARK_CLOTHES);
	rightLowerLeg.setColor(USE_COLOR_DARK_ARMOR);
	leftShoulder.setColor(USE_COLOR_DARK_CLOTHES);
	rightShoulder.setColor(USE_COLOR_DARK_CLOTHES);
	leftHand.setColor(USE_COLOR_DARK_ARMOR);
	rightHand.setColor(USE_COLOR_DARK_ARMOR);
	leftFoot.setColor(USE_COLOR_DARK_ARMOR);
	rightFoot.setColor(USE_COLOR_DARK_ARMOR);
	excaliburGrip.setColor(USE_COLOR_DARK);
	excaliburGuard.setColor(USE_COLOR_DARK);
	leftShoulder.cylinderScale(1.0, 1.8, 0.4);
	rightShoulder.cylinderScale(1.0, 1.8, 0.4);

	lowerTorso.setAngle(0.0, 0.0, 0.0);
	lowerTorso.setColor(USE_COLOR_BLACK_CLOTHES);
	lowerTorso.setScale(0.75, 4.0, 0.47);
	lowerTorso.setStartPos(0.0, -2.5, 0.0);
	lowerTorso.setTrans(0.0, 0.0, 0.0);//center at weist
	lowerTorso.cylinderScale(2.7, 1.0, 0.5);

	lowerArmor[0].setAngle(0.0, 0.0, 0.0);
	lowerArmor[0].setColor(USE_COLOR_DARK_ARMOR);
	lowerArmor[0].setScale(0.8, 0.7, 0.5);
	lowerArmor[0].setStartPos(0.0, 0.0, 0.0);
	lowerArmor[0].setTrans(0.0, 0.0, 0.0);//center at weist
	lowerArmor[0].cylinderScale(1.4, 1.0, 0.5);
	lowerArmor[0].setStartAndEndAngle(-70, 70);

	lowerArmor[1].setAngle(0.0, 0.0, 0.0);
	lowerArmor[1].setColor(USE_COLOR_DARK_ARMOR);
	lowerArmor[1].setScale(1.15, 0.7, 0.7);
	lowerArmor[1].setStartPos(0.0, -0.75, 0.0);
	lowerArmor[1].setTrans(0.0, 0.0, 0.0);
	lowerArmor[1].cylinderScale(1.3, 1.0, 0.5);
	lowerArmor[1].setStartAndEndAngle(-70, 70);

	lowerArmor[2].setAngle(0.0, 0.0, 0.0);
	lowerArmor[2].setColor(USE_COLOR_DARK_ARMOR);
	lowerArmor[2].setScale(1.5, 0.7, 0.9);
	lowerArmor[2].setStartPos(0.0, -0.75, 0.0);
	lowerArmor[2].setTrans(0.0, 0.0, 0.0);
	lowerArmor[2].cylinderScale(1.25, 1.0, 0.5);
	lowerArmor[2].setStartAndEndAngle(-70, 70);

	lowerArmor[3].setAngle(0.0, 0.0, 0.0);
	lowerArmor[3].setColor(USE_COLOR_DARK_ARMOR);
	lowerArmor[3].setScale(0.8, 0.7, 0.5);
	lowerArmor[3].setStartPos(0.0, 0.0, 0.0);
	lowerArmor[3].setTrans(0.0, 0.0, 0.0);//center at weist
	lowerArmor[3].cylinderScale(1.4, 1.0, 0.5);
	lowerArmor[3].setStartAndEndAngle(110, 250);

	lowerArmor[4].setAngle(0.0, 0.0, 0.0);
	lowerArmor[4].setColor(USE_COLOR_DARK_ARMOR);
	lowerArmor[4].setScale(1.15, 0.7, 0.7);
	lowerArmor[4].setStartPos(0.0, -0.75, 0.0);
	lowerArmor[4].setTrans(0.0, 0.0, 0.0);
	lowerArmor[4].cylinderScale(1.3, 1.0, 0.5);
	lowerArmor[4].setStartAndEndAngle(110, 250);

	lowerArmor[5].setAngle(0.0, 0.0, 0.0);
	lowerArmor[5].setColor(USE_COLOR_DARK_ARMOR);
	lowerArmor[5].setScale(1.5, 0.7, 0.9);
	lowerArmor[5].setStartPos(0.0, -0.75, 0.0);
	lowerArmor[5].setTrans(0.0, 0.0, 0.0);
	lowerArmor[5].cylinderScale(1.25, 1.0, 0.5);
	lowerArmor[5].setStartAndEndAngle(110, 250);

	lowerFront[0].disableNode();

	lowerOuter[0].setAngle(0.0, 0.0, 0.0);
	lowerOuter[0].setColor(USE_COLOR_DARK_CLOTHES);
	lowerOuter[0].setScale(0.8, 4.5, 0.5);
	lowerOuter[0].setStartPos(0.0, 0.0, 0.0);
	lowerOuter[0].setTrans(0.0, 0.0, 0.0);
	lowerOuter[0].cylinderScale(3.3, 1.0, 0.5);
	lowerOuter[0].setStartAndEndAngle(-75, 255);


	lowerFront[1].disableNode();

	lowerOuter[1].disableNode();
	lowerOuter[2].disableNode();
	lowerOuter[3].disableNode();
	lowerOuter[4].disableNode();
	lowerOuter[5].disableNode();
}
void SaberModel::CostumeSaberLily(){
	CostumeSaber();
	excaliburBlade.setSwordType(TYPE_CALIBURN);
	head.setHeadType(COSTUME_SABER_LILY);
	
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
	leftShoulder.setColor(USE_COLOR_BODY);
	rightShoulder.setColor(USE_COLOR_BODY);
	leftHand.setColor(USE_COLOR_SILVER);
	rightHand.setColor(USE_COLOR_SILVER);
	leftFoot.setColor(USE_COLOR_SILVER);
	rightFoot.setColor(USE_COLOR_SILVER);
	excaliburGuard.setColor(USE_COLOR_DARK_GOLD);
	leftShoulder.setScale(0.3, 1.0, 0.3);
	rightShoulder.setScale(0.3, 1.0, 0.3);
	leftShoulder.cylinderScale(1.05, 1.2, 0.4);
	rightShoulder.cylinderScale(1.05, 1.2, 0.4);

	lowerTorso.setAngle(0.0, 0.0, 0.0);
	lowerTorso.setColor(USE_COLOR_WHITE);
	lowerTorso.setScale(0.75, 4.5, 0.47);
	lowerTorso.setStartPos(0.0, -2.5, 0.0);
	lowerTorso.setTrans(0.0, 0.0, 0.0);//center at weist
	lowerTorso.cylinderScale(4.0, 1.0, 0.5);


	lowerFront[0].enableNode();
	lowerFront[1].enableNode();

	lowerOuter[1].enableNode();
	lowerOuter[2].enableNode();
	lowerOuter[3].enableNode();
	lowerOuter[4].enableNode();
	lowerOuter[5].enableNode();

	lowerArmor[0].setAngle(0.0, 0.0, 0.0);
	lowerArmor[0].setColor(USE_COLOR_SILVER);
	lowerArmor[0].setScale(0.8, 0.8, 0.5);
	lowerArmor[0].setStartPos(0.0, 0.0, 0.0);
	lowerArmor[0].setTrans(0.0, 0.0, 0.0);//center at weist
	lowerArmor[0].cylinderScale(2.3, 1.0, 0.5);
	lowerArmor[0].setStartAndEndAngle(-50, 50);

	lowerArmor[1].setAngle(0.0, 0.0, 0.0);
	lowerArmor[1].setColor(USE_COLOR_SILVER);
	lowerArmor[1].setScale(1.0, 0.8, 0.6);
	lowerArmor[1].setStartPos(0.0, -0.5, 0.0);
	lowerArmor[1].setTrans(0.0, 0.0, 0.0);
	lowerArmor[1].cylinderScale(2.0, 1.0, 0.5);
	lowerArmor[1].setStartAndEndAngle(-50, 50);

	lowerArmor[2].setAngle(0.0, 0.0, 0.0);
	lowerArmor[2].setColor(USE_COLOR_SILVER);
	lowerArmor[2].setScale(1.3, 0.8, 0.7);
	lowerArmor[2].setStartPos(0.0, -0.5, 0.0);
	lowerArmor[2].setTrans(0.0, 0.0, 0.0);
	lowerArmor[2].cylinderScale(1.8, 1.0, 0.5);
	lowerArmor[2].setStartAndEndAngle(-50, 50);

	lowerArmor[3].setAngle(0.0, 0.0, 0.0);
	lowerArmor[3].setColor(USE_COLOR_SILVER);
	lowerArmor[3].setScale(0.8, 0.8, 0.5);
	lowerArmor[3].setStartPos(0.0, 0.0, 0.0);
	lowerArmor[3].setTrans(0.0, 0.0, 0.0);//center at weist
	lowerArmor[3].cylinderScale(2.3, 1.0, 0.5);
	lowerArmor[3].setStartAndEndAngle(130, 230);

	lowerArmor[4].setAngle(0.0, 0.0, 0.0);
	lowerArmor[4].setColor(USE_COLOR_SILVER);
	lowerArmor[4].setScale(1.0, 0.8, 0.6);
	lowerArmor[4].setStartPos(0.0, -0.5, 0.0);
	lowerArmor[4].setTrans(0.0, 0.0, 0.0);
	lowerArmor[4].cylinderScale(2.0, 1.0, 0.5);
	lowerArmor[4].setStartAndEndAngle(130, 230);

	lowerArmor[5].setAngle(0.0, 0.0, 0.0);
	lowerArmor[5].setColor(USE_COLOR_SILVER);
	lowerArmor[5].setScale(1.3, 0.8, 0.7);
	lowerArmor[5].setStartPos(0.0, -0.5, 0.0);
	lowerArmor[5].setTrans(0.0, 0.0, 0.0);
	lowerArmor[5].cylinderScale(1.8, 1.0, 0.5);
	lowerArmor[5].setStartAndEndAngle(130, 230);

	lowerFront[0].setAngle(0.0, 0.0, 0.0);
	lowerFront[0].setColor(USE_COLOR_BLACK);
	lowerFront[0].setScale(0.8, 1.8, 0.5);
	lowerFront[0].setStartPos(0.0, 0.0, 0.0);
	lowerFront[0].setTrans(0.0, 0.0, 0.0);
	lowerFront[0].cylinderScale(2.4, 1.0, 0.5);
	lowerFront[0].setStartAndEndAngle(250, 265);

	lowerFront[1].setAngle(0.0, 0.0, 0.0);
	lowerFront[1].setColor(USE_COLOR_BLACK);
	lowerFront[1].setScale(0.8, 1.8, 0.5);
	lowerFront[1].setStartPos(0.0, 0.0, 0.0);
	lowerFront[1].setTrans(0.0, 0.0, 0.0);
	lowerFront[1].cylinderScale(2.4, 1.0, 0.5);
	lowerFront[1].setStartAndEndAngle(275, 290);

	lowerOuter[0].setAngle(0.0, 0.0, 0.0);
	lowerOuter[0].setColor(USE_COLOR_SILVER);
	lowerOuter[0].setScale(0.8, 3.8, 0.5);
	lowerOuter[0].setStartPos(0.0, 0.0, 0.0);
	lowerOuter[0].setTrans(0.0, 0.0, 0.0);
	lowerOuter[0].cylinderScale(3.6, 1.0, 0.5);
	lowerOuter[0].setStartAndEndAngle(-75, -45);

	lowerOuter[1].setAngle(0.0, 0.0, 0.0);
	lowerOuter[1].setColor(USE_COLOR_SILVER);
	lowerOuter[1].setScale(0.8, 4.2, 0.5);
	lowerOuter[1].setStartPos(0.0, 0.0, 0.0);
	lowerOuter[1].setTrans(0.0, 0.0, 0.0);
	lowerOuter[1].cylinderScale(3.8, 1.0, 0.5);
	lowerOuter[1].setStartAndEndAngle(-35, 35);

	lowerOuter[2].setAngle(0.0, 0.0, 0.0);
	lowerOuter[2].setColor(USE_COLOR_SILVER);
	lowerOuter[2].setScale(0.8, 3.8, 0.5);
	lowerOuter[2].setStartPos(0.0, 0.0, 0.0);
	lowerOuter[2].setTrans(0.0, 0.0, 0.0);
	lowerOuter[2].cylinderScale(3.6, 1.0, 0.5);
	lowerOuter[2].setStartAndEndAngle(45,75);

	lowerOuter[3].setAngle(0.0, 0.0, 0.0);
	lowerOuter[3].setColor(USE_COLOR_SILVER);
	lowerOuter[3].setScale(0.8, 3.8, 0.5);
	lowerOuter[3].setStartPos(0.0, 0.0, 0.0);
	lowerOuter[3].setTrans(0.0, 0.0, 0.0);
	lowerOuter[3].cylinderScale(3.6, 1.0, 0.5);
	lowerOuter[3].setStartAndEndAngle(105, 135);

	lowerOuter[4].setAngle(0.0, 0.0, 0.0);
	lowerOuter[4].setColor(USE_COLOR_SILVER);
	lowerOuter[4].setScale(0.8, 4.2, 0.5);
	lowerOuter[4].setStartPos(0.0, 0.0, 0.0);
	lowerOuter[4].setTrans(0.0, 0.0, 0.0);
	lowerOuter[4].cylinderScale(3.8, 1.0, 0.5);
	lowerOuter[4].setStartAndEndAngle(145, 215);

	lowerOuter[5].setAngle(0.0, 0.0, 0.0);
	lowerOuter[5].setColor(USE_COLOR_SILVER);
	lowerOuter[5].setScale(0.8, 3.8, 0.5);
	lowerOuter[5].setStartPos(0.0, 0.0, 0.0);
	lowerOuter[5].setTrans(0.0, 0.0, 0.0);
	lowerOuter[5].cylinderScale(3.6, 1.0, 0.5);
	lowerOuter[5].setStartAndEndAngle(225, 255);

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