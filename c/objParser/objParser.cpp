#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#define MESH_FILE_LINE_BUFFER_SIZE 256
#define NUMBER_OF_VERTEX_COORDINATES 3
#define NUMBER_OF_TEXTURE_COORDINATES 2
#define NUMBER_OF_NORMAL_COORDINATES 3
#define NUMBER_OF_FACE_TOKENS 3

typedef struct MeshData {
    // ‘>>’ should be ‘> >’ within a nested template argument list
    std::vector< std::vector<float> > vertices;
    std::vector< std::vector<float> > textures;
    std::vector< std::vector<float> > normals;
    std::vector< std::vector<int> > faceTriangles;
    std::vector< std::vector<int> > faceTextures;
    std::vector< std::vector<int> > faceNormals;
} MeshData;

void loadMeshData(char *fileName, MeshData *data);

int main(int argc, char *argv[])
{
    if(argc == 2)
    {
        MeshData data;
        loadMeshData(argv[1], &data);
    }
    else
    {
        printf("\nMissing mesh file.\n");
        printf("Usage:\n");
        printf("\t%s fileName.obj\n", argv[0]);
    }

    return EXIT_SUCCESS;
}

void loadMeshData(char *fileName, MeshData *data)
{
    FILE *meshFile = fopen(fileName, "r");

    if(meshFile == NULL)
    {
        printf("Not able to open mesh file: %s", fileName);
        exit(EXIT_FAILURE);
    }

    const char * const separatorSpace = " ";
    const char * const separatorForwardShash = "/";
    char *firstToken = NULL;
    char *token = NULL;
    char *tokenVertexIndex = NULL;
    char *tokenTextureIndex = NULL;
    char *tokenNormalIndex = NULL;
    char *faceTokens[NUMBER_OF_FACE_TOKENS] = {NULL};
    char line[MESH_FILE_LINE_BUFFER_SIZE] = {'\0'};

    int numberOfTokens = 0;

    while(fgets(line, MESH_FILE_LINE_BUFFER_SIZE, meshFile) != NULL)
    {
        firstToken = strtok(line, separatorSpace);

        if(strcmp(firstToken, "v") == 0)
        {
            std::vector<float> vectorPointCoordinate(NUMBER_OF_VERTEX_COORDINATES);

            for(int counter = 0; counter < NUMBER_OF_VERTEX_COORDINATES; ++counter)
            {
                vectorPointCoordinate[counter] = atof(strtok(NULL, separatorSpace));
            }

            data->vertices.push_back(vectorPointCoordinate);
        }
        else if(strcmp(firstToken, "vt") == 0)
        {
            std::vector<float> vectorTextureCoordinate(NUMBER_OF_TEXTURE_COORDINATES);

            for(int counter = 0; counter < NUMBER_OF_TEXTURE_COORDINATES; ++counter)
            {
                vectorTextureCoordinate[counter] = atof(strtok(NULL, separatorSpace));
            }

            data->textures.push_back(vectorTextureCoordinate);
        }
        else if(strcmp(firstToken, "vn") == 0)
        {
            std::vector<float> vectorNormalCoordinate(NUMBER_OF_NORMAL_COORDINATES);

            for(int counter = 0; counter < NUMBER_OF_NORMAL_COORDINATES; ++counter)
            {
                vectorNormalCoordinate[counter] = atof(strtok(NULL, separatorSpace));
            }

            data->normals.push_back(vectorNormalCoordinate);
        }
        else if(strcmp(firstToken, "f") == 0)
        {
            std::vector<int> triangleVertexIndices(3);
            std::vector<int> textureVertexIndices(3);
            std::vector<int> normalVertexIndices(3);

            memset((void *)faceTokens, 0, NUMBER_OF_FACE_TOKENS);
            numberOfTokens = 0;

            while(token = strtok(NULL, separatorSpace))
            {
                if(strlen(token) < 3)
                {
                    break;
                }

                faceTokens[numberOfTokens] = token;
                numberOfTokens++;
            }

            for(int counter = 0; counter < NUMBER_OF_FACE_TOKENS; ++counter)
            {
                tokenVertexIndex = strtok(faceTokens[counter], separatorForwardShash);
                tokenTextureIndex = strtok(NULL, separatorForwardShash);
                tokenNormalIndex = strtok(NULL, separatorForwardShash);
                triangleVertexIndices[counter] = atoi(tokenVertexIndex);
                textureVertexIndices[counter] = atoi(tokenTextureIndex);
                normalVertexIndices[counter] = atoi(tokenNormalIndex);
            }

            data->faceTriangles.push_back(triangleVertexIndices);
            data->faceTextures.push_back(textureVertexIndices);
            data->faceNormals.push_back(normalVertexIndices);
        }

        memset((void *)line, (int)'\0', MESH_FILE_LINE_BUFFER_SIZE);
    }

    fclose(meshFile);
    meshFile = NULL;

    printf("--------------------\n");
    printf("Mesh file: %s\n", fileName);
    printf("Vertices: %lu\n", data->vertices.size());
    printf("Textures: %lu\n", data->textures.size());
    printf("Normals: %lu\n", data->normals.size());
    printf("Face triangles: %lu\n", data->faceTriangles.size());
    printf("Face textures: %lu\n", data->faceTextures.size());
    printf("Face normals: %lu\n", data->faceNormals.size());
    printf("--------------------\n");
}
