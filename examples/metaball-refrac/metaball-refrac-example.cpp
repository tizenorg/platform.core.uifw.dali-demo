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

#include "stdio.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char * const BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-2.jpg" );
const char * const TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
}

#define METABALL_NUMBER	3

const char*const METABALL_FRAG_SHADER=
	"precision mediump float;\n"
	"uniform vec2 posMeta;\n"
	"uniform float radius;\n"
	"void main()\n"
	"{\n"
	"	vec2 adjustedCoords = vTexCoord * 2.0 - 1.0;\n"
	"	float distance = (adjustedCoords.x - posMeta.x) * (adjustedCoords.x - posMeta.x) + (adjustedCoords.y - posMeta.y) * (adjustedCoords.y - posMeta.y);\n"
	"	float color = radius / sqrt( distance );\n"
	"	gl_FragColor = vec4(color,color,color,1.0);\n"	
	"}\n";


const char*const REFRACTION_FRAG_SHADER=
	"precision mediump float;\n"
	"void main()\n"
	"{\n"
	"	vec4 metaColor = texture2D(sEffect, vTexCoord);\n"
	"	vec2 zoomCoords;\n"
	"	float bright = 1.0;\n"
	"	if (metaColor.r > 0.85)"
	"	{\n"
	"		zoomCoords = ((vTexCoord - 0.5) * 0.95) + 0.5;\n"
	"		//zoomCoords.y = zoomCoords.y * -1.0;\n"		
	"	}\n"
	"	else if (metaColor.r > 0.81)"
	"	{\n"
	"		float interpolation = mix(1.1, 1.0, (0.85 - metaColor.r) * 50.0);\n"
	"		zoomCoords = ((vTexCoord - 0.5) * interpolation) + 0.5;\n"
	"		bright = 1.1;\n"
	"	}\n"
	"	else\n"
	"		zoomCoords = vTexCoord;\n"
	"	gl_FragColor = texture2D(sTexture, zoomCoords) * bright;\n"
	"}\n";


const char*const FRAG_SHADER=
	"precision mediump float;\n"
	"void main()\n"
	"{\n"
	"	gl_FragColor = texture2D(sTexture, vTexCoord);\n"
	"}\n";


typedef struct _MetaballInfo
{
	ShaderEffect shader;
	MeshActor meshActor;
	Vector2 position;
	float	radius;
	float	initRadius;
} t_MetaballInfo;


//
class MetaballRefracController : public ConnectionTracker
{
public:
 	MetaballRefracController( Application& application );
	~MetaballRefracController();

	void Create( Application& app );
	bool OnTouch( Actor actor, const TouchEvent& touch );
	bool OnTimerTick();


private:
	Application&  		mApplication;
	Vector2 			mScreenSize;

	Toolkit::View 		mView;                              ///< The View instance.
  	Toolkit::ToolBar    mToolBar;                           ///< The View's Toolbar.

	Layer         		mContentLayer;

	Image				mBackImage;
	FrameBufferImage	mMetaballFBO;

	Actor 				mMetaballRoot;
	t_MetaballInfo		mMetaballs[METABALL_NUMBER];

	Dali::ImageActor 	mCompositionActor;

	ShaderEffect 		mRefractionShader;
	ShaderEffect 		mNormalShader;

	//Motion
	bool				mBeginMotion;
	Vector2				mCurrentTouchPosition;
	Vector2				mMetaballPosVariation;
	Vector2				mMetaballPosVariationFrom;
	Vector2				mMetaballPosVariationTo;
	Vector2				mMetaballCenter;

	Timer               mTimerForMetaballs;
	float				mTime;

	//Animations
	Animation         	mSeparationAnimation;

	//Private functions
	Mesh				CreateMesh();
	void				CreateMetaballActors();
	void				CreateMetaballImage();
	void				AddRefractionImg();

	void				UpdateMetaballsPosition();
	bool				onTheEdgeOfTheScreen();
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
    ImageAttributes attributes;
    mBackImage = ResourceImage::New( BACKGROUND_IMAGE, attributes );

