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

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "../shared/view.h"

#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace DemoHelper;

namespace // unnamed namespace
{

const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "bg-1024x1024.jpg" );
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char* QUIT_IMAGE( DALI_IMAGE_DIR "exit-button.png" );
const char* TEST_IMAGE( DALI_IMAGE_DIR "blocks-brick-1.png" );
const char* APPLICATION_TITLE( "Alignment" );

const Vector3 BUTTON_SIZE( 100.f, 50.f, 1.f );
const Vector3 BUTTON_SIZE2( 200.f, 50.f, 1.f );
const Vector4 BUTTON_BACKGROUND_COLOR( 0.4f, 0.4f, 0.4f, 1.f );
const Vector4 BUTTON_PRESSED_COLOR( 0.6f, 0.6f, 0.6f, 1.f );

const std::string HORIZONTAL_ALIGNMENT_STR( "H Alignment" );
const std::string VERTICAL_ALIGNMENT_STR( "V Alignment" );
const Vector3 HORIZONTAL_ALIGNMENT_BUTTON_POS( 50.f, 100.f, 0.5f );
const Vector3 VERTICAL_ALIGNMENT_BUTTON_POS( 50.f, 150.f, 0.5f );
const Vector3 HORIZONTAL_ALIGNMENT_TEXT_POS( 150.f, 100.f, 0.5f );
const Vector3 VERTICAL_ALIGNMENT_TEXT_POS( 150.f, 150.f, 0.5f );

const std::string WIDTH_STR( "Actor width" );
const std::string HEIGHT_STR( "Actor height" );
const Vector3 WIDTH_BUTTON_POS( 350.f, 100.f, 0.5f );
const Vector3 HEIGHT_BUTTON_POS( 350.f, 150.f, 0.5f );

const std::string SCALE_STR( "Scaling" );
const Vector3 SCALE_BUTTON_POS( 50.f, 200.f, 0.5f );
const Vector3 SCALE_TEXT_POS( 150.f, 200.f, 0.5f );

const std::string PADDING_STR_X( "Padding X" );
const Vector3 PADDING_BUTTON_POS_X( 50.f, 250.f, 0.5f );
const std::string PADDING_STR_Y( "Padding Y" );
const Vector3 PADDING_BUTTON_POS_Y( 150.f, 250.f, 0.5f );
const std::string PADDING_STR_Z( "Padding Z" );
const Vector3 PADDING_BUTTON_POS_Z( 250.f, 250.f, 0.5f );
const std::string PADDING_STR_W( "Padding W" );
const Vector3 PADDING_BUTTON_POS_W( 350.f, 250.f, 0.5f );

const char* HORIZONTAL_ALIGNMENT_TEXTS[] = { "Left", "Center", "Right" };
const char* VERTICAL_ALIGNMENT_TEXTS[] = { "Top", "Center", "Bottom" };
const float ACTOR_SIZES[] = { 50.f, 100.f, 300.f, 400.f };
const char* SCALE_TEXTS[] = { "ScaleNone", "ScaleToFill", "ScaleToFitKeepAspect", "ScaleToFillKeepAspect", "ShrinkToFit", "ShrinkToFitKeepAspect" };
const float ALIGNMENT_PADDING[] = { 0.f, 2.f, 4.f, 10.f };

} // unnamed namespace

// This example shows the usage of an alignment actor.
//
class AlignmentController : public ConnectionTracker
{
public:

