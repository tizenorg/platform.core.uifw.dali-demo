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

  const float BORDER_WIDTH = 4.0f;

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

    mButton = CreateFolderButton();
    mButton.ClickedSignal().Connect( this, &TextFieldExample::OnButtonClicked );
    stage.Add( mButton );
  }

  PushButton CreateFolderButton()
  {
    PushButton button = PushButton::New();
    button.SetUnselectedImage( FOLDER_ICON_IMAGE );
    button.SetSelectedImage( FOLDER_OPEN_ICON_IMAGE );
    button.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    button.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    ResourceImage imageClosed = ResourceImage::New( FOLDER_ICON_IMAGE );
    button.SetSize( imageClosed.GetWidth(), imageClosed.GetHeight() );

    return button;
  }

  bool OnButtonClicked( Toolkit::Button button )
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    // Launch a TextField
    mField = CreateTextField( stageSize, mButtonLabel );
    stage.Add( mField );
 
    return true;
  }

  TextField CreateTextField( const Vector2& stageSize, const std::string& text )
  {
    TextField field = TextField::New();
    field.SetName("text-field");
    field.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    field.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    field.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
    field.SetProperty( TextField::Property::TEXT, text );
    field.SetProperty( TextField::Property::TEXT_COLOR, Vector4( 0.0f, 1.0f, 1.0f, 1.0f ) ); // CYAN
    field.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "Unnamed folder" );
    field.SetProperty( TextField::Property::PLACEHOLDER_TEXT_FOCUSED, "Enter folder name." );
    field.SetProperty( TextField::Property::DECORATION_BOUNDING_BOX, Rect<int>( BORDER_WIDTH, BORDER_WIDTH, stageSize.width - BORDER_WIDTH*2, stageSize.height - BORDER_WIDTH*2 ) );

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

  // This button launches a TextField
  PushButton mButton;
  std::string mButtonLabel;

  // contents
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
