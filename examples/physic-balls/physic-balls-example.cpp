/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <dali-toolkit/dali-toolkit.h>
#include "shared/view.h"

#include <cstdio>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include "BoneMesh.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char * const BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-2.jpg" );
const char * const REFLECTION_IMAGE( DALI_IMAGE_DIR "Angular-Reflection-Map.png" );
//const char * const REFLECTION_IMAGE( DALI_IMAGE_DIR "angmap1.jpg" );
//const char * const REFLECTION_IMAGE( DALI_IMAGE_DIR "CornwallSunset_Probe.jpg" );
const char * const TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );

const int BONE_NUMBER(24);
const int VTX_NUMBER(720);
const int CP_DISTANCE_COLLISION(7);

const float MAX_JOINT_DISTANCE(1.2f);
const float MIN_JOINT_DISTANCE(0.6f);

const float SPRING_STRENGTH(7.5f);

const bool BALLS_MOVING(true);

const float MAX_ACTIVE_RADIUS(0.285f);
const float MAX_STILL_RADIUS(0.165f);

//const char* MESH_FILES[] =
//{
// DALI_MODEL_DIR "BoneBlob.dat"
//};

}

const char*const VERTEX_SHADER = DALI_COMPOSE_SHADER (
  uniform vec3 controlPoints[96+1];\n
  uniform vec3 center;\n
  uniform vec2 screenSize;
  uniform float boneNumber;\n
  uniform float vtxPerBone;\n
  uniform float radius;
  varying vec3 color;\n
  varying vec2 newPointPos;\n

  vec3 CubicInterpolate( vec3 p0, vec3 p1, vec3  p2, vec3  p3, float progress )\n
  {\n
    vec3 result;
    vec3 a3 = p3*0.5 - p2*1.5 + p1*1.5 - p0*0.5;\n
    vec3 a2 = p0 - p1*2.5 + p2*2.0 - p3*0.5;\n
    vec3 a1 = (p2-p0)*0.5;\n

    result = a3*progress*progress*progress + a2*progress*progress + a1*progress + p1;\n
    return result;\n
  }\n

  void main()\n
  {\n
    float thirdbone = mod(aTexCoord.x - 1.0, boneNumber);\n
    float fourthbone = mod(aTexCoord.y + 1.0, boneNumber);\n

    if (aNormal.y > 0.0)\n
    {\n
      float interp = aNormal.x / vtxPerBone;\n

      vec3 position = CubicInterpolate(controlPoints[int(thirdbone)], controlPoints[int(aTexCoord.x)],
                                       controlPoints[int(aTexCoord.y)], controlPoints[int(fourthbone)],
                                       interp);\n
      newPointPos = position.xy;\n
    }\n
    else\n
    {\n
      newPointPos = center.xy;\n
    }\n

    vTexCoord.x = (newPointPos.x + (screenSize.x * 0.5)) / screenSize.x;\n
    vTexCoord.y = (newPointPos.y + (screenSize.y * 0.5)) / screenSize.y;\n

    vec2 dif = newPointPos - center.xy;\n
    float len = length(dif);\n
    if (len > 0.5)\n
      color = vec3(1.0);\n
    else\n
      color = vec3(0);\n

    gl_Position = uMvpMatrix * vec4( newPointPos.xy, 0.0, 1.0 );\n
  }\n
);

const char*const FRAG_SHADER = DALI_COMPOSE_SHADER (
  precision mediump float;\n
  varying vec3 color;\n
  varying vec2 newPointPos;\n

  const float Eta = 0.95;\n
  const float FresnelPower = 0.7;\n
  const float F = ((1.0-Eta) * (1.0-Eta)) / ((1.0+Eta ) * (1.0+Eta));\n

  void main()\n
  {\n
    vec3 normal;
    vec2 zoomCoords = (vTexCoord - 0.5) * 1.1;\n

    if (color.r > 0.8)\n
    {\n
      float texInterp = mix( 1.1, 0.6, (color.r - 0.8) * 5.0);\n
      zoomCoords = (vTexCoord - 0.5) * texInterp;\n

      float len = length(newPointPos);\n
      float normInterp = mix(1.0, 0.0, (color.r - 0.80) * 5.0);\n
      normal = vec3(newPointPos.x * (1.0 - normInterp)/ len, newPointPos.y * (1.0 - normInterp) / len, normInterp);\n
      normal = normalize(normal);\n
    }\n
    else\n
    {\n
      normal = vec3(0.,0.,1.0);\n
    }\n

    zoomCoords.y = -zoomCoords.y;\n
    zoomCoords = zoomCoords + 0.5;\n


    vec3 lightPosition = vec3(-500,-750,1000);
    vec3 vertex = vec3(newPointPos.x,newPointPos.y,0.0);

    vec3 vecToLight = normalize( lightPosition - vertex );\n

    float lightDiffuse = dot( vecToLight, normal );\n
    lightDiffuse = max(0.0,lightDiffuse);\n
    //lightDiffuse = lightDiffuse * 0.5 + 0.5;

    vec3 vertexToEye = normalize(-vertex);\n
    vec3 lightReflect = normalize(reflect(vecToLight, normal));\n
    float specularFactor = dot(vertexToEye, lightReflect);\n
    specularFactor = max(0.0,pow(specularFactor, 32.0));\n

    vec4 tex = texture2D(sTexture, zoomCoords);\n

    float Ratio = F + (1.0 - F) * pow((1.0 - dot(-vertexToEye, normal)), FresnelPower);\n
    //float Ratio = dot(-vertexToEye, normal);\n

    vec3 reflecVect = reflect(vertexToEye, normal);\n

    float M = 2.0 * sqrt(reflecVect.x * reflecVect.x + reflecVect.y * reflecVect.y +
                         (reflecVect.z + 1.0) * (reflecVect.z + 1.0));\n
    float u = reflecVect.x / M + 0.5;\n
    float v = reflecVect.y / M + 0.5;\n
    vec4 reflecTex = texture2D(sEffect, vec2(u,v));\n

    vec3 refracColor = tex.xyz;\n
    vec3 reflecColor = reflecTex.xyz;\n

    vec3 diffuseTerm = vec3(lightDiffuse, lightDiffuse, lightDiffuse);\n
    vec3 ambientTerm = mix(reflecColor, refracColor * uMaterial.mDiffuse.rgb, Ratio);\n
    //vec3 ambientTerm = vec3(Ratio);\n

    gl_FragColor.xyz = ambientTerm * 0.7 + diffuseTerm * 0.3 * uMaterial.mDiffuse.rgb + vec3(specularFactor);\n
    gl_FragColor.a = tex.a;\n


    //gl_FragColor = vec4(reflecColor.x, reflecColor.y, reflecColor.z, 1.0);\n

    //gl_FragColor = vec4(lightDiffuse + 0.1 + specularFactor, lightDiffuse + 0.1 + specularFactor, lightDiffuse + 0.1 + specularFactor, 1.0);\n
    //gl_FragColor = vec4(specularFactor, specularFactor, specularFactor, 1.0);\n

    //gl_FragColor = vec4(normal.x*0.5+0.5, normal.y*0.5+0.5, normal.z*0.5+0.5, 1.0);\n
    //gl_FragColor = vec4(Ratio, Ratio, Ratio, 1.0);\n
	  //gl_FragColor = texture2D(sTexture, vTexCoord);\n
    //gl_FragColor = vec4(uMaterial.mDiffuse.x,uMaterial.mDiffuse.y,uMaterial.mDiffuse.z,1.0);\n
    //gl_FragColor = vec4(tex.r * uMaterial.mDiffuse.x,tex.g*uMaterial.mDiffuse.y,tex.b*uMaterial.mDiffuse.z,1.0);\n
  }\n
);

