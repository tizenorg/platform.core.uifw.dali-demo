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
  uniform vec2 positionMetaball;\n
  uniform vec2 positionVar;\n
  uniform vec2 gravityVector;\n
  uniform float radius;\n
  uniform float radiusVar;\n
  uniform float aspect;\n
  void main()\n
  {\n
    vec2 adjustedCoords = vTexCoord * 2.0 - 1.0;\n
    vec2 finalMetaballPosition = positionMetaball + gravityVector + positionVar;\n

    float distance = (adjustedCoords.x - finalMetaballPosition.x) * (adjustedCoords.x - finalMetaballPosition.x) +
                     (adjustedCoords.y - finalMetaballPosition.y) * (adjustedCoords.y - finalMetaballPosition.y);\n
    float finalRadius = radius + radiusVar;\n
    float color = finalRadius / sqrt( distance );\n

    gl_FragColor = vec4(color * uMaterial.mDiffuse.r,color * uMaterial.mDiffuse.g,color * uMaterial.mDiffuse.b,1.0);\n
  }\n
);

const char*const REFRACTION_FRAG_SHADER = DALI_COMPOSE_SHADER (
  precision highp float;\n
  uniform vec2 positionMetaball;\n
  uniform vec2 textureSize;\n
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

      fakePos = (vTexCoord.xy - 0.5) - positionMetaball;
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
/*Demo using Metaballs for Refraction when clicking the screen *************/
/***************************************************************************/
class MetaballDesintegrationController : public ConnectionTracker
{
public:
  MetaballDesintegrationController( Application& application );
  ~MetaballDesintegrationController();

  void Create( Application& app );
  bool OnTouch( Actor actor, const TouchEvent& touch );
  void OnKeyDown( const KeyEvent& touch );

private:
  Application&      mApplication;
  Vector2           mScreenSize;

  Toolkit::View     mView;                              ///< The View instance.
  Toolkit::ToolBar  mToolBar;                           ///< The View's Toolbar.

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

  Timer             mTimerDispersion;

  //Private functions
  Mesh              CreateMesh(int group);
  void              CreateMetaballActors();
  void              CreateMetaballImage();
  void              AddRefractionImage();

  void              CreateAnimations();

  void              ResetMetaballs(bool resetAnims);
  void              DisperseBallAnimation(int ball);

  void              ResizeAnimations();
  void              ResetSize();

  void              LaunchResizeAnimations(Animation &source);
  void              LaunchResizeAnimations2(Animation &source);
  void              EndResizeAnimations(Animation &source);

  void              LaunchResetMetaballPosition(Animation &source);
  //void              LaunchResetMetaballPosition();
  void              EndMetaballAnimtaion(Animation &source);
  bool              OnTimerDispersionTick();

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

/**
 * Main create function, it creates the metaballs and all the
 */
void MetaballDesintegrationController::Create( Application& app )
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

  srand((unsigned)time(0));

  mInitialPosition[0] = Vector2(0.5f,0.f);
  mInitialPosition[1] = Vector2(-0.5f,0.f);

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

  //we use the click position for the metaballs
  //Vector2 metaballCenter = Vector2((point.screen.x / mScreenSize.x) - 0.5, (aspectR * (mScreenSize.y - point.screen.y) / mScreenSize.y) - 0.5) * 2.0;
  Vector2 metaballCenter(0.0,0);
  SetPositionToMetaballs(metaballCenter);
  metaballCenter.x = metaballCenter.x * 0.5;
  metaballCenter.y = metaballCenter.y * 0.5;
  mRefractionShader.SetUniform("positionMetaball", metaballCenter);


  // Connect the callback to the touch signal on the mesh actor
  mView.TouchedSignal().Connect( this, &MetaballDesintegrationController::OnTouch );
  stage.KeyEventSignal().Connect( this, &MetaballDesintegrationController::OnKeyDown );
}

/**
 * Create a mesh data with the geometry for the metaball rendering
 */
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
  vertices[ 3 ] = MeshData::Vertex( Vector3(  xsize,  xsize * aspect,   //if (metaColor.r > 0.1)\n
                                              //metaballIntensity = metaColor.r;
                                            //else
 0.0f ), Vector2(1.0f, 1.0f * aspect), Vector3(0.0f, 0.0f, 1.0f) );

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

/**
 * Create a mesh actor for the metaballs
 */
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
    mMetaballs[i].shader.SetUniform("positionMetaball", mMetaballs[i].position);
    mMetaballs[i].shader.SetUniform("positionVar", Vector2(0,0));
    mMetaballs[i].shader.SetUniform("gravityVector", Vector2(0.f,0.f));
    //mMetaballs[i].shader.SetUniform("gravityVector", Vector2(randomNumber(-0.1,0.1),randomNumber(-0.1,0.1)));
    mMetaballs[i].shader.SetUniform("radius", mMetaballs[i].radius);
    mMetaballs[i].shader.SetUniform("radiusVar", 0.f);
    mMetaballs[i].shader.SetUniform("aspect", aspect);

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

  //mView.Add( mMetaballRoot );

  //Initialization of variables related to metaballs
  mMetaballPosVariation = Vector2(0,0);
  mMetaballPosVariationFrom = Vector2(0,0);
  mMetaballPosVariationTo = Vector2(0,0);
  mCurrentTouchPosition = Vector2(0,0);
}

