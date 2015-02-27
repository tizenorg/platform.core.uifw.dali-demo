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

using namespace Dali;
using namespace Dali::Toolkit;

// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application )
  {
    std::cout << "HelloWorldController::HelloWorldController" << std::endl;

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
    Vector2 size(200, 200);

    mActor = CreateSolidColorActor(Color::RED);
    mActor.SetAnchorPoint(AnchorPoint::CENTER);
    mActor.SetParentOrigin(ParentOrigin::CENTER);
    mActor.SetSize(size);
    mActor.SetName("ACTOR");
    mActor.SetLeaveRequired(true);
    stage.Add(mActor);


    mActor.TouchedSignal().Connect(this, &HelloWorldController::OnNoConsumeTouch);

    mTimer = Timer::New(2000);
    mTimer.TickSignal().Connect(this, &HelloWorldController::OnTimeTick);
    mTimer.Start();
  }

  bool OnConsumeTouch( Actor actor, const TouchEvent& touch )
  {
    std::cout << actor.GetName() << ", " << touch.GetPoint(0).state << std::endl;
    return true;
  }

  bool OnNoConsumeTouch( Actor actor, const TouchEvent& touch )
  {
    std::cout << actor.GetName() << ", " << touch.GetPoint(0).state << std::endl;
    return false;
  }

  bool OnTimeTick()
  {
    std::cout << "TimeTick" << std::endl;
    static int i = 0;

    if (i == 0)
    {
      Stage stage = Stage::GetCurrent();
      Vector2 size(200, 200);

      mActor.Unparent();
 
      stage.Add(mActor);
     
      Actor actor = CreateSolidColorActor(Color::BLUE);
      actor.SetAnchorPoint(AnchorPoint::CENTER);
      actor.SetParentOrigin(ParentOrigin::CENTER);
      actor.SetSize(size);
      actor.SetName("LAYER");
      actor.SetZ(10);
      stage.Add(actor);
     
      actor.TouchedSignal().Connect(this, &HelloWorldController::OnNoConsumeTouch);

      i += 3;
      return true;
    }
    else if (i == 1)
    {
      mActor.SetZ(20);
    }

    return false;
  }

private:
  Application&  mApplication;
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
