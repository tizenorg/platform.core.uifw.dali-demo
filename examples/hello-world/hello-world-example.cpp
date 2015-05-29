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

#include <dali-toolkit/dali-toolkit.h>
#include <string>

using namespace Dali;
using namespace std;

namespace
{
const char* const DEFAULT_BACKGROUND_PATH = DALI_IMAGE_DIR"background-3.jpg";
const char* const DEFAULT_IMAGE_PATH = DALI_IMAGE_DIR"gallery-large-1.jpg";

const char* const DEFAULT_VERTEX_SHADER = "";
}

// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application )
  {
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
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );

    ImageActor background = ImageActor::New(ResourceImage::New(DEFAULT_BACKGROUND_PATH));
    background.SetAnchorPoint(AnchorPoint::CENTER);
    background.SetParentOrigin(ParentOrigin::CENTER);
    background.SetZ(-0.1f);
    stage.Add(background);

    string fragmentShader = DALI_COMPOSE_SHADER(
      precision mediump float;                                                           \n
                                                                                         \n
      uniform float uRadius;                                                             \n
      uniform float uOpacity;                                                            \n
      uniform struct Border                                                              \n
      {                                                                                  \n
        float width;                                                                     \n
        vec4  color;                                                                     \n
      } uBorder;                                                                         \n
                                                                                         \n
      void main()                                                                        \n
      {                                                                                  \n
        vec2 center = vec2(0.5, 0.5);                                                    \n
        float distance = length(vTexCoord - center);                                     \n
        float halfWidth = uBorder.width * 0.5;                                           \n
        float minRadius = uRadius - halfWidth;                                           \n
        float maxRadius = uRadius + halfWidth;                                           \n
        float offset = halfWidth * 0.3;                                                  \n
                                                                                         \n
        if (maxRadius < distance)                                                        \n
        {                                                                                \n
          discard;                                                                       \n
        }                                                                                \n
        else if (minRadius <= distance && distance <= uRadius)                           \n
        {                                                                                \n
          float ratio = smoothstep(minRadius, uRadius - offset, distance);               \n
          if (uOpacity > 0.0)                                                            \n
          {                                                                              \n
            vec4 textureColor = vec4(texture2D(sTexture, vTexCoord).rgb, uOpacity);      \n 
            gl_FragColor = mix(textureColor, uBorder.color, ratio);                      \n
          }                                                                              \n
          else                                                                           \n
          {                                                                              \n
            gl_FragColor = vec4(uBorder.color.rgb, clamp(ratio, 0.0, uBorder.color.a));  \n
          }                                                                              \n
        }                                                                                \n
        else if (uRadius < distance && distance < maxRadius)                             \n
        {                                                                                \n
          float ratio = 1.0 - smoothstep(uRadius + offset, maxRadius, distance);         \n
          gl_FragColor = vec4(uBorder.color.rgb, clamp(ratio, 0.0, uBorder.color.a));    \n
        }                                                                                \n
        else                                                                             \n
        {                                                                                \n
          gl_FragColor = vec4(texture2D(sTexture, vTexCoord).rgb, uOpacity);             \n
        }                                                                                \n
      }                                                                                  \n
    );

    ShaderEffect circleShaderEffect = ShaderEffect::New(DEFAULT_VERTEX_SHADER, fragmentShader);
    circleShaderEffect.SetUniform("uRadius", 0.05f);
    circleShaderEffect.SetUniform("uOpacity", 0.0f);
    circleShaderEffect.SetUniform("uBorder.width", 0.02f);
    circleShaderEffect.SetUniform("uBorder.color", Vector4(0.0f, 0.0f, 0.0f, 1.0f));

    ResourceImage image = ResourceImage::New(DEFAULT_IMAGE_PATH);
    ImageActor actor = ImageActor::New(image);
    actor.SetAnchorPoint(AnchorPoint::CENTER);
    actor.SetParentOrigin(ParentOrigin::CENTER);
    actor.SetSize(300.0f, 300.0f);
    actor.SetBlendMode(BlendingMode::ON);
    actor.SetShaderEffect(circleShaderEffect);
    stage.Add(actor);


    Animation animation = Animation::New(4.0f);
    animation.SetLooping(true);
    animation.AnimateTo(Property(circleShaderEffect, "uRadius"), 0.49f, AlphaFunction::BOUNCE);
    animation.AnimateTo(Property(circleShaderEffect, "uOpacity"), 1.0f, AlphaFunction::BOUNCE);
    animation.AnimateTo(Property(circleShaderEffect, "uBorder.width"), 0.01f, AlphaFunction::BOUNCE);
    animation.AnimateTo(Property(circleShaderEffect, "uBorder.color"), Color::WHITE, AlphaFunction::BOUNCE);
    animation.Play();
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
  ImageActor mCircle;
};

void RunTest( Application& application )
{
  HelloWorldController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
