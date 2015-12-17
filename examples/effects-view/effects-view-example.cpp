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
#include "shared/view.h"

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/effects-view/effects-view.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char * const TITLE_DROP_SHADOW( "Drop Shadow" );
const char * const TITLE_EMBOSS( "Emboss" );
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char* VIEW_SWAP_IMAGE( DALI_IMAGE_DIR "icon-change.png" );
const char* VIEW_SWAP_SELECTED_IMAGE( DALI_IMAGE_DIR "icon-change-selected.png" );
const char* TEST_IMAGE_1( DALI_IMAGE_DIR "music-libray-star.png" );
const char* TEST_IMAGE_2( DALI_IMAGE_DIR "Kid.png" );
const char* const CONTENT_TEXT = "Hello, DALi";
} // namespace

// This example illustrates the capabilities of the EffectsView container
//
class EffectsViewApp : public ConnectionTracker
{
public:

  /**
   * Constructor
   */
  EffectsViewApp( Application& application );
  /**
   * Destructor
   */
  ~EffectsViewApp();

private:

  /**
   * Initialisation. This method gets called once the main loop of application is up and running
   */
  void OnAppInitialize( Application& application );

  /**
   * Create a scene of drop shadows with EffectsView of DROP_SHADOW type
   */
  void CreateDropShadowScene();

  /**
   * Create a effect view of drop shadow
   * @param[in] position Position of the effect view
   * @param[in] panelSize Size of the effect view
   * @param[in] forgroundColor Foreground color
   * @param[in] backgroundColor Color of the background panel
   * @param[in] shadowColor1 The original shadow color.
   * @param[in] shadowColor2 The shadow color to animate to
   * @param[in] size The effect size used in image filters.
   */
  void CreateDropShadow( const Vector2& position, const Vector2& panelSize,
                         const Vector4& foregroundColor, const Vector4& backgroundColor,
                         const Vector4& shadowColor1, const Vector4&shadowColor2,
                         int size );
  /**
   * Create a emboss scene with EffectsView of EMBOSS type
   */
  void CreateEmbossScene();

  /**
   * Remove the current scene.
   */
  void TearDownScene();

  /**
   * Callback of button clicked signal to switch between scenes.
   */
  bool ChangeScene( Toolkit::Button button );

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event);

  /**
   * Set title onto the toolbar
   */
  void SetTitle(const std::string& title);

private:
  Application&           mApplication;
  Layer                  mContents;
  Toolkit::Control       mView;
  Toolkit::ToolBar       mToolBar;
  Toolkit::TextLabel     mTitleActor; ///< The title on the toolbar
  std::vector<Animation> mAnimations;
  Vector2                mStageSize;
  int                    mCurrentScene;
};

EffectsViewApp::EffectsViewApp( Application& application )
: mApplication( application ),
  mView(),
  mCurrentScene( 0 )
{
  // Connect to the Application's Init signal
  mApplication.InitSignal().Connect( this, &EffectsViewApp::OnAppInitialize );
}

EffectsViewApp::~EffectsViewApp()
{
  // Nothing to do here;
}