struct Joint_t
{
  int idx1;
  int idx2;
  float distance;
  float distMin;
  float distMax;
};


//
//
class PhysicsBallsController : public ConnectionTracker
{
public:
  PhysicsBallsController( Application& application );
  ~PhysicsBallsController();

  void Create( Application& app );
  bool OnTouch( Actor actor, const TouchEvent& touch );

  bool OnTimerTick();

private:
  Application&      mApplication;

  Vector2           mScreenSize;

  Toolkit::View     mView;                              ///< The View instance.

  Image             mBackImage;
  Image             mReflectionImage;
  Dali::ImageActor  mCompositionActor;

  MeshActor         mWatch;

  MeshActor         mActive;
  MeshActor         mStill;

  //int               mState;
  std::vector<Vector3>  mActiveControlPoints;
  std::vector<Vector3>  mNewActiveControlPoints;
  std::vector<Vector3>  mStillControlPoints;
  std::vector<Vector3>  mNewStillControlPoints;

  std::vector<Joint_t>  mActiveJoints;
  std::vector<Joint_t>  mStillJoints;

  ShaderEffect      mShaderActive;
  ShaderEffect      mShaderStill;

  //BoneMesh          mData;

  //-----------------------------------
  // Prototyping
  bool        mPause;

  Vector3     mActivePos;
  Vector3     mStillPos;

  float       mActiveRadius;
  float       mNewActiveRadius;
  float       mStillRadius;
  float       mNewStillRadius;

  Vector3     mDirectionActive;
  Vector3     mDirectionStill;

  float       mActiveMass;
  float       mStillMass;

  float       mActiveScale;
  float       mStillScale;

  std::vector<float>    mActiveNeedsUpdate;
  std::vector<Vector3>  mActiveCPForces;
  std::vector<Vector3>  mActiveSpringForces;
  std::vector<Vector3>  mActiveLastFrameSpeeds;

  std::vector<float>    mStillNeedsUpdate;
  std::vector<Vector3>  mStillCPForces;
  std::vector<Vector3>  mStillSpringForces;
  std::vector<Vector3>  mStillLastFrameSpeeds;

  Timer       mTimer;
  float       mTime;

  //For moving the ball
  int         mSelectedBall;
  Vector2     mLastPosition;
  //Functions
  //---------

  //Creation initialization
  void        LoadDataFile(const std::string& objFileName);
  Mesh        CreateCircle(int numberOfVertices, float radius, Vector4 color);
  Mesh        CreateCircleWithBones(int numberOfVertices, int numberOfBones, float radius, Vector4 color,
                                    std::vector<Vector3>& controlPoints);
  void        CreateJoints(std::vector<Joint_t>& joints, int boneNumber);

  //Simulation

  bool        VerifyCollisionForCP(Vector3 center, Vector3 centerPos, std::vector<Vector3> & controlPoints,
                                   Vector3 cpPos, std::vector<int> & collisions);
  bool        VerifyCollision(std::vector<Vector2>& collisions);
  bool        VerifyEdgeCollision(std::vector<Vector3>& collisions, unsigned int& halflist);

  void        ResetFrame();

  void        MoveBalls();
  void        ScaleBalls();

  void        CalculateNewPositions();

  bool        GetBallsCollision();
  bool        GetControlPointsAffectedByCollisions();
  bool        CalculateCollisionForces();

  void        CalculateJointDistance(std::vector<Joint_t>& joints, float radius);
  void        UpdateNeighbourJoints(std::vector<Joint_t>& joints, std::vector<Vector3>& controlPoints,
                                    std::vector<Vector3>& forces);
  void        UpdateSprings();
  bool        CheckJointSize(int bone, float radius, std::vector<Vector3>& controlPoints, std::vector<Joint_t>& joints);

  void        ApplyInteria(int bone, std::vector<Vector3>& newControlPoints, std::vector<Vector3>& controlPoints,
                           std::vector<Vector3>& lastFrameSpeeds, std::vector<Vector3>& springForces);

  bool        SetNewPositionsToControlPoints();

  void        DampenForces();

  void        CalculateBounding(std::vector<Vector3>& controlPoints, Vector3& pntMax, Vector3& pntMin);
  void        RepositionCenterOfCircle(std::vector<Vector3> &controlPoints);

  void        CreateRipple(std::vector<Vector3>& controlPoints, std::vector<Vector3>& springForces);

  void        SetShaderInfo();

  void        PushControlPoints(Vector3 movementVector, int ball);
  bool        MoveBallTo(Vector3 movementVector, int ball);

  //UI
  int         DetectHit(const TouchPoint& point);
};


PhysicsBallsController::PhysicsBallsController( Application& application )
: mApplication( application )
{
  // Connect to the Application's Init signal
  mApplication.InitSignal().Connect( this, &PhysicsBallsController::Create );
}

PhysicsBallsController::~PhysicsBallsController()
{
  // Nothing to do here;
}

