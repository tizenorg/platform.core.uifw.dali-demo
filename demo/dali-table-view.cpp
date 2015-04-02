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

// CLASS HEADER
#include "dali-table-view.h"

// EXTERNAL INCLUDES
#include <algorithm>
#include <sstream>
#include <unistd.h>

// INTERNAL INCLUDES
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

///////////////////////////////////////////////////////////////////////////////

namespace
{

const std::string BUTTON_BACKWARD( "Backward" );
const std::string BUTTON_FORWARD( "Forward" );
const std::string BUTTON_QUIT( "Quit" );
const std::string BUTTON_OK( "Ok" );
const std::string BUTTON_CANCEL( "Cancel" );

const std::string DEFAULT_BACKGROUND_IMAGE_PATH( DALI_IMAGE_DIR "background-gradient.jpg" );
const std::string LOGO_PATH( DALI_IMAGE_DIR "dali-logo.png" );
const std::string DEFAULT_TOOLBAR_IMAGE_PATH( DALI_IMAGE_DIR "top-bar.png" );
const std::string BUTTON_BACKGROUND(DALI_IMAGE_DIR "button-background.png");
const std::string TILE_BACKGROUND(DALI_IMAGE_DIR "item-background.png");
const std::string TILE_BACKGROUND_ALPHA(DALI_IMAGE_DIR "item-background-alpha.png");

const char * const DEFAULT_TOOLBAR_TEXT( "TOUCH TO LAUNCH EXAMPLE" );

const float BUTTON_PRESS_ANIMATION_TIME = 0.25f;                ///< Time to perform button scale effect.
const float ROTATE_ANIMATION_TIME = 0.5f;                       ///< Time to perform rotate effect.
const int MAX_PAGES = 256;                                      ///< Maximum pages (arbitrary safety limit)
const int EXAMPLES_PER_ROW = 3;
const int ROWS_PER_PAGE = 3;
const int EXAMPLES_PER_PAGE = EXAMPLES_PER_ROW * ROWS_PER_PAGE;
const float LOGO_MARGIN_RATIO = 0.1f / 0.3f;
const float BOTTOM_PADDING_RATIO = 0.4f / 0.9f;
const Vector3 SCROLLVIEW_RELATIVE_SIZE(0.9f, 1.0f, 0.8f );     ///< ScrollView's relative size to its parent
const Vector3 TABLE_RELATIVE_SIZE(0.95f, 0.9f, 0.8f );          ///< TableView's relative size to the entire stage. The Y value means sum of the logo and table relative heights.
const float STENCIL_RELATIVE_SIZE = 1.0f;

const float EFFECT_SNAP_DURATION = 0.66f;                       ///< Scroll Snap Duration for Effects
const float EFFECT_FLICK_DURATION = 0.5f;                       ///< Scroll Flick Duration for Effects
const Vector3 ANGLE_CUBE_PAGE_ROTATE(Math::PI * 0.5f, Math::PI * 0.5f, 0.0f);

const int NUM_BACKGROUND_IMAGES = 18;
const float BACKGROUND_SWIPE_SCALE = 0.025f;
const float BACKGROUND_SPREAD_SCALE = 1.5f;
const float SCALE_MOD = 1000.0f * Math::PI * 2.0f;
const float SCALE_SPEED = 10.0f;
const float SCALE_SPEED_SIN = 0.1f;

const unsigned int BACKGROUND_ANIMATION_DURATION = 15000; // 15 secs

const float BACKGROUND_Z = -1.0f;
const Vector4 BACKGROUND_COLOR( 1.0f, 1.0f, 1.0f, 1.0f );

const float BUBBLE_MIN_Z = -1.0;
const float BUBBLE_MAX_Z = 0.0f;

// 3D Effect constants
const Vector2 ANGLE_SWING_3DEFFECT( Math::PI_2 * 0.75, Math::PI_2 * 0.75f ); ///< Angle Swing in radians
const Vector2 POSITION_SWING_3DEFFECT( 0.55f, 0.4f );             ///< Position Swing relative to stage size.
const Vector3 ANCHOR_3DEFFECT_STYLE0( -105.0f, 30.0f, -240.0f ); ///< Rotation Anchor position for 3D Effect (Style 0)
const Vector3 ANCHOR_3DEFFECT_STYLE1( 65.0f, -70.0f, -500.0f );  ///< Rotation Anchor position for 3D Effect (Style 1)

const std::string             DEFAULT_TEXT_STYLE_FONT_FAMILY("HelveticaNeue");
const std::string             DEFAULT_TEXT_STYLE_FONT_STYLE("Regular");
const Dali::PointSize         DEFAULT_TEXT_STYLE_POINT_SIZE( 8.0f );
const Dali::TextStyle::Weight DEFAULT_TEXT_STYLE_WEIGHT(Dali::TextStyle::REGULAR);
const Dali::Vector4           DEFAULT_TEXT_STYLE_COLOR(0.7f, 0.7f, 0.7f, 1.0f);

const std::string             TABLE_TEXT_STYLE_FONT_FAMILY("HelveticaNeue");
const std::string             TABLE_TEXT_STYLE_FONT_STYLE("Regular");
const Dali::PointSize         TABLE_TEXT_STYLE_POINT_SIZE( 8.0f );
const Dali::TextStyle::Weight TABLE_TEXT_STYLE_WEIGHT(Dali::TextStyle::LIGHT);
const Dali::Vector4           TABLE_TEXT_STYLE_COLOR(0.0f, 0.0f, 0.0f, 1.0f);

Vector3 ScalePointSize(const Vector3& vec)
{
  return Vector3( DemoHelper::ScalePointSize( vec.x ), DemoHelper::ScalePointSize( vec.y ), DemoHelper::ScalePointSize( vec.z ) );
}

#define DP(x) DemoHelper::ScalePointSize(x)

TextStyle GetTableTextStyle()
{
  TextStyle textStyle;
  textStyle.SetFontName(TABLE_TEXT_STYLE_FONT_FAMILY);
  textStyle.SetFontStyle(TABLE_TEXT_STYLE_FONT_STYLE);
  textStyle.SetFontPointSize( Dali::PointSize(DemoHelper::ScalePointSize(TABLE_TEXT_STYLE_POINT_SIZE)));
  textStyle.SetWeight(TABLE_TEXT_STYLE_WEIGHT);
  textStyle.SetTextColor(TABLE_TEXT_STYLE_COLOR);
  return textStyle;
}

/**
 * Creates the background image
 */
ImageActor CreateBackground( std::string imagePath )
{
  Image image = ResourceImage::New( imagePath );
  ImageActor background = ImageActor::New( image );
  background.SetName( "BACKGROUND" );
  background.SetAnchorPoint( AnchorPoint::CENTER );
  background.SetParentOrigin( ParentOrigin::CENTER );
  background.SetZ( -1.0f );
  background.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );

