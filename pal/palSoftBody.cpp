#include "palSoftBody.h"

int *palTetrahedralSoftBody::ConvertTetrahedronToTriangles(const int *pIndices, const int nIndices) {
	int *iout = new int[(nIndices/4)*12];
	int x =0;
	for (int i=0;i<nIndices/4;i++) {
		int p0 = pIndices[i*4];
		int p1 = pIndices[i*4+1];
		int p2 = pIndices[i*4+2];
		int p3 = pIndices[i*4+3];
	//f0
		iout[x++] = p2;
		iout[x++] = p1;
		iout[x++] = p0;
	//f1
		iout[x++] = p0;
		iout[x++] = p1;
		iout[x++] = p3;
	//f2
		iout[x++] = p1;
		iout[x++] = p2;
		iout[x++] = p3;
	//f3
		iout[x++] = p2;
		iout[x++] = p0;
		iout[x++] = p3;
	}
	return iout;
}