// The Init signal is received once (only) during the Application lifetime
void PhysicsBallsController::Create( Application& application )
{
  DemoHelper::RequestThemeChange();

  Stage stage = Stage::GetCurrent();
  mScreenSize = stage.GetSize();

  stage.SetBackgroundColor(Color::BLACK);

  //Create a view
  mView = Dali::Toolkit::View::New();
  mView.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  stage.Add( mView );

  //Set background image for the view
  mBackImage = ResourceImage::New( BACKGROUND_IMAGE );

  mReflectionImage = ResourceImage::New( REFLECTION_IMAGE );

  mCompositionActor = ImageActor::New( mBackImage );
  mCompositionActor.SetParentOrigin(ParentOrigin::CENTER);
  mCompositionActor.SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  mCompositionActor.SetSize(mScreenSize.x, mScreenSize.y);
  //mCompositionActor.SetShaderEffect(mNormalShader);

  mView.Add( mCompositionActor );

  //Active circle
  //-------------
  mActiveRadius = mScreenSize.x * 0.5f * 0.2f;
  mActiveMass = 1.0f;
  mActive = MeshActor::New( CreateCircleWithBones(VTX_NUMBER, BONE_NUMBER, mActiveRadius, Vector4(0.3,0.9,0.6,1), mActiveControlPoints) );
  mActive.SetScale( 1.0f );
  mActive.SetPosition(Vector3(0, 0.0f, 0.2f));
  mActive.SetParentOrigin( ParentOrigin::CENTER );

  mShaderActive = ShaderEffect::New( VERTEX_SHADER, FRAG_SHADER, GEOMETRY_TYPE_TEXTURED_MESH  );
  mShaderActive.SetUniform( "boneNumber", BONE_NUMBER );
  mShaderActive.SetUniform( "radius", mActiveRadius );
  mShaderActive.SetUniform( "screenSize", mScreenSize );
  mShaderActive.SetUniform( "vtxPerBone", VTX_NUMBER / BONE_NUMBER);
  mShaderActive.SetEffectImage( mReflectionImage );
  //Set control points to the shader and init other arrays

  for (unsigned int uiCP = 0 ; uiCP < mActiveControlPoints.size() ; uiCP++)
  {
    mNewActiveControlPoints.push_back(Vector3(0,0,0));
    mActiveNeedsUpdate.push_back(0);
    mActiveCPForces.push_back(Vector3(0,0,0));
    mActiveSpringForces.push_back(Vector3(0,0,0));
    mActiveLastFrameSpeeds.push_back(Vector3(0,0,0));

    std::ostringstream oss;
    oss<< "controlPoints["<< uiCP << "]";
    Vector3 position = mActiveControlPoints[uiCP];
    mShaderActive.SetUniform( oss.str(), position );
  }

  mActive.SetShaderEffect(mShaderActive);

  CreateJoints(mActiveJoints,BONE_NUMBER);

  mView.Add( mActive );

  //Still circle
  //------------
  mStillRadius = mScreenSize.x * 0.5f * 0.2f;
  mStillMass = 0.6f;
  mStill = MeshActor::New( CreateCircleWithBones(VTX_NUMBER, BONE_NUMBER, mStillRadius, Vector4(0.95,0.45,0.45,1), mStillControlPoints) );
  mStill.SetScale( 1.0f );
  mStill.SetPosition(Vector3(0, 0.0f, 0.2f));
  mStill.SetParentOrigin( ParentOrigin::CENTER );

  mShaderStill = ShaderEffect::New( VERTEX_SHADER, FRAG_SHADER, GEOMETRY_TYPE_TEXTURED_MESH  );
  mShaderStill.SetUniform( "boneNumber", BONE_NUMBER );
  mShaderStill.SetUniform( "radius", mStillRadius );
  mShaderStill.SetUniform( "screenSize", mScreenSize );
  mShaderStill.SetUniform( "vtxPerBone", VTX_NUMBER / BONE_NUMBER);
  mShaderStill.SetEffectImage( mReflectionImage );
  //Set control points to the shader and init other arrays
  for (unsigned int uiCP = 0 ; uiCP < mStillControlPoints.size() ; uiCP++)
  {
    mNewStillControlPoints.push_back(Vector3(0,0,0));
    mStillNeedsUpdate.push_back(0);
    mStillCPForces.push_back(Vector3(0,0,0));
    mStillSpringForces.push_back(Vector3(0,0,0));
    mStillLastFrameSpeeds.push_back(Vector3(0,0,0));

    std::ostringstream oss;
    oss<< "controlPoints["<< uiCP << "]";
    Vector3 position = mStillControlPoints[uiCP];
    mShaderStill.SetUniform( oss.str(), position );
  }

  mStill.SetShaderEffect(mShaderStill);

  CreateJoints(mStillJoints,BONE_NUMBER);

  mView.Add( mStill );

  //Watch
  mWatch = MeshActor::New( CreateCircle(VTX_NUMBER, mScreenSize.x * 0.5f, Vector4(0,0,1,1)) );
  mWatch.SetScale( 1.0f );
  mWatch.SetPosition(Vector3(0, 0.0f, 0.1f));
  mWatch.SetParentOrigin( ParentOrigin::CENTER );

  //mView.Add( mWatch );


  //Loaded circle
  //mData.LoadDataFile(MESH_FILES[0]);
  //mData.CreateActor();
  //mView.Add( mData.GetActor() );

  // Respond to a click anywhere on the stage
  stage.GetRootLayer().TouchedSignal().Connect( this, &PhysicsBallsController::OnTouch );


  //Inicializations
  mDirectionActive = Vector3(0,0,0);
  mDirectionStill = Vector3(0,0,0);

  mActiveScale = (MAX_ACTIVE_RADIUS * mScreenSize.x - mActiveRadius) * 0.375f;
  mStillScale = (MAX_STILL_RADIUS * mScreenSize.x - mStillRadius) * 0.275f;

  mTimer = Timer::New( 16 );
  mTimer.TickSignal().Connect(this, &PhysicsBallsController::OnTimerTick);

  float var = (MAX_ACTIVE_RADIUS + MAX_STILL_RADIUS) * 0.25;

  Vector3 iniPos;
  iniPos = Vector3(mScreenSize.x * (var + 0.05), 0.0f, 0.2f);
  for (unsigned int uiCP = 0 ; uiCP < BONE_NUMBER + 1 ; uiCP++)
  {
    mActiveControlPoints[uiCP] += iniPos;
  }
  iniPos = Vector3(-mScreenSize.x * (var + 0.12), 0.0f, 0.2f);
  for (unsigned int uiCP = 0 ; uiCP < BONE_NUMBER + 1 ; uiCP++)
  {
    mStillControlPoints[uiCP] += iniPos;
  }
//  mActivePos = mIniActivePos = Vector3(mScreenSize.x * 0.2, 0.0f, 0.2f);
//  mStillPos = mIniStillPos = Vector3(-mScreenSize.x * 0.2, 0.0f, 0.2f);

  CalculateJointDistance(mActiveJoints, mActiveRadius);
  CalculateJointDistance(mStillJoints, mStillRadius);

  mShaderActive.SetUniform( "center", mActivePos );
  mShaderStill.SetUniform( "center", mStillPos );

  mTime = 0.f;

  mPause = false;
  mTimer.Start();
}


Mesh PhysicsBallsController::CreateCircle(int numberOfVertices, float radius, Vector4 color)
{
  float aspect = (float)mScreenSize.y / (float)mScreenSize.x;

  float angle = 0;
  float deltaPi = 2. * Math::PI / (float)numberOfVertices;

  float X,Y,u,v;

  MeshData::VertexContainer vertices(numberOfVertices + 1);

  for (int i = 0 ; i < numberOfVertices ; i++)
  {
    X = radius * sin(angle);
    Y = radius * cos(angle);

    u = 0;
    v = 0;

    vertices[i] = MeshData::Vertex( Vector3( X, Y * aspect, 0.f ), Vector2(u, v), Vector3(0,0,1) );
    angle += deltaPi;
  }
  vertices[numberOfVertices] = MeshData::Vertex( Vector3( 0, 0, 0.f ), Vector2(u, v), Vector3(0,0,1) );

  // Specify all the faces radius()
  int vtx, vtxnext;
  MeshData::FaceIndices faces;
  faces.reserve( numberOfVertices * 3 ); // 2 triangles in Quad
  for (int i = 0 ; i < numberOfVertices ; i++)
  {
    vtx = i;
    vtxnext = (i + 1) % numberOfVertices;
    faces.push_back( numberOfVertices ); faces.push_back( vtx ); faces.push_back( vtxnext );
  }

  Material baseMaterial = Material::New( "Material1" );
  baseMaterial.SetDiffuseColor( color );
  baseMaterial.SetDiffuseTexture( mBackImage );

  // Create the mesh data from the vertices and faces
  MeshData meshData;
  meshData.SetMaterial( baseMaterial );
  meshData.SetVertices( vertices );
  meshData.SetFaceIndices( faces );
  meshData.SetHasTextureCoords( true );
  meshData.SetHasNormals( true );

  // Create a mesh from the data
  Dali::Mesh mesh = Mesh::New( meshData );
  return mesh;

}