  return background;
}

// These values depend on the tile image
const float IMAGE_BORDER_LEFT = 11.0f;
const float IMAGE_BORDER_RIGHT = IMAGE_BORDER_LEFT;
const float IMAGE_BORDER_TOP = IMAGE_BORDER_LEFT;
const float IMAGE_BORDER_BOTTOM = IMAGE_BORDER_LEFT;

/**
 * Constraint to return a position for a bubble based on the scroll value and vertical wrapping
 */
struct AnimateBubbleConstraint
{
public:
  AnimateBubbleConstraint( const Vector3& initialPos, float scale, float size )
      : mInitialX( initialPos.x ),
        mScale( scale ),
        mShapeSize( size )
  {
  }

  void operator()( Vector3& position, const PropertyInputContainer& inputs )
  {
    const Vector3& parentSize = inputs[1]->GetVector3();

    // Wrap bubbles verically.
    if( position.y + mShapeSize * 0.5f < -parentSize.y * 0.5f )
    {
      position.y = parentSize.y * 0.5f + mShapeSize * 0.5f;
    }

    // Bubbles X position moves parallax to horizontal
    // panning by a scale factor unique to each bubble.
    position.x = mInitialX + ( inputs[0]->GetVector3().x * mScale );
  }

private:
  float mInitialX;
  float mScale;
  float mShapeSize;
};

bool CompareByTitle( const Example& lhs, const Example& rhs )
{
  return lhs.title < rhs.title;
}

} // namespace

DaliTableView::DaliTableView( Application& application )
: mApplication( application ),
  mBackgroundLayer(),
  mRootActor(),
  mRotateAnimation(),
  mBackground(),
  mPressedAnimation(),
  mScrollViewLayer(),
  mScrollView(),
  mScrollViewEffect(),
  mScrollRulerX(),
  mScrollRulerY(),
  mButtons(),
  mPressedActor(),
  mAnimationTimer(),
  mLogoTapDetector(),
  mVersionPopup(),
  mButtonsPageRelativeSize(),
  mPages(),
  mTableViewImages(),
  mBackgroundActors(),
  mBackgroundAnimations(),
  mExampleList(),
  mExampleMap(),
  mBackgroundImagePath( DEFAULT_BACKGROUND_IMAGE_PATH ),
  mTotalPages(),
  mScrolling( false ),
  mSortAlphabetically( false ),
  mBackgroundAnimsPlaying( false ),
  mVersionPopupShown( false )
{
  application.InitSignal().Connect( this, &DaliTableView::Initialize );
}

DaliTableView::~DaliTableView()
{
}

void DaliTableView::AddExample( Example example )
{
  mExampleList.push_back( example );
  mExampleMap[ example.name ] = example;
}

void DaliTableView::SetBackgroundPath( std::string imagePath )
{
  mBackgroundImagePath = imagePath;
}

void DaliTableView::SortAlphabetically( bool sortAlphabetically )
{
  mSortAlphabetically = sortAlphabetically;
}

