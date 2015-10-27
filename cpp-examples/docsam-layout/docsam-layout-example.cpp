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
  HelloWorldExample( Application& application ): mApplication( application )
  {
    mApplication.InitSignal().Connect( this, &HelloWorldExample::Create );
  }

  void Create( Application& application )
  {
    std::cout << "HelloWorldExample::Create" << std::endl;
    Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect( this, &HelloWorldExample::OnTouch );
    Stage::GetCurrent().KeyEventSignal().Connect(this, &HelloWorldExample::OnKeyEvent);

    if(0)
    {
      Control control = Control::New();
      control.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      control.SetBackgroundColor( Color::BLUE );
      control.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      control.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );
      Stage::GetCurrent().Add( control );

      ImageView imageView = ImageView::New( DALI_IMAGE_DIR "gallery-small-1.jpg" );
      imageView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      imageView.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
      control.Add( imageView );
    }

    if(0) // DIMENSION_DEPENDENCY
    {
      control = Control::New();
      control.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      control.SetBackgroundColor( Color::BLUE );
      control.SetSize( 200, 200 );
      Stage::GetCurrent().Add( control );

      TextLabel textLabel = TextLabel::New("Example");
      textLabel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      textLabel.SetBackgroundColor( Color::WHITE );
      textLabel.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      textLabel.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
      control.Add( textLabel );
    }

    if(1) // one dim FIXED
    {
      control = Control::New();
      control.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      control.SetBackgroundColor( Color::BLUE );
      control.SetSize( 200, 200 );
      Stage::GetCurrent().Add( control );

      TextLabel textLabel = TextLabel::New("Example");
      textLabel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      textLabel.SetBackgroundColor( Color::WHITE );
      textLabel.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      textLabel.SetResizePolicy( ResizePolicy::FIXED, Dimension::HEIGHT );
      control.Add( textLabel );

      textLabel.SetSize( 100, 100 );
    }

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
    control.SetSize( 300, 300 );
    return true;
  }

private:
  Application&  mApplication;
  Control control;
};

int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  HelloWorldExample test( application );
  application.MainLoop();
  return 0;
}
