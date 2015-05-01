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
const char * const TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );

const int BONE_NUMBER(24);
const int VTX_NUMBER(480);
const int CP_DISTANCE_COLLISION(3);
const int MAX_BONES_X_VERTEX(3);

const char* MESH_FILES[] =
{
 DALI_MODEL_DIR "BoneBlob.dat"
};

}

//2 - 3 Influences
/*
const char*const VERTEX_SHADER = DALI_COMPOSE_SHADER (
  uniform vec3 controlPoints[96+1];\n
  uniform vec3 center;\n
  uniform vec2 screenSize;
  uniform float boneNumber;\n
  uniform float radius;
  varying vec3 color;\n
  varying vec2 newPointPos;\n
  void main()\n
  {\n
    float thirdbone = mod(aTexCoord.x - 1.0, boneNumber);
    vec2 newPointPos = aPosition.xy + controlPoints[int(aTexCoord.x)].xy * aNormal.x +
                       controlPoints[int(aTexCoord.y)].xy * aNormal.y +
                       controlPoints[int(thirdbone)].xy * aNormal.z;\n

    vTexCoord.x = (newPointPos.x + center.x + (screenSize.x * 0.5)) / screenSize.x;\n
    vTexCoord.y = (newPointPos.y + center.y + (screenSize.y * 0.5)) / screenSize.y;\n

    vec2 dif = newPointPos - center.xy;\n
    float len = length(dif);\n
    if (len > 0.5)\n
      color = vec3(1.0);\n
    else\n
      color = vec3(0);\n

    gl_Position = uMvpMatrix * vec4( newPointPos.xy, 0.0, 1.0 );\n
  }\n
);*/