Mesh PhysicsBallsController::CreateCircleWithBones(int numberOfVertices, int numberOfBones, float radius, Vector4 color,
                                          std::vector<Vector3>& controlPoints)
{
  //We create the meshdata for the metaballs
  float aspect = (float)mScreenSize.y / (float)mScreenSize.x;

  float X,Y;
  float u,v;
  // Create vertices and specify their color
  float angleBone = 0.f;
  float deltaPiBone = 2. * Math::PI / (float)numberOfBones;

  int numberOfVtxBone = numberOfVertices / numberOfBones;

  MeshData::VertexContainer vertices(numberOfVertices + 1);
  //vertices of the circle

  int vtxNum = 0;
  Vector3 bone,boneNext,bonePrev,pnt;
  for (int i = 0 ; i < numberOfBones ; i++)
  {
    for (int j = 0 ; j < numberOfVtxBone ; j++)
    {
       //We encode bone 0
      u = i;
      //We encode bone +1
      v = (i + 1) % numberOfBones;

      vertices[vtxNum] = MeshData::Vertex( Vector3( 0, 0, 0 ), Vector2(u, v), Vector3(j,1.0f,0) );

      vtxNum++;
    }

    X = radius * sin(angleBone);
    Y = radius * cos(angleBone);
    angleBone += deltaPiBone;

    controlPoints.push_back(Vector3( X, Y * aspect, 0.f ));
  }
  //center of the circle
  //It needs just one influence
  vertices[numberOfVertices] = MeshData::Vertex( Vector3( 0.0f, 0.0f, 0.0f ), Vector2(numberOfBones, 0), Vector3(0, 0.f, 0.f) );
  controlPoints.push_back(Vector3( 0, 0, 0.f ));

  // Specify all the faces radius()
  int vtx, vtxnext;
  MeshData::FaceIndices faces;
  faces.reserve( numberOfVertices * 3 ); // 2 triangles in Quad
  for (int i = 0 ; i < numberOfVertices ; i++)
  {
	  vtx = i;
    vtxnext = (i + 1) % numberOfVertices;
    faces.push_back( numberOfVertices ); faces.push_back( vtx ); faces.push_back( vtxnext );
  }

  Material baseMaterial = Material::New( "Material1" );
  baseMaterial.SetDiffuseColor( color );
  baseMaterial.SetDiffuseTexture( mBackImage );

  // Create the mesh data from the vertices and faces
  MeshData meshData;
  meshData.SetMaterial( baseMaterial );
  meshData.SetVertices( vertices );
  meshData.SetFaceIndices( faces );
  meshData.SetHasTextureCoords( true );
  meshData.SetHasNormals( true );

  // Create a mesh from the data
  Dali::Mesh mesh = Mesh::New( meshData );
  return mesh;
}

void PhysicsBallsController::CreateJoints(std::vector<Joint_t>& joints, int boneNumber)
{
  for (int i = 0 ; i < boneNumber ; i++)
  {
    Joint_t joint;
    joint.idx1 = i;
    joint.idx2 = (i + 1) % BONE_NUMBER;
    joint.distMax = 1.2;
    joint.distMin = 0.8;
    joints.push_back(joint);
  }
}













//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// Prototyping

bool IntersectCircle(Vector3 lineStart, Vector3 lineEnd, Vector3 circle, double radius, Vector3 & circleWhenHit)
{
  //circleWhenHit = null;

  // find the closest point on the line segment to the center of the circle
  Vector3 line = lineEnd - lineStart;
  float lineLength = line.Length();
  Vector3 lineNorm = line * (1/lineLength);
  Vector3 segmentToCircle = circle - lineStart;
  float closestPointOnSegment = segmentToCircle.Dot(line) / lineLength;

  // Special cases where the closest point happens to be the end points
  Vector3 closest;
  if (closestPointOnSegment < 0)
    closest = lineStart;
  else if (closestPointOnSegment > lineLength)
    closest = lineEnd;
  else
    closest = lineStart + lineNorm * closestPointOnSegment;

  // Find that distance.  If it is less than the radius, then we
  // are within the circle
  Vector3 distanceFromClosest = circle - closest;
  float distanceFromClosestLength = distanceFromClosest.Length();
  if (distanceFromClosestLength > radius)
    return false;

  // So find the distance that places the intersection point right at
  // the radius.  This is the center of the circle at the time of collision
  // and is different than the result from Doswa
  Vector3 offset = (distanceFromClosest * (1/distanceFromClosestLength)) *
                   (radius - distanceFromClosestLength);
  circleWhenHit = circle - offset;

  return true;
}

bool PhysicsBallsController::VerifyCollisionForCP(Vector3 center, Vector3 centerPos, std::vector<Vector3> & controlPoints,
                                                  Vector3 cpPos, std::vector<int> & collisions)
{
  bool ret = false;
  Vector3 cp1 = center + centerPos;
  for (int i = 0 ; i < BONE_NUMBER ; i++)
  {
    Vector3 cp2 = controlPoints[i] + cpPos;
    Vector3 Distance = cp1 - cp2;
    if (Distance.Length() < CP_DISTANCE_COLLISION)
    {
      collisions.push_back(i);
      ret = true;
    }
  }
  return ret;
}

bool PhysicsBallsController::VerifyCollision(std::vector<Vector2> & collisions)
{
  bool ret = false;

  for (int i = 0 ; i < BONE_NUMBER ; i++)
  {
    Vector3 Active = mNewActiveControlPoints[i];
    for (int j = 0 ; j < BONE_NUMBER ; j++)
    {
      Vector3 Still = mNewStillControlPoints[j];
      Vector3 Distance = Active - Still;

      //Inside each other and not getting away
      if (Distance.Length() < CP_DISTANCE_COLLISION)
      {
        collisions.push_back(Vector2(i,j));
        ret = true;
      }
    }
  }
  return ret;
}

bool PhysicsBallsController::VerifyEdgeCollision(std::vector<Vector3> & collisions, unsigned int & halflist)
{
  bool ret = false;

  halflist = 0;

  //VtxActive vs Still segments
  for (int i = 0 ; i < BONE_NUMBER ; i++)
  {
    Vector3 center = mNewActiveControlPoints[i];
    for (int j = 0 ; j < BONE_NUMBER ; j++)
    {
      Vector3 start = mNewStillControlPoints[j];
      Vector3 end = mNewStillControlPoints[(j+1)%BONE_NUMBER];

      Vector3 result;
      if (IntersectCircle(start, end, center, CP_DISTANCE_COLLISION, result))
      {
        collisions.push_back(Vector3(i,j,(j+1)%BONE_NUMBER));
        halflist++;
        ret = true;
      }
    }
  }

  //VtxStill vs Active segments
  for (int i = 0 ; i < BONE_NUMBER ; i++)
  {
    Vector3 center = mNewStillControlPoints[i];
    for (int j = 0 ; j < BONE_NUMBER ; j++)
    {
      Vector3 start = mNewActiveControlPoints[j];
      Vector3 end = mNewActiveControlPoints[(j+1)%BONE_NUMBER];

      Vector3 result;
      if (IntersectCircle(start, end, center, CP_DISTANCE_COLLISION, result))
      {
        collisions.push_back(Vector3(i,j,(j+1)%BONE_NUMBER));
        ret = true;
      }
    }
  }

  return ret;
}

void PhysicsBallsController::ResetFrame()
{
  for (unsigned int ui = 0 ; ui < mActiveNeedsUpdate.size() ; ui++)
  {
    mActiveNeedsUpdate[ui] = 0;
    mActiveCPForces[ui] = Vector3(0,0,0);
   }

  for (unsigned int ui = 0 ; ui < mStillNeedsUpdate.size() ; ui++)
  {
    mStillNeedsUpdate[ui] = 0;
    mStillCPForces[ui] = Vector3(0,0,0);
  }
}

void PhysicsBallsController::MoveBalls()
{
  if (BALLS_MOVING)
  {
    float maxVelocity = 10;
    //Apply motion to balls
    if (mDirectionActive.Length() > maxVelocity)
    {

    }
    mActivePos = mDirectionActive * 1/16.f;

    mStillPos = mDirectionStill * 1/16.f;
  }
}

