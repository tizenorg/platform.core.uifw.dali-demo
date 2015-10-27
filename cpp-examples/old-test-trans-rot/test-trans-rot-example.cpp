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
#include "test-trans-rot-example.h"

TestTransRotController::TestTransRotController( Application& application )
: mApplication( application )
{
  std::cout << "TestTransRotController::TestTransRotController" << std::endl;

  // Connect to the Application's Init signal
  mApplication.InitSignal().Connect( this, &TestTransRotController::Create );
}

// The Init signal is received once (only) during the Application lifetime
void TestTransRotController::Create( Application& application )
{
  std::cout << "TestTransRotController::Create" << std::endl;

  // key handler
  Stage::GetCurrent().KeyEventSignal().Connect( this, &TestTransRotController::OnKeyEvent );

  // Respond to a click anywhere on the stage
  Stage stage = Stage::GetCurrent();

  TextActor baseText = TextActor::New( "Base" );
  baseText.SetParentOrigin( ParentOrigin::CENTER );
  stage.Add( baseText );

  // more text
  TextActor nearText = TextActor::New("NearNear");
  TextActor farText = TextActor::New("FarFar");
  nearText.SetParentOrigin(ParentOrigin::CENTER);
  farText.SetParentOrigin(ParentOrigin::CENTER);
  baseText.Add(nearText);
  baseText.Add(farText);

  nearText.MoveBy(Vector3(0, 50,0));
  farText.MoveBy(Vector3(0,-50,0));

  bool actorRotTest = false;
  //bool actorRotTest = true;

  if(actorRotTest)
  {
    baseText.RotateBy(Degree(30), Vector3(0,0,1));

    /////////////////////////////////////////////////
    // MoveBy - RotateBy and RotateBy - MoveBy cause the same result,
    // disimilarly to OpenGL transformation matrix.

    //nearText.MoveBy(Vector3(200,0,0));
    //nearText.RotateBy(Degree(30), Vector3(0,0,1));

    nearText.RotateBy(Degree(30), Vector3(0,0,1));
    nearText.MoveBy(Vector3(200,0,0));

    /////////////////////////////////////////////////
    // Rotate functions rotates w.r.t. local frame
    nearText.RotateBy(Degree(90), Vector3(1,0,0));

    //Animation ani2;
    //ani2 = Animation::New(2);
    //ani2.RotateBy(nearText, Degree(360), Vector3(0,1,0));
    //ani2.SetLooping(true);
    //ani2.Play();

    /////////////////////////////////////////////////
    // order of sequential Rotate calls
    // - first call, first rotate

    farText.RotateBy(Degree(90), Vector3(1,0,0));
    farText.RotateBy(Degree(30), Vector3(0,1,0));

    //farText.RotateBy(Degree(30), Vector3(0,1,0));
    //farText.RotateBy(Degree(90), Vector3(1,0,0));
  }

  else
  {
    /////////////////////////////////////////////////
    // camera actor rotation order
    // - first call, first rotate
    CameraActor cameraActor = Stage::GetCurrent().GetRenderTaskList().GetTask(0).GetCameraActor();

    //cameraActor.RotateBy(Quaternion(Radian(Degree(10)), Vector3(1,0,0)));
    //cameraActor.RotateBy(Quaternion(Radian(Degree(30)), Vector3(0,0,1)));

    cameraActor.RotateBy(Quaternion(Radian(Degree(30)), Vector3(0,0,1)));
    cameraActor.RotateBy(Quaternion(Radian(Degree(10)), Vector3(1,0,0)));
  }
}

void TestTransRotController::OnKeyEvent( const KeyEvent& event )
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
  TestTransRotController test( application );

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
