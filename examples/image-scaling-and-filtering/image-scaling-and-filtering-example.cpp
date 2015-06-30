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
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/popup/popup.h>
#include "shared/view.h"
#include <iostream>

using namespace Dali;
using Toolkit::TextLabel;

namespace
{

const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-gradient.jpg" );
const Vector4 BACKGROUND_COLOUR( 1.0f, 1.0f, 1.0f, 0.15f );

const int MARGIN_SIZE = 10;

const char* const NEXT_BUTTON_ID = "NEXT_BUTTON";
const char* const PREVIOUS_BUTTON_ID = "PREVIOUS_BUTTON";
const char * const DALI_ICON_PLAY = DALI_IMAGE_DIR "icon-play.png";

const char* const FITTING_BUTTON_ID = "FITTING_BUTTON";
const char* const SAMPLING_BUTTON_ID = "SAMPLING_BUTTON";
const char* const FITTING_BUTTON_TEXT = "Fitting";
const char* const SAMPLING_BUTTON_TEXT = "Sampling";

const char* const STYLE_LABEL_TEXT  = "grouplabel";
const char* const STYLE_BUTTON_TEXT = "buttonlabel";



const char* IMAGE_PATHS[] =
{
  // Worst case for aliasing in downscaling, 2k x 2k 1 bit per pixel dithered
  // black and white image:
  DALI_IMAGE_DIR "gallery-large-14.wbmp",
  // Variety of sizes, shapes and formats:
  DALI_IMAGE_DIR "animation-list.png",
  DALI_IMAGE_DIR "layer1.png",
  DALI_IMAGE_DIR "layer2.png",
  DALI_IMAGE_DIR "music-libray-main-screen.png",
  DALI_IMAGE_DIR "music-libray-record-cover.png",
  DALI_IMAGE_DIR "contacts-background.png",
  DALI_IMAGE_DIR "portrait_screen_primitive_shapes.gif",
  DALI_IMAGE_DIR "landscape_screen_primitive_shapes.gif",
  DALI_IMAGE_DIR "square_primitive_shapes.bmp",
  DALI_IMAGE_DIR "dali-logo.png",
  DALI_IMAGE_DIR "com.samsung.dali-demo.ico",
  DALI_IMAGE_DIR "gallery-large-14.jpg",
  DALI_IMAGE_DIR "book-landscape-cover.jpg",
  DALI_IMAGE_DIR "book-portrait-p1.jpg",
  DALI_IMAGE_DIR "book-landscape-cover-back.jpg",
  DALI_IMAGE_DIR "background-1.jpg",
  DALI_IMAGE_DIR "background-blocks.jpg",
  DALI_IMAGE_DIR "background-magnifier.jpg",
  DALI_IMAGE_DIR "gallery-large-14.jpg",
  NULL
};
const int NUM_IMAGE_PATHS = sizeof(IMAGE_PATHS) / sizeof(IMAGE_PATHS[0]) - 1u;

/** Cycle the scaling mode options. */
FittingMode::Type NextScalingMode( FittingMode::Type oldMode )
{
  FittingMode::Type newMode = FittingMode::SHRINK_TO_FIT;
  switch ( oldMode )
  {
    case FittingMode::SHRINK_TO_FIT:
      newMode = FittingMode::SCALE_TO_FILL;
      break;
    case FittingMode::SCALE_TO_FILL:
      newMode = FittingMode::FIT_WIDTH;
      break;
    case FittingMode::FIT_WIDTH:
      newMode = FittingMode::FIT_HEIGHT;
      break;
    case FittingMode::FIT_HEIGHT:
      newMode = FittingMode::SHRINK_TO_FIT;
      break;
  }
  return newMode;
}

/** Cycle through filter mode options. */
SamplingMode::Type NextFilterMode( SamplingMode::Type oldMode )
{
  SamplingMode::Type newMode = SamplingMode::BOX;

  switch ( oldMode )
  {
    case SamplingMode::BOX:
      newMode = SamplingMode::NEAREST;
      break;
    case SamplingMode::NEAREST:
      newMode = SamplingMode::LINEAR;
      break;
    case SamplingMode::LINEAR:
      newMode = SamplingMode::BOX_THEN_NEAREST;
      break;
    case SamplingMode::BOX_THEN_NEAREST:
      newMode = SamplingMode::BOX_THEN_LINEAR;
      break;
    case SamplingMode::BOX_THEN_LINEAR:
      newMode = SamplingMode::NO_FILTER;
      break;
    case SamplingMode::NO_FILTER:
      newMode = SamplingMode::BOX;
      break;
    case SamplingMode::DONT_CARE:
      newMode = SamplingMode::BOX;
      break;
  }
  return newMode;
}

const char* StringFromScalingMode( FittingMode::Type scalingMode )
{
  return scalingMode == FittingMode::SCALE_TO_FILL ? "SCALE_TO_FILL" : scalingMode == FittingMode::SHRINK_TO_FIT ? "SHRINK_TO_FIT" : scalingMode == FittingMode::FIT_WIDTH ? "FIT_WIDTH" : scalingMode == FittingMode::FIT_HEIGHT ? "FIT_HEIGHT" : "UnknownScalingMode";
}

const char* StringFromFilterMode( SamplingMode::Type filterMode )
{
  return filterMode == SamplingMode::BOX ? "BOX" : filterMode == SamplingMode::BOX_THEN_NEAREST ? "BOX_THEN_NEAREST" : filterMode == SamplingMode::BOX_THEN_LINEAR ? "BOX_THEN_LINEAR" : filterMode == SamplingMode::NEAREST ? "NEAREST" : filterMode == SamplingMode::LINEAR ? "LINEAR" : filterMode == SamplingMode::NO_FILTER ? "NO_FILTER" : filterMode == SamplingMode::DONT_CARE ? "DONT_CARE" : "UnknownFilterMode";
}

}

