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

const char* BACKGROUND_IMAGE( "" );
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char* VIEW_SWAP_IMAGE( DALI_IMAGE_DIR "icon_mode.png" );
const char* APPLICATION_TITLE( "EffectsView" );
const char* ICON_IMAGE( DALI_IMAGE_DIR "icon_pageflip.png" );

} // namespace

// This example illustrates the capabilities of the EffectsView container
//
class ExampleApp : public ConnectionTracker
{
public:
  enum Scenes
  {
    DropShadow,
    Emboss,
    MAX_SCENE
  };

public:

  ExampleApp( Application& application );
  ~ExampleApp();

  void CreateDropShadowScene();
  void CreateDropShadow( const Vector2& position, const Vector2& panelSize,
                         const Vector4& foregroundColor, const Vector4& backgroundColor, const Vector4& shadowColor,
                         const Vector2& distance, const int size, const bool animate );
  void CreateEmbossScene();

  void TearDownScene();
  void ChangeView();

  bool OnView( Toolkit::Button button );
  void OnAppInitialize( Application& application );
  void OnKeyEvent(const KeyEvent& event);

private:
  Application&          mApplication;
  Layer                 mContents;
  Toolkit::View         mView;
  Toolkit::ToolBar      mToolBar;
  Actor                 mLocalRoot;
  std::vector<Animation> mAnimations;
  Stage                 mStage;
  Vector2               mStageSize;
  int                   mCurrentScene;
  Vector2               mViewSize;
};

ExampleApp::ExampleApp( Application& application )
: mApplication( application ),
  mView(),
  mCurrentScene( MAX_SCENE )
{
  // Connect to the Application's Init signal
  mApplication.InitSignal().Connect( this, &ExampleApp::OnAppInitialize );
}

ExampleApp::~ExampleApp()
{
  // Nothing to do here;
}

void ExampleApp::CreateDropShadowScene()
{
  Vector2 effectSize( mStageSize.width, mStageSize.height * 0.2f );
  Vector2 shadowOffset( 1.0f, 1.0f );

  const Vector4 Cyan( 0.22f, 0.78f, 0.78f, 1.0f );
  const Vector4 Blue( 0.22f, 0.40f, 0.58f, 1.0f );
  const Vector4 Black( 0.2f, 0.2f, 0.2f, 1.0f );
  const Vector4 White( 0.95f, 0.95f, 0.95f, 1.0f );
  const Vector4 Yellow( Color::YELLOW );
  const Vector4 Orange( 1.0f, 0.6f, 0.0f, 1.0f );

  mViewSize = effectSize;

  CreateDropShadow( Vector2(0.0f, effectSize.height * 0.0f ), effectSize, White, Cyan,  Black,  Vector2(2.0f, 2.0f), 0, false );
  CreateDropShadow( Vector2(0.0f, effectSize.height * 1.0f ), effectSize, Black, White, Orange, Vector2(2.0f, 2.0f), 1, false );
  CreateDropShadow( Vector2(0.0f, effectSize.height * 2.0f ), effectSize, White, Cyan,  Orange, Vector2(2.0f, 2.0f), 2, false );
  CreateDropShadow( Vector2(0.0f, effectSize.height * 3.0f ), effectSize, Cyan,  White, Black,  Vector2(2.0f, 2.0f), 3, true );
  CreateDropShadow( Vector2(0.0f, effectSize.height * 4.0f ), effectSize, Blue,  Black, White,  Vector2(2.0f, 2.0f), 4, true );
}

