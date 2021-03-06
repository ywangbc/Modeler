#include <windows.h>
#include <Fl/gl.h>
#include <gl/glu.h>
#include <cmath>

#include "camera.h"

#pragma warning(push)
#pragma warning(disable : 4244)

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502
#endif 

const float kMouseRotationSensitivity		= 1.0f/90.0f;
const float kMouseTranslationXSensitivity	= 0.03f;
const float kMouseTranslationYSensitivity	= 0.03f;
const float kMouseZoomSensitivity			= 0.08f;

void MakeDiagonal(Mat4f &m, float k)
{
	register int i,j;

	for (i=0; i<4; i++)
		for (j=0; j<4; j++)
			m[i][j] = (i==j) ? k : 0.0f;
}

void MakeHScale(Mat4f &m, const Vec3f &s)	
{
	MakeDiagonal(m,1.0f);
	m[0][0] = s[0]; m[1][1] = s[1];	m[2][2] = s[2];
}

void MakeHTrans(Mat4f &m, const Vec3f &s)
{
	MakeDiagonal(m,1.0f);
	m[0][3] = s[0]; m[1][3] = s[1]; m[2][3] = s[2];
}

void MakeHRotX(Mat4f &m, float theta)
{
	MakeDiagonal(m,1.0f);
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[1][1] = cosTheta;
	m[1][2] = -sinTheta;
	m[2][1] = sinTheta;
	m[2][2] = cosTheta;
}

void MakeHRotY(Mat4f &m, float theta)
{
	MakeDiagonal(m,1.0f);
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[0][0] = cosTheta;
	m[2][0] = -sinTheta;
	m[0][2] = sinTheta;
	m[2][2] = cosTheta;
}

void MakeHRotZ(Mat4f &m, float theta)
{
	MakeDiagonal(m,1.0f);
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[0][0] = cosTheta;
	m[0][1] = -sinTheta;
	m[1][0] = sinTheta;
	m[1][1] = cosTheta;
}


void Camera::calculateViewingTransformParameters() 
{
	Mat4f dollyXform;
	Mat4f azimXform;
	Mat4f elevXform;
	Mat4f twistXform;
	Mat4f originXform;

	Vec3f upVector;

	MakeHTrans(dollyXform, Vec3f(0,0,mDolly));
	MakeHRotY(azimXform, mAzimuth);
	MakeHRotX(elevXform, mElevation);
	MakeDiagonal(twistXform, 1.0f);
	MakeHTrans(originXform, mLookAt);
	
	mPosition = Vec3f(0,0,0);
	// grouped for (mat4 * vec3) ops instead of (mat4 * mat4) ops
	mPosition = originXform * (azimXform * (elevXform * (dollyXform * mPosition)));

	float xval = cos(mTwist);
	float yval = sin(mTwist);

	if (enableTwist)
	{
		if (fmod((double)mElevation, 2.0*M_PI) < 3 * M_PI / 2 && fmod((double)mElevation, 2.0*M_PI) > M_PI / 2)
			mUpVector = Vec3f(-xval, -yval, -xval);
		else
			mUpVector = Vec3f(xval, yval, -xval);
	}

	else
	{
		if (fmod((double)mElevation, 2.0*M_PI) < 3 * M_PI / 2 && fmod((double)mElevation, 2.0*M_PI) > M_PI / 2)
			mUpVector = Vec3f(0, -1, 0);
		else
			mUpVector = Vec3f(0, 1, 0);
	}

	mDirtyTransform = false;
}

Camera::Camera() 
{
	mElevation = mAzimuth = mTwist = 0.0f;
	mDolly = -20.0f;
	mElevation = 0.2f;
	mAzimuth = (float)M_PI;
	mLookAt = Vec3f( 0, 0, 0 );
	mCurrentMouseAction = kActionNone;
	enableTwist = false;

	calculateViewingTransformParameters();
}

void Camera::clickMouse( MouseAction_t action, int x, int y )
{
	mCurrentMouseAction = action;
	mLastMousePosition[0] = x;
	mLastMousePosition[1] = y;
}

