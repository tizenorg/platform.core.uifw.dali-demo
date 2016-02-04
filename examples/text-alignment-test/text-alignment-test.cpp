/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <string>

#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using Dali::Toolkit::TextLabel;

// Used to store the command-line options
struct Options
{
  Options()
  : text( "Hello World" ),
    position( 0, 0 ),
    usePerspective( false )
  {
  }

  Options( const Options& copyMe )
  : text( copyMe.text ),
    position( copyMe.position ),
    usePerspective( copyMe.usePerspective )
  {
  }

  Options& operator=( const Options& rhs );

  std::string text;
  Vector2 position;
  bool usePerspective;
};

// Testing the result of text rendering for pixel-alignment issues
//
class TestController : public ConnectionTracker
{
public:

  TestController( Application& application, const Options& options )
  : mApplication( application ),
    mOptions( options )
  {
    mApplication.InitSignal().Connect( this, &TestController::Create );
  }

  ~TestController()
  {
  }

  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );
    Vector2 stageSize( stage.GetSize() );

    TextLabel helpText = TextLabel::New( "Touch screen to test" );
    helpText.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    stage.Add( helpText );

    TextLabel offscreenText = TextLabel::New( mOptions.text );
    offscreenText.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    stage.Add( offscreenText );

    CameraActor offscreenCameraActor = CameraActor::New();
    offscreenCameraActor.SetParentOrigin( ParentOrigin::CENTER );
    offscreenCameraActor.SetOrthographicProjection( stageSize );
    stage.Add( offscreenCameraActor );

    RenderTask renderTask = stage.GetRenderTaskList().CreateTask();
    renderTask.SetSourceActor( offscreenText );
    renderTask.SetClearColor( Color::TRANSPARENT );
    renderTask.SetClearEnabled( true );
    renderTask.SetExclusive( true );
    renderTask.SetCameraActor( offscreenCameraActor );

    FrameBufferImage frameBufferImage = FrameBufferImage::New( stageSize.width, stageSize.height, Pixel::RGBA8888 );
    renderTask.SetTargetFrameBuffer( frameBufferImage );

    stage.GetRootLayer().TouchedSignal().Connect( this, &TestController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    mApplication.Quit();
    return true;
  }

private:

  Application&  mApplication;

  Options mOptions;
};

// Returns the options from command-line
//
void ParseCommandLineArgs( int argc, char **argv, Options& options )
{
  for( int i(1) ; i < argc; ++i )
  {
    std::string arg( argv[i] );
    if( arg.compare( 0, 2, "--text=" ) == 0 )
    {
      options.text = atoi( arg.substr( 7 ).c_str() );
    }
    else if( arg.compare( 0, 2, "-x" ) == 0 )
    {
      options.position.x = static_cast<float>( atof( arg.substr( 2 ).c_str() ) );
    }
    else if( arg.compare( 0, 2, "-y" ) == 0 )
    {
      options.position.y = static_cast<float>( atof( arg.substr( 2 ).c_str() ) );
    }
    else if( arg.compare( "--use-perspective" ) == 0 )
    {
      options.usePerspective = true;
    }
  }
}

void RunTest( Application& application, const Options& options )
{
  TestController test( application, options );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  Options options;
  ParseCommandLineArgs( argc, argv, options );

  RunTest( application, options );

  return 0;
}
