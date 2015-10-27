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
 */

#include <string>
#include <cstdarg>
#include <dali-toolkit/dali-toolkit.h>
#include "test-trackball-example.h"
#include "../test-shared/common.h"
#include "../test-shared/Trackball.h"


TestTrackballController::TestTrackballController( Application& application )
: mApplication( application )
{
  std::cout << "TestTrackballController::TestTrackballController" << std::endl;

  // Connect to the Application's Init signal
  mApplication.InitSignal().Connect( this, &TestTrackballController::Create );
}

// The Init signal is received once (only) during the Application lifetime
void TestTrackballController::Create( Application& application )
{
  std::cout << "TestTrackballController::Create" << std::endl;

  Stage stage = Stage::GetCurrent();
  stage.GetRootLayer().TouchedSignal().Connect( this, &TestTrackballController::OnTouch );
  Stage::GetCurrent().KeyEventSignal().Connect( this, &TestTrackballController::OnKeyEvent );

  mView = Toolkit::View::New();
  stage.Add(mView);

  TextActor baseText = TextActor::New("Trackball");
  baseText.SetParentOrigin( ParentOrigin::CENTER );
  stage.Add( baseText );

  Actor coord = CreateCoordAxesText(100);
  stage.Add(coord);
  coord.SetParentOrigin( ParentOrigin::CENTER );

  mPanGestureDetector = PanGestureDetector::New();
  mPanGestureDetector.Attach(mView);
  mPanGestureDetector.DetectedSignal().Connect(this, &TestTrackballController::OnPan);
}

void TestTrackballController::OnPan(Actor actor, const PanGesture& gesture)
{
  switch (gesture.state)
  {
    case Gesture::Started:
      {
      start_x = gesture.screenPosition.x;
      start_y = gesture.screenPosition.y;
      CameraActor cameraActor = Stage::GetCurrent().GetRenderTaskList().GetTask(0).GetCameraActor();
      start_camera_p = cameraActor.GetCurrentWorldPosition();
      start_camera_q = cameraActor.GetCurrentWorldRotation();
      start_view_q = mView.GetCurrentWorldRotation();
      break;
      }

    case Gesture::Continuing:
      {
      int cur_x = gesture.screenPosition.x;
      int cur_y = gesture.screenPosition.y;

      Vector2 stageSize = Stage::GetCurrent().GetSize();
      int W = stageSize[0];
      int H = stageSize[1];
      double q[4];
      trackball(q, 2*(float(start_x)/W-.5), 2*(float(start_y)/H-.5), 2*(float(cur_x)/W-.5), 2*(float(cur_y)/H-.5));
      //cout << stringf("%f %f, %f %f", 2*(float(start_x)/W-.5), 2*(float(start_y)/H-.5), 2*(float(cur_x)/W-.5), 2*(float(cur_y)/H-.5)) << endl;

      Quaternion basicCamRot(Radian(Degree(-180)), Vector3(0,1,0));
      Quaternion Q(Vector4(q[0],q[1],q[2],q[3]));

      CameraActor cameraActor = Stage::GetCurrent().GetRenderTaskList().GetTask(0).GetCameraActor();
      cameraActor.SetPosition(Q*start_camera_p);
      cameraActor.SetRotation(Quaternion());
      cameraActor.RotateBy(Q);
      cameraActor.RotateBy(start_camera_q);

      mView.SetRotation(Quaternion());
      mView.RotateBy(Q);
      mView.RotateBy(start_view_q);
      break;
      }

    case Gesture::Finished:
      break;

    default:
      break;
  }
}


bool TestTrackballController::OnTouch( Actor actor, const TouchEvent& touch )
{
  // quit the application
  //mApplication.Quit();
  return true;
}
void TestTrackballController::OnKeyEvent( const KeyEvent& event )
{
  if( event.state == KeyEvent::Down )
  {
    if ( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
    {
      mApplication.Quit();
    }
  }
}


void RunTest( Application& application )
{
  TestTrackballController test( application );

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
