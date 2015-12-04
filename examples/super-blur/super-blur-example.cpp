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
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/super-blur-view/super-blur-view.h>
#include "shared/view.h"

#include <iostream>

using namespace Dali;

namespace
{
const char * const TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char * const APPLICATION_TITLE( "Super Blur" );
const char * const CHANGE_BACKGROUND_ICON( DALI_IMAGE_DIR "icon-change.png" );
const char * const CHANGE_BACKGROUND_ICON_SELECTED( DALI_IMAGE_DIR "icon-change-selected.png" );

const char* BACKGROUND_IMAGES[]=
{
  DALI_IMAGE_DIR "background-1.jpg",
  DALI_IMAGE_DIR "background-2.jpg",
  DALI_IMAGE_DIR "background-3.jpg",
  DALI_IMAGE_DIR "background-4.jpg",
  DALI_IMAGE_DIR "background-5.jpg",
  DALI_IMAGE_DIR "background-magnifier.jpg",
};
const unsigned int NUM_BACKGROUND_IMAGES( sizeof( BACKGROUND_IMAGES ) / sizeof( BACKGROUND_IMAGES[0] ) );
}

/**
 * @brief Load an image, scaled-down to no more than the stage dimensions.
 *
 * Uses image scaling mode FittingMode::SCALE_TO_FILL to resize the image at
 * load time to cover the entire stage with pixels with no borders,
 * and filter mode BOX_THEN_LINEAR to sample the image with
 * maximum quality.
 */
ResourceImage LoadStageFillingImage( const char * const imagePath )
{
  Size stageSize = Stage::GetCurrent().GetSize();
  return ResourceImage::New( imagePath, Dali::ImageDimensions( stageSize.x, stageSize.y ), Dali::FittingMode::SCALE_TO_FILL, Dali::SamplingMode::BOX_THEN_LINEAR );
}

class SuperBlurExample : public ConnectionTracker
{
public:
  SuperBlurExample(Application &app)
  : mApp(app),
    mImageIndex( 0 ),
    mIsBlurring( false )
  {
    // Connect to the Application's Init signal
    app.InitSignal().Connect(this, &SuperBlurExample::Create);
  }

  ~SuperBlurExample()
  {
  }
private:
  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& app)
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    stage.KeyEventSignal().Connect(this, &SuperBlurExample::OnKeyEvent);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    Toolkit::ToolBar toolBar;
    Layer content = DemoHelper::CreateView( app,
                                            mBackground,
                                            toolBar,
                                            "",
                                            TOOLBAR_IMAGE,
                                            APPLICATION_TITLE );

    // Add a button to change background. (right of toolbar)
    mChangeBackgroundButton = Toolkit::PushButton::New();
    mChangeBackgroundButton.SetUnselectedImage( CHANGE_BACKGROUND_ICON );
    mChangeBackgroundButton.SetSelectedImage( CHANGE_BACKGROUND_ICON_SELECTED );
    mChangeBackgroundButton.ClickedSignal().Connect( this, &SuperBlurExample::OnChangeIconClicked );
    toolBar.AddControl( mChangeBackgroundButton,
        DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage,
        Toolkit::Alignment::HorizontalRight,
        DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    mSuperBlurView = Toolkit::SuperBlurView::New( 5 );
    mSuperBlurView.SetSize( stageSize );
    ResourceImage image = LoadStageFillingImage( BACKGROUND_IMAGES[mImageIndex] );
    mSuperBlurView.SetImage( image );
    mIsBlurring = true;
    mSuperBlurView.SetParentOrigin( ParentOrigin::CENTER );
    mSuperBlurView.SetAnchorPoint( AnchorPoint::CENTER );
    mBackground.Add( mSuperBlurView );

    // Connect the callback to the touch signal on the background
    mSuperBlurView.TouchedSignal().Connect( this, &SuperBlurExample::OnTouch );
    mSuperBlurView.BlurFinishedSignal().Connect(this, &SuperBlurExample::OnBlurFinished);
  }

  // Callback function of the touch signal on the background
  bool OnTouch(Dali::Actor actor, const Dali::TouchEvent& event)
  {
    const TouchPoint &point = event.GetPoint(0);
    switch(point.state)
    {
      case TouchPoint::Down:
      {
        if( mAnimation )
        {
          mAnimation.Clear();
        }
        mAnimation = Animation::New( 2.f );
        mAnimation.AnimateTo( Property( mSuperBlurView, mSuperBlurView.GetBlurStrengthPropertyIndex() ), 1.f );
        mAnimation.Play();
        break;
      }
      case TouchPoint::Up:
      case TouchPoint::Leave:
      case TouchPoint::Interrupted:
      {
        if( mAnimation )
        {
          mAnimation.Clear();
        }
        mAnimation = Animation::New( 2.f );
        mAnimation.AnimateTo( Property( mSuperBlurView, mSuperBlurView.GetBlurStrengthPropertyIndex() ), 0.f );
        mAnimation.Play();
        break;
      }
      case TouchPoint::Motion:
      case TouchPoint::Stationary:
      case TouchPoint::Last:
      default:
      {
        break;
      }
    }
    return true;
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApp.Quit();
      }
    }
  }

  bool OnChangeIconClicked( Toolkit::Button button )
  {
    if( mIsBlurring )
    {
      return false;
    }

    mIsBlurring = true;

    if( mAnimation )
    {
      mAnimation.Clear();
    }

    mSuperBlurView.SetBlurStrength( 0.f );
    mImageIndex = (mImageIndex+1u)%NUM_BACKGROUND_IMAGES;
    ResourceImage image = LoadStageFillingImage( BACKGROUND_IMAGES[mImageIndex] );
    mSuperBlurView.SetImage( image );
    return true;
  }

  void OnBlurFinished( Toolkit::SuperBlurView blurView )
  {
    mIsBlurring = false;
  }

private:

  Application&               mApp;
  Toolkit::Control           mBackground;
  Toolkit::PushButton        mChangeBackgroundButton;
  Toolkit::SuperBlurView     mSuperBlurView;
  Animation                  mAnimation;
  unsigned int               mImageIndex;
  bool                       mIsBlurring;
};

/*****************************************************************************/

static void
RunTest(Application& app)
{
  SuperBlurExample theApp(app);
  app.MainLoop();
}

/*****************************************************************************/

int
main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv, DALI_DEMO_THEME_PATH);

  RunTest(app);

  return 0;
}