  AlignmentController( Application& application )
  : mApplication( application ),
    mView(),
    mHorizontalAlignmentIndex( 1 ),
    mVerticalAlignmentIndex( 1 ),
    mScaleIndex( 0 ),
    mActorWidthIndex( 0 ),
    mActorHeightIndex( 0 ),
    mPaddingIndexX( 0 ),
    mPaddingIndexY( 0 ),
    mPaddingIndexZ( 0 ),
    mPaddingIndexW( 0 )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &AlignmentController::Create );
  }

  ~AlignmentController()
  {
    // Nothing to do here;
  }

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime

    // Hide the indicator bar
    application.GetWindow().ShowIndicator( Window::AUTO );

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    Toolkit::ToolBar toolBar;
    Layer contents = DemoHelper::CreateView( application,
                                             mView,
                                             toolBar,
                                             BACKGROUND_IMAGE,
                                             TOOLBAR_IMAGE,
                                             APPLICATION_TITLE );

    // Creates a quit button.
    Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New();
    alignmentContainer.SetScaling( Toolkit::Alignment::ShrinkToFitKeepAspect );
    Image image = Image::New( QUIT_IMAGE );
    alignmentContainer.Add( ImageActor::New( image ) );
    Toolkit::PushButton quitButton = Toolkit::PushButton::New();
    quitButton.SetBackgroundImage( alignmentContainer );

    // Connects the quit button clicked signal to the quit method.
    quitButton.ClickedSignal().Connect( this, &AlignmentController::OnQuitButtonClicked );

    toolBar.AddControl( quitButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalLeft );

    // Add an extra space on the right to center the title text.
    toolBar.AddControl( Actor::New(), DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight );

    Toolkit::PushButton horizontalAlignmentButton = Toolkit::PushButton::New();
    horizontalAlignmentButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    horizontalAlignmentButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    horizontalAlignmentButton.SetPosition( HORIZONTAL_ALIGNMENT_BUTTON_POS );
    horizontalAlignmentButton.SetSize( BUTTON_SIZE );
    horizontalAlignmentButton.SetBackgroundImage( Toolkit::CreateSolidColorActor( BUTTON_BACKGROUND_COLOR, true, Color::WHITE ) );
    horizontalAlignmentButton.SetSelectedImage( Toolkit::CreateSolidColorActor( BUTTON_PRESSED_COLOR ) );
    horizontalAlignmentButton.SetLabel( HORIZONTAL_ALIGNMENT_STR );

    horizontalAlignmentButton.ClickedSignal().Connect( this, &AlignmentController::OnHorizontalAlignmentButtonClicked );

    Toolkit::PushButton verticalAlignmentButton = Toolkit::PushButton::New();
    verticalAlignmentButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    verticalAlignmentButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    verticalAlignmentButton.SetPosition( VERTICAL_ALIGNMENT_BUTTON_POS );
    verticalAlignmentButton.SetSize( BUTTON_SIZE );
    verticalAlignmentButton.SetBackgroundImage( Toolkit::CreateSolidColorActor( BUTTON_BACKGROUND_COLOR, true, Color::WHITE ) );
    verticalAlignmentButton.SetSelectedImage( Toolkit::CreateSolidColorActor( BUTTON_PRESSED_COLOR ) );
    verticalAlignmentButton.SetLabel( VERTICAL_ALIGNMENT_STR );

    verticalAlignmentButton.ClickedSignal().Connect( this, &AlignmentController::OnVerticalAlignmentButtonClicked );

    mHorizontalAlignmentText = TextView::New( HORIZONTAL_ALIGNMENT_TEXTS[ mHorizontalAlignmentIndex ] );
    mHorizontalAlignmentText.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mHorizontalAlignmentText.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mHorizontalAlignmentText.SetPosition( HORIZONTAL_ALIGNMENT_TEXT_POS );

    mVerticalAlignmentText = TextView::New( VERTICAL_ALIGNMENT_TEXTS[ mVerticalAlignmentIndex ] );
    mVerticalAlignmentText.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mVerticalAlignmentText.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mVerticalAlignmentText.SetPosition( VERTICAL_ALIGNMENT_TEXT_POS );

    Toolkit::PushButton widthButton = Toolkit::PushButton::New();
    widthButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    widthButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    widthButton.SetPosition( WIDTH_BUTTON_POS );
    widthButton.SetSize( BUTTON_SIZE );
    widthButton.SetBackgroundImage( Toolkit::CreateSolidColorActor( BUTTON_BACKGROUND_COLOR, true, Color::WHITE ) );
    widthButton.SetSelectedImage( Toolkit::CreateSolidColorActor( BUTTON_PRESSED_COLOR ) );
    widthButton.SetLabel( WIDTH_STR );

    widthButton.ClickedSignal().Connect( this, &AlignmentController::OnWidthButtonClicked );

    Toolkit::PushButton heightButton = Toolkit::PushButton::New();
    heightButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    heightButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    heightButton.SetPosition( HEIGHT_BUTTON_POS );
    heightButton.SetSize( BUTTON_SIZE );
    heightButton.SetBackgroundImage( Toolkit::CreateSolidColorActor( BUTTON_BACKGROUND_COLOR, true, Color::WHITE ) );
    heightButton.SetSelectedImage( Toolkit::CreateSolidColorActor( BUTTON_PRESSED_COLOR ) );
    heightButton.SetLabel( HEIGHT_STR );

    heightButton.ClickedSignal().Connect( this, &AlignmentController::OnHeightButtonClicked );

    Toolkit::PushButton scaleButton = Toolkit::PushButton::New();
    scaleButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    scaleButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    scaleButton.SetPosition( SCALE_BUTTON_POS );
    scaleButton.SetSize( BUTTON_SIZE );
    scaleButton.SetBackgroundImage( Toolkit::CreateSolidColorActor( BUTTON_BACKGROUND_COLOR, true, Color::WHITE ) );
    scaleButton.SetSelectedImage( Toolkit::CreateSolidColorActor( BUTTON_PRESSED_COLOR ) );
    scaleButton.SetLabel( SCALE_STR );

    scaleButton.ClickedSignal().Connect( this, &AlignmentController::OnScaleButtonClicked );

    mScaleText = TextView::New( SCALE_TEXTS[ mScaleIndex ] );
    mScaleText.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mScaleText.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mScaleText.SetPosition( SCALE_TEXT_POS );

    Toolkit::PushButton paddingButtonX = Toolkit::PushButton::New();
    paddingButtonX.SetParentOrigin( ParentOrigin::TOP_LEFT );
    paddingButtonX.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    paddingButtonX.SetPosition( PADDING_BUTTON_POS_X );
    paddingButtonX.SetSize( BUTTON_SIZE );
    paddingButtonX.SetBackgroundImage( Toolkit::CreateSolidColorActor( BUTTON_BACKGROUND_COLOR, true, Color::WHITE ) );
    paddingButtonX.SetSelectedImage( Toolkit::CreateSolidColorActor( BUTTON_PRESSED_COLOR ) );
    paddingButtonX.SetLabel( PADDING_STR_X );

    paddingButtonX.ClickedSignal().Connect( this, &AlignmentController::OnPaddingButtonClickedX );

    Toolkit::PushButton paddingButtonY = Toolkit::PushButton::New();
    paddingButtonY.SetParentOrigin( ParentOrigin::TOP_LEFT );
    paddingButtonY.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    paddingButtonY.SetPosition( PADDING_BUTTON_POS_Y );
    paddingButtonY.SetSize( BUTTON_SIZE );
    paddingButtonY.SetBackgroundImage( Toolkit::CreateSolidColorActor( BUTTON_BACKGROUND_COLOR, true, Color::WHITE ) );
    paddingButtonY.SetSelectedImage( Toolkit::CreateSolidColorActor( BUTTON_PRESSED_COLOR ) );
    paddingButtonY.SetLabel( PADDING_STR_Y );

    paddingButtonY.ClickedSignal().Connect( this, &AlignmentController::OnPaddingButtonClickedY );

    Toolkit::PushButton paddingButtonZ = Toolkit::PushButton::New();
    paddingButtonZ.SetParentOrigin( ParentOrigin::TOP_LEFT );
    paddingButtonZ.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    paddingButtonZ.SetPosition( PADDING_BUTTON_POS_Z );
    paddingButtonZ.SetSize( BUTTON_SIZE );
    paddingButtonZ.SetBackgroundImage( Toolkit::CreateSolidColorActor( BUTTON_BACKGROUND_COLOR, true, Color::WHITE ) );
    paddingButtonZ.SetSelectedImage( Toolkit::CreateSolidColorActor( BUTTON_PRESSED_COLOR ) );
    paddingButtonZ.SetLabel( PADDING_STR_Z );

    paddingButtonZ.ClickedSignal().Connect( this, &AlignmentController::OnPaddingButtonClickedZ );

    Toolkit::PushButton paddingButtonW = Toolkit::PushButton::New();
    paddingButtonW.SetParentOrigin( ParentOrigin::TOP_LEFT );
    paddingButtonW.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    paddingButtonW.SetPosition( PADDING_BUTTON_POS_W );
    paddingButtonW.SetSize( BUTTON_SIZE );
    paddingButtonW.SetBackgroundImage( Toolkit::CreateSolidColorActor( BUTTON_BACKGROUND_COLOR, true, Color::WHITE ) );
    paddingButtonW.SetSelectedImage( Toolkit::CreateSolidColorActor( BUTTON_PRESSED_COLOR ) );
    paddingButtonW.SetLabel( PADDING_STR_W );

    paddingButtonW.ClickedSignal().Connect( this, &AlignmentController::OnPaddingButtonClickedW );

    ImageActor frame = Toolkit::CreateSolidColorActor( Color::BLACK, true, Color::WHITE );
    frame.SetParentOrigin( ParentOrigin::CENTER );
    frame.SetAnchorPoint( AnchorPoint::CENTER );
    frame.SetSize( 200.f, 200.f );
    frame.SetPosition( 0.f, 165.f );
    frame.SetSortModifier( 1.f );

    mAlignment = Toolkit::Alignment::New();
    mAlignment.SetAlignmentType( static_cast<Toolkit::Alignment::Type>( Toolkit::Alignment::HorizontalCenter | Toolkit::Alignment::VerticalCenter ) );
    mAlignment.SetParentOrigin( ParentOrigin::CENTER );
    mAlignment.SetAnchorPoint( AnchorPoint::CENTER );
    mAlignment.SetPadding( Toolkit::Alignment::Padding( ALIGNMENT_PADDING[mPaddingIndexX], ALIGNMENT_PADDING[mPaddingIndexY], ALIGNMENT_PADDING[mPaddingIndexZ], ALIGNMENT_PADDING[mPaddingIndexW] ) );
    mAlignment.SetSize( 200.f,200.f );

    frame.Add( mAlignment );

    //mRed = Toolkit::CreateSolidColorActor( Color::RED );
    //mRed.SetSize( ACTOR_SIZES[mActorWidthIndex], ACTOR_SIZES[mActorHeightIndex] );
    mRed = ImageActor::New(Image::New(TEST_IMAGE));
    //mRed.SetSortModifier( -1.f );

    mAlignment.Add( mRed );

