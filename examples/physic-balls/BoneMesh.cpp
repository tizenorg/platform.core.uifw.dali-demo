#include <cstdio>
#include <sstream>
#include <fstream>

#include "BoneMesh.h"

//using namespace Dali::Toolkit;

const char*const SIMPLE_VERTEX_SHADER = DALI_COMPOSE_SHADER (
  void main()\n
  {\n
    gl_Position = uMvpMatrix * vec4( aPosition.xz, 0.0, 1.0 );\n
  }\n
);

const char*const SIMPLE_FRAG_SHADER = DALI_COMPOSE_SHADER (
  precision mediump float;\n
  void main()\n
  {\n
    gl_FragColor = vec4(uMaterial.mDiffuse.x,uMaterial.mDiffuse.y,uMaterial.mDiffuse.z,1.0);\n
  }\n
);



BoneMesh::BoneMesh( )
{

}
BoneMesh::~BoneMesh()
{

}

void BoneMesh::LoadDataFile(const std::string& objFileName)
{
  int number, vtxNum = 0, idxNum, weightIdxNum = 0;
  int state = 0;
  char type[256];
  int i = 0;

  std::ifstream ifs( objFileName.c_str(), std::ios::in );

  std::string line, tag;
  while( std::getline( ifs, line ) )
  {
    switch(state)
    {
      case 0:
        memset(type,0,sizeof(type));
        sscanf(line.c_str(), "%s %d", type, &number);
        tag = type;
        if( tag == "Vertex")  // vertex
        {
          state = 1;
          vtxNum = number;

          vertexPositions.reserve(vtxNum);
        }
        else if( tag == "Triangles")  // Triangle idxa
        {
          state = 2;
          i = 0;
          idxNum = number * 3 * 2;

          faceIndices.resize(idxNum);
        }
        else if( tag == "Bind_Matrix")  // Bind matrix pose
        {
          state = 3;
        }
        else if( tag == "Bones")  // Bone pose
        {
          state = 4;
          mInverseBindMat.reserve(number);
        }
        else if( tag == "Weights")  // Array of Weights
        {
          state = 5;
          i = 0;

          weights.resize(number);
        }
        else if( tag == "Number_index_x_vertex")  // Number of indices per vertex
        {
          state = 6;
          i = 0;

          indexNumVertex.resize(vtxNum);
        }
        else if( tag == "Indexes_to_weights")  // Indexes to the weight array
        {
          weightIdxNum = 0;
          for (unsigned int ui = 0 ; ui < indexNumVertex.size() ; ui++)
            weightIdxNum += indexNumVertex[ui];

          weightIdxNum *= 2;
          state = 7;
          i = 0;
          indexToWeight.resize(weightIdxNum);
        }
        else if( tag == "Matrixs")  // Matrix of the joint in the 0 instant
        {
          state = 8;
          mPositionMat.reserve(number);
        }
        break;
      case 1: //vertices
      {
        float X,Y,Z;
        sscanf(line.c_str(), "%f %f %f", &X, &Y, &Z);
        vertexPositions.push_back(Vector3(X, Y, Z));

        number--;
        if (number < 1)
          state = 0;
        break;
      }
      case 2: //faces
      {
        std::istringstream iss(line, std::istringstream::in);
        while( iss >> faceIndices[i++] && i < idxNum);
        if (i >= idxNum)
          state = 0;
        break;
      }
      case 3: //bind matrix
      {
        sscanf(line.c_str(), "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
               &mBindMat[0], &mBindMat[1], &mBindMat[2], &mBindMat[3], &mBindMat[4], &mBindMat[5], &mBindMat[6], &mBindMat[7],
               &mBindMat[8], &mBindMat[9], &mBindMat[10], &mBindMat[11], &mBindMat[12], &mBindMat[13], &mBindMat[14], &mBindMat[15]);
        state = 0;
        break;
      }
      case 4:
      {
        float matrix[16];
        char boneName[256];
        sscanf(line.c_str(), "%s %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", boneName,
               &matrix[0], &matrix[1], &matrix[2], &matrix[3], &matrix[4], &matrix[5], &matrix[6], &matrix[7],
               &matrix[8], &matrix[9], &matrix[10], &matrix[11], &matrix[12], &matrix[13], &matrix[14], &matrix[15]);

        Bone_t bone;
        bone.name = boneName;
        memcpy(bone.matrix,matrix,sizeof(matrix));
        mInverseBindMat.push_back(bone);

        number--;
        if (number < 1)
          state = 0;

        break;
      }
      case 5: //array of weights
      {
        std::istringstream iss(line, std::istringstream::in);
        while( iss >> weights[i++] && i < number);
        if (i >= number)
          state = 0;
        break;
      }
      case 6: //array with the number of indices per vertex
      {
        std::istringstream iss(line, std::istringstream::in);
        while( iss >> indexNumVertex[i++] && i < vtxNum);
        if (i >= vtxNum)
          state = 0;
        break;
      }
      case 7: //indices to the weight array
      {
        std::istringstream iss(line, std::istringstream::in);
        while( iss >> indexToWeight[i++] && i < weightIdxNum);
        if (i >= weightIdxNum)
          state = 0;
        break;
      }
      case 8:
      {
        float matrix[16];
        char boneName[256];
        sscanf(line.c_str(), "%s %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", boneName,
               &matrix[0], &matrix[1], &matrix[2], &matrix[3], &matrix[4], &matrix[5], &matrix[6], &matrix[7],
               &matrix[8], &matrix[9], &matrix[10], &matrix[11], &matrix[12], &matrix[13], &matrix[14], &matrix[15]);

        Bone_t bone;
        bone.name = boneName;
        memcpy(bone.matrix,matrix,sizeof(matrix));
        mPositionMat.push_back(bone);

        number--;
        if (number < 1)
          state = 0;

        break;
      }
    }
  }
  ifs.close();

  //PrintInfo();
}

