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

class ResourceImageController : public ConnectionTracker
{
public:
  ResourceImageController( Application& application ) : mApplication( application )
  {
    mApplication.InitSignal().Connect( this, &ResourceImageController::Create );
  }

  void Create( Application& application )
  {
    Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect( this, &ResourceImageController::OnTouch );
    Stage::GetCurrent().KeyEventSignal().Connect(this, &ResourceImageController::OnKeyEvent);

    ResourceImage image = ResourceImage::New("https://www.tizen.org/sites/default/files/admins/tizen-branding-lockup-on-light.png");
    //ResourceImage image = ResourceImage::New(DALI_IMAGE_DIR "gallery-large-2.jpg");
    //ResourceImage image = ResourceImage::New("/home/yoonsang/Downloads/hello_world_dali1.9.png");
    image.LoadingFinishedSignal().Connect(this, &ResourceImageController::OnLoadFinished); 

    ImageView imageView = ImageView::New(image);
    imageView.SetSize(400,200);
    imageView.SetParentOrigin(ParentOrigin::CENTER);
    Stage::GetCurrent().Add(imageView);
  }

  void OnLoadFinished(ResourceImage image)
  {
    LoadingState state = image.GetLoadingState();
    if(state == ResourceLoadingSucceeded)
      cout << "Loading " << image.GetUrl() << " is succeeded" << endl;
    else if(state == ResourceLoadingFailed)
      cout << "Loading " << image.GetUrl() << " is failed" << endl;
  }

  void OnKeyEvent( const KeyEvent& event )
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
    // quit the application
    //mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
};

// Entry point for Linux & SLP applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  ResourceImageController test( application );
  application.MainLoop();
  return 0;
}
