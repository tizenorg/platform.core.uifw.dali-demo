#ifndef DALI_DEMO_BONE_MESH_H
#define DALI_DEMO_BONE_MESH_H

#include <dali-toolkit/dali-toolkit.h>

#include <string>
#include <vector>

using namespace Dali;

struct Bone_t
{
  std::string name;
  float matrix[16];
  Vector3 cp;
};

class BoneMesh
{
public:
  BoneMesh( );
  ~BoneMesh();
  
  void LoadDataFile(const std::string& objFileName);
  void CreateActor();
  
  MeshActor & GetActor() { return mActor; } 
  
  void PrintInfo();
  
private:
  
  MeshActor         mActor;
  
  std::vector<Vector3> vertexPositions;
  std::vector<int> faceIndices;
  std::vector<float> weights;
  std::vector<int> indexNumVertex;
  std::vector<int> indexToWeight;
  
  float mBindMat[16];
  std::vector<Bone_t> mInverseBindMat;
  std::vector<Bone_t> mPositionMat;
};

#endif