void BoneMesh::CreateActor()
{
  Matrix bindMat(mBindMat);

  MeshData::VertexContainer vertices(vertexPositions.size());
  //vertices of the circle

  int iCount = 0;
  for (int i = 0 ; i < (int)vertexPositions.size() ; i++)
  {
    //vertex in bind pose
    Vector3 vec(vertexPositions[i].x,vertexPositions[i].y,vertexPositions[i].z);

    //process vertex with bones
    int idxNum = indexNumVertex[i];
    float sumWeight = 0;
    Vector3 vecSkin(0,0,0);
    for (int j = 0 ; j < idxNum ; j++)
    {
      int idxBone = indexToWeight[iCount*2];
      int idxWeight = indexToWeight[iCount*2+1];
      float weight = weights[idxWeight];

      //Vector3 vertBind = (vec + mInverseBindMat[idxBone].cp) * weight;
      //vecSkin += (vertBind + mPositionMat[idxBone].cp) * weight;
      Matrix boneMat(mInverseBindMat[idxBone].matrix);
      Vector3 icp(mInverseBindMat[idxBone].matrix[3],mInverseBindMat[idxBone].matrix[7],mInverseBindMat[idxBone].matrix[11]);
      idxBone = mPositionMat.size() - idxBone;
      Vector3 cp(mPositionMat[idxBone].matrix[3],mPositionMat[idxBone].matrix[7],mPositionMat[idxBone].matrix[11]);
      Vector3 tmp = (vec - icp + cp);
      vecSkin += tmp * weight;

      sumWeight += weight;
      iCount++;
    }

    vertices[i] = MeshData::Vertex( vecSkin, Vector2(0, 0), Vector3(0.0f, 1.f, 0.f) );
  }

  // Specify all the faces radius()
  MeshData::FaceIndices faces;
  faces.reserve( faceIndices.size() * 0.5 + 1 ); // 2 triangles in Quad
  for (int i = 0 ; i < (int)faceIndices.size() ; i += 6)
  {
    int idx1 = faceIndices[i];
    int idx2 = faceIndices[i+2];
    int idx3 = faceIndices[i+4];

    faces.push_back( idx1 ); faces.push_back( idx2 ); faces.push_back( idx3 );
  }

  Vector4 color(0,0,1,1);
  Material baseMaterial = Material::New( "Material1" );
  baseMaterial.SetDiffuseColor( color );

  // Create the mesh data from the vertices and faces
  MeshData meshData;
  meshData.SetMaterial( baseMaterial );
  meshData.SetVertices( vertices );
  meshData.SetFaceIndices( faces );
  meshData.SetHasTextureCoords( false );
  meshData.SetHasNormals( false );

  // Create a mesh from the data
  Dali::Mesh mesh = Mesh::New( meshData );

  mActor = MeshActor::New( mesh );
  mActor.SetScale( 10.0f );
  mActor.SetPosition(Vector3(0, 0.0f, 0.1f));
  mActor.SetParentOrigin( ParentOrigin::CENTER );

  ShaderEffect shader = ShaderEffect::New( SIMPLE_VERTEX_SHADER, SIMPLE_FRAG_SHADER, GEOMETRY_TYPE_UNTEXTURED_MESH  );
  mActor.SetShaderEffect(shader);
}

