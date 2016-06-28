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

#include <dali/dali.h>
#include <dali/devel-api/rendering/renderer.h>
#include <dali/devel-api/adaptor-framework/bitmap-loader.h>


#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using Dali::Toolkit::TextLabel;

namespace
{

static const char* VERTEX_SHADER_TEXTURE = DALI_COMPOSE_SHADER(
    attribute mediump vec2 aPosition;\n
    attribute mediump vec2 aTexCoord;\n
    uniform mediump mat4 uMvpMatrix;\n
    uniform mediump vec3 uSize;\n
    varying mediump vec2 vTexCoord;\n
    void main()\n
    {\n
      vec4 position = vec4(aPosition,0.0,1.0)*vec4(uSize,1.0);\n
      gl_Position = uMvpMatrix * position;\n
      vTexCoord = aTexCoord;\n
    }\n
);

static const char* FRAGMENT_SHADER_TEXTURE = DALI_COMPOSE_SHADER(
    uniform lowp vec4 uColor;\n
    uniform sampler2D sTexture;\n
    varying mediump vec2 vTexCoord;\n

    void main()\n
    {\n
      gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
    }\n
);
/**
 * @brief Creates a quad with texture coordinates
 * @return A geometry
 */
Geometry CreateQuadGeometry()
{
  struct Vertex
  {
    Vector2 position;
    Vector2 texCoord;
  };

  static const Vertex gQuadWithTexture[] = {{ Vector2( -0.5f, -0.5f ), Vector2( 0.0f, 0.0f ) },
                                            { Vector2(  0.5f, -0.5f ), Vector2( 1.0f, 0.0f ) },
                                            { Vector2( -0.5f,  0.5f ), Vector2( 0.0f, 1.0f ) },
                                            { Vector2(  0.5f,  0.5f ), Vector2( 1.0f, 1.0f ) }};
  PropertyBuffer vertexBuffer;
  Property::Map vertexFormat;
  vertexFormat["aPosition"] = Property::VECTOR2;
  vertexFormat["aTexCoord"] = Property::VECTOR2;

  //Create a vertex buffer for vertex positions and texture coordinates
  vertexBuffer = PropertyBuffer::New( vertexFormat );
  vertexBuffer.SetData( gQuadWithTexture, 4u );

  //Create the geometry
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexBuffer );
  geometry.SetGeometryType(Geometry::TRIANGLE_STRIP );

  return geometry;
}


/**
 * @brief Creates an actor to render a native image
 * @param[in] texture The texture creates from a native image
 * @param[in] nativeImageInterface The native image interface used to create the texture
 * @return An actor that renders the texture
 */
Actor CreateNativeImageActor( Texture texture )
{
  Actor actor = Actor::New();
  Geometry geometry = CreateQuadGeometry();
  Shader shader = Shader::New(VERTEX_SHADER_TEXTURE, FRAGMENT_SHADER_TEXTURE);
  Renderer renderer = Renderer::New( geometry, shader );
  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture( 0u, texture );
  renderer.SetTextures( textureSet );

  actor.AddRenderer( renderer );
  actor.SetSize( texture.GetWidth(), texture.GetHeight() );
  return actor;
}

Dali::PixelData LoadPixelData( const char*imagePath, bool fillStage )
{
  Dali::BitmapLoader loader;
  if( fillStage )
  {
    Dali::Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
    loader = Dali::BitmapLoader::New( imagePath, Dali::ImageDimensions( stageSize.x, stageSize.y ), Dali::FittingMode::SCALE_TO_FILL, Dali::SamplingMode::BOX_THEN_LINEAR );
  }
  else
  {
    loader = Dali::BitmapLoader::New( imagePath );
  }
  loader.Load();

  return loader.GetPixelData();
}

Texture LoadTexture( const char* url )
{
  PixelData pixelData = LoadPixelData( url, false );
  Texture texture = Texture::New( TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight() );
  texture.Upload( pixelData );
  return texture;
}

}
// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &HelloWorldController::Create );
  }

  ~HelloWorldController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    TextLabel textLabel = TextLabel::New( "Hello World" );
    textLabel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    textLabel.SetName( "helloWorldLabel" );
    stage.Add( textLabel );

    //Texture texture = LoadTexture( DEMO_IMAGE_DIR "gallery-large-4.astc");
    Texture texture = LoadTexture( DEMO_IMAGE_DIR "gallery-large-4.jpg");
    Actor actor = CreateNativeImageActor(texture);
    actor.SetSize( 400.0f, 400.0f );
    actor.SetAnchorPoint( AnchorPoint::CENTER);
    actor.SetParentOrigin( ParentOrigin::CENTER );
    actor.SetPosition( 0.0f, -200.0f, 0.0f);
    stage.Add( actor );

    Texture texture2 = LoadTexture( DEMO_IMAGE_DIR "gallery-large-4.astc");
    Actor actor2 = CreateNativeImageActor(texture2);
    actor2.SetSize( 400.0f, 400.0f );
    actor2.SetAnchorPoint( AnchorPoint::CENTER);
    actor2.SetParentOrigin( ParentOrigin::CENTER );
    actor2.SetOrientation(Degree(180.0f), Vector3(1.0f,0.0f,0.0f));
    actor2.SetPosition( 0.0f, 200.0f, 0.0f);
    stage.Add( actor2 );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchData& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
};

void RunTest( Application& application )
{
  HelloWorldController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
