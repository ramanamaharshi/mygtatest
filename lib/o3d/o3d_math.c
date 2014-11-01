

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>

#include <GL/gl.h>

#include "o3d_math.h"




void o3d_m_vPrintV (Vector vVector) {
	
	printf("(%.4f , %.4f , %.4f , %.4f)\n", vVector[0], vVector[1], vVector[2], vVector[3]);
	
}




void o3d_m_vPrintM (Matrix mMatrix) {
	
	int iX, iY;
	for (iY = 0; iY < 4; iY ++) {
		for (iX = 0; iX < 4; iX ++) {
			printf(" %.4f ", mMatrix[4 * iX + iY]);
		}
		printf("\n");
	}
	
}




void o3d_m_vIdentityM (Matrix mMatrix) {
	
	mMatrix[ 0] = 1.0f; mMatrix[ 1] = 0.0f; mMatrix[ 2] = 0.0f; mMatrix[ 3] = 0.0f;
	mMatrix[ 4] = 0.0f; mMatrix[ 5] = 1.0f; mMatrix[ 6] = 0.0f; mMatrix[ 7] = 0.0f;
	mMatrix[ 8] = 0.0f; mMatrix[ 9] = 0.0f; mMatrix[10] = 1.0f; mMatrix[11] = 0.0f;
	mMatrix[12] = 0.0f; mMatrix[13] = 0.0f; mMatrix[14] = 0.0f; mMatrix[15] = 1.0f;
	
}




void o3d_m_vMultiplyM (Matrix mA, Matrix mB, Matrix mC) {
	
	return o3d_m_vGLMultiplyM(mA, mB, mC);
	
	GLfloat nSum;
	int iX, iY, iI;
	for (iY = 0; iY < 4; iY ++) {
		for (iX = 0; iX < 4; iX ++) {
			nSum = 0;
			for (iI = 0; iI < 4; iI ++) {
				nSum += mB[4 * iI + iY] * mC[4 * iX + iI];
			}
			mA[4 * iX + iY] = nSum;
		}
	}
	
}




static void o3d_m_vGLMultiplyM (Matrix mA, Matrix mB, Matrix mC) {
	
	glMatrixMode(GL_MODELVIEW_MATRIX);
	glLoadIdentity();
	glMultMatrixf(mB);
	glMultMatrixf(mC);
	glGetFloatv(GL_MODELVIEW_MATRIX, mA);
	
}




void o3d_m_vTranslateM (Matrix mMatrix, Vector vTranslation) {
	
	return o3d_m_vGLTranslateM(mMatrix, vTranslation);
	
	mMatrix[12] = vTranslation[0];
	mMatrix[13] = vTranslation[1];
	mMatrix[14] = vTranslation[2];
	
}




static void o3d_m_vGLTranslateM (Matrix mMatrix, Vector vTranslation) {
	
	glMatrixMode(GL_MODELVIEW_MATRIX);
	glLoadMatrixf(mMatrix);
	glTranslatef(vTranslation[0], vTranslation[1], vTranslation[2]);
	glGetFloatv(GL_MODELVIEW_MATRIX, mMatrix);
	
}




void o3d_m_vRotateM (Matrix mMatrix, GLfloat nRotation, Vector vAxis) {
	
	glMatrixMode(GL_MODELVIEW_MATRIX);
	glLoadMatrixf(mMatrix);
	glRotatef(nRotation, vAxis[0], vAxis[1], vAxis[2]);
	glGetFloatv(GL_MODELVIEW_MATRIX, mMatrix);
	
}




void o3d_m_vRotateHoM (Matrix mMatrix, GLfloat nRotation) {
	
	GLfloat vAxis[3] = {0.0f, 1.0f, 0.0f};
	o3d_m_vRotateM(mMatrix, nRotation, vAxis);
	
}




void o3d_m_vRotateVeM (Matrix mMatrix, GLfloat nRotation) {
	
	GLfloat vAxis[3] = {1.0f, 0.0f, 0.0f};
	o3d_m_vRotateM(mMatrix, nRotation, vAxis);
	
}





void o3d_m_vFrustum (
	Matrix mMatrix,
	GLfloat nLeft,
	GLfloat nRight,
	GLfloat nBottom,
	GLfloat nTop,
	GLfloat nNear,
	GLfloat nFar
) {
	
	
	GLfloat nA = (nRight + nLeft) / (nRight - nLeft);
	GLfloat nB = (nTop + nBottom) / (nTop - nBottom);
	GLfloat nC = - (nFar + nNear) / (nFar - nNear);
	GLfloat nD = - (2 * nFar * nNear) / (nFar - nNear);
	
	mMatrix[0] = (2 * nNear) / (nRight - nLeft); mMatrix[4] = 0; mMatrix[8] = nA; mMatrix[12] = 0;
	mMatrix[1] = 0; mMatrix[5] = (2 * nNear) / (nTop - nBottom); mMatrix[9] = nB; mMatrix[13] = 0;
	mMatrix[2] = 0; mMatrix[6] = 0; mMatrix[10] = nC; mMatrix[14] = nD;
	mMatrix[3] = 0; mMatrix[7] = 0; mMatrix[11] = -1; mMatrix[15] = 0;
	
}




