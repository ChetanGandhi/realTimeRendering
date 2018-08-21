#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

struct SphereData
{
	float *vertices;
	float *normals;
	float *textureCoordinates;
	short *elements;
	int numberOfVertices;
	int numberOfElements;
};

class Sphere
{
private:
	int maxElements;
    int numElements;
    int numVertices;

    float model_vertices[1146];
    float model_normals[1146];
    float model_textures[764];
    short model_elements[2280];

	void processSphereData(short indices[760][9], float vertices[382][3], float normals[382][3], float textures[1][2]);
    void addTriangle(float single_vertex[3][3], float single_normal[3][3], float single_texture[3][3]);
    void normalizeVector(float v[3]);
    bool isFoundIdentical(float val1, float val2, float diff);

public:
	Sphere();

	void getSphereVertexData(SphereData *data);
};