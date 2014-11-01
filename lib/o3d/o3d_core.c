

#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <GL/gl.h>

#include "math.h"

#include "o3d_math.h"
#include "o3d_core.h"




#define PROJECTION_NEAR_PLANE   0.0625f
#define PROJECTION_FAR_PLANE  256.0000f
#define PROJECTION_FOV_RATIO    0.7000f




SDL_Window* oWindow;
SDL_GLContext* oContext;




struct oglenv oGR;

struct {
	void (*vHandleEvents) ();
	void (*vAfterCalc) ();
	void (*vAfterDraw) ();
} oBinds;




void o3d_c_vBindEventHandler (void(*func)(SDL_Event)) {
	oBinds.vHandleEvents = *func;
}
void o3d_c_vBindCalc (void(*func)()) {
	oBinds.vAfterCalc = *func;
}
void o3d_c_vBindDraw (void(*func)()) {
	oBinds.vAfterDraw = *func;
}




void o3d_c_vOpenWindow (int iWindowW, int iWindowH, char* sTitle) {
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		SDL_Quit(); exit(1);
	}
	
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	oWindow = SDL_CreateWindow(
		sTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		iWindowW, iWindowH, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
	);
	if (!oWindow) {
		printf("Unable to create window: %s\n", SDL_GetError());
		SDL_Quit(); exit(1);
	}
	
	oContext = SDL_GL_CreateContext(oWindow);
	o3d_c_vSetViewPort(iWindowW, iWindowH);
	
}




void o3d_c_vCloseWindow () {
	
	SDL_GL_DeleteContext(oContext);
	SDL_DestroyWindow(oWindow);
	SDL_Quit();
	
}




void o3d_c_vSetViewPort (int iWindowW, int iWindowH) {
	
	glViewport(0, 0, (GLsizei)iWindowW, (GLsizei)iWindowH);
	o3d_m_vPerspective(oGR.mProjection, 80.0f, (GLfloat)iWindowW / (GLfloat)iWindowH, 0.125f, 512.0f);
	
}




Mesh* o3d_c_oCreateMesh (int iElements, GLuint* aElements, int iVertices, Vertex* aVertices, GLuint iTexture) {
	
	Mesh* oMesh = o3d_c_oMakeMesh(iElements, iVertices);
	
	int iE; for (iE = 0; iE < iElements; iE ++) {
		oMesh->aElements[iE] = aElements[iE];
	}
	int iV; for (iV = 0; iV < iVertices; iV ++) {
		oMesh->aVertices[iV] = aVertices[iV];
	}
	
	oMesh->iTexture = iTexture;
	
	o3d_c_vAddMesh(oMesh);
	
	return oMesh;
	
}




Mesh* o3d_c_oMakeMesh (int iElements, int iVertices) {
	
	Mesh* oMesh = (Mesh*) malloc(sizeof(Mesh));
	
	oMesh->iElements = iElements;
	oMesh->iVertices = iVertices;
	
	oMesh->aElements = (GLushort*) malloc(oMesh->iElements * sizeof(GLushort));
	oMesh->aVertices = (Vertex*) malloc(oMesh->iVertices * sizeof(Vertex));
	
	o3d_m_vIdentityM(oMesh->mModelMatrix);
	
	oMesh->iTexture = 0;
	
	return oMesh;
	
}




void o3d_c_vAddMesh (Mesh* oMesh) {
	
	if (oGR.iMeshes >= OGL_MAX_MESHES) {
		printf("ERROR: could not add mesh, because array is full. ");
		printf("oGR.iMeshes = %d, OGL_MAX_MESHES = %d)\n", oGR.iMeshes, OGL_MAX_MESHES);
		exit(1);
		return;
	}
	
	glGenBuffers(1, &(oMesh->iVerticesBuffer));
	glGenBuffers(1, &(oMesh->iElementsBuffer));
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oMesh->iElementsBuffer);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		oMesh->iElements * sizeof(GLushort), oMesh->aElements,
		GL_STATIC_DRAW
	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	oGR.aMeshes[oGR.iMeshes ++] = oMesh;
	
}




