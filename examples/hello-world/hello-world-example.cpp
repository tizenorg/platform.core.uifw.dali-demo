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

#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/control-renderer.h>

using namespace Dali;

bool gBorderAntiAliasing(false);

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

    /*TextLabel textLabel = TextLabel::New( "Hello World" );
    textLabel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    textLabel.SetName( "helloWorldLabel" );
    stage.Add( textLabel );*/

    Toolkit::RendererFactory rendererFactory = Toolkit::RendererFactory::Get();
    Vector2 size(280.f, 280.f);

    Actor actor1 = Actor::New();
    actor1.SetSize(size);
    actor1.SetParentOrigin( ParentOrigin::CENTER);
    actor1.SetAnchorPoint( AnchorPoint::CENTER );
    stage.Add( actor1 );
    actor1.SetY( -200.f );
    Toolkit::ControlRenderer borderRenderer1 = rendererFactory.GetControlRenderer(10.f, Color::BLACK, false);
    borderRenderer1.SetOnStage( actor1 );

    Actor actor2 = Actor::New();
    actor2.SetSize(size);
    actor2.SetParentOrigin( ParentOrigin::CENTER);
    actor2.SetAnchorPoint( AnchorPoint::CENTER );
    stage.Add( actor2 );
    actor2.SetY( 200.f );
    Toolkit::ControlRenderer borderRenderer2 = rendererFactory.GetControlRenderer(10.f, Color::BLACK, true);
    borderRenderer2.SetOnStage( actor2 );

    Animation animation = Animation::New( 10.f );
    Degree relativeRotationDegrees(180.0f);
    Radian relativeRotationRadians(relativeRotationDegrees);
    animation.AnimateTo( Property( actor1, Actor::Property::ORIENTATION ),  Quaternion( relativeRotationRadians, Vector3::ZAXIS ));
    animation.AnimateTo( Property( actor2, Actor::Property::ORIENTATION ),  Quaternion( relativeRotationRadians, Vector3::ZAXIS ));
    animation.SetLooping( true );
    animation.Play();

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
