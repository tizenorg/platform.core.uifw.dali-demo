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
#include <dali-toolkit/dali-toolkit.h>
#include <iostream>

using namespace std;
using namespace Dali;
using namespace Dali::Toolkit;

const char *strEvents[] =
{
  "Down",
  "Up",
  "Motion",
  "Leave",
  "Stationary",
  "Interrupted"
};

// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application )
  {
    cout << "HelloWorldController::HelloWorldController" << endl;

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
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();
    Vector2 actorSize(200, 200);

    mParent1 = CreateSolidColorActor(Color::RED);
    mParent1.SetParentOrigin(ParentOrigin::CENTER);
    mParent1.SetAnchorPoint(AnchorPoint::BOTTOM_CENTER);
    mParent1.SetSize(stageSize.width, stageSize.height * 0.5f);
    mParent1.SetName("mParent1");
    stage.Add(mParent1);

    mParent2 = CreateSolidColorActor(Color::BLUE);
    mParent2.SetParentOrigin(ParentOrigin::CENTER);
    mParent2.SetAnchorPoint(AnchorPoint::TOP_CENTER);
    mParent2.SetSize(stageSize.width, stageSize.height * 0.5f);
    mParent2.SetName("mParent2");
    stage.Add(mParent2);

    mActor = CreateSolidColorActor(Color::YELLOW);
    mActor.SetParentOrigin(ParentOrigin::BOTTOM_CENTER);
    mActor.SetAnchorPoint(AnchorPoint::CENTER);
    mActor.SetSize(actorSize);
    mActor.SetName("mActor");
    mActor.SetLeaveRequired(true);
    mActor.SetZ(1.f);
    mParent1.Add(mActor);

    mActor.TouchedSignal().Connect(this, &HelloWorldController::OnActorTouched);

    mTimer = Timer::New(3000); // waiting for 3 seconds
    mTimer.TickSignal().Connect(this, &HelloWorldController::OnTimerExpired);
    mTimer.Start();
  }

  bool OnActorTouched( Actor actor, const TouchEvent& touch )
  {
    cout << actor.GetName() << ", " << strEvents[int(touch.GetPoint(0).state)] << endl;

    return true;
  }

  bool OnTimerExpired()
  {
    cout << "OnTimerExpired" << endl;

#if 0 // case #1
    mParent2.Add(mActor);
    mActor.SetParentOrigin(ParentOrigin::TOP_CENTER);
#endif

#if 1 // case #2
    mActor.Unparent();
#endif

    return false;
  }

private:
  Application&  mApplication;
  ImageActor    mParent1;
  ImageActor    mParent2;
  ImageActor    mActor;
  Timer         mTimer;
};

void RunTest( Application& application )
{
  HelloWorldController test( application );

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
