#include "marchingcube.h"
#include <cmath>
#include <FL/gl.h>

MarchingCube::MarchingCube(int x, int y, int z, double cubeSize)
	:x(x), y(y), z(z), cubeSize(cubeSize)
{
	vertices = new Vertex[x * y * z];

	int index = 0;
	for (int k = 0; k < z; k++) {
		for (int j = 0; j < y; j++) {
			for (int i = 0; i < x; i++) {
				vertices[index].point[0] = i * cubeSize - cubeSize / 2;
				vertices[index].point[1] = j * cubeSize - cubeSize / 2;
				vertices[index].point[2] = k * cubeSize - cubeSize / 2;
				index++;
			}
		}
	}
}

MarchingCube::~MarchingCube() {
	delete[] vertices;
}

Vertex MarchingCube::interpVertex(const Vertex& v1, const Vertex& v2, double isoLevel) {
	Vertex v;

	if (abs(isoLevel - v1.value) < 1e-5) return v1;
	if (abs(isoLevel - v2.value) < 1e-5) return v2;
	if (abs(v2.value - v1.value) < 1e-5) return v1;

	double c = (isoLevel - v1.value) / (v2.value - v1.value);

	v.point[0] = v1.point[0] + c * (v2.point[0] - v1.point[0]);
	v.point[1] = v1.point[1] + c * (v2.point[1] - v1.point[1]);
	v.point[2] = v1.point[2] + c * (v2.point[2] - v1.point[2]);

	v.normal[0] = v1.normal[0] + c * (v2.normal[0] - v1.normal[0]);
	v.normal[1] = v1.normal[1] + c * (v2.normal[1] - v1.normal[1]);
	v.normal[2] = v1.normal[2] + c * (v2.normal[2] - v1.normal[2]);

	return v;
}

void MarchingCube::drawCube(int i, int j, int k, double isoLevel) {
	if (i >= x-1 || j >= y-1 || k >= z-1) return;

	// The index of vertices in the cube
	int vertexIndex[8] = {
		i + j * x + k * x * y , (i + 1) + j * x + k * x * y ,
		(i + 1) + j * x + (k + 1) * x * y, i + j * x + (k + 1) * x * y,
		i + (j + 1) * x + k * x * y, (i + 1) + (j + 1) * x + k * x * y,
		(i + 1) + (j + 1) * x + (k + 1) * x * y, i + (j + 1) * x + (k + 1) * x * y
	};
	// The vertices lying on the edges of the cube and to be drawn
	Vertex edgeVertices[12];

	// Magic steps following the reference site
	int cubeIndex = 0;
	if (vertices[vertexIndex[0]].value < isoLevel) cubeIndex |= 1;
	if (vertices[vertexIndex[1]].value < isoLevel) cubeIndex |= 2;
	if (vertices[vertexIndex[2]].value < isoLevel) cubeIndex |= 4;
	if (vertices[vertexIndex[3]].value < isoLevel) cubeIndex |= 8;
	if (vertices[vertexIndex[4]].value < isoLevel) cubeIndex |= 16;
	if (vertices[vertexIndex[5]].value < isoLevel) cubeIndex |= 32;
	if (vertices[vertexIndex[6]].value < isoLevel) cubeIndex |= 64;
	if (vertices[vertexIndex[7]].value < isoLevel) cubeIndex |= 128;

	int edgeCube = edgeTable[cubeIndex];
	if (edgeCube == 0) return;
	for (int i = 0; i < 12; i++) {
		if (edgeCube & (1 << i)) {
			edgeVertices[i] = interpVertex(vertices[vertexIndex[endPointTable[i][0]]], vertices[vertexIndex[endPointTable[i][1]]], isoLevel);
		}
	}

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_NORMALIZE);

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 12; i+=3) {
		if (triTable[cubeIndex][i] == -1) continue;
		glNormal3dv(edgeVertices[triTable[cubeIndex][i]].normal.getPointer());
		glVertex3dv(edgeVertices[triTable[cubeIndex][i]].point.getPointer());
		glNormal3dv(edgeVertices[triTable[cubeIndex][i+1]].normal.getPointer());
		glVertex3dv(edgeVertices[triTable[cubeIndex][i+1]].point.getPointer());
		glNormal3dv(edgeVertices[triTable[cubeIndex][i+2]].normal.getPointer());
		glVertex3dv(edgeVertices[triTable[cubeIndex][i+2]].point.getPointer());
	}
	glEnd();

}

void MarchingCube::draw(double isoLevel) {
	for (int k = 0; k < z-1; k++) {
		for (int j = 0; j < y-1; j++) {
			for (int i = 0; i < x-1; i++) {
				drawCube(i, j, k, isoLevel);
			}
		}
	}
}

const int MarchingCube::getNumVertex() {
	return x * y * z;
}

const Vec3i MarchingCube::getXYZ() {
	return Vec3i{x, y, z};
}

const double MarchingCube::getCubeSize() {
	return cubeSize;
}