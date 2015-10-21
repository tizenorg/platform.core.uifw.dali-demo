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
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/image-atlas/image-atlas.h>
#include <dali/devel-api/adaptor-framework/bitmap-loader.h>

using namespace Dali;

namespace
{
const char * const TEST_IMAGES[] =
{
  DALI_IMAGE_DIR "background-magnifier.jpg",
  DALI_IMAGE_DIR "book-landscape-cover.jpg",
  DALI_IMAGE_DIR "gallery-large-1.jpg",
  DALI_IMAGE_DIR "gallery-medium-1.jpg",
  DALI_IMAGE_DIR "gallery-small-1.jpg"
};
const unsigned int NUMBER_OF_IMAGE( sizeof(TEST_IMAGES) / sizeof(TEST_IMAGES[0]) );

}

class ImageAtlasController : public ConnectionTracker
{
public:

  ImageAtlasController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ImageAtlasController::Create );
  }

  ~ImageAtlasController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();
    stage.KeyEventSignal().Connect(this, &ImageAtlasController::OnKeyEvent);
    imageAtlas = Toolkit::ImageAtlas::New(720, 1280, Pixel::RGB888);
    for( index = 0u; index< NUMBER_OF_IMAGE-1u; index++ )
    {

      imageAtlas.Upload(TEST_IMAGES[index]);
    }

    Image atlas = imageAtlas.GetAtlas();

    Toolkit::ImageView view = Toolkit::ImageView::New( atlas );

    view.SetParentOrigin( ParentOrigin::TOP_LEFT );
    view.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    stage.Add( view );

    std::cout<<"application thread"<<std::endl;

  }

  void OnKeyEvent( const KeyEvent& event )
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        //mApplication.Quit();
        if( index < NUMBER_OF_IMAGE )
        {
          imageAtlas.Upload(TEST_IMAGES[index++]);
        }
        else
        {
          mApplication.Quit();
        }
      }
    }
  }


private:
  Application&  mApplication;
  Toolkit::ImageAtlas imageAtlas;
  unsigned int index;


};

void RunTest( Application& application )
{
  ImageAtlasController test( application );

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
