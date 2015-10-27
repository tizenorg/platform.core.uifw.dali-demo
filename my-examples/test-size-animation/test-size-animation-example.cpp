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
#include <stdio.h>
#include <dali-toolkit/dali-toolkit.h>
#include <cstdarg>
#include <sstream>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace std;

bool gAnimation = true;
//bool gAnimation = false;

bool gParentMode = true;
//bool gParentMode = false;

bool gSizing = true;
//bool gSizing = false;

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

    Stage::GetCurrent().GetRootLayer().SetDepthTestDisabled(true);

    if(gParentMode)
    {
      mParent = Control::New();
      mParent.SetParentOrigin(ParentOrigin::CENTER);
      mParent.SetBackgroundColor(Vector4(0,0,.5,1));
      Stage::GetCurrent().Add(mParent);

      mParent.SetSize(200,200);
    }

    mText = TextLabel::New();
    mText.SetProperty( TextLabel::Property::TEXT, "Connect to the Application's Init signal." );
    mText.SetParentOrigin(ParentOrigin::CENTER);
    mText.SetBackgroundColor(Vector4(0,0,1,1));

    if(gParentMode)
    {
      mText.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
      mParent.Add(mText);
    }
    else
    {
      mText.SetSize(200,200);
      Stage::GetCurrent().Add(mText);
    }

    mText.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    mText.SetProperty( TextLabel::Property::MULTI_LINE, true );
    mText.SetProperty( TextLabel::Property::TEXT_COLOR, Vector4(1,1,1,1) );

    Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    Actor targetActor;
    if(gParentMode)
      targetActor = mParent;
    else
      targetActor = mText;

    if(gSizing)
    {
      Vector3 targetSize(100,400,0);

      if(gAnimation)
      {
        Animation ani = Animation::New(1.);
        ani.AnimateTo( Property( targetActor, Actor::Property::SIZE ), targetSize );
        ani.Play();
      }
      else
        targetActor.SetSize(targetSize);
    }
    else
    {
      Vector3 targetPos(100,0,0);

      if(gAnimation)
      {
        Animation ani = Animation::New(1.);
        ani.AnimateTo( Property( targetActor, Actor::Property::POSITION ), targetPos );
        ani.Play();
      }
      else
        targetActor.SetPosition(targetPos);
    }

    return true;
  }

private:
  Application&  mApplication;
  TextLabel mText;
  Control mParent;
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
