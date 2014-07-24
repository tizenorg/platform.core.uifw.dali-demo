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

/** The allowed widths of the grid. These are cycled-through on button-press. */
const unsigned GRID_WIDTHS[] = { 3, 4, 5, 6, 7 };
const unsigned NUM_GRID_WIDTHS = sizeof(GRID_WIDTHS) / sizeof(GRID_WIDTHS[0]);

/** The target image sizes in grid cells. */
const Vector2 IMAGE_SIZES[] = {
 Vector2( 1, 1 ),
 Vector2( 2, 1 ),
 Vector2( 3, 1 ),
 Vector2( 1, 2 ),
 Vector2( 1, 2 ),
 Vector2( 2, 2 ),
 Vector2( 2, 3 ),
 Vector2( 3, 2 )
};
const unsigned NUM_IMAGE_SIZES = sizeof(IMAGE_SIZES) / sizeof(IMAGE_SIZES[0]);

/** Images to load into the grid. These are mostly large and non-square to
 *  show the scaling. */
const char* IMAGE_PATHS[] = {
  // Artificial images that are labeled with their aspect ratios and contain
  // many squares to allow aspect ratio preservation to be visually inspected:
  DALI_IMAGE_DIR "tall.png",
  DALI_IMAGE_DIR "square.png",
  DALI_IMAGE_DIR "wide.png",
  DALI_IMAGE_DIR "tall.gif",
  DALI_IMAGE_DIR "square.gif",
  DALI_IMAGE_DIR "wide.gif",

  // A large, 3840 * 2160 pixel image:
  DALI_IMAGE_DIR "wide.4k.png",

  // Images from other demos that are tall, wide or just large:
  DALI_IMAGE_DIR "blocks-paddle.png",
  DALI_IMAGE_DIR "background-1.jpg",
  DALI_IMAGE_DIR "background-2.jpg",
  DALI_IMAGE_DIR "background-3.jpg",
  DALI_IMAGE_DIR "background-4.jpg",
  DALI_IMAGE_DIR "background-5.jpg",
  DALI_IMAGE_DIR "background-blocks.jpg",
  DALI_IMAGE_DIR "background-gradient.jpg",
  DALI_IMAGE_DIR "background-magnifier.jpg",
  DALI_IMAGE_DIR "book-landscape-cover-back.jpg",
  DALI_IMAGE_DIR "book-landscape-cover.jpg",
  DALI_IMAGE_DIR "book-landscape-p1.jpg",
  DALI_IMAGE_DIR "book-landscape-p2.jpg",
  DALI_IMAGE_DIR "book-landscape-p3.jpg",
  DALI_IMAGE_DIR "book-landscape-p4.jpg",
  DALI_IMAGE_DIR "book-landscape-p5.jpg",
  DALI_IMAGE_DIR "book-landscape-p6.jpg",
  DALI_IMAGE_DIR "book-landscape-p7.jpg",
  DALI_IMAGE_DIR "book-landscape-p8.jpg",
  DALI_IMAGE_DIR "book-portrait-cover.jpg",
  DALI_IMAGE_DIR "book-portrait-p1.jpg",
  DALI_IMAGE_DIR "book-portrait-p2.jpg",
  DALI_IMAGE_DIR "book-portrait-p3.jpg",
  DALI_IMAGE_DIR "book-portrait-p4.jpg",
  DALI_IMAGE_DIR "book-portrait-p5.jpg",
  DALI_IMAGE_DIR "book-portrait-p6.jpg",
  DALI_IMAGE_DIR "book-portrait-p7.jpg",
  DALI_IMAGE_DIR "book-portrait-p8.jpg",
  DALI_IMAGE_DIR "brick-wall.jpg",
  DALI_IMAGE_DIR "dali-logo.png",
  DALI_IMAGE_DIR "gallery-large-10.jpg",
  DALI_IMAGE_DIR "gallery-large-11.jpg",
  DALI_IMAGE_DIR "gallery-large-12.jpg",
  DALI_IMAGE_DIR "gallery-large-13.jpg",
  DALI_IMAGE_DIR "gallery-large-14.jpg",
  DALI_IMAGE_DIR "gallery-large-15.jpg",
  DALI_IMAGE_DIR "gallery-large-16.jpg",
  DALI_IMAGE_DIR "gallery-large-17.jpg",
  DALI_IMAGE_DIR "gallery-large-18.jpg",
  DALI_IMAGE_DIR "gallery-large-19.jpg",
  DALI_IMAGE_DIR "gallery-large-1.jpg",
  DALI_IMAGE_DIR "gallery-large-20.jpg",
  DALI_IMAGE_DIR "gallery-large-21.jpg",
  DALI_IMAGE_DIR "gallery-large-2.jpg",
  DALI_IMAGE_DIR "gallery-large-3.jpg",
  DALI_IMAGE_DIR "gallery-large-4.jpg",
  DALI_IMAGE_DIR "gallery-large-5.jpg",
  DALI_IMAGE_DIR "gallery-large-6.jpg",
  DALI_IMAGE_DIR "gallery-large-7.jpg",
  DALI_IMAGE_DIR "gallery-large-8.jpg",
  DALI_IMAGE_DIR "gallery-large-9.jpg",
  DALI_IMAGE_DIR "image-with-border-1.jpg",
  DALI_IMAGE_DIR "image-with-border-2.jpg",
  DALI_IMAGE_DIR "image-with-border-3.jpg",
  DALI_IMAGE_DIR "image-with-border-4.jpg",
  DALI_IMAGE_DIR "image-with-border-5.jpg",
  DALI_IMAGE_DIR "layer1.png",
  DALI_IMAGE_DIR "people-medium-10.jpg",
  DALI_IMAGE_DIR "people-medium-2.jpg",
  DALI_IMAGE_DIR "people-medium-6.jpg",
  NULL
};
const unsigned NUM_IMAGE_PATHS = sizeof(IMAGE_PATHS) / sizeof(IMAGE_PATHS[0]) - 1u;

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
