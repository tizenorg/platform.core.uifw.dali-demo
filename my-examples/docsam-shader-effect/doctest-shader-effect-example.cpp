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

// Simple vertex shader code
const char* SIMPLE_VERTEX_SHADER = DALI_COMPOSE_SHADER(
  void main()
  {
    vec4 pos = uProjection * uModelView * vec4(aPosition, 1.0);
    gl_Position = vec4(pos.x + 50., pos.y - 50., pos.zw);
    vTexCoord = aTexCoord;
  }
);

// Simple vertex shader code
const char* SIMPLE_ANIMATE_VERTEX_SHADER = DALI_COMPOSE_SHADER(
  uniform   float uOffset;
  void main()
  {
    vec4 pos = uProjection * uModelView * vec4(aPosition, 1.0);
    gl_Position = vec4(pos.x + uOffset, pos.y, pos.zw);
    vTexCoord = aTexCoord;
  }
);

const char* MY_IMAGE_PATH = DALI_IMAGE_DIR "blocks-ball.png";

void HelloWorldExample::Create( Application& application )
{
  if(1) // SIMPLE_VERTEX_SHADER
  {
  // Create an ImageActor
  ImageActor imageActor = ImageActor::New( ResourceImage::New( MY_IMAGE_PATH ) );
  Stage::GetCurrent().Add( imageActor );

  // Create a ShaderEffect. Use the default pixel shader.
  ShaderEffect shaderEffect =  ShaderEffect::New( SIMPLE_VERTEX_SHADER, "" );
  imageActor.SetShaderEffect( shaderEffect );
  }

  if(0) // SIMPLE_ANIMATE_VERTEX_SHADER
  {
  // Create an ImageActor
  ImageActor imageActor = ImageActor::New( ResourceImage::New( MY_IMAGE_PATH ) );
  Stage::GetCurrent().Add( imageActor );

  // Create a ShaderEffect. Use the default pixel shader.
  ShaderEffect shaderEffect =  ShaderEffect::New( SIMPLE_ANIMATE_VERTEX_SHADER, "" );
  imageActor.SetShaderEffect( shaderEffect );

  // Create an Animation for the shaderEffect
  Animation animation = Animation::New(1.0f);

  // Set the initial value for the uniform
  shaderEffect.SetUniform("uOffset", 100.0f);

  // Animate the uniform to a target value
  animation.AnimateTo( Property( shaderEffect, "uOffset"), 1000.0f);
  animation.Play();
  }

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
