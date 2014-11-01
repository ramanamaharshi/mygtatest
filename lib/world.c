



#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <GL/gl.h>
#include "o3d/o3d_math.h"
#include "o3d/o3d_core.h"

#include "world.h"




#define FOR(VAR, STOP) int VAR; for (VAR = 0; VAR < STOP; VAR ++)
#define FOR3(VAR,STOP)\
	int VAR[3];\
	for (VAR[0] = 0; VAR[0] < STOP; VAR[0] ++)\
	for (VAR[1] = 0; VAR[1] < STOP; VAR[1] ++)\
	for (VAR[2] = 0; VAR[2] < STOP; VAR[2] ++)\




Container* oRootContainer;




void vInitWorld () {
	
	oRootContainer = oNewContainer();
	FOR (iIA, 3) oRootContainer->vAbsBlockPos[iIA] = - CONTAINER_EDGE / 2;
	
	int vFirstBlockPos[3];
	FOR (iIB, 3) vFirstBlockPos[iIB] = 0;
	Block* oFirstBlock = oCreateBlock(vFirstBlockPos);
	oMakeBlockMesh(oFirstBlock);
	oFirstBlock->aAtoms[0] = 1;
	oFirstBlock->aAtoms[1] = 1;
	oMakeBlockMesh(oFirstBlock);
	
	int vSecondBlockPos[3];
	FOR (iIC, 3) vSecondBlockPos[iIC] = -1;//(rand() % 6) - 4;
	Block* oSecondBlock = oCreateBlock(vSecondBlockPos);
	
	Block* oAlsoSecondBlock = oGetBlock(vSecondBlockPos);
	FOR (iAtom, BLOCK_SIZE) {
		oAlsoSecondBlock->aAtoms[iAtom] = (rand() % 3 == 0);
	}
	oMakeBlockMesh(oAlsoSecondBlock);
	
	printf("%p\n%p\n%p\n", oFirstBlock, oSecondBlock, oAlsoSecondBlock);
	
}




Block* oNewBlock () {
	
	Block* oNew = (Block*) malloc(sizeof(Block));
	*oNew = oZeroBlock;
	oNew->bEmpty = TRUE;
	oNew->bFull = FALSE;
	return oNew;
	
}




Container* oNewContainer () {
	
	Container* oNew = (Container*) malloc(sizeof(Container));
	*oNew = oZeroContainer;
	oNew->bEmpty = TRUE;
	oNew->bFull = FALSE;
	return oNew;
	
}




Block* oGetBlock (int vAbsBlockPos[3]) {
	
	return oGetOrCreateBlock(vAbsBlockPos, FALSE);
	
}




Block* oCreateBlock (int vAbsBlockPos[3]) {
	
	return oGetOrCreateBlock(vAbsBlockPos, TRUE);
	
}




