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

#include <cstdio>
#include <string>

#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/actors/mesh-actor.h>
#include <dali/devel-api/modeling/material.h>
#include <dali/devel-api/geometry/mesh.h>

#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char * const BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-2.jpg" );
const char * const TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );

const float GRAVITY_X(0);
const float GRAVITY_Y(-0.09);
const float UNITY_RADIUS(0.0625);
}

#define METABALL_NUMBER 24
#define METABALL_GROUPS 2

const char*const METABALL_FRAG_SHADER = DALI_COMPOSE_SHADER (
  precision mediump float;\n
  uniform vec2 uPositionMetaball;\n
  uniform vec2 uPositionVar;\n
  uniform vec2 uGravityVector;\n
  uniform float uRadius;\n
  uniform float uRadiusVar;\n
  uniform float uAspectRatio;\n
  void main()\n
  {\n
    vec2 adjustedCoords = vTexCoord * 2.0 - 1.0;\n
    vec2 finalMetaballPosition = uPositionMetaball + uGravityVector + uPositionVar;\n

    vec2 distanceVec = adjustedCoords - finalMetaballPosition;
    float distance = length(distanceVec);
    float finalRadius = uRadius + uRadiusVar;\n
    float color = finalRadius / distance;\n

    gl_FragColor = vec4(color * uMaterial.mDiffuse.r,color * uMaterial.mDiffuse.g,color * uMaterial.mDiffuse.b,1.0);\n
  }\n
);

const char*const REFRACTION_FRAG_SHADER = DALI_COMPOSE_SHADER (
  precision highp float;\n
  uniform vec2 uPositionMetaball;\n
  uniform vec2 uTextureSize;\n
  void main()\n
  {\n
    vec2 zoomCoords;\n
    float bright = 1.0;\n
    vec3 normal = vec3(0.0,0.0,1.0);\n
    vec2 fakePos = vec2(0.0,0.0);\n
    vec3 color = vec3(0,0,0);\n

    vec4 metaColor = texture2D(sEffect, vTexCoord);\n

    //Get color and intensity
    float metaballIntensity = 0.0;\n
    if (metaColor.r < 0.85 && metaColor.r > 0.75)
    {
      metaballIntensity = metaColor.r;
      color = vec3(1.0,0.5,0.5);\n
    }
    else if ((metaColor.r > 0.75) && (metaColor.g < 0.85 && metaColor.g > 0.75))
    {
      metaballIntensity = metaColor.r;
      color = vec3(1.0,0.5,0.5);\n
    }
    else if (metaColor.g < 0.85 && metaColor.g > 0.75)
    {
      metaballIntensity = metaColor.g;
      color = vec3(0.5,1.0,0.5);\n
    }
    else
    {
      metaballIntensity = max(metaColor.r, metaColor.g);
      if (metaColor.r > 0.75)
        color = vec3(1.0,0.5,0.5);\n
      else if (metaColor.g > 0.75)
        color = vec3(0.5,1.0,0.5);\n
    }

    //Calculate metaballs
    if (metaballIntensity > 0.85)\n
    {\n
      zoomCoords = ((vTexCoord - 0.5) * 0.9);\n
      //zoomCoords.y = -zoomCoords.y;
      zoomCoords = zoomCoords + 0.5;\n

      normal = vec3(0.0,0.0,1.0);\n
    }\n
    else if (metaballIntensity > 0.75)\n
    {\n
      float interpolation = mix(0.9, 1.15, (0.85 - metaballIntensity) * 10.0);\n
      zoomCoords = ((vTexCoord - 0.5) * interpolation);\n
      //zoomCoords.y = -zoomCoords.y;
      zoomCoords = zoomCoords + 0.5;\n
      bright = 1.2;\n

      fakePos = (vTexCoord.xy - 0.5) - uPositionMetaball;
      float len = length(fakePos);\n

      float interpNormal = mix(1.0, 0.0, (0.85 - metaballIntensity) * 10.0);\n
      normal.xyz = vec3(fakePos.x * (1.0 - interpNormal) / len, fakePos.y * (1.0 - interpNormal) / len, interpNormal);\n
      normal.xyz = normalize(normal.xyz);\n
    }\n
    else\n
    {\n
      zoomCoords = vTexCoord;\n
      color = vec3(1.0,1.0,1.0);\n
    }\n

    //Calculate lights
    vec3 lightPosition = vec3(-500.0,-750.0,1000.0);\n
    vec3 vertex = vec3(fakePos.x,fakePos.y,0.0);\n

    vec3 vecToLight = normalize( lightPosition - vertex );\n

    float lightDiffuse = dot( vecToLight, normal );\n
    lightDiffuse = max(0.0,lightDiffuse);\n
    lightDiffuse = lightDiffuse * 0.5 + 0.5;

    vec3 vertexToEye = normalize(-vertex);\n
    vec3 lightReflect = normalize(reflect(vecToLight, normal));\n
    float specularFactor = dot(vertexToEye, lightReflect);\n
    specularFactor = specularFactor * specularFactor;
    specularFactor = specularFactor * specularFactor;
    specularFactor = specularFactor * specularFactor;
    specularFactor = specularFactor * specularFactor;
    specularFactor = max(0.0,specularFactor);\n

    vec4 texColor = texture2D(sTexture, zoomCoords);\n
    gl_FragColor.rgb = color.rgb * texColor.rgb * lightDiffuse + vec3(specularFactor);\n
    gl_FragColor.a = 1.0;
  }\n
 );