//4 Influences
const char*const VERTEX_SHADER = DALI_COMPOSE_SHADER (
  uniform vec3 controlPoints[96+1];\n
  uniform vec3 center;\n
  uniform vec2 screenSize;
  uniform float boneNumber;\n
  uniform float radius;
  varying vec3 color;\n
  varying vec2 newPointPos;\n
  void main()\n
  {\n
    float thirdbone = mod(aTexCoord.x - 1.0, boneNumber);
    float fourthbone = mod(aTexCoord.y + 1.0, boneNumber);
    float fourthWeight = 1.0 - (aNormal.x + aNormal.y + aNormal.z);
    newPointPos = aPosition.xy + controlPoints[int(aTexCoord.x)].xy * aNormal.x +
                  controlPoints[int(aTexCoord.y)].xy * aNormal.y +
                  controlPoints[int(thirdbone)].xy * aNormal.z +
                  controlPoints[int(fourthbone)].xy * fourthWeight;\n

    vTexCoord.x = (newPointPos.x + center.x + (screenSize.x * 0.5)) / screenSize.x;\n
    vTexCoord.y = (newPointPos.y + center.y + (screenSize.y * 0.5)) / screenSize.y;\n

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

  const float Eta = 0.66;\n
  const float FresnelPower = 1.0;\n
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
      normal = normalize(normal);
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
    lightDiffuse = max(0.0,lightDiffuse);
    //lightDiffuse = lightDiffuse * 0.5 + 0.5;

    vec3 vertexToEye = normalize(-vertex);
    vertexToEye = normalize(vertexToEye);
    vec3 lightReflect = normalize(reflect(vecToLight, normal));
    float specularFactor = dot(vertexToEye, lightReflect);
    specularFactor = max(0.0,pow(specularFactor, 32.0));

    vec4 tex = texture2D(sTexture, zoomCoords);\n

    float Ratio = F + (1.0 - F) * pow((1.0 - dot(-vertexToEye, normal)), FresnelPower);\n
    //float Ratio = dot(-vertexToEye, normal);\n

    vec3 reflecVect = reflect(vertexToEye, normal);\n

    float M = 2.0 * sqrt(reflecVect.x * reflecVect.x + reflecVect.y * reflecVect.y +
                         (reflecVect.z + 1.0) * (reflecVect.z + 1.0));\n
    float u = reflecVect.x / M + 0.5;\n
    float v = reflecVect.y / M + 0.5;\n
    vec4 reflecTex = texture2D(sEffect, vec2(u,v));\n

    vec3 refracColor = tex.xyz;
    vec3 reflecColor = reflecTex.xyz;

    vec3 diffuseTerm = vec3(lightDiffuse, lightDiffuse, lightDiffuse);\n
    vec3 ambientTerm = mix(reflecColor, refracColor, Ratio);\n

    gl_FragColor.xyz = ambientTerm * 0.7 + diffuseTerm * 0.3 + vec3(specularFactor);\n
    gl_FragColor.a = tex.a;\n


    //gl_FragColor = vec4(refracColor.x, refracColor.y, refracColor.z, 1.0);\n

    //gl_FragColor = vec4(lightDiffuse + 0.1 + specularFactor, lightDiffuse + 0.1 + specularFactor, lightDiffuse + 0.1 + specularFactor, 1.0);\n
    //gl_FragColor = vec4(specularFactor, specularFactor, specularFactor, 1.0);\n

    //gl_FragColor = vec4(normal.x*0.5+0.5, normal.y*0.5+0.5, normal.z*0.5+0.5, 1.0);\n
    //gl_FragColor = vec4(Ratio, Ratio, Ratio, 1.0);\n
	  //gl_FragColor = texture2D(sTexture, vTexCoord);\n
    //gl_FragColor = vec4(uMaterial.mDiffuse.x,uMaterial.mDiffuse.y,uMaterial.mDiffuse.z,1.0);\n
    //gl_FragColor = vec4(tex.r * uMaterial.mDiffuse.x,tex.g*uMaterial.mDiffuse.y,tex.b*uMaterial.mDiffuse.z,1.0);\n
  }\n
);


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

  std::vector<Vector3>  mActiveControlPoints;
  std::vector<Vector3>  mStillControlPoints;

  ShaderEffect      mShaderActive;
  ShaderEffect      mShaderStill;

  BoneMesh          mData;

  void              LoadDataFile(const std::string& objFileName);
  Mesh              CreateCircle(int numberOfVertices, float radius, Vector4 color);
  Mesh              CreateCircleWithBones(int numberOfVertices, int numberOfBones, float radius, Vector4 color,
                                 std::vector<Vector3>& controlPoints);

  //-----------------------------------
  // Prototyping
  bool        mPause;

  Vector3     mIniActivePos;
  Vector3     mIniStillPos;
  Vector3     mActivePos;
  Vector3     mNewActivePos;
  Vector3     mStillPos;
  Vector3     mNewStillPos;

  float       mActiveRadius;
  float       mStillRadius;

  Vector3     mDirectionActive;
  Vector3     mDirectionStill;

  float       mActiveMass;
  float       mStillMass;

  float       mActiveScale;
  float       mStillScale;

  std::vector<float>    mActiveNeedsUpdate;
  std::vector<Vector3>  mActiveCPForces;
  std::vector<float>    mStillNeedsUpdate;
  std::vector<Vector3>  mStillCPForces;

  Timer       mTimer;
  float       mTime;

  bool        VerifyCollision(std::vector<Vector2>& collisions);
  bool        VerifyEdgeCollision(std::vector<Vector3>& collisions, unsigned int& halflist);

  void        ResetFrame();

  void        MoveBalls();
  void        ScaleBalls();

  bool        GetBallsCollision();
  bool        GetControlPointsAffectedByCollisions();
  void        CalculateForces();
  void        UpdateSprings();
  void        SetNewPositionsToControlPoints();

  void        CalculateBounding(std::vector<Vector3>& controlPoints, Vector3& pntMax, Vector3& pntMin);
  void        RepositionCenterOfCircle(std::vector<Vector3> &controlPoints);
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
  mActive = MeshActor::New( CreateCircleWithBones(VTX_NUMBER, BONE_NUMBER, mActiveRadius, Vector4(0,1,0,1), mActiveControlPoints) );
  mActive.SetScale( 1.0f );
  mActive.SetPosition(Vector3(0, 0.0f, 0.2f));
  mActive.SetParentOrigin( ParentOrigin::CENTER );

  mShaderActive = ShaderEffect::New( VERTEX_SHADER, FRAG_SHADER, GEOMETRY_TYPE_TEXTURED_MESH  );
  mShaderActive.SetUniform( "boneNumber", BONE_NUMBER );
  mShaderActive.SetUniform( "radius", mActiveRadius );
  mShaderActive.SetUniform( "screenSize", mScreenSize );
  mShaderActive.SetEffectImage( mReflectionImage );
  //Set control points to the shader and init other arrays
  for (unsigned int uiCP = 0 ; uiCP < mActiveControlPoints.size() ; uiCP++)
  {
    mActiveNeedsUpdate.push_back(0);
    mActiveCPForces.push_back(Vector3(0,0,0));

    std::ostringstream oss;
    oss<< "controlPoints["<< uiCP << "]";
    Vector3 position = mActiveControlPoints[uiCP];
    mShaderActive.SetUniform( oss.str(), position );
  }

  mActive.SetShaderEffect(mShaderActive);

  mView.Add( mActive );

  //Still circle
  //------------
  mStillRadius = mScreenSize.x * 0.5f * 0.2f;
  mStillMass = 0.6f;
  mStill = MeshActor::New( CreateCircleWithBones(VTX_NUMBER, BONE_NUMBER, mStillRadius, Vector4(1,0,0,1), mStillControlPoints) );
  mStill.SetScale( 1.0f );
  mStill.SetPosition(Vector3(0, 0.0f, 0.2f));
  mStill.SetParentOrigin( ParentOrigin::CENTER );

  mShaderStill = ShaderEffect::New( VERTEX_SHADER, FRAG_SHADER, GEOMETRY_TYPE_TEXTURED_MESH  );
  mShaderStill.SetUniform( "boneNumber", BONE_NUMBER );
  mShaderStill.SetUniform( "radius", mStillRadius );
  mShaderStill.SetUniform( "screenSize", mScreenSize );
  mShaderStill.SetEffectImage( mReflectionImage );
  //Set control points to the shader and init other arrays
  for (unsigned int uiCP = 0 ; uiCP < mStillControlPoints.size() ; uiCP++)
  {
    mStillNeedsUpdate.push_back(0);
    mStillCPForces.push_back(Vector3(0,0,0));

    std::ostringstream oss;
    oss<< "controlPoints["<< uiCP << "]";
    Vector3 position = mStillControlPoints[uiCP];
    mShaderStill.SetUniform( oss.str(), position );
  }

  mStill.SetShaderEffect(mShaderStill);

  mView.Add( mStill );

  //Watch
  mWatch = MeshActor::New( CreateCircle(VTX_NUMBER, mScreenSize.x * 0.5f, Vector4(0,0,1,1)) );
  mWatch.SetScale( 1.0f );
  mWatch.SetPosition(Vector3(0, 0.0f, 0.1f));
  mWatch.SetParentOrigin( ParentOrigin::CENTER );

  //mView.Add( mWatch );


  //Loaded circle
  mData.LoadDataFile(MESH_FILES[0]);
  //mData.CreateActor();
  //mView.Add( mData.GetActor() );

  // Respond to a click anywhere on the stage
  stage.GetRootLayer().TouchedSignal().Connect( this, &PhysicsBallsController::OnTouch );


  //Inicializations
  mDirectionActive = Vector3(0,0,0);
  mDirectionStill = Vector3(0,0,0);

  mActiveScale = 2.0f;
  mStillScale = 1.0f;

  mTimer = Timer::New( 16 );
  mTimer.TickSignal().Connect(this, &PhysicsBallsController::OnTimerTick);

  mActivePos = mIniActivePos = Vector3(mScreenSize.x * 0.2, 0.0f, 0.2f);
  mStillPos = mIniStillPos = Vector3(-mScreenSize.x * 0.2, 0.0f, 0.2f);

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

  float X,Y,nextX,nextY;
  float x,y,u,v,nx,ny,nz;
  // Create vertices and specify their color
  float angleBone = 0.f;
  float angleVtx = 0.f;
  float deltaPiBone = 2. * Math::PI / (float)numberOfBones;
  float deltaPiVtx = 2. * Math::PI / (float)numberOfVertices;

  int numberOfVtxBone = numberOfVertices / numberOfBones;

  MeshData::VertexContainer vertices(numberOfVertices + 1);
  //vertices of the circle

  int vtxNum = 0;
  Vector3 bone,boneNext,bonePrev,pnt;
  for (int i = 0 ; i < numberOfBones ; i++)
  {
    //2 influences
    //------------
    /*for (int j = 0 ; j < numberOfVtxBone ; j++)
    {
      X = radius * sin(angleBone);
      Y = radius * cos(angleBone);
      bone = Vector3(X,Y,0);
      nextX = radius * sin(angleBone+deltaPiBone);
      nextY = radius * cos(angleBone+deltaPiBone);
      boneNext = Vector3(nextX,nextY,0);

      //We encode bone 0
      u = i;
      //We encode bone +1
      v = (i + 1) % numberOfBones;

      x = radius * sin(angleVtx);
      y = radius * cos(angleVtx);
      pnt = Vector3(x,y,0);

      float distanceBone = (bone - pnt).Length();
      float distanceBoneNext = (boneNext - pnt).Length();
      float max = distanceBone + distanceBoneNext;

      //Now we encode weights for each vertex in its normals
      nx = 1.0f - distanceBone / max;
      ny = (1.0f - nx);
      nz = 0;

      x = ((x - X) * nx) + ((x - nextX) * ny);
      y = ((y - Y) * nx) + ((y - nextY) * ny);

      vertices[vtxNum] = MeshData::Vertex( Vector3( x, y * aspect, 0.f ), Vector2(u, v), Vector3(nx,ny,nz) );
      //vertices[vtxNum] = MeshData::Vertex( Vector3( 0, 0, 0.f ), Vector2(u, v), Vector3(nx,ny,nz) );
      vtxNum++;
      angleVtx += deltaPiVtx;
    }*/

    //3 influences
    //------------
    /*
    float prevX,prevY;
    for (int j = 0 ; j < numberOfVtxBone ; j++)
    {
      if (j <= numberOfVtxBone * 0.5)
      {
        prevX = radius * sin(angleBone-deltaPiBone);
        prevY = radius * cos(angleBone-deltaPiBone);
        bonePrev = Vector3(prevX,prevY,0);
        X = radius * sin(angleBone);
        Y = radius * cos(angleBone);
        bone = Vector3(X,Y,0);
        nextX = radius * sin(angleBone+deltaPiBone);
        nextY = radius * cos(angleBone+deltaPiBone);
        boneNext = Vector3(nextX,nextY,0);

        //We encode bone 0
        u = i;
        //We encode bone +1
        v = (i + 1) % numberOfBones;
      }
      else
      {
        prevX = radius * sin(angleBone);
        prevY = radius * cos(angleBone);
        bonePrev = Vector3(prevX,prevY,0);
        X = radius * sin(angleBone+deltaPiBone);
        Y = radius * cos(angleBone+deltaPiBone);
        bone = Vector3(X,Y,0);
        nextX = radius * sin(angleBone+deltaPiBone+deltaPiBone);
        nextY = radius * cos(angleBone+deltaPiBone+deltaPiBone);
        boneNext = Vector3(nextX,nextY,0);

        //We encode bone 0
        u = (i + 1) % numberOfBones;
        //We encode bone +1
        v = (i + 2) % numberOfBones;
      }

      x = radius * sin(angleVtx);
      y = radius * cos(angleVtx);
      pnt = Vector3(x,y,0);

      float distanceBone = (bone - pnt).Length();
      float distanceBoneNext = (boneNext - pnt).Length();
      float distanceBonePrev = (bonePrev - pnt).Length();
      float max = distanceBone + distanceBoneNext + distanceBonePrev;

      {
        nx = 1.0 - (distanceBone / max);
        ny = 1.0 - (distanceBoneNext / max);
        nz = 1.0 - (distanceBonePrev / max);
        nx = nx*nx;
        ny = ny*ny;
        nz = nz*nz;
        float sum = nx+ny+nz;
        nx /= sum;
        ny /= sum;
        nz /= sum;
      }

      x = ((x - X) * nx) + ((x - nextX) * ny) + ((x - prevX) * nz);
      y = ((y - Y) * nx) + ((y - nextY) * ny) + ((y - prevY) * nz);

      vertices[vtxNum] = MeshData::Vertex( Vector3( x, y * aspect, 0.f ), Vector2(u, v), Vector3(nx,ny,nz) );
      //vertices[vtxNum] = MeshData::Vertex( Vector3( 0, 0, 0.f ), Vector2(u, v), Vector3(nx,ny,nz) );
      vtxNum++;
      angleVtx += deltaPiVtx;
    }*/

    //4 influences
    //------------
    float prevX,prevY,next2X,next2Y;
    Vector3 boneNext2;
    for (int j = 0 ; j < numberOfVtxBone ; j++)
    {
      prevX = radius * sin(angleBone-deltaPiBone);
      prevY = radius * cos(angleBone-deltaPiBone);
      bonePrev = Vector3(prevX,prevY,0);

      X = radius * sin(angleBone);
      Y = radius * cos(angleBone);
      bone = Vector3(X,Y,0);

      nextX = radius * sin(angleBone+deltaPiBone);
      nextY = radius * cos(angleBone+deltaPiBone);
      boneNext = Vector3(nextX,nextY,0);

      next2X = radius * sin(angleBone+deltaPiBone+deltaPiBone);
      next2Y = radius * cos(angleBone+deltaPiBone+deltaPiBone);
      boneNext2 = Vector3(next2X,next2Y,0);

      //We encode bone 0
      u = i;
      //We encode bone +1
      v = (i + 1) % numberOfBones;

      x = radius * sin(angleVtx);
      y = radius * cos(angleVtx);
      pnt = Vector3(x,y,0);

      float distanceBone = (bone - pnt).Length();
      float distanceBoneNext = (boneNext - pnt).Length();
      float distanceBoneNext2 = (boneNext2 - pnt).Length();
      float distanceBonePrev = (bonePrev - pnt).Length();
      float max = distanceBone + distanceBoneNext + distanceBoneNext2 + distanceBonePrev;

      nx = 1.0 - (distanceBone / max);
      ny = 1.0 - (distanceBoneNext / max);
      nz = 1.0 - (distanceBonePrev / max);
      float nyy = 1.0 - (distanceBoneNext2 / max);
      nx = nx * nx * nx;
      ny = ny * ny * ny;
      nz = nz * nz * nz;
      nyy = nyy * nyy * nyy;
      float sum = nx+ny+nz+nyy;
      nx /= sum;
      ny /= sum;
      nz /= sum;
      nyy /= sum;

      x = ((x - X) * nx) + ((x - nextX) * ny) + ((x - prevX) * nz) + ((x - next2X) * nyy);
      y = ((y - Y) * nx) + ((y - nextY) * ny) + ((y - prevY) * nz) + ((y - next2Y) * nyy);

      vertices[vtxNum] = MeshData::Vertex( Vector3( x, y * aspect, float(j)/numberOfVtxBone ), Vector2(u, v), Vector3(nx,ny,nz) );

      vtxNum++;
      angleVtx += deltaPiVtx;
    }

    X = radius * sin(angleBone);
    Y = radius * cos(angleBone);
    angleBone += deltaPiBone;

    controlPoints.push_back(Vector3( X, Y * aspect, 0.f ));
  }
  //center of the circle
  //It needs just one influence
  vertices[numberOfVertices] = MeshData::Vertex( Vector3( 0.0f, 0.0f, 0.0f ), Vector2(numberOfBones, 0), Vector3(1.0f, 0.f, 0.f) );
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

