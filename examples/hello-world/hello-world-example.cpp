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

#include <locale.h>

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <stdio.h>

const char* BAR_IMAGE( DEMO_IMAGE_DIR "bar.png" );

using namespace Dali;
using namespace Toolkit;

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
    std::string fShader = DALI_COMPOSE_SHADER(
      precision highp float;                                                 \n
      uniform   float uHeight;                                               \n
                                                                             \n
      void main()                                                            \n
      {                                                                      \n
        if( vTexCoord.y < (1.0 - uHeight) )                                  \n
        {                                                                    \n
          discard;                                                           \n
        }                                                                    \n
                                                                             \n
        gl_FragColor = texture2D(sTexture, vTexCoord) * uColor;              \n
      }                                                                      \n
    );

    Dali::ShaderEffect shaderEffect = Dali::ShaderEffect::New( "", fShader );
    shaderEffect.SetUniform( "uHeight", 0.f );

    return shaderEffect;
  }


  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    stage.SetBackgroundColor( Color::WHITE );

    ImageActor actor = ImageActor::New( ResourceImage::New(BAR_IMAGE) );
    actor.SetParentOrigin( ParentOrigin::CENTER );
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    stage.Add( actor );

    ShaderEffect effect = NewEffect();
    actor.SetShaderEffect( effect );

    Animation anim = Animation::New( 3.f );
    anim.SetLooping( true );
    anim.AnimateTo( Property( effect, "uHeight" ), 1.f, AlphaFunction::BOUNCE );
    anim.Play();

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchData& touch )
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
int DALI_EXPORT_API main( int argc, char **argv )
{
  char *str = setlocale(LC_ALL, "fr_FR.UTF-8");


   if(str == NULL)
       printf("setlocale() cannot be honored");
   else
       printf("setlocale() return = %s \n", str);


  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