void DaliTableView::Initialize( Application& application )
{
  Stage::GetCurrent().KeyEventSignal().Connect( this, &DaliTableView::OnKeyEvent );

  const Vector2 stageSize = Stage::GetCurrent().GetSize();

  // Background
  Actor background = CreateBackground( mBackgroundImagePath );
  Stage::GetCurrent().Add( background );

  // Render entire content as overlays, as is all on same 2D plane.
  mRootActor = TableView::New( 4, 1 );
  mRootActor.SetAnchorPoint( AnchorPoint::CENTER );
  mRootActor.SetParentOrigin( ParentOrigin::CENTER );
  mRootActor.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
  Stage::GetCurrent().Add( mRootActor );

  // Toolbar at top
  Dali::Toolkit::ToolBar toolbar;
  Dali::Layer toolBarLayer = DemoHelper::CreateToolbar(toolbar,
                                                       DEFAULT_TOOLBAR_IMAGE_PATH,
                                                       DEFAULT_TOOLBAR_TEXT,
                                                       DemoHelper::DEFAULT_VIEW_STYLE,
                                                       DemoHelper::GetDefaultTextStyle());

  mRootActor.AddChild( toolBarLayer, TableView::CellPosition( 0, 0 ) );
  mRootActor.SetFitHeight( 0 );

  // Add logo
  Dali::ImageActor logo = CreateLogo( LOGO_PATH );
  logo.SetName( "LOGO_IMAGE" );
  logo.SetResizePolicy( USE_NATURAL_SIZE, ALL_DIMENSIONS );
  const float paddingHeight = ( ( 1.f-TABLE_RELATIVE_SIZE.y ) * stageSize.y );
  const float logoMargin = paddingHeight * LOGO_MARGIN_RATIO;

  // Show version in a popup when log is tapped
  mLogoTapDetector = TapGestureDetector::New();
  mLogoTapDetector.Attach( logo );
  mLogoTapDetector.DetectedSignal().Connect( this, &DaliTableView::OnLogoTapped );

  const float bottomMargin = paddingHeight * BOTTOM_PADDING_RATIO;

  Alignment alignment = Alignment::New();
  alignment.SetName( "LOGO_ALIGNMENT" );
  alignment.Add( logo );
  alignment.SetResizePolicy( FILL_TO_PARENT, WIDTH );
  alignment.SetResizePolicy( FIT_TO_CHILDREN, HEIGHT );
  Actor alignmentActor = alignment;
  alignmentActor.SetPadding( Padding( 0.0f, 0.0f, logoMargin, logoMargin ));
  mRootActor.AddChild( alignment, TableView::CellPosition( 1, 0 ) );
  mRootActor.SetFitHeight( 1 );

  // scrollview occupying the majority of the screen
  mScrollView = ScrollView::New();
  mScrollView.SetRelayoutEnabled( true );

  mScrollView.SetAnchorPoint( AnchorPoint::CENTER );
  mScrollView.SetParentOrigin( ParentOrigin::CENTER );
  mScrollView.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
  const float buttonsPageMargin = ( 1.0f - TABLE_RELATIVE_SIZE.x ) * 0.5f * stageSize.width;
  mScrollView.SetPadding( Padding( buttonsPageMargin, buttonsPageMargin, 0.0f, 0.0f ) );

  mScrollView.SetAxisAutoLock( true );
  mScrollView.ScrollCompletedSignal().Connect( this, &DaliTableView::OnScrollComplete );
  mScrollView.ScrollStartedSignal().Connect( this, &DaliTableView::OnScrollStart );
  mScrollView.TouchedSignal().Connect( this, &DaliTableView::OnScrollTouched );

  mScrollViewLayer = Layer::New();
  mScrollViewLayer.SetAnchorPoint( AnchorPoint::CENTER );
  mScrollViewLayer.SetParentOrigin( ParentOrigin::CENTER );
  mScrollViewLayer.SetDrawMode( DrawMode::OVERLAY );
  mScrollViewLayer.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );

  // Create solid background colour.
  ImageActor backgroundColourActor = Dali::Toolkit::CreateSolidColorActor( BACKGROUND_COLOR );
  backgroundColourActor.SetAnchorPoint( AnchorPoint::CENTER );
  backgroundColourActor.SetParentOrigin( ParentOrigin::CENTER );
  backgroundColourActor.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
  backgroundColourActor.SetSizeMode( SIZE_RELATIVE_TO_PARENT );
  backgroundColourActor.SetSizeModeFactor( Vector3( 1.0f, 1.5f, 1.0f ) );
  backgroundColourActor.SetZ( BACKGROUND_Z );
  mScrollViewLayer.Add( backgroundColourActor );

  // Populate background and bubbles - needs to be scrollViewLayer so scroll ends show
  Actor bubbleContainer = Actor::New();
  bubbleContainer.SetRelayoutEnabled( true );
  bubbleContainer.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
  bubbleContainer.SetAnchorPoint( AnchorPoint::CENTER );
  bubbleContainer.SetParentOrigin( ParentOrigin::CENTER );
  mScrollViewLayer.Add( bubbleContainer );

  SetupBackground( bubbleContainer );

  Alignment buttonsAlignment = Alignment::New();
  buttonsAlignment.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
  buttonsAlignment.Add( mScrollViewLayer );

  mScrollViewLayer.Add( mScrollView );

  mRootActor.AddChild( buttonsAlignment, TableView::CellPosition( 2, 0 ) );

  mRootActor.SetFixedHeight( 3, bottomMargin );

  // Add scroll view effect and setup constraints on pages
  ApplyScrollViewEffect();

  // Add pages and tiles
  Populate();

  // Remove constraints for inner cube effect
  ApplyCubeEffectToActors();

  // Set initial orientation
  unsigned int degrees = application.GetOrientation().GetDegrees();
  Rotate( degrees );

  Dali::Window winHandle = application.GetWindow();
  winHandle.AddAvailableOrientation( Dali::Window::PORTRAIT );
  winHandle.RemoveAvailableOrientation( Dali::Window::LANDSCAPE );
  winHandle.AddAvailableOrientation( Dali::Window::PORTRAIT_INVERSE );
  winHandle.RemoveAvailableOrientation( Dali::Window::LANDSCAPE_INVERSE );

  Dali::Orientation orientation = winHandle.GetOrientation();
  orientation.ChangedSignal().Connect( this, &DaliTableView::OrientationChanged );

  winHandle.ShowIndicator( Dali::Window::INVISIBLE );

  // Background animation
  mAnimationTimer = Timer::New( BACKGROUND_ANIMATION_DURATION );
  mAnimationTimer.TickSignal().Connect( this, &DaliTableView::PauseBackgroundAnimation );
  mAnimationTimer.Start();
  mBackgroundAnimsPlaying = true;

  KeyboardFocusManager::Get().PreFocusChangeSignal().Connect( this, &DaliTableView::OnKeyboardPreFocusChange );
  KeyboardFocusManager::Get().FocusedActorActivatedSignal().Connect( this, &DaliTableView::OnFocusedActorActivated );
}

