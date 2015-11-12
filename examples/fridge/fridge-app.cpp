//
// Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <string>
#include <sstream>
#include <fstream>

#include "fridge-app.h"

using namespace Dali;
using namespace Dali::Toolkit;

#include <iostream>
#include <sys/time.h>
using namespace std;

namespace
{

const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "background.png" );

Property::Map CreateMenuBackgroundEffect()
{
  std::string vertexSource;
  vertexSource =
      "attribute vec3 aPosition;\n"
      "attribute vec2 aTexCoord;\n"
      "varying vec2 vTexCoord;\n"
      "uniform mat4 uModelView;\n"
      "uniform mat4 uProjection;\n"
      "uniform lowp vec3 uSize;\n"
      "uniform lowp vec2 uCircleCenter;\n"
      "varying lowp vec2 vPointToCenter;\n"

      "void main()\n"
      "{\n"
      "  vTexCoord = vec2( aPosition.x + 0.5, aPosition.y + 0.5 );\n"
      "  gl_Position = uProjection * uModelView * ( vec4(aPosition, 1.0) * vec4(uSize, 1.0) );\n"
      "  vPointToCenter = vec2(vTexCoord.x,(uSize.y/uSize.x)*vTexCoord.y + (abs(uSize.x-uSize.y)*0.5)/uSize.x) -  uCircleCenter;\n"
      "}\n";


  std::string fragmentSource;
  fragmentSource =
      "uniform lowp float uCircleRadius;\n"
      "varying lowp vec2 vPointToCenter;\n"
      "varying lowp vec2 vTexCoord;\n"
      "uniform sampler2D sTexture;\n"
      "uniform lowp vec4  uColor;\n"

      "void main()\n"
      "{\n"
      "  lowp float distanceToSphere = sqrt(dot(vPointToCenter,vPointToCenter)) - uCircleRadius;\n"
      "  lowp float alpha = min(0.5,-distanceToSphere*1000.0 + 1.0);\n"
      "  if(alpha < 0.0) discard;\n"
      "  gl_FragColor = vec4( texture2D( sTexture, vTexCoord ).xyz * uColor.xyz, alpha);\n"
      "}\n";

  Property::Map map;

  Property::Map customShader;
  customShader[ "vertexShader" ] = vertexSource;
  customShader[ "fragmentShader" ] = fragmentSource;

  // NOTE: we must turn on alpha blending for the actor (HINT_BLENDING)
  customShader[ "hints" ] = "outputIsTransparent";

  map[ "shader" ] = customShader;
  return map;
}

} // unnamed namespace

FridgeApp::FridgeApp( Application application )
  : mApplication( application ),
    mIsMenuVisible( false ),
    mIsMenuBackgroundVisible( false ),
    mIsFocusInPosters( false ),
    mPosterItemSize( Vector3::ZERO ),
    mMenuItemSize( Vector3::ZERO )
{
  mApplication.InitSignal().Connect( this, &FridgeApp::OnInitialize );

  mApplication.TerminateSignal().Connect(this, &FridgeApp::OnTerminate);
  mApplication.PauseSignal().Connect(this, &FridgeApp::OnPause);
  mApplication.ResumeSignal().Connect(this, &FridgeApp::OnResume);
}

FridgeApp::~FridgeApp()
{
}

long getMS()
{
  struct timeval tp;
  gettimeofday(&tp, NULL);
  long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
  return ms;
}