/**
 * Create the render task and FBO to render the metaballs into a texture
 */
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

/**
 * Create a mesh image to render the final composition
 */
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
  mRefractionShader.SetUniform("textureSize",mScreenSize);

  mCompositionActor = ImageActor::New( mBackImage );
  mCompositionActor.SetParentOrigin(ParentOrigin::CENTER);
  mCompositionActor.SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  mCompositionActor.SetSize(mScreenSize.x, mScreenSize.y);
  mCompositionActor.SetShaderEffect(mRefractionShader);

  mView.Add( mCompositionActor );
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
    mPositionVarAnimation[i].AnimateBetween(Property( mMetaballs[i].shader, "positionVar" ), keySinCosVariation);
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
    mMetaballs[i].shader.SetUniform("positionMetaball", mMetaballs[i].position);
  }
  mTimerDispersion.Stop();
  mDispersion = 0;
}

/**
 * Function to
 */
void MetaballDesintegrationController::DisperseBallAnimation(int ball)
{
  //c++11 not supported :(
  //std::random_device rseed;
  //std::mt19937 rng(rseed());
  //std::uniform_int<int> dist(-100,100);

  Vector2 direction;
  direction.x = randomNumber(-100.f,100.f);
  direction.y = randomNumber(-100.f,100.f);

  direction.Normalize();
  direction *= randomNumber(0.8f, 1.2f);

  mDispersionAnimation[ball] = Animation::New(1.f);
  mDispersionAnimation[ball].AnimateBy(Property( mMetaballs[ball].shader, "positionMetaball" ), direction);
  mDispersionAnimation[ball].SetLooping( false );
  mDispersionAnimation[ball].Play();

  if (ball == (METABALL_NUMBER * 0.5) - 1)
    mDispersionAnimation[ball].FinishedSignal().Connect( this, &MetaballDesintegrationController::LaunchResetMetaballPosition );
}

/**
 * Function to make metaballs come back to reset position
 */