void DaliTableView::ApplyCubeEffectToActors()
{
  for( ActorIter pageIter = mPages.begin(); pageIter != mPages.end(); ++pageIter )
  {
    Actor page = *pageIter;

    for( unsigned int i = 0, numChildren = page.GetChildCount(); i < numChildren; ++i)
    {
      // Remove old effect's manual constraints.
      Actor child = page.GetChildAt(i);
      if( child )
      {
        ApplyCubeEffectToActor( child );
      }
    }
  }
}

void DaliTableView::OnButtonsPageRelayout( const Dali::Actor& actor )
{

}

void DaliTableView::Populate()
{
  const Vector2 stageSize = Stage::GetCurrent().GetSize();

  mTotalPages = ( mExampleList.size() + EXAMPLES_PER_PAGE - 1 ) / EXAMPLES_PER_PAGE;

  // Populate ScrollView.
  if( mExampleList.size() > 0 )
  {
    if( mSortAlphabetically )
    {
      sort( mExampleList.begin(), mExampleList.end(), CompareByTitle );
    }

    unsigned int exampleCount = 0;
    ExampleListConstIter iter = mExampleList.begin();

    for( int t = 0; t < mTotalPages; t++ )
    {
      // Create Table. (contains up to 9 Examples)
      TableView page = TableView::New( 3, 3 );
      page.SetAnchorPoint( AnchorPoint::CENTER );
      page.SetParentOrigin( ParentOrigin::CENTER );
      page.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
      mScrollView.Add( page );

      // Calculate the number of images going across (columns) within a page, according to the screen resolution and dpi.
      const float margin = 2.0f;
      const float tileParentMultiplier = 1.0f / EXAMPLES_PER_ROW;

      for(int row = 0; row < ROWS_PER_PAGE; row++)
      {
        for(int column = 0; column < EXAMPLES_PER_ROW; column++)
        {
          const Example& example = ( *iter );

          Actor tile = CreateTile( example.name, example.title, Vector3( tileParentMultiplier, tileParentMultiplier, 1.0f ), true );
          FocusManager focusManager = FocusManager::Get();
          focusManager.SetFocusOrder( tile, ++exampleCount );
          focusManager.SetAccessibilityAttribute( tile, Dali::Toolkit::FocusManager::ACCESSIBILITY_LABEL,
                                                  example.title );
          focusManager.SetAccessibilityAttribute( tile, Dali::Toolkit::FocusManager::ACCESSIBILITY_TRAIT, "Tile" );
          focusManager.SetAccessibilityAttribute( tile, Dali::Toolkit::FocusManager::ACCESSIBILITY_HINT,
                                                  "You can run this example" );

          tile.SetPadding( Padding( margin, margin, margin, margin ) );

          page.AddChild( tile, TableView::CellPosition( row, column ) );

          iter++;

          if( iter == mExampleList.end() )
          {
            break;
          }
        }

        if( iter == mExampleList.end() )
        {
          break;
        }
      }

      // Set tableview position
      Vector3 pagePos( stageSize.width * TABLE_RELATIVE_SIZE.x * t, 0.0f, 0.0f );
      page.SetPosition( pagePos );

      mPages.push_back( page );

      if( iter == mExampleList.end() )
      {
        break;
      }
    }
  }

  // Update Ruler info.
  mScrollRulerX = new FixedRuler( stageSize.width * TABLE_RELATIVE_SIZE.x );
  mScrollRulerY = new DefaultRuler();
  mScrollRulerX->SetDomain( RulerDomain( 0.0f, mTotalPages * stageSize.width * TABLE_RELATIVE_SIZE.x, true ) );
  mScrollRulerY->Disable();
  mScrollView.SetRulerX( mScrollRulerX );
  mScrollView.SetRulerY( mScrollRulerY );
}

void DaliTableView::OrientationChanged( Orientation orientation )
{
  // TODO: Implement if orientation change required
}

void DaliTableView::Rotate( unsigned int degrees )
{
  // Resize the root actor
  Vector2 stageSize = Stage::GetCurrent().GetSize();
  Vector3 targetSize( stageSize.x, stageSize.y, 1.0f );

  if( degrees == 90 || degrees == 270 )
  {
    targetSize = Vector3( stageSize.y, stageSize.x, 1.0f );
  }

  if( mRotateAnimation )
  {
    mRotateAnimation.Stop();
    mRotateAnimation.Clear();
  }

  mRotateAnimation = Animation::New( ROTATE_ANIMATION_TIME );
  mRotateAnimation.RotateTo( mRootActor, Degree( 360 - degrees ), Vector3::ZAXIS, AlphaFunctions::EaseOut );
  mRotateAnimation.Resize( mRootActor, targetSize, AlphaFunctions::EaseOut );
  mRotateAnimation.Play();
}