	//Create internal data
	CreateMetaballActors();
	CreateMetaballImage();
	AddRefractionImg();

	//Timer for metaball update
    mTimerForMetaballs = Timer::New( 20 );
	mTimerForMetaballs.TickSignal().Connect(this, &MetaballRefracController::OnTimerTick);

	mTime = 0;
	mBeginMotion = false;

    // Connect the callback to the touch signal on the mesh actor
    mView.TouchedSignal().Connect( this, &MetaballRefracController::OnTouch );
}

Mesh MetaballRefracController::CreateMesh()
{
	float aspect = (float)mScreenSize.y / (float)mScreenSize.x;

	// Create vertices and specify their color
	float xsize = mScreenSize.x * 0.5; 
	printf("x %f y %f\n",xsize, xsize * aspect);
	MeshData::VertexContainer vertices(4);
	vertices[ 0 ] = MeshData::Vertex( Vector3( -xsize, -xsize * aspect, 0.0f ), Vector2(0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f) );
	vertices[ 1 ] = MeshData::Vertex( Vector3(  xsize, -xsize * aspect, 0.0f ), Vector2(1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f) );
	vertices[ 2 ] = MeshData::Vertex( Vector3( -xsize,  xsize * aspect, 0.0f ), Vector2(0.0f, 1.0f * aspect), Vector3(0.0f, 0.0f, 1.0f) );
	vertices[ 3 ] = MeshData::Vertex( Vector3(  xsize,  xsize * aspect, 0.0f ), Vector2(1.0f, 1.0f * aspect), Vector3(0.0f, 0.0f, 1.0f) );

	// Specify all the faces
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
	//With MeshData Textured

	//First metaball
	mMetaballs[0].position = Vector2(0.3f, 0.1f);
	mMetaballs[0].radius = mMetaballs[0].initRadius = 0.145f;

	mMetaballs[0].shader = ShaderEffect::New( "", METABALL_FRAG_SHADER, GEOMETRY_TYPE_TEXTURED_MESH  );
	mMetaballs[0].shader.SetUniform("posMeta", mMetaballs[0].position); 
	mMetaballs[0].shader.SetUniform("radius", mMetaballs[0].radius); 

	mMetaballs[0].meshActor = MeshActor::New( CreateMesh() );
	mMetaballs[0].meshActor.SetScale( 1.0f );
	mMetaballs[0].meshActor.SetParentOrigin( ParentOrigin::CENTER );
	mMetaballs[0].meshActor.SetName("Metaball");
	mMetaballs[0].meshActor.SetShaderEffect(mMetaballs[0].shader);
	mMetaballs[0].meshActor.SetBlendMode(BlendingMode::ON );
    mMetaballs[0].meshActor.SetBlendFunc(BlendingFactor::ONE, BlendingFactor::ONE,
                                 BlendingFactor::ONE, BlendingFactor::ONE);

	//Second metaball
	mMetaballs[1].position = Vector2(-0.2f, 0.1f);
	mMetaballs[1].radius = mMetaballs[1].initRadius = 0.12f;

	mMetaballs[1].shader = ShaderEffect::New( "", METABALL_FRAG_SHADER, GEOMETRY_TYPE_TEXTURED_MESH  );
	mMetaballs[1].shader.SetUniform("posMeta", mMetaballs[1].position); 
	mMetaballs[1].shader.SetUniform("radius", mMetaballs[1].radius); 

	mMetaballs[1].meshActor = MeshActor::New( CreateMesh() );
	mMetaballs[1].meshActor.SetScale( 1.0f );
	mMetaballs[1].meshActor.SetParentOrigin( ParentOrigin::CENTER );
	mMetaballs[1].meshActor.SetName("Metaball2");
	mMetaballs[1].meshActor.SetShaderEffect(mMetaballs[1].shader);
	mMetaballs[1].meshActor.SetBlendMode(BlendingMode::ON );
    mMetaballs[1].meshActor.SetBlendFunc(BlendingFactor::ONE, BlendingFactor::ONE,
                                 BlendingFactor::ONE, BlendingFactor::ONE);

	//Third metaball
	mMetaballs[2].position = Vector2(0.1f, -0.2f);
	mMetaballs[2].radius = mMetaballs[2].initRadius = 0.125f;

	mMetaballs[2].shader = ShaderEffect::New( "", METABALL_FRAG_SHADER, GEOMETRY_TYPE_TEXTURED_MESH  );
	mMetaballs[2].shader.SetUniform("posMeta", mMetaballs[2].position); 
	mMetaballs[2].shader.SetUniform("radius", mMetaballs[2].radius); 

	mMetaballs[2].meshActor = MeshActor::New( CreateMesh() );
	mMetaballs[2].meshActor.SetScale( 1.0f );
	mMetaballs[2].meshActor.SetParentOrigin( ParentOrigin::CENTER );
	mMetaballs[2].meshActor.SetName("Metaball3");
	mMetaballs[2].meshActor.SetShaderEffect(mMetaballs[2].shader);
	mMetaballs[2].meshActor.SetBlendMode(BlendingMode::ON );
    mMetaballs[2].meshActor.SetBlendFunc(BlendingFactor::ONE, BlendingFactor::ONE,
                                 BlendingFactor::ONE, BlendingFactor::ONE);

	mMetaballRoot = Actor::New();
	mMetaballRoot.SetParentOrigin( ParentOrigin::CENTER );
	for (int i = 0 ; i < METABALL_NUMBER ; i++)
	{
		mMetaballRoot.Add( mMetaballs[i].meshActor );
	}

//	mView.Add( mMetaballRoot );

	mMetaballPosVariation = Vector2(0,0);
	mMetaballPosVariationFrom = Vector2(0,0);
	mMetaballPosVariationTo = Vector2(0,0);
	mCurrentTouchPosition = Vector2(0,0);
	mMetaballCenter = Vector2(0,0);
}

