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
#include <iostream>

using namespace Dali;
using Dali::Toolkit::TextLabel;

namespace
{

const char* IMAGE_PATH( DEMO_IMAGE_DIR "gallery-medium-14.jpg" );

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  varying mediump vec2 vTexCoord;\n
  \n
  void main()\n
  {\n
    mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n
    vertexPosition.xyz *= uSize;\n
    vertexPosition = uMvpMatrix * vertexPosition;\n
    \n
    vTexCoord = aPosition + vec2(0.5);\n
    gl_Position = vertexPosition;\n
  }\n
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uColor;\n
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
  }\n
);
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

    Image image = ResourceImage::New(IMAGE_PATH);
    Toolkit::ImageView imageView1 = Toolkit::ImageView::New( image);
    Toolkit::ImageView imageView2 = Toolkit::ImageView::New( image);
    Toolkit::ImageView imageView3 = Toolkit::ImageView::New( image);
    Toolkit::ImageView imageView4 = Toolkit::ImageView::New( image);

    imageView1.SetParentOrigin( ParentOrigin::TOP_LEFT );
    imageView1.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    stage.Add( imageView1 );

    imageView2.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    imageView2.SetAnchorPoint( AnchorPoint::TOP_RIGHT );
    stage.Add( imageView2 );

    imageView3.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    imageView3.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    stage.Add( imageView3 );

    imageView4.SetParentOrigin( ParentOrigin::BOTTOM_RIGHT );
    imageView4.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
    stage.Add( imageView4 );

    std::cout<<"customVertexDefaultFragment"<<std::endl;
    Property::Map customVertexDefaultFragment;
    Property::Map customVertexShader;
    customVertexShader[ "vertexShader" ] = VERTEX_SHADER;
    customVertexDefaultFragment[ "shader" ] = customVertexShader;
    imageView2.SetProperty( Toolkit::ImageView::Property::IMAGE, customVertexDefaultFragment );

    std::cout<<"defaultVertexCustomFragment"<<std::endl;
    Property::Map defaultVertexCustomFragment;
    Property::Map customfragmentShader;
    customfragmentShader[ "fragmentShader" ] = FRAGMENT_SHADER;
    defaultVertexCustomFragment[ "shader" ] = customfragmentShader;
    imageView3.SetProperty( Toolkit::ImageView::Property::IMAGE, defaultVertexCustomFragment );

    std::cout<<"customVertexcustomFragment"<<std::endl;
    Property::Map customVertexcustomFragment;
    Property::Map customVertexFragmentShader;
    customVertexFragmentShader[ "vertexShader" ] = VERTEX_SHADER;
    customVertexFragmentShader[ "fragmentShader" ] = FRAGMENT_SHADER;
    customVertexcustomFragment[ "shader" ] = customVertexFragmentShader;
    imageView4.SetProperty( Toolkit::ImageView::Property::IMAGE, customVertexcustomFragment );


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