static Block* oGetOrCreateBlock (int vAbsBlockPos[3], Boolean bCreate) {
	
	Boolean bLog = TRUE;
	
	if (bLog) {
		if (bCreate) {
			printf("oCreateBlock(%d,%d,%d)\n", vAbsBlockPos[0], vAbsBlockPos[1], vAbsBlockPos[2]);
		} else {
			printf("oGetBlock(%d,%d,%d)\n", vAbsBlockPos[0], vAbsBlockPos[1], vAbsBlockPos[2]);
		}
	}
	
	Container* oContainer = oRootContainer;
	
	if (bLog) vLogContainer(oContainer);
	
	while (1) {
		
		int iCBL = iGetContainerEdgeBlockLength(oContainer);
		
		Boolean bContainsBlock = TRUE;
		FOR (iIA, 3) {
			bContainsBlock &= vAbsBlockPos[iIA] > oContainer->vAbsBlockPos[iIA];
			bContainsBlock &= vAbsBlockPos[iIA] < oContainer->vAbsBlockPos[iIA] + iCBL;
		}
		if (bContainsBlock) break;
		
		if (!oContainer->oParent) {
			if (!bCreate) return NULL;
			if (bLog) printf("creating parent container\n");
			oContainer->oParent = oNewContainer();
			oContainer->oParent->iLevel = oContainer->iLevel + 1;
			oContainer->oParent->bEmpty = oContainer->bEmpty;
			FOR (iIB, 3) {
				oContainer->vPosInC[iIB] = CONTAINER_EDGE / 2;
				oContainer->oParent->vAbsBlockPos[iIB] = oContainer->vAbsBlockPos[iIB];
				oContainer->oParent->vAbsBlockPos[iIB] -= iCBL * oContainer->vPosInC[iIB];
			}
			oContainer->oParent->aChildren[iGetContainerArrayOffset(oContainer->vPosInC)] = oContainer;
			oRootContainer = oContainer->oParent;
		}
		
		oContainer = oContainer->oParent;
		
		if (bLog) vLogContainer(oContainer);
	
	}
	
	while (oContainer->iLevel > 0) {
		
		int iCBL = iGetContainerEdgeBlockLength(oContainer);
		int iChildCBL = iPow(CONTAINER_EDGE, oContainer->iLevel);
		
		int vPosInC[3];
		FOR (iIC, 3) {
			vPosInC[iIC] = (vAbsBlockPos[iIC] - oContainer->vAbsBlockPos[iIC]) / iChildCBL;
		}
		
		int iOffset = iGetContainerArrayOffset(vPosInC);
		if (!oContainer->aChildren[iOffset]) {
			if (!bCreate) return NULL;
			if (bLog) printf("creating child container\n");
			Container* oNewChild = oNewContainer();
			oContainer->aChildren[iOffset] = oNewChild;
			vCopyPos(vPosInC, oNewChild->vPosInC);
			FOR (iID, 3) {
				oNewChild->vAbsBlockPos[iID] = oContainer->vAbsBlockPos[iID];
				oNewChild->vAbsBlockPos[iID] += iChildCBL * oNewChild->vPosInC[iID];
			}
			oNewChild->iLevel = oContainer->iLevel - 1;
			oNewChild->oParent = oContainer;
		}
		
		oContainer = oContainer->aChildren[iOffset];
	
		if (bLog) vLogContainer(oContainer);
	
	}
	
	int vPosInC[3];
	FOR (iIE, 3) vPosInC[iIE] = vAbsBlockPos[iIE] - oContainer->vAbsBlockPos[iIE];
	int iOffset = iGetContainerArrayOffset(vPosInC);
	if (!oContainer->aChildren[iOffset]) {
		if (!bCreate) return NULL;
		Block* oNewB = oNewBlock();
		oContainer->aChildren[iOffset] = oNewB;
		vCopyPos(vAbsBlockPos, oNewB->vAbsBlockPos);
		vCopyPos(vPosInC, oNewB->vPosInC);
printf("oGetOrCreateBlock():187\n");
		//oMakeBlockMesh(oNewB);
	}
	
	return oContainer->aChildren[iOffset];
	
}




static void vLogContainer (Container* oContainer) {
	
	char* sLogString = "L%d  (%d,%d,%d)  (%d - %d , %d - %d , %d - %d)  %s\n";
	int iCBLx = iGetContainerEdgeBlockLength(oContainer);
	printf(
		sLogString,
		oContainer->iLevel,
		oContainer->vPosInC[0],
		oContainer->vPosInC[1],
		oContainer->vPosInC[2],
		oContainer->vAbsBlockPos[0],
		oContainer->vAbsBlockPos[0] + iCBLx,
		oContainer->vAbsBlockPos[1],
		oContainer->vAbsBlockPos[1] + iCBLx,
		oContainer->vAbsBlockPos[2],
		oContainer->vAbsBlockPos[2] + iCBLx,
		""
	);
	
}