Actor DaliTableView::CreateTile( const std::string& name, const std::string& title, const Dali::Vector3& sizeMultiplier, bool addBackground )
{
  Actor content = Actor::New();
  content.SetName( name );
  content.SetAnchorPoint( AnchorPoint::CENTER );
  content.SetParentOrigin( ParentOrigin::CENTER );
  content.SetRelayoutEnabled( true );
  content.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
  content.SetSizeMode( SIZE_RELATIVE_TO_PARENT );
  content.SetSizeModeFactor( sizeMultiplier );

  // create background image
  if( addBackground )
  {
    Image bg = ResourceImage::New( TILE_BACKGROUND );
    ImageActor image = ImageActor::New( bg );
    image.SetAnchorPoint( AnchorPoint::CENTER );
    image.SetParentOrigin( ParentOrigin::CENTER );
    // make the image 100% of tile
    image.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );

    // move image back to get text appear in front
    image.SetZ( -1 );
    image.SetStyle( ImageActor::STYLE_NINE_PATCH );
    image.SetNinePatchBorder( Vector4( IMAGE_BORDER_LEFT, IMAGE_BORDER_TOP, IMAGE_BORDER_RIGHT, IMAGE_BORDER_BOTTOM ) );
    content.Add( image );

    // Add stencil
    ImageActor stencil = NewStencilImage();
    stencil.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
    image.Add( stencil );
  }

  TextView text = TextView::New( title );
  text.SetAnchorPoint( AnchorPoint::CENTER );
  text.SetParentOrigin( ParentOrigin::CENTER );
  text.SetWidthExceedPolicy( Toolkit::TextView::ShrinkToFit );
  text.SetMultilinePolicy( Toolkit::TextView::SplitByWord );
  text.SetLineJustification( Toolkit::TextView::Center );
  text.SetTextAlignment( Toolkit::Alignment::Type( Alignment::HorizontalCenter | Alignment::VerticalCenter ) );
  text.SetZ( 1 );
  // make the text 90% of tile
  text.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
  text.SetSizeMode( SIZE_RELATIVE_TO_PARENT );
  text.SetSizeModeFactor( Vector3( 0.9f, 0.9f, 0.0f ) );
  text.SetStyleToCurrentText( GetTableTextStyle() );
  text.SetSnapshotModeEnabled( false );
  content.Add( text );

  // Set the tile to be keyboard focusable
  content.SetKeyboardFocusable(true);

  // connect to the touch events
  content.TouchedSignal().Connect( this, &DaliTableView::OnTilePressed );
  content.HoveredSignal().Connect( this, &DaliTableView::OnTileHovered );

  return content;
}

ImageActor DaliTableView::NewStencilImage()
{
  Image alpha = ResourceImage::New( TILE_BACKGROUND_ALPHA );

  ImageActor stencilActor = ImageActor::New( alpha );
  stencilActor.SetStyle( ImageActor::STYLE_NINE_PATCH );
  stencilActor.SetNinePatchBorder( Vector4( IMAGE_BORDER_LEFT, IMAGE_BORDER_TOP, IMAGE_BORDER_RIGHT, IMAGE_BORDER_BOTTOM ) );

  stencilActor.SetParentOrigin( ParentOrigin::CENTER );
  stencilActor.SetAnchorPoint( AnchorPoint::CENTER );
  stencilActor.SetDrawMode( DrawMode::STENCIL );

  Dali::ShaderEffect shaderEffect = AlphaDiscardEffect::New();
  stencilActor.SetShaderEffect( shaderEffect );

  return stencilActor;
}

bool DaliTableView::OnTilePressed( Actor actor, const TouchEvent& event )
{
  bool consumed = false;

  const TouchPoint& point = event.GetPoint( 0 );
  if( TouchPoint::Down == point.state )
  {
    mPressedActor = actor;
    consumed = true;
  }

  // A button press is only valid if the Down & Up events
  // both occurred within the button.
  if( ( TouchPoint::Up == point.state ) &&
      ( mPressedActor == actor ) )
  {
    std::string name = actor.GetName();
    ExampleMapConstIter iter = mExampleMap.find( name );

    FocusManager focusManager = FocusManager::Get();

    if( iter != mExampleMap.end() )
    {
      // ignore Example button presses when scrolling or button animating.
      if( ( !mScrolling ) && ( !mPressedAnimation ) )
      {
        // do nothing, until pressed animation finished.
        consumed = true;
      }
    }

    if( consumed )
    {
      mPressedAnimation = Animation::New( BUTTON_PRESS_ANIMATION_TIME );
      mPressedAnimation.SetEndAction( Animation::Discard );

      // scale the content actor within the Tile, as to not affect the placement within the Table.
      Actor content = actor.GetChildAt(0);
      mPressedAnimation.ScaleTo( content, Vector3( 0.9f, 0.9f, 1.0f ), AlphaFunctions::EaseInOut, 0.0f,
                                 BUTTON_PRESS_ANIMATION_TIME * 0.5f );
      mPressedAnimation.ScaleTo( content, Vector3::ONE, AlphaFunctions::EaseInOut, BUTTON_PRESS_ANIMATION_TIME * 0.5f,
                                 BUTTON_PRESS_ANIMATION_TIME * 0.5f );
      mPressedAnimation.Play();
      mPressedAnimation.FinishedSignal().Connect( this, &DaliTableView::OnPressedAnimationFinished );
    }
  }
  return consumed;
}

