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


class HelloWorldExample : public ConnectionTracker
{
public:
  HelloWorldExample( Application& application ) : mApplication( application )
  {
    mApplication.InitSignal().Connect( this, &HelloWorldExample::Create );
  }

  void OnKeyEvent( const KeyEvent& event )
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    //mApplication.Quit();
    return true;
  }

  void Create( Application& application );
  bool OnButtonClicked( Button button );
  bool OnButtonPressed( Button button );
  bool OnButtonReleased( Button button );

private:
  Application&  mApplication;
};

void HelloWorldExample::Create( Application& application )
{
  PushButton button = PushButton::New();
  button.SetParentOrigin( ParentOrigin::CENTER );
  button.SetLabelText( "Press" );
  Stage::GetCurrent().Add( button );

  // Connect to button signals emitted by the button
  button.ClickedSignal().Connect( this, &HelloWorldExample::OnButtonClicked );
  button.PressedSignal().Connect( this, &HelloWorldExample::OnButtonPressed );
  button.ReleasedSignal().Connect( this, &HelloWorldExample::OnButtonReleased );

  // REMOVE
  Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect( this, &HelloWorldExample::OnTouch );
  Stage::GetCurrent().KeyEventSignal().Connect(this, &HelloWorldExample::OnKeyEvent);
}

bool HelloWorldExample::OnButtonClicked( Button button )
{
  // Do something when the button is clicked
  //cout << "OnButtonClicked" << endl;
  return true;
}

bool HelloWorldExample::OnButtonPressed( Button button )
{
  // Do something when the button is pressed
  //cout << "OnButtonPressed" << endl;
  return true;
}

bool HelloWorldExample::OnButtonReleased( Button button )
{
  // Do something when the button is released
  //cout << "OnButtonReleased" << endl;
  return true;
}

// Entry point for Linux & SLP applications
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  HelloWorldExample test( application );
  application.MainLoop();
  return 0;
}
