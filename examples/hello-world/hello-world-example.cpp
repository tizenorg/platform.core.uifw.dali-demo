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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

#include "circular-number.h"

using namespace Dali;
using namespace Toolkit;

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
    Size stageSize = stage.GetSize();
    stage.SetBackgroundColor( Color::BLUE );

    Control control1 = Control::New();
    control1.SetBackgroundColor( Color::BLACK );
    control1.SetParentOrigin( ParentOrigin::CENTER );
    control1.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
    control1.SetSize( stageSize * 0.5f );
    stage.Add( control1 );

    Control control2 = Control::New();
    control2.SetBackgroundColor( Color::BLACK );
    control2.SetParentOrigin( ParentOrigin::CENTER );
    control2.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    control2.SetSize( stageSize * 0.5f );
    stage.Add( control2 );

    mNumber = CircularNumber::New();
    mNumber.SetParentOrigin( ParentOrigin::CENTER );
    mNumber.SetAnchorPoint( AnchorPoint::CENTER );
    stage.Add( mNumber );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchData& event )
  {
    if( PointState::DOWN == event.GetState( 0 ) )
    {
      mNumber.ChangeNumber( int(Random::Range(0.f, 1.f) * 1000.f), 3.f );
    }

    return true;
  }

private:
  Application&  mApplication;

  CircularNumber mNumber;
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