void MetaballDesintegrationController::LaunchResetMetaballPosition(Animation &source)
{
  //ResetMetaballs(false);
  for (int i = 0 ; i < METABALL_NUMBER * 0.5 ; i++)
  {
    mDispersionAnimation[i] = Animation::New(1.5f+i*0.25f);
    mDispersionAnimation[i].AnimateTo(Property( mMetaballs[i].shader, "positionMetaball" ), mInitialPosition[mMetaballs[i].group]);
    mDispersionAnimation[i].SetLooping( false );
    mDispersionAnimation[i].Play();

    mDispersionAnimation[i + int(METABALL_NUMBER * 0.5)] = Animation::New(1.5f+i*0.25f);
    mDispersionAnimation[i + int(METABALL_NUMBER * 0.5)].AnimateTo(Property( mMetaballs[i + int(METABALL_NUMBER * 0.5)].shader, "positionMetaball" ),
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


/**
 * Function to
 */
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

/**
 * Function to set the actual position of the metaballs when the user clicks the screen
 */
void MetaballDesintegrationController::SetPositionToMetaballs(Vector2 & metaballCenter)
{
  //We set the position for the metaballs based on click position
  for (int i = 0 ; i < METABALL_NUMBER ; i++)
  {
    //mMetaballs[i].position = metaballCenter;
    mMetaballs[i].shader.SetUniform("positionMetaball", mMetaballs[i].position);
  }
}

bool MetaballDesintegrationController::OnTouch( Actor actor, const TouchEvent& touch )
{
  const TouchPoint &point = touch.GetPoint(0);
  //float aspectR = mScreenSize.y / mScreenSize.x;

  switch(point.state)
  {
    case TouchPoint::Down:
    {
      mSwipe = 0;
      ResetMetaballs(true);

      mOldTouchPos = point.screen;

      //Vector2 metaballCenter = Vector2((point.screen.x / mScreenSize.x) - 0.5, (aspectR * (mScreenSize.y - point.screen.y) / mScreenSize.y) - 0.5) * 2.0;
      //SetPositionToMetaballs(metaballCenter);

      break;
    }
    case TouchPoint::Motion:
    {
      if (point.screen.x - mOldTouchPos.x > 0)
        mSwipe = 1;
      else if (point.screen.x - mOldTouchPos.x < 0)
        mSwipe = -1;
      //Vector2 metaballCenter = Vector2((point.screen.x / mScreenSize.x) - 0.5, (aspectR * (mScreenSize.y - point.screen.y) / mScreenSize.y) - 0.5) * 2.0;
      //SetPositionToMetaballs(metaballCenter);
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

Animation mov[METABALL_NUMBER];
Animation resize[METABALL_NUMBER];
bool animating = false;
float variance = 1.0f;

float force = variance * 1.5f + 1.f;
float timeMult = 0.8f;
float baseMov = 0.1f;
float baseRadius = UNITY_RADIUS / 10.f;


void MetaballDesintegrationController::ResizeAnimations()
{
  if (animating)
    return;

  animating = true;

  force = mMetaballs[0].radius * 150.f;

  int i = 0;
  int init;
  int limit = int(METABALL_NUMBER * 0.125);
  for (i = 0 ; i < limit ; i++)
  {
    mMetaballs[i].position.x -= (variance+force) * baseMov;

    mov[i] = Animation::New(0.25f * timeMult);
    mov[i].AnimateTo(Property( mMetaballs[i].shader, "positionMetaball" ), mMetaballs[i].position);
    mov[i].SetLooping( false );
    mov[i].Play();
  }

  limit = int(METABALL_NUMBER * 0.375);
  for ( ; i < limit ; i++)
  {
    mMetaballs[i].position.x -= variance * baseMov;

    mov[i] = Animation::New(0.25f * timeMult);
    mov[i].AnimateTo(Property( mMetaballs[i].shader, "positionMetaball" ), mMetaballs[i].position);
    mov[i].SetLooping( false );
    mov[i].Play();

  }

  limit = int(METABALL_NUMBER * 0.5);
  for ( ; i < limit ; i++)
  {
    mMetaballs[i].position.x += (force - variance) * baseMov;

    mov[i] = Animation::New(0.25f * timeMult);
    mov[i].AnimateTo(Property( mMetaballs[i].shader, "positionMetaball" ), mMetaballs[i].position);
    mov[i].SetLooping( false );
    mov[i].Play();

    if (i == limit - 1)
      mov[i].FinishedSignal().Connect( this, &MetaballDesintegrationController::LaunchResizeAnimations );

  }

  //-----------------------------------------------------------------------------------------------

  init = int(METABALL_NUMBER * 0.625);
  limit = int(METABALL_NUMBER * 0.75);
  for ( i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.y -= force * 0.75 * baseMov;
  }

  init = int(METABALL_NUMBER * 0.75);
  limit = int(METABALL_NUMBER * 0.875);
  for ( i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.y += force * 0.75 * baseMov;
  }

  for ( i = METABALL_NUMBER * 0.5 ; i < METABALL_NUMBER; i++)
  {
    mMetaballs[i].position.x -= (force * 0.5) * baseMov;

    mov[i] = Animation::New(0.25f * timeMult);
    mov[i].AnimateTo(Property( mMetaballs[i].shader, "positionMetaball" ), mMetaballs[i].position);
    mov[i].SetLooping( false );
    mov[i].Play();
  }

  init = int(METABALL_NUMBER * 0.5);
  limit = int(METABALL_NUMBER);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].radius -= baseRadius * variance * 0.5f;

    resize[i] = Animation::New(0.25f * timeMult);
    resize[i].AnimateTo(Property( mMetaballs[i].shader, "radius" ), mMetaballs[i].radius);
    resize[i].SetLooping( false );
    resize[i].Play();
  }

}

void MetaballDesintegrationController::LaunchResizeAnimations(Animation &source)
{
  int init = 0;
  int limit = int(METABALL_NUMBER * 0.125);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.x += force * baseMov;

    mov[i] = Animation::New(0.25f * timeMult);
    mov[i].AnimateTo(Property( mMetaballs[i].shader, "positionMetaball" ), mMetaballs[i].position);
    mov[i].SetLooping( false );
    mov[i].Play();
  }

  init = int(METABALL_NUMBER * 0.125);
  limit = int(METABALL_NUMBER * 0.25);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.y += force * baseMov;

    mov[i] = Animation::New(0.25f * timeMult);
    mov[i].AnimateTo(Property( mMetaballs[i].shader, "positionMetaball" ), mMetaballs[i].position);
    mov[i].SetLooping( false );
    mov[i].Play();
  }

  init = int(METABALL_NUMBER * 0.25);
  limit = int(METABALL_NUMBER * 0.375);
  for (int i = init ; i < limit; i++)
  {
    mMetaballs[i].position.y -= force * baseMov;

    mov[i] = Animation::New(0.25f * timeMult);
    mov[i].AnimateTo(Property( mMetaballs[i].shader, "positionMetaball" ), mMetaballs[i].position);
    mov[i].SetLooping( false );
    mov[i].Play();
  }

  init = int(METABALL_NUMBER * 0.375);
  limit = int(METABALL_NUMBER * 0.5);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.x -= force * baseMov;

    mov[i] = Animation::New(0.25f * timeMult);
    mov[i].AnimateTo(Property( mMetaballs[i].shader, "positionMetaball" ), mMetaballs[i].position);
    mov[i].SetLooping( false );
    mov[i].Play();

    if (i == limit - 1)
      mov[i].FinishedSignal().Connect( this, &MetaballDesintegrationController::LaunchResizeAnimations2 );
  }



  //-----------------------------------------------------------------------------------------------



  init = int(METABALL_NUMBER * 0.5);
  limit = int(METABALL_NUMBER * 0.625);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.x += force * baseMov;
  }

  init = int(METABALL_NUMBER * 0.625);
  limit = int(METABALL_NUMBER * 0.875);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.x += force * 0.5 * baseMov;
  }

  init = int(METABALL_NUMBER * 0.875);
  limit = int(METABALL_NUMBER);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.x += (force - variance - (force * 0.5)) * baseMov;
  }

  init = int(METABALL_NUMBER * 0.625);
  limit = int(METABALL_NUMBER * 0.875);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.y = 0;
  }

  for (int i = METABALL_NUMBER * 0.5 ; i < METABALL_NUMBER; i++)
  {
    mov[i] = Animation::New(0.25f * timeMult);
    mov[i].AnimateTo(Property( mMetaballs[i].shader, "positionMetaball" ), mMetaballs[i].position);
    mov[i].SetLooping( false );
    mov[i].Play();
  }

  init = int(METABALL_NUMBER * 0.5);
  limit = int(METABALL_NUMBER);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].radius -= baseRadius * variance * 0.5f;

    resize[i] = Animation::New(0.25f * timeMult);
    resize[i].AnimateTo(Property( mMetaballs[i].shader, "radius" ), mMetaballs[i].radius);
    resize[i].SetLooping( false );
    resize[i].Play();
  }

}

