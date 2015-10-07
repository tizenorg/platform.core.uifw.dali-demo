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
#include <dali/devel-api/rendering/renderer.h>

using namespace Dali;


namespace
{

const char* IMAGE_PATH[] = {
                            DALI_IMAGE_DIR "gallery-medium-1.jpg",
                            DALI_IMAGE_DIR "gallery-medium-2.jpg",
                            DALI_IMAGE_DIR "gallery-medium-3.jpg",
                            DALI_IMAGE_DIR "gallery-medium-4.jpg",
                            DALI_IMAGE_DIR "gallery-medium-5.jpg",
                            DALI_IMAGE_DIR "gallery-medium-6.jpg",
                            DALI_IMAGE_DIR "gallery-medium-7.jpg",
                            DALI_IMAGE_DIR "gallery-medium-8.jpg",
                            DALI_IMAGE_DIR "gallery-medium-9.jpg",
                            DALI_IMAGE_DIR "gallery-medium-10.jpg",
                            DALI_IMAGE_DIR "gallery-medium-11.jpg",
                            DALI_IMAGE_DIR "gallery-medium-12.jpg",
                            DALI_IMAGE_DIR "gallery-medium-13.jpg",
                            DALI_IMAGE_DIR "gallery-medium-14.jpg",
                            DALI_IMAGE_DIR "gallery-medium-15.jpg",
                            DALI_IMAGE_DIR "gallery-medium-16.jpg",
                            DALI_IMAGE_DIR "gallery-medium-17.jpg",
                            DALI_IMAGE_DIR "gallery-medium-18.jpg",
                            DALI_IMAGE_DIR "gallery-medium-19.jpg",
                            DALI_IMAGE_DIR "gallery-medium-20.jpg",
                            DALI_IMAGE_DIR "gallery-medium-21.jpg",
                            DALI_IMAGE_DIR "gallery-medium-22.jpg",
                            DALI_IMAGE_DIR "gallery-medium-23.jpg",
                            DALI_IMAGE_DIR "gallery-medium-24.jpg",
                            DALI_IMAGE_DIR "gallery-medium-25.jpg",
                            DALI_IMAGE_DIR "gallery-medium-26.jpg",
                            DALI_IMAGE_DIR "gallery-medium-27.jpg",
                            DALI_IMAGE_DIR "gallery-medium-28.jpg",
                            DALI_IMAGE_DIR "gallery-medium-29.jpg",
                            DALI_IMAGE_DIR "gallery-medium-30.jpg",
                            DALI_IMAGE_DIR "gallery-medium-31.jpg",
                            DALI_IMAGE_DIR "gallery-medium-32.jpg",
                            DALI_IMAGE_DIR "gallery-medium-33.jpg",
                            DALI_IMAGE_DIR "gallery-medium-34.jpg",
                            DALI_IMAGE_DIR "gallery-medium-35.jpg",
                            DALI_IMAGE_DIR "gallery-medium-36.jpg",
                            DALI_IMAGE_DIR "gallery-medium-37.jpg",
                            DALI_IMAGE_DIR "gallery-medium-38.jpg",
                            DALI_IMAGE_DIR "gallery-medium-39.jpg",
                            DALI_IMAGE_DIR "gallery-medium-40.jpg",
                            DALI_IMAGE_DIR "gallery-medium-41.jpg",
                            DALI_IMAGE_DIR "gallery-medium-42.jpg",
                            DALI_IMAGE_DIR "gallery-medium-43.jpg",
                            DALI_IMAGE_DIR "gallery-medium-44.jpg",
                            DALI_IMAGE_DIR "gallery-medium-45.jpg",
                            DALI_IMAGE_DIR "gallery-medium-46.jpg",
                            DALI_IMAGE_DIR "gallery-medium-47.jpg",
                            DALI_IMAGE_DIR "gallery-medium-48.jpg",
                            DALI_IMAGE_DIR "gallery-medium-49.jpg",
                            DALI_IMAGE_DIR "gallery-medium-50.jpg",
                            DALI_IMAGE_DIR "gallery-medium-51.jpg",
                            DALI_IMAGE_DIR "gallery-medium-52.jpg",
                            DALI_IMAGE_DIR "gallery-medium-53.jpg",
};

struct VertexWithTexture
{
  Vector2 position;
  Vector2 texCoord;
};

VertexWithTexture gQuadWithTexture[] = {
                                        { Vector2( -0.5f, -0.5f ), Vector2( 0.0f, 0.0f ) },
                                        { Vector2(  0.5f, -0.5f ), Vector2( 1.0f, 0.0f ) },
                                        { Vector2( -0.5f,  0.5f ), Vector2( 0.0f, 1.0f ) },
                                        { Vector2(  0.5f,  0.5f ), Vector2( 1.0f, 1.0f ) }
};

const char* VERTEX_SHADER_TEXTURE = DALI_COMPOSE_SHADER(
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

const char* FRAGMENT_SHADER_TEXTURE = DALI_COMPOSE_SHADER(
    uniform lowp vec4 uColor;\n
    uniform sampler2D sTexture;\n
    varying mediump vec2 vTexCoord;\n

    void main()\n
    {\n
      gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
    }\n
);


Geometry& QuadMesh()
{
  static Geometry mesh;
  if( !mesh )
  {
    PropertyBuffer vertexBuffer;
    Property::Map vertexFormat;
    vertexFormat["aPosition"] = Property::VECTOR2;
    vertexFormat["aTexCoord"] = Property::VECTOR2;

    //Create a vertex buffer for vertex positions and texture coordinates
    vertexBuffer = PropertyBuffer::New( vertexFormat, 4u );
    vertexBuffer.SetData( gQuadWithTexture );

    //Create the geometry
    mesh = Geometry::New();
    mesh.AddVertexBuffer( vertexBuffer );
    mesh.SetGeometryType(Geometry::TRIANGLE_STRIP );
  }
  return mesh;
}

Renderer CreateRenderer( unsigned int index )
{
     //Create the renderer
    Shader shader = Shader::New( VERTEX_SHADER_TEXTURE, FRAGMENT_SHADER_TEXTURE );
    static Image image = ResourceImage::New(IMAGE_PATH[index]);
    //Sampler textureSampler = Sampler::New( image, "sTexture" );
    Material material = Material::New( shader );
    material.AddTexture( image, "sTexture");
    material.SetBlendMode( BlendingMode::OFF );
    Renderer renderer = Renderer::New( QuadMesh(), material );


    return renderer;
}

Actor CreateMeshActor( unsigned int index)
{
  Renderer renderer = CreateRenderer(index);
  Actor meshActor = Actor::New();
  meshActor.AddRenderer( renderer );
  return meshActor;
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

    Actor actor = CreateMeshActor( 0 );
    actor.SetPosition(100.0f,100.0f,0.0f);
    actor.SetSize(100.0f,100.0f);
    stage.Add( actor );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
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
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