void DaliTableView::OnPressedAnimationFinished( Dali::Animation& source )
{
  mPressedAnimation.Reset();
  if( mPressedActor )
  {
    std::string name = mPressedActor.GetName();
    ExampleMapConstIter iter = mExampleMap.find( name );

    if( iter == mExampleMap.end() )
    {
      if( name == BUTTON_QUIT )
      {
        // Move focus to the OK button
        FocusManager focusManager = FocusManager::Get();

        // Enable the group mode and wrap mode
        focusManager.SetGroupMode( true );
        focusManager.SetWrapMode( true );
      }
    }
    else
    {
      const Example& example( iter->second );

      std::stringstream stream;
      stream << DALI_EXAMPLE_BIN << example.name.c_str();
      pid_t pid = fork();
      if( pid == 0)
      {
        execlp( stream.str().c_str(), example.name.c_str(), NULL );
        DALI_ASSERT_ALWAYS(false && "exec failed!");
      }
    }
    mPressedActor.Reset();
  }
}

void DaliTableView::OnScrollStart( const Dali::Vector3& position )
{
  mScrolling = true;

  PlayAnimation();
}

void DaliTableView::OnScrollComplete( const Dali::Vector3& position )
{
  mScrolling = false;

  // move focus to 1st item of new page
  FocusManager focusManager = FocusManager::Get();
  focusManager.SetCurrentFocusActor(mPages[mScrollView.GetCurrentPage()].GetChildAt(0) );
}

bool DaliTableView::OnScrollTouched( Actor actor, const TouchEvent& event )
{
  const TouchPoint& point = event.GetPoint( 0 );
  if( TouchPoint::Down == point.state )
  {
    mPressedActor = actor;
  }

  return false;
}

void DaliTableView::ApplyScrollViewEffect()
{
  // Remove old effect if exists.

  if( mScrollViewEffect )
  {
    mScrollView.RemoveEffect( mScrollViewEffect );
  }

  // Just one effect for now
  SetupInnerPageCubeEffect();

  mScrollView.ApplyEffect( mScrollViewEffect );
}

void DaliTableView::SetupInnerPageCubeEffect()
{
  mScrollViewEffect = ScrollViewCubeEffect::New();
  mScrollView.SetScrollSnapDuration( EFFECT_SNAP_DURATION );
  mScrollView.SetScrollFlickDuration( EFFECT_FLICK_DURATION );
  mScrollView.RemoveConstraintsFromChildren();
}

void DaliTableView::ApplyCubeEffectToActor( Actor actor )
{
  actor.RemoveConstraints();

  ScrollViewCubeEffect cubeEffect = ScrollViewCubeEffect::DownCast(mScrollViewEffect);
  cubeEffect.ApplyToActor( actor,
                           ScalePointSize( ( rand() & 1 ) ? ANCHOR_3DEFFECT_STYLE0 : ANCHOR_3DEFFECT_STYLE1 ),
                           ANGLE_SWING_3DEFFECT,
                           POSITION_SWING_3DEFFECT * Vector2(Stage::GetCurrent().GetSize()));
}