void FridgeApp::OnInitialize( Application& application )
{
  cout << endl << "(time-logger)Begin FridgeApp-OnInitialize: " << getMS() << endl;

  cout << endl << "(time-logger)Begin   init process: " << getMS() << endl;
  application.InitSignal().Disconnect(this, &FridgeApp::OnInitialize);

  Stage stage( Stage::GetCurrent() );
  const Vector2 stageSize = stage.GetSize();

  stage.KeyEventSignal().Connect( this, &FridgeApp::OnKeyboardEvent );
  KeyboardFocusManager::Get().PreFocusChangeSignal().Connect( this, &FridgeApp::OnKeyboardPreFocusChange );

  // initialise JSON builder
  std::ifstream t(DALI_SCRIPT_DIR "fridge.json");
  std::string script = std::string( (std::istreambuf_iterator< char >( t )), std::istreambuf_iterator< char >() );
  mBuilder = Builder::New();

  // Set default constants
  mBuilder.AddConstant( "IMAGE_PATH", DALI_IMAGE_DIR );
  mBuilder.AddConstant( "SCRIPT_PATH", DALI_SCRIPT_DIR );

  // load JSON file
  mBuilder.LoadFromString( script );

  mPosterItemSize = Vector3(stageSize.x / 4.0f, stageSize.y * 0.85f, 0.0f);
  mPosters.Initialize(mPosterItemSize);

  mMenuItemSize = Vector3(stageSize.x / 8.0f, stageSize.y * 0.25f, 0.0f);
  mMenu.Initialize(mMenuItemSize);

  cout << endl << "(time-logger)Begin   CreateBackground(): " << getMS() << endl;
  CreateBackground();
  cout << endl << "(time-logger)Begin   CreatePosters(): " << getMS() << endl;
  CreatePosters();
  cout << endl << "(time-logger)Begin   CreateTopPanel(): " << getMS() << endl;
  CreateTopPanel();
  cout << endl << "(time-logger)Begin   CreateMenu(): " << getMS() << endl;
  CreateMenu();

  cout << endl << "(time-logger)End__ FridgeApp::OnInitialize: " << getMS() << endl;
}

void FridgeApp::OnTerminate( Application& application )
{
  application.TerminateSignal().Disconnect(this, &FridgeApp::OnTerminate);
  application.PauseSignal().Disconnect(this, &FridgeApp::OnPause);
  application.ResumeSignal().Disconnect(this, &FridgeApp::OnResume);
}

void FridgeApp::OnPause( Application& application )
{
}

void FridgeApp::OnResume( Application& application )
{
}

void FridgeApp::CreateBackground()
{
  ImageView image = ImageView::New( BACKGROUND_IMAGE );
  image.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  image.SetParentOrigin( ParentOrigin::TOP_LEFT );
  image.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  image.SetZ( -4.0f );

  Stage::GetCurrent().Add( image );
}

void FridgeApp::CreatePosters()
{
  Stage stage( Stage::GetCurrent() );
  const Vector2 stageSize = stage.GetSize();

  Actor posterContainer = mPosters.GetContainer();
  posterContainer.SetPosition( Vector3(0.0f, stageSize.y * 0.15f, -2.0f) );
  stage.Add( posterContainer );

  for(int i = 0; i < 4; i++)
  {
    ImageView item = ImageView::New( DALI_IMAGE_DIR "item0.png" );
    mPosters.AddItem(item);

    item = ImageView::New( DALI_IMAGE_DIR "item1.png" );
    mPosters.AddItem(item);

    item = ImageView::New( DALI_IMAGE_DIR "item2.png" );
    mPosters.AddItem(item);

    item = ImageView::New( DALI_IMAGE_DIR "item3.png" );
    mPosters.AddItem(item);

    item = ImageView::New( DALI_IMAGE_DIR "item4.png" );
    mPosters.AddItem(item);

    item = ImageView::New( DALI_IMAGE_DIR "item5.png" );
    mPosters.AddItem(item);
  }

  Actor itemRoot = mPosters.GetItemRoot();
  itemRoot.SetSize(stageSize.x, stageSize.y * 0.85f, 0.0f);
  itemRoot.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  itemRoot.SetParentOrigin( ParentOrigin::TOP_LEFT );
}

void FridgeApp::CreateTopPanel()
{
  Stage stage( Stage::GetCurrent() );
  const Vector2 stageSize = stage.GetSize();

  mTopPanel = ImageView::New( DALI_IMAGE_DIR "banner.png" );
  mTopPanel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mTopPanel.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mTopPanel.SetSize(stageSize.x, stageSize.y * 0.15f, 0.0f);
  mTopPanel.SetZ( -3.0f );

  stage.Add( mTopPanel );
}

void FridgeApp::CreateMenu()
{
  Stage stage( Stage::GetCurrent() );
  const Vector2 stageSize = stage.GetSize();

  Actor menuContainer = mMenu.GetContainer();
  menuContainer.SetPosition( Vector3(0.0f, stageSize.y * 0.75f, 0.0f) );
  stage.Add( menuContainer );

  for(int i = 0; i < 10; i++)
  {
    Actor menuItem = CreateMenuItem("icon0.png", "Weather");
    mMenu.AddItem(menuItem);

    menuItem = CreateMenuItem("icon1.png", "Quotes");
    mMenu.AddItem(menuItem);

    menuItem = CreateMenuItem("icon2.png", "Movie");
    mMenu.AddItem(menuItem);
  }

  mMenu.Hide(stageSize.y * 0.25f, 0.0f, 0.0f);

  CreateMenuBackground();

  Actor itemRoot = mMenu.GetItemRoot();
  itemRoot.SetSize(stageSize.x, stageSize.y * 0.25f, 0.0f);
  itemRoot.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  itemRoot.SetParentOrigin( ParentOrigin::TOP_LEFT );
}

