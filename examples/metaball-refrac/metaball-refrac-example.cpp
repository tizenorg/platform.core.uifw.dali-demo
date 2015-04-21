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
}

#define METABALL_NUMBER 4

const char*const METABALL_FRAG_SHADER=
  "precision mediump float;\n"
  "uniform vec2 positionMetaball;\n"
  "uniform float radius;\n"
  "uniform float aspect;\n"
  "void main()\n"
  "{\n"
  "  vec2 adjustedCoords = vTexCoord * 2.0 - 1.0;\n"
  "  float distance = (adjustedCoords.x - positionMetaball.x) * (adjustedCoords.x - positionMetaball.x) + (adjustedCoords.y - positionMetaball.y) * (adjustedCoords.y - positionMetaball.y);\n"
  "  float color = radius / sqrt( distance );\n"
  "  vec2 bordercolor = vec2(0.0,0.0);\n"
  "  if (vTexCoord.x < 0.1)\n"
  "  {\n"
  "    bordercolor.x = (0.1 - vTexCoord.x) * 0.8;"
  "  }\n"
  "  if (vTexCoord.x > 0.9)\n"
  "  {\n"
  "    bordercolor.x = (vTexCoord.x - 0.9) * 0.8;"
  "  }\n"
  "  if (vTexCoord.y < 0.1)\n"
  "  {\n"
  "    bordercolor.y = (0.1 - vTexCoord.y) * 0.8;"
  "  }\n"
  "  if (vTexCoord.y > (0.9 * aspect))\n"
  "  {\n"
  "    bordercolor.y = (vTexCoord.y - (0.9 * aspect)) * 0.8;"
  "  }\n"
  "  float border = (bordercolor.x + bordercolor.y) * 0.5;\n"
  "  gl_FragColor = vec4(color + border,color + border,color + border,1.0);\n"
  "}\n";


const char*const REFRACTION_FRAG_SHADER=
  "precision mediump float;\n"
  "void main()\n"
  "{\n"
  "  vec4 metaColor = texture2D(sEffect, vTexCoord);\n"
  "  vec2 zoomCoords;\n"
  "  float bright = 1.0;\n"
  "  if (metaColor.r > 0.85)"
  "  {\n"
  "    zoomCoords = ((vTexCoord - 0.5) * 0.95) + 0.5;\n"
  "	   //zoomCoords.y = zoomCoords.y * -1.0;\n"
  "  }\n"
  "  else if (metaColor.r > 0.81)"
  "  {\n"
  "    float interpolation = mix(1.1, 1.0, (0.85 - metaColor.r) * 50.0);\n"
  "	   zoomCoords = ((vTexCoord - 0.5) * interpolation) + 0.5;\n"
  "    bright = 1.2;\n"
  "  }\n"
  "  else\n"
  "    zoomCoords = vTexCoord;\n"
  "  gl_FragColor = texture2D(sTexture, zoomCoords) * bright;\n"
  "}\n";

/*
const char*const FRAG_SHADER=
  "precision mediump float;\n"
  "void main()\n"
  "{\n"
  "  gl_FragColor = texture2D(sTexture, vTexCoord);\n"
  "}\n";
*/

const char*const FRAG_SHADER = DALI_COMPOSE_SHADER (
  precision mediump float;
  void main()
  {
    gl_FragColor = texture2D(sTexture, vTexCoord);
  }
);


struct MetaballInfo
{
	ShaderEffect shader;
	MeshActor meshActor;
	Vector2 position;
	float	radius;
	float	initRadius;
};


/***************************************************************************/
/*Demo using Metaballs for Refraction when clicking the screen *************/
/***************************************************************************/
class MetaballRefracController : public ConnectionTracker
{
public:
  MetaballRefracController( Application& application );
  ~MetaballRefracController();

  void Create( Application& app );
  bool OnTouch( Actor actor, const TouchEvent& touch );
  bool OnTimerTick();

  void SetGravity(const Vector2 & gravity);


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
  bool              mExitClick;
  Vector2           mCurrentTouchPosition;
  Vector2           mMetaballPosVariation;
  Vector2           mMetaballPosVariationFrom;
  Vector2           mMetaballPosVariationTo;
  Vector2           mMetaballCenter;

