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

#define DALI_PROFILE_UBUNTU

#include <dali/dali.h>
#ifndef DALI_PROFILE_UBUNTU
#include <dali/public-api/adaptor-framework/key-grab.h>
#endif
#include <iostream>
#include <stdio.h>
#include <dali-toolkit/dali-toolkit.h>
#include <cstdarg>
#include <sstream>

using namespace std;
using namespace Dali;
using namespace Dali::Toolkit;

std::string stringf(const char* format, ...)
{
    va_list arg_list;                                                           
    va_start(arg_list, format);                                                 

    // SUSv2 version doesn't work for buf NULL/size 0, so try printing
    // into a small buffer that avoids the double-rendering and alloca path too...
    char short_buf[256];                                                        
    const size_t needed = vsnprintf(short_buf, sizeof short_buf,
                                    format, arg_list) + 1;
    if (needed <= sizeof short_buf)
        return short_buf;

    // need more space...
    char* p = static_cast<char*>(alloca(needed));
    vsnprintf(p, needed, format, arg_list);
    return p;
}

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
    Vector2 stageSize = Stage::GetCurrent().GetSize();

    mGrabbing = false;

    //float fontSize = 1000.f;
    //float fontSize = 100.f;
    float fontSize = 10.f;

    mGrabModeLabel = TextLabel::New("Not Grabbing");
    mGrabModeLabel.SetParentOrigin(ParentOrigin::CENTER);
    mGrabModeLabel.SetSize(stageSize.x*.5, stageSize.y*.25);
    mGrabModeLabel.SetPosition(Vector3(0,-stageSize.y*.25,0));
    mGrabModeLabel.SetBackgroundColor(Vector4(0,0,.5,1));
    mGrabModeLabel.SetProperty(TextLabel::Property::POINT_SIZE, fontSize);
    mGrabModeLabel.SetProperty(TextLabel::Property::TEXT_COLOR, Vector4(1,1,1,1));
    Stage::GetCurrent().Add(mGrabModeLabel);

    mGrabbedKeyLabel = TextLabel::New("Grabbed Key");
    mGrabbedKeyLabel.SetParentOrigin(ParentOrigin::CENTER);
    mGrabbedKeyLabel.SetSize(stageSize.x*.5, stageSize.y*.25);
    mGrabbedKeyLabel.SetPosition(Vector3(0,stageSize.y*.25,0));
    mGrabbedKeyLabel.SetBackgroundColor(Vector4(.5,0,0,1));
    mGrabbedKeyLabel.SetProperty(TextLabel::Property::POINT_SIZE, fontSize);
    mGrabbedKeyLabel.SetProperty(TextLabel::Property::TEXT_COLOR, Vector4(1,1,1,1));
    Stage::GetCurrent().Add(mGrabbedKeyLabel);

    Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
    Stage::GetCurrent().KeyEventSignal().Connect( this, &HelloWorldController::OnKeyEvent );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    if(touch.GetPoint(0).state == TouchPoint::Finished)
    {
      KEY grabKey = DALI_KEY_VOLUME_UP;
      cout << "DALI_KEY_VOLUME_UP: " << grabKey << endl;

      if(!mGrabbing)
      {
        Window window = mApplication.GetWindow();

        mGrabModeLabel.SetProperty(TextLabel::Property::TEXT, "Status: Not Grabbing");
        cout << "Status: Not Grabbing" << endl;

#ifndef DALI_PROFILE_UBUNTU
        bool ret = KeyGrab::GrabKey(window, grabKey, KeyGrab::SHARED);
#else
        bool ret = false;
#endif

        if( ret )
        {
          mGrabbing = true;
          mGrabbedKeyLabel.SetProperty(TextLabel::Property::TEXT, 
                                      stringf("GrabKey() succeeded \nfor DALI_KEY_VOLUME_UP\n" ));
          cout << "GrabKey() succeeded for DALI_KEY_VOLUME_UP" << endl;

          mGrabModeLabel.SetProperty(TextLabel::Property::TEXT, "Status: Grabbing");
          cout << "Status: Grabbing" << endl;
        }
        else
        {
          mGrabbedKeyLabel.SetProperty(TextLabel::Property::TEXT, stringf("GrabKey() failed\n" ));
          cout << "GrabKey() failed" << endl;

          mGrabModeLabel.SetProperty(TextLabel::Property::TEXT, "Status: Not Grabbing");
          cout << "Status: Not Grabbing" << endl;
        }
        cout << endl;
      }
      else
      {
        Window window = mApplication.GetWindow();

        mGrabModeLabel.SetProperty(TextLabel::Property::TEXT, "Status: Grabbing");
        cout << "Status: Grabbing" << endl;

#ifndef DALI_PROFILE_UBUNTU
        bool ret = KeyGrab::UngrabKey(window, grabKey);
#else
        bool ret = false;
#endif

        if( ret )
        {
          mGrabbing = false;

          mGrabbedKeyLabel.SetProperty(TextLabel::Property::TEXT, stringf("UngrabKey() succeeded\n" ));
          cout << "UngrabKey() succeeded" << endl;

          mGrabModeLabel.SetProperty(TextLabel::Property::TEXT, "Status: Not Grabbing");
          cout << "Status: Not Grabbing" << endl;
        }
        else
        {
          mGrabbedKeyLabel.SetProperty(TextLabel::Property::TEXT, stringf("UngrabKey() failed\n" ));
          cout << "GrabKey() failed" << endl;

          mGrabModeLabel.SetProperty(TextLabel::Property::TEXT, "Status: Grabbing");
          cout << "Status: Grabbing" << endl;
        }
        cout << endl;
      }
    }
    return true;
  }

  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if(IsKey(event, DALI_KEY_MENU))
      {
      } 
      else
      {
        mGrabbedKeyLabel.SetProperty(TextLabel::Property::TEXT, event.keyPressedName);
        cout << event.keyPressedName << endl;
      }
    }
  }

private:
  Application&  mApplication;

  TextLabel mGrabModeLabel, mGrabbedKeyLabel;
  bool mGrabbing;
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