Actor FridgeApp::CreateMenuItem(std::string iconName, std::string title)
{
  mBuilder.AddConstant( "MENU_ITEM_SIZE_X", mMenuItemSize.x );
  mBuilder.AddConstant( "MENU_ITEM_SIZE_Y", mMenuItemSize.y );
  mBuilder.AddConstant( "MENU_ITEM_ICON", iconName );
  mBuilder.AddConstant( "MENU_ITEM_TITLE", title );

  // Load from template
  Actor menuItem = Actor::DownCast( mBuilder.Create( "template-menu-item" ) );

  Actor icon = menuItem.FindChildByName("icon");
  icon.SetSize( Vector3(mMenuItemSize.x * 0.5f, mMenuItemSize.x * 0.5f ,0.0f) );
  icon.SetPosition( Vector3(0.0f, -mMenuItemSize.y * 0.1f, 0.0f) );

  Actor text = icon.FindChildByName("title");
  text.SetPosition( Vector3(0.0f, mMenuItemSize.x * 0.1f, 0.0f) );

  Actor line = menuItem.FindChildByName("line");
  line.SetSize( Vector3(1.0f, mMenuItemSize.y, 0.0f) );

  return menuItem;
}

void FridgeApp::CreateMenuBackground()
{
  Stage stage( Stage::GetCurrent() );
  const Vector2 stageSize = stage.GetSize();

  mMenuBackground = ImageView::New( DALI_IMAGE_DIR "white-pixel.png" );
  mMenuBackground.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mMenuBackground.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mMenuBackground.SetSize(stageSize.x, stageSize.y * 0.25f, 0.0f);
  mMenuBackground.SetPosition( Vector3(0.0f, stageSize.y * 0.75f, -1.0f) );

  mArrowButton = ImageView::New( DALI_IMAGE_DIR "arrow.png" );
  mArrowButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  mArrowButton.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
  mArrowButton.SetSize(stageSize.x * 0.02f, stageSize.y * 0.025f, 0.0f);
  mArrowButton.SetDrawMode( DrawMode::OVERLAY_2D );

  mArrowButton.TouchedSignal().Disconnect( this, &FridgeApp::OnArrowButtonTouched );

  mMenuBackground.Add(mArrowButton);
  stage.Add(mMenuBackground);

  mMenuBackground.RegisterProperty( "uColor", Vector4::ZERO );
  mMenuBackground.RegisterProperty( "uSize", Vector3(stageSize.x, stageSize.y*0.25f, 0.0f) );
  mMenuBackground.RegisterProperty( "uCircleRadius", 0.02f );
  mMenuBackground.RegisterProperty( "uCircleCenter", Vector2(0.5f, ( (0.5f * stageSize.y*0.25f) / stageSize.x) + 0.5 ) );

  Property::Map menuBackgroundEffect = CreateMenuBackgroundEffect();
  mMenuBackground.SetProperty( Toolkit::ImageView::Property::IMAGE, menuBackgroundEffect );
}

void FridgeApp::ShowTopPanel(float duration, float delay)
{
  float progress = 0.0;
  if(mTopPanelHideAnimation)
  {
    progress = mTopPanelHideAnimation.GetCurrentProgress();
    if( progress != 0.0f )
      progress = 1.0f - progress;

    mTopPanelHideAnimation.Stop();
  }

  mTopPanelShowAnimation = Animation::New(duration + delay);
  mTopPanelShowAnimation.AnimateTo(Property(mTopPanel, Actor::Property::COLOR_ALPHA), 1.0f, AlphaFunction::LINEAR, TimePeriod(delay, duration) );
  mTopPanelShowAnimation.PlayFrom(progress);
}