const char*const FRAG_SHADER = DALI_COMPOSE_SHADER (
  precision mediump float;\n
  void main()\n
  {\n
    gl_FragColor = texture2D(sTexture, vTexCoord);\n
  }\n
);


struct MetaballInfo
{
  ShaderEffect shader;
  MeshActor meshActor;
  Vector2 position;
  float   radius;
  float   initRadius;
  int     group;
};


/***************************************************************************/
/*Demo using Metaballs for collision and explosion of metaballs ************/
/***************************************************************************/
class MetaballDesintegrationController : public ConnectionTracker
{
public:
  MetaballDesintegrationController( Application& application );
  ~MetaballDesintegrationController();

  /**
   * Main create function, it creates the metaballs and all the basic animations
   */
  void Create( Application& app );

  /**
   * Touch Event
   */
  bool OnTouch( Actor actor, const TouchEvent& touch );

  /**
   * Key Down event
   */
  void OnKeyDown( const KeyEvent& touch );

private:
  Application&      mApplication;
  Vector2           mScreenSize;

  Layer             mContentLayer;

  Image             mBackImage;
  FrameBufferImage  mMetaballFBO;

  Actor             mMetaballRoot;
  MetaballInfo      mMetaballs[METABALL_NUMBER];

  Dali::ImageActor  mCompositionActor;

  ShaderEffect      mRefractionShader;
  ShaderEffect      mNormalShader;

  //Motion
  Vector2           mCurrentTouchPosition;
  Vector2           mMetaballPosVariation;
  Vector2           mMetaballPosVariationFrom;
  Vector2           mMetaballPosVariationTo;

  Vector2           mInitialPosition[METABALL_GROUPS];
  Vector2           mMetaballCenter[METABALL_GROUPS];

  //Animations
  Animation         mPositionVarAnimation[METABALL_NUMBER];

  int               mDispersion;
  Animation         mDispersionAnimation[METABALL_NUMBER];

  bool              mDispersed;

  int               mSwipe;
  Vector2           mOldTouchPos;

  Animation         mMovement[METABALL_NUMBER];
  Animation         mResize[METABALL_NUMBER];
  bool              mAnimating;
  float             mResizeAmount;

  float             mStrength;
  float             mTimeMult;
  float             mBaseMov;
  float             mBaseRadius;

  Timer             mTimerDispersion;

  //Private functions

  /**
   * Create a mesh data with the geometry for the metaball rendering
   */
  Mesh              CreateMesh(int group);

  /**
   * Create a mesh actor for the metaballs
   */
  void              CreateMetaballActors();

  /**
   * Create the render task and FBO to render the metaballs into a texture
   */
  void              CreateMetaballImage();

  /**
   * Create a mesh image to render the final composition
   */
  void              AddRefractionImage();

  /**
   * Function to create animations for the small variations of position inside the metaball
   */
  void              CreateAnimations();

  /**
   * Function to reset metaball state
   */
  void              ResetMetaballs(bool resetAnims);

  /**
   * Function to create disperse each of the ball that compose the metaball
   */
  void              DisperseBallAnimation(int ball);

  /**
   * Function to create resize animations (resize and internal movement of the metaballs)
   */
  void              ResizeAnimations();

  /**
   * Function to reset size and position of the two balls
   */
  void              ResetSize();

  /**
   * Function to set next step of the animation
   */
  void              LaunchResizeAnimations(Animation &source);

  /**
   * Function to set next step of the animation
   */
  void              LaunchResizeAnimations2(Animation &source);

