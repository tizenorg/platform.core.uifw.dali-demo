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

    Stage::GetCurrent().GetRootLayer().SetDepthTestDisabled(true);

    //bool useImageView = false;
    bool useImageView = true;

    Actor parent;
    if(useImageView)
      parent = ImageView::New(ResourceImage::New(DALI_IMAGE_DIR "gallery-large-2.jpg"));
    else
      parent = ImageActor::New(ResourceImage::New(DALI_IMAGE_DIR "gallery-large-2.jpg"));
    parent.SetParentOrigin(ParentOrigin::CENTER);
    parent.SetAnchorPoint(AnchorPoint::CENTER);
    Stage::GetCurrent().Add(parent);

    Actor child;
    if(useImageView)
      child = ImageView::New(ResourceImage::New(DALI_IMAGE_DIR "music-libray-the-solars.png"));
    else
      child = ImageActor::New(ResourceImage::New(DALI_IMAGE_DIR "music-libray-the-solars.png"));
    child.SetParentOrigin(ParentOrigin::CENTER);
    child.SetAnchorPoint(AnchorPoint::CENTER);
    parent.Add(child);

    Actor grandchild;
    if(useImageView)
      grandchild = ImageView::New(ResourceImage::New(DALI_IMAGE_DIR "gallery-small-1.jpg"));
    else
      grandchild = ImageActor::New(ResourceImage::New(DALI_IMAGE_DIR "gallery-small-1.jpg"));
    grandchild.SetParentOrigin(ParentOrigin::CENTER);
    grandchild.SetAnchorPoint(AnchorPoint::CENTER);
    child.Add(grandchild);


    ////////////
    // SetSize, SetSizeMode

    //child.SetSize(100,400);

    //child.SetSize(Vector2(100,400));
    //child.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);

    ////////////
    // SetResizePolicy

    ////
    // USE_ASSIGNED_SIZE

    //child.SetSize(100,400);
    //child.SetSize(Vector2(100,400));
    //child.SetResizePolicy(ResizePolicy::USE_ASSIGNED_SIZE, Dimension::ALL_DIMENSIONS);
    ////child.SetResizePolicy(ResizePolicy::USE_ASSIGNED_SIZE, Dimension::WIDTH);

    ////
    // combinations

    child.SetSize(Vector2(200,400));

    //child.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
    //child.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);
    //child.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    //child.SetResizePolicy(ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS);

    //child.SetResizePolicy(ResizePolicy::FIXED, Dimension::WIDTH);
    //child.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::WIDTH);
    //child.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
    child.SetResizePolicy(ResizePolicy::FIT_TO_CHILDREN, Dimension::WIDTH);

    ////////////
    // SetSizeScalePolicy

    //child.SetSize(Vector2(200,400));

    ////
    // ResizePolicy - FIXED

    //child.SetResizePolicy(FIXED, ALL_DIMENSIONS);
    ////child.SetSizeScalePolicy(FIT_WITH_ASPECT_RATIO);
    //child.SetSizeScalePolicy(FILL_WITH_ASPECT_RATIO);

    //child.SetResizePolicy(FIXED, WIDTH);
    ////child.SetSizeScalePolicy(FIT_WITH_ASPECT_RATIO);
    //child.SetSizeScalePolicy(FILL_WITH_ASPECT_RATIO);

    //child.SetResizePolicy(FIXED, HEIGHT);
    ////child.SetSizeScalePolicy(FIT_WITH_ASPECT_RATIO);
    //child.SetSizeScalePolicy(FILL_WITH_ASPECT_RATIO);


    ////
    // ResizePolicy - FILL_TO_PARENT

    //child.SetResizePolicy(FILL_TO_PARENT, ALL_DIMENSIONS);
    ////child.SetSizeScalePolicy(FIT_WITH_ASPECT_RATIO);
    //child.SetSizeScalePolicy(FILL_WITH_ASPECT_RATIO);

    //child.SetResizePolicy(FILL_TO_PARENT, WIDTH);
    ////child.SetSizeScalePolicy(FIT_WITH_ASPECT_RATIO);
    //child.SetSizeScalePolicy(FILL_WITH_ASPECT_RATIO);

    //child.SetResizePolicy(FILL_TO_PARENT, HEIGHT);
    ////child.SetSizeScalePolicy(FIT_WITH_ASPECT_RATIO);
    //child.SetSizeScalePolicy(FILL_WITH_ASPECT_RATIO);


    ////
    // ResizePolicy - FIT_TO_CHILDREN

    //child.SetResizePolicy(FIT_TO_CHILDREN, ALL_DIMENSIONS);
    ////child.SetSizeScalePolicy(FIT_WITH_ASPECT_RATIO);
    //child.SetSizeScalePolicy(FILL_WITH_ASPECT_RATIO);

    //child.SetResizePolicy(FIT_TO_CHILDREN, WIDTH);
    ////child.SetSizeScalePolicy(FIT_WITH_ASPECT_RATIO);
    //child.SetSizeScalePolicy(FILL_WITH_ASPECT_RATIO);

    //child.SetResizePolicy(FIT_TO_CHILDREN, HEIGHT);
    ////child.SetSizeScalePolicy(FIT_WITH_ASPECT_RATIO);
    //child.SetSizeScalePolicy(FILL_WITH_ASPECT_RATIO);



    ////////////
    // SetPadding
    //child.SetPadding(Padding(10,20,40,80));
    //child.SetResizePolicy(FILL_TO_PARENT, ALL_DIMENSIONS);


    //////////////
    //// Control bug
    //Control parent = Control::New();
    //parent.SetParentOrigin(ParentOrigin::CENTER);
    //parent.SetAnchorPoint(AnchorPoint::CENTER);
    //parent.SetBackgroundColor(Vector4(1,1,1,1));
    //parent.SetSize(500,400);
    //parent.SetRelayoutEnabled(false);
    //Stage::GetCurrent().Add(parent);



    ////////
    // animation test
    mAni = Animation::New(2.);
    mAni.AnimateBy(Property(parent, Actor::Property::POSITION_X), 200.f);
    mAni.AnimateTo(Property(parent, Actor::Property::SCALE), Vector3(1.5,1.5,1.5));
    mAni.SetLooping(true);

    // Respond to a click anywhere on the stage
    Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    mAni.Play();
    // quit the application
    //mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
  Animation mAni;
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
