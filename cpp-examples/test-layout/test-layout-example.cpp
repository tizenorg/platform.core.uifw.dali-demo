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

    Actor parent = ImageActor::New(ResourceImage::New(DALI_IMAGE_DIR "gallery-large-1.jpg"));
    parent.SetParentOrigin(ParentOrigin::CENTER);
    parent.SetAnchorPoint(AnchorPoint::CENTER);
    //parent.SetSize(300,300);
    Stage::GetCurrent().Add(parent);

    Actor child = ImageActor::New(ResourceImage::New(DALI_IMAGE_DIR "music-libray-the-solars.png"));
    child.SetParentOrigin(ParentOrigin::CENTER);
    child.SetAnchorPoint(AnchorPoint::CENTER);
    //child.SetSize(100,400);
    parent.Add(child);

    /////////////////////////////////////////////////////
    // alignment

    ///////////////////////////
    // Actor

    //// left aligned
    //child.SetParentOrigin(ParentOrigin::CENTER_LEFT);
    //child.SetAnchorPoint(ParentOrigin::CENTER_LEFT);

    //// bottom-right aligned
    //child.SetParentOrigin(ParentOrigin::BOTTOM_RIGHT);
    //child.SetAnchorPoint(ParentOrigin::BOTTOM_RIGHT);

    ///////////////////////////
    // Toolkit::Alignment

    //Alignment align = Alignment::New();
    //align.SetResizePolicy(FILL_TO_PARENT, ALL_DIMENSIONS);
    //align.SetParentOrigin(ParentOrigin::CENTER);
    //align.SetAnchorPoint(AnchorPoint::CENTER);

    ////// left aligned
    ////align.SetAlignmentType(Alignment::HorizontalLeft);
    
    //// bottom-right aligned
    //align.SetAlignmentType( Alignment::Type(Alignment::HorizontalRight | Alignment::VerticalBottom) );

    //parent.Remove(child);
    //align.Add(child);
    //parent.Add(align);


    /////////////////////////////////////////////////////
    // resizing

    /////////////////////////////
    //// ImageAttribute
    //ImageAttributes imgAttr;
    //imgAttr.SetSize(50,50);
    ////imgAttr.SetScalingMode(ImageAttributes::ShrinkToFit);
    ////imgAttr.SetScalingMode(ImageAttributes::ScaleToFill);
    ////imgAttr.SetScalingMode(ImageAttributes::FitWidth);
    ////imgAttr.SetScalingMode(ImageAttributes::FitHeight);
    //ImageActor::DownCast(parent).SetImage(ResourceImage::New(DALI_IMAGE_DIR "gallery-large-1.jpg", imgAttr));
    
    /////////////////////////////
    //// Actor
    //child.SetResizePolicy(FILL_TO_PARENT, ALL_DIMENSIONS);

    ///////////////////////////
    // Toolkit::Alignment

    //Alignment align = Alignment::New();
    //align.SetResizePolicy(FILL_TO_PARENT, ALL_DIMENSIONS);
    //align.SetParentOrigin(ParentOrigin::CENTER);
    //align.SetAnchorPoint(AnchorPoint::CENTER);

    ////parent.SetRelayoutEnabled(false);
    ////child.SetRelayoutEnabled(false);
    ////align.SetRelayoutEnabled(false);

    ////align.SetScaling(Alignment::ScaleToFill);
    ////align.SetScaling(Alignment::ScaleToFitKeepAspect);
    //align.SetScaling(Alignment::ScaleToFillKeepAspect);
    ////align.SetScaling(Alignment::ShrinkToFit);
    ////align.SetScaling(Alignment::ShrinkToFitKeepAspect);
    
    //parent.Remove(child);
    //align.Add(child);
    //parent.Add(align);


    /////////////////////////////////////////////////////
    // cropping


    /////////////////////////////////////////////////////
    // padding

    ///////////////////////////
    // Actor

    //// left aligned
    //child.SetParentOrigin(ParentOrigin::CENTER_LEFT);
    //child.SetAnchorPoint(ParentOrigin::CENTER_LEFT);

    //// fill
    //child.SetResizePolicy(FILL_TO_PARENT, ALL_DIMENSIONS);

    //// padding
    //child.SetPadding(Padding(10,20,30,40));

    ///////////////////////////
    // Toolkit::Alignment

    //Alignment align = Alignment::New();
    //align.SetResizePolicy(FILL_TO_PARENT, ALL_DIMENSIONS);
    //align.SetParentOrigin(ParentOrigin::CENTER);
    //align.SetAnchorPoint(AnchorPoint::CENTER);

    ////// left aligned
    ////align.SetAlignmentType(Alignment::HorizontalLeft);
    
    //// bottom-right aligned
    //align.SetAlignmentType( Alignment::Type(Alignment::HorizontalRight | Alignment::VerticalBottom) );

    //// padding
    //align.SetPadding(Alignment::Padding(10,20,30,40));
    
    //parent.Remove(child);
    //align.Add(child);
    //parent.Add(align);




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
