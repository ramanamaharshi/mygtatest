

#if 0
#!/bin/sh
./run;
exit;
#endif




#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "o3d/o3d_math.h"
#include "o3d/o3d_core.h"

#include "world.h"
#include "main.h"




#define FLAG_W (GLfloat) 4
#define FLAG_H (GLfloat) 3
#define FLAG_RES_X (GLsizei) 9
#define FLAG_RES_Y (GLsizei) 3




const int iMaxFPS = 30;

SDL_Window* oWindow;
SDL_GLContext* oContext;




int main (int argc, char* argv[]) {
	
	printf("\n");
	
	int iWindowW = 640;
	int iWindowH = 480;
	
	o3d_c_vOpenWindow(iWindowW, iWindowH, "mysdl");
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	o3d_c_bStandardProgram();
	
	vInit();
	
	vInitWorld();
	
	float nFPS;
	Uint32 iMillis;
	Uint32 iPrevMillis;
	int aFrameMillis[32];
	int iFrameMillisAt = 0;
	int iI = 0; for (iI = 0; iI < 32; iI ++) aFrameMillis[iI] = 0;
	
	SDL_SetRelativeMouseMode(1);
	
	int bFullScreen = 0;
	
	int bQuit = 0;
	
	while (!bQuit) {
		
		iPrevMillis = iMillis;
		iMillis = SDL_GetTicks();
		nFPS = 1000.0f / (float)(iMillis - iPrevMillis);
printf("%.2f\r", nFPS);
		
		SDL_Event oEvent;
		while (SDL_PollEvent(&oEvent)) {
			
			if (oEvent.type == SDL_KEYDOWN) {
				int iScancode = oEvent.key.keysym.scancode;
//printf("key: %d\n", iScancode);
				if (iScancode == SDL_SCANCODE_F || iScancode == SDL_SCANCODE_RETURN) {
					bFullScreen = !bFullScreen;
					SDL_SetWindowFullscreen(oWindow, bFullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
				}
			}
			
			if (oEvent.type == SDL_WINDOWEVENT) {
				
				if (oEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					
					int iNewWindowWidth = oEvent.window.data1;
					int iNewWindowHeight = oEvent.window.data2;
					o3d_c_vSetViewPort(iNewWindowWidth, iNewWindowHeight);
					
				}
				
			}
			
			if (oEvent.type == SDL_QUIT) bQuit = 1;
			
		}
		
		vCalc();
		
		o3d_c_vStandardDraw();
		
		SDL_GL_SwapWindow(oWindow);
		
		SDL_Delay(1);
		
	}
	
	o3d_c_vCloseWindow();
	
	return 0;
	
}




void vInit () {
	
	GLuint iSurfaceTexture = o3d_c_iMakeTexture("../res/images/oXplwkA.jpg");
	
	int iFlagsX = 2;
	int iFlagsY = 2;
	int iFlagsZ = 2;
	float nFlagsDistX = 6.0f;
	float nFlagsDistY = 4.0f;
	float nFlagsDistZ = 5.0f;
	float nFlagsStartX = - (nFlagsDistX * ((float)iFlagsX - 1.0f)) / 2.0f;
	float nFlagsStartY = - (nFlagsDistY * ((float)iFlagsY - 1.0f)) / 2.0f;
	float nFlagsStartZ = - (nFlagsDistY * ((float)iFlagsZ - 1.0f)) / 2.0f;
	Mesh* oAnotherFlag;
	int iZ; for (iZ = 0; iZ < iFlagsZ; iZ ++) {
		int iY; for (iY = 0; iY < iFlagsY; iY ++) {
			int iX; for (iX = 0; iX < iFlagsX; iX ++) {
				oAnotherFlag = oAddFlagMesh();
				oAnotherFlag->vTranslation[0] = nFlagsStartX + iX * nFlagsDistX;
				oAnotherFlag->vTranslation[1] = nFlagsStartY + iY * nFlagsDistY;
				oAnotherFlag->vTranslation[2] = nFlagsStartZ - (iZ * nFlagsDistZ + iFlagsZ * 0.5f);
				oAnotherFlag->iTexture = iSurfaceTexture;
			}
		}
	}
	
	/*int iI; for (iI = 0; iI < iFlagsX * iFlagsY / 16; iI ++) {
		int iDeleteNr = rand() % oGR.iMeshes;
		Mesh* oDeleteMesh = oGR.aMeshes[iDeleteNr];
		o3d_c_vDeleteMesh(oDeleteMesh);
	}*/
	
	GLfloat nSeconds = (GLfloat)SDL_GetTicks() * (1.0f/1000.0f);
	int iM; for (iM = 0; iM < oGR.iMeshes; iM ++) {
		Mesh* oMesh = oGR.aMeshes[iM];
		if (oMesh->iVertices != FLAG_RES_X * FLAG_RES_Y) continue;
		update_flag_mesh(oMesh, nSeconds);
	}
	
}




void vCalc () {
	
	const Uint8* aKeyboardState = SDL_GetKeyboardState(0);
	
	GLfloat nWalkSpeed = 0.25f;
	GLfloat vWalk[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	if (aKeyboardState[SDL_SCANCODE_W]) {
		vWalk[0] +=  0.0f * nWalkSpeed;
		vWalk[2] += -1.0f * nWalkSpeed;
	}
	if (aKeyboardState[SDL_SCANCODE_S]) {
		vWalk[0] +=  0.0f * nWalkSpeed;
		vWalk[2] +=  1.0f * nWalkSpeed;
	}
	if (aKeyboardState[SDL_SCANCODE_A]) {
		vWalk[0] += -1.0f * nWalkSpeed;
		vWalk[2] +=  0.0f * nWalkSpeed;
	}
	if (aKeyboardState[SDL_SCANCODE_D]) {
		vWalk[0] +=  1.0f * nWalkSpeed;
		vWalk[2] +=  0.0f * nWalkSpeed;
	}
	GLfloat mWalkMatrix[16];
	GLfloat vYAxis[4] = {0.0f, 1.0f, 0.0f, 0.0f};
	o3d_m_vIdentityM(mWalkMatrix);
	o3d_m_vRotateM(mWalkMatrix, oGR.aViewDir[0], vYAxis);
	o3d_m_vMultiplyVM(vWalk, mWalkMatrix);
	oGR.vViewPos[0] += vWalk[0];
	oGR.vViewPos[2] += vWalk[2];
	
	int iMX, iMY;
	SDL_GetRelativeMouseState(&iMX, &iMY);
	oGR.aViewDir[0] -= 0.2f * (GLfloat)iMX;
	oGR.aViewDir[1] -= 0.2f * (GLfloat)iMY;
	if (oGR.aViewDir[1] < -90.0f) oGR.aViewDir[1] = -90.0f;
	if (oGR.aViewDir[1] >  90.0f) oGR.aViewDir[1] =  90.0f;
	
	//GLfloat nSeconds = (GLfloat)SDL_GetTicks() * (1.0f/1000.0f);
	//int iM; for (iM = 0; iM < oGR.iMeshes; iM ++) {
	//	Mesh* oMesh = oGR.aMeshes[iM];
	//	update_flag_mesh(oMesh, nSeconds);
	//}
	
}




static void calculate_flag_vertices (Mesh* oMesh, float nSeconds) {
	
	Vertex* aVertices = oMesh->aVertices;
	
	GLsizei iS, iT, iV;
	
	for (iT = 0; iT < FLAG_RES_Y; iT ++) {
		for (iS = 0; iS < FLAG_RES_X; iS ++) {
			
			iV = (FLAG_RES_X) * iT + iS;
			Vertex* oV = &aVertices[iV];
			
			float nXrel = (float)iS / (FLAG_RES_X - 1);
			float nYrel = (float)iT / (FLAG_RES_Y - 1);
			
			oV->nShininess   = 3.333333f;
			
			oV->aTexcoord[0] = nXrel;
			oV->aTexcoord[1] = nYrel;
			
			oV->aSpecular[0] = 0;
			oV->aSpecular[1] = 0;
			oV->aSpecular[2] = 0;
			oV->aSpecular[3] = 0;
			
			oV->aPosition[0] = FLAG_W * (nXrel - 0.5f);
			oV->aPosition[1] = FLAG_H * (nYrel - 0.5f);
			oV->aPosition[2] = 0.3f * nXrel * sinf(5 * nSeconds - 5 * nXrel);
			oV->aPosition[3] = 0.0f;
			
			oV->aNormal[0] = 0.0f;
			oV->aNormal[1] = 0.0f;
			oV->aNormal[2] = 0.0f;
			oV->aNormal[3] = 0.0f;
			
		}
	}
	
}




Mesh* oAddFlagMesh () {
	
	int iVertices = (FLAG_RES_X * FLAG_RES_Y);
	int iElements = (6 * (FLAG_RES_X - 1) * (FLAG_RES_Y - 1));
	Mesh* oMesh = o3d_c_oMakeMesh(iElements, iVertices);
	
	GLsizei iSquareNr = -1;
	GLsizei iSquareX, iSquareY, iStartIndex, iVA, iVB, iVC, iVD;
	for (iSquareY = 0; iSquareY < FLAG_RES_Y - 1; iSquareY ++) {
		for (iSquareX = 0; iSquareX < FLAG_RES_X - 1; iSquareX ++) {
			
			iSquareNr ++;
			iStartIndex = 6 * iSquareNr;
			
			iVA = FLAG_RES_X * iSquareY + iSquareX;
			iVB = iVA + 1;
			iVC = iVA + FLAG_RES_X;
			iVD = iVA + FLAG_RES_X + 1;
			
			oMesh->aElements[iStartIndex + 0] = iVA;
			oMesh->aElements[iStartIndex + 1] = iVB;
			oMesh->aElements[iStartIndex + 2] = iVC;
			oMesh->aElements[iStartIndex + 3] = iVC;
			oMesh->aElements[iStartIndex + 4] = iVB;
			oMesh->aElements[iStartIndex + 5] = iVD;
			
		}
	}
	
	o3d_c_vAddMesh(oMesh);
	
	return oMesh;
	
}




void update_flag_mesh (Mesh* oMesh, float nSeconds) {
	
	calculate_flag_vertices(oMesh, nSeconds);
	
	glBindBuffer(GL_ARRAY_BUFFER, oMesh->iVerticesBuffer);
	glBufferData(
		GL_ARRAY_BUFFER,
		oMesh->iVertices * sizeof(Vertex), oMesh->aVertices,
		GL_STREAM_DRAW
	);
	glBindBuffer(0);
	
}