void BoneMesh::PrintInfo()
{
  printf("Data vtxnum %d facenum %d weightnum %d index %d idxToWeight %d\n", (int)vertexPositions.size(),
         (int)faceIndices.size(), (int)weights.size(), (int)indexNumVertex.size(), (int)indexToWeight.size());

  for (unsigned int ui = 0 ; ui < vertexPositions.size() ; ui++)
    printf("Vertex %f %f %f\n",vertexPositions[ui].x,vertexPositions[ui].y,vertexPositions[ui].z);

  for (unsigned int ui = 0 ; ui < faceIndices.size() ; ui+=6)
    printf("Face %d %d %d\n",faceIndices[ui],faceIndices[ui+2],faceIndices[ui+4]);

  printf("Weight\n");
  for (unsigned int ui = 0 ; ui < weights.size() ; ui++)
    printf("%f ",weights[ui]);
  printf("\n");

  printf("Index num per vertex\n");
  for (unsigned int ui = 0 ; ui < indexNumVertex.size() ; ui++)
    printf("%d ",indexNumVertex[ui]);
  printf("\n");

  printf("Index to weight\n");
  for (unsigned int ui = 0 ; ui < indexToWeight.size() ; ui++)
    printf("%d ",indexToWeight[ui]);
  printf("\n");

  printf("Bind matrix\n");
  printf("%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
         mBindMat[0], mBindMat[1], mBindMat[2], mBindMat[3], mBindMat[4], mBindMat[5], mBindMat[6], mBindMat[7],
         mBindMat[8], mBindMat[9], mBindMat[10], mBindMat[11], mBindMat[12], mBindMat[13], mBindMat[14], mBindMat[15]);
  printf("\n");

  printf("Bind matrix Bones\n");
  for (unsigned int ui = 0 ; ui < mInverseBindMat.size() ; ui++)
    printf("%s %f %f %f %f\n",mInverseBindMat[ui].name.c_str(),
           mInverseBindMat[ui].matrix[3],mInverseBindMat[ui].matrix[7],mInverseBindMat[ui].matrix[11],mInverseBindMat[ui].matrix[15]);
  printf("\n");

  printf("Bones\n");
  for (unsigned int ui = 0 ; ui < mPositionMat.size() ; ui++)
    printf("%s %f %f %f %f\n",mPositionMat[ui].name.c_str(),
           mPositionMat[ui].matrix[3],mPositionMat[ui].matrix[7],mPositionMat[ui].matrix[11],mPositionMat[ui].matrix[15]);
  printf("\n");

}

/*

int solve3System(float x, float y, float z, float re, float x2, float y2, float z2, float re2,
                 float x3, float y3, float z3, float re3, float &X, float &Y, float &Z)
{
  float deter;

  deter= x*y2*z3 +x2*y3*z +x3*y*z2 -z*y2*x3 -z2*y3*x -z3*y*x2 ;

  X= (re*y2*z3 +re2*y3*z +re3*y*z2 -z*y2*re3 -z2*y3*re -z3*y*re2)/deter ;

  Y= (x*re2*z3 +x2*re3*z +x3*re*z2 -z*re2*x3 -z2*re3*x -z3*re*x2 )/deter ;

  Z= (x*y2*re3 +x2*y3*re +x3*y*re2 -re*y2*x3 -re2*y3*x -re3*y*x2)/deter ;

  return 0;
}


*/