/*    // create the text actor
       TextView text = TextView::New( "TEST" );
       // anchor text from its center point to the middle of its parent
       //text.SetAnchorPoint( AnchorPoint::CENTER );
       //text.SetParentOrigin( ParentOrigin::CENTER );
       // align text to the middle
       text.SetTextAlignment( Alignment::Type( Alignment::HorizontalCenter | Alignment::VerticalCenter ) );
       text.SetColor( Color::WHITE );
       // move text a bit forwards
       text.SetZ( 1 );
       text.SetSize( ACTOR_SIZES[mActorWidthIndex], ACTOR_SIZES[mActorHeightIndex] );
       // make the text 90% of tile
       //text.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), RelativeToConstraint( Vector3( 0.5f, 0.5f, 0.5f ) ) ) );
       mAlignment.Add( text );*/

    // Display the actor on the stage
    contents.Add( frame );
    contents.Add( horizontalAlignmentButton );
    contents.Add( verticalAlignmentButton );
    contents.Add( mHorizontalAlignmentText );
    contents.Add( mVerticalAlignmentText );
    contents.Add( widthButton );
    contents.Add( heightButton );
    contents.Add( scaleButton );
    contents.Add( mScaleText );
    contents.Add( paddingButtonX );
    contents.Add( paddingButtonY );
    contents.Add( paddingButtonW );
    contents.Add( paddingButtonZ );
  }

  bool OnQuitButtonClicked( Toolkit::Button button )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  bool OnHorizontalAlignmentButtonClicked( Toolkit::Button button )
  {
    ++mHorizontalAlignmentIndex;
    mHorizontalAlignmentIndex %= ( sizeof( HORIZONTAL_ALIGNMENT_TEXTS ) / sizeof( const char* ) );

    mHorizontalAlignmentText.SetText( HORIZONTAL_ALIGNMENT_TEXTS[ mHorizontalAlignmentIndex ] );

    Toolkit::Alignment::Type horizontal = Toolkit::Alignment::HorizontalLeft;
    switch( mHorizontalAlignmentIndex )
    {
    case 0:
      horizontal = Toolkit::Alignment::HorizontalLeft;
      break;
    case 1:
      horizontal = Toolkit::Alignment::HorizontalCenter;
      break;
    case 2:
      horizontal = Toolkit::Alignment::HorizontalRight;
      break;
    }

    Toolkit::Alignment::Type vertical = Toolkit::Alignment::VerticalTop;
    switch( mVerticalAlignmentIndex )
    {
    case 0:
      vertical = Toolkit::Alignment::VerticalTop;
      break;
    case 1:
      vertical = Toolkit::Alignment::VerticalCenter;
      break;
    case 2:
      vertical = Toolkit::Alignment::VerticalBottom;
      break;
    }

    mAlignment.SetAlignmentType( static_cast<Toolkit::Alignment::Type>( horizontal | vertical ) );

    return true;
  }

  bool OnVerticalAlignmentButtonClicked( Toolkit::Button button )
  {
    ++mVerticalAlignmentIndex;
    mVerticalAlignmentIndex %= ( sizeof( VERTICAL_ALIGNMENT_TEXTS ) / sizeof( const char* ) );

    mVerticalAlignmentText.SetText( VERTICAL_ALIGNMENT_TEXTS[ mVerticalAlignmentIndex ] );

    Toolkit::Alignment::Type horizontal = Toolkit::Alignment::HorizontalLeft;
    switch( mHorizontalAlignmentIndex )
    {
    case 0:
      horizontal = Toolkit::Alignment::HorizontalLeft;
      break;
    case 1:
      horizontal = Toolkit::Alignment::HorizontalCenter;
      break;
    case 2:
      horizontal = Toolkit::Alignment::HorizontalRight;
      break;
    }

    Toolkit::Alignment::Type vertical = Toolkit::Alignment::VerticalTop;
    switch( mVerticalAlignmentIndex )
    {
    case 0:
      vertical = Toolkit::Alignment::VerticalTop;
      break;
    case 1:
      vertical = Toolkit::Alignment::VerticalCenter;
      break;
    case 2:
      vertical = Toolkit::Alignment::VerticalBottom;
      break;
    }

    mAlignment.SetAlignmentType( static_cast<Toolkit::Alignment::Type>( horizontal | vertical ) );

    return true;
  }

  bool OnWidthButtonClicked( Toolkit::Button button )
  {
    ++mActorWidthIndex;
    mActorWidthIndex %= ( sizeof( ACTOR_SIZES ) / sizeof( float ) );

    mRed.SetSize( ACTOR_SIZES[mActorWidthIndex], ACTOR_SIZES[mActorHeightIndex] );

    return true;
  }

  bool OnHeightButtonClicked( Toolkit::Button button )
  {
    ++mActorHeightIndex;
    mActorHeightIndex %= ( sizeof( ACTOR_SIZES ) / sizeof( float ) );

    mRed.SetSize( ACTOR_SIZES[mActorWidthIndex], ACTOR_SIZES[mActorHeightIndex] );

    return true;
  }

  bool OnScaleButtonClicked( Toolkit::Button button )
  {
    ++mScaleIndex;
    mScaleIndex %= ( sizeof( SCALE_TEXTS ) / sizeof( const char* ) );

    mScaleText.SetText( SCALE_TEXTS[ mScaleIndex ] );

    switch( mScaleIndex )
    {
    case 0:
      mAlignment.SetScaling( Toolkit::Alignment::ScaleNone );
      break;
    case 1:
      mAlignment.SetScaling( Toolkit::Alignment::ScaleToFill );
      break;
    case 2:
      mAlignment.SetScaling( Toolkit::Alignment::ScaleToFitKeepAspect );
      break;
    case 3:
      mAlignment.SetScaling( Toolkit::Alignment::ScaleToFillKeepAspect );
      break;
    case 4:
      mAlignment.SetScaling( Toolkit::Alignment::ShrinkToFit );
      break;
    case 5:
      mAlignment.SetScaling( Toolkit::Alignment::ShrinkToFitKeepAspect );
      break;
    }
    return true;
  }

  bool OnPaddingButtonClickedX( Toolkit::Button button )
  {
    ++mPaddingIndexX;
    mPaddingIndexX %= ( sizeof( ALIGNMENT_PADDING ) / sizeof( float ) );

    mAlignment.SetPadding( Toolkit::Alignment::Padding( ALIGNMENT_PADDING[mPaddingIndexX], ALIGNMENT_PADDING[mPaddingIndexY], ALIGNMENT_PADDING[mPaddingIndexZ], ALIGNMENT_PADDING[mPaddingIndexW] ) );
    return true;
  }

  bool OnPaddingButtonClickedY( Toolkit::Button button )
  {
    ++mPaddingIndexY;
    mPaddingIndexY %= ( sizeof( ALIGNMENT_PADDING ) / sizeof( float ) );

    mAlignment.SetPadding( Toolkit::Alignment::Padding( ALIGNMENT_PADDING[mPaddingIndexX], ALIGNMENT_PADDING[mPaddingIndexY], ALIGNMENT_PADDING[mPaddingIndexZ], ALIGNMENT_PADDING[mPaddingIndexW] ) );
    return true;
  }

  bool OnPaddingButtonClickedZ( Toolkit::Button button )
  {
    ++mPaddingIndexZ;
    mPaddingIndexZ %= ( sizeof( ALIGNMENT_PADDING ) / sizeof( float ) );

    mAlignment.SetPadding( Toolkit::Alignment::Padding( ALIGNMENT_PADDING[mPaddingIndexX], ALIGNMENT_PADDING[mPaddingIndexY], ALIGNMENT_PADDING[mPaddingIndexZ], ALIGNMENT_PADDING[mPaddingIndexW] ) );
    return true;
  }

  bool OnPaddingButtonClickedW( Toolkit::Button button )
  {
    ++mPaddingIndexW;
    mPaddingIndexW %= ( sizeof( ALIGNMENT_PADDING ) / sizeof( float ) );

    mAlignment.SetPadding( Toolkit::Alignment::Padding( ALIGNMENT_PADDING[mPaddingIndexX], ALIGNMENT_PADDING[mPaddingIndexY], ALIGNMENT_PADDING[mPaddingIndexZ], ALIGNMENT_PADDING[mPaddingIndexW] ) );
    return true;
  }

private:
  Application&       mApplication;
  Toolkit::View      mView;
  Toolkit::Alignment mAlignment;
  TextView          mHorizontalAlignmentText;
  TextView          mVerticalAlignmentText;
  TextView          mScaleText;
  ImageActor         mRed;
  unsigned int       mHorizontalAlignmentIndex;
  unsigned int       mVerticalAlignmentIndex;
  unsigned int       mScaleIndex;
  unsigned int       mActorWidthIndex;
  unsigned int       mActorHeightIndex;
  unsigned int       mPaddingIndexX;
  unsigned int       mPaddingIndexY;
  unsigned int       mPaddingIndexZ;
  unsigned int       mPaddingIndexW;
};

void RunTest( Application& application )
{
  AlignmentController test( application );

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