void FridgeApp::HideTopPanel(float duration, float delay)
{
  float progress = 0.0;
  if(mTopPanelShowAnimation)
  {
    progress = mTopPanelShowAnimation.GetCurrentProgress();
    if( progress != 0.0f )
      progress = 1.0f - progress;

    mTopPanelShowAnimation.Stop();
  }

  mTopPanelHideAnimation = Animation::New(duration + delay);
  mTopPanelHideAnimation.AnimateTo(Property(mTopPanel, Actor::Property::COLOR_ALPHA), 0.0f, AlphaFunction::LINEAR, TimePeriod(delay, duration) );
  mTopPanelHideAnimation.PlayFrom(progress);
}

void FridgeApp::ShowMenuBackground(float duration, float delay)
{
  float progress = 0.0;
  if(mMenuBackgroundHideAnimation)
  {
    progress = mMenuBackgroundHideAnimation.GetCurrentProgress();
    if( progress > 0.0f )
      progress = 1.0f - progress;

    mMenuBackgroundHideAnimation.Stop();
  }

  mMenuBackgroundShowAnimation = Animation::New(duration);
  mMenuBackgroundShowAnimation.AnimateTo(Property(mMenuBackground, "uCircleRadius"), 1.0f, AlphaFunction::EASE_IN_OUT_SINE, TimePeriod(delay, duration) );
  mMenuBackgroundShowAnimation.AnimateTo(Property(mMenuBackground, "uColor"), Vector4(1.0f,1.0f,1.0f,1.0f), AlphaFunction::EASE_IN_OUT_SINE, TimePeriod(delay, duration) );
  mMenuBackgroundShowAnimation.AnimateTo(Property(mArrowButton, Actor::Property::COLOR_ALPHA), 0.0f, AlphaFunction::LINEAR, TimePeriod(delay, duration) );
  mMenuBackgroundShowAnimation.SetSpeedFactor(1.0f);
  mMenuBackgroundShowAnimation.PlayFrom(progress);

  mIsMenuBackgroundVisible = true;
}

void FridgeApp::HideMenuBackground(float duration, float delay)
{
  float progress = 0.0;
  if(mMenuBackgroundShowAnimation)
  {
    progress = mMenuBackgroundShowAnimation.GetCurrentProgress();
    if( progress > 0.0f )
      progress = 1.0f - progress;

    mMenuBackgroundShowAnimation.Stop();
  }

  mMenuBackgroundHideAnimation = Animation::New(duration);
  mMenuBackgroundHideAnimation.AnimateTo(Property(mMenuBackground, "uCircleRadius"), 0.02f, AlphaFunction::EASE_IN_OUT_SINE, TimePeriod(delay, duration) );
  mMenuBackgroundHideAnimation.AnimateTo(Property(mMenuBackground, "uColor"), Vector4(0.0f,0.0f,0.0f,1.0f), AlphaFunction::EASE_IN_OUT_SINE, TimePeriod(delay, duration) );
  mMenuBackgroundHideAnimation.AnimateTo(Property(mArrowButton, Actor::Property::COLOR_ALPHA), 1.0f, AlphaFunction::LINEAR, TimePeriod(delay, duration) );
  mMenuBackgroundHideAnimation.SetSpeedFactor(1.0f);
  mMenuBackgroundHideAnimation.PlayFrom(progress);

  mIsMenuBackgroundVisible = false;
}

void FridgeApp::ShowMenu()
{
  Stage stage( Stage::GetCurrent() );
  const Vector2 stageSize = stage.GetSize();

  Vector3 postersViewSize(stageSize.x, (stageSize.y * 0.8f) * 0.85f, 0.0f);
  Vector3 menuItemSize((stageSize.x / 4.0f) * 0.85f, (stageSize.y * 0.8f) * 0.85f, 0.0f);
  float posterPositionY = 0.15f * stageSize.y - (0.75f - 0.8f * 0.85f) * stageSize.y * 0.5f;
  mPosters.Expand(-posterPositionY, postersViewSize, menuItemSize, 0.5, 0.0);
  HideTopPanel(0.2f, 0.0f);

  ShowMenuBackground(1.0f, 0.3f);
  mMenu.Show(0.5f, 0.8f, 0.05f);
  mIsMenuVisible = true;
  mIsFocusInPosters = false;

  mMenu.Focus(0, 0.0f, 0.0f);
}