void ExampleApp::CreateDropShadow( const Vector2& position, const Vector2& panelSize,
                               const Vector4& foregroundColor, const Vector4& backgroundColor, const Vector4& shadowColor,
                               const Vector2& distance, const int size, bool animate )
{
  // Create image for composited result of the EffectsView
  FrameBufferImage image( FrameBufferImage::New( panelSize.width, panelSize.height, Pixel::RGBA8888, Image::Unused ) );

  Toolkit::EffectsView effectView = Toolkit::EffectsView::New();
  // set position and format
  effectView.SetParentOrigin( ParentOrigin::CENTER );
  //effectView.SetSize( panelSize.width*0.75f, panelSize.height*0.75f );
  effectView.SetSize( panelSize.width, panelSize.height );
  effectView.SetPixelFormat( Pixel::RGBA8888 );
  // set effect type and properties
  effectView.SetType( Toolkit::EffectsView::DROP_SHADOW );
  effectView.SetProperty( effectView.GetEffectSizePropertyIndex(), static_cast< float >( size ) );
  effectView.SetProperty( effectView.GetEffectOffsetPropertyIndex(), Vector3(distance) );
  effectView.SetProperty( effectView.GetEffectColorPropertyIndex(), shadowColor );
  // set output target
  effectView.SetOutputImage( image );
  effectView.SetBackgroundColor( Vector4(backgroundColor.r, backgroundColor.g, backgroundColor.b, 0.0f ) );
  mLocalRoot.Add( effectView );

  // Create some content
  // text
  TextStyle style;
  style.SetSmoothEdge(0.25f);
  TextView textActor( TextView::New( "Hello" ) );
  textActor.SetStyleToCurrentText(style);
  textActor.SetParentOrigin( ParentOrigin::CENTER_LEFT );
  textActor.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
  textActor.ApplyConstraint( Constraint::New<float>( Actor::POSITION_X, ParentSource( Actor::SIZE_WIDTH ), RelativeToConstraintFloat( 0.5f ) ) );
  textActor.SetScale(2.0f);
  textActor.SetColor( foregroundColor );
  effectView.Add( textActor );


  // image
  Image imageIcon( Image::New(ICON_IMAGE) );
  ImageActor icon( ImageActor::New( imageIcon ) );
  icon.SetParentOrigin( ParentOrigin::CENTER_LEFT );
  icon.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
  icon.ApplyConstraint( Constraint::New<float>( Actor::POSITION_X, ParentSource( Actor::SIZE_WIDTH ), RelativeToConstraintFloat( 0.1f ) ) );
  icon.SetSize( panelSize.height * 0.65f, panelSize.height * 0.65f );
  effectView.Add( icon );

  // refresh continuously if animating
  effectView.SetRefreshOnDemand( !animate );

  // enable EffectsView
  effectView.Enable();

  ////////////////////////////////////////////////////////////////////////
  // using the output

  // create a background panel
  ImageActor backgroundActor( ImageActor::New( BitmapImage::WHITE() ) );
  backgroundActor.SetParentOrigin( ParentOrigin::TOP_CENTER );
  backgroundActor.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  backgroundActor.SetPosition( position.x, position.y, -1.0f );
  backgroundActor.SetSize( panelSize.width, panelSize.height );
  backgroundActor.SetColor( backgroundColor );
  mLocalRoot.Add( backgroundActor );

  // create an image to render the result of the EffectsView
  ImageActor imageActor( ImageActor::New( image ) );
  imageActor.SetParentOrigin( ParentOrigin::TOP_CENTER );
  imageActor.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  imageActor.SetPosition( position.x, position.y );
  mLocalRoot.Add( imageActor );

  if( animate )
  {
    const float animationTime( Random::Range( 10.0f, 20.0f ) );
    Animation animation( Animation::New(animationTime) );
    Property::Index shadowOffsetIndex( effectView.GetEffectOffsetPropertyIndex() );
    animation.AnimateTo( Property( effectView, shadowOffsetIndex ), Vector3( distance.x,-distance.y, 0.0f), TimePeriod(animationTime * 0.0f, animationTime * 0.1f) );
    animation.AnimateTo( Property( effectView, shadowOffsetIndex ), Vector3(-distance.x,-distance.y, 0.0f), TimePeriod(animationTime * 0.1f, animationTime * 0.1f) );
    animation.AnimateTo( Property( effectView, shadowOffsetIndex ), Vector3(-distance.x, distance.y, 0.0f), TimePeriod(animationTime * 0.2f, animationTime * 0.1f) );
    animation.AnimateTo( Property( effectView, shadowOffsetIndex ), Vector3( distance.x, distance.y, 0.0f), TimePeriod(animationTime * 0.3f, animationTime * 0.1f) );
    animation.AnimateTo( Property( effectView, shadowOffsetIndex ), Vector3( distance.x, distance.y, 0.0f) * 10.0f,
                         AlphaFunctions::Sin, TimePeriod(animationTime * 0.4f, animationTime * 0.2f) );
    animation.SetLooping( true );
    animation.Play();

    mAnimations.push_back( animation );
  }
}

