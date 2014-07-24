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

// INTERNAL INCLUDES
#include "../shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-4.jpg" );
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char* APPLICATION_TITLE( "Image Scaling: ScaleToFill" );
const char* TOGGLE_SCALING_IMAGE( DALI_IMAGE_DIR "icon-change.png" );

//const char* BUTTON_BACKGROUND_IMAGE_PATH = DALI_IMAGE_DIR "button-background.png";
const Vector3 ICON_SIZE(100.0f, 100.0f, 0.0f);
}

/**
 * @brief Demonstrate how to use the ScaleToFill image scaling mode when loading
 * images.
 *
 * If the rectangle on-screen that an image is to occupy contains fewer pixels
 * than the image to be loaded into it, the scaling feature of the image loader
 * can be used to reduce the image to save memory, improve performance, and
 * potentially display a better small version of the image than if the default
 * size were loaded and downsampled at render time.
 * The ScaleToFill mode of ImageAttributes makes sure that every pixel in the
 * loaded image is filled with a source colour from the image's central region
 * while losing the minimum number of pixels from its periphery.
 * It is the best option for producing thumbnails of input images that have
 * diverse aspect ratios.
 *
 * This demo uses ImageAttributes::ScaleToFill to load a grid of thumbnails.
 */
class ImageScalingScaleToFillController : public ConnectionTracker
{
public:

  ImageScalingScaleToFillController( Application& application )
  : mApplication( application ),
    mScrolling( false )
  {
    std::cout << "ImageScalingScaleToFillController::ImageScalingScaleToFillController" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ImageScalingScaleToFillController::Create );
  }

  ~ImageScalingScaleToFillController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    std::cout << "ImageScalingScaleToFillController::Create" << std::endl;

    // Get a handle to the stage:
    Stage stage = Stage::GetCurrent();

    // Connect to input event signals:
    stage.KeyEventSignal().Connect(this, &ImageScalingScaleToFillController::OnKeyEvent);
    stage.GetRootLayer().TouchedSignal().Connect( this, &ImageScalingScaleToFillController::OnTouch );

    // Hide the indicator bar
    mApplication.GetWindow().ShowIndicator(false);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.

    mContentLayer = DemoHelper::CreateView( mApplication,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            "" );

    // Create an image scaling toggle button. (right of toolbar)
    Image toggleScalingImage = Image::New( TOGGLE_SCALING_IMAGE );
    Toolkit::PushButton toggleScalingButton = Toolkit::PushButton::New();
    toggleScalingButton.SetBackgroundImage( toggleScalingImage );
    toggleScalingButton.ClickedSignal().Connect( this, &ImageScalingScaleToFillController::OnToggleScalingTouched );
    mToolBar.AddControl( toggleScalingButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    // Build the main content of the widow:
    PopulateContentLayer();
  }

  void PopulateContentLayer()
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    mScrollView = ScrollView::New();

    mScrollView.ScrollStartedSignal().Connect( this, &ImageScalingScaleToFillController::OnScrollStarted );
    mScrollView.ScrollCompletedSignal().Connect( this, &ImageScalingScaleToFillController::OnScrollCompleted );

    mScrollView.EnableScrollComponent( Scrollable::VerticalScrollBar );
    mScrollView.EnableScrollComponent( Scrollable::HorizontalScrollBar );

    mScrollView.SetAnchorPoint(AnchorPoint::CENTER);
    mScrollView.SetParentOrigin(ParentOrigin::CENTER);

    mScrollView.SetSize( stageSize );
    mScrollView.SetAxisAutoLock( true );
    mScrollView.SetAxisAutoLockGradient( 1.0f );

    // Restrict scrolling to mostly vertical only, but with some horizontal wiggle-room:

    RulerPtr rulerX = new FixedRuler( stageSize.width );
    rulerX->SetDomain( RulerDomain( stageSize.width * -0.125f, stageSize.width * 1.125f ) );
    mScrollView.SetRulerX ( rulerX );

    RulerPtr rulerY = new FixedRuler( stageSize.height );
    rulerY->SetDomain( RulerDomain( stageSize.height * -2, stageSize.height * 3 ) );
    mScrollView.SetRulerY ( rulerY );


    mContentLayer.Add( mScrollView );

    // Initialize the actor
    TextActor textActor = TextActor::New( APPLICATION_TITLE );

    // Reposition the actor
    textActor.SetParentOrigin( ParentOrigin::CENTER );


    // Display the actor on the stage
    mScrollView.Add( textActor );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    return true;
  }

 /**
  * Main key event handler.
  * Quit on escape key.
  */
  void OnKeyEvent(const KeyEvent& event)
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE )
          || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

 /**
  * Signal handler, called when the 'Scaling' button has been touched.
  *
  * @param[in] button The button that was pressed.
  */
  bool OnToggleScalingTouched( Button button )
  {
    // Rebuild stage with scaling on or off.
    ///@ToDo: <----------------------------------------------------------------------------------------------------------<<<
    return true;
  }

  /**
   * When scroll starts (i.e. user starts to drag scrollview),
   * note this state (mScrolling = true)
   * @param[in] position Current Scroll Position
   */
  void OnScrollStarted( const Vector3& position )
  {
    mScrolling = true;
  }

  /**
   * When scroll starts (i.e. user stops dragging scrollview, and scrollview has snapped to destination),
   * note this state (mScrolling = false)
   * @param[in] position Current Scroll Position
   */
  void OnScrollCompleted( const Vector3& position )
  {
    mScrolling = false;
  }

private:
  Application&  mApplication;

  Layer mContentLayer;                ///< The content layer (contains non gui chrome actors)
  Toolkit::View mView;                ///< The View instance.
  Toolkit::ToolBar mToolBar;          ///< The View's Toolbar.
  TextView mTitleActor;               ///< The Toolbar's Title.
  ScrollView mScrollView;             ///< ScrollView UI Component
  bool mScrolling;                   ///< ScrollView scrolling state (true = scrolling, false = stationary)
  ScrollViewEffect mScrollViewEffect; ///< ScrollView Effect instance.
  Actor mScrollViewContainer;         ///< Container Sprite within ScrollView.
};

void RunTest( Application& application )
{
  ImageScalingScaleToFillController test( application );

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
