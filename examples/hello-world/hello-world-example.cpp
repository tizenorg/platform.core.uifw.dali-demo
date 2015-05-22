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

  Dali::ShaderEffect NewEffect()
  {
    std::string fragmentShader = DALI_COMPOSE_SHADER(
      precision highp float;

      uniform float uFanAngle;

      void main()
      {
        vec2 coord        = vec2(vTexCoord.x, vTexCoord.y);
        vec2 centerCoord  = vec2(0.5, 1.0);

        vec2 diffCoord    = coord - centerCoord;
        float angle       = 90.0 - abs(degrees(atan(diffCoord.y, abs(diffCoord.x))));

        float len = length(diffCoord);
        if (len < 0.49 || len > 0.5)
        {
          discard;
        }

        if (angle > uFanAngle)
        {
          discard;
        }

        float level = mix(1.0, 0.53, angle / uFanAngle);
        gl_FragColor = uColor * level;
      }
    );

    Dali::ShaderEffect shaderEffect = Dali::ShaderEffect::New( "", fragmentShader,
                                                         Dali::GeometryType( Dali::GEOMETRY_TYPE_IMAGE ),
                                                         Dali::ShaderEffect::GeometryHints( Dali::ShaderEffect::HINT_GRID ) );
    return shaderEffect;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    ImageActor bgActor = CreateSolidColorActor(Color::BLUE);
    bgActor.SetSize(251.f, 251.f);
    bgActor.SetParentOrigin(ParentOrigin::CENTER);
    bgActor.SetAnchorPoint(AnchorPoint::BOTTOM_CENTER);
    stage.Add(bgActor);

    ImageActor actor = CreateSolidColorActor(Color::WHITE);
    actor.SetColor(Color::WHITE);
    actor.SetSize(250.f, 250.f);
    actor.SetParentOrigin(ParentOrigin::CENTER);
    actor.SetAnchorPoint(AnchorPoint::BOTTOM_CENTER);
    actor.SetOrientation(Quaternion(Radian(Degree(-90.f)), Vector3::ZAXIS));
    actor.SetZ(1.f);
    stage.Add(actor);

    ShaderEffect effect = NewEffect();
    effect.SetUniform("uFanAngle", 0.f);
    actor.SetShaderEffect(effect);

    Animation anim = Animation::New(3.f);
    anim.SetLooping(true);
    anim.AnimateBy(Property(actor, Actor::Property::ORIENTATION), Quaternion(Radian(Degree(-180.f)), Vector3::ZAXIS), AlphaFunction::LINEAR);
    anim.AnimateTo(Property(effect, "uFanAngle"), 45.f, AlphaFunction::BOUNCE);
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