// This example shows the load-time image scaling and filtering features.
//
class ImageScalingAndFilteringController : public ConnectionTracker
{
public:

  ImageScalingAndFilteringController( Application& application )
  : mApplication( application ),
    mImageStageScale( 0.5f, 0.5f ),
    mCurrentPath( 0 ),
    mFittingMode( FittingMode::SCALE_TO_FILL ),
    mSamplingMode( SamplingMode::BOX_THEN_LINEAR)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ImageScalingAndFilteringController::Create );
  }

  ~ImageScalingAndFilteringController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    // Background image:
    ResourceImage backgroundImage = ResourceImage::New( BACKGROUND_IMAGE, ImageDimensions( stage.GetSize().width, stage.GetSize().height ), FittingMode::SCALE_TO_FILL, SamplingMode::BOX_THEN_LINEAR );
    ImageActor background = ImageActor::New( backgroundImage );
    background.SetZ( -2.0f );
    background.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    background.SetSize( stage.GetSize() );
    stage.Add( background );

    // Make grey pixels for the desired box, the desired height the desired width:
    BufferImage desiredBackground = BufferImage::WHITE();

    BufferImage heightBackground = BufferImage::WHITE();
    PixelBuffer* const heightPixel = heightBackground.GetBuffer();
    heightPixel[0] = 0x8f;
    heightPixel[1] = 0x8f;
    heightPixel[2] = 0x8f;

    BufferImage widthBackground = BufferImage::WHITE();
    PixelBuffer* const widthPixel = widthBackground.GetBuffer();
    widthPixel[0] = 0x4f;
    widthPixel[1] = 0x4f;
    widthPixel[2] = 0x4f;

    mHeightBox = ImageActor::New( heightBackground );
    mHeightBox.SetOpacity( 0.2f );
    stage.Add( mHeightBox );

    mWidthBox = ImageActor::New( widthBackground );
    mWidthBox.SetOpacity( 0.2f );
    stage.Add( mWidthBox );

    mDesiredBox = ImageActor::New( desiredBackground );
    stage.Add( mDesiredBox );

    mDesiredBox.SetSize( stage.GetSize() * mImageStageScale );
    mDesiredBox.SetParentOrigin( ParentOrigin::CENTER );
    mDesiredBox.SetAnchorPoint( AnchorPoint::CENTER );
    mDesiredBox.SetPosition( 0, 0, -1 );
    mDesiredBox.SetSortModifier(4.f);

    mHeightBox.SetSize( stage.GetSize().width,  (stage.GetSize() * mImageStageScale).height );
    mHeightBox.SetParentOrigin( ParentOrigin::CENTER );
    mHeightBox.SetAnchorPoint( AnchorPoint::CENTER );
    mHeightBox.SetPosition( 0, 0, -1 );
    mHeightBox.SetSortModifier(3.f);

    mWidthBox.SetSize( (stage.GetSize() * mImageStageScale).width, stage.GetSize().height );
    mWidthBox.SetParentOrigin( ParentOrigin::CENTER );
    mWidthBox.SetAnchorPoint( AnchorPoint::CENTER );
    mWidthBox.SetPosition( 0, 0, -1 );
    mWidthBox.SetSortModifier(2.f);

    // Make a grab-handle for resizing the image:
    mGrabCorner = Toolkit::PushButton::New();
    mGrabCorner.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::WIDTH );
    mGrabCorner.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    mGrabCorner.SetName( "GrabCorner" );
    mGrabCorner.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
    mGrabCorner.SetParentOrigin( ParentOrigin::BOTTOM_RIGHT );
    mGrabCorner.SetSize( Vector2( stage.GetSize().width*0.08f, stage.GetSize().width*0.08f ) );
    mGrabCorner.SetOpacity( 0.6f );

    Layer grabCornerLayer = Layer::New();
    grabCornerLayer.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
    grabCornerLayer.SetParentOrigin( ParentOrigin::BOTTOM_RIGHT );

    grabCornerLayer.Add( mGrabCorner );
    mDesiredBox.Add( grabCornerLayer );
    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach( mGrabCorner );
    mPanGestureDetector.DetectedSignal().Connect( this, &ImageScalingAndFilteringController::OnPan );

    // Initialize the actor
    mImageActor = ImageActor::New();

    // Reposition the actor
    mImageActor.SetParentOrigin( ParentOrigin::CENTER );
    mImageActor.SetAnchorPoint( AnchorPoint::CENTER );
    mImageActor.SetSortModifier(5.f);

    // Display the actor on the stage
    stage.Add( mImageActor );

    mImageActor.SetSize( stage.GetSize() * mImageStageScale );

    // Setup the pinch detector for scaling the desired image load dimensions:
    mPinchDetector = PinchGestureDetector::New();
    mPinchDetector.Attach( mImageActor );
    mPinchDetector.DetectedSignal().Connect( this, &ImageScalingAndFilteringController::OnPinch );

    // Tie-in input event handlers:
    stage.KeyEventSignal().Connect( this, &ImageScalingAndFilteringController::OnKeyEvent );

    CreateControls();

    ResizeImage();
  }

  /**
   * Create the GUI controls which float above the scene
   */
  void CreateControls()
  {
    Stage stage = Stage::GetCurrent();

    Dali::Layer controlsLayer = Dali::Layer::New();
    controlsLayer.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    controlsLayer.SetSizeModeFactor( Vector3( 1.0f, 1.0f, 1.0f ) );
    controlsLayer.SetAnchorPoint( AnchorPoint::TOP_LEFT);
    controlsLayer.SetParentOrigin( ParentOrigin::TOP_LEFT);
    stage.Add( controlsLayer );

    // Back and next image buttons in corners of stage:
    unsigned int playWidth = std::min( stage.GetSize().x * (1 / 5.0f), 58.0f );
    Image playImage = ResourceImage::New( DALI_ICON_PLAY, ImageDimensions( playWidth, playWidth ), FittingMode::SHRINK_TO_FIT, SamplingMode::BOX_THEN_LINEAR );
    Actor imagePrevious = ImageActor::New( playImage );

    // Last image button:
    imagePrevious.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    imagePrevious.RotateBy( Radian(3.14159265358979323846f), Vector3( 0, 1.0f, 0 ) );
    imagePrevious.SetY( playWidth * 0.5f );
    imagePrevious.SetX( playWidth + playWidth * 0.5f );
    imagePrevious.SetOpacity( 0.6f );
    controlsLayer.Add( imagePrevious );
    imagePrevious.SetName( PREVIOUS_BUTTON_ID );
    imagePrevious.TouchedSignal().Connect( this, &ImageScalingAndFilteringController::OnControlTouched );

    // Next image button:
    Actor imageNext = ImageActor::New( playImage );
    imageNext.SetAnchorPoint( AnchorPoint::TOP_RIGHT );
    imageNext.SetY( playWidth * 0.5f );
    imageNext.SetX( stage.GetSize().x - playWidth * 0.5f );
    imageNext.SetOpacity( 0.6f );
    controlsLayer.Add( imageNext );
    imageNext.SetName( NEXT_BUTTON_ID );
    imageNext.TouchedSignal().Connect( this, &ImageScalingAndFilteringController::OnControlTouched );

    // Buttons to popup selectors for fitting and sampling modes:

    // Wrapper table to hold two buttons side by side:
    Toolkit::TableView modesGroupBackground = Toolkit::TableView::New( 1, 2 );
    modesGroupBackground.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    modesGroupBackground.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    modesGroupBackground.SetBackgroundColor( BACKGROUND_COLOUR );
    modesGroupBackground.SetCellPadding( Size( MARGIN_SIZE * 0.5f, MARGIN_SIZE ) );
    modesGroupBackground.SetFitHeight( 0 );

    modesGroupBackground.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    modesGroupBackground.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    modesGroupBackground.SetPosition( 0.0f, 0.0f );

    controlsLayer.Add( modesGroupBackground );

    {
      // Vertical table to hold label and button:
      Toolkit::TableView fittingModeGroup = Toolkit::TableView::New( 2, 1 );
      fittingModeGroup.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      fittingModeGroup.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      fittingModeGroup.SetBackgroundColor( BACKGROUND_COLOUR );
      fittingModeGroup.SetCellPadding( Size( MARGIN_SIZE * 0.5f, MARGIN_SIZE * 0.5f ) );
      fittingModeGroup.SetFitHeight( 0 );
      fittingModeGroup.SetFitHeight( 1 );

      TextLabel label = TextLabel::New( "Image fitting mode:" );
      label.SetProperty( Toolkit::Control::Property::STYLE_NAME, STYLE_LABEL_TEXT );
      fittingModeGroup.Add( label );

      Toolkit::PushButton button = CreateButton( FITTING_BUTTON_ID, StringFromScalingMode( mFittingMode ) );
      button.GetLabel().SetProperty( Toolkit::Control::Property::STYLE_NAME, STYLE_BUTTON_TEXT );
      fittingModeGroup.Add( button );
      mFittingModeButton = button;

      modesGroupBackground.Add( fittingModeGroup );
    }

    {
      // Vertical table to hold label and button:
      Toolkit::TableView samplingModeGroup = Toolkit::TableView::New( 2, 1 );
      samplingModeGroup.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      samplingModeGroup.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      samplingModeGroup.SetBackgroundColor( BACKGROUND_COLOUR );
      samplingModeGroup.SetCellPadding( Size( MARGIN_SIZE * 0.5f, MARGIN_SIZE * 0.5f ) );
      samplingModeGroup.SetFitHeight( 0 );
      samplingModeGroup.SetFitHeight( 1 );

      TextLabel label = TextLabel::New( "Image sampling mode:" );
      label.SetProperty( Toolkit::Control::Property::STYLE_NAME, STYLE_LABEL_TEXT );
      samplingModeGroup.Add( label );

      Toolkit::PushButton button = CreateButton( SAMPLING_BUTTON_ID, StringFromFilterMode( mSamplingMode ) );
      button.GetLabel().SetProperty( Toolkit::Control::Property::STYLE_NAME, STYLE_BUTTON_TEXT );
      samplingModeGroup.Add( button );
      mSamplingModeButton = button;

      modesGroupBackground.Add( samplingModeGroup );
    }
  }

  Toolkit::PushButton CreateButton( const char * id, const char * label )
  {
    Toolkit::PushButton button = Toolkit::PushButton::New();
    button.SetProperty( Toolkit::Control::Property::STYLE_NAME, STYLE_BUTTON_TEXT );
    button.SetName( id );
    button.SetLabel( label );
    button.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    button.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    button.ClickedSignal().Connect( this, &ImageScalingAndFilteringController::OnButtonClicked );
    return button;
  }

  Toolkit::Popup CreatePopup()
  {
    Stage stage = Stage::GetCurrent();
    const float POPUP_WIDTH_DP = stage.GetSize().width * 0.75f;

    Toolkit::Popup popup = Toolkit::Popup::New();
    popup.SetName( "POPUP" );
    popup.SetParentOrigin( ParentOrigin::CENTER );
    popup.SetAnchorPoint( AnchorPoint::CENTER );
    popup.SetSize( POPUP_WIDTH_DP, 0.0f );
    popup.HideTail();

    popup.OutsideTouchedSignal().Connect( this, &ImageScalingAndFilteringController::OnPopupOutsideTouched );

    return popup;
  }

  //void CreatePopupButton( Toolkit::Popup popup, const char* id )
  Toolkit::PushButton CreatePopupButton( Actor parent, const char* id )
  {
    Toolkit::PushButton button = Toolkit::PushButton::New();
    button.SetName( id );
    button.SetLabel( id );
    Toolkit::TextLabel textLabel = Toolkit::TextLabel::DownCast( button.GetLabel() );
    textLabel.SetProperty( TextLabel::Property::POINT_SIZE, 12.0f );

    button.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    button.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    button.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    button.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );

    button.ClickedSignal().Connect( this, &ImageScalingAndFilteringController::OnButtonClicked );

    parent.Add( button );
    return button;
  }

  bool OnButtonClicked( Toolkit::Button button )
  {
    if( button.GetName() == FITTING_BUTTON_ID )
    {
      mPopup = CreatePopup();

      // Four-row table to hold buttons:
      Toolkit::TableView fittingModes = Toolkit::TableView::New( 4, 1 );
      fittingModes.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      fittingModes.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      fittingModes.SetBackgroundColor( BACKGROUND_COLOUR );
      fittingModes.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE * 0.5 ) );
      fittingModes.SetFitHeight( 0 );
      fittingModes.SetFitHeight( 1 );
      fittingModes.SetFitHeight( 2 );
      fittingModes.SetFitHeight( 3 );

      CreatePopupButton( fittingModes, StringFromScalingMode( FittingMode::SCALE_TO_FILL ) );
      CreatePopupButton( fittingModes, StringFromScalingMode( FittingMode::SHRINK_TO_FIT ) );
      CreatePopupButton( fittingModes, StringFromScalingMode( FittingMode::FIT_WIDTH ) );
      CreatePopupButton( fittingModes, StringFromScalingMode( FittingMode::FIT_HEIGHT ) );

      mPopup.Add( fittingModes );
      mPopup.Show();
    }
    else if( button.GetName() == SAMPLING_BUTTON_ID )
    {
      mPopup = CreatePopup();

      // Table to hold buttons for each sampling mode:
      Toolkit::TableView samplingModes = Toolkit::TableView::New( 6, 1 );
      samplingModes.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      samplingModes.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      samplingModes.SetBackgroundColor( BACKGROUND_COLOUR );
      samplingModes.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE * 0.5 ) );
      samplingModes.SetFitHeight( 0 );
      samplingModes.SetFitHeight( 1 );
      samplingModes.SetFitHeight( 2 );
      samplingModes.SetFitHeight( 3 );
      samplingModes.SetFitHeight( 4 );
      samplingModes.SetFitHeight( 5 );

      CreatePopupButton( samplingModes, StringFromFilterMode( SamplingMode::NEAREST ) );
      CreatePopupButton( samplingModes, StringFromFilterMode( SamplingMode::LINEAR ) );
      CreatePopupButton( samplingModes, StringFromFilterMode( SamplingMode::BOX ) );
      CreatePopupButton( samplingModes, StringFromFilterMode( SamplingMode::BOX_THEN_NEAREST ) );
      CreatePopupButton( samplingModes, StringFromFilterMode( SamplingMode::BOX_THEN_LINEAR ) );
      CreatePopupButton( samplingModes, StringFromFilterMode( SamplingMode::NO_FILTER ) );

      mPopup.Add( samplingModes );
      mPopup.Show();
    }
    else if( CheckFittingModeButton( button, FittingMode::SCALE_TO_FILL) ||
             CheckFittingModeButton( button, FittingMode::SHRINK_TO_FIT) ||
             CheckFittingModeButton( button, FittingMode::FIT_WIDTH) ||
             CheckFittingModeButton( button, FittingMode::FIT_HEIGHT) )
    {
    }
    else if( CheckSamplingModeButton( button, SamplingMode::NEAREST ) ||
             CheckSamplingModeButton( button, SamplingMode::LINEAR ) ||
             CheckSamplingModeButton( button, SamplingMode::BOX ) ||
             CheckSamplingModeButton( button, SamplingMode::LINEAR ) ||
             CheckSamplingModeButton( button, SamplingMode::BOX_THEN_NEAREST ) ||
             CheckSamplingModeButton( button, SamplingMode::BOX_THEN_LINEAR ) ||
             CheckSamplingModeButton( button, SamplingMode::NO_FILTER ) )
    {
    }
    return true;
  }

  bool CheckFittingModeButton( Actor &button, FittingMode::Type mode )
  {
    const char * const modeName = StringFromScalingMode( mode );
    if( button.GetName() == modeName )
    {
      mFittingMode = mode;
      mFittingModeButton.SetLabel( modeName );
      mFittingModeButton.GetLabel().SetProperty( Toolkit::Control::Property::STYLE_NAME, STYLE_BUTTON_TEXT );
      ResizeImage();
      mPopup.Hide();
      mPopup.Reset();
      return true;
    }
    return false;
  }

  bool CheckSamplingModeButton( Actor &button, SamplingMode::Type mode )
  {
    const char * const modeName = StringFromFilterMode( mode );
    if( button.GetName() == modeName )
    {
      mSamplingMode = mode;
      mSamplingModeButton.SetLabel( modeName );
      mSamplingModeButton.GetLabel().SetProperty( Toolkit::Control::Property::STYLE_NAME, STYLE_BUTTON_TEXT );
      ResizeImage();
      mPopup.Hide();
      mPopup.Reset();
      return true;
    }
    return false;
  }

  void OnPopupOutsideTouched()
  {
    if( mPopup )
    {
      mPopup.Hide();
      mPopup.Reset();
    }
  }

  void OnImageLoaded( ResourceImage image )
  {
      DALI_ASSERT_DEBUG( image == mNextImage );
      mImageActor.SetImage( image );
      mImageActor.SetSize( Size( image.GetWidth(), image.GetHeight() ) );
  }

  bool OnControlTouched( Actor actor, const TouchEvent& event )
  {
    if(event.GetPointCount() > 0)
    {
      const TouchPoint& point = event.GetPoint(0);
      switch(point.state)
      {
        case TouchPoint::Up:
        {
          const std::string & name = actor.GetName();
          if( name == NEXT_BUTTON_ID )
          {
            mCurrentPath = mCurrentPath + 1;
            mCurrentPath = mCurrentPath <  NUM_IMAGE_PATHS ? mCurrentPath : 0;
            ResizeImage();
          }
          else if( name == PREVIOUS_BUTTON_ID )
          {
            mCurrentPath = mCurrentPath - 1;
            mCurrentPath = mCurrentPath >= 0 ? mCurrentPath : NUM_IMAGE_PATHS - 1;
            ResizeImage();
          }
          break;
        }
        default:
        {
          break;
        }
      } // end switch
    }

    return false;
  }

  void OnPinch( Actor actor, const PinchGesture& pinch )
  {
    if( pinch.state == Gesture::Started )
    {
      mLastPinchScale = pinch.scale;
    }
    const float scale = pinch.scale;

    if( scale != mLastPinchScale )
    {
      if ( scale < mLastPinchScale )
      {
        mImageStageScale.x = std::max( 0.05f, mImageStageScale.x * 0.9f );
        mImageStageScale.y = std::max( 0.05f, mImageStageScale.y * 0.9f );
      }
      else
      {
        mImageStageScale.x = std::max( 0.05f, std::min( 1.0f, mImageStageScale.x * 1.1f ) );
        mImageStageScale.y = std::max( 0.05f, std::min( 1.0f, mImageStageScale.y * 1.1f ) );
      }
      ResizeImage();
    }
    mLastPinchScale = scale;
  }

  void OnPan( Actor actor, const PanGesture& gesture )
  {
    Stage stage = Stage::GetCurrent();
    mImageStageScale.x = std::max( 0.05f, std::min( 1.0f, mImageStageScale.x + (gesture.displacement.x * 2.0f / stage.GetSize().width ) ) );
    mImageStageScale.y = std::max( 0.05f, std::min( 1.0f, mImageStageScale.y + (gesture.displacement.y * 2.0f / stage.GetSize().height ) ) );
    ResizeImage();
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        if( mPopup && mPopup.IsVisible() )
        {
          mPopup.Hide();
          mPopup.Reset();
        }
        else
        {
          mApplication.Quit();
        }
      }
      else if ( event.keyPressedName == "Right" )
      {
        mImageStageScale.x = std::max( 0.05f, std::min( 1.0f, mImageStageScale.x * 1.1f ) );
      }
      else if ( event.keyPressedName == "Left" )
      {
        mImageStageScale.x = std::max( 0.05f, mImageStageScale.x * 0.9f );
      }
      else if ( event.keyPressedName == "Up" )
      {
        mImageStageScale.y = std::max( 0.05f, std::min( 1.0f, mImageStageScale.y * 1.1f ) );
      }
      else if ( event.keyPressedName == "Down" )
      {
        mImageStageScale.y = std::max( 0.05f, mImageStageScale.y * 0.9f );
      }
      else if ( event.keyPressedName == "o" )
      {
        mImageStageScale.x = std::max( 0.05f, mImageStageScale.x * 0.9f );
        mImageStageScale.y = std::max( 0.05f, mImageStageScale.y * 0.9f );
      }
      else if ( event.keyPressedName == "p" )
      {
        mImageStageScale.x = std::max( 0.05f, std::min( 1.0f, mImageStageScale.x * 1.1f ) );
        mImageStageScale.y = std::max( 0.05f, std::min( 1.0f, mImageStageScale.y * 1.1f ) );
      }
      else if ( event.keyPressedName == "n" )
      {
        mCurrentPath = mCurrentPath + 1;
        mCurrentPath = mCurrentPath <  NUM_IMAGE_PATHS ? mCurrentPath : 0;
      }
      else if ( event.keyPressedName == "b" )
      {
        mCurrentPath = mCurrentPath - 1;
        mCurrentPath = mCurrentPath >= 0 ? mCurrentPath : NUM_IMAGE_PATHS - 1;
      }
      // Cycle filter and scaling modes:
      else if ( event.keyPressedName == "f" )
      {
        mSamplingMode = NextFilterMode( mSamplingMode );
        mSamplingModeButton.SetLabel( StringFromFilterMode( mSamplingMode ) );
        mSamplingModeButton.GetLabel().SetProperty( Toolkit::Control::Property::STYLE_NAME, STYLE_BUTTON_TEXT );
      }
      // Cycle filter and scaling modes:
      else if ( event.keyPressedName == "s" )
      {
        mFittingMode = NextScalingMode( mFittingMode );
        mFittingModeButton.SetLabel( StringFromScalingMode( mFittingMode ) );
        mFittingModeButton.GetLabel().SetProperty( Toolkit::Control::Property::STYLE_NAME, STYLE_BUTTON_TEXT );
      }
      else
      {
        return;
      }

      ResizeImage();
    }
  }