void o3d_c_vDeleteMesh (Mesh* oMesh) {
	
	int iMeshPosition = -1;
	int iM; for (iM = 0; iM < OGL_MAX_MESHES; iM ++) {
		if (oGR.aMeshes[iM] == oMesh) {
			iMeshPosition = iM;
			break;
		}
	}
	
	oGR.aMeshes[iMeshPosition] = oGR.aMeshes[-- oGR.iMeshes];
	
	glDeleteBuffers(1, &(oMesh->iElementsBuffer));
	glDeleteBuffers(1, &(oMesh->iVerticesBuffer));
	
	free(oMesh->aElements);
	free(oMesh->aVertices);
	
	free(oMesh);
	
}




GLuint o3d_c_iMakeTexture (char* sImageFile) {
	
	SDL_Surface* oStandardSurface = SDL_CreateRGBSurface(
		0, 2, 2, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000
	);
	
	SDL_Surface* oSurface = IMG_Load(sImageFile);
	oSurface = SDL_ConvertSurface(oSurface, oStandardSurface->format, 0);
	
	GLuint iTexture = 0;
	glGenTextures(1, &iTexture);
	glBindTexture(GL_TEXTURE_2D, iTexture);
	GLuint iImageFormat = oSurface->format->BytesPerPixel == 4 ? GL_BGRA : GL_BGR;
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGB, oSurface->w, oSurface->h, 0,
		iImageFormat, GL_UNSIGNED_BYTE, oSurface->pixels
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	return iTexture;
	
}




static GLuint o3d_c_iMakeShader (GLenum type, const char* filename) {
	
	GLint length;
	GLchar* source = o3d_c_sGetFileContents(filename, &length);
	GLuint shader;
	GLint shader_ok;
	
	if (!source) return 0;
	
	shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar**)&source, &length);
	free(source);
	glCompileShader(shader);
	
	glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
	if (!shader_ok) {
		fprintf(stderr, "Failed to compile %s:\n", filename);
		glDeleteShader(shader);
		return 0;
	}
	
	return shader;
	
}




int o3d_c_bStandardProgram () {
	
	GLuint* iProgram = &oGR.oProgram.iProgram;
	
	*iProgram = o3d_c_iMakeProgram("../res/shaders/shader.v.glsl", "../res/shaders/shader.f.glsl");
	if (*iProgram == 0) return 0;
	
	oGR.oProgram.oUniforms.texture  = glGetUniformLocation(*iProgram, "texture");
	oGR.oProgram.oUniforms.m_matrix = glGetUniformLocation(*iProgram, "m_matrix");
	oGR.oProgram.oUniforms.v_matrix = glGetUniformLocation(*iProgram, "v_matrix");
	oGR.oProgram.oUniforms.p_matrix = glGetUniformLocation(*iProgram, "p_matrix");
	
	oGR.oProgram.oAttributes.position  = glGetAttribLocation(*iProgram, "position");
	oGR.oProgram.oAttributes.normal    = glGetAttribLocation(*iProgram, "normal");
	oGR.oProgram.oAttributes.texcoord  = glGetAttribLocation(*iProgram, "texcoord");
	oGR.oProgram.oAttributes.shininess = glGetAttribLocation(*iProgram, "shininess");
	oGR.oProgram.oAttributes.specular  = glGetAttribLocation(*iProgram, "specular");
	
	return 1;
	
}




GLuint o3d_c_iMakeProgram (char* sVertexShader, char* sFragmentShader) {
	
	GLuint iProgram = glCreateProgram();
	
	GLuint iVertexShader = o3d_c_iMakeShader(GL_VERTEX_SHADER, sVertexShader);
	if (iVertexShader == 0) return 0;
	GLuint iFragmentShader = o3d_c_iMakeShader(GL_FRAGMENT_SHADER, sFragmentShader);
	if (iFragmentShader == 0) return 0;
	
	glAttachShader(iProgram, iVertexShader);
	glAttachShader(iProgram, iFragmentShader);
	glLinkProgram(iProgram);
	
	GLint bProgramSuccess;
	glGetProgramiv(iProgram, GL_LINK_STATUS, &bProgramSuccess);
	if (!bProgramSuccess) {
		fprintf(stderr, "Failed to link shader program:\n");
		glDeleteProgram(iProgram);
		return 0;
	}
	
	return iProgram;
	
}




static char* o3d_c_sGetFileContents (const char* filename, GLint* length) {
	
	FILE* f = fopen(filename, "r");
	void* buffer;
	
	if (!f) {
		fprintf(stderr, "Unable to open %s for reading\n", filename);
		return NULL;
	}
	
	fseek(f, 0, SEEK_END);
	*length = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	buffer = malloc(*length+1);
	*length = fread(buffer, 1, *length, f);
	fclose(f);
	((char*)buffer)[*length] = '\0';
	
	return buffer;
	
}




