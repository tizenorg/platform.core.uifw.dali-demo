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
    stage.GetRootLayer().SetBehavior(Layer::LAYER_3D);

    item = Actor::New();
    item.SetParentOrigin(ParentOrigin::CENTER);
    item.SetAnchorPoint(AnchorPoint::CENTER);
    item.SetSize(Vector2(300, 300));
    stage.Add(item);

    ImageActor bg = CreateSolidColorActor(Color::RED);
    bg.SetParentOrigin(ParentOrigin::CENTER);
    bg.SetAnchorPoint(AnchorPoint::CENTER);
    bg.SetSize(Vector2(300, 300));
    item.Add(bg);

    ImageActor icon = CreateSolidColorActor(Color::GREEN);
    icon.SetParentOrigin(ParentOrigin::TOP_RIGHT);
    icon.SetAnchorPoint(AnchorPoint::TOP_RIGHT);
    icon.SetSize(Vector2(50, 50));
    icon.SetZ(0.2f);
    bg.Add(icon);

    TextLabel textLabel = TextLabel::New( "Hello World" );
    textLabel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    textLabel.SetName( "hello-world-label" );
    item.Add(textLabel);

    CheckBoxButton btn = CheckBoxButton::New();
    btn.SetParentOrigin(ParentOrigin::BOTTOM_RIGHT);
    btn.SetAnchorPoint(AnchorPoint::BOTTOM_RIGHT);
    btn.SetSize(Vector2(50,50));
    item.Add(btn);



    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    Animation animation = Animation::New(3.0f);
    animation.AnimateTo(Property(item, Actor::Property::ORIENTATION), Quaternion(Radian(3.14), Vector3::XAXIS), AlphaFunction::BOUNCE);
    animation.Play();
    return true;
  }

private:
  Actor item;
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
