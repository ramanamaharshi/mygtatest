



/// constants

#define OGL_MAX_MESHES 16 * 16 * 16




/// structs

typedef GLfloat Vector[4];

typedef GLfloat Matrix[16];

typedef struct vertex {
	GLfloat aNormal[4];
	GLfloat aPosition[4];
	GLubyte aSpecular[4];
	GLfloat aTexcoord[2];
	GLfloat nShininess;
} Vertex;

typedef struct mesh {
	GLuint iTexture;
	GLsizei iElements;
	GLsizei iVertices;
	GLushort *aElements;
	Vertex *aVertices;
	GLuint iElementsBuffer;
	GLuint iVerticesBuffer;
	GLfloat mModelMatrix[16];
	GLfloat vTranslation[3];
	GLfloat vRotation[3];
} Mesh;

struct mesh force_definition_for_mesh;

struct program {
	GLuint iVertexShader, iFragmentShader, iProgram;
	struct {
		GLint texture, m_matrix, v_matrix, p_matrix;
	} oUniforms;
	struct {
		GLint position, normal, texcoord, shininess, specular;
	} oAttributes;
};

struct oglenv {
	
	int iMeshes;
	Mesh *aMeshes[OGL_MAX_MESHES];
	
	struct program oProgram;
	
//	struct {
		GLsizei iWindowW, iWindowH;
		GLfloat mProjection[16];
		GLfloat mView[16];
		GLfloat vViewPos[3];
		GLfloat aViewDir[2];
//	} oEnv;
	
};




/// instances

extern struct oglenv oGR;




/// functions

///#///

void o3d_c_vOpenWindow (int iWindowW, int iWindowH, char* sTitle) ;
void o3d_c_vCloseWindow () ;
void o3d_c_vSetViewPort (int iWindowW, int iWindowH) ;
Mesh* o3d_c_oCreateMesh (int iElements, GLuint* aElements, int iVertices, Vertex* aVertices, GLuint iTexture) ;
Mesh* o3d_c_oMakeMesh (int iElements, int iVertices) ;
void o3d_c_vAddMesh (Mesh* oMesh) ;
void o3d_c_vDeleteMesh (Mesh* oMesh) ;
GLuint o3d_c_iMakeTexture (char* sImageFile) ;
static GLuint o3d_c_iMakeShader (GLenum type, const char* filename) ;
int o3d_c_bStandardProgram () ;
GLuint o3d_c_iMakeProgram (char* sVertexShader, char* sFragmentShader) ;
static char* o3d_c_sGetFileContents (const char* filename, GLint* length) ;
static void o3d_c_vUpdateModelMatrix (Mesh* oMesh) ;
static void o3d_c_vUpdateViewMatrix () ;
void o3d_c_vStandardDraw () ;
void o3d_c_vStandardRenderMesh (Mesh* oMesh) ;

///#///