void o3d_m_vPerspective (
	Matrix mMatrix,
	GLfloat nFOVdeg,
	GLfloat nRatio,
	GLfloat nNear,
	GLfloat nFar
) {
	
	GLfloat nH = tan(nFOVdeg * M_PI / 360) * nNear;
	GLfloat nW = nH * nRatio;
	
	o3d_m_vFrustum(mMatrix, -nW, nW, -nH, nH, nNear, nFar);
	
}




void o3d_m_vMultiplyVM (Vector vA, Matrix mA) {
	
	GLfloat fTemp0 = mA[0]*vA[0] + mA[4]*vA[1] + mA[ 8]*vA[2] + mA[12];//*vA[3];
	GLfloat fTemp1 = mA[1]*vA[0] + mA[5]*vA[1] + mA[ 9]*vA[2] + mA[13];//*vA[3];
	GLfloat fTemp2 = mA[2]*vA[0] + mA[6]*vA[1] + mA[10]*vA[2] + mA[14];//*vA[3];
	GLfloat fTemp3 = mA[3]*vA[0] + mA[7]*vA[1] + mA[11]*vA[2] + mA[15];//*vA[3];
	
	vA[0] = fTemp0;
	vA[1] = fTemp1;
	vA[2] = fTemp2;
	vA[3] = fTemp3;
	
}




void o3d_m_vMultiplyV (Vector vA, GLfloat nB) {
	
	vA[0] = nB * vA[0];
	vA[1] = nB * vA[1];
	vA[2] = nB * vA[2];
	vA[3] = nB * vA[3];
	
}




void o3d_m_vCrossVV (Vector vA, Vector vB, Vector vC) {
	
	vA[0] = vB[1]*vC[2] - vB[2]*vC[1];
	vA[1] = vB[2]*vC[0] - vB[0]*vC[2];
	vA[2] = vB[0]*vC[1] - vB[1]*vC[0];
	
}




void o3d_m_vNormalizeV (Vector vA) {
	
	GLfloat nLength = o3d_m_nLengthV(vA);
	vA[0] /= nLength;
	vA[1] /= nLength;
	vA[2] /= nLength;
	
}




GLfloat o3d_m_nLengthV (Vector vA) {
	
	return sqrtf(vA[0]*vA[0] + vA[1]*vA[1] + vA[2]*vA[2]);
	
}




void o3d_m_vCopyV (Vector vSource, Vector vTarget) {
	
	vTarget[0] = vSource[0];
	vTarget[1] = vSource[1];
	vTarget[2] = vSource[2];
	vTarget[3] = vSource[3];
	
}




void o3d_m_vQuickMatrix (Matrix mMatrix, Vector vTranslation, Vector vZAxis) {
	
	o3d_m_vNormalizeV(vZAxis);
	
	Vector vXAxis = {-1, 0, 0, 0};
	
	if (fabsf(vZAxis[0]) > 0.000001 || fabsf(vZAxis[2]) > 0.000001) {
		o3d_m_vCopyV(vZAxis, vXAxis);
		vXAxis[1] = 0;
		o3d_m_vNormalizeV(vXAxis);
		GLfloat nNewXX = vXAxis[2];
		GLfloat nNewXZ = - vXAxis[0];
		vXAxis[0] = nNewXX;
		vXAxis[2] = nNewXZ;
	}
	
	Vector vYAxis;
	o3d_m_vCrossVV(vYAxis, vZAxis, vXAxis);
	
	mMatrix[ 0] = vXAxis[0]; mMatrix[ 4] = vYAxis[0]; mMatrix[ 8] = vZAxis[0]; mMatrix[12] = vTranslation[0];
	mMatrix[ 1] = vXAxis[1]; mMatrix[ 5] = vYAxis[1]; mMatrix[ 9] = vZAxis[1]; mMatrix[13] = vTranslation[1];
	mMatrix[ 2] = vXAxis[2]; mMatrix[ 6] = vYAxis[2]; mMatrix[10] = vZAxis[2]; mMatrix[14] = vTranslation[2];
	mMatrix[ 3] = vXAxis[3]; mMatrix[ 7] = vYAxis[3]; mMatrix[11] = vZAxis[3]; mMatrix[15] = vTranslation[3];
	
}



