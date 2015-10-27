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

  for( int i=0; i<5; ++i )
  {
    Toolkit::PushButton button = Toolkit::PushButton::New();
    button.SetSize( 100, 100 );
    button.SetPosition( 100*i+50, 50 );
    button.ClickedSignal().Connect( this, &HelloWorldController::OnButtonClicked );

    // Register a custom property having button index.
    // Store the property index so you can look it up later.
    // Note: This is much faster than looking the property up by property name and should always be used if possible.
    // As all control types are the same (PushButtons) the indices to the unique custom property are all same.
    Property::Value data( i );
    mCustomDataIndex = button.RegisterProperty( "custom-data", data );

    Stage::GetCurrent().Add(button);
  }

  Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
  Stage::GetCurrent().KeyEventSignal().Connect(this, &HelloWorldController::OnKeyEvent);

{
Actor actor = Actor::New();
actor.SetName("test actor");
std::cout << actor.GetName() << std::endl;  // "test actor"
}

{
Actor actor = Actor::New();
actor.SetProperty( Actor::Property::NAME, "test actor" );
std::cout << actor.GetProperty( Actor::Property::NAME ) << std::endl;  // "test actor"
std::cout << actor.GetProperty<std::string>( Actor::Property::NAME ) << std::endl;  // "test actor"
std::cout << actor.GetProperty( Actor::Property::NAME ).Get<std::string>() << std::endl;  // "test actor"
}

}

bool OnButtonClicked(Toolkit::Button button)
{
  // Look up the custom property by the stored property index.
  // Note: If the property belongs to a control in another library, or we do not know the index, we can look the index up first with:
  // Property::Index index = button.GetPropertyIndex( "custom-data" );
  cout << button.GetProperty( mCustomDataIndex ) << endl;
  return true;
}

  void OnKeyEvent(const KeyEvent& event)
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

private:
  Application&  mApplication;
  Property::Index mCustomDataIndex;
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