bool PhysicsBallsController::VerifyCollision(std::vector<Vector2> & collisions)
{
  bool ret = false;
  for (int i = 0 ; i < BONE_NUMBER ; i++)
  {
    for (int j = 0 ; j < BONE_NUMBER ; j++)
    {
      Vector3 Active = mActiveControlPoints[i] + mActivePos;
      Vector3 Still = mStillControlPoints[j] + mStillPos;
      Vector3 Distance = Active - Still;
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
    for (int j = 0 ; j < BONE_NUMBER ; j++)
    {
      Vector3 center = mActiveControlPoints[i] + mNewActivePos;
      Vector3 start = mStillControlPoints[j] + mNewStillPos;
      Vector3 end = mStillControlPoints[(j+1)%BONE_NUMBER] + mNewStillPos;

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
    for (int j = 0 ; j < BONE_NUMBER ; j++)
    {
      Vector3 center = mStillControlPoints[i] + mNewStillPos;
      Vector3 start = mActiveControlPoints[j] + mNewActivePos;
      Vector3 end = mActiveControlPoints[(j+1)%BONE_NUMBER] + mNewActivePos;

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
  mNewActivePos = mActivePos;
  mNewStillPos = mStillPos;
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
  //Apply motion to balls
  mNewActivePos = mDirectionActive * 1/16.f + mActivePos;
  mDirectionActive*= 0.9;

  mNewStillPos = mDirectionStill * 1/16.f + mStillPos;
  mDirectionStill*= 0.9;
}

void PhysicsBallsController::ScaleBalls()
{
  //if (mActiveRadius < mScreenSize.x * 0.275)
  if (mActiveRadius < mScreenSize.x * 0.355)
  {
    mActiveRadius += (mActiveScale * 1/16.f);
    for (unsigned int ui = 0 ; ui < mActiveCPForces.size() ; ui++)
    {
      Vector3 direction = mActiveControlPoints[ui];// - mActiveControlPoints[BONE_NUMBER];
      direction.Normalize();
      Vector3 forceVec = direction *  1/16.f * mActiveScale;
      mActiveCPForces[ui] += forceVec;
    }

    mActiveScale += 0.4 * mActiveScale * 1/16.f;
  }


  if (mStillRadius < mScreenSize.x * 0.1875)
  {
    mStillRadius += (mStillScale * 1/16.f);
    for (unsigned int ui = 0 ; ui < mStillCPForces.size() ; ui++)
    {
      Vector3 direction = mStillControlPoints[ui];// - mStillControlPoints[BONE_NUMBER];
      direction.Normalize();
      Vector3 forceVec = direction *  1/16.f * mStillScale;
      mStillCPForces[ui] += forceVec;
    }

    mStillScale += 0.4 * mStillScale * 1/16.f;
  }
}

bool PhysicsBallsController::GetBallsCollision()
{
  //Is there collision between the balls?
  Vector3 Distance = mNewActivePos - mNewStillPos;
  if (Distance.Length() < (mStillRadius + mActiveRadius + (0.05 * mStillRadius)))
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
      mActiveNeedsUpdate[collisionSegments[ui].x] = 1.0;
      //mStillNeedsUpdate[collisionSegments[ui].y] = 1.0;
      //mStillNeedsUpdate[collisionSegments[ui].z] = 1.0;

      Vector3 dist1 = mActiveControlPoints[collisionSegments[ui].x] - mStillControlPoints[collisionSegments[ui].y];
      Vector3 dist2 = mActiveControlPoints[collisionSegments[ui].x] - mStillControlPoints[collisionSegments[ui].z];
      float len1 = dist1.Length();
      float len2 = dist2.Length();
      mStillNeedsUpdate[collisionSegments[ui].y] += 1.0 - len1 / (len1+len2);
      mStillNeedsUpdate[collisionSegments[ui].z] += 1.0 - len2 / (len1+len2);

      if (mStillNeedsUpdate[collisionSegments[ui].y] > 1.0) mStillNeedsUpdate[collisionSegments[ui].y] = 1.0;
      if (mStillNeedsUpdate[collisionSegments[ui].z] > 1.0) mStillNeedsUpdate[collisionSegments[ui].z] = 1.0;

    }
    for (unsigned int ui = halflist ; ui < collisionSegments.size() ; ui++)
    {
      mStillNeedsUpdate[collisionSegments[ui].x] = 1.0;
      //mActiveNeedsUpdate[collisionSegments[ui].y] = 1.0;
      //mActiveNeedsUpdate[collisionSegments[ui].z] = 1.0;

      Vector3 dist1 = mStillControlPoints[collisionSegments[ui].x] - mActiveControlPoints[collisionSegments[ui].y];
      Vector3 dist2 = mStillControlPoints[collisionSegments[ui].x] - mActiveControlPoints[collisionSegments[ui].z];
      float len1 = dist1.Length();
      float len2 = dist2.Length();
      mActiveNeedsUpdate[collisionSegments[ui].y] += 1.0 - len1 / (len1+len2);
      mActiveNeedsUpdate[collisionSegments[ui].z] += 1.0 - len2 / (len1+len2);

      if (mActiveNeedsUpdate[collisionSegments[ui].y] > 1.0) mActiveNeedsUpdate[collisionSegments[ui].y] = 1.0;
      if (mActiveNeedsUpdate[collisionSegments[ui].z] > 1.0) mActiveNeedsUpdate[collisionSegments[ui].z] = 1.0;

    }
    collisionSegments.clear();
    ret = true;
  }

  return ret;
}

void PhysicsBallsController::CalculateForces()
{
  if (GetBallsCollision())
  {
    //Calculate forces derived the collisions between the balls
    if (GetControlPointsAffectedByCollisions())
    {
      float modifStrength = 1.2f;

      //We move the small ball if there is a collision
      //Vector3 direction = (mStillControlPoints[BONE_NUMBER] + mStillPos) - (mActiveControlPoints[BONE_NUMBER] + mActivePos);
      Vector3 direction = mStillPos - mActivePos;
      direction.Normalize();

      mDirectionStill = direction * (mActiveScale - mStillScale) * modifStrength*0.5;

      for (unsigned int ui = 0 ; ui < BONE_NUMBER ; ui++)
      {
        Vector3 direction = mActiveControlPoints[ui];// - mActiveControlPoints[BONE_NUMBER];
        direction.Normalize();
        if (mActiveNeedsUpdate[ui] > 0)
        {
          Vector3 modificationForSize = -direction * 1/16.f * (mActiveScale - mStillScale) * modifStrength * mActiveNeedsUpdate[ui];
          Vector3 forceVec = direction * 1/16.f * mActiveScale * mActiveNeedsUpdate[ui] + modificationForSize;
          mActiveCPForces[ui] -= forceVec;
        }
      }

      for (unsigned int ui = 0 ; ui < BONE_NUMBER ; ui++)
      {
        Vector3 direction = mStillControlPoints[ui];// - mStillControlPoints[BONE_NUMBER];
        direction.Normalize();
        if (mStillNeedsUpdate[ui] > 0)
        {
          Vector3 modificationForSize = -direction * 1/16.f * (mStillScale - mActiveScale) * modifStrength * mStillNeedsUpdate[ui];
          Vector3 forceVec = direction * 1/16.f * mStillScale * mStillNeedsUpdate[ui] + modificationForSize;
          mStillCPForces[ui] -= forceVec;
        }
      }
    }
  }
}

void PhysicsBallsController::UpdateSprings()
{
  //return ;
  //if there are forces applied to the bones
  if (true)
  {
    for (int i = 0 ; i < BONE_NUMBER ; i++)
    {
      if (mStillNeedsUpdate[i] < 0.0001)
      {
        Vector3 direction = mStillControlPoints[i];// - mStillControlPoints[BONE_NUMBER];
        float distance = direction.Length();
        if (distance < mStillRadius)
        {
          direction.Normalize();
          mStillCPForces[i] += direction * 1/16.f*1.1f;
        }
      }
      if (mActiveNeedsUpdate[i] < 0.0001)
      {
        Vector3 direction = mActiveControlPoints[i];// - mActiveControlPoints[BONE_NUMBER];
        float distance = direction.Length();
        if (distance < mActiveRadius)
        {
          direction.Normalize();
          mActiveCPForces[i] += direction * 1/16.f*1.1f;
        }
      }

      //if new length (bone-center) of spring is less than min or max than max length stop movement
      //Calculate distance to the center

      //if length from i to i+1 bone of spring is less than min or max than max length stop movement
      //Calculate distance from the other joint
    }
  }
  //do some small animation
  else
  {

  }
}

void PhysicsBallsController::SetNewPositionsToControlPoints()
{
  for (unsigned int ui = 0 ; ui < BONE_NUMBER ; ui++)
  {
    Vector3 newPosition = mActiveControlPoints[ui] + mActiveCPForces[ui];
    //If new position is inside the watch
    Vector3 newPositionCentered = newPosition + mActivePos;
    float distance = newPositionCentered.Length();
    if ((mScreenSize.x * 0.5) > distance)
      mActiveControlPoints[ui] = newPosition;
    else
    {
      //Hitting the screen
      //Try to go to the center
      Vector3 direction = Vector3(mActivePos.x,mActivePos.y,0);
      direction.Normalize();
      mDirectionActive = -direction * 10.f;
    }
  }

  for (unsigned int ui = 0 ; ui < BONE_NUMBER ; ui++)
  {
    Vector3 newPosition = mStillControlPoints[ui] + mStillCPForces[ui];
    //If new position is inside the watch
    Vector3 newPositionCentered = newPosition + mStillPos;
    float distance = newPositionCentered.Length();
    if ((mScreenSize.x * 0.5) > distance)
      mStillControlPoints[ui] = newPosition;
    else
    {
      //Hitting the screen
    }
  }

  //We move the circle if it is not going to get out of the screen
  //Vector3 dif = mNewActivePos - Vector3(mScreenSize.x * 0.5,mScreenSize.y * 0.5,0);
  //if ((mNewActivePos.Length() + mActiveRadius) < mScreenSize.x * 0.5)
  mActivePos = mNewActivePos;

  //Vector3 dif = mNewStillPos - Vector3(mScreenSize.x * 0.5,mScreenSize.y * 0.5,0);
  if ((mNewStillPos.Length() + mStillRadius) < mScreenSize.x * 0.5)
    mStillPos = mNewStillPos;
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
  controlPoints[BONE_NUMBER].x = center.x;
  controlPoints[BONE_NUMBER].y = center.y;
}

bool PhysicsBallsController::OnTimerTick()
{
  //Reset last frame vals
  ResetFrame();

  MoveBalls();

  ScaleBalls();

  CalculateForces();

  UpdateSprings();

  SetNewPositionsToControlPoints();

  RepositionCenterOfCircle(mActiveControlPoints);
  RepositionCenterOfCircle(mStillControlPoints);

  //Set position to Actors
  mActive.SetPosition(mActivePos);
  mStill.SetPosition(mStillPos);

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

  mTime += 1/16.f;

  return true;
}

//--------------------------------------------------------------------------------------------

bool PhysicsBallsController::OnTouch( Actor actor, const TouchEvent& touch )
{
  const TouchPoint &point = touch.GetPoint(0);

  switch(point.state)
  {
    case TouchPoint::Down:
    {
      if (mPause)
        mTimer.Start();
      else
        mTimer.Stop();
      mPause = ! mPause;
      break;
    }
    case TouchPoint::Motion:
    {
      break;
    }
    case TouchPoint::Up:
    case TouchPoint::Leave:
    case TouchPoint::Interrupted:
    {
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
