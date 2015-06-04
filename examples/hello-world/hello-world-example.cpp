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

using namespace Dali;
using namespace Toolkit;

const char * const HEART_IMAGE(DALI_IMAGE_DIR "heart.png");
const char * const LIGHT_IMAGE(DALI_IMAGE_DIR "light.png");

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
    std::string fragmentShader = DALI_COMPOSE_SHADER(
      precision highp float;

      uniform float uOffset;
      uniform float uWidth;

      void main()
      {
        float alpha = 1.0;

        if (vTexCoord.x > uOffset / uWidth)
        {
          alpha = 0.3;
        }

        mediump vec4 color = texture2D(sTexture, vTexCoord) * uColor;
        gl_FragColor = vec4(color.rgb, color.a * alpha);
      }
    );

    ShaderEffect effect = ShaderEffect::New("", fragmentShader, GEOMETRY_TYPE_IMAGE, ShaderEffect::HINT_GRID);
    effect.SetUniform("uOffset", 0.f);

    return effect;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();

    // Create heart actor
    ImageDimensions heartDimension = ResourceImage::GetImageSize(HEART_IMAGE);
    float heartWidth = heartDimension.GetWidth();
    ImageActor heartAactor = ImageActor::New(ResourceImage::New(HEART_IMAGE));
    heartAactor.SetParentOrigin(ParentOrigin::CENTER);
    heartAactor.SetAnchorPoint(AnchorPoint::CENTER);
    stage.Add(heartAactor);

    // Create light actor
    ImageActor lightActor = ImageActor::New(ResourceImage::New(LIGHT_IMAGE));
    lightActor.SetParentOrigin(ParentOrigin::CENTER_LEFT);
    lightActor.SetAnchorPoint(AnchorPoint::CENTER);
    lightActor.SetZ(1.f);
    heartAactor.Add(lightActor);

    // Create effect and set its property
    mEffect = NewEffect();
    mEffect.SetUniform("uWidth", heartWidth);
    heartAactor.SetShaderEffect(mEffect);

    // Play animation
    Animation anim = Animation::New(3.f);
    anim.SetLooping(true);
    anim.AnimateTo(Property(lightActor, Actor::Property::POSITION_X), heartWidth, AlphaFunction::BOUNCE);
    anim.AnimateTo(Property(mEffect, "uOffset"), heartWidth, AlphaFunction::BOUNCE);
    anim.Play();

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

  ShaderEffect  mEffect;
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
