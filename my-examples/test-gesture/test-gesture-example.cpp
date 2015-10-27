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

    Actor grandchild = ImageActor::New(ResourceImage::New(DALI_IMAGE_DIR "gallery-small-1.jpg"));
    grandchild.SetParentOrigin(ParentOrigin::CENTER);
    grandchild.SetAnchorPoint(AnchorPoint::CENTER);
    Stage::GetCurrent().Add(grandchild);
    mGrandChild = grandchild;

    mDetector = PanGestureDetector::New();
    mDetector.Attach(mGrandChild);
    mDetector.DetectedSignal().Connect(this, &HelloWorldController::OnPan);

    pressDetector = LongPressGestureDetector::New();
    pressDetector.Attach(mGrandChild);
    pressDetector.DetectedSignal().Connect(this, &HelloWorldController::OnLongPress);
  }

  void OnPan(Actor actor, const PanGesture& pan)
  {
    cout << "pan displacement " << pan.displacement << endl;
  }

  void OnLongPress(Actor actor, const LongPressGesture& press)
  {
    cout << "long press" << endl;
  }

private:
  Application&  mApplication;
  Animation mAni;
  Actor mGrandChild;
  PanGestureDetector mDetector;
  LongPressGestureDetector pressDetector;
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
