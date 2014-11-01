
#ifndef M_PI
#define M_PI 3.141592653589793
#endif

typedef GLfloat Vector[4];

typedef GLfloat Matrix[16];

///#///

void o3d_m_vPrintV (Vector vVector) ;
void o3d_m_vPrintM (Matrix mMatrix) ;
void o3d_m_vIdentityM (Matrix mMatrix) ;
void o3d_m_vMultiplyM (Matrix mA, Matrix mB, Matrix mC) ;
static void o3d_m_vGLMultiplyM (Matrix mA, Matrix mB, Matrix mC) ;
void o3d_m_vTranslateM (Matrix mMatrix, Vector vTranslation) ;
static void o3d_m_vGLTranslateM (Matrix mMatrix, Vector vTranslation) ;
void o3d_m_vRotateM (Matrix mMatrix, GLfloat nRotation, Vector vAxis) ;
void o3d_m_vRotateHoM (Matrix mMatrix, GLfloat nRotation) ;
void o3d_m_vRotateVeM (Matrix mMatrix, GLfloat nRotation) ;
void o3d_m_vFrustum (
	Matrix mMatrix,
	GLfloat nLeft,
	GLfloat nRight,
	GLfloat nBottom,
	GLfloat nTop,
	GLfloat nNear,
	GLfloat nFar
) ;
void o3d_m_vPerspective (
	Matrix mMatrix,
	GLfloat nFOVdeg,
	GLfloat nRatio,
	GLfloat nNear,
	GLfloat nFar
) ;
void o3d_m_vMultiplyVM (Vector vA, Matrix mA) ;
void o3d_m_vMultiplyV (Vector vA, GLfloat nB) ;
void o3d_m_vCrossVV (Vector vA, Vector vB, Vector vC) ;
void o3d_m_vNormalizeV (Vector vA) ;
GLfloat o3d_m_nLengthV (Vector vA) ;
void o3d_m_vCopyV (Vector vSource, Vector vTarget) ;
void o3d_m_vQuickMatrix (Matrix mMatrix, Vector vTranslation, Vector vZAxis) ;

///#///
