/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "shared/view.h"

#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;

namespace
{
const char* MATERIAL_SAMPLE( DALI_IMAGE_DIR "gallery-small-48.jpg" );
const char* MATERIAL_SAMPLE2( DALI_IMAGE_DIR "gallery-medium-19.jpg" );

#define MAKE_SHADER(A)#A

const char* VERTEX_SHADER = MAKE_SHADER(
attribute mediump vec2    aPosition;
attribute highp   vec2    aTexCoord;
varying   mediump vec2    vTexCoord;
uniform   mediump mat4    uMvpMatrix;
uniform   mediump vec3    uSize;
uniform   lowp    vec4    uFadeColor;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;
  vTexCoord = aTexCoord;
  gl_Position = vertexPosition;
}
);

const char* FRAGMENT_SHADER = MAKE_SHADER(
varying mediump vec2  vTexCoord;
uniform lowp  vec4    uColor;
uniform sampler2D     sTexture;
uniform lowp  vec4    uFadeColor;

void main()
{
  gl_FragColor = uColor * uFadeColor;
}
);

Geometry CreateGeometry()
{
  // Create vertices
  struct Vertex { Vector2 position; Vector2 textureCoordinates; };
  Vertex pentagonVertexData[5] = {
    { Vector2(   0.0f, -1.00f), Vector2(0.f, 0.f) },
    { Vector2(  -0.95f,-0.31f), Vector2(1.f, 0.f) },
    { Vector2(  -0.59f, 0.81f), Vector2(0.f, 1.f) },
    { Vector2(   0.59f, 0.81f), Vector2(1.f, 1.f) },
    { Vector2(   0.95f,-0.31f), Vector2(1.f, 1.f) } };

  Property::Map pentagonVertexFormat;
  pentagonVertexFormat["aPosition"] = Property::VECTOR2;
  pentagonVertexFormat["aTexCoord"] = Property::VECTOR2;
  PropertyBuffer pentagonVertices = PropertyBuffer::New( PropertyBuffer::STATIC, pentagonVertexFormat, 5 );
  pentagonVertices.SetData(pentagonVertexData);

  // Create indices
  unsigned short indexData[10] = { 0, 1, 1, 2, 2, 3, 3, 4, 4, 0 };
  Property::Map indexFormat;
  indexFormat["indices"] = Property::UNSIGNED_INTEGER;
  PropertyBuffer indices = PropertyBuffer::New( PropertyBuffer::STATIC, indexFormat, 5 );
  indices.SetData(indexData);

  // Create the geometry object
  Geometry pentagonGeometry = Geometry::New();
  pentagonGeometry.AddVertexBuffer( pentagonVertices );
  pentagonGeometry.SetIndexBuffer( indices );

  pentagonGeometry.SetGeometryType( Geometry::LINES );

  return pentagonGeometry;
}

} // anonymous namespace

// This example shows how to use a simple mesh
//
class ExampleController : public ConnectionTracker
{
public:

  /**
   * The example controller constructor.
   * @param[in] application The application instance
   */
  ExampleController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ExampleController::Create );
  }

  /**
   * The example controller destructor
   */
  ~ExampleController()
  {
    // Nothing to do here;
  }

  /**
   * Invoked upon creation of application
   * @param[in] application The application instance
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();
    stage.KeyEventSignal().Connect(this, &ExampleController::OnKeyEvent);

    mStageSize = stage.GetSize();

    // The Init signal is received once (only) during the Application lifetime

    // Hide the indicator bar
    application.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

    mImage = ResourceImage::New( MATERIAL_SAMPLE );
    mSampler = Sampler::New(mImage, "sTexture");
    mShader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );

    mMaterial = Material::New( mShader );
    mMaterial.AddSampler( mSampler );

    mGeometry = CreateGeometry();

    mRenderer = Renderer::New( mGeometry, mMaterial );

    mMeshActor = Actor::New();
    mMeshActor.AddRenderer( mRenderer );
    mMeshActor.SetSize(200, 200);

    Property::Index fadeColorIndex = mMeshActor.RegisterProperty( "fade-color", Color::GREEN );
    mMeshActor.AddUniformMapping( fadeColorIndex, std::string("uFadeColor") );

    fadeColorIndex = mRenderer.RegisterProperty( "fade-color", Color::MAGENTA );
    mRenderer.AddUniformMapping( fadeColorIndex, std::string("uFadeColor" ) );
    mRenderer.SetDepthIndex(0);

    mMeshActor.SetParentOrigin( ParentOrigin::CENTER );
    mMeshActor.SetAnchorPoint( AnchorPoint::CENTER );
    stage.Add( mMeshActor );

    mChangeImageTimer = Timer::New( 5000 );
    mChangeImageTimer.TickSignal().Connect( this, &ExampleController::OnTimer );
    mChangeImageTimer.Start();

    Animation  animation = Animation::New(5);
    KeyFrames keyFrames = KeyFrames::New();
    keyFrames.Add(0.0f, Vector4::ZERO);
    keyFrames.Add(1.0f, Vector4( 1.0f, 0.0f, 1.0f, 1.0f ));

    animation.AnimateBetween( Property( mRenderer, fadeColorIndex ), keyFrames, AlphaFunctions::Sin );
    animation.SetLooping(true);
    animation.Play();

    stage.SetBackgroundColor(Vector4(0.0f, 0.2f, 0.2f, 1.0f));;
  }

  /**
   * Invoked whenever the quit button is clicked
   * @param[in] button the quit button
   */
  bool OnQuitButtonClicked( Toolkit::Button button )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  bool OnTimer()
  {
    Image image = ResourceImage::New( MATERIAL_SAMPLE2 );
    mSampler.SetImage( image );
    return false;
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApplication.Quit();
      }
    }
  }

private:

  Application&  mApplication;                             ///< Application instance
  Vector3 mStageSize;                                     ///< The size of the stage

  Image    mImage;
  Sampler  mSampler;
  Shader   mShader;
  Material mMaterial;
  Geometry mGeometry;
  Renderer mRenderer;
  Actor    mMeshActor;
  Renderer mRenderer2;
  Actor    mMeshActor2;
  Timer    mChangeImageTimer;
};

void RunTest( Application& application )
{
  ExampleController test( application );

  application.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