void DaliTableView::OnKeyEvent( const KeyEvent& event )
{
  if( event.state == KeyEvent::Down )
  {
    if ( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
    {
      if ( mVersionPopup && mVersionPopupShown )
      {
        HideVersionPopup();
      }
      else
      {
        mApplication.Quit();
      }
    }
  }
}

void DaliTableView::SetupBackground( Actor bubbleContainer )
{
  // Create distance field shape.
  BufferImage distanceField;
  Size imageSize( 512, 512 );
  CreateShapeImage( CIRCLE, imageSize, distanceField );

  // Add bubbles to the bubbleContainer.
  // Note: The bubbleContainer is parented externally to this function.
  AddBackgroundActors( bubbleContainer, NUM_BACKGROUND_IMAGES, distanceField );
}

void DaliTableView::InitialiseBackgroundActors( Actor actor )
{
  // Delete current animations
  mBackgroundAnimations.clear();

  // Create new animations
  const Vector3 size = actor.GetTargetSize();

  for( unsigned int i = 0, childCount = actor.GetChildCount(); i < childCount; ++i )
  {
    Actor child = actor.GetChildAt( i );

    const Vector3 childSize = child.GetTargetSize();

    // Calculate a random position
    Vector3 childPos( Random::Range( -size.x * 0.5f * BACKGROUND_SPREAD_SCALE, size.x * 0.5f * BACKGROUND_SPREAD_SCALE ),
                      Random::Range( -size.y * 0.5f - childSize.height, size.y * 0.5f + childSize.height ),
                      Random::Range( BUBBLE_MIN_Z, BUBBLE_MAX_Z ) );

    child.SetPosition( childPos );

    // Define bubble horizontal parallax and vertical wrapping
    Constraint animConstraint = Constraint::New < Vector3 > ( child, Actor::Property::POSITION, AnimateBubbleConstraint( childPos, Random::Range( -0.85f, 0.25f ), childSize.height ) );
    animConstraint.AddSource( Source( mScrollView, mScrollView.GetPropertyIndex( ScrollView::SCROLL_POSITION_PROPERTY_NAME ) ) );
    animConstraint.AddSource( Dali::ParentSource( Dali::Actor::Property::SIZE ) );
    animConstraint.Apply();

    // Kickoff animation
    Animation animation = Animation::New( Random::Range( 40.0f, 80.0f ) );
    animation.MoveBy( child, Vector3( 0.0f, -1.0f, 0.0f ), AlphaFunctions::Linear );
    animation.SetLooping( true );
    animation.Play();
    mBackgroundAnimations.push_back( animation );
  }
}

void DaliTableView::AddBackgroundActors( Actor layer, int count, BufferImage distanceField )
{
  for( int i = 0; i < count; ++i )
  {
    float randSize = Random::Range( 10.0f, 400.0f );
    float hue = Random::Range( 0.3f, 1.0f );
    Vector4 randColour( hue, hue * 0.5, 0.0f, Random::Range( 0.3f, 0.6f ));

    ImageActor dfActor = ImageActor::New( distanceField );
    dfActor.SetRelayoutEnabled( false );
    dfActor.SetSize( Vector2( randSize, randSize ) );
    dfActor.SetParentOrigin( ParentOrigin::CENTER );

    Toolkit::DistanceFieldEffect effect = Toolkit::DistanceFieldEffect::New();
    dfActor.SetShaderEffect( effect );
    dfActor.SetColor( randColour );
    effect.SetOutlineParams( Vector2( 0.55f, 0.00f ) );
    effect.SetSmoothingEdge( 0.5f );
    layer.Add( dfActor );
  }

  // Positioning will occur when the layer is relaid out
  layer.OnRelayoutSignal().Connect( this, &DaliTableView::InitialiseBackgroundActors );
}

void DaliTableView::CreateShapeImage( ShapeType shapeType, const Size& size, BufferImage& distanceFieldOut )
{
  // this bitmap will hold the alpha map for the distance field shader
  distanceFieldOut = BufferImage::New( size.width, size.height, Pixel::A8 );

  // Generate bit pattern
  std::vector< unsigned char > imageDataA8;
  imageDataA8.reserve( size.width * size.height ); // A8

  switch( shapeType )
  {
    case CIRCLE:
      GenerateCircle( size, imageDataA8 );
      break;
    case SQUARE:
      GenerateSquare( size, imageDataA8 );
      break;
    default:
      break;
  }

  PixelBuffer* buffer = distanceFieldOut.GetBuffer();
  if( buffer )
  {
    GenerateDistanceFieldMap( &imageDataA8[ 0 ], size, buffer, size, 8.0f, size );
    distanceFieldOut.Update();
  }
}

void DaliTableView::GenerateSquare( const Size& size, std::vector< unsigned char >& distanceFieldOut )
{
  for( int h = 0; h < size.height; ++h )
  {
    for( int w = 0; w < size.width; ++w )
    {
      distanceFieldOut.push_back( 0xFF );
    }
  }
}

void DaliTableView::GenerateCircle( const Size& size, std::vector< unsigned char >& distanceFieldOut )
{
  const float radius = size.width * 0.5f * size.width * 0.5f;
  Vector2 center( size.width / 2, size.height / 2 );

  for( int h = 0; h < size.height; ++h )
  {
    for( int w = 0; w < size.width; ++w )
    {
      Vector2 pos( w, h );
      Vector2 dist = pos - center;

      if( dist.x * dist.x + dist.y * dist.y > radius )
      {
        distanceFieldOut.push_back( 0x00 );
      }
      else
      {
        distanceFieldOut.push_back( 0xFF );
      }
    }
  }
}

ImageActor DaliTableView::CreateLogo( std::string imagePath )
{
  Image image = ResourceImage::New( imagePath );
  ImageActor logo = ImageActor::New( image );

  logo.SetAnchorPoint( AnchorPoint::CENTER );
  logo.SetParentOrigin( ParentOrigin::CENTER );

  return logo;
}

bool DaliTableView::PauseBackgroundAnimation()
{
  PauseAnimation();

  return false;
}

void DaliTableView::PauseAnimation()
{
  if( mBackgroundAnimsPlaying )
  {
    for( AnimationListIter animIter = mBackgroundAnimations.begin(); animIter != mBackgroundAnimations.end(); ++animIter )
    {
      Animation anim = *animIter;

      anim.Pause();
    }

    mBackgroundAnimsPlaying = false;
  }
}

void DaliTableView::PlayAnimation()
{
  if ( !mBackgroundAnimsPlaying )
  {
    for( AnimationListIter animIter = mBackgroundAnimations.begin(); animIter != mBackgroundAnimations.end(); ++animIter )
    {
      Animation anim = *animIter;

      anim.Play();
    }

    mBackgroundAnimsPlaying = true;
  }

  mAnimationTimer.SetInterval( BACKGROUND_ANIMATION_DURATION );
}

Dali::Actor DaliTableView::OnKeyboardPreFocusChange( Dali::Actor current, Dali::Actor proposed, Dali::Toolkit::Control::KeyboardFocusNavigationDirection direction )
{
  Actor nextFocusActor = proposed;

  if ( !current && !proposed  )
  {
    // Set the initial focus to the first tile in the current page should be focused.
    nextFocusActor = mPages[mScrollView.GetCurrentPage()].GetChildAt(0);
  }
  else if( !proposed || (proposed && proposed == mScrollViewLayer) )
  {
    // ScrollView is being focused but nothing in the current page can be focused further
    // in the given direction. We should work out which page to scroll to next.
    int currentPage = mScrollView.GetCurrentPage();
    int newPage = currentPage;
    if( direction == Dali::Toolkit::Control::Left )
    {
      newPage--;
    }
    else if( direction == Dali::Toolkit::Control::Right )
    {
      newPage++;
    }

    newPage = std::max(0, std::min(static_cast<int>(mScrollRulerX->GetTotalPages() - 1), newPage));
    if( newPage == currentPage )
    {
      if( direction == Dali::Toolkit::Control::Left )
      {
        newPage = mScrollRulerX->GetTotalPages() - 1;
      } else if( direction == Dali::Toolkit::Control::Right )
      {
        newPage = 0;
      }
    }

    // Scroll to the page in the given direction
    mScrollView.ScrollTo(newPage);

    if( direction == Dali::Toolkit::Control::Left )
    {
      // Work out the cell position for the last tile
      int remainingExamples = mExampleList.size() - newPage * EXAMPLES_PER_PAGE;
      int rowPos = (remainingExamples >= EXAMPLES_PER_PAGE) ? ROWS_PER_PAGE - 1 : ( (remainingExamples % EXAMPLES_PER_PAGE + EXAMPLES_PER_ROW) / EXAMPLES_PER_ROW - 1 );
      int colPos = remainingExamples >= EXAMPLES_PER_PAGE ? EXAMPLES_PER_ROW - 1 : ( remainingExamples % EXAMPLES_PER_PAGE - rowPos * EXAMPLES_PER_ROW - 1 );

      // Move the focus to the last tile in the new page.
      nextFocusActor = mPages[newPage].GetChildAt(colPos * EXAMPLES_PER_ROW + rowPos);
    }
    else
    {
      // Move the focus to the first tile in the new page.
      nextFocusActor = mPages[newPage].GetChildAt(0);
    }
  }

  return nextFocusActor;
}

void DaliTableView::OnFocusedActorActivated( Dali::Actor activatedActor )
{
  if(activatedActor)
  {
    mPressedActor = activatedActor;

    // Activate the current focused actor;
    TouchEvent touchEventUp;
    touchEventUp.points.push_back( TouchPoint ( 0, TouchPoint::Up, 0.0f, 0.0f ) );
    OnTilePressed(mPressedActor, touchEventUp);
  }
}

bool DaliTableView::OnTileHovered( Actor actor, const HoverEvent& event )
{
  KeyboardFocusManager::Get().SetCurrentFocusActor( actor );
  return true;
}

void DaliTableView::OnLogoTapped( Dali::Actor actor, const Dali::TapGesture& tap )
{
  if ( !mVersionPopupShown )
  {
    if ( !mVersionPopup )
    {
      std::ostringstream stream;
      stream << "DALi Core: "    << CORE_MAJOR_VERSION << "." << CORE_MINOR_VERSION << "." << CORE_MICRO_VERSION << std::endl << "(" << CORE_BUILD_DATE << ")" << std::endl << std::endl;
      stream << "DALi Adaptor: " << ADAPTOR_MAJOR_VERSION << "." << ADAPTOR_MINOR_VERSION << "." << ADAPTOR_MICRO_VERSION << std::endl << "(" << ADAPTOR_BUILD_DATE << ")" << std::endl << std::endl;
      stream << "DALi Toolkit: " << TOOLKIT_MAJOR_VERSION << "." << TOOLKIT_MINOR_VERSION << "." << TOOLKIT_MICRO_VERSION << std::endl << "(" << TOOLKIT_BUILD_DATE << ")";

      mVersionPopup = Dali::Toolkit::Popup::New();
      mVersionPopup.SetParentOrigin( ParentOrigin::CENTER );
      mVersionPopup.SetAnchorPoint( AnchorPoint::CENTER );
      mVersionPopup.SetResizePolicy( FILL_TO_PARENT, WIDTH );
      mVersionPopup.SetSizeMode( SIZE_RELATIVE_TO_PARENT );
      mVersionPopup.SetSizeModeFactor( Vector3( 0.75f, 1.0f, 1.0f ) );
      mVersionPopup.SetResizePolicy( FIT_TO_CHILDREN, HEIGHT );
      mVersionPopup.SetTitle( stream.str() );
      mVersionPopup.HideTail();
      mVersionPopup.OutsideTouchedSignal().Connect( this, &DaliTableView::HideVersionPopup );
      mVersionPopup.HiddenSignal().Connect( this, &DaliTableView::PopupHidden );

      mVersionPopup.MarkDirtyForRelayout();
    }

    mVersionPopup.Show();
    mVersionPopupShown = true;
  }
}

void DaliTableView::HideVersionPopup()
{
  if ( mVersionPopup )
  {
    mVersionPopup.Hide();
  }
}

void DaliTableView::PopupHidden()
{
  if ( mVersionPopup )
  {
    mVersionPopupShown = false;
  }
}
