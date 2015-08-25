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

#include "shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/slider/slider.h>

using namespace Dali;

namespace
{

const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-gradient.jpg" );
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char* APPLICATION_TITLE( "Blending" );

const char* MATERIAL_SAMPLE( DALI_IMAGE_DIR "heartsframe.9.png" );

}  // namespace

class BlendingController: public ConnectionTracker
{
 public:

  BlendingController( Application& application )
    : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &BlendingController::Create );
  }

  ~BlendingController()
  {
    // Nothing to do here
  }

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            APPLICATION_TITLE );


    mImage = Toolkit::ImageView::New( MATERIAL_SAMPLE );
    mImage.SetSize( Vector2( 300, 600 ) );
    mImage.SetParentOrigin( ParentOrigin::CENTER );
    mImage.SetAnchorPoint( AnchorPoint::CENTER );
    mContentLayer.Add( mImage );

    Toolkit::PushButton button = Toolkit::PushButton::New();
    button.SetLabelText( "Toggle on/off stage" );
    button.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    button.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    button.ClickedSignal().Connect( this, &BlendingController::ToggleImageOnStage );

    mContentLayer.Add(button);
  }

private:
  bool ToggleImageOnStage( Toolkit::Button button )
  {
    if( mImage.OnStage() )
    {
      mImage.Unparent();
    }
    else
    {
      mContentLayer.Add( mImage );
    }

    return true;
  }

private:
  Application&  mApplication;

  Toolkit::Control           mView;                              ///< The View instance.
  Toolkit::ToolBar           mToolBar;                           ///< The View's Toolbar.
  Layer                      mContentLayer;                      ///< Content layer (scrolling cluster content)
  Toolkit::PushButton        mLoseContextButton;
  Toolkit::ImageView         mImage;
};

void RunTest( Application& application )
{
  BlendingController test( application );

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