  /**
   * End of the different animations
   */
  void              EndResizeAnimations(Animation &source);

  /**
   * Function to make metaballs come back to reset position
   */
  void              LaunchResetMetaballPosition(Animation &source);

  /**
   * Function to set some states when dispersion finishes
   */
  void              EndMetaballAnimtaion(Animation &source);

  /**
   * Function to init dispersion of the metaballs one by one (not all at the same time)
   */
  bool              OnTimerDispersionTick();

  /**
   * Function to set the actual position of the metaballs when the user clicks the screen
   */
  void              SetPositionToMetaballs(Vector2 & metaballCenter);
};


//-----------------------------------------------------------------------------------------------
//
//	IMPLEMENTATION
//
//----------------

MetaballDesintegrationController::MetaballDesintegrationController( Application& application )
  : mApplication( application )
{
  // Connect to the Application's Init signal
  mApplication.InitSignal().Connect( this, &MetaballDesintegrationController::Create );
}

MetaballDesintegrationController::~MetaballDesintegrationController()
{
  // Nothing to do here;
}

void MetaballDesintegrationController::Create( Application& app )
{
  DemoHelper::RequestThemeChange();

  Stage stage = Stage::GetCurrent();
  mScreenSize = stage.GetSize();

  stage.SetBackgroundColor(Color::BLACK);

  //Set background image for the view
  mBackImage = ResourceImage::New( BACKGROUND_IMAGE );

  srand((unsigned)time(0));

  mInitialPosition[0] = Vector2(0.5f,0.f);
  mInitialPosition[1] = Vector2(-0.5f,0.f);

  mAnimating = false;
  mResizeAmount = 1.0f;

  mStrength = mResizeAmount * 1.5f + 1.f;
  mTimeMult = 0.8f;
  mBaseMov = 0.1f;
  mBaseRadius = UNITY_RADIUS / 10.f;

  //Create internal data
  CreateMetaballActors();
  CreateMetaballImage();
  AddRefractionImage();

  CreateAnimations();

  mDispersion = 0;
  mTimerDispersion = Timer::New( 150 );
  mTimerDispersion.TickSignal().Connect(this, &MetaballDesintegrationController::OnTimerDispersionTick);

  mDispersed = false;

  //We draw with the refraction-composition shader
  mCompositionActor.SetShaderEffect(mRefractionShader);

  //Set the metaballs in the center for illumination purposes
  Vector2 metaballCenter(0.0,0);
  SetPositionToMetaballs(metaballCenter);
  metaballCenter.x = metaballCenter.x * 0.5;
  metaballCenter.y = metaballCenter.y * 0.5;
  mRefractionShader.SetUniform("uPositionMetaball", metaballCenter);


  // Connect the callback to the touch signal on the mesh actor
  stage.GetRootLayer().TouchedSignal().Connect( this, &MetaballDesintegrationController::OnTouch );
  stage.KeyEventSignal().Connect( this, &MetaballDesintegrationController::OnKeyDown );
}

Mesh MetaballDesintegrationController::CreateMesh(int group)
{
  //We create the meshdata for the metaballs

  float aspect = (float)mScreenSize.y / (float)mScreenSize.x;

  // Create vertices and specify their color
  float xsize = mScreenSize.x * 0.5;
  MeshData::VertexContainer vertices(4);
  vertices[ 0 ] = MeshData::Vertex( Vector3( -xsize, -xsize * aspect, 0.0f ), Vector2(0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f) );
  vertices[ 1 ] = MeshData::Vertex( Vector3(  xsize, -xsize * aspect, 0.0f ), Vector2(1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f) );
  vertices[ 2 ] = MeshData::Vertex( Vector3( -xsize,  xsize * aspect, 0.0f ), Vector2(0.0f, 1.0f * aspect), Vector3(0.0f, 0.0f, 1.0f) );
  vertices[ 3 ] = MeshData::Vertex( Vector3(  xsize,  xsize * aspect, 0.0f ), Vector2(1.0f, 1.0f * aspect), Vector3(0.0f, 0.0f, 1.0f) );

  // Specify all the facesradius
  MeshData::FaceIndices faces;
  faces.reserve( 6 ); // 2 triangles in Quad
  faces.push_back( 0 ); faces.push_back( 3 ); faces.push_back( 1 );
  faces.push_back( 0 ); faces.push_back( 2 ); faces.push_back( 3 );

  Material baseMaterial = Material::New( "Material1" );
  if (group)
  {
    baseMaterial.SetDiffuseColor( Vector4(1.0f,0.0f,0.0f,1.0f));
  }
  else
  {
    baseMaterial.SetDiffuseColor( Vector4(0.0f,1.0f,0.0f,1.0f));
  }
  baseMaterial.SetDiffuseTexture( mBackImage );

  // Create the mesh data from the vertices and faces
  MeshData meshData;
  meshData.SetMaterial( baseMaterial );
  meshData.SetVertices( vertices );
  meshData.SetFaceIndices( faces );
  meshData.SetHasTextureCoords( true );
  //meshData.SetHasNormals( true );

  // Create a mesh from the data
  Dali::Mesh mesh = Mesh::New( meshData );
  return mesh;
}

