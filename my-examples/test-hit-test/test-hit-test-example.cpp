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
#include <dali/devel-api/events/hit-test-algorithm.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace std;

enum MODE{ TEST1, TEST2, TEST_ORTHO };
MODE gMode = TEST1;

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

bool DefaultIsActorTouchableFunction(Dali::Actor actor, Dali::HitTestAlgorithm::TraverseType type)
{
  bool hittable = false;

  switch (type)
  {
    case Dali::HitTestAlgorithm::CHECK_ACTOR:
    {
      if( actor.IsVisible() &&
          actor.IsSensitive() &&
          actor.GetCurrentWorldColor().a > 0.01f)
      {
        hittable = true;
      }
      break;
    }
    case Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE:
    {
      if( actor.IsVisible() && // Actor is visible, if not visible then none of its children are visible.
          actor.IsSensitive()) // Actor is sensitive, if insensitive none of its children should be hittable either.
      {
        hittable = true;
      }
      break;
    }
    default:
    {
      break;
    }
  }

  return hittable;
};


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

    gMode = TEST2;

    Stage stage = Stage::GetCurrent();
    RenderTaskList renderTaskList = stage.GetRenderTaskList();
    RenderTask defaultRenderTask = renderTaskList.GetTask(0u);
    Dali::CameraActor cameraActor = defaultRenderTask.GetCameraActor();

    Vector2 stageSize ( stage.GetSize() );

    if( gMode == TEST2 )
    {
      cameraActor.SetOrthographicProjection(-stageSize.x * 0.3f,  stageSize.x * 0.7f,
                                             stageSize.y * 0.3f, -stageSize.y * 0.7f,
                                             800.0f, 4895.0f);
    }
    else
      cameraActor.SetOrthographicProjection( stageSize );

    //cameraActor.SetPosition(0.0f, 0.0f, 1600.0f);

    Vector2 actorSize( stageSize * 0.5f );
    // Create two actors with half the size of the stage and set them to be partially overlapping
    //Actor blue = Actor::New();
    Control blue = Control::New();
    blue.SetName( "Blue" );
    blue.SetBackgroundColor(Vector4(0.0,0.0,0.5,1));
    blue.SetAnchorPoint( AnchorPoint::CENTER );

    if( gMode == TEST1 )
      blue.SetParentOrigin( Vector3(1.0f/3.0f, 1.0f/3.0f, 0.5f) );
    else if( gMode == TEST2 )
    {
      blue.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      blue.SetParentOrigin( Vector3(0.2f, 0.2f, 0.5f) );
    }
    else if( gMode == TEST_ORTHO )
    {
      blue.SetParentOrigin( ParentOrigin::CENTER );
      blue.SetPosition(-100,0,0);
    }

    blue.SetSize( actorSize );
    blue.SetZ(30.0f);

    //blue.SetZ(300.0f);

    //Actor green = Actor::New( );
    Control green = Control::New( );
    green.SetName( "Green" );
    green.SetBackgroundColor(Vector4(0.0,0.5,0.0,1));
    green.SetAnchorPoint( AnchorPoint::CENTER );

    if( gMode == TEST1 )
      green.SetParentOrigin( Vector3(2.0f/3.0f, 2.0f/3.0f, 0.5f) );
    else if( gMode == TEST2 )
    {
      green.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      green.SetParentOrigin( Vector3(0.4f, 0.4f, 0.5f) );
    }
    else if( gMode == TEST_ORTHO )
    {
      green.SetParentOrigin( ParentOrigin::CENTER );
      green.SetPosition(100,0,0);
    }

    green.SetSize( actorSize );

    // Add the actors to the view
    stage.Add( blue );
    stage.Add( green );

    Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    if(touch.GetPoint(0).state == TouchPoint::Finished)
    {
      Stage stage = Stage::GetCurrent();
      Vector2 stageSize ( stage.GetSize() );
      Vector2 actorSize( stageSize * 0.5f );

      if( gMode == TEST1 )
      {
        HitTestAlgorithm::Results results;
        HitTest(stage, stageSize*.5, results, &DefaultIsActorTouchableFunction);
        cout << results.actor.GetName() << endl;
        cout << results.actorCoordinates << " " << actorSize * 5.0f/6.0f << endl;
        //DALI_TEST_CHECK( results.actor == blue );
        //DALI_TEST_EQUALS( results.actorCoordinates, actorSize * 5.0f/6.0f, TEST_LOCATION );

        HitTest(stage, stageSize / 3.0f, results, &DefaultIsActorTouchableFunction);
        cout << results.actor.GetName() << endl;
        cout << results.actorCoordinates << " " << actorSize * 0.5f << endl;
        //DALI_TEST_CHECK( results.actor == blue );
        //DALI_TEST_EQUALS( results.actorCoordinates, actorSize * 0.5f, TEST_LOCATION );

        HitTest(stage, stageSize * 2.0f / 3.0f, results, &DefaultIsActorTouchableFunction);
        cout << results.actor.GetName() << endl;
        cout << results.actorCoordinates << " " << actorSize * 0.5f << endl;
        //DALI_TEST_CHECK( results.actor == green );
        //DALI_TEST_EQUALS( results.actorCoordinates, actorSize * 0.5f, TEST_LOCATION );
      }
      else if( gMode == TEST2 )
      {
        {
          HitTestAlgorithm::Results results;
          HitTest(stage, stageSize*.5, results, &DefaultIsActorTouchableFunction);
          cout << results.actor.GetName() << endl;
          cout << results.actorCoordinates << " " << actorSize * 0.6f << endl;
          //DALI_TEST_CHECK( results.actor == green );
          //DALI_TEST_EQUALS( results.actorCoordinates, actorSize * 0.6f, 0.01f, TEST_LOCATION );
        }

        {
          HitTestAlgorithm::Results results;
          HitTest(stage, Vector2::ZERO, results, &DefaultIsActorTouchableFunction);
          cout << results.actor.GetName() << endl;
          cout << results.actorCoordinates << " " << Vector2::ZERO << endl;
          //DALI_TEST_CHECK( results.actor == blue );
          //DALI_TEST_EQUALS( results.actorCoordinates, Vector2::ZERO, TEST_LOCATION );
        }

        {
          HitTestAlgorithm::Results results;
          HitTest(stage, stageSize, results, &DefaultIsActorTouchableFunction);
          cout << results.actor << endl;
          cout << results.actorCoordinates << " " << Vector2::ZERO << endl;
          //DALI_TEST_CHECK( ! results.actor );
          //DALI_TEST_EQUALS( results.actorCoordinates, Vector2::ZERO, TEST_LOCATION );
        }

        // Just inside green
        {
          HitTestAlgorithm::Results results;
          HitTest(stage, stageSize*0.69f, results, &DefaultIsActorTouchableFunction);
          cout << results.actor.GetName() << endl;
          cout << results.actorCoordinates << " " << actorSize * 0.98f << endl;
          //DALI_TEST_CHECK( results.actor == green );
          //DALI_TEST_EQUALS( results.actorCoordinates, actorSize * 0.98f, 0.01f, TEST_LOCATION );
        }
      }
    }

    // quit the application
    //mApplication.Quit();
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

// Entry point for Linux & SLP applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
