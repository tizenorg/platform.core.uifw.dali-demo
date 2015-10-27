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
#include "image-region-effect.h"

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

class TestController : public ConnectionTracker
{
public:
  TestController( Application& application ): mApplication( application )
  {
    mApplication.InitSignal().Connect( this, &TestController::Create );
  }

  void Create( Application& application )
  {
    std::cout << "TestController::Create" << std::endl;
    Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect( this, &TestController::OnTouch );
    Stage::GetCurrent().KeyEventSignal().Connect(this, &TestController::OnKeyEvent);

    Stage::GetCurrent().GetRootLayer().SetBehavior(Layer::LAYER_3D);

    ImageActor a1 = ImageActor::New(ResourceImage::New(DALI_IMAGE_DIR "gallery-small-1.jpg"));

    //Dali::ShaderEffect mEffect = ClusterHome3D::ImageRegionEffect::Create();
    //ClusterHome3D::ImageRegionEffect::SetBottomRight(mEffect, Vector2(1.0f, 1.0f));

    //a1.SetShaderEffect(mEffect);

    a1.SetParentOrigin(ParentOrigin::CENTER);
    a1.SetZ(-0.2f);
    //a1.SetOpacity(1.0f);
    Stage::GetCurrent().Add(a1);
		a1.SetOpacity(0.8);

    ImageActor a2 = CreateSolidColorActor(Vector4(1,0,0,.5));
    a2.SetSize(400,50);
    a2.SetZ(-0.19f);
    //a2.SetOpacity(1.0f);
    //a2.SetOpacity(.8f);
    Stage::GetCurrent().Add(a2);
		a1.SetOpacity(0.8);
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

private:
  Application&  mApplication;
};

int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  TestController test( application );
  application.MainLoop();
  return 0;
}
