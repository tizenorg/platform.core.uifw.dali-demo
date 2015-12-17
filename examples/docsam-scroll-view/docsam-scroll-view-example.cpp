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
  // Create a ScrollView instance
  ScrollView scrollView = ScrollView::New();
  scrollView.SetParentOrigin( ParentOrigin::CENTER );
  scrollView.SetOrientation( Degree(90.), Vector3(0,0,1) );
  Stage::GetCurrent().Add(scrollView);

  // Set the size of scrollView; it covers the entire stage 
  Size size = Stage::GetCurrent().GetSize();
  scrollView.SetSize(size);

  // Add actors to the ScrollView 
  for( int i=0; i < MY_ITEM_COUNT; ++i )
  {
    ImageView imageView = ImageView::New( MY_IMAGE_PATHS[i] );
    imageView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    imageView.SetPosition( i * size.x, 0.0f );
    scrollView.Add( imageView );
  }

  // The ScrollView contents are now draggable

  // To enforce horizontal-only scrolling, the Y axis ruler can be disabled 
  RulerPtr rulerY = new DefaultRuler();
  rulerY->Disable();
  scrollView.SetRulerY( rulerY );

  // To enable snapping, a FixedRuler can be applied to the X axis,
  // with snap points spaced to the width of the stage. 
  RulerPtr rulerX1 = new FixedRuler( size.width );
  scrollView.SetRulerX( rulerX1 );

  // A domain can be applied to rulers to prevent scrolling beyond this boundary
  // In this case, to 4 times the width of the stage, allowing for 4 pages to be scrolled
  RulerPtr rulerX2 = new FixedRuler( size.width );
  rulerX2->SetDomain( RulerDomain( 0.0f, size.width*4.0f ) );
  scrollView.SetRulerX( rulerX2 );

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