void PhysicsBallsController::DampenForces()
{
  for (unsigned int ui = 0 ; ui < mActiveSpringForces.size() ; ui++)
  {
    mActiveSpringForces[ui] *= 0.6;
    mActiveCPForces[ui] *= 0.6;
  }

  for (unsigned int ui = 0 ; ui < mStillSpringForces.size() ; ui++)
  {
    mStillSpringForces[ui] *= 0.6;
    mStillCPForces[ui] *= 0.6f;
  }

  mDirectionActive*= 0.9;
  mDirectionStill*= 0.9;

  mActivePos = Vector3(0,0,0);
  mStillPos = Vector3(0,0,0);
}

void PhysicsBallsController::ScaleBalls()
{
  if (mActiveRadius < mScreenSize.x * MAX_ACTIVE_RADIUS)
  {
    mNewActiveRadius = mActiveRadius + (mActiveScale * 1/16.f);
    for (unsigned int ui = 0 ; ui < mActiveCPForces.size() ; ui++)
    {
      Vector3 direction = mActiveControlPoints[ui] - mActiveControlPoints[BONE_NUMBER];
      direction.Normalize();
      Vector3 forceVec = direction * mActiveScale;
      mActiveCPForces[ui] = forceVec;
    }

    //mActiveScale += mActiveScale * 1/16.f;
  }
  else
  {
    mNewActiveRadius = mActiveRadius;
  }

  if (mStillRadius < mScreenSize.x * MAX_STILL_RADIUS)
  {
    mNewStillRadius = mStillRadius + (mStillScale * 1/16.f);
    for (unsigned int ui = 0 ; ui < mStillCPForces.size() ; ui++)
    {
      Vector3 direction = mStillControlPoints[ui] - mStillControlPoints[BONE_NUMBER];
      direction.Normalize();
      Vector3 forceVec = direction * mStillScale;
      mStillCPForces[ui] = forceVec;
    }

    //mStillScale += mStillScale * 1/16.f;
  }
  else
  {
    mNewStillRadius = mStillRadius;
  }
}

void PhysicsBallsController::CalculateJointDistance(std::vector<Joint_t>& joints, float radius)
{
  float angle = 0;
  float deltaPi = 2.0 * Math::PI / BONE_NUMBER;
  float aspect = (float)mScreenSize.y / (float)mScreenSize.x;

  for (unsigned int i = 0 ; i < joints.size() ; i++)
  {
    float X = radius * sin(angle);
    float Y = radius * cos(angle);

    float nextX = radius * sin(angle + deltaPi);
    float nextY = radius * cos(angle + deltaPi);

    angle += deltaPi;

    Vector3 cp = Vector3( X, Y * aspect, 0);
    Vector3 cpNext = Vector3( nextX, nextY * aspect, 0);

    float distance = (cp - cpNext).Length();
    joints[i].distance = distance;
  }
}

void PhysicsBallsController::UpdateNeighbourJoints(std::vector<Joint_t>& joints, std::vector<Vector3>& controlPoints,
                                                   std::vector<Vector3>& forces)
{
  for (unsigned int ui = 0 ; ui < joints.size() ; ui++)
  {
    //if new length (bone-center) of spring is less than min or max than max length stop movement
    //Calculate distance to the center

    //if length from i to i+1 bone of spring is less than min or max than max length stop movement
    //Calculate distance from the other joint

    float distance = joints[ui].distance;
    Vector3 vecJoint = controlPoints[joints[ui].idx1] - controlPoints[joints[ui].idx2];
    float distanceReal = vecJoint.Length();

    float difDistance = distance - distanceReal;
    if (fabs(difDistance) > 1.f)
    {
      float percent = distanceReal / distance;
      //Vector3 force = vecJoint * fabs(percent * 0.15);

      if (percent > 1.02f)
      {
        float springStrength = distanceReal - (joints[ui].distMin * distance);

        vecJoint.Normalize();
        forces[joints[ui].idx1] += -vecJoint * 0.5f * springStrength;
        forces[joints[ui].idx2] += vecJoint * 0.5f * springStrength;
        //printf("spring max %f %f %f\n", force.x,force.y,force.z);
      }
      else if (percent < 0.98f)
      {
        float springStrength = (joints[ui].distMax * distance) - distanceReal;

        vecJoint.Normalize();
        forces[joints[ui].idx1] += vecJoint * 0.5f * springStrength;
        forces[joints[ui].idx2] += -vecJoint * 0.5f * springStrength;
        //printf("spring min %f %f %f\n", force.x,force.y,force.z);
      }
    }
  }
}

void PhysicsBallsController::UpdateSprings()
{
  //Joint from CP to center of the ball
  float distance;
  Vector3 direction;
  for (int i = 0 ; i < BONE_NUMBER ; i++)
  {
    //Memory force - If possible they try to get back to initial possition
    direction = mActiveControlPoints[i] - mActiveControlPoints[BONE_NUMBER];
    distance = direction.Length();
    if (fabs(distance - mActiveRadius) > 1)
    {
      //Compressed
      if (distance < mActiveRadius*0.98f)
      {
        float springStrength = distance - (MIN_JOINT_DISTANCE*mActiveRadius);

        direction.Normalize();
        mActiveSpringForces[i] += direction * 0.4 * springStrength;
        //mActiveSpringForces[i] += direction * 0.1;
      }
      //Stretched
      else if (distance > mActiveRadius*1.02f)
      {
        float springStrength = (MAX_JOINT_DISTANCE*mActiveRadius) - distance;

        direction.Normalize();
        mActiveSpringForces[i] += -direction * 0.4 * springStrength;
      }
    }

    //Memory force - If possible they try to get back to initial possition
    direction = mStillControlPoints[i] - mStillControlPoints[BONE_NUMBER];
    distance = direction.Length();
    if (fabs(distance - mStillRadius) > 1)
    {
      if (distance < mStillRadius*0.98f)
      {
        float springStrength = distance - (MIN_JOINT_DISTANCE*mStillRadius);

        direction.Normalize();
        mStillSpringForces[i] += direction * 0.4 * springStrength;
      }
      else if (distance > mStillRadius*1.02f)
      {
        float springStrength = (MAX_JOINT_DISTANCE*mStillRadius) - distance;

        direction.Normalize();
        mStillSpringForces[i] += -direction * 0.4 * springStrength;
      }
    }
  }

  UpdateNeighbourJoints(mActiveJoints, mActiveControlPoints, mActiveSpringForces);
  UpdateNeighbourJoints(mStillJoints, mStillControlPoints, mStillSpringForces);
}

bool PhysicsBallsController::CheckJointSize(int bone, float radius, std::vector<Vector3>& controlPoints, std::vector<Joint_t>& joints)
{
  //We verify this control point with the center
  Vector3 jointVector = controlPoints[bone] - controlPoints[BONE_NUMBER];
  float jointSize = jointVector.Length();

  if (jointSize < radius * MIN_JOINT_DISTANCE)
  {
    return false;
  }
  else if (jointSize > radius * MAX_JOINT_DISTANCE)
  {
    return false;
  }

  //Now we verify this control point with the next cp
  jointVector = controlPoints[bone] - controlPoints[(bone+1)%BONE_NUMBER];
  jointSize = jointVector.Length();

  if (jointSize < joints[bone].distance * MIN_JOINT_DISTANCE)
  {
    return false;
  }
  else if (jointSize > joints[bone].distance * MAX_JOINT_DISTANCE)
  {
    return false;
  }

  return true;
}