void FridgeApp::HideMenu()
{
  Stage stage( Stage::GetCurrent() );
  const Vector2 stageSize = stage.GetSize();

  HideMenuBackground(1.0f, 0.0f);
  mMenu.Hide(stageSize.y * 0.25f, 0.5f, 0.0f);

  Vector3 postersViewSize(stageSize.x, stageSize.y * 0.85f, 0.0f);
  Vector3 menuItemSize(stageSize.x / 4.0f, stageSize.y * 0.85f, 0.0f);
  mPosters.Expand(0.0f, postersViewSize, menuItemSize, 0.5, 0.0);
  ShowTopPanel(0.2f, 0.8f);

  mIsMenuVisible = false;
  mIsFocusInPosters = true;
}

void FridgeApp::OnKeyboardEvent(const KeyEvent& event)
{
  if( event.state == KeyEvent::Down )
  {
    Stage stage( Stage::GetCurrent() );
    const Vector2 stageSize = stage.GetSize();

    if ( event.keyPressedName == "BackSpace" || event.keyPressedName == "XF86Back" )
    {
      if( mIsMenuVisible )
      {
        HideMenu();

        if( !mIsFocusInPosters )
        {
          mPosters.Focus(mPosters.GetFocusedItem(), 0.3f, 0.05f);
          mIsFocusInPosters = true;
        }
      }
    }
    else if ( event.keyPressedName == "XF86AudioNext" || event.keyPressedName == "Next" )
    {
      if( mIsFocusInPosters )
      {
        mPosters.Scroll( -stageSize.x, 1.0f, 0.05f, mPosters.GetFirstVisibleItemId());
      }
      else if( mMenu.IsFocused() )
      {
        mMenu.Scroll( -stageSize.x, 1.0f, 0.05f,  mMenu.GetFirstVisibleItemId());
      }
    }
    else if ( event.keyPressedName == "XF86AudioPrev" || event.keyPressedName == "Prior" )
    {
      if( mIsFocusInPosters )
      {
        mPosters.Scroll( stageSize.x, 1.0f, 0.05f, mPosters.GetLastVisibleItemId());
      }
      else if( mMenu.IsFocused() )
      {
        mMenu.Scroll( stageSize.x, 1.0f, 0.05f,  mMenu.GetLastVisibleItemId());
      }
    }
  }
}

Actor FridgeApp::OnKeyboardPreFocusChange( Actor current, Actor proposed, Control::KeyboardFocus::Direction direction )
{
  if (direction == Dali::Toolkit::Control::KeyboardFocus::UP )
  {
    if( !mIsFocusInPosters )
    {
      mIsFocusInPosters = true;
      return mPosters.Focus(mPosters.GetFirstVisibleItemId(), 0.0f, 0.0f);
    }
  }
  else if (direction == Dali::Toolkit::Control::KeyboardFocus::DOWN)
  {
      if( mIsFocusInPosters )
      {
        if( !mIsMenuVisible )
        {
          ShowMenu();
        }
        else
        {
          mMenu.Focus(mMenu.GetFirstVisibleItemId(), 0.0f, 0.0f);
        }

        mIsFocusInPosters = false;
        return mMenu.GetCurrentFocusedActor();
      }
  }
  else if (direction == Dali::Toolkit::Control::KeyboardFocus::LEFT)
  {
    if( mIsFocusInPosters )
    {
      return mPosters.FocusPrevious();
    }
    else
    {
      return mMenu.FocusPrevious();
    }
  }
  else if (direction == Dali::Toolkit::Control::KeyboardFocus::RIGHT)
  {
    if( mIsFocusInPosters )
    {
      return mPosters.FocusNext();
    }
    else
    {
      if( mIsMenuVisible )
      {
        return mMenu.FocusNext();
      }
      else
      {
        mIsFocusInPosters = true;
      }
    }
  }

  return current;
}

bool FridgeApp::OnArrowButtonTouched(Actor indicator, const TouchEvent& touchEvent)
{
  const TouchPoint& touchPoint = touchEvent.GetPoint( 0 );

  if (touchPoint.state == TouchPoint::Down)
  {
    ShowMenu();
  }

  return true;
}

void FridgeApp::MainLoop()
{
  mApplication.MainLoop();
}

void FridgeApp::OnQuit(PropertyNotification& source)
{
  mApplication.Quit();
}

int main( int argc, char* argv[] )
{
  FridgeApp fridgeApp( Application::New( &argc, &argv, "Dali-Fridge" ) );
  fridgeApp.MainLoop();
  return 0;
}

