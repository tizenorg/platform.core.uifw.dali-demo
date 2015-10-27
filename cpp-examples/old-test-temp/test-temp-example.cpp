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
#include <curl/curl.h>

using namespace Dali;
using namespace std;

// This example shows how to create and display Hello World! using a simple TextActor
//
class YSTestController : public ConnectionTracker
{
public:

  YSTestController( Application& application )
  : mApplication( application )
  {
    std::cout << "YSTestController::YSTestController" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &YSTestController::Create );
  }

  ~YSTestController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    std::cout << "YSTestController::Create" << std::endl;

    Stage stage = Stage::GetCurrent();

    stage.GetRootLayer().TouchedSignal().Connect( this, &YSTestController::OnTouch );
    stage.KeyEventSignal().Connect( this, &YSTestController::OnKeyEvent );

    // local image file
    //Image image = Image::New(DALI_IMAGE_DIR "gallery-large-1.jpg");
    //Image image = Image::New(DALI_IMAGE_DIR "gallery-small-1.jpg");
    //Image image = Image::New(DALI_IMAGE_DIR "gallery-large-1asdf.jpg");

    // animated gif
    //Image image = Image::New("/home/yoonsang/Downloads/Rotating_earth_(large).gif");

    // image file from http
    Image image = Image::New("http://upload.wikimedia.org/wikipedia/commons/e/e5/Raster_to_Vector_Mechanical_Example.jpg");
    //Image image = Image::New("http://upload.wikimedia.org/wikipedia/commons/e/e5/Raster_to_Vector_Mechanical_Example12.jpg");
    //Image image = Image::New("http://4.bp.blogspot.com/-w-_y6_b01m4/VG0SYUYmPLI/AAAAAAAAARo/1MLD-dfUMPQ/s1600/many.png");
    //Image image = Image::New("http://www2.picturepush.com/photo/a/8040430/img/8040430.bmp");
    //cout << "============" << endl;
    //cout << image << endl;

    ImageActor imageAct = ImageActor::New( image );
    imageAct.SetSize(100,100);
    imageAct.SetParentOrigin(ParentOrigin::CENTER);
    stage.Add(imageAct);

  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    return true;
  }

  void OnKeyEvent(const KeyEvent& event);

private:
  Application&  mApplication;
  TextActor baseText;
};

void YSTestController::OnKeyEvent( const KeyEvent& event )
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
  YSTestController test( application );

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
