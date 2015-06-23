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

using namespace std;
using namespace Dali;
using Dali::Toolkit::TextLabel;
using Dali::Toolkit::TextField;

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

    mTextLabel = TextLabel::New( "Hello World" );
    mTextLabel.SetAnchorPoint(AnchorPoint::CENTER);
    mTextLabel.SetParentOrigin(ParentOrigin::CENTER);
    mTextLabel.SetName( "hello-world-label" );
    stage.Add( mTextLabel );

    TextField field = TextField::New();
    field.SetProperty( TextField::Property::TEXT, "Country:" );
    field.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    stage.Add( field );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );

    mTimer = Timer::New(1000);
    mTimer.TickSignal().Connect(this, &HelloWorldController::OnTimerTick);
    mTimer.Start();
  }

  bool OnTimerTick()
  {
    static int i = 0;

    string title[5] =
    {
      "Seoul",
      "Beijing",
      "London",
      "",
      "Paris"
    };

    mTextLabel.SetProperty( TextLabel::Property::TEXT, title[i] );

    i++;
    if (i >= 5)
    {
       i = 0;
    }

    return true;
  }


  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;

  TextLabel     mTextLabel;
  Timer         mTimer;
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
