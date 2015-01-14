
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

using namespace Dali;

class AppControlSecond : public ConnectionTracker
{
public:
  TextActor mTitleActor;
  TextActor mTextActor;

  AppControlSecond( Application& application )
  : mApplication( application )
  {
    std::cout << "AppControlSecond::AppControlSecond" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &AppControlSecond::Create );
    mApplication.ControlSignal().Connect( this, &AppControlSecond::OnControl );
  }

  ~AppControlSecond()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    std::cout << "HelloWorldController::Create" << std::endl;

    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    initTextActor();
    // Display the actor on the stage
    stage.Add( mTitleActor );
    stage.Add( mTextActor );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &AppControlSecond::OnTouch );
  }

  void initTextActor()
  {
    mTitleActor = TextActor::New( "This application is launched as" );

    // Reposition the actor
    mTitleActor.SetParentOrigin( ParentOrigin::CENTER );
    mTitleActor.SetPosition(0, -40);

    // Initialize the actor
    mTextActor = TextActor::New( "No AppControl" );

    // Reposition the actor
    mTextActor.SetParentOrigin( ParentOrigin::CENTER );
    mTextActor.SetPosition(0, 20);

    //  Set text style
    Dali::TextStyle textStyle;
    textStyle.SetFontName("HelveticaNue");
    textStyle.SetFontStyle("Regular");
    textStyle.SetFontPointSize(Dali::PointSize(18));
    textStyle.SetWeight( Dali::TextStyle::BOLD );
    textStyle.SetTextColor( Vector4( 0.5f, 1.0f, 0.5f, 1.0f ) );
    mTextActor.SetTextStyle( textStyle );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  void OnControl( Application& application, void *controlData)
  {
    app_control_h app_control = (app_control_h)controlData;

    char *operation;
    char *app_id;

    app_control_get_app_id(app_control, &app_id);
    app_control_get_operation(app_control, &operation);

    if (operation &&
        !strcasecmp(operation, APP_CONTROL_OPERATION_EDIT)) {
      mTextActor.SetText("EDIT");
    } else if (operation &&
        !strcasecmp(operation, APP_CONTROL_OPERATION_VIEW)) {
      mTextActor.SetText("VIEW");
    } else {
      mTextActor.SetText("Normal");
    }
  }

private:
  Application&  mApplication;
};

void RunTest( Application& application )
{
  AppControlSecond test( application );

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