void PhysicsBallsController::CalculateNewPositions()
{
  float maxForce = 5.f;
  Vector3 newPosition, newForce, newSpringForce;
  for (unsigned int ui = 0 ; ui < BONE_NUMBER ; ui++)
  {
    newForce = mActiveCPForces[ui] * 1/16.f;
    newSpringForce = mActiveSpringForces[ui] * 1/16.f;

    //cap forces too big
    if (newForce.Length() > maxForce)
    {
      newForce.Normalize();
      newForce *= maxForce;
    }
    if (newSpringForce.Length() > maxForce)
    {
      newSpringForce.Normalize();
      newSpringForce *= maxForce;
    }

    newPosition = mActiveControlPoints[ui] + newForce + newSpringForce;
    mNewActiveControlPoints[ui] = newPosition + mActivePos;
  }
  mNewActiveControlPoints[BONE_NUMBER] = mActiveControlPoints[BONE_NUMBER] + mActivePos;

  for (unsigned int ui = 0 ; ui < BONE_NUMBER ; ui++)
  {
    newForce = mStillCPForces[ui] * 1/16.f;
    newSpringForce = mStillSpringForces[ui] * 1/16.f;

    //cap forces too big

    if (newForce.Length() > maxForce)
    {
      newForce.Normalize();
      newForce *= maxForce;
    }
    if (newSpringForce.Length() > maxForce)
    {
      newSpringForce.Normalize();
      newSpringForce *= maxForce;
    }


    newPosition = mStillControlPoints[ui] + newForce + newSpringForce;
    mNewStillControlPoints[ui] = newPosition + mStillPos;
  }
  mNewStillControlPoints[BONE_NUMBER] = mStillControlPoints[BONE_NUMBER] + mStillPos;
}

bool PhysicsBallsController::GetBallsCollision()
{
  //Is there collision between the balls?
  Vector3 Distance = mActiveControlPoints[BONE_NUMBER] - mStillControlPoints[BONE_NUMBER];
  if (Distance.Length() < (mNewStillRadius + mNewActiveRadius + (0.05 * mNewStillRadius)))
  {
    return true;
  }

  return false;
}

bool PhysicsBallsController::GetControlPointsAffectedByCollisions()
{
  bool ret = false;

  std::vector<Vector2> collisions;
  if (VerifyCollision(collisions))
  {
    for (unsigned int ui = 0 ; ui < collisions.size() ; ui++)
    {
      mNewActiveControlPoints[collisions[ui].x] = mActiveControlPoints[collisions[ui].x];
      mNewStillControlPoints[collisions[ui].y] = mStillControlPoints[collisions[ui].y];

      mActiveCPForces[collisions[ui].x] = Vector3(0,0,0);
      mStillCPForces[collisions[ui].y] = Vector3(0,0,0);

      mActiveSpringForces[collisions[ui].x] = Vector3(0,0,0);
      mStillSpringForces[collisions[ui].y] = Vector3(0,0,0);

      mActiveNeedsUpdate[collisions[ui].x] = 1.0;
      mStillNeedsUpdate[collisions[ui].y] = 1.0;
    }
    collisions.clear();
    ret = true;
  }

  //Verify individual collisions between CPs and nearest segment (CP0 - CP1)
  unsigned int halflist = 0;
  std::vector<Vector3> collisionSegments;
  if (VerifyEdgeCollision(collisionSegments, halflist))
  {
    //NO EDGE movement, it only works with 48+ bones
    for (unsigned int ui = 0 ; ui < halflist ; ui++)
    {
      mNewActiveControlPoints[collisionSegments[ui].x] = mActiveControlPoints[collisionSegments[ui].x];
      mNewStillControlPoints[collisionSegments[ui].y] = mStillControlPoints[collisionSegments[ui].y];
      mNewStillControlPoints[collisionSegments[ui].z] = mStillControlPoints[collisionSegments[ui].z];

      mActiveCPForces[collisionSegments[ui].x] = Vector3(0,0,0);
      mStillCPForces[collisionSegments[ui].y] = Vector3(0,0,0);
      mStillCPForces[collisionSegments[ui].z] = Vector3(0,0,0);

      mActiveSpringForces[collisionSegments[ui].x] = Vector3(0,0,0);
      mStillSpringForces[collisionSegments[ui].y] = Vector3(0,0,0);
      mStillSpringForces[collisionSegments[ui].z] = Vector3(0,0,0);

      mActiveNeedsUpdate[collisionSegments[ui].x] = 1.0;
      mStillNeedsUpdate[collisionSegments[ui].y] = 1.0;
      mStillNeedsUpdate[collisionSegments[ui].z] = 1.0;
    }
    for (unsigned int ui = halflist ; ui < collisionSegments.size() ; ui++)
    {
      mNewStillControlPoints[collisionSegments[ui].x] = mStillControlPoints[collisionSegments[ui].x];
      mNewActiveControlPoints[collisionSegments[ui].y] = mActiveControlPoints[collisionSegments[ui].y];
      mNewActiveControlPoints[collisionSegments[ui].z] = mActiveControlPoints[collisionSegments[ui].z];

      mStillCPForces[collisionSegments[ui].x] = Vector3(0,0,0);
      mActiveCPForces[collisionSegments[ui].y] = Vector3(0,0,0);
      mActiveCPForces[collisionSegments[ui].z] = Vector3(0,0,0);

      mStillSpringForces[collisionSegments[ui].x] = Vector3(0,0,0);
      mActiveSpringForces[collisionSegments[ui].y] = Vector3(0,0,0);
      mActiveSpringForces[collisionSegments[ui].z] = Vector3(0,0,0);

      mStillNeedsUpdate[collisionSegments[ui].x] = 1.0;
      mActiveNeedsUpdate[collisionSegments[ui].y] = 1.0;
      mActiveNeedsUpdate[collisionSegments[ui].z] = 1.0;

    }
    collisionSegments.clear();
    ret = true;
  }

  return ret;
}

bool PhysicsBallsController::CalculateCollisionForces()
{
  if (GetBallsCollision())
  {
    //Calculate forces derived the collisions between the balls
    if (GetControlPointsAffectedByCollisions())
    {
      float modifStrength = 1.8f;

      //Small ball always get pushed by big ball if there is a collision
      Vector3 direction = mStillControlPoints[BONE_NUMBER] - mActiveControlPoints[BONE_NUMBER];
      float distance = direction.Length();

      float forceStrength = (mActiveRadius + mStillRadius) - distance;

      direction.Normalize();

      mDirectionStill = direction * forceStrength * modifStrength;
      mDirectionActive -= direction * forceStrength * modifStrength * 0.2;

    }
    return false;
  }
  return true;
}

void PhysicsBallsController::ApplyInteria(int bone, std::vector<Vector3>& newControlPoints, std::vector<Vector3>& controlPoints,
                                          std::vector<Vector3>& lastFrameSpeeds, std::vector<Vector3>& springForces)
{
  /*Vector3 inertia;
  //Calculate difference in speed from this frame to the next. Deceleration?
  Vector3 speed = newControlPoints[bone] - controlPoints[bone];
  float speedDiff = (speed - lastFrameSpeeds[bone]).Length();
  if (speedDiff > 1)
  {
    Vector3 direction = (controlPoints[bone] - controlPoints[BONE_NUMBER]);
    //direction.Normalize();
    //Do something if big deceleration
    springForces[bone] += direction * lastFrameSpeeds[bone].Length() * 0.03f;
   }
  //we get speed for this cp for this frame
  lastFrameSpeeds[bone] = newControlPoints[bone] - controlPoints[bone];*/
}

