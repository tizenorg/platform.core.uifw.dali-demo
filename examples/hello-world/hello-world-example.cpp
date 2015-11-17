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
#include "visual-profiler.h"
#include <stdio.h>
using namespace Dali;
using Dali::Toolkit::TextLabel;


// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mTouchCount( 0 ),
    mApplication( application )

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

    mTextLabel = TextLabel::New( "Hello World" );
    mTextLabel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mTextLabel.SetName( "hello-world-label" );
    stage.Add( mTextLabel );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    mTouchCount++;

    if( mTouchCount == 1)
    {
      mProfiler.Connect("106.1.9.134", 3031);
    // mProfiler.Connect("127.0.0.1", 3031);
      mBufferImage = mProfiler.GetGraph();

      mProfileGraph = ImageActor::New( mBufferImage );


      mProfileGraph.SetSize( Vector2( mBufferImage.GetWidth(),mBufferImage.GetHeight()));

      mProfileGraph.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      mProfileGraph.SetPosition( 0,0);

      Stage stage = Stage::GetCurrent();
      stage.Add( mProfileGraph );
    }

    if( mTouchCount == 10)
    {
      // quit the application on second touch
      //mApplication.Quit();
    }
    return true;
  }

private:
  int mTouchCount;
  Application&  mApplication;
  Profiler mProfiler;
  TextLabel mTextLabel;
  ImageActor mProfileGraph;
  BufferImage mBufferImage;
  PixelBuffer* mPixelBuffer;
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