float randomNumber(float lowest, float highest)
{
  float range=(highest-lowest);
  return lowest+range*rand()/RAND_MAX;
}

void MetaballDesintegrationController::CreateMetaballActors()
{
  //We create metaball structures
  //With MeshData Textured
  float aspect = (float)mScreenSize.y / (float)mScreenSize.x;

  //Initialization of each of the metaballs
  for (int i = 0 ; i < METABALL_NUMBER ; i++)
  {
    if (i < METABALL_NUMBER * 0.5f)
    {
      mMetaballs[i].group = 0;
      mMetaballs[i].position = mInitialPosition[0];
    }
    else
    {
      mMetaballs[i].group = 1;
      mMetaballs[i].position = mInitialPosition[1];
    }

    float radius = UNITY_RADIUS / 10.f * 5.f;
    mMetaballs[i].radius = mMetaballs[i].initRadius = radius;//randomNumber(radius,radius);

    mMetaballs[i].shader = ShaderEffect::New( "", METABALL_FRAG_SHADER, GEOMETRY_TYPE_TEXTURED_MESH  );
    mMetaballs[i].shader.SetUniform("uPositionMetaball", mMetaballs[i].position);
    mMetaballs[i].shader.SetUniform("uPositionVar", Vector2(0,0));
    mMetaballs[i].shader.SetUniform("uGravityVector", Vector2(0.f,0.f));
    //mMetaballs[i].shader.SetUniform("uGravityVector", Vector2(randomNumber(-0.1,0.1),randomNumber(-0.1,0.1)));
    mMetaballs[i].shader.SetUniform("uRadius", mMetaballs[i].radius);
    mMetaballs[i].shader.SetUniform("uRadiusVar", 0.f);
    mMetaballs[i].shader.SetUniform("uAspectRatio", aspect);

    mMetaballs[i].meshActor = MeshActor::New( CreateMesh(mMetaballs[i].group) );
    mMetaballs[i].meshActor.SetName("Metaball");
    mMetaballs[i].meshActor.SetScale( 1.0f );
    mMetaballs[i].meshActor.SetParentOrigin( ParentOrigin::CENTER );
    mMetaballs[i].meshActor.SetShaderEffect(mMetaballs[i].shader);
    mMetaballs[i].meshActor.SetBlendMode(BlendingMode::ON );
    mMetaballs[i].meshActor.SetBlendFunc(BlendingFactor::ONE, BlendingFactor::ONE,
                                         BlendingFactor::ONE, BlendingFactor::ONE);
  }

  //Root creation
  mMetaballRoot = Actor::New();
  mMetaballRoot.SetParentOrigin( ParentOrigin::CENTER );
  for (int i = 0 ; i < METABALL_NUMBER ; i++)
  {
    mMetaballRoot.Add( mMetaballs[i].meshActor );
  }

  //Initialization of variables related to metaballs
  mMetaballPosVariation = Vector2(0,0);
  mMetaballPosVariationFrom = Vector2(0,0);
  mMetaballPosVariationTo = Vector2(0,0);
  mCurrentTouchPosition = Vector2(0,0);
}

void MetaballDesintegrationController::CreateMetaballImage()
{
  //We create an FBO and a render task to create to render the metaballs with a fragment shader
  Stage stage = Stage::GetCurrent();
  mMetaballFBO = FrameBufferImage::New(mScreenSize.x, mScreenSize.y );

  stage.Add(mMetaballRoot);

  //Creation of the render task used to render the metaballs
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask task = taskList.CreateTask();
  task.SetRefreshRate( RenderTask::REFRESH_ALWAYS );
  task.SetSourceActor( mMetaballRoot );
  task.SetExclusive(true);
  task.SetClearColor( Color::BLACK );
  task.SetClearEnabled( true );
  task.SetTargetFrameBuffer( mMetaballFBO );
}