bool PhysicsBallsController::SetNewPositionsToControlPoints()
{
  bool ret = true;
  for (unsigned int ui = 0 ; ui < BONE_NUMBER ; ui++)
  {
    //If new position doesn't break the spring
    if (CheckJointSize(ui, mActiveRadius, mNewActiveControlPoints, mActiveJoints))
    {
      //If new position is inside the watch
      float distance = mNewActiveControlPoints[ui].Length();
      if ((mScreenSize.x * 0.5) > distance)
      {
        ApplyInteria(ui, mNewActiveControlPoints, mActiveControlPoints, mActiveLastFrameSpeeds, mActiveSpringForces);

        //we update cp position
        mActiveControlPoints[ui] = mNewActiveControlPoints[ui];
      }
      else
      {
        //Hitting the screen
        //Try to go to the center
        Vector3 directionToScreenCenter = Vector3(0 - mActiveControlPoints[BONE_NUMBER].x, 0 - mActiveControlPoints[BONE_NUMBER].y, 0);
        directionToScreenCenter.Normalize();
        mDirectionActive = directionToScreenCenter * 5.f;

        mNewActiveControlPoints[ui] = mActiveControlPoints[ui];
        mActiveCPForces[ui] = Vector3(0,0,0);
        mActiveSpringForces[ui] = Vector3(0,0,0);
      }
    }
    else
    {
      mNewActiveControlPoints[ui] = mActiveControlPoints[ui];
      mActiveCPForces[ui] = Vector3(0,0,0);
      mActiveSpringForces[ui] = Vector3(0,0,0);

      ret = false;
    }
  }

  mActiveControlPoints[BONE_NUMBER] = mNewActiveControlPoints[BONE_NUMBER];

  for (unsigned int ui = 0 ; ui < BONE_NUMBER ; ui++)
  {
    //If new position doesn't break the spring
    if (CheckJointSize(ui, mStillRadius, mNewStillControlPoints, mStillJoints))
    {
      //If new position is inside the watch
       float distance = mNewStillControlPoints[ui].Length();
       if ((mScreenSize.x * 0.5) > distance)
       {
         ApplyInteria(ui, mNewStillControlPoints, mStillControlPoints, mStillLastFrameSpeeds, mStillSpringForces);

         //we update cp position
         mStillControlPoints[ui] = mNewStillControlPoints[ui];
       }
       else
       {
         //Hitting the screen
         //Try to go to the center
         Vector3 directionToScreenCenter = Vector3(0 - mStillControlPoints[BONE_NUMBER].x, 0 - mStillControlPoints[BONE_NUMBER].y, 0);
         directionToScreenCenter.Normalize();
         mDirectionStill = directionToScreenCenter * 5.f;

         mNewStillControlPoints[ui] = mStillControlPoints[ui];
         mStillCPForces[ui] = Vector3(0,0,0);
         mStillSpringForces[ui] = Vector3(0,0,0);
       }
    }
    else
    {
      mNewStillControlPoints[ui] = mStillControlPoints[ui];
      mStillCPForces[ui] = Vector3(0,0,0);
      mStillSpringForces[ui] = Vector3(0,0,0);

      ret = false;
    }
  }

  mStillControlPoints[BONE_NUMBER] = mNewStillControlPoints[BONE_NUMBER];

  mActiveRadius = mNewActiveRadius;
  mStillRadius = mNewStillRadius;

  CalculateJointDistance(mActiveJoints, mActiveRadius);
  CalculateJointDistance(mStillJoints, mStillRadius);

  return ret;
}

void PhysicsBallsController::CalculateBounding(std::vector<Vector3>& controlPoints, Vector3& pntMax, Vector3& pntMin)
{
  pntMax = Vector3(-999999,-999999,-999999);
  pntMin = Vector3(999999,999999,999999);

  for (unsigned int ui = 0 ; ui < controlPoints.size() ; ui++)
  {
    if (controlPoints[ui].x > pntMax.x)
      pntMax.x = controlPoints[ui].x;
    if (controlPoints[ui].y > pntMax.y)
      pntMax.y = controlPoints[ui].y;

    if (controlPoints[ui].x < pntMin.x)
      pntMin.x = controlPoints[ui].x;
    if (controlPoints[ui].y < pntMin.y)
      pntMin.y = controlPoints[ui].y;
  }
}

void PhysicsBallsController::RepositionCenterOfCircle(std::vector<Vector3>& controlPoints)
{
  Vector3 pntMax,pntMin,center;
  CalculateBounding(controlPoints,pntMax,pntMin);

  center = pntMin + (pntMax - pntMin) * 0.5;

  Vector3 direction = center - controlPoints[BONE_NUMBER];

  if (direction.Length() > 20)
  {
    direction.z = 0;
    direction.Normalize();

    controlPoints[BONE_NUMBER] += direction;
    //controlPoints[BONE_NUMBER].x = center.x;
    //controlPoints[BONE_NUMBER].y = center.y;
  }
}

void PhysicsBallsController::CreateRipple(std::vector<Vector3>& controlPoints, std::vector<Vector3>& springForces)
{
  for (unsigned int ui = 0 ; ui < BONE_NUMBER ; ui++)
  {
    springForces[ui] = (controlPoints[ui] - controlPoints[BONE_NUMBER]) * 0.3f;
  }
}

void PhysicsBallsController::SetShaderInfo()
{
  //Set new control points positions
  for (unsigned int uiCP = 0 ; uiCP < mActiveControlPoints.size() ; uiCP++)
  {
    std::ostringstream oss;
    oss<< "controlPoints["<< uiCP << "]";
    Vector3 position = mActiveControlPoints[uiCP];
    mShaderActive.SetUniform( oss.str(), position );
  }
  mShaderActive.SetUniform( "center", mActiveControlPoints[BONE_NUMBER] );
  mShaderActive.SetUniform( "radius", mActiveRadius );

  for (unsigned int uiCP = 0 ; uiCP < mStillControlPoints.size() ; uiCP++)
  {
    std::ostringstream oss;
    oss<< "controlPoints["<< uiCP << "]";
    Vector3 position = mStillControlPoints[uiCP];
    mShaderStill.SetUniform( oss.str(), position );
  }
  mShaderStill.SetUniform( "center", mStillControlPoints[BONE_NUMBER] );
  mShaderStill.SetUniform( "radius", mStillRadius );
}

bool PhysicsBallsController::OnTimerTick()
{
  //Reset last frame vals
  ResetFrame();

  MoveBalls();

  ScaleBalls();

  UpdateSprings();

  CalculateNewPositions();

  CalculateCollisionForces();

  SetNewPositionsToControlPoints();

  //RepositionCenterOfCircle(mActiveControlPoints);
  //RepositionCenterOfCircle(mStillControlPoints);

  DampenForces();

  //Set position to Actors
  //mActive.SetPosition(mActivePos);
  //mStill.SetPosition(mStillPos);

  SetShaderInfo();

  mTime += 1/16.f;

  return true;
}

//--------------------------------------------------------------------------------------------



void PhysicsBallsController::PushControlPoints(Vector3 movementVector, int ball)
{
  if (GetBallsCollision())
  {
    //Calculate forces derived the collisions between the balls
    if (GetControlPointsAffectedByCollisions())
    {
      //float modifStrength = 1.0f;
      for (unsigned int ui = 0 ; ui < BONE_NUMBER ; ui++)
      {
        Vector3 direction = movementVector;
        //direction.Normalize();
        if (mActiveNeedsUpdate[ui] > 0)
        {
          //Vector3 modificationForSize = -direction * modifStrength * mActiveNeedsUpdate[ui];
          Vector3 forceVec = direction * mActiveNeedsUpdate[ui] * 0.9f;
          mActiveCPForces[ui] -= forceVec * ball;
        }
      }

      for (unsigned int ui = 0 ; ui < BONE_NUMBER ; ui++)
      {
        Vector3 direction = movementVector;
        //direction.Normalize();
        if (mStillNeedsUpdate[ui] > 0)
        {
          //Vector3 modificationForSize = -direction * modifStrength * mStillNeedsUpdate[ui];
          Vector3 forceVec = direction * mStillNeedsUpdate[ui] * 1.1f;
          mStillCPForces[ui] += forceVec * ball;
        }
      }
    }
  }
}