  Vector2           mGravity;
  Vector2           mGravityVar;

  float             mTime;
  float             mTimerTime;
  Timer             mTimerForMetaballs;
  float             mTimePosition;
  float             mTimeScale;

  //Animations
  Animation         mSeparationAnimation;

  //Private functions
  Mesh              CreateMesh();
  void              CreateMetaballActors();
  void              CreateMetaballImage();
  void              AddRefractionImage();

  void              UpdateMetaballsPosition();

  void              ResetMetaballsState();
};


//-----------------------------------------------------------------------------------------------
//
//	IMPLEMENTATION
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

  //Create a view
  mView = Dali::Toolkit::View::New();
  mView.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  stage.Add( mView );

  //Set background image for the view
  mBackImage = ResourceImage::New( BACKGROUND_IMAGE );

  //Create internal data
  CreateMetaballActors();
  CreateMetaballImage();
  AddRefractionImage();

  //Timer for metaball update
  mTimerForMetaballs = Timer::New( 16 );
  mTimerForMetaballs.TickSignal().Connect(this, &MetaballRefracController::OnTimerTick);

  mTimerTime = 1.f / 16.f;
  mTime = 0;

  mTimePosition = 0;
  mExitClick = false;

  mGravity = Vector2(GRAVITY_X,GRAVITY_Y);
  mGravityVar = Vector2(0,0);

  // Connect the callback to the touch signal on the mesh actor
  mView.TouchedSignal().Connect( this, &MetaballRefracController::OnTouch );
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
  mMetaballs[2].radius = mMetaballs[2].initRadius = 0.0125f;
  mMetaballs[3].radius = mMetaballs[3].initRadius = 0.0125f;

  //Initialization of each of the metaballs
  for (int i = 0 ; i < METABALL_NUMBER ; i++)
  {
    mMetaballs[i].position = Vector2(0.0f, 0.0f);

    mMetaballs[i].shader = ShaderEffect::New( "", METABALL_FRAG_SHADER, GEOMETRY_TYPE_TEXTURED_MESH  );
    mMetaballs[i].shader.SetUniform("positionMetaball", mMetaballs[i].position);
    mMetaballs[i].shader.SetUniform("radius", mMetaballs[i].radius);
    mMetaballs[i].shader.SetUniform("aspect", aspect);

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

  //	mView.Add( mMetaballRoot );

  //Initialization of variables related to metaballs
  mMetaballPosVariation = Vector2(0,0);
  mMetaballPosVariationFrom = Vector2(0,0);
  mMetaballPosVariationTo = Vector2(0,0);
  mCurrentTouchPosition = Vector2(0,0);
  mMetaballCenter = Vector2(0,0);
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

  mView.Add( mCompositionActor );
}

void MetaballRefracController::UpdateMetaballsPosition()
{
  float radiusInc;
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
  /*
  //We move one of the metaballs, using movement from the finger
  if (fabs(mMetaballPosVariationTo.x - mMetaballPosVariation.x) > 0.01 || fabs(mMetaballPosVariationTo.y - mMetaballPosVariation.y) > 0.01)
  {
    mMetaballPosVariation.x += (mMetaballPosVariationTo.x - mMetaballPosVariation.x) * mTimerTime;
    mMetaballPosVariation.y += (mMetaballPosVariationTo.y - mMetaballPosVariation.y) * mTimerTime;
  }
  else
  {
    mMetaballPosVariationFrom = mMetaballPosVariationTo;
    mMetaballPosVariationTo = Vector2(0,0);
  }
  */

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

  //We set the position for the metaballs based on click position
  for (int i = 0 ; i < METABALL_NUMBER ; i++)
  {
    mMetaballs[i].position = mMetaballCenter;
  }

  Vector2 sinCosVariation(0,0);
  sinCosVariation.x = (-sin(mTime*0.5)+cos(mTime*0.5)) * 0.03;
  sinCosVariation.y = (sin(mTime*0.5)-cos(mTime*0.5)) * 0.03;

  Vector2 sinCosVariation2(0,0);
  sinCosVariation.x = (-sin(mTime*0.5)-cos(mTime*0.5)) * 0.02;
  sinCosVariation.y = (sin(mTime*0.5)+cos(mTime*0.5)) * 0.02;

  mMetaballs[0].radius += radiusInc;
  mMetaballs[1].radius += radiusInc;
  mMetaballs[2].radius += radiusInc + sin(mTime*0.5) * 0.0002f;
  mMetaballs[3].radius += radiusInc + cos(mTime*0.5) * 0.0002f;

  //We set all information for the shader
  mMetaballs[0].shader.SetUniform("radius", mMetaballs[0].radius);
  mMetaballs[0].shader.SetUniform("positionMetaball", mMetaballs[0].position + mGravityVar);

  mMetaballs[1].shader.SetUniform("radius", mMetaballs[1].radius);
  mMetaballs[1].shader.SetUniform("positionMetaball", mMetaballs[1].position + mMetaballPosVariation + mGravityVar);

  mMetaballs[2].shader.SetUniform("radius", mMetaballs[2].radius);
  mMetaballs[2].shader.SetUniform("positionMetaball", mMetaballs[2].position + sinCosVariation + mGravityVar);

  mMetaballs[3].shader.SetUniform("radius", mMetaballs[3].radius);
  mMetaballs[3].shader.SetUniform("positionMetaball", mMetaballs[3].position + sinCosVariation2 + mGravityVar);

  //We update the two different variables for time
  mTime += mTimerTime;
  mTimePosition += mTimerTime;
  mTimeScale += mTimerTime;
}

