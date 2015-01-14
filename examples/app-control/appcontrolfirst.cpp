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
#include <app_control.h>
#include <dlog.h>


using namespace Dali;

// This example shows how to create and display Hello World! using a simple TextActor
//
class AppControlFirst : public ConnectionTracker
{
public:
  TextActor mTitleActor1;
  TextActor mTitleActor2;
  TextActor mTextActor1;
  TextActor mTextActor2;

  AppControlFirst( Application& application )
  : mApplication( application )
  {
    std::cout << "AppControlFirst::AppControlFirst" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &AppControlFirst::Create );
    mApplication.LowBatterySignal().Connect( this, &AppControlFirst::LowBattery );
  }

  ~AppControlFirst()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    std::cout << "AppControlFirst::Create" << std::endl;

    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    InitTextActor();

    // Display the actor on the stage
    stage.Add( mTitleActor1 );
    stage.Add( mTitleActor2 );
    stage.Add( mTextActor1 );
    stage.Add( mTextActor2 );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &AppControlFirst::OnTouch );
  }

  void InitTextActor()
  {
    mTitleActor1 = TextActor::New( "Next application launch demo" );

    // Reposition the actor
    mTitleActor1.SetParentOrigin( ParentOrigin::CENTER );
    mTitleActor1.SetPosition(0, -80);

    mTitleActor2 = TextActor::New( "Select one" );

    // Reposition the actor
    mTitleActor2.SetParentOrigin( ParentOrigin::CENTER );
    mTitleActor2.SetPosition(0, -30);

    //  Set text style
    Dali::TextStyle textStyle;
    textStyle.SetFontName("HelveticaNue");
    textStyle.SetFontStyle("Regular");
    textStyle.SetFontPointSize(Dali::PointSize(18));
    textStyle.SetWeight( Dali::TextStyle::BOLD );
    textStyle.SetTextColor( Vector4( 0.5f, 1.0f, 0.5f, 1.0f ) );

    // Initialize the actor
    mTextActor1 = TextActor::New( "VIEW" );
    mTextActor1.SetParentOrigin( ParentOrigin::CENTER );
    mTextActor1.SetPosition(-100, 20);
    mTextActor1.SetTextStyle( textStyle );
    mTextActor1.TouchedSignal().Connect( this, &AppControlFirst::OnClick1 );

    // Initialize the actor
    mTextActor2 = TextActor::New( "EDIT" );
    mTextActor2.SetParentOrigin( ParentOrigin::CENTER );
    mTextActor2.SetPosition(100, 20);
    mTextActor2.SetTextStyle( textStyle );
    mTextActor2.TouchedSignal().Connect( this, &AppControlFirst::OnClick2 );
  }


  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  bool OnClick1( Actor actor, const TouchEvent& touch )
  {
    // Launch next application
    LaunchApplication(false);
    return true;
  }

  bool OnClick2( Actor actor, const TouchEvent& touch )
  {
    // Launch next application
    LaunchApplication(true);
    return true;
  }

  void LaunchApplication(bool bEdit) {
    app_control_h app_control;

    app_control_create(&app_control);
    if (bEdit)
      app_control_set_operation(app_control, APP_CONTROL_OPERATION_EDIT);
    else
      app_control_set_operation(app_control, APP_CONTROL_OPERATION_VIEW);
    app_control_set_app_id(app_control, "org.tizen.appcontrolsecond");

    int error = app_control_send_launch_request(app_control, NULL, NULL);
    if (error != APP_CONTROL_ERROR_NONE)
    {
      dlog_print(DLOG_ERROR, LOG_TAG, "app_control_send_launch_request is failed. err = %d", error);
    }
    app_control_destroy(app_control);
  }

  void LowBattery( Application& application )
  {
    mTextActor1.SetVisible(false);
    mTextActor2.SetVisible(false);
    mTitleActor2.SetText("Received Low Battery Event!!");
  }

private:
  Application&  mApplication;
};

void RunTest( Application& application )
{
  AppControlFirst test( application );

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