bool PhysicsBallsController::MoveBallTo(Vector3 movementVector, int ball)
{
  std::vector<Vector3>* controlPoints;

  if (ball == 0)
  {
    controlPoints = &mNewActiveControlPoints;
    for (unsigned int uiCP = 0 ; uiCP < BONE_NUMBER ; uiCP++)
    {
      mNewStillControlPoints[uiCP] = mStillControlPoints[uiCP];
    }
  }
  else
  {
    controlPoints = &mNewStillControlPoints;
    for (unsigned int uiCP = 0 ; uiCP < BONE_NUMBER ; uiCP++)
    {
      mNewActiveControlPoints[uiCP] = mActiveControlPoints[uiCP];
    }
  }

  for (unsigned int uiCP = 0 ; uiCP < BONE_NUMBER ; uiCP++)
  {
    (*controlPoints)[uiCP] += movementVector;
  }

  (*controlPoints)[BONE_NUMBER] += movementVector;

  ResetFrame();

  if (ball == 0)
    PushControlPoints(movementVector, 1);
  else
    PushControlPoints(movementVector, -1);

  for (unsigned int uiCP = 0 ; uiCP < BONE_NUMBER ; uiCP++)
  {
    mNewActiveControlPoints[uiCP] += mActiveCPForces[uiCP];
    mNewStillControlPoints[uiCP] += mStillCPForces[uiCP];
  }

  if (!SetNewPositionsToControlPoints())
    return false;

  SetShaderInfo();
  return true;
}



//--------------------------------------------------------------------------------------------

int PhysicsBallsController::DetectHit(const TouchPoint& point)
{
  int ret = 0;
  Vector3 click = Vector3(point.screen.x - (mScreenSize.x * 0.5f), point.screen.y - (mScreenSize.y * 0.5f), 0.2);

  Vector3 distanceVec = mActiveControlPoints[BONE_NUMBER] - click;
  float distance = distanceVec.Length();
  if (mActiveRadius > distance)
    ret = ret | 1;

  distanceVec = mStillControlPoints[BONE_NUMBER] - click;
  distance = distanceVec.Length();
  if (mStillRadius > distance)
    ret = ret | 2;

  return ret;
}

bool PhysicsBallsController::OnTouch( Actor actor, const TouchEvent& touch )
{
  const TouchPoint &point = touch.GetPoint(0);

  //Vector3 click = Vector3(point.screen.x - (mScreenSize.x * 0.5f), point.screen.y - (mScreenSize.y * 0.5f), 0.2);

  switch(point.state)
  {
    case TouchPoint::Down:
    {
      mLastPosition = point.screen;

      mSelectedBall = DetectHit(point);

      mTimer.Stop();
      break;
    }
    case TouchPoint::Motion:
    {
      Vector3 dif = Vector3(point.screen.x - mLastPosition.x, point.screen.y - mLastPosition.y, 0);
      if (mSelectedBall & 1)
      {
        if (!MoveBallTo(dif, 0))
          mSelectedBall = 0;
      }
      else if (mSelectedBall & 2)
      {
        if (!MoveBallTo(dif, 1))
          mSelectedBall = 0;
      }
      mLastPosition = point.screen;
      break;

    }
    case TouchPoint::Up:
    case TouchPoint::Leave:
    case TouchPoint::Interrupted:
    {
      mTimer.Start();

      mSelectedBall = DetectHit(point);
      if (mSelectedBall & 1)
        CreateRipple(mActiveControlPoints, mActiveSpringForces);
      if (mSelectedBall & 2)
        CreateRipple(mStillControlPoints, mStillSpringForces);

      mSelectedBall = 0;
      break;
    }
    default:
      break;
    }
  return true;
}

void RunTest( Application& application )
{
  PhysicsBallsController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}



















/*
Mesh PhysicsBallsController::CreateCircle(int numberOfVertices, float radius, std::vector<Vector3>& controlPoints)
{
  //We create the meshdata for the metaballs
  float aspect = (float)mScreenSize.y / (float)mScreenSize.x;

  float X,Y,u,v;
  // Create vertices and specify their color
  float finalRadius = mScreenSize.x * 0.5 * radius;
  float angle = 0.f, deltaPi = 2. * Math::PIi / (float)numberOfVertices;

  //IdentityMatrix
  float a16fData[16] = {1,0,0,0,
                        0,1,0,0,
                        0,0,1,0,
                        0,0,0,1};
  Bone root("root", Matrix(a16fData));

  //Creation of all the bones
  BoneContainer bones;
  for (int i = 0 ; i < numberOfVertices ; i++)
  {
    X = finalRadius * sin(angle);
    Y = finalRadius * cos(angle);
    angle += deltaPi;

    a16fData[12] = X;
    a16fData[13] = Y * aspect;

    Bone b("bone", Matrix(a16fData));

    bones.push_back(b);
    controlPoints.push_back(Vector3(X,Y,0.f));
  }
  bones.push_back(root);
t
  MeshData::VertexContainer vertices(numberOfVertices + 1);
  //vertices of the circle
  angle = 0;
  for (int i = 0 ; i < numberOfVertices ; i++)
  {
    u = sin(angle) * 0.5 + 0.5;)
    v = cos(angle) * 0.5 + 0.5;
    angle += deltaPi;

    vertices[i] = MeshData::Vertex( Vector3( 0.f, 0.f, 0.f ), Vector2(u, v), Vector3(0.0f, 0.0f, 1.0f) );
    vertices[i].boneIndices[0] = i;
    vertices[i].boneWeights[0] = 1.0;
  }
  //center of the circle
  vertices[numberOfVertices] = MeshData::Vertex( Vector3( 0.0f, 0.0f, 0.0f ), Vector2(0.5f, 0.5f), Vector3(0.0f, 0.0f, 1.0f) );
  vertices[numberOfVertices].boneIndices[0] = numberOfVertices;
  vertices[numberOfVertices].boneWeights[0] = 1.0;

  // Specify all the facesradius
  int vtx, vtxnext;
  MeshData::FaceIndices faces;
  faces.reserve( numberOfVertices * 3 ); // 2 triangles in Quad
  for (int i = 0 ; i < numberOfVertices ; i++)
  {
    vtx = i;
    vtxnext = (i + 1) % numberOfVertices;
    faces.push_back( numberOfVertices ); faces.push_back( vtx ); faces.push_back( vtxnext );
  }

  Material baseMaterial = Material::New( "Material1" );
  baseMaterial.SetDiffuseColor( Vector4(1.0f,1.0f,1.0f,1.0f));
  baseMaterial.SetDiffuseTexture( mBackImage );

  // Create the mesh data from the vertices and faces
  MeshData meshData;
  meshData.SetData(vertices, faces, bones, baseMaterial);
  meshData.SetHasTextureCoords( true );
  //meshData.SetHasNormals( true );

  int iBones = meshData.GetBoneCount();
  iBones++;

  // Create a mesh from the data
  Dali::Mesh mesh = Mesh::New( meshData );
  return mesh;
}
*/