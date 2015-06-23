/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

const char * const DIGITAL_01_IMAGE( DALI_IMAGE_DIR "Digital_01.png" );
const char * const DIGITAL_02_IMAGE( DALI_IMAGE_DIR "Digital_02.png" );

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

  ShaderEffect NewEffect()
  {
    std::string vertexShader = DALI_COMPOSE_SHADER(
      precision mediump float;                                         \n
      uniform   float  uAngle;                                         \n

      varying   mediump vec2  vTexCoordRotated;                        \n
                                                                       \n
      void main()                                                      \n
      {                                                                \n
        gl_Position = uMvpMatrix * vec4( aPosition, 1.0 );             \n
        vTexCoord = aTexCoord;                                         \n

        float rad = radians(uAngle);                                   \n
        mediump mat2 rotMatrix = mat2(cos(rad), -sin(rad), sin(rad), cos(rad));  \n

        mediump vec2 tempCoord = (aTexCoord - 0.5) * 2.0;              \n
        tempCoord = rotMatrix * tempCoord;                             \n
        vTexCoordRotated = tempCoord / 2.0 + 0.5;                      \n
      }                                                                \n
    );

    std::string fragmentShader = DALI_COMPOSE_SHADER(                  \n
      varying   mediump vec2  vTexCoordRotated;                        \n
                                                                       \n
      void main()                                                      \n
      {                                                                \n
        mediump vec4 texColor = texture2D(sTexture, vTexCoordRotated); \n
        mediump vec4 effectColor = texture2D(sEffect, vTexCoord);      \n

        if (texColor.a * effectColor.a == 0.0)                         \n
        {                                                              \n
          discard;                                                     \n
        }                                                              \n

        gl_FragColor = texColor * effectColor * uColor;                \n
      }                                                                \n
    );

    ShaderEffect shaderEffect = ShaderEffect::New( vertexShader, fragmentShader,
                                                   GeometryType( Dali::GEOMETRY_TYPE_IMAGE ) );
    return shaderEffect;

  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    ImageActor actor = ImageActor::New(ResourceImage::New(DIGITAL_01_IMAGE));
    actor.SetParentOrigin(ParentOrigin::CENTER);
    actor.SetAnchorPoint(AnchorPoint::CENTER);
    stage.Add(actor);

    ShaderEffect effect = NewEffect();
    effect.SetEffectImage(ResourceImage::New(DIGITAL_02_IMAGE));
    effect.SetUniform("uAngle", 0.f);
    actor.SetShaderEffect(effect);

    Animation anim = Animation::New(4.f);
    anim.SetLooping(true);
    anim.AnimateTo(Property(effect, "uAngle"), 360.f);
    anim.Play();

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
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

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
