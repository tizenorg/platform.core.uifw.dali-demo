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
}

#define METABALL_NUMBER 4

const char * METABALL_FRAG_SHADER = DALI_COMPOSE_SHADER (
  precision mediump float;\n
  uniform vec2 uPositionMetaball;\n
  uniform vec2 uPositionVar;\n
  uniform vec2 uGravityVector;\n
  uniform float uRadius;\n
  uniform float uRadiusVar;\n
  uniform float uAspect;\n
  void main()\n
  {\n
    vec2 adjustedCoords = vTexCoord * 2.0 - 1.0;\n
    vec2 finalMetaballPosition = uPositionMetaball + uGravityVector + uPositionVar;\n
    \n
    vec2 distanceVec = adjustedCoords - finalMetaballPosition;
    float distance = length(distanceVec);
    float finalRadius = uRadius + uRadiusVar;\n
    float color = finalRadius / distance;\n
    \n
    vec2 bordercolor = vec2(0.0,0.0);\n
    \n
    if (vTexCoord.x < 0.1)\n
    {\n
      bordercolor.x = (0.1 - vTexCoord.x) * 0.8;\n
    }\n
    if (vTexCoord.x > 0.9)\n
    {\n
      bordercolor.x = (vTexCoord.x - 0.9) * 0.8;\n
    }\n
    if (vTexCoord.y < 0.1)\n
    {\n
      bordercolor.y = (0.1 - vTexCoord.y) * 0.8;\n
    }\n
    if (vTexCoord.y > (0.9 * uAspect))\n
    {\n
      bordercolor.y = (vTexCoord.y - (0.9 * uAspect)) * 0.8;\n
    }\n
    float border = (bordercolor.x + bordercolor.y) * 0.5;\n
    gl_FragColor = vec4(color + border,color + border,color + border,1.0);\n
  }\n
);

const char * REFRACTION_FRAG_SHADER = DALI_COMPOSE_SHADER (
  precision mediump float;\n
  void main()\n
  {\n
    vec4 metaColor = texture2D(sEffect, vTexCoord);\n
    vec2 zoomCoords;\n
    float bright = 1.0;\n
    if (metaColor.r > 0.85)\n
    {\n
      zoomCoords = ((vTexCoord - 0.5) * 0.95) + 0.5;\n
    }\n
    else if (metaColor.r > 0.81)\n
    {\n
      float interpolation = mix(0.95, 1.05, (0.85 - metaColor.r) * 50.0);\n
      zoomCoords = ((vTexCoord - 0.5) * interpolation) + 0.5;\n
      bright = 1.2;\n
    }\n
    else\n
    {\n
      zoomCoords = vTexCoord;\n
    }\n
    \n
    gl_FragColor = texture2D(sTexture, zoomCoords) * bright;\n
  }\n
 );

const char* FRAG_SHADER = DALI_COMPOSE_SHADER (
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
};


/***************************************************************************/
/*Demo using Metaballs for Refraction when clicking the screen *************/
/***************************************************************************/
class MetaballRefracController : public ConnectionTracker
{
public:
  MetaballRefracController( Application& application );
  ~MetaballRefracController();

  /**
   * Main create function, it creates the metaballs and all the
   */
  void Create( Application& app );

  /**
   * Touch event
   */
  bool OnTouch( Actor actor, const TouchEvent& touch );

  /**
   * Function to set gravity property
   */
  void SetGravity(const Vector2 & gravity);


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
  bool              mExitClick;
  Vector2           mCurrentTouchPosition;
  Vector2           mMetaballPosVariation;
  Vector2           mMetaballPosVariationFrom;
  Vector2           mMetaballPosVariationTo;
  Vector2           mMetaballCenter;

  Vector2           mGravity;
  Vector2           mGravityVar;

  //Animations
  Animation         mGravityAnimation[METABALL_NUMBER];
  Animation         mRadiusDecAnimation[METABALL_NUMBER];
  Animation         mRadiusIncFastAnimation[METABALL_NUMBER];
  Animation         mRadiusIncSlowAnimation[METABALL_NUMBER];
  Animation         mRadiusVarAnimation[METABALL_NUMBER];
  Animation         mPositionVarAnimation[METABALL_NUMBER];