void oMakeBlockMesh (Block* oBlock) {
	
	if (oBlock->oMesh) o3d_c_vDeleteMesh(oBlock->oMesh);
	
	GLuint aSquareElements[6] = {3, 2, 1, 0, 1, 2};
	Vertex aSquareVertices[4];
	int iVY, iVX, iV;
	for (iVY = -1; iVY <= 1; iVY += 2) {
		for (iVX = -1; iVX <= 1; iVX += 2) {
			int iV = 2 * ((iVY + 1) / 2) + ((iVX + 1) / 2);
			Vertex* oVertex = &aSquareVertices[iV];
			oVertex->aPosition[0] = (GLfloat)iVX / 2.0f;
			oVertex->aPosition[1] = (GLfloat)iVY / 2.0f;
			oVertex->aPosition[2] = 0.5f;
			oVertex->aPosition[3] = 0.0f;
			oVertex->aNormal[0] = 0;
			oVertex->aNormal[1] = 0;
			oVertex->aNormal[2] = 1;
			oVertex->aNormal[3] = 0;
			oVertex->aTexcoord[0] = (iVX + 1) / 2;
			oVertex->aTexcoord[1] = (iVY + 1) / 2;
			oVertex->aSpecular[0] = 1;
			oVertex->aSpecular[1] = 1;
			oVertex->aSpecular[2] = 1;
			oVertex->aSpecular[3] = 1;
			oVertex->nShininess = 1;
		}
	}
	
	GLuint aElements[99 * iPow(BLOCK_EDGE + 1, 3)];
	int iElements = 0;
	Vertex aVertices[99 * iPow(BLOCK_EDGE + 1, 3)];
	int iVertices = 0;
	
	Vector vDir;
	Vector vTranslation;
	Vector vZero = {0, 0, 0, 0};
	Matrix mProjector;
	
	Vertex* oNewVertex;
	
	AtomValue iNeighborVal;
	AtomValue iAtomVal;
	int vNeighbor[3];
	int iF;
	
	FOR3 (vAtom, BLOCK_EDGE) {
		iAtomVal = oBlock->aAtoms[iGetBlockArrayOffset(vAtom)];
		if (iAtomVal != ATOM_EMPTY && iAtomVal != ATOM_UNDEFINED) {
			vCopyPos(vAtom, vNeighbor);
			FOR (iDir, 3) {
				for (iF = -1; iF <= 1; iF += 2) {
					vNeighbor[iDir] += iF;
					Boolean bNeighborInBlock = (0 < vNeighbor[iDir] && vNeighbor[iDir] < BLOCK_EDGE - 1);
					Boolean bNeighborEmpty = !bNeighborInBlock || oBlock->aAtoms[iGetBlockArrayOffset(vNeighbor)] == ATOM_EMPTY;
					if (bNeighborEmpty) {
						FOR (iNE, 6) {
							aElements[iElements++] = iVertices + aSquareElements[iNE];
						}
						FOR (iABP, 3) {
							vTranslation[iABP] = (GLfloat)oBlock->vAbsBlockPos[iABP];
							vTranslation[iABP] *= (GLfloat)BLOCK_EDGE;
							vTranslation[iABP] += (GLfloat)vAtom[iABP];
							vTranslation[iABP] *= (GLfloat)ATOM_EDGE;
							vTranslation[iABP] += (GLfloat)ATOM_EDGE / 2.0f;
						}
						vDir[0] = 0; vDir[1] = 0; vDir[2] = 0; vDir[3] = 0;
						vDir[iDir] = iF;
						o3d_m_vQuickMatrix(mProjector, vZero, vDir);
						FOR (iNV, 4) {
							oNewVertex = &aVertices[iVertices++];
							*oNewVertex = aSquareVertices[iNV];
							o3d_m_vMultiplyVM(oNewVertex->aNormal, mProjector);
							o3d_m_vMultiplyVM(oNewVertex->aPosition, mProjector);
							FOR (iP, 4) oNewVertex->aPosition[iP] += vTranslation[iP];
						}
					}
					vNeighbor[iDir] -= iF;
				}
			}
		}
	}
	
	if (iElements && iVertices) {
		
		oBlock->oMesh = o3d_c_oCreateMesh(iElements, aElements, iVertices, aVertices, 1);
		
		glBindBuffer(GL_ARRAY_BUFFER, oBlock->oMesh->iVerticesBuffer);
		glBufferData(
			GL_ARRAY_BUFFER,
			oBlock->oMesh->iVertices * sizeof(Vertex), oBlock->oMesh->aVertices,
			GL_STATIC_DRAW
		);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
	}
	
}




void vSetBlockAtom (Block* oBlock, int vPosInB[3], AtomValue iValue) {
	
	oBlock->aAtoms[iGetBlockArrayOffset(vPosInB)] = iValue;
	
}




void vRefreshBlockFlags (Block* oBlock) {
	
	Boolean bEmpty = TRUE;
	Boolean bFull = TRUE;
	
	AtomValue iAtom;
	
	FOR (iA, BLOCK_SIZE) {
		iAtom = oBlock->aAtoms[iA];
		if (iAtom != ATOM_EMPTY && iAtom != ATOM_UNDEFINED) {
			bEmpty = FALSE;
			if (!bFull) break;
		}
		if (iAtom == ATOM_EMPTY) {
			bFull = FALSE;
			if (!bEmpty) break;
		}
	}
	
	Boolean bOldEmpty = oBlock->bEmpty;
	Boolean bOldFull = oBlock->bFull;
	oBlock->bEmpty = bEmpty;
	oBlock->bFull = bFull;
	
	if (bEmpty != bOldEmpty || bFull != bOldFull) {
		vRefreshContainerFlags(oBlock->oContainer, !bFull, !bEmpty);
	}
	
}