bool MetaballRefracController::OnTimerTick()
{
  UpdateMetaballsPosition();

  return true;
}

void MetaballRefracController::ResetMetaballsState()
{
  mCompositionActor.SetShaderEffect(mNormalShader);
  mTimerForMetaballs.Stop();

  for (int i = 0 ; i < METABALL_NUMBER ; i++)
  {
    mMetaballs[i].radius = mMetaballs[i].initRadius;
  }

  mMetaballPosVariationTo = Vector2(0,0);
  mMetaballPosVariationFrom = Vector2(0,0);
  mMetaballPosVariation = Vector2(0,0);

  mGravityVar = Vector2(0,0);
}

bool MetaballRefracController::OnTouch( Actor actor, const TouchEvent& touch )
{
  const TouchPoint &point = touch.GetPoint(0);
  float aspectR = mScreenSize.y / mScreenSize.x;
  switch(point.state)
  {
    case TouchPoint::Down:
    {
	  //If there are stil metaballs in the screen
	  if (mExitClick)
	  {
	    mExitClick = false;
	    ResetMetaballsState();
	  }

	  //We draw with the refraction-composition shader
	  mCompositionActor.SetShaderEffect(mRefractionShader);

	  mCurrentTouchPosition = point.screen;

	  //we use the click position for the metaballs
	  mMetaballCenter = Vector2((point.screen.x / mScreenSize.x) - 0.5, (aspectR * (mScreenSize.y - point.screen.y) / mScreenSize.y) - 0.5) * 2.0;

	  //we start the animation thread
	  mTimerForMetaballs.Start();
	  mTimeScale = 0;
	  break;
    }
    case TouchPoint::Motion:
    {
	  Vector2 displacement = point.screen - mCurrentTouchPosition;
	  mCurrentTouchPosition = point.screen;

	  Vector2 direction(0,0);
	  if (fabs(displacement.x) > 0)
	  {
	    direction.x = displacement.x / fabs(displacement.x);
	  }
	  if (fabs(displacement.y) > 0)
	  {
	    direction.y = displacement.y / fabs(displacement.y);
	  }

	  //we use the actual displacement vector to move the metaballs
	  mTimePosition *= 0.6;
	  mMetaballPosVariationFrom = mMetaballPosVariation;
	  mMetaballPosVariationTo.x += (displacement.x / mScreenSize.x) * 2.2;
	  mMetaballPosVariationTo.y += (- displacement.y / mScreenSize.y) * 2.2;

	  //we use the click position for the metaballs
	  mMetaballCenter = Vector2((point.screen.x / mScreenSize.x) - 0.5, (aspectR * (mScreenSize.y - point.screen.y) / mScreenSize.y) - 0.5) * 2.0;
	  break;
    }
    case TouchPoint::Up:
    case TouchPoint::Leave:
    case TouchPoint::Interrupted:
    {
	  //we are releasing the click
	  mExitClick = true;

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
