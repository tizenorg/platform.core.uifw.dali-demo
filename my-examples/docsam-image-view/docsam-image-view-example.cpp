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

const int MY_ITEM_COUNT = 5;

const char* MY_IMAGE_PATHS[] = {
    DALI_IMAGE_DIR "gallery-medium-1.jpg",
    DALI_IMAGE_DIR "gallery-medium-2.jpg",
    DALI_IMAGE_DIR "gallery-medium-3.jpg",
    DALI_IMAGE_DIR "gallery-medium-4.jpg",
    DALI_IMAGE_DIR "gallery-medium-5.jpg",
};

class HelloWorldExample : public ConnectionTracker
{
public:
  HelloWorldExample( Application& application ): mApplication( application )
  {
    mApplication.InitSignal().Connect( this, &HelloWorldExample::Create );
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
    return true;
  }

  void Create( Application& application );

private:
  Application&  mApplication;
};

void HelloWorldExample::Create( Application& application )
{
  ImageView parent = ImageView::New( DALI_IMAGE_DIR "gallery-medium-5.jpg" );
  parent.SetParentOrigin( ParentOrigin::CENTER );
  parent.TranslateBy( Vector3( -50, 0, 0 ) );

  ImageView child = ImageView::New( DALI_IMAGE_DIR "gallery-medium-4.jpg" );
  child.SetParentOrigin( ParentOrigin::CENTER );
  child.TranslateBy( Vector3( 100, 20, 0 ) );

  parent.Add( child );
  Stage::GetCurrent().Add( parent );

  // REMOVE
  Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect( this, &HelloWorldExample::OnTouch );
  Stage::GetCurrent().KeyEventSignal().Connect(this, &HelloWorldExample::OnKeyEvent);
}

int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  HelloWorldExample test( application );
  application.MainLoop();
  return 0;
}