static void vRefreshContainerFlags (Container* oContainer, Boolean bNotFull, Boolean bNotEmpty) {
	
	Boolean bEmpty = !bNotEmpty;
	Boolean bFull = !bNotFull;
	
	if (!bNotEmpty || !bNotFull) {
		FOR (iC, CONTAINER_SIZE) {
			/// TODO
		}
	}
	
}




Block* oGetNeighborBlock (Block* oBlock, int vDir[3]) {
	
	int iAbsSum = 0;
	char *sErrorPrefix = "ERROR: oGetNeighborBlock()";
	FOR (iD, 3) {
		if (!(-1 <= vDir[iD] && vDir[iD] <= 1)) {
			printf("%s: vDir[%d] must be between -1 and 1, but is %d\n", sErrorPrefix, iD, vDir[iD]); exit;
		}
	}
	if (iAbsSum > 1) {
		printf("%s: of vDir[0], vDir[1], vDir[2] two must be 0\n", sErrorPrefix); exit;
	}
	
	Container* oContainer = oBlock->oContainer;
	
	if (!oContainer) return NULL;
	
	int aB[3];
	FOR (iB, 3) {
		aB[iB] = oBlock->vPosInC[iB] + vDir[iB];
	}
	
	FOR (iI, 3) {
		if (!(0 <= aB[iI] && aB[iI] < CONTAINER_EDGE)) {
			if (aB[iI] < 0) aB[iI] += CONTAINER_EDGE;
			else aB[iI] -= CONTAINER_EDGE;
			oContainer = oGetNeighborContainer(oContainer, vDir);
			break;
		}
	}
	
	if (!oContainer) return NULL;
	
	return oContainer->aChildren[iGetBlockArrayOffset(aB)];
	
}




Container* oGetNeighborContainer (Container* oContainer, int vDir[3]) {
	
	int iAbsSum = 0;
	char *sErrorPrefix = "ERROR: oGetNeighborContainer()";
	FOR (iD, 3) {
		if (!(-1 <= vDir[iD] && vDir[iD] <= 1)) {
			printf("%s: vDir[%d] must be between -1 and 1, but is %d\n", sErrorPrefix, iD, vDir[iD]); exit;
		}
	}
	if (iAbsSum > 1) {
		printf("%s: of vDir[0], vDir[1], vDir[2] two must be 0\n", sErrorPrefix); exit;
	}
	
	
	Container* oParent = oContainer->oParent;
	
	if (!oParent) return NULL;
	
	int aC[3];
	FOR (iC, 3) {
		aC[iC] = oContainer->vPosInC[iC] + vDir[iC];
	}
	
	FOR (iI, 3) {
		if (!(0 <= aC[iI] && aC[iI] < CONTAINER_EDGE)) {
			if (aC[iI] < 0) aC[iI] += CONTAINER_EDGE;
			else aC[iI] -= CONTAINER_EDGE;
			oContainer = oGetNeighborContainer(oContainer, vDir);
			break;
		}
	}
	
	if (!oParent) return NULL;
	
	return oParent->aChildren[iGetContainerArrayOffset(aC)];
	
}




//void vPutBlockIntoContainer (Container* oContainer, Block* oBlock, int vPosInC[3]) {
//	
//	oContainer->aChildren[iGetContainerArrayOffset(vPosInC)] = oBlock;
//	oBlock->oContainer = oContainer;
//	vCopyPos(vPosInC, oBlock->vPosInC);
//	
//}




static int iGetContainerEdgeBlockLength (Container* oContainer) {
	
	return iPow(CONTAINER_EDGE, oContainer->iLevel + 1);
	
}




static int iGetContainerArrayOffset (int vPosInC[3]) {
	
	return CONTAINER_EDGE * CONTAINER_EDGE * vPosInC[2] + CONTAINER_EDGE * vPosInC[1] + vPosInC[0];
	
}




static int iGetBlockArrayOffset (int vPosInB[3]) {
	
	return BLOCK_EDGE * BLOCK_EDGE * vPosInB[2] + BLOCK_EDGE * vPosInB[1] + vPosInB[0];
	
}




static void vCopyPos (int vSource[3], int vTarget[3]) {
	
	FOR (iI, 3) vTarget[iI] = vSource[iI];
	
}




static int iPow (int iInt, int iPow) {
	
	int iReturn = 1;
	FOR (iI, iPow) iReturn *= iInt;
	return iReturn;
	
}



