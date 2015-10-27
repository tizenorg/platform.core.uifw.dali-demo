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
  void OnPan( Actor actor, const PanGesture& pinch );

private:
  Application&  mApplication;

  // A gesture detector should be a member variable to exist outside the local scope.
  PanGestureDetector mDetector;
};

void MyConstraintFunction( Vector4& current, const PropertyInputContainer& inputs )
{
  const Vector3& position( inputs[0]->GetVector3() );

  float distance = fabs( position.x );
  if( distance > 100.0f ) // More than 100.0f away, color is red
  {
    current.g = current.b = 0.0f;
  }
  else // Otherwise it blends between white and red
  {
    current.g = current.b = ( 100.0f - distance ) / 100.0f;
  }
}

struct MyConstraintFunctor
{
  // Constructor which takes the distance at which the actor is red
  MyConstraintFunctor(float distance)
    : mDistance(distance)  {}

  // Functor
  void operator()( Vector4& current, const PropertyInputContainer& inputs )
  {
    const Vector3& position( inputs[0]->GetVector3() );

    float distance = fabs( position.x );
    if( distance > mDistance ) // More than mDistance away, color is red
    {
      current.g = current.b = 0.0f;
    }
    else // Otherwise it blends between white and red
    {
      current.g = current.b = ( mDistance - distance ) / mDistance;
    }
  }

  const float mDistance; // Data
};

void HelloWorldExample::Create( Application& application )
{
  // Create a Control
  Control control = Control::New();
  control.SetParentOrigin( ParentOrigin::CENTER );
  control.SetSize( 100.0f, 100.0f );
  control.SetBackgroundColor( Color::WHITE );
  Stage::GetCurrent().Add( control );

  // Use PanGestureDetector to move the control with touch panning
  mDetector = PanGestureDetector::New();
  mDetector.Attach( control );
  mDetector.DetectedSignal().Connect( this, &HelloWorldExample::OnPan );

  // Create a Constraint that targets the control
  //Constraint constraint = Constraint::New< Vector4 >( control, Actor::Property::COLOR, MyConstraintFunction );

  Constraint constraint = Constraint::New< Vector4 >( control, Actor::Property::COLOR, MyConstraintFunctor( 200.0f ) );

  // Add the POSITION property of the control as a constraint input
  constraint.AddSource( LocalSource( Actor::Property::POSITION ) ); 

  // Apply the constraint
  constraint.Apply(); 

  // REMOVE
  Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect( this, &HelloWorldExample::OnTouch );
  Stage::GetCurrent().KeyEventSignal().Connect(this, &HelloWorldExample::OnKeyEvent);
}

void HelloWorldExample::OnPan( Actor actor, const PanGesture& gesture )
{
  // Move the button using detected gesture
  actor.TranslateBy( Vector3( gesture.displacement ) );
}

int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  HelloWorldExample test( application );
  application.MainLoop();
  return 0;
}