void MetaballDesintegrationController::AddRefractionImage()
{
  //Create Gaussian blur for the rendered image
  FrameBufferImage fbo;
  fbo = FrameBufferImage::New( mScreenSize.x, mScreenSize.y, Pixel::RGBA8888);

  GaussianBlurView gbv = GaussianBlurView::New(5, 2.0f, Pixel::RGBA8888, 0.5f, 0.5f, true);
  gbv.SetBackgroundColor(Color::TRANSPARENT);
  gbv.SetUserImageAndOutputRenderTarget( mMetaballFBO, fbo );
  gbv.SetSize(mScreenSize.x, mScreenSize.y);
  Stage::GetCurrent().Add(gbv);
  gbv.Activate();

  //Creation of the composition image
  mRefractionShader = ShaderEffect::New( "", REFRACTION_FRAG_SHADER );
  mRefractionShader.SetEffectImage( fbo );
  mRefractionShader.SetUniform("uTextureSize",mScreenSize);

  mCompositionActor = ImageActor::New( mBackImage );
  mCompositionActor.SetParentOrigin(ParentOrigin::CENTER);
  mCompositionActor.SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  mCompositionActor.SetSize(mScreenSize.x, mScreenSize.y);
  mCompositionActor.SetShaderEffect(mRefractionShader);

  Stage stage = Stage::GetCurrent();
  stage.Add( mCompositionActor );
}

void MetaballDesintegrationController::CreateAnimations()
{
  Vector2 direction;

  for (int i = 0 ; i < METABALL_NUMBER ; i++)
  {
    float key;
    KeyFrames keySinCosVariation = KeyFrames::New();
    Vector2 sinCosVariation(0,0);

    direction.x = randomNumber(-100.f,100.f);
    direction.y = randomNumber(-100.f,100.f);

    direction.Normalize();
    direction *= 0.03f;

    for (int j = 0 ; j < 360 ; j++)
    {
      sinCosVariation.x = sin(j * Math::PI/180.f) * direction.x;
      sinCosVariation.y = cos(j * Math::PI/180.f) * direction.y;
      key = j/360.f;
      keySinCosVariation.Add(key, sinCosVariation);
    }

    mPositionVarAnimation[i] = Animation::New(3.f);
    mPositionVarAnimation[i].AnimateBetween(Property( mMetaballs[i].shader, "uPositionVar" ), keySinCosVariation);
    mPositionVarAnimation[i].SetLooping( true );
    mPositionVarAnimation[i].Play();
 }
}

void MetaballDesintegrationController::ResetMetaballs(bool resetAnims)
{
  for (int i = 0 ; i < METABALL_NUMBER ; i++)
  {
    if (mDispersionAnimation[i])
      mDispersionAnimation[i].Clear();

    mMetaballs[i].position = mInitialPosition[mMetaballs[i].group];
    mMetaballs[i].shader.SetUniform("uPositionMetaball", mMetaballs[i].position);
  }
  mTimerDispersion.Stop();
  mDispersion = 0;
}

void MetaballDesintegrationController::DisperseBallAnimation(int ball)
{
  Vector2 direction;
  direction.x = randomNumber(-100.f,100.f);
  direction.y = randomNumber(-100.f,100.f);

  direction.Normalize();
  direction *= randomNumber(0.8f, 1.2f);

  mDispersionAnimation[ball] = Animation::New(1.f);
  mDispersionAnimation[ball].AnimateBy(Property( mMetaballs[ball].shader, "uPositionMetaball" ), direction);
  mDispersionAnimation[ball].SetLooping( false );
  mDispersionAnimation[ball].Play();

  if (ball == (METABALL_NUMBER * 0.5) - 1)
    mDispersionAnimation[ball].FinishedSignal().Connect( this, &MetaballDesintegrationController::LaunchResetMetaballPosition );
}

void MetaballDesintegrationController::LaunchResetMetaballPosition(Animation &source)
{
  //ResetMetaballs(false);
  for (int i = 0 ; i < METABALL_NUMBER * 0.5 ; i++)
  {
    mDispersionAnimation[i] = Animation::New(1.5f+i*0.25f);
    mDispersionAnimation[i].AnimateTo(Property( mMetaballs[i].shader, "uPositionMetaball" ), mInitialPosition[mMetaballs[i].group]);
    mDispersionAnimation[i].SetLooping( false );
    mDispersionAnimation[i].Play();

    mDispersionAnimation[i + int(METABALL_NUMBER * 0.5)] = Animation::New(1.5f+i*0.25f);
    mDispersionAnimation[i + int(METABALL_NUMBER * 0.5)].AnimateTo(Property( mMetaballs[i + int(METABALL_NUMBER * 0.5)].shader, "uPositionMetaball" ),
                                                                   mInitialPosition[mMetaballs[i + int(METABALL_NUMBER * 0.5)].group]);
    mDispersionAnimation[i + int(METABALL_NUMBER * 0.5)].SetLooping( false );
    mDispersionAnimation[i + int(METABALL_NUMBER * 0.5)].Play();

    if (i == (METABALL_NUMBER * 0.5) - 1)
      mDispersionAnimation[i].FinishedSignal().Connect( this, &MetaballDesintegrationController::EndMetaballAnimtaion );

  }
}

