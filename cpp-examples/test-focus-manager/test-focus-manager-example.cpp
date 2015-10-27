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
#include <iostream>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace std;

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
    std::cout << "HelloWorldController::Create" << std::endl;

    ctrl1 = Control::New();
    ctrl1.SetName("ctrl1");
    ctrl1.SetBackgroundColor(Vector4(0.5,0.5,0.5,1));
    ctrl1.SetSize(100,100);
    ctrl1.SetPosition(300,300);
    ctrl1.SetKeyboardFocusable(true);
    Stage::GetCurrent().Add(ctrl1); 

    ctrl2 = Control::New();
    ctrl2.SetName("ctrl2");
    ctrl2.SetBackgroundColor(Vector4(0.5,0.5,0.5,1));
    ctrl2.SetSize(100,100);
    ctrl2.SetPosition(500,300);
    ctrl2.SetKeyboardFocusable(true);
    Stage::GetCurrent().Add(ctrl2); 

    KeyboardFocusManager::Get().SetFocusIndicatorActor(Actor());
    KeyboardFocusManager::Get().SetCurrentFocusActor(ctrl1);

    KeyboardFocusManager::Get().FocusChangedSignal().Connect( this, &HelloWorldController::OnKeyboardFocusChanged);
    //KeyboardFocusManager::Get().FocusedActorActivatedSignal().Connect( this, &HelloWorldController::OnKeyboardFocusedActorActivated);
    KeyboardFocusManager::Get().PreFocusChangeSignal().Connect( this, &HelloWorldController::OnKeyboardPreFocusChange);

    KeyboardFocusManager::Get().SetCurrentFocusActor(ctrl1);
    KeyboardFocusManager::Get().SetCurrentFocusActor(ctrl2);
  }

  void OnKeyboardFocusedActorActivated(Actor activatedFocusedActor)
  {
      cout << "activated " << activatedFocusedActor.GetName() << endl;
      //Control::DownCast(activatedFocusedActor).SetBackgroundColor(Vector4(0.5,0.5,0.5,1));
  }

  void OnKeyboardFocusChanged(Actor originalFocusedActor, Actor currentFocusedActor)
  {
    if( originalFocusedActor )
    {
      cout << "orig " << originalFocusedActor.GetName() << endl;
      Control::DownCast(originalFocusedActor).SetBackgroundColor(Vector4(0.5,0.5,0.5,1));
    }
    if( currentFocusedActor )
    {
      cout << "curr " << currentFocusedActor.GetName() << endl;
      Control::DownCast(currentFocusedActor).SetBackgroundColor(Vector4(1,1,1,1));
    }
    cout << endl;
  }

  Actor OnKeyboardPreFocusChange( Actor current, Actor proposed, Control::KeyboardFocus::Direction direction )
  {
    proposed = current;
    if( direction == Control::KeyboardFocus::LEFT && current == ctrl2 )
    {
      proposed = ctrl1;
    }
    else if( direction == Control::KeyboardFocus::RIGHT && current == ctrl1 )
    {
      proposed = ctrl2;
    }
    return proposed;
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    //mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
  Control ctrl1, ctrl2;
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