void EffectsViewApp::OnAppInitialize( Application& application )
{
  // The Init signal is received once (only) during the Application lifetime

  Stage stage = Stage::GetCurrent();
  stage.KeyEventSignal().Connect(this, &EffectsViewApp::OnKeyEvent);
  stage.SetBackgroundColor( Color::WHITE );

  mStageSize = stage.GetSize();

  // Creates a default view with a default tool bar.
  // The view is added to the stage.
  mContents = DemoHelper::CreateView( application, mView, mToolBar, "", TOOLBAR_IMAGE, "" );

  // Creates view change button.
  Toolkit::PushButton viewButton = Toolkit::PushButton::New();
  viewButton.SetUnselectedImage( VIEW_SWAP_IMAGE );
  viewButton.SetSelectedImage( VIEW_SWAP_SELECTED_IMAGE );
  // Connects the view change button clicked signal to the OnView method.
  viewButton.ClickedSignal().Connect( this, &EffectsViewApp::ChangeScene );
  mToolBar.AddControl( viewButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

  ChangeScene( Button() );
}

void EffectsViewApp::CreateDropShadowScene()
{
  Vector2 effectSize( mStageSize.width, mStageSize.height * 0.15f );
  Vector2 shadowOffset( 1.0f, 1.0f );

  const Vector4 Cyan( 0.22f, 0.78f, 0.78f, 1.0f );
  const Vector4 Magenta( 0.78f, 0.22f, 0.78f, 1.f );
  const Vector4 Blue( 0.22f, 0.40f, 0.58f, 1.0f );
  const Vector4 Green( 0.22f, 0.80f, 0.58f, 1.0f );
  const Vector4 Black( 0.2f, 0.2f, 0.2f, 1.0f );
  const Vector4 White( 0.95f, 0.95f, 0.95f, 1.0f );
  const Vector4 Orange( 1.0f, 0.5f, 0.0f, 1.0f );

  CreateDropShadow( Vector2(0.0f, effectSize.height * 1.0f ), effectSize, White, Cyan,  Blue, Black, 0 );
  CreateDropShadow( Vector2(0.0f, effectSize.height * 2.0f ), effectSize, Black, White,Orange, Magenta, 1 );
  CreateDropShadow( Vector2(0.0f, effectSize.height * 3.0f ), effectSize, White, Cyan, Blue, Orange, 2 );
  CreateDropShadow( Vector2(0.0f, effectSize.height * 4.0f ), effectSize, Cyan,  White, Green,Black, 3 );
  CreateDropShadow( Vector2(0.0f, effectSize.height * 5.0f ), effectSize, Blue,  Black, Magenta, White, 4 );
}

void EffectsViewApp::CreateDropShadow( const Vector2& position, const Vector2& panelSize,
                                       const Vector4& foregroundColor, const Vector4& backgroundColor,
                                       const Vector4& shadowColor1, const Vector4&shadowColor2,
                                       int size )
{
  Toolkit::EffectsView effectsView = Toolkit::EffectsView::New(Toolkit::EffectsView::DROP_SHADOW);
  // set position and format
  effectsView.SetParentOrigin( ParentOrigin::TOP_CENTER );
  effectsView.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  effectsView.SetSize( panelSize.width, panelSize.height );
  effectsView.SetPixelFormat( Pixel::RGBA8888 );
  effectsView.SetPosition( position.x, position.y );
  // set effect properties
  effectsView.SetProperty( effectsView.GetEffectSizePropertyIndex(), static_cast< float >( size ) );
  effectsView.SetProperty( effectsView.GetEffectColorPropertyIndex(), shadowColor1 );
  effectsView.SetBackgroundColor( Vector4(backgroundColor.r, backgroundColor.g, backgroundColor.b, 0.0f ) );
  mContents.Add( effectsView );

  // Create some content
  // text
  TextLabel textActor( TextLabel::New( "Hello" ) );
  textActor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  textActor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  textActor.SetSize( panelSize );
  textActor.SetPosition( panelSize.width*0.5f, panelSize.height*0.3f );
  textActor.SetProperty(  TextLabel::Property::POINT_SIZE, 50.f );
  textActor.SetProperty(  TextLabel::Property::TEXT_COLOR, foregroundColor );
  effectsView.Add( textActor );

  // image
  ImageView icon = ImageView::New( TEST_IMAGE_1 );
  icon.SetParentOrigin( ParentOrigin::CENTER_LEFT );
  icon.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
  icon.SetX( panelSize.width*0.1f );
  icon.SetSize( panelSize.height * 0.65f, panelSize.height * 0.65f );
  effectsView.Add( icon );

  // refresh continuously for animating
  effectsView.SetRefreshOnDemand( true );

  // create a background panel
  Control backgroundActor = Control::New();
  backgroundActor.SetParentOrigin( ParentOrigin::TOP_CENTER );
  backgroundActor.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  backgroundActor.SetPosition( position.x, position.y );
  backgroundActor.SetSize( panelSize.width, panelSize.height );
  backgroundActor.SetBackgroundColor( backgroundColor );
  mContents.Add( backgroundActor );

  const float animationTime( Random::Range( 5.0f, 10.0f ) );
  Animation animation( Animation::New(animationTime) );
  Property::Index shadowOffsetIndex( effectsView.GetEffectOffsetPropertyIndex() );
  animation.AnimateTo( Property( effectsView, shadowOffsetIndex ), Vector3( 2.f,-2.f, 0.0f), TimePeriod(animationTime * 0.0f, animationTime * 0.1f) );
  animation.AnimateTo( Property( effectsView, shadowOffsetIndex ), Vector3(-2.f,-2.f, 0.0f), TimePeriod(animationTime * 0.1f, animationTime * 0.1f) );
  animation.AnimateTo( Property( effectsView, shadowOffsetIndex ), Vector3(-2.f, 2.f, 0.0f), TimePeriod(animationTime * 0.2f, animationTime * 0.1f) );
  animation.AnimateTo( Property( effectsView, shadowOffsetIndex ), Vector3( 6.f, 6.f, 0.0f), TimePeriod(animationTime * 0.3f, animationTime * 0.3f) );
  animation.AnimateTo( Property( effectsView, shadowOffsetIndex ), Vector3::ZERO, TimePeriod(animationTime * 0.6f, animationTime * 0.4f) );
  animation.AnimateTo( Property( effectsView, effectsView.GetEffectColorPropertyIndex() ), shadowColor2, AlphaFunction::BOUNCE);
  animation.SetLooping( true );
  animation.Play();

  mAnimations.push_back( animation );
}

void EffectsViewApp::CreateEmbossScene()
{
  Vector2 size(mStageSize * 0.75f);

  Toolkit::EffectsView effectsView = Toolkit::EffectsView::New( Toolkit::EffectsView::EMBOSS );
  // set position and format
  effectsView.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
  effectsView.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  effectsView.SetSize( size );
  effectsView.SetPixelFormat( Pixel::RGBA8888 );
  // set effect properties
  effectsView.SetProperty( effectsView.GetEffectSizePropertyIndex(), 3.f );
  effectsView.SetBackgroundColor( Vector4( 1.0f, 0.0f, 0.0f, 0.0f ) );
  mContents.Add( effectsView );

  // Create some content
  // text
  TextLabel textActor( TextLabel::New( CONTENT_TEXT ) );
  textActor.SetParentOrigin( ParentOrigin::CENTER );
  textActor.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  textActor.SetPosition( 0.f, -150.0f );
  textActor.SetSize( size.x, size.y*0.2f );
  textActor.SetProperty(  TextLabel::Property::POINT_SIZE, 50.f );
  textActor.SetProperty( TextLabel::Property::MULTI_LINE, true );
  effectsView.Add( textActor );

  // image
  ImageView icon = ImageView::New( TEST_IMAGE_2 );
  icon.SetParentOrigin( ParentOrigin::CENTER );
  icon.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  icon.SetY(-150.f);
  effectsView.Add( icon );

  const float animationTime( 5.0f );
  Animation animation( Animation::New(animationTime) );
  Property::Index shadowOffsetIndex( effectsView.GetEffectOffsetPropertyIndex() );
  animation.AnimateTo( Property( effectsView, shadowOffsetIndex ), Vector3( 1.f,-1.f, 0.0f), TimePeriod(animationTime * 0.0f, animationTime * 0.2f) );
  animation.AnimateTo( Property( effectsView, shadowOffsetIndex ), Vector3(-1.f,-1.f, 0.0f), TimePeriod(animationTime * 0.2f, animationTime * 0.2f) );
  animation.AnimateTo( Property( effectsView, shadowOffsetIndex ), Vector3(-1.f, 1.f, 0.0f), TimePeriod(animationTime * 0.4f, animationTime * 0.2f) );
  animation.AnimateTo( Property( effectsView, shadowOffsetIndex ), Vector3( 1.f, 1.f, 0.0f), TimePeriod(animationTime * 0.6f, animationTime * 0.2f) );
  animation.AnimateTo( Property( effectsView, shadowOffsetIndex ), Vector3::ZERO, TimePeriod(animationTime * 0.8f, animationTime * 0.2f) );

  animation.SetLooping( true );
  animation.Play();

  mAnimations.push_back( animation );
}

void EffectsViewApp::TearDownScene()
{
  // stop animations
  const size_t numberOfAnimations( mAnimations.size() );
  if( numberOfAnimations )
  {
    for( size_t i = 0; i < numberOfAnimations; ++i )
    {
      mAnimations[i].Clear();
    }
    mAnimations.clear();
  }

  // destroy actors/controls
  while( 0 != mContents.GetChildCount() )
  {
    mContents.Remove( mContents.GetChildAt(0) );
  }
}

bool EffectsViewApp::ChangeScene( Toolkit::Button button )
{
  TearDownScene();

  ++mCurrentScene;

  if( mCurrentScene%2 == 0)
  {
    SetTitle( TITLE_EMBOSS );
    CreateEmbossScene();
  }
  else
  {
    SetTitle( TITLE_DROP_SHADOW );
    CreateDropShadowScene();
  }

  return true;
}

void EffectsViewApp::OnKeyEvent(const KeyEvent& event)
{
  if(event.state == KeyEvent::Down)
  {
    if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
    {
      // Delete objects
      TearDownScene();
      mApplication.Quit();
    }
  }
}

void EffectsViewApp::SetTitle(const std::string& title)
{
  if(!mTitleActor)
  {
    mTitleActor = DemoHelper::CreateToolBarLabel( title );
    // Add title to the tool bar.
    mToolBar.AddControl( mTitleActor, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Toolkit::Alignment::HorizontalCenter );
  }

  mTitleActor.SetProperty( Toolkit::TextLabel::Property::TEXT, title );
}

/*****************************************************************************/

static void RunTest( Application& application )
{
  EffectsViewApp test( application );

  application.MainLoop();
}

/*****************************************************************************/

int main(int argc, char **argv)
{
  Application application = Application::New(&argc, &argv);

  RunTest(application);

  return 0;
}