void MetaballDesintegrationController::EndMetaballAnimtaion(Animation &source)
{
  ResetMetaballs(true);
  mDispersed = false;
}


bool MetaballDesintegrationController::OnTimerDispersionTick()
{
  if (mDispersion < METABALL_NUMBER * 0.5)
  {
    DisperseBallAnimation(mDispersion);
    DisperseBallAnimation(mDispersion + METABALL_NUMBER * 0.5);
    mDispersion++;
  }

  mDispersed = true;

  return true;
}

void MetaballDesintegrationController::SetPositionToMetaballs(Vector2 & metaballCenter)
{
  //We set the position for the metaballs based on click position
  for (int i = 0 ; i < METABALL_NUMBER ; i++)
  {
    //mMetaballs[i].position = metaballCenter;
    mMetaballs[i].shader.SetUniform("uPositionMetaball", mMetaballs[i].position);
  }
}

bool MetaballDesintegrationController::OnTouch( Actor actor, const TouchEvent& touch )
{
  const TouchPoint &point = touch.GetPoint(0);

  switch(point.state)
  {
    case TouchPoint::Down:
    {
      mSwipe = 0;
      ResetMetaballs(true);

      mOldTouchPos = point.screen;
      break;
    }
    case TouchPoint::Motion:
    {
      if (point.screen.x - mOldTouchPos.x > 0)
        mSwipe = 1;
      else if (point.screen.x - mOldTouchPos.x < 0)
        mSwipe = -1;
      break;
    }
    case TouchPoint::Up:
    case TouchPoint::Leave:
    case TouchPoint::Interrupted:
    {
      if (mSwipe > 0)
        ResizeAnimations();
      else if (mSwipe < 0)
        ResetSize();
      else
        mTimerDispersion.Start();
      break;
    }
    default:
      break;
    }
  return true;
}

//-----------------------------------------------------------------------

void MetaballDesintegrationController::ResizeAnimations()
{
  if (mAnimating)
    return;

  mAnimating = true;

  mStrength = mMetaballs[0].radius * 150.f;

  int i = 0;
  int init;
  int limit = int(METABALL_NUMBER * 0.125);
  for (i = 0 ; i < limit ; i++)
  {
    mMetaballs[i].position.x -= (mResizeAmount+mStrength) * mBaseMov;

    mMovement[i] = Animation::New(0.25f * mTimeMult);
    mMovement[i].AnimateTo(Property( mMetaballs[i].shader, "uPositionMetaball" ), mMetaballs[i].position);
    mMovement[i].SetLooping( false );
    mMovement[i].Play();
  }

  limit = int(METABALL_NUMBER * 0.375);
  for ( ; i < limit ; i++)
  {
    mMetaballs[i].position.x -= mResizeAmount * mBaseMov;

    mMovement[i] = Animation::New(0.25f * mTimeMult);
    mMovement[i].AnimateTo(Property( mMetaballs[i].shader, "uPositionMetaball" ), mMetaballs[i].position);
    mMovement[i].SetLooping( false );
    mMovement[i].Play();

  }

  limit = int(METABALL_NUMBER * 0.5);
  for ( ; i < limit ; i++)
  {
    mMetaballs[i].position.x += (mStrength - mResizeAmount) * mBaseMov;

    mMovement[i] = Animation::New(0.25f * mTimeMult);
    mMovement[i].AnimateTo(Property( mMetaballs[i].shader, "uPositionMetaball" ), mMetaballs[i].position);
    mMovement[i].SetLooping( false );
    mMovement[i].Play();

    if (i == limit - 1)
      mMovement[i].FinishedSignal().Connect( this, &MetaballDesintegrationController::LaunchResizeAnimations );

  }

  //-----------------------------------------------------------------------------------------------

  init = int(METABALL_NUMBER * 0.625);
  limit = int(METABALL_NUMBER * 0.75);
  for ( i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.y -= mStrength * 0.75 * mBaseMov;
  }

  init = int(METABALL_NUMBER * 0.75);
  limit = int(METABALL_NUMBER * 0.875);
  for ( i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.y += mStrength * 0.75 * mBaseMov;
  }

  for ( i = METABALL_NUMBER * 0.5 ; i < METABALL_NUMBER; i++)
  {
    mMetaballs[i].position.x -= (mStrength * 0.5) * mBaseMov;

    mMovement[i] = Animation::New(0.25f * mTimeMult);
    mMovement[i].AnimateTo(Property( mMetaballs[i].shader, "uPositionMetaball" ), mMetaballs[i].position);
    mMovement[i].SetLooping( false );
    mMovement[i].Play();
  }

  init = int(METABALL_NUMBER * 0.5);
  limit = int(METABALL_NUMBER);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].radius -= mBaseRadius * mResizeAmount * 0.5f;

    mResize[i] = Animation::New(0.25f * mTimeMult);
    mResize[i].AnimateTo(Property( mMetaballs[i].shader, "uRadius" ), mMetaballs[i].radius);
    mResize[i].SetLooping( false );
    mResize[i].Play();
  }

}

