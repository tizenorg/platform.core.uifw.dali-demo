//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

#include "../shared/view.h"

using namespace Dali;
using Dali::Toolkit::MaskedImageView;

namespace
{
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char* QUIT_IMAGE( DALI_IMAGE_DIR "exit-button.png" );
const char* CHANGE_MODE_IMAGE( DALI_IMAGE_DIR "view-swap.png" );
}

// This example shows how to apply a dynamic mask effect using Toolkit::MaskedImageView.
// Initially the image behind the mask can be panned/zoomed with touch gestures.
// The top-right control button toggles between positioning the image or mask.
//
class MaskedImageViewExample : public ConnectionTracker
{
public:

  MaskedImageViewExample( Application& application )
  : mApplication( application ),
    mMoveMask( false )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &MaskedImageViewExample::Create );
  }

  ~MaskedImageViewExample()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Create a default view with a default tool bar.
    Toolkit::ToolBar toolBar;
    DemoHelper::CreateView( application,
                            mDemoView,
                            toolBar,
                            "", // No background image
                            TOOLBAR_IMAGE,
                            "MaskedImageView Example" );

    // Create a quit button.
    Image image = Image::New( QUIT_IMAGE );
    Toolkit::PushButton quitButton = Toolkit::PushButton::New();
    quitButton.SetBackgroundImage( image );
    quitButton.ClickedSignal().Connect( this, &MaskedImageViewExample::OnQuitClicked );
    toolBar.AddControl( quitButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalLeft );

    // Create a MaskedImageView example

    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();
    mStartSize = stageSize * 0.66f;
    mSize = mStartSize;

    stage.SetBackgroundColor( Vector4(0.4f, 0.4f, 0.4f, 1.0f) );

    Image sourceImage = Image::New(DALI_IMAGE_DIR "magnifier_example_background.jpg");
    Image maskImage   = Image::New(DALI_IMAGE_DIR "masked-image-view-checked.png");

    Vector2 sourceSize = Image::GetImageSize(DALI_IMAGE_DIR "magnifier_example_background.jpg");

    mMaskedImageView = Toolkit::MaskedImageView::New( mStartSize.width, mStartSize.height, sourceImage, maskImage );
    mMaskedImageView.SetParentOrigin( ParentOrigin::CENTER );
    mMaskedImageView.SetSize( mStartSize );
    mMaskedImageView.SetProperty( mMaskedImageView.GetPropertyIndex( MaskedImageView::BACKGROUND_COLOR ), Color::BLUE );
    mMaskedImageView.SetEditMode( Toolkit::MaskedImageView::EDIT_SOURCE );
    mMaskedImageView.SetSourceAspectRatio( sourceSize.width / sourceSize.height );
    stage.Add( mMaskedImageView );

    // Create a button for toggling control of image or mask
    Toolkit::PushButton button = Toolkit::PushButton::New();
    button.SetBackgroundImage( Image::New( CHANGE_MODE_IMAGE ) );
    button.ClickedSignal().Connect( this, &MaskedImageViewExample::OnModeSwitch );
    toolBar.AddControl( button, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight );

    // Rotate the souce image when the device orientation changes
    Dali::Window window = application.GetWindow();
    window.AddAvailableOrientation( Dali::Window::PORTRAIT );
    window.AddAvailableOrientation( Dali::Window::LANDSCAPE );
    window.AddAvailableOrientation( Dali::Window::PORTRAIT_INVERSE  );
    window.AddAvailableOrientation( Dali::Window::LANDSCAPE_INVERSE );

    // set initial orientation
    application.GetOrientation().ChangedSignal().Connect( this, &MaskedImageViewExample::OnOrientationChanged );
  }

  bool OnQuitClicked( Toolkit::Button button )
  {
    mApplication.Quit();
    return true;
  }

  bool OnModeSwitch( Toolkit::Button button )
  {
    mMoveMask = !mMoveMask;

    if( mMoveMask )
    {
      mMaskedImageView.SetEditMode( Toolkit::MaskedImageView::EDIT_MASK );
    }
    else
    {
      mMaskedImageView.SetEditMode( Toolkit::MaskedImageView::EDIT_SOURCE );
    }

    return true;
  }

  void OnOrientationChanged( Orientation orientation )
  {
    if( mMaskedImageView )
    {
      switch( orientation.GetDegrees() )
      {
        case 90u:
        {
          mMaskedImageView.SetSourceRotation( Toolkit::MaskedImageView::ROTATE_270 );
          break;
        }

        case 180u:
        {
          mMaskedImageView.SetSourceRotation( Toolkit::MaskedImageView::ROTATE_180 );
          break;
        }

        case 270u:
        {
          mMaskedImageView.SetSourceRotation( Toolkit::MaskedImageView::ROTATE_90 );
          break;
        }

        case 0u: // fall
        default:
        {
          mMaskedImageView.SetSourceRotation( Toolkit::MaskedImageView::ROTATE_0 );
          break;
        }
      }
    }
  }

private:

  Application&  mApplication;

  Toolkit::View mDemoView;

  Toolkit::MaskedImageView mMaskedImageView;

  PanGestureDetector mPanGestureDetector;
  PinchGestureDetector mPinchDetector;

  Vector2 mOffset;
  Vector2 mStartSize;
  Vector2 mSize;

  bool mMoveMask; // True when controlling the mask position, otherwise image is being controlled
};

void RunTest( Application& application )
{
  MaskedImageViewExample test( application );

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
