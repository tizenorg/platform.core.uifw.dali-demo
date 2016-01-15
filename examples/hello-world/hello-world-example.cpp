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

#include <iostream>

using namespace Dali;
using Dali::Toolkit::TextLabel;

void EqualToPanPosition( Vector3& current, const PropertyInputContainer& inputs )
{
  current = inputs[0]->GetVector2();
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
    stage.KeyEventSignal().Connect( this, &HelloWorldController::OnKeyEvent );

    mControl = Toolkit::Control::New();
    mControl.SetBackgroundColor( Color::RED );
    mControl.SetAnchorPoint( AnchorPoint::CENTER );
    mControl.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mControl.SetSize( 200.0f, 200.0f );
    stage.Add( mControl );

    // Attach a pan detector to the root layer
    mPan = PanGestureDetector::New();
    mPan.Attach( stage.GetRootLayer() );

    // Constrain the position of the red control to the pan position
    Constraint constraint = Constraint::New< Vector3 >( mControl, Actor::Property::POSITION, &EqualToPanPosition );
    constraint.AddSource( Source( mPan, PanGestureDetector::Property::SCREEN_POSITION ) );
    constraint.Apply();

    // Create a linear constrainer to constrain the opacity of the control
    mLinearConstrainer = LinearConstrainer::New();
    Dali::Property::Value pointsProperty = Property::Value(Property::ARRAY);
    Property::Array* array = pointsProperty.GetArray();

    if( array )
    {
      array->PushBack(0.0f); // Fully transparent on the Left Margin
      array->PushBack(1.0f); // Fully opaque in the Center
      array->PushBack(0.0f); // Fully transparent on the Right Margin
    }
    mLinearConstrainer.SetProperty( LinearConstrainer::Property::VALUE, pointsProperty );

    mLinearConstrainer.Apply( Dali::Property( mControl, Dali::Actor::Property::COLOR_ALPHA ),
                              Dali::Property( mControl, Actor::Property::POSITION_X ),
                              Vector2( 0, stage.GetSize().width ) );
  }

private:

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApplication.Quit();
      }
    }
  }

  Application&  mApplication;
  Toolkit::Control mControl;
  PanGestureDetector mPan;
  LinearConstrainer mLinearConstrainer;
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
