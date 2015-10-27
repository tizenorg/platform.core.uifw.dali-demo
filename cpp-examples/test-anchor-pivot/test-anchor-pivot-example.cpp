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
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace std;

// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application )
  {
    std::cout << "HelloWorldController::HelloWorldController" << std::endl;

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
    std::cout << "HelloWorldController::Create" << std::endl;

    ImageActor parent = ImageActor::New(ResourceImage::New(DALI_IMAGE_DIR "gallery-large-1.jpg"));
    ImageActor child = ImageActor::New(ResourceImage::New(DALI_IMAGE_DIR "gallery-large-2.jpg"));
    parent.Add(child);
    Stage::GetCurrent().Add(parent);
    parent.SetParentOrigin(ParentOrigin::CENTER);

    Vector2 origin(0.5, 0.5);
    Vector2 pivot(0.5, 0.5); // rotation
    Vector2 anchor(0., 0.);
    Vector3 position(0., 0., 0.);

    Widget_setOrigin(child, origin);
    Widget_setPivot(child, pivot);
    Widget_setAnchor(child, anchor);
    Widget_setPosition(child, position);

    Vector2 pivot2(0., 0.); // rotation
    Widget_setPivot(child, pivot2);

    Vector2 anchor2(0.5, 0.5);
    Widget_setAnchor(child, anchor2);



    mRotAni = Animation::New(2.0);
    mRotAni.AnimateBy(Property(child, Actor::Property::ORIENTATION), Quaternion(Radian(Degree(360)), Vector3(0,0,1)));
    mRotAni.SetLooping(true);

    // Respond to a click anywhere on the stage
    Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  //// temporary Actor member function implementation

  // Actor::GetAnchorPoint() is supposed to be available.
  Vector3 Actor_GetAnchorPoint(Actor actor)
  {
    return temp_anchor;
  }

  // temporary replacement of Actor::SetAnchorPoint()
  void Actor_SetAnchorPoint(Actor actor, const Vector3& anchor)
  {
    actor.SetAnchorPoint(anchor);
    temp_anchor = anchor;
  }

  // Actor::GetPosition() is supposed to be available.
  Vector3 Actor_GetPosition(Actor actor)
  {
    return temp_position;
  }

  // temporary replacement of Actor::SetPosition()
  void Actor_SetPosition(Actor actor, const Vector3& position)
  {
    actor.SetPosition(position);
    temp_position = position;
  }

  //// utility functions

  Vector2 V3toV2(const Vector3& v3)
  {
    return Vector2(v3.x, v3.y);
  }   

  Vector3 V2toV3(const Vector2& v2, float z)
  {
    return Vector3(v2.x, v2.y, z);
  }   

  //// Widget member function simulating implementation

  void Widget_setPosition(Actor actor, const Vector3& position)
  {
    Actor_SetPosition(actor, position + V2toV3(Widget_getPivot(actor) - Widget_getAnchor(actor), 0) * actor.GetTargetSize()); 
  }

  Vector3 Widget_getPosition(Actor actor)
  {
    Vector3 actorPos = Actor_GetPosition(actor);
    return actorPos - V2toV3(Widget_getPivot(actor) - Widget_getAnchor(actor), 0) * actor.GetTargetSize();
  }

  Vector2 Widget_getPivot(Actor actor)
  {
    return V3toV2(Actor_GetAnchorPoint(actor));
  }

  Vector2 Widget_getAnchor(Actor actor)
  {
    return Vector2(widget_anchor_x, widget_anchor_y);
  }

  void Widget_setPivot(Actor actor, const Vector2& pivot)
  {
    Actor_SetPosition(actor, Widget_getPosition(actor) + V2toV3(pivot - Widget_getAnchor(actor), 0) * actor.GetTargetSize()); 
    Actor_SetAnchorPoint(actor, V2toV3(pivot, 0.5));
  }

  void Widget_setAnchor(Actor actor, const Vector2& anchor)
  {
    Actor_SetPosition(actor, Widget_getPosition(actor) + V2toV3(Widget_getPivot(actor) - anchor, 0) * actor.GetTargetSize()); 
    widget_anchor_x = anchor.x;
    widget_anchor_y = anchor.y;
  }

  void Widget_setOrigin(Actor actor, const Vector2& origin)
  {
    actor.SetParentOrigin(V2toV3(origin, 0.5));
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    mRotAni.Play();
    // quit the application
    //mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
  Animation mRotAni;

  // member variables to be maintained in Volt Widget.
  float widget_anchor_x;
  float widget_anchor_y;

  // temporary
  Vector3 temp_position;
  Vector3 temp_anchor;
};

void RunTest( Application& application )
{
  HelloWorldController test( application );

  application.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
