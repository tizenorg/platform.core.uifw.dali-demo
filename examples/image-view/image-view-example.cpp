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

#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "../shared/view.h"
#include <dali-toolkit/dali-toolkit.h>

using namespace std;
using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "bg-1024x1024.jpg" );
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char* QUIT_IMAGE( DALI_IMAGE_DIR "exit-button.png" );
const char* APPLICATION_TITLE( "ImageView Example" );

const char* DISTANCE_FIELD_IMAGE = DALI_IMAGE_DIR "clk-face.png";
const char* BUTTON_BACKGROUND_IMAGE_PATH = DALI_IMAGE_DIR "button-background.png";
const char* SAMPLE_IMAGE = DALI_IMAGE_DIR "pictures_cluster_2.jpg";

// These values depend on the button background image
const Vector4 BUTTON_IMAGE_BORDER( Vector4::ONE * 3.0f );
const float SCREEN_MARGIN(10.0f);                                           ///< Margin indentation around screen
const Vector3 MENU_BUTTON_SIZE = Vector3(0.15f, 0.05f, 1.0f);               ///< Standard Menu Buttons.
const float CAMERA_FIELD_OF_VIEW(26.565f);                                  ///< Field of view in degrees
const float CAMERA_NEAR_CLIP(10.0f);                                        ///< Camera near clip distance
const float CAMERA_FAR_CLIP(50000.0f);                                      ///< Camera far clip distance
const int IMAGE_ROWS(8);                                                    ///< Number of rows of images to display
const int IMAGE_COLUMNS(8);                                                 ///< Number of columns of images to display
const float MINIMUM_PINCH_SCALE(0.1f);                                      ///< Minimum amount user can scale relatively in a single pinch
const float MAXIMUM_PINCH_SCALE(10.0f);                                     ///< Maximum amount user can scale relatively in a single pinch
} // unnamed namespace

/**
 * This example shows how to use ImageView to automatically
 * load and display the correct image for the required detail
 */
class ExampleController : public ConnectionTracker
{
public:

  /**
   * Constructor
   * @param application class, stored as reference
   */
  ExampleController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init and orientation changed signal
    mApplication.InitSignal().Connect(this, &ExampleController::Create);
  }

  /**
   * This method gets called once the main loop of application is up and running
   * @param[in] application The application class, stored as reference
   */
  void Create(Application& application)
  {
    // The Init signal is received once (only) during the Application lifetime

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    Toolkit::ToolBar toolBar;
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            toolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            APPLICATION_TITLE );

    // Creates a quit button.
    Image image = Image::New( QUIT_IMAGE );
    Toolkit::PushButton quitButton = Toolkit::PushButton::New();
    quitButton.SetBackgroundImage( image );

    // Connects the quit button clicked signal to the quit method.
    quitButton.ClickedSignal().Connect( this, &ExampleController::OnQuitTouched );

    toolBar.AddControl( quitButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalLeft );

    // Add an extra space on the right to center the title text.
    toolBar.AddControl( Actor::New(), DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight );

    // Add a Camera to view this.
    AddCamera();

    // Create the content layer, which is where main actors appear.
    AddContentLayer();
  }

