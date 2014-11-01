

struct mesh* o3d_mh_oRectangle (GLfloat iW, GLfloat iH, int iResW, int iResH) {
	
	int iVertices = (iResW * iResH);
	int iElements = (6 * (iResW - 1) * (iResH - 1));
	struct mesh *oMesh = o3d_c_oMakeMesh(iElements, iVertices);
	
	GLsizei iSquareNr = -1;
	GLsizei iSquareX, iSquareY, iStartIndex, iVA, iVB, iVC, iVD;
	for (iSquareY = 0; iSquareY < iResH - 1; iSquareY ++) {
		for (iSquareX = 0; iSquareX < iResW - 1; iSquareX ++) {
			
			iSquareNr ++;
			iStartIndex = 6 * iSquareNr;
			
			iVA = iResW * iSquareY + iSquareX;
			iVB = iVA + 1;
			iVC = iVA + iResW;
			iVD = iVA + iResW + 1;
			
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