  //Private functions

  /**
   * Create a mesh data with the geometry for the metaball rendering
   */
  Mesh              CreateMesh();

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
   * Creation of all the metaballs animations (gravity, movement, size, etc.)
   */
  void              CreateAnimations();

  /**
   * Function to launch the gro slow radius for the metaballs, and also the small variations for metaball[2] and [3]
   */
  void              LaunchRadiusIncSlowAnimations(Animation &source);

  /**
   * Function to launch the animation to get the metaball[1] back to the center
   */
  void              LaunchGetBackToPositionAnimation(Animation &source);

  /**
   * Function to stop all animations related to the click of the user in the screen
   */
  void              StopClickAnimations();

  /**
   * Function to stop all animations related to the after click of the user in the screen
   */
  void              StopAfterClickAnimations();

  /**
   * Function to reset state of all metaballs
   */
  void              ResetMetaballsState();

  /**
   * Function to set the actual position of the metaballs when the user clicks the screen
   */
  void              SetPositionToMetaballs(Vector2 & metaballCenter);
};


//-----------------------------------------------------------------------------------------------
//
//  IMPLEMENTATION
//
//----------------

MetaballRefracController::MetaballRefracController( Application& application )
  : mApplication( application )
{
  // Connect to the Application's Init signal
  mApplication.InitSignal().Connect( this, &MetaballRefracController::Create );
}

MetaballRefracController::~MetaballRefracController()
{
  // Nothing to do here;
}

void MetaballRefracController::SetGravity(const Vector2 & gravity)
{
  mGravity = gravity;
}

void MetaballRefracController::Create( Application& app )
{
  DemoHelper::RequestThemeChange();

  Stage stage = Stage::GetCurrent();
  mScreenSize = stage.GetSize();

  stage.SetBackgroundColor(Color::BLACK);

  //Set background image for the view
  mBackImage = ResourceImage::New( BACKGROUND_IMAGE );

  mGravity = Vector2(GRAVITY_X,GRAVITY_Y);
  mGravityVar = Vector2(0,0);

  //Create internal data
  CreateMetaballActors();
  CreateMetaballImage();
  AddRefractionImage();

  CreateAnimations();

  // Connect the callback to the touch signal on the mesh actor
  stage.GetRootLayer().TouchedSignal().Connect( this, &MetaballRefracController::OnTouch );
}

Mesh MetaballRefracController::CreateMesh()
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
  baseMaterial.SetDiffuseColor( Vector4(1.0f,1.0f,1.0f,1.0f));
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

