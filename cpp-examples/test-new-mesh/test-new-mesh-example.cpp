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

    /////////////////////////
    // setting
    enum ActorMode
    {
      IMAGEACTORS,
      CONTROLS,
      TEXTLABELS,
      CONTROLS_IMAGEACTOR,
      TEXTLABELS_IMAGEACTOR,
    };

    ActorMode actorMode = IMAGEACTORS;
    //ActorMode actorMode = CONTROLS;

    //float a2_z = 1.f;
    float a2_z = -1.f;

    /////////////////////////

    Layer layer = Layer::New();

    layer.SetBehavior(Layer::LAYER_2D);
    //layer.SetBehavior(Layer::LAYER_3D);

    layer.SetParentOrigin(ParentOrigin::CENTER);
    layer.SetAnchorPoint(AnchorPoint::CENTER);
    Stage::GetCurrent().Add(layer);

    Actor a1, a2, a3;

    switch(actorMode)
    {
      case IMAGEACTORS:
      {
        a1 = ImageActor::New(ResourceImage::New(DALI_IMAGE_DIR "gallery-large-2.jpg"));
        a2 = ImageActor::New(ResourceImage::New(DALI_IMAGE_DIR "music-libray-the-solars.png"));
        a3 = ImageActor::New(ResourceImage::New(DALI_IMAGE_DIR "gallery-small-1.jpg"));

        //a2.SetSortModifier(-1);
        break;
      }
      case CONTROLS:
      {
        a1 = Control::New();  // red
        Control::DownCast(a1).SetBackgroundColor(Vector4(1,0,0,0));

        a2 = Control::New();  // green
        Control::DownCast(a2).SetBackgroundColor(Vector4(0,1,0,0));

        a3 = Control::New();  // blue
        Control::DownCast(a3).SetBackgroundColor(Vector4(0,0,1,0));
        break;
      }
      case TEXTLABELS:
      {
        a1 = TextLabel::New("a1");  // red
        Control::DownCast(a1).SetBackgroundColor(Vector4(1,0,0,0));

        a2 = TextLabel::New("a2");  // green
        Control::DownCast(a2).SetBackgroundColor(Vector4(0,1,0,0));

        a3 = TextLabel::New("a3");  // blue
        Control::DownCast(a3).SetBackgroundColor(Vector4(0,0,1,0));
        break;
      }
      case CONTROLS_IMAGEACTOR:
      {
        a1 = Control::New();  // red
        Control::DownCast(a1).SetBackgroundColor(Vector4(1,0,0,0));

        a2 = ImageActor::New(ResourceImage::New(DALI_IMAGE_DIR "music-libray-the-solars.png"));

        a3 = Control::New();  // blue
        Control::DownCast(a3).SetBackgroundColor(Vector4(0,0,1,0));
        break;
      }
      case TEXTLABELS_IMAGEACTOR:
      {
        a1 = TextLabel::New("a1");  // red
        Control::DownCast(a1).SetBackgroundColor(Vector4(1,0,0,0));

        a2 = ImageActor::New(ResourceImage::New(DALI_IMAGE_DIR "music-libray-the-solars.png"));

        a3 = TextLabel::New("a3");  // blue
        Control::DownCast(a3).SetBackgroundColor(Vector4(0,0,1,0));
        break;
      }
    }

    a2.SetZ(a2_z);

    a1.SetName("a1");
    a2.SetName("a2");
    a3.SetName("a3");

    a1.SetSize(400,400);
    a2.SetSize(200,200);
    a3.SetSize(100,100);

    a1.SetParentOrigin(ParentOrigin::CENTER);
    a1.SetAnchorPoint(AnchorPoint::CENTER);
    a2.SetParentOrigin(ParentOrigin::CENTER);
    a2.SetAnchorPoint(AnchorPoint::CENTER);
    a3.SetParentOrigin(ParentOrigin::CENTER);
    a3.SetAnchorPoint(AnchorPoint::CENTER);

    if(0)
    {
      layer.Add(a1);
      a1.Add(a2);
      a2.Add(a3);
    }
    if(1)
    {
      layer.Add(a1);
      a1.Add(a3);
      a3.Add(a2);
    }

    a1.TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
    a2.TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
    a3.TouchedSignal().Connect( this, &HelloWorldController::OnTouch );

    Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
    Stage::GetCurrent().KeyEventSignal().Connect(this, &HelloWorldController::OnKeyEvent);
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
    cout << actor.GetName() << endl;
    // quit the application
    //mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
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