void MetaballDesintegrationController::LaunchResizeAnimations2(Animation &source)
{
  int init = int(METABALL_NUMBER * 0.125);
  int limit = int(METABALL_NUMBER * 0.375);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.y = 0;

    mov[i] = Animation::New(0.25f * timeMult);
    mov[i].AnimateTo(Property( mMetaballs[i].shader, "positionMetaball" ), mMetaballs[i].position);
    mov[i].SetLooping( false );
    mov[i].Play();

    if (i == limit - 1)
      mov[i].FinishedSignal().Connect( this, &MetaballDesintegrationController::EndResizeAnimations );
  }

  for (int i = 0 ; i < METABALL_NUMBER * 0.5; i++)
  {
    mMetaballs[i].radius += baseRadius * variance;

    resize[i] = Animation::New(0.25f * timeMult);
    resize[i].AnimateTo(Property( mMetaballs[i].shader, "radius" ), mMetaballs[i].radius);
    resize[i].SetLooping( false );
    resize[i].Play();
  }

  //-----------------------------------------------------------------------------------------------


  init = int(METABALL_NUMBER * 0.5);
  limit = int(METABALL_NUMBER * 0.625);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.x = mInitialPosition[1].x - baseMov * variance;
  }
  init = int(METABALL_NUMBER * 0.625);
  limit = int(METABALL_NUMBER * 0.875);
  for (int i = init ; i < limit ; i++)
  {
    mMetaballs[i].position.x = mInitialPosition[1].x - baseMov * variance;
  }

  for (int i = METABALL_NUMBER * 0.5 ; i < METABALL_NUMBER; i++)
  {
    mov[i] = Animation::New(0.25f * timeMult);
    mov[i].AnimateTo(Property( mMetaballs[i].shader, "positionMetaball" ), mMetaballs[i].position);
    mov[i].SetLooping( false );
    mov[i].Play();
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

  animating = false;
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

    mov[i] = Animation::New(0.25f * timeMult);
    mov[i].AnimateTo(Property( mMetaballs[i].shader, "positionMetaball" ), mMetaballs[i].position);
    mov[i].SetLooping( false );
    mov[i].Play();

    resize[i] = Animation::New(0.25f * timeMult);
    resize[i].AnimateTo(Property( mMetaballs[i].shader, "radius" ), mMetaballs[i].radius);
    resize[i].SetLooping( false );
    resize[i].Play();
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

/*
  float base = UNITY_RADIUS / 10.f * 1.f;
  float variation = 0;
  float newPosition = 0;
  if ( event.keyCode == Dali::DALI_KEY_CURSOR_UP)
  {
    variation = base;
    newPosition = 0.1f;
  }
  if ( event.keyCode == Dali::DALI_KEY_CURSOR_DOWN)
  {
    variation = -base;
    newPosition = -0.1f;
  }

  mInitialPosition[0].x -= newPosition * 1/16.f;
  mInitialPosition[1].x -= newPosition * 1/16.f;
  for (int i = 0 ; i < METABALL_NUMBER ; i++)
  {
    if (i < METABALL_NUMBER * 0.5)
    {
      mMetaballs[i].position.x -= newPosition * 1/16.f;
      mMetaballs[i].radius += variation * 1/16.f;
    }
    else
    {
      mMetaballs[i].position.x -= newPosition * 1/16.f;
      mMetaballs[i].radius -= variation * 1/16.f;
    }
    mMetaballs[i].shader.SetUniform("positionMetaball", mMetaballs[i].position);
    mMetaballs[i].shader.SetUniform("radius", mMetaballs[i].radius);
  }
  */
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

//----------------------------------------------------------------------------
//   Old Func
/*
void MetaballDesintegrationController::UpdateMetaballsPosition()
{
  float radiusInc = 0.f;
  //If the user just released the screen
  if (mExitClick)
  {
    //we update gravity position
    mGravityVar.x += mGravity.x * mTimerTime;
    mGravityVar.y += mGravity.y * mTimerTime;

    //we make the metaballs smaller
    radiusInc = -0.004f * mTimerTime;

    //If there are very small, we make them disappear and reset the state
    if ((mMetaballs[0].radius + radiusInc) < 0.01)
	{
	  mExitClick = false;
      ResetMetaballsState();
	  return;
	}
  }
  else
  {
    //When you begin clicking metaball gets bigger faster
    if (mTimeScale < 1.0)
    {
	  radiusInc = 0.06f * mTimerTime;
    }
    //Later it get bigger slower
    else
    {
	  radiusInc = 0.0005f * mTimerTime;
    }
  }

  //We move one of the metaballs, using movement from the finger
  //if (fabs(mMetaballPosVariationTo.x - mMetaballPosVariation.x) > 0.01 || fabs(mMetaballPosVariationTo.y - mMetaballPosVariation.y) > 0.01)
  //{
  //  mMetaballPosVariation.x += (mMetaballPosVariationTo.x - mMetaballPosVariation.x) * mTimerTime;
  //  mMetaballPosVariation.y += (mMetaballPosVariationTo.y - mMetaballPosVariation.y) * mTimerTime;
  //}
  //else
  //{
  //  mMetaballPosVariationFrom = mMetaballPosVariationTo;
  //  mMetaballPosVariationTo = Vector2(0,0);
  //}

  float fDuration = 2.000;
  if (mTimePosition < fDuration)
  {
    mMetaballPosVariation.x = (mMetaballPosVariationTo.x - mMetaballPosVariationFrom.x) * (1.0 / fDuration) * mTimePosition + mMetaballPosVariationFrom.x;
    mMetaballPosVariation.y = (mMetaballPosVariationTo.y - mMetaballPosVariationFrom.y) * (1.0 / fDuration) * mTimePosition + mMetaballPosVariationFrom.y;
  }
  else
  {
    mTimePosition = 0;
    mMetaballPosVariationFrom = mMetaballPosVariation;
    mMetaballPosVariationTo = Vector2(0,0);
  }

  Vector2 sinCosVariation(0,0);
  sinCosVariation.x = (-sin(mTime*0.5)+cos(mTime*0.5)) * 0.03;
  sinCosVariation.y = (sin(mTime*0.5)-cos(mTime*0.5)) * 0.03;

  Vector2 sinCosVariation2(0,0);
  sinCosVariation2.x = (-sin(mTime*0.5)-cos(mTime*0.5)) * 0.02;
  sinCosVariation2.y = (sin(mTime*0.5)+cos(mTime*0.5)) * 0.02;

  mMetaballs[0].radius += radiusInc;
  mMetaballs[1].radius += radiusInc;
  mMetaballs[2].radius += radiusInc + sin(mTime*0.5) * 0.0002f;
  mMetaballs[3].radius += radiusInc + cos(mTime*0.5) * 0.0002f;

  //We set all information for the shader
  mMetaballs[0].shader.SetUniform("radius", mMetaballs[0].radius);
  mMetaballs[0].shader.SetUniform("positionMetaball", mMetaballs[0].position);
  mMetaballs[0].shader.SetUniform("gravityVector", mGravityVar);

  mMetaballs[1].shader.SetUniform("radius", mMetaballs[1].radius);
  mMetaballs[1].shader.SetUniform("positionMetaball", mMetaballs[1].position + mMetaballPosVariation);
  mMetaballs[1].shader.SetUniform("gravityVector", mGravityVar);

  mMetaballs[2].shader.SetUniform("radius", mMetaballs[2].radius);
  mMetaballs[2].shader.SetUniform("positionMetaball", mMetaballs[2].position + sinCosVariation);
  mMetaballs[2].shader.SetUniform("gravityVector", mGravityVar);

  mMetaballs[3].shader.SetUniform("radius", mMetaballs[3].radius);
  mMetaballs[3].shader.SetUniform("positionMetaball", mMetaballs[3].position + sinCosVariation2);
  mMetaballs[3].shader.SetUniform("gravityVector", mGravityVar);

  //We update the two different variables for time
  mTime += mTimerTime;
  mTimePosition += mTimerTime;
  mTimeScale += mTimerTime;
}
*/