void MetaballDesintegrationController::LaunchResizeAnimations(Animation &source)
{
  int init = 0;
  int limit = int(METABALL_NUMBER * 0.125);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.x += mStrength * mBaseMov;

    mMovement[i] = Animation::New(0.25f * mTimeMult);
    mMovement[i].AnimateTo(Property( mMetaballs[i].shader, "uPositionMetaball" ), mMetaballs[i].position);
    mMovement[i].SetLooping( false );
    mMovement[i].Play();
  }

  init = int(METABALL_NUMBER * 0.125);
  limit = int(METABALL_NUMBER * 0.25);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.y += mStrength * mBaseMov;

    mMovement[i] = Animation::New(0.25f * mTimeMult);
    mMovement[i].AnimateTo(Property( mMetaballs[i].shader, "uPositionMetaball" ), mMetaballs[i].position);
    mMovement[i].SetLooping( false );
    mMovement[i].Play();
  }

  init = int(METABALL_NUMBER * 0.25);
  limit = int(METABALL_NUMBER * 0.375);
  for (int i = init ; i < limit; i++)
  {
    mMetaballs[i].position.y -= mStrength * mBaseMov;

    mMovement[i] = Animation::New(0.25f * mTimeMult);
    mMovement[i].AnimateTo(Property( mMetaballs[i].shader, "uPositionMetaball" ), mMetaballs[i].position);
    mMovement[i].SetLooping( false );
    mMovement[i].Play();
  }

  init = int(METABALL_NUMBER * 0.375);
  limit = int(METABALL_NUMBER * 0.5);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.x -= mStrength * mBaseMov;

    mMovement[i] = Animation::New(0.25f * mTimeMult);
    mMovement[i].AnimateTo(Property( mMetaballs[i].shader, "uPositionMetaball" ), mMetaballs[i].position);
    mMovement[i].SetLooping( false );
    mMovement[i].Play();

    if (i == limit - 1)
      mMovement[i].FinishedSignal().Connect( this, &MetaballDesintegrationController::LaunchResizeAnimations2 );
  }



  //-----------------------------------------------------------------------------------------------



  init = int(METABALL_NUMBER * 0.5);
  limit = int(METABALL_NUMBER * 0.625);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.x += mStrength * mBaseMov;
  }

  init = int(METABALL_NUMBER * 0.625);
  limit = int(METABALL_NUMBER * 0.875);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.x += mStrength * 0.5 * mBaseMov;
  }

  init = int(METABALL_NUMBER * 0.875);
  limit = int(METABALL_NUMBER);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.x += (mStrength - mResizeAmount - (mStrength * 0.5)) * mBaseMov;
  }

  init = int(METABALL_NUMBER * 0.625);
  limit = int(METABALL_NUMBER * 0.875);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.y = 0;
  }

  for (int i = METABALL_NUMBER * 0.5 ; i < METABALL_NUMBER; i++)
  {
    mMovement[i] = Animation::New(0.25f * mTimeMult);
    mMovement[i].AnimateTo(Property( mMetaballs[i].shader, "uPositionMetaball" ), mMetaballs[i].position);
    mMovement[i].SetLooping( false );
    mMovement[i].Play();
  }

  init = int(METABALL_NUMBER * 0.5);
  limit = int(METABALL_NUMBER);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].radius -= mBaseRadius * mResizeAmount * 0.5f;

    mResize[i] = Animation::New(0.25f * mTimeMult);
    mResize[i].AnimateTo(Property( mMetaballs[i].shader, "uRadius" ), mMetaballs[i].radius);
    mResize[i].SetLooping( false );
    mResize[i].Play();
  }

}

