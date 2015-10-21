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
  DALI_IMAGE_DIR "blocks-ball.png",
  DALI_IMAGE_DIR "blocks-paddle.png",
  DALI_IMAGE_DIR "bubble-ball.png",
  DALI_IMAGE_DIR "cluster-image-frame.png",
  DALI_IMAGE_DIR "contacts-add.png",
  DALI_IMAGE_DIR "dali-logo.png",
  DALI_IMAGE_DIR "folder_appicon_empty_bg.png",
  DALI_IMAGE_DIR "folder_appicon_empty_open_bg.png",
  DALI_IMAGE_DIR "grab-handle.png",
  DALI_IMAGE_DIR "icon-change.png",
  DALI_IMAGE_DIR "icon-cluster-carousel.png",
  DALI_IMAGE_DIR "icon-cluster-none.png",
  DALI_IMAGE_DIR "icon-cluster-sphere.png",
  DALI_IMAGE_DIR "icon-play.png",
  DALI_IMAGE_DIR "icon-replace.png",
  DALI_IMAGE_DIR "gallery-small-1.jpg",
  DALI_IMAGE_DIR "gallery-small-2.jpg",
  DALI_IMAGE_DIR "icon-scroll-view-depth.png",
  DALI_IMAGE_DIR "icon-stop.png",
  DALI_IMAGE_DIR "Logo-for-demo.png",
  DALI_IMAGE_DIR "icon-item-view-layout-depth-selected.png",
  DALI_IMAGE_DIR "icon-effect-fold-selected.png",
  DALI_IMAGE_DIR "music-libray-rectangle.png",
  DALI_IMAGE_DIR "spot_button_off.png",
  DALI_IMAGE_DIR "spot_button_on.png",
  DALI_IMAGE_DIR "spot_jelly_on.png",
  DALI_IMAGE_DIR "item-select-check.png",
  DALI_IMAGE_DIR "popup_button_background.9.png",
  DALI_IMAGE_DIR "selection-popup-bg.1.9.png",
  DALI_IMAGE_DIR "button-up-1.9.png",
  DALI_IMAGE_DIR "button-up-8.9.png",
  DALI_IMAGE_DIR "icon-mediump.png",
  DALI_IMAGE_DIR "icon-item-view-layout-spiral.png",
  DALI_IMAGE_DIR "music-libray-triangle.png",
  DALI_IMAGE_DIR "spot_jelly_off.png",
  DALI_IMAGE_DIR "icon-cluster-wobble.png",
  DALI_IMAGE_DIR "icon-delete.png",
  DALI_IMAGE_DIR "icon-edit.png",
  DALI_IMAGE_DIR "gallery-small-3.jpg",
  DALI_IMAGE_DIR "gallery-small-4.jpg",
  DALI_IMAGE_DIR "icon-effect-cross.png",
  DALI_IMAGE_DIR "icon-effect-fold.png",
  DALI_IMAGE_DIR "icon-effects-off.png",
  DALI_IMAGE_DIR "icon-effects-on.png",
  DALI_IMAGE_DIR "icon-effect-wave.png",
  DALI_IMAGE_DIR "icon-highp.png",
  DALI_IMAGE_DIR "icon-insert.png",
  DALI_IMAGE_DIR "icon-item-view-layout-depth.png",
  DALI_IMAGE_DIR "icon-mediump.png",
  DALI_IMAGE_DIR "icon-replace.png"
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
    Vector2 stageSize = stage.GetSize();
    Toolkit::ImageAtlas imageAtlas = Toolkit::ImageAtlas::New(stageSize.width, stageSize.height, Pixel::RGBA8888);
    Vector4 textureRect;
    for(unsigned int index = 0u; index< NUMBER_OF_IMAGE; index++ )
    {
      imageAtlas.Upload( textureRect, TEST_IMAGES[index] );
    }
    Image atlas = imageAtlas.GetAtlas();
    Toolkit::ImageView view = Toolkit::ImageView::New( atlas );

    view.SetParentOrigin( ParentOrigin::TOP_LEFT );
    view.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    stage.Add( view );
  }

  void OnKeyEvent( const KeyEvent& event )
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApplication.Quit();
      }
    }
  }


private:
  Application&  mApplication;
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