void Camera::dragMouse( int x, int y )
{
	Vec3f mouseDelta   = Vec3f(x,y,0.0f) - mLastMousePosition;
	mLastMousePosition = Vec3f(x,y,0.0f);

	switch(mCurrentMouseAction)
	{
	case kActionTranslate:
		{
			calculateViewingTransformParameters();

			double xTrack =  -mouseDelta[0] * kMouseTranslationXSensitivity;
			double yTrack =  mouseDelta[1] * kMouseTranslationYSensitivity;

			Vec3f transXAxis = mUpVector ^ (mPosition - mLookAt);
			transXAxis /= sqrt((transXAxis*transXAxis));
			Vec3f transYAxis = (mPosition - mLookAt) ^ transXAxis;
			transYAxis /= sqrt((transYAxis*transYAxis));

			setLookAt(getLookAt() + transXAxis*xTrack + transYAxis*yTrack);
			
			break;
		}
	case kActionRotate:
		{
			float dAzimuth		=   -mouseDelta[0] * kMouseRotationSensitivity;
			float dElevation	=   mouseDelta[1] * kMouseRotationSensitivity;
			
			setAzimuth(getAzimuth() + dAzimuth);
			setElevation(getElevation() + dElevation);
			
			break;
		}
	case kActionZoom:
		{
			float dDolly = -mouseDelta[1] * kMouseZoomSensitivity;
			setDolly(getDolly() + dDolly);
			break;
		}
	case kActionTwist:
		// Not implemented
		{
			float dAzimuth = -mouseDelta[0] * kMouseRotationSensitivity;
			float dElevation = mouseDelta[1] * kMouseRotationSensitivity;
			float dTwist = sqrt(mouseDelta[0] * mouseDelta[0] + mouseDelta[1] * mouseDelta[1]) * kMouseRotationSensitivity;

			setAzimuth(getAzimuth() + dAzimuth);
			setElevation(getElevation() + dElevation);
			setTwist(getTwist() + dTwist);
		
		}
	default:
		break;
	}

}

void Camera::releaseMouse( int x, int y )
{
	mCurrentMouseAction = kActionNone;
}


void Camera::applyViewingTransform() {
	if( mDirtyTransform )
		calculateViewingTransformParameters();

	// Place the camera at mPosition, aim the camera at
	// mLookAt, and twist the camera such that mUpVector is up
	/*
	gluLookAt(	mPosition[0], mPosition[1], mPosition[2],
				mLookAt[0],   mLookAt[1],   mLookAt[2],
				mUpVector[0], mUpVector[1], mUpVector[2]);
				*/

	lookAt(mPosition, mLookAt, mUpVector);
}

void Camera::lookAt(Vec3f eye, Vec3f at, Vec3f up)
{

	Vec3f F;
	for (int i = 0; i < 3; i++)
	{
		F[i] = at[i] - eye[i];
	}
	Vec3f f = vectNorm(F);

	Vec3f up1 = vectNorm(up);

	Vec3f S = vectCross(f, up1);
	Vec3f s = vectNorm(S);

	

	printf("S %f %f %f\n", &S[0], &S[1], &S[2]);
	printf("S %f %f %f\n", &s[0], &s[1], &s[2]);

	Vec3f u = vectCross(s, f);

	GLfloat* matrix = new float[16];
	/*
	matrix[0] = s[0];
	matrix[1] = s[1];
	matrix[2] = s[2];
	matrix[3] = 0;
	matrix[4] = u[0];
	matrix[5] = u[1];
	matrix[6] = u[2];
	matrix[7] = 0;
	matrix[8] = -f[0];
	matrix[9] = -f[1];
	matrix[10] = -f[2];
	matrix[11] = 0;
	*/

	for (int i = 0; i < 3; i++)
	{
		matrix[i * 4 + 0] = s[i];
		matrix[i * 4 + 1] = u[i];
		matrix[i * 4 + 2] = -f[i];
		matrix[i * 4 + 3] = 0;
	}


	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
	matrix[15] = 1;

	glMultMatrixf(matrix);
	glTranslated(-eye[0], -eye[1], -eye[2]);
}

Vec3f Camera::vectNorm(Vec3f vect)
{
	double lengthSqr = vect[0] * vect[0] + vect[1] * vect[1] + vect[2] * vect[2];
	double length = sqrt(lengthSqr);

	Vec3f vectNew;

	vectNew[0] = vect[0] / length;
	vectNew[1] = vect[1] / length;
	vectNew[2] = vect[2] / length;

	return vectNew;
}

Vec3f Camera::vectCross(Vec3f a, Vec3f b)
{
	Vec3f c;
	c[0] = a[1] * b[2] - a[2] * b[1];
	c[1] = -(a[0] * b[2] - a[2] * b[0]);
	c[2] = a[0] * b[1] - a[1] * b[0];

	return c;
}

#pragma warning(pop)