void MetaballDesintegrationController::LaunchResizeAnimations2(Animation &source)
{
  int init = int(METABALL_NUMBER * 0.125);
  int limit = int(METABALL_NUMBER * 0.375);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.y = 0;

    mMovement[i] = Animation::New(0.25f * mTimeMult);
    mMovement[i].AnimateTo(Property( mMetaballs[i].shader, "uPositionMetaball" ), mMetaballs[i].position);
    mMovement[i].SetLooping( false );
    mMovement[i].Play();

    if (i == limit - 1)
      mMovement[i].FinishedSignal().Connect( this, &MetaballDesintegrationController::EndResizeAnimations );
  }

  for (int i = 0 ; i < METABALL_NUMBER * 0.5; i++)
  {
    mMetaballs[i].radius += mBaseRadius * mResizeAmount;

    mResize[i] = Animation::New(0.25f * mTimeMult);
    mResize[i].AnimateTo(Property( mMetaballs[i].shader, "uRadius" ), mMetaballs[i].radius);
    mResize[i].SetLooping( false );
    mResize[i].Play();
  }

  //-----------------------------------------------------------------------------------------------


  init = int(METABALL_NUMBER * 0.5);
  limit = int(METABALL_NUMBER * 0.625);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.x = mInitialPosition[1].x - mBaseMov * mResizeAmount;
  }
  init = int(METABALL_NUMBER * 0.625);
  limit = int(METABALL_NUMBER * 0.875);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.x = mInitialPosition[1].x - mBaseMov * mResizeAmount;
  }

  for (int i = METABALL_NUMBER * 0.5 ; i < METABALL_NUMBER; i++)
  {
    mMovement[i] = Animation::New(0.25f * mTimeMult);
    mMovement[i].AnimateTo(Property( mMetaballs[i].shader, "uPositionMetaball" ), mMetaballs[i].position);
    mMovement[i].SetLooping( false );
    mMovement[i].Play();
  }

}

void MetaballDesintegrationController::EndResizeAnimations(Animation &source)
{
  mInitialPosition[0] = mMetaballs[0].position;
  mInitialPosition[1] = mMetaballs[int(METABALL_NUMBER * 0.5)].position;

  /*for (int i = 0 ; i < METABALL_NUMBER; i++)
  {
    printf("position %d %f %f\n", i, mMetaballs[i].position.x, mMetaballs[i].position.y);
  }*/

  mAnimating = false;
}

void MetaballDesintegrationController::ResetSize()
{
  mInitialPosition[0] = Vector2(0.5f,0.f);
  mInitialPosition[1] = Vector2(-0.5f,0.f);

  for (int i = 0 ; i < METABALL_NUMBER; i++)
  {
    if (i < METABALL_NUMBER * 0.5f)
    {
      mMetaballs[i].position = mInitialPosition[0];
    }
    else
    {
      mMetaballs[i].position = mInitialPosition[1];
    }

    float radius = UNITY_RADIUS / 10.f * 5.f;
    mMetaballs[i].radius = radius;

    mMovement[i] = Animation::New(0.25f * mTimeMult);
    mMovement[i].AnimateTo(Property( mMetaballs[i].shader, "uPositionMetaball" ), mMetaballs[i].position);
    mMovement[i].SetLooping( false );
    mMovement[i].Play();

    mResize[i] = Animation::New(0.25f * mTimeMult);
    mResize[i].AnimateTo(Property( mMetaballs[i].shader, "uRadius" ), mMetaballs[i].radius);
    mResize[i].SetLooping( false );
    mResize[i].Play();
  }
}

void MetaballDesintegrationController::OnKeyDown( const KeyEvent& event )
{
  if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
    mApplication.Quit();

  if (mDispersed)
    return;

  if (event.state == KeyEvent::Up)
  {
    if ( event.keyCode == Dali::DALI_KEY_CURSOR_UP )
        ResizeAnimations();
    if ( event.keyCode == Dali::DALI_KEY_CURSOR_DOWN )
        ResetSize();
  }

}


//
//
//-----------------------------------------------------------------------------------------------

void RunTest( Application& application )
{
  MetaballDesintegrationController test( application );

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
