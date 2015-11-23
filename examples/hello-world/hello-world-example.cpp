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

#include <iostream>

#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using Dali::Toolkit::TextLabel;

// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application ),
    mPositionX( 0.0f ),
    mScale( 1.0f )
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
    mTextLabel.SetProperty( TextLabel::Property::RENDERING_BACKEND, Toolkit::Text::RENDERING_VECTOR_BASED );
    mTextLabel.SetProperty( TextLabel::Property::POINT_SIZE, 13.0f );
    mTextLabel.SetProperty( TextLabel::Property::FONT_FAMILY, "UbuntuMono" );
    mTextLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    mTextLabel.SetParentOrigin( AnchorPoint::CENTER );
    mTextLabel.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mTextLabel.SetScale( mScale, mScale, 1.0f );
    stage.Add( mTextLabel );

    mTextLabel2 = TextLabel::New( "Hello World" );
    mTextLabel2.SetProperty( TextLabel::Property::POINT_SIZE, 13.0f );
    mTextLabel2.SetProperty( TextLabel::Property::FONT_FAMILY, "UbuntuMono" );
    mTextLabel2.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    mTextLabel2.SetParentOrigin( AnchorPoint::CENTER );
    mTextLabel2.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    mTextLabel2.SetScale( mScale, mScale, 1.0f );
    stage.Add( mTextLabel2 );

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach( stage.GetRootLayer() );
    mPanGestureDetector.DetectedSignal().Connect( this, &HelloWorldController::OnPan );
  }

  void OnPan( Actor actor, const PanGesture& gesture )
  {
    Stage stage = Stage::GetCurrent();
    mPositionX += gesture.displacement.x;
    mTextLabel.SetX( mPositionX );
    mTextLabel2.SetX( mPositionX );

    mScale += gesture.displacement.y * -0.005f;
    mTextLabel.SetScale( mScale, mScale, 1.0f );
    mTextLabel2.SetScale( mScale, mScale, 1.0f );
  }

private:

  Application&  mApplication;

  TextLabel mTextLabel;
  TextLabel mTextLabel2;

  PanGestureDetector mPanGestureDetector;

  float mPositionX;
  float mScale;
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
