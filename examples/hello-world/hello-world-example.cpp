/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
using Dali::Toolkit::TextLabel;

//const char* IMAGE_FILENAME = DALI_IMAGE_DIR "comptex1.astc";
//const char* IMAGE_FILENAME = DALI_IMAGE_DIR "comptex1.ktx";
//const char* IMAGE_FILENAME = DALI_IMAGE_DIR "tx-etc1.ktx";
//const char* IMAGE_FILENAME = DALI_IMAGE_DIR "tx-astc-4x4-linear.ktx";
//const char* IMAGE_FILENAME = DALI_IMAGE_DIR "tx-astc-4x4-srgb.ktx";
//const char* IMAGE_FILENAME = DALI_IMAGE_DIR "comptex1-wrapped-astc-nm.ktx";

const char* IMAGE_FILENAME_ETC         = DALI_IMAGE_DIR "tx-etc1.ktx";
const char* IMAGE_FILENAME_ASTC_LINEAR = DALI_IMAGE_DIR "tx-astc-4x4-linear.ktx";
const char* IMAGE_FILENAME_ASTC_SRGB   = DALI_IMAGE_DIR "tx-astc-4x4-srgb.ktx";

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

    TextLabel textLabel = TextLabel::New( "Hello World" );
    textLabel.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    textLabel.SetName( "helloWorldLabel" );
    stage.Add( textLabel );


    Image image = ResourceImage::New( IMAGE_FILENAME_ETC );
    Toolkit::ImageView imageView = Toolkit::ImageView::New( image );
    imageView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    imageView.SetParentOrigin( ParentOrigin::TOP_LEFT );
    stage.Add( imageView );

    Image image2 = ResourceImage::New( IMAGE_FILENAME_ASTC_LINEAR );
    Toolkit::ImageView imageView2 = Toolkit::ImageView::New( image2 );
    imageView2.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
    imageView2.SetParentOrigin( ParentOrigin::CENTER_LEFT );
    stage.Add( imageView2 );

    Image image3 = ResourceImage::New( IMAGE_FILENAME_ASTC_SRGB );
    Toolkit::ImageView imageView3 = Toolkit::ImageView::New( image3 );
    imageView3.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    imageView3.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    stage.Add( imageView3 );


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
