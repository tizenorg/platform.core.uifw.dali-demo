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

//#define DALI_PROFILE_UBUNTU

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

struct KeyLookup
{
  const char* keyName;      ///< X string representation
  const KEY   daliKeyCode;  ///< Dali Enum Representation
  const bool  deviceButton; ///< Whether the key is from a button on the device
};

// Taken from key-impl.cpp
KeyLookup TestKeyLookupTable[]=
{
  { "Escape",                DALI_KEY_ESCAPE,          false },  // item not defined in utilX
  { "Menu",                  DALI_KEY_MENU,            false },  // item not defined in utilX

  // Now the key names are used as literal string not defined symbols,
  // since these definition in utilX.h is deprecated and we're guided not to use them
  { "XF86Camera",            DALI_KEY_CAMERA,          false },
  { "XF86Camera_Full",       DALI_KEY_CONFIG,          false },
  { "XF86PowerOff",          DALI_KEY_POWER,           true  },
  { "Cancel",                DALI_KEY_CANCEL,          false },
  { "XF86AudioPlay",         DALI_KEY_PLAY_CD,         false },
  { "XF86AudioStop",         DALI_KEY_STOP_CD,         false },
  { "XF86AudioPause",        DALI_KEY_PAUSE_CD,        false },
  { "XF86AudioNext",         DALI_KEY_NEXT_SONG,       false },
  { "XF86AudioPrev",         DALI_KEY_PREVIOUS_SONG,   false },
  { "XF86AudioRewind",       DALI_KEY_REWIND,          false },
  { "XF86AudioForward",      DALI_KEY_FASTFORWARD,     false },
  { "XF86AudioMedia",        DALI_KEY_MEDIA,           false },
  { "XF86AudioPlayPause",    DALI_KEY_PLAY_PAUSE,      false },
  { "XF86AudioMute",         DALI_KEY_MUTE,            false },
  { "XF86Menu",              DALI_KEY_MENU,            true  },
  { "XF86HomePage",          DALI_KEY_HOMEPAGE,        false },
  { "XF86WWW",               DALI_KEY_WEBPAGE,         false },
  { "XF86Mail",              DALI_KEY_MAIL,            false },
  { "XF86ScreenSaver",       DALI_KEY_SCREENSAVER,     false },
  { "XF86MonBrightnessUp",   DALI_KEY_BRIGHTNESS_UP,   false },
  { "XF86MonBrightnessDown", DALI_KEY_BRIGHTNESS_DOWN, false },
  { "XF86SoftKBD",           DALI_KEY_SOFT_KBD,        false },
  { "XF86QuickPanel",        DALI_KEY_QUICK_PANEL,     false },
  { "XF86TaskPane",          DALI_KEY_TASK_SWITCH,     false },
  { "XF86Apps",              DALI_KEY_APPS,            false },
  { "XF86Search",            DALI_KEY_SEARCH,          false },
  { "XF86Voice",             DALI_KEY_VOICE,           false },
  { "Hangul",                DALI_KEY_LANGUAGE,        false },
  { "XF86AudioRaiseVolume",  DALI_KEY_VOLUME_UP,       true  },
  { "XF86AudioLowerVolume",  DALI_KEY_VOLUME_DOWN,     true  },

  { "BackSpace",             DALI_KEY_BACKSPACE,       false },
  { "Left",                  DALI_KEY_CURSOR_LEFT,     false },
  { "Right",                 DALI_KEY_CURSOR_RIGHT,    false }
};

const std::size_t KEY_LOOKUP_COUNT = (sizeof( TestKeyLookupTable))/ (sizeof(KeyLookup));

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

#ifndef DALI_PROFILE_UBUNTU
  void GrabAllKeys()
  {
    for ( std::size_t i = 0; i < KEY_LOOKUP_COUNT; ++i )
    {
      bool ret = KeyGrab::GrabKey( mApplication.GetWindow(), TestKeyLookupTable[i].daliKeyCode, KeyGrab::TOPMOST );
      cout << i << " GrabKey TOPMOST " << TestKeyLookupTable[i].keyName << " returns " << ret << endl;
    }
    cout << endl;
  }

  void UngrabAllKeys()
  {
    for ( std::size_t i = 0; i < KEY_LOOKUP_COUNT; ++i )
    {
      bool ret = KeyGrab::UngrabKey( mApplication.GetWindow(), TestKeyLookupTable[i].daliKeyCode );
      cout << i << " UnGrabKey " << TestKeyLookupTable[i].keyName << " returns " << ret << endl;
    }
    cout << endl;
  }
#endif

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    std::cout << "HelloWorldController::Create" << std::endl;

    mGrabbing = false;

    TextLabel label = TextLabel::New("Key Grab All Test");
    label.SetParentOrigin(ParentOrigin::CENTER);
    label.SetProperty(TextLabel::Property::TEXT_COLOR, Vector4(1,1,1,1));
    Stage::GetCurrent().Add(label);

    Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
    Stage::GetCurrent().KeyEventSignal().Connect( this, &HelloWorldController::OnKeyEvent );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    if(touch.GetPoint(0).state == TouchPoint::Finished)
    {
      if(!mGrabbing)
      {
        Window window = mApplication.GetWindow();
#ifndef DALI_PROFILE_UBUNTU
        GrabAllKeys();
#endif
      }
      else
      {
        Window window = mApplication.GetWindow();
#ifndef DALI_PROFILE_UBUNTU
        UngrabAllKeys();
#endif
      }
      mGrabbing = !mGrabbing;
    }
    return true;
  }

  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
    }
  }

private:
  Application&  mApplication;
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

