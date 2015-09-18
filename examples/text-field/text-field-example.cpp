/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

/**
 * @file text-field-example.cpp
 * @brief Basic usage of TextField control
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/popup/popup.h>
#include <iostream>
#include <dali/public-api/events/touch-point.h>

// INTERNAL INCLUDES
#include "shared/multi-language-strings.h"
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace MultiLanguageStrings;

namespace
{

  const char* const FOLDER_ICON_IMAGE = DALI_IMAGE_DIR "folder_appicon_empty_bg.png";
  const char* const FOLDER_OPEN_ICON_IMAGE = DALI_IMAGE_DIR "folder_appicon_empty_open_bg.png";
  const std::string TEXT_SELECTION_POPUP_BUTTON_STYLE_NAME( "textselectionpopupbutton" );

  const float BORDER_WIDTH = 4.0f;

  const Vector3 POPUP_SIZE_FACTOR_TO_PARENT = Vector3( 0.0, 0.25, 0.0 );

} // unnamed namespace

/**
 * @brief The main class of the demo.
 */
class TextFieldExample : public ConnectionTracker
{
public:

  TextFieldExample( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextFieldExample::Create );
  }

  ~TextFieldExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();

    stage.SetBackgroundColor( Vector4( 0.04f, 0.345f, 0.392f, 1.0f ) );
    stage.KeyEventSignal().Connect(this, &TextFieldExample::OnKeyEvent);

    mButton = CreateFolderButton( AnchorPoint::TOP_LEFT );
    mButton.ClickedSignal().Connect( this, &TextFieldExample::OnButtonClicked );

    mButton2 = CreateFolderButton(  AnchorPoint::TOP_RIGHT );
    mButton2.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    mButton2.ClickedSignal().Connect( this, &TextFieldExample::OnButton2Clicked );

    stage.Add( mButton );
    stage.Add( mButton2 );
  }

  PushButton CreateFolderButton( Vector3 anchorPoint )
  {
    PushButton button = PushButton::New();
    button.SetUnselectedImage( FOLDER_ICON_IMAGE );
    button.SetSelectedImage( FOLDER_OPEN_ICON_IMAGE );
    button.SetAnchorPoint(  anchorPoint );
    button.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    ResourceImage imageClosed = ResourceImage::New( FOLDER_ICON_IMAGE );
    button.SetSize( imageClosed.GetWidth(), imageClosed.GetHeight() );

    return button;
  }

  PushButton CreateButton()
  {
    // 1. Create a option.
    Toolkit::PushButton option = Toolkit::PushButton::New();
    option.SetName( "the-push-me-button" );
    option.SetAnimationTime( 0.0f );
    option.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    option.SetParentOrigin( ParentOrigin::TOP_CENTER );
    option.SetAnchorPoint( AnchorPoint:: TOP_CENTER );
    option.SetProperty( Toolkit::Control::Property::STYLE_NAME, TEXT_SELECTION_POPUP_BUTTON_STYLE_NAME );
    // Label properties.
    Property::Map buttonLabelProperties;
    buttonLabelProperties.Insert( "text", "push me" );
    option.SetProperty( Toolkit::Button::Property::LABEL, buttonLabelProperties );
    return option;
  }

  bool OnButtonClicked( Toolkit::Button button )
  {
    Stage stage = Stage::GetCurrent();

    mLabelledButton = CreateButton();

    stage.Add( mLabelledButton );

    return true;
  }

  bool OnButton2Clicked( Toolkit::Button button )
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();
    // Launch a pop-up containing TextField
    mField = CreateTextField( stageSize );
    stage.Add( mField );

    return true;
  }

  TextField CreateTextField( const Vector2& stageSize )
  {
    TextField field = TextField::New();
    field.SetName("text-field");
    field.SetAnchorPoint( AnchorPoint::CENTER );
    field.SetParentOrigin( ParentOrigin::CENTER );
    field.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    field.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
    field.SetProperty( TextField::Property::TEXT_COLOR, Vector4( 0.0f, 1.0f, 1.0f, 1.0f ) ); // CYAN
    field.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "Unnamed folder" );
    field.SetProperty( TextField::Property::PLACEHOLDER_TEXT_FOCUSED, "Enter folder name." );
    field.SetProperty( TextField::Property::DECORATION_BOUNDING_BOX, Rect<int>( BORDER_WIDTH, BORDER_WIDTH, stageSize.width - BORDER_WIDTH*2, stageSize.height - BORDER_WIDTH*2 ) );
    field.SetProperty( TextField::Property::FONT_FAMILY, "choco cooky" );

    return field;
  }
  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:

  Application& mApplication;

  // This button launches a pop-up containing TextField
  PushButton mButton;
  PushButton mButton2;

  PushButton mLabelledButton;
  std::string mButtonLabel;

  TextField mField;

};

void RunTest( Application& application )
{
  TextFieldExample test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int main( int argc, char **argv )
{
  // DALI_DEMO_THEME_PATH not passed to Application so TextField example uses default Toolkit style sheet.
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