void ExampleApp::CreateEmbossScene()
{
  Vector2 size(mStageSize * 0.85f);
  // Create image for composited result of the EffectsView
  FrameBufferImage image( FrameBufferImage::New( size.width, size.height, Pixel::RGBA8888, Image::Unused ) );

  Toolkit::EffectsView effectsView( Toolkit::EffectsView::New() );
  // set position and format
  effectsView.SetParentOrigin( ParentOrigin::CENTER );
  effectsView.SetSize( size );
  effectsView.SetPixelFormat( Pixel::RGBA8888 );
  // set effect type and properties
  effectsView.SetType( Toolkit::EffectsView::EMBOSS );
  effectsView.SetProperty( effectsView.GetEffectSizePropertyIndex(), static_cast< float >( 2.0f ) );
  effectsView.SetOutputImage( image );
  effectsView.SetBackgroundColor( Vector4( 0.0f, 0.0f, 0.0f, 0.0f ) );
  mLocalRoot.Add( effectsView );

  // Create some content
  // text
  TextStyle style;
  style.SetTextColor( Color::BLACK );

  TextView textActor( TextView::New( "Wishing You a" ) );
  textActor.SetStyleToCurrentText(style);
  textActor.SetParentOrigin( ParentOrigin::CENTER );
  textActor.SetY( -50.0f );
  textActor.SetScale(2.0f);
  effectsView.Add( textActor );

  textActor = TextView::New( "Happy Birthday" );
  textActor.SetStyleToCurrentText(style);
  textActor.SetParentOrigin( ParentOrigin::CENTER );
  textActor.SetY( 50.0f );
  textActor.SetScale(2.0f);
  textActor.SetColor( Color::BLACK );
  effectsView.Add( textActor );

  // image
  Image imageIcon( Image::New( DALI_IMAGE_DIR "staricon.png" ) );
  ImageActor icon( ImageActor::New( imageIcon ) );
  icon.SetParentOrigin( ParentOrigin::CENTER );
  icon.SetY( 180.0f );
  effectsView.Add( icon );

  // refresh when requested
  effectsView.SetRefreshOnDemand( true );

  // enable EffectsView
  effectsView.Enable();

  // And display result
  Actor finalActor( Actor::New() );
  finalActor.SetParentOrigin( ParentOrigin::CENTER );
  finalActor.SetSize( size );
  finalActor.RotateBy(Degree(30.0f), Vector3::YAXIS );
  finalActor.RotateBy(Degree(15.0f), Vector3::XAXIS );
  mLocalRoot.Add( finalActor );

  ImageActor background( ImageActor::New( Image::New( DALI_IMAGE_DIR "paper.png" ) ) );
  background.SetParentOrigin( ParentOrigin::CENTER );
  background.SetSize( size.width, size.height * 0.65f );
  background.SetPosition( 0.0f, 75.0f, -1.0f);
  finalActor.Add( background );

  ImageActor effectResult( ImageActor::New( image ) );
  effectResult.SetParentOrigin( ParentOrigin::CENTER );
  effectResult.SetSize( size.width, size.height );
  finalActor.Add( effectResult );
}

void ExampleApp::TearDownScene()
{
  // stop animations
  const size_t numberOfAnimations( mAnimations.size() );
  if( numberOfAnimations )
  {
    for( size_t i = 0; i < numberOfAnimations; ++i )
    {
      mAnimations[i].Stop();
    }
    mAnimations.clear();
  }

  // destroy actors/controls
  while( 0 != mLocalRoot.GetChildCount() )
  {
    mLocalRoot.Remove( mLocalRoot.GetChildAt(0) );
  }
}

void ExampleApp::ChangeView()
{
  TearDownScene();

  ++mCurrentScene;
  if( mCurrentScene >= MAX_SCENE )
  {
    mCurrentScene = 0;
  }

  switch( mCurrentScene )
  {
    case DropShadow:
    {
      CreateDropShadowScene();
      break;
    }
    case Emboss:
    default:
    {
      CreateEmbossScene();
      break;
    }
  }
}

bool ExampleApp::OnView( Toolkit::Button button )
{
  ChangeView();
  return true;
}

void ExampleApp::OnAppInitialize( Application& application )
{
  // The Init signal is received once (only) during the Application lifetime

  Stage::GetCurrent().KeyEventSignal().Connect(this, &ExampleApp::OnKeyEvent);

  mStage = Stage::GetCurrent();
  mStageSize = mStage.GetSize();

  // Creates a default view with a default tool bar.
  // The view is added to the stage.
  mContents = DemoHelper::CreateView( application,
                                      mView,
                                      mToolBar,
                                      BACKGROUND_IMAGE,
                                      TOOLBAR_IMAGE,
                                      APPLICATION_TITLE );

  // Creates view change button.
  Image image = Image::New( VIEW_SWAP_IMAGE );
  Toolkit::PushButton viewButton = Toolkit::PushButton::New();
  viewButton.SetBackgroundImage( image );
  // Connects the view change button clicked signal to the OnView method.
  viewButton.ClickedSignal().Connect( this, &ExampleApp::OnView );
  mToolBar.AddControl( viewButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

  mLocalRoot = Actor::New();
  mLocalRoot.SetParentOrigin( ParentOrigin::CENTER );
  mLocalRoot.SetSize(mViewSize);
  mLocalRoot.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
  mContents.Add( mLocalRoot );

  ChangeView();
}

void ExampleApp::OnKeyEvent(const KeyEvent& event)
{
  if(event.state == KeyEvent::Down)
  {
    if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
    {
      // Delete objects
      TearDownScene();
      mContents.Remove( mLocalRoot );
      mLocalRoot.Reset();

      mApplication.Quit();
    }
  }
}

/*****************************************************************************/

static void RunTest( Application& application )
{
  ExampleApp test( application );

  application.MainLoop();
}

/*****************************************************************************/

int main(int argc, char **argv)
{
  Application application = Application::New(&argc, &argv);

  RunTest(application);

  return 0;
}
