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

#include "shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/slider/slider.h>

using namespace Dali;

namespace
{

}  // namespace

class ImageViewController: public ConnectionTracker
{
 public:

  ImageViewController( Application& application )
    : mApplication( application ),
      mCurrentPositionToggle( 0, 0 ),
      mCurrentPositionImage( 0, 0 ),
      mToggleOff( true ),
      mUseResource( false ),
      mImageIdx( 1 )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ImageViewController::Create );
  }

  ~ImageViewController()
  {
    // Nothing to do here
  }

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime

    //mImageView = Toolkit::ImageView::New( DALI_IMAGE_DIR "watch1.jpg" );
    mImageView = Toolkit::ImageView::New( DALI_IMAGE_DIR "blocks-ball.png" );
    mImageView.SetParentOrigin( ParentOrigin::CENTER );
    mImageView.SetAnchorPoint( AnchorPoint::CENTER );

    Stage::GetCurrent().Add( mImageView );

    Stage::GetCurrent().KeyEventSignal().Connect(this, &ImageViewController::OnKeyEvent);
  }

private:

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application&  mApplication;

  Toolkit::Control           mView;                              ///< The View instance.
  Toolkit::ToolBar           mToolBar;                           ///< The View's Toolbar.
  Layer                      mContentLayer;                      ///< Content layer
  Toolkit::TableView         mTable;
  Toolkit::ImageView        mImageView;

  Toolkit::TableView::CellPosition mCurrentPositionToggle;
  Toolkit::TableView::CellPosition mCurrentPositionImage;

  bool mToggleOff;
  bool mUseResource;
  int mImageIdx;

};

void RunTest( Application& application )
{
  ImageViewController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DALI_DEMO_THEME_PATH );

  RunTest( application );

  return 0;
}
