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
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

#define GENERATE 0

const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-gradient.jpg" );
#if GENERATE
const char* GRID_IMAGE( DALI_IMAGE_DIR "home_grid_4x6.png" );
#else
const char* GRID_IMAGE( DALI_IMAGE_DIR "home_grid_4x6.jpg" );
#endif

const char* FRAGMENT_PREMULTIPLY_SHADER = DALI_COMPOSE_SHADER(
	void main()                                                  \n
	{                                                            \n
		mediump vec4 fragColor = texture2D(sTexture, vTexCoord);   \n
																															 \n
		fragColor.rgb *= fragColor.a;                              \n
		if (fragColor.r < 0.5 ||
				fragColor.g < 0.5 ||
				fragColor.b < 0.5)
		{
			fragColor.rgb = vec3(0.0, 0.0, 0.0);
		}                                                      \n
																															 \n
		gl_FragColor = vec4(fragColor.rgb, 1.0);                   \n
	}                                                            \n
);

const char* FRAGMENT_DISCARD_SHADER = DALI_COMPOSE_SHADER(
	void main()                                                  \n
	{                                                            \n
		mediump vec4 fragColor = texture2D(sTexture, vTexCoord);   \n
																															 \n
		if (fragColor.r < 0.5 ||
				fragColor.g < 0.5 ||
				fragColor.b < 0.5)                                    \n
		{
			discard;
		}
																															 \n
		gl_FragColor = vec4(fragColor.rgb, 1.0);                   \n
	}                                                            \n
);

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

		ImageActor actor = ImageActor::New(ResourceImage::New(BACKGROUND_IMAGE));
		actor.SetParentOrigin(ParentOrigin::CENTER);
		actor.SetAnchorPoint(AnchorPoint::CENTER);
		stage.Add(actor);

		ImageActor actor2 = ImageActor::New(ResourceImage::New(GRID_IMAGE));
		actor2.SetParentOrigin(ParentOrigin::CENTER);
		actor2.SetAnchorPoint(AnchorPoint::CENTER);
		actor2.SetZ(.1f);
		stage.Add(actor2);

#if GENERATE
		ShaderEffect premultiplyRenderShader = ShaderEffect::New("", FRAGMENT_PREMULTIPLY_SHADER);
#else
		ShaderEffect premultiplyRenderShader = ShaderEffect::New("", FRAGMENT_DISCARD_SHADER);
#endif
		actor2.SetShaderEffect(premultiplyRenderShader);

    // Respond to a click anywhere on the stage
		stage.GetRootLayer().SetBehavior(Layer::LAYER_3D);
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
