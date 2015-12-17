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
using namespace Dali::Toolkit;
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

    ///////////////////////////////////
    // image test

    //Image image = ResourceImage::New(DALI_IMAGE_DIR "gallery-large-1.jpg");
    //Image image = ResourceImage::New("http://vignette4.wikia.nocookie.net/robber-penguin-agency/images/6/6e/Small-mario.png/revision/latest?cb=20150107080404"); // 1,034px × 1,017px
    //Image image = Image::New("http://upload.wikimedia.org/wikipedia/commons/a/a8/TEIDE.JPG");
    //Image image = Image::New("http://upload.wikimedia.org/wikipedia/commons/e/e5/Raster_to_Vector_Mechanical_Example.jpg");
    //Image image = Image::New("/home/yoonsang/Downloads/Raster_to_Vector_Mechanical_Example.jpg");

    ResourceImage image = ResourceImage::New("http://168.219.209.55/icons/unknown.gif"); // 1,034px × 1,017px
    image.LoadingFinishedSignal().Connect(this, &HelloWorldController::OnImageLoaded); 
    ImageActor imageAct = ImageActor::New(image);
    imageAct.SetSize(100,100);
    imageAct.SetPosition(100,100);
    //imageAct.SetPixelArea(Dali::ImageActor::PixelArea(17,17,400,400));
    Stage::GetCurrent().Add(imageAct);

    ResourceImage image2 = ResourceImage::New("http://mindprod.com/image/coord/pixelgrid.png"); // 1,034px × 1,017px
    image2.LoadingFinishedSignal().Connect(this, &HelloWorldController::OnImageLoaded); 
    ImageView imageAct2 = ImageView::New(image2);
    imageAct2.SetSize(100,100);
    imageAct2.SetPosition(100,250);
    //imageAct.SetPixelArea(Dali::ImageActor::PixelArea(17,17,400,400));
    Stage::GetCurrent().Add(imageAct2);

    ResourceImage image3 = ResourceImage::New("https://www.google.co.kr/images/srpr/logo11w.png"); // 1,034px × 1,017px
    image3.LoadingFinishedSignal().Connect(this, &HelloWorldController::OnImageLoaded); 
    ImageActor imageAct3 = ImageActor::New(image3);
    imageAct3.SetSize(100,100);
    imageAct3.SetPosition(100,400);
    //imageAct.SetPixelArea(Dali::ImageActor::PixelArea(17,17,400,400));
    Stage::GetCurrent().Add(imageAct3);


    // Respond to a click anywhere on the stage
    Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  void OnImageLoaded(ResourceImage image)
  {
    cout << image.GetUrl() << endl;
    cout << image.GetLoadingState() << endl;
    cout << endl;
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
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
