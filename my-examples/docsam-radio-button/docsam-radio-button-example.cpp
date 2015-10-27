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
  bool OnButtonStateChanged( Button button );

private:
  Application&  mApplication;
};

void HelloWorldExample::Create( Application& application )
{
  Actor radioGroup = Actor::New();
  radioGroup.SetParentOrigin( ParentOrigin::CENTER );
  Stage::GetCurrent().Add(radioGroup);

  RadioButton button1 = RadioButton::New();
  button1.SetLabelText( "button1" );
  button1.SetBackgroundColor( Color::WHITE );
  button1.SetPosition( 0, -40 );
  radioGroup.Add( button1 );

  RadioButton button2 = RadioButton::New();
  button2.SetLabelText( "button2" );
  button2.SetBackgroundColor( Color::WHITE );
  button2.SetPosition( 0, 40 );
  radioGroup.Add( button2 );

  // Connect a single callback to button signals emitted by both button1 & button2
  button1.StateChangedSignal().Connect( this, &HelloWorldExample::OnButtonStateChanged );
  button2.StateChangedSignal().Connect( this, &HelloWorldExample::OnButtonStateChanged );

  // REMOVE
  Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect( this, &HelloWorldExample::OnTouch );
  Stage::GetCurrent().KeyEventSignal().Connect(this, &HelloWorldExample::OnKeyEvent);
}

bool HelloWorldExample::OnButtonStateChanged( Toolkit::Button button )
{
  // Do something when the two button's states are changed.
  // The parameter button can be both button1 and button2.
  // You can use button.GetLabelText() and button.IsSelected() to know which button is selected.
  //cout << "OnButtonStateChanged " << button.GetLabelText() << " " << button.IsSelected() << endl;
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