void MetaballRefracController::CreateMetaballActors()
{
  //We create metaball structures
  //With MeshData Textured
  float aspect = (float)mScreenSize.y / (float)mScreenSize.x;

  //Each metaball has a different radius
  mMetaballs[0].radius = mMetaballs[0].initRadius = 0.0145f;
  mMetaballs[1].radius = mMetaballs[1].initRadius = 0.012f;
  mMetaballs[2].radius = mMetaballs[2].initRadius = 0.0135f;
  mMetaballs[3].radius = mMetaballs[3].initRadius = 0.0135f;

  //Initialization of each of the metaballs
  for (int i = 0 ; i < METABALL_NUMBER ; i++)
  {
    mMetaballs[i].position = Vector2(0.0f, 0.0f);

    mMetaballs[i].shader = ShaderEffect::New( "", METABALL_FRAG_SHADER, GEOMETRY_TYPE_TEXTURED_MESH  );
    mMetaballs[i].shader.SetUniform("uPositionMetaball", mMetaballs[i].position);
    mMetaballs[i].shader.SetUniform("uPositionVar", Vector2(0,0));
    mMetaballs[i].shader.SetUniform("uGravityVector", Vector2(0,0));
    mMetaballs[i].shader.SetUniform("uRadius", mMetaballs[i].radius);
    mMetaballs[i].shader.SetUniform("uRadiusVar", 0.f);
    mMetaballs[i].shader.SetUniform("uAspect", aspect);

    mMetaballs[i].meshActor = MeshActor::New( CreateMesh() );
    mMetaballs[i].meshActor.SetScale( 1.0f );
    mMetaballs[i].meshActor.SetParentOrigin( ParentOrigin::CENTER );
    mMetaballs[i].meshActor.SetShaderEffect(mMetaballs[i].shader);
    mMetaballs[i].meshActor.SetBlendMode(BlendingMode::ON );
    mMetaballs[i].meshActor.SetBlendFunc(BlendingFactor::ONE, BlendingFactor::ONE,
                                         BlendingFactor::ONE, BlendingFactor::ONE);
  }
  //Each actor has a different name
  mMetaballs[0].meshActor.SetName("Metaball0");
  mMetaballs[1].meshActor.SetName("Metaball1");
  mMetaballs[2].meshActor.SetName("Metaball2");
  mMetaballs[3].meshActor.SetName("Metaball3");

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

void MetaballRefracController::CreateMetaballImage()
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

void MetaballRefracController::AddRefractionImage()
{
  //Creation of the composition image
  mRefractionShader = ShaderEffect::New( "", REFRACTION_FRAG_SHADER );
  mRefractionShader.SetEffectImage( mMetaballFBO );

  mNormalShader = ShaderEffect::New( "", FRAG_SHADER );

  mCompositionActor = ImageActor::New( mBackImage );
  mCompositionActor.SetParentOrigin(ParentOrigin::CENTER);
  mCompositionActor.SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  mCompositionActor.SetSize(mScreenSize.x, mScreenSize.y);
  mCompositionActor.SetShaderEffect(mNormalShader);

  Stage stage = Stage::GetCurrent();
  stage.Add( mCompositionActor );
}

void MetaballRefracController::CreateAnimations()
{
  int i = 0;
  float key;

  mPositionVarAnimation[1] = Animation::New(2.f);
  mPositionVarAnimation[1].SetLooping( false );
  mPositionVarAnimation[1].Pause();
  mPositionVarAnimation[1].FinishedSignal().Connect( this, &MetaballRefracController::LaunchGetBackToPositionAnimation );

  KeyFrames keySinCosVariation = KeyFrames::New();
  Vector2 sinCosVariation(0,0);
  for ( i = 0 ; i < 360 ; i++)
  {
    sinCosVariation.x = 0.05f * (-sin(i * Math::PI/180.f) + cos(i * Math::PI/180.f));
    sinCosVariation.y = 0.05f * (sin(i * Math::PI/180.f) - cos(i * Math::PI/180.f));
    key = i/360.f;
    keySinCosVariation.Add(key, sinCosVariation);
  }

  mPositionVarAnimation[2] = Animation::New(6.f);
  mPositionVarAnimation[2].AnimateBetween(Property( mMetaballs[2].shader, "uPositionVar" ), keySinCosVariation);
  mPositionVarAnimation[2].SetLooping( true );
  mPositionVarAnimation[2].Pause();

  KeyFrames keyCosSinVariation = KeyFrames::New();
  Vector2 cosSinVariation(0,0);
  for ( i = 0 ; i < 360 ; i++)
  {
    cosSinVariation.x = 0.05f * (-sin(i * Math::PI/180.f) - cos(i * Math::PI/180.f));
    cosSinVariation.y = 0.05f * (sin(i * Math::PI/180.f) + cos(i * Math::PI/180.f));
    key = i/360.f;
    keyCosSinVariation.Add(key, cosSinVariation);
  }

  mPositionVarAnimation[3] = Animation::New(6.f);
  mPositionVarAnimation[3].AnimateBetween(Property( mMetaballs[3].shader, "uPositionVar" ), keyCosSinVariation);
  mPositionVarAnimation[3].SetLooping( true );
  mPositionVarAnimation[3].Pause();

  //Animations for gravity
  for ( i = 0 ; i < METABALL_NUMBER ; i++)
  {
    mGravityAnimation[i] = Animation::New(25.f);
    mGravityAnimation[i].AnimateBy( Property( mMetaballs[i].shader, "uGravityVector" ), mGravity * 25.f * 3.f);
    mGravityAnimation[i].SetLooping( false );
    mGravityAnimation[i].Pause();
  }

  //Animation to decrease size of metaballs when there is no click
  for ( i = 0 ; i < METABALL_NUMBER ; i++)
  {
    mRadiusDecAnimation[i] = Animation::New(25.f);
    mRadiusDecAnimation[i].AnimateBy( Property( mMetaballs[i].shader, "uRadius" ), -0.004f * 25.f * 3.f);
    mRadiusDecAnimation[i].SetLooping( false );
    mRadiusDecAnimation[i].Pause();
  }

  //Animation to grow the size of the metaballs the first second of the click
  for ( i = 0 ; i < METABALL_NUMBER ; i++)
  {
    mRadiusIncFastAnimation[i] = Animation::New(0.3f);
    mRadiusIncFastAnimation[i].AnimateBy( Property( mMetaballs[i].shader, "uRadius" ), 0.06f);
    mRadiusIncFastAnimation[i].SetLooping( false );
    mRadiusIncFastAnimation[i].Pause();
  }
  mRadiusIncFastAnimation[0].FinishedSignal().Connect( this, &MetaballRefracController::LaunchRadiusIncSlowAnimations );

  //Animation to grow the size of the metaballs afterwards
  for ( i = 0 ; i < METABALL_NUMBER ; i++)
  {
    mRadiusIncSlowAnimation[i] = Animation::New(20.f);
    mRadiusIncSlowAnimation[i].AnimateBy( Property( mMetaballs[i].shader, "uRadius" ), 0.04f);
    mRadiusIncSlowAnimation[i].SetLooping( false );
    mRadiusIncSlowAnimation[i].Pause();
  }

  //keyframes of a sin function
  KeyFrames keySin = KeyFrames::New();
  float val;
  for ( i = 0 ; i < 360 ; i++)
  {
    val = 0.01f * sin(i * Math::PI/180.f);
    key = i/360.f;
    keySin.Add(key, val);
  }

  //Animation to change the size of the metaball
  mRadiusVarAnimation[2] = Animation::New(8.f);
  mRadiusVarAnimation[2].AnimateBetween(Property( mMetaballs[2].shader, "uRadiusVar" ), keySin);
  mRadiusVarAnimation[2].SetLooping( true );

  //keyframes of a cos function
  KeyFrames keyCos = KeyFrames::New();
  for ( i = 0 ; i < 360 ; i++)
  {
    val = 0.01f * cos(i * Math::PI/180.f);
    key = i/360.f;
    keyCos.Add(key, val);
  }

  //Animation to change the size of the metaball
  mRadiusVarAnimation[3] = Animation::New(8.f);
  mRadiusVarAnimation[3].AnimateBetween(Property( mMetaballs[3].shader, "uRadiusVar" ), keyCos);
  mRadiusVarAnimation[3].SetLooping( true );
}

void MetaballRefracController::LaunchGetBackToPositionAnimation(Animation &source)
{
  mMetaballPosVariationTo = Vector2(0,0);

  mPositionVarAnimation[1] = Animation::New(1.f);
  mPositionVarAnimation[1].SetLooping( false );
  mPositionVarAnimation[1].AnimateTo(Property( mMetaballs[1].shader, "uPositionVar" ), Vector2(0,0));
  mPositionVarAnimation[1].Play();
}

void MetaballRefracController::LaunchRadiusIncSlowAnimations(Animation &source)
{
  for (int i = 0 ; i < METABALL_NUMBER ; i++)
  {
    mRadiusIncSlowAnimation[i].Play();
  }
  mPositionVarAnimation[2].Play();
  mPositionVarAnimation[3].Play();
}

void MetaballRefracController::StopClickAnimations()
{
  for (int i = 0 ; i < METABALL_NUMBER ; i++)
  {
    mRadiusIncSlowAnimation[i].Stop();
    mRadiusIncFastAnimation[i].Stop();
  }
  mPositionVarAnimation[1].Stop();
  mPositionVarAnimation[2].Stop();
  mPositionVarAnimation[3].Stop();
}

void MetaballRefracController::StopAfterClickAnimations()
{
  for (int i = 0 ; i < METABALL_NUMBER ; i++)
  {
    mGravityAnimation[i].Stop();
    mRadiusDecAnimation[i].Stop();

    mMetaballs[i].radius = mMetaballs[i].initRadius;

    mMetaballs[i].shader.SetUniform("uGravityVector", Vector2(0,0));
    mMetaballs[i].shader.SetUniform("uRadius", mMetaballs[i].radius);
    mMetaballs[i].shader.SetUniform("uRadiusVar", 0);
  }
  mRadiusVarAnimation[2].Stop();
  mRadiusVarAnimation[3].Stop();
}

void MetaballRefracController::ResetMetaballsState()
{
  mCompositionActor.SetShaderEffect(mNormalShader);

  for (int i = 0 ; i < METABALL_NUMBER ; i++)
  {
    mMetaballs[i].radius = mMetaballs[i].initRadius;
  }

  mMetaballPosVariationTo = Vector2(0,0);
  mMetaballPosVariationFrom = Vector2(0,0);
  mMetaballPosVariation = Vector2(0,0);

  mGravityVar = Vector2(0,0);
}

void MetaballRefracController::SetPositionToMetaballs(Vector2 & metaballCenter)
{
  //We set the position for the metaballs based on click position
  for (int i = 0 ; i < METABALL_NUMBER ; i++)
  {
    mMetaballs[i].position = metaballCenter;
    mMetaballs[i].shader.SetUniform("uPositionMetaball", mMetaballs[0].position);
  }
}

bool MetaballRefracController::OnTouch( Actor actor, const TouchEvent& touch )
{
  const TouchPoint &point = touch.GetPoint(0);
  float aspectR = mScreenSize.y / mScreenSize.x;
  switch(point.state)
  {
    case TouchPoint::Down:
    {
      StopAfterClickAnimations();
      for (int i = 0 ; i < METABALL_NUMBER ; i++)
        mRadiusIncFastAnimation[i].Play();
      mRadiusVarAnimation[2].Play();
      mRadiusVarAnimation[3].Play();

      //We draw with the refraction-composition shader
      mCompositionActor.SetShaderEffect(mRefractionShader);

      mCurrentTouchPosition = point.screen;

      //we use the click position for the metaballs
      Vector2 metaballCenter = Vector2((point.screen.x / mScreenSize.x) - 0.5, (aspectR * (mScreenSize.y - point.screen.y) / mScreenSize.y) - 0.5) * 2.0;
      SetPositionToMetaballs(metaballCenter);
      break;
    }
    case TouchPoint::Motion:
    {
      Vector2 displacement = point.screen - mCurrentTouchPosition;
      mCurrentTouchPosition = point.screen;

      mMetaballPosVariationTo.x += (displacement.x / mScreenSize.x) * 2.2;
      mMetaballPosVariationTo.y += (- displacement.y / mScreenSize.y) * 2.2;

      if (mPositionVarAnimation[1])
      {
        mPositionVarAnimation[1].FinishedSignal().Disconnect( this, &MetaballRefracController::LaunchGetBackToPositionAnimation );
        mPositionVarAnimation[1].Stop();
      }
      mPositionVarAnimation[1] = Animation::New(1.f);
      mPositionVarAnimation[1].SetLooping( false );
      mPositionVarAnimation[1].AnimateTo(Property( mMetaballs[1].shader, "uPositionVar" ), mMetaballPosVariationTo);
      mPositionVarAnimation[1].FinishedSignal().Connect( this, &MetaballRefracController::LaunchGetBackToPositionAnimation );
      mPositionVarAnimation[1].Play();

      //we use the click position for the metaballs
      Vector2 metaballCenter = Vector2((point.screen.x / mScreenSize.x) - 0.5, (aspectR * (mScreenSize.y - point.screen.y) / mScreenSize.y) - 0.5) * 2.0;
      SetPositionToMetaballs(metaballCenter);
      break;
    }
    case TouchPoint::Up:
    case TouchPoint::Leave:
    case TouchPoint::Interrupted:
    {
      //Stop click animations
      StopClickAnimations();

      //Launch out of screen animations
      for (int i = 0 ; i < METABALL_NUMBER ; i++)
        mGravityAnimation[i].Play();

      for (int i = 0 ; i < METABALL_NUMBER ; i++)
        mRadiusDecAnimation[i].Play();

      break;
    }
    default:
      break;
    }
  return true;
}

//
//
//-----------------------------------------------------------------------------------------------

void RunTest( Application& application )
{
  MetaballRefracController test( application );

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