static void o3d_c_vUpdateModelMatrix (Mesh* oMesh) {
	
	o3d_m_vIdentityM(oMesh->mModelMatrix);
	o3d_m_vTranslateM(oMesh->mModelMatrix, oMesh->vTranslation);
	
}




static void o3d_c_vUpdateViewMatrix () {
	
	GLfloat* mMatrix = oGR.mView;
	o3d_m_vIdentityM(mMatrix);
	o3d_m_vRotateVeM(mMatrix, - oGR.aViewDir[1]);
	o3d_m_vRotateHoM(mMatrix, - oGR.aViewDir[0]);
	oGR.vViewPos[0] *= -1.0f;
	oGR.vViewPos[1] *= -1.0f;
	oGR.vViewPos[2] *= -1.0f;
	o3d_m_vTranslateM(oGR.mView, oGR.vViewPos);
	oGR.vViewPos[0] *= -1.0f;
	oGR.vViewPos[1] *= -1.0f;
	oGR.vViewPos[2] *= -1.0f;
	
}




void o3d_c_vStandardDraw () {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(oGR.oProgram.iProgram);
	
	//glActiveTexture(GL_TEXTURE0);
	//glUniform1i(oGR.oProgram.oUniforms.texture, 0);
	
	glUniformMatrix4fv(
		oGR.oProgram.oUniforms.p_matrix,
		1, GL_FALSE,
		oGR.mProjection
	);
	
	o3d_c_vUpdateViewMatrix();
	glUniformMatrix4fv(
		oGR.oProgram.oUniforms.v_matrix,
		1, GL_FALSE,
		oGR.mView
	);
	
	glEnableVertexAttribArray(oGR.oProgram.oAttributes.normal);
	glEnableVertexAttribArray(oGR.oProgram.oAttributes.position);
	glEnableVertexAttribArray(oGR.oProgram.oAttributes.specular);
	glEnableVertexAttribArray(oGR.oProgram.oAttributes.texcoord);
	glEnableVertexAttribArray(oGR.oProgram.oAttributes.shininess);
	
	int iM; for (iM = 0; iM < oGR.iMeshes; iM ++) {
		Mesh* oMesh = oGR.aMeshes[iM];
		o3d_c_vUpdateModelMatrix(oMesh);
		o3d_c_vStandardRenderMesh(oMesh);
	}
	
	glDisableVertexAttribArray(oGR.oProgram.oAttributes.normal);
	glDisableVertexAttribArray(oGR.oProgram.oAttributes.position);
	glDisableVertexAttribArray(oGR.oProgram.oAttributes.specular);
	glDisableVertexAttribArray(oGR.oProgram.oAttributes.texcoord);
	glDisableVertexAttribArray(oGR.oProgram.oAttributes.shininess);
	
	if (oBinds.vAfterDraw != 0) oBinds.vAfterDraw();
	
}




void o3d_c_vStandardRenderMesh (Mesh* oMesh) {
	
	glBindTexture(GL_TEXTURE_2D, oMesh->iTexture);
	glBindBuffer(GL_ARRAY_BUFFER, oMesh->iVerticesBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oMesh->iElementsBuffer);
	
	glVertexAttribPointer(
		oGR.oProgram.oAttributes.normal,
		3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, aNormal)
	);
	glVertexAttribPointer(
		oGR.oProgram.oAttributes.position,
		3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, aPosition)
	);
	glVertexAttribPointer(
		oGR.oProgram.oAttributes.specular,
		4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex),
		(void*)offsetof(Vertex, aSpecular)
	);
	glVertexAttribPointer(
		oGR.oProgram.oAttributes.texcoord,
		2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, aTexcoord)
	);
	glVertexAttribPointer(
		oGR.oProgram.oAttributes.shininess,
		1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, nShininess)
	);
	
	glUniformMatrix4fv(
		oGR.oProgram.oUniforms.m_matrix,
		1, GL_FALSE,
		oMesh->mModelMatrix
	);
	
	glDrawElements(
		GL_TRIANGLES,
		oMesh->iElements,
		GL_UNSIGNED_SHORT,
		(void*)0
	);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
}