void MetaballRefracController::CreateMetaballImage()
{
	Stage stage = Stage::GetCurrent();
	mMetaballFBO = FrameBufferImage::New(mScreenSize.x, mScreenSize.y );

	stage.Add(mMetaballRoot);

	RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
	RenderTask task = taskList.CreateTask();
	task.SetRefreshRate( RenderTask::REFRESH_ALWAYS );
	task.SetSourceActor( mMetaballRoot );
	task.SetExclusive(true);
	//task.SetCameraActor(cameraActor);
	task.SetClearColor( Color::BLACK );
	task.SetClearEnabled( true );
	task.SetTargetFrameBuffer( mMetaballFBO );
}

void MetaballRefracController::AddRefractionImg()
{ 
	//With ImgActor
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

bool MetaballRefracController::onTheEdgeOfTheScreen()
{
	if ((mCurrentTouchPosition.x / mScreenSize.x) < 0.15)
		return true;
	if ((mCurrentTouchPosition.x / mScreenSize.x) < 0.15)
		return true;
	if ((mCurrentTouchPosition.y / mScreenSize.y) > 0.85)
		return true;
	if ((mCurrentTouchPosition.y / mScreenSize.y) > 0.85)
		return true;

	return false;
}

void MetaballRefracController::UpdateMetaballsPosition()
{
	mMetaballs[0].radius += 0.001f * 1/20.0f;
	mMetaballs[1].radius += 0.001f * 1/20.0f;
	mMetaballs[2].radius += 0.001f * 1/20.0f;

	mMetaballs[0].position = mMetaballCenter;
	mMetaballs[1].position = mMetaballCenter;
	mMetaballs[2].position = mMetaballCenter;

	if (fabs(mMetaballPosVariationTo.x - mMetaballPosVariation.x) > 0.01 || fabs(mMetaballPosVariationTo.y - mMetaballPosVariation.y) > 0.01)
	{
		mMetaballPosVariation.x += (mMetaballPosVariationTo.x - mMetaballPosVariation.x) * 1/20.0f;
		mMetaballPosVariation.y += (mMetaballPosVariationTo.y - mMetaballPosVariation.y) * 1/20.0f;
	}
	else
	{
		mMetaballPosVariationFrom = mMetaballPosVariationTo;
		mMetaballPosVariationTo = Vector2(0,0);
	}

	/*printf("PoxX %f PosY %f mTime %f\n", mMetaballPosVariation.x, mMetaballPosVariation.y, mTime);
	if (mTime < 2.00001)
	{
		mMetaballPosVariation.x = (mMetaballPosVariationTo.x - mMetaballPosVariationFrom.x) * mTime * 0.5;
		mMetaballPosVariation.y = (mMetaballPosVariationTo.y - mMetaballPosVariationFrom.y) * mTime * 0.5;
	}
	else
	{
		printf("Reset timer\n");
		mTime = 0;
		mMetaballPosVariationFrom = mMetaballPosVariation;
		mMetaballPosVariationTo = Vector2(0,0);
	}*/

	mMetaballs[0].shader.SetUniform("radius", mMetaballs[0].radius);
	mMetaballs[0].shader.SetUniform("posMeta", mMetaballs[0].position); 

	mMetaballs[1].shader.SetUniform("radius", mMetaballs[1].radius);	
	mMetaballs[1].shader.SetUniform("posMeta", mMetaballs[1].position + mMetaballPosVariation); 

	mMetaballs[2].shader.SetUniform("radius", mMetaballs[2].radius);
	mMetaballs[2].shader.SetUniform("posMeta", mMetaballs[2].position); 

	mTime += 1/20.0f;
}

bool MetaballRefracController::OnTimerTick()
{
	UpdateMetaballsPosition();

	return true;
}

bool MetaballRefracController::OnTouch( Actor actor, const TouchEvent& touch )
{
	const TouchPoint &point = touch.GetPoint(0);
	float aspectR = mScreenSize.y / mScreenSize.x;
	switch(point.state)
	{
		case TouchPoint::Down:
		{
			mBeginMotion = false;
			mCompositionActor.SetShaderEffect(mRefractionShader);

			mCurrentTouchPosition = point.screen;

			mMetaballCenter = Vector2((point.screen.x / mScreenSize.x) - 0.5, (aspectR * (mScreenSize.y - point.screen.y) / mScreenSize.y) - 0.5) * 2.0;

			mTimerForMetaballs.Start();
			break;
		}
		case TouchPoint::Motion:
		{
			Vector2 displacement = point.screen - mCurrentTouchPosition;
			mCurrentTouchPosition = point.screen;

			Vector2 direction(0,0);
			if (fabs(displacement.x) > 0)
				direction.x = displacement.x / fabs(displacement.x);
			if (fabs(displacement.y) > 0)
				direction.y = displacement.y / fabs(displacement.y);

			mTime = 0;
			mMetaballPosVariationFrom = mMetaballPosVariation;
			mMetaballPosVariationTo.x += (displacement.x / mScreenSize.x) * 2.0; 
			mMetaballPosVariationTo.y += (- displacement.y / mScreenSize.y) * 2.0; 

			mMetaballCenter = Vector2((point.screen.x / mScreenSize.x) - 0.5, (aspectR * (mScreenSize.y - point.screen.y) / mScreenSize.y) - 0.5) * 2.0;
			break;
		}
		case TouchPoint::Up:
		case TouchPoint::Leave:
		case TouchPoint::Interrupted:
		{
			mCompositionActor.SetShaderEffect(mNormalShader);
			mTimerForMetaballs.Stop();

			mMetaballs[0].radius = mMetaballs[0].initRadius;
			mMetaballs[1].radius = mMetaballs[1].initRadius;
			mMetaballs[2].radius = mMetaballs[2].initRadius;

			mMetaballPosVariationTo = Vector2(0,0);
			mMetaballPosVariationFrom = Vector2(0,0);
			mMetaballPosVariation = Vector2(0,0);

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