private:
  void ResizeImage()
  {
    const char * const path = IMAGE_PATHS[mCurrentPath];

    Stage stage = Stage::GetCurrent();
    Size imageSize = stage.GetSize() * mImageStageScale;
    const ImageDimensions imageSizeInt = ImageDimensions::FromFloatArray( &imageSize.x );

    ResourceImage image = ResourceImage::New( path, imageSizeInt, mFittingMode, mSamplingMode );
    image.LoadingFinishedSignal().Connect( this, &ImageScalingAndFilteringController::OnImageLoaded );

    mNextImage = image;

    mDesiredBox.SetSize( stage.GetSize() * mImageStageScale );
    mHeightBox.SetSize( stage.GetSize().width,  (stage.GetSize() * mImageStageScale).height );
    mWidthBox.SetSize( (stage.GetSize() * mImageStageScale).width, stage.GetSize().height );
  }

private:
  Application&  mApplication;
  ImageActor mDesiredBox; //< Background rectangle to show requested image size.
  ImageActor mHeightBox;  //< Background horizontal stripe to show requested image height.
  ImageActor mWidthBox;   //< Background vertical stripe to show requested image width.
  Toolkit::PushButton mFittingModeButton;
  Toolkit::PushButton mSamplingModeButton;
  Toolkit::Popup mPopup;
  PinchGestureDetector mPinchDetector;
  float mLastPinchScale;
  Toolkit::PushButton  mGrabCorner;
  PanGestureDetector mPanGestureDetector;
  ImageActor mImageActor;
  ResourceImage mNextImage; //< Currently-loading image
  Vector2 mImageStageScale;
  int mCurrentPath;
  FittingMode::Type mFittingMode;
  SamplingMode::Type mSamplingMode;
};

void RunTest( Application& application )
{
  ImageScalingAndFilteringController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DALI_DEMO_THEME_PATH );

  RunTest( application );

  return 0;
}