private:

  /**
   * Adds a content to the main mContentLayer.
   */
  void AddContentLayer()
  {
    Stage stage = Stage::GetCurrent();
    const Vector2& stageSize = stage.GetSize();

    mContainer = Actor::New();
    mContentLayer.Add( mContainer );
    mContainer.SetAnchorPoint( AnchorPoint::CENTER );
    mContainer.SetParentOrigin( ParentOrigin::CENTER );

    // Litter ImageViews around the screen.
    for(int y = (-IMAGE_ROWS/2); (y<IMAGE_ROWS/2); y++)
    {
      for(int x = (-IMAGE_COLUMNS/2); (x<IMAGE_COLUMNS/2); x++)
      {
        ImageView imageView = ImageView::New();
        imageView.SetAnchorPoint( AnchorPoint::CENTER );
        imageView.SetParentOrigin( ParentOrigin::CENTER );
        const float stageLength = std::min(stageSize.width, stageSize.height);
        Vector3 size(stageLength * 1.0f, stageLength * 0.75f, 0.0f);
        imageView.SetSize(size); // 4:3 ratio image for example.
        mContainer.Add( imageView );

        imageView.SetPosition( size.x * x, size.y * y );
        imageView.SetCameraActor( mCameraActor  );

        if(((x^y)&1))
        {
          imageView.SetImage(DISTANCE_FIELD_IMAGE, ImageView::DistanceFieldType);
        }
        else
        {
          imageView.SetImage(SAMPLE_IMAGE, ImageView::BitmapType, 0.125f, 4.0f);
        }
      }
    }

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach( mContentLayer );
    mPanGestureDetector.DetectedSignal().Connect(this, &ExampleController::OnPan);

    mPinchGestureDetector = PinchGestureDetector::New();
    mPinchGestureDetector.Attach( mContentLayer );
    mPinchGestureDetector.DetectedSignal().Connect(this, &ExampleController::OnPinch);
  }

  /**
   * Adds Camera to scene. Renderer will use this camera's
   * perspective to display contents.
   */
  void AddCamera()
  {
    Stage stage = Stage::GetCurrent();
    const Vector2& stageSize = stage.GetSize();

    // Create and place a camera
    mCameraActor = CameraActor::New();
    mCameraActor.SetParentOrigin(ParentOrigin::CENTER);
    // position camera such that it is a distance from the origin
    // that results in content at the origin appearing at approx 1 pixel
    // to 1 geometric unit ratio.
    const float depthForUnity = getCameraDepthForUnity();
    mCameraActor.SetPosition(Vector3(0.0f, 0.0f, depthForUnity));
    mCameraActor.SetName("MainCamera");
    mCameraActor.SetTargetPosition(-Vector3::ZAXIS);         // pointing into the stage.
    // Set camera actors size (useful for child actor positioning)
    mCameraActor.SetSize(stageSize);
    mCameraActor.SetFieldOfView( CAMERA_FIELD_OF_VIEW * Math::PI / 180.0f );
    mCameraActor.SetNearClippingPlane( CAMERA_NEAR_CLIP );
    mCameraActor.SetFarClippingPlane( CAMERA_FAR_CLIP );
    mCameraActor.SetAspectRatio(stageSize.width / stageSize.height);

    stage.Add(mCameraActor);
  }

  /**
   * Creates a Button (Helper)
   *
   * @param[in] text The text to appear on this button
   */
  PushButton CreateButton(const std::string& text)
  {
    // Create the button
    Image img = Image::New(BUTTON_BACKGROUND_IMAGE_PATH);
    ImageActor background = ImageActor::New(img);

    background.SetStyle(ImageActor::STYLE_NINE_PATCH);
    background.SetNinePatchBorder(BUTTON_IMAGE_BORDER);
    PushButton button = PushButton::New();
    button.SetBackgroundImage(background);

    // Add a text label to the button
    button.SetLabel(text);

    return button;
  }

  /**
   * Invoked upon user using one finger to drag screen
   * @param[in] source The actor being dragged
   * @param[in] gesture The gesture information.
   */
  void OnPan(Actor source, const PanGesture& gesture)
  {
    switch(gesture.state)
    {
      case Gesture::Started:
      {
        mReferencePosition = mContainer.GetCurrentPosition();
        break;
      }
      default:
      {
        break;
      }
    }

    const Vector3 cameraPosition = mCameraActor.GetCurrentPosition();
    const float depth = cameraPosition.z - mReferencePosition.z;
    const float depthForUnity = getCameraDepthForUnity();

    mReferencePosition += Vector3(gesture.displacement * depth / depthForUnity);
    mContainer.SetPosition(mReferencePosition);
  }

  /**
   * Returns camera depth in Z axis from origin required
   * for objects at the origin to appear as 1 pixel per 1 geometric
   * point.
   * @param[in] camera The current camera settings.
   * @return The Z depth required.
   */
  float getCameraDepthForUnity()
  {
    Stage stage = Stage::GetCurrent();
    const Vector2& stageSize = stage.GetSize();
    const float heightPerDepth = tanf( mCameraActor.GetFieldOfView() * 0.5f ) * 2.0f;
    return stageSize.height / heightPerDepth;
  }

  /**
   * Invoked upon user using two fingers to pinch screen
   * @param[in] source The actor being pinched
   * @param[in] gesture The gesture information.
   */
  void OnPinch(Actor source, const PinchGesture& gesture)
  {
    switch(gesture.state)
    {
      case Gesture::Started:
      {
        mReferencePosition = mContainer.GetCurrentPosition();
        break;
      }
      default:
      {
        break;
      }
    }

    const Vector3 cameraPosition = mCameraActor.GetCurrentPosition();
    const float scaleFactor = Clamp(gesture.scale, MINIMUM_PINCH_SCALE, MAXIMUM_PINCH_SCALE);

    mContainer.SetPosition( Vector3( mReferencePosition.x,
                                     mReferencePosition.y,
                                     cameraPosition.z + (mReferencePosition.z - cameraPosition.z) / scaleFactor ) );
  }

  /**
   * Signal handler, called when the 'Quit' button in the bottom right corner is touched.
   *
   * @param[in] button The button that was pressed.
   */
  bool OnQuitTouched(Button button)
  {
    mApplication.Quit();
    return true;
  }

private:

  Application& mApplication;                            ///< Application instance
  Toolkit::View mView;                                  ///< View control holds everything we see
  Layer mContentLayer;                                  ///< The content layer (contains all Actors except toolbar)
  Actor mContainer;                                     ///< The container actor which we move around screen
  PanGestureDetector mPanGestureDetector;               ///< Detects pan movements for moving laterally (X/Y)
  PinchGestureDetector mPinchGestureDetector;           ///< Detects pinch movements for moving in depth (Z)
  Vector3 mReferencePosition;                           ///< Keeps track of position when gesture begins
  CameraActor mCameraActor;                             ///< The camera - content is rendered from this perspective
};

void RunTest(Application& app)
{
  ExampleController test(app);

  app.MainLoop();
}

int main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv);

  RunTest(app);

  return 0;
}
