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
  const char* const DELETE_IMAGE      = DALI_IMAGE_DIR "text-field-delete.png";

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

    mButton = CreateFolderButton();
    mButton.ClickedSignal().Connect( this, &TextFieldExample::OnButtonClicked );
    stage.Add( mButton );
  }

  PushButton CreateFolderButton()
  {
    PushButton button = PushButton::New();
    ResourceImage image = ResourceImage::New( FOLDER_ICON_IMAGE );
    ImageActor folderButton = ImageActor::New( image );
    folderButton.SetColor( Color::WHITE );
    button.SetButtonImage( folderButton );
    button.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    button.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    button.SetSize( image.GetWidth(), image.GetHeight() );

    return button;
  }

  bool OnButtonClicked( Toolkit::Button button )
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    // Launch a pop-up containing TextField & Delete button
    mTable = TableView::New( 1, 2 );
    mTable.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mTable.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    mTable.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mTable.SetFitWidth( 1 );

    mField = CreateTextField( stageSize, mButtonLabel );
    mField.KeyInputFocusGainedSignal().Connect( this, &TextFieldExample::OnKeyInputFocusGained );
    mField.KeyInputFocusLostSignal().Connect(   this, &TextFieldExample::OnKeyInputFocusLost );
    mField.TextChangedSignal().Connect( this, &TextFieldExample::SetDeleteButtonVisibility );
    mTable.AddChild( mField, TableView::CellPosition( 0, 0 ) );

    Image deleteImage = ResourceImage::New( DELETE_IMAGE );
    mDeleteButton = PushButton::New();
    mDeleteButton.TouchedSignal().Connect( this, &TextFieldExample::OnDeleteButtonTouched );
    mDeleteButton.SetButtonImage( deleteImage );
    SetDeleteButtonVisibility( mField );

    mPopup = CreatePopup( stageSize.width * 0.8f );
    mPopup.Add( mTable );
    mPopup.TouchedSignal().Connect( this, &TextFieldExample::OnPopupTouched );
    mPopup.OutsideTouchedSignal().Connect( this, &TextFieldExample::OnPopupOutsideTouched );
    mPopup.Show();

    return true;
  }

  TextField CreateTextField( const Vector2& stageSize, const std::string& text )
  {
    TextField field = TextField::New();
    field.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    field.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    field.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
    field.SetProperty( TextField::Property::TEXT, text );
    field.SetProperty( TextField::Property::TEXT_COLOR, Vector4( 0.0f, 1.0f, 1.0f, 1.0f ) ); // CYAN
    field.SetProperty( TextField::Property::PRIMARY_CURSOR_COLOR, Color::WHITE );
    field.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "Unnamed folder" );
    field.SetProperty( TextField::Property::PLACEHOLDER_TEXT_FOCUSED, "Enter folder name." );
    field.SetProperty( TextField::Property::DECORATION_BOUNDING_BOX, Rect<int>( BORDER_WIDTH, BORDER_WIDTH, stageSize.width - BORDER_WIDTH*2, stageSize.height - BORDER_WIDTH*2 ) );
    field.SetProperty( TextField::Property::VERTICAL_ALIGNMENT, "CENTER" );
    field.SetProperty( TextField::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

    return field;
  }

  void OnKeyInputFocusGained( Control textField )
  {
    SetDeleteButtonVisibility( TextField::DownCast(textField) );
  }

  void OnKeyInputFocusLost( Control textField )
  {
    mTable.RemoveChildAt( TableView::CellPosition( 0, 1 ) );
  }

  void SetDeleteButtonVisibility( TextField textField )
  {
    std::string str = textField.GetProperty<std::string>(TextField::Property::TEXT);
    int textCount = str.size();

    if( textCount > 0 )
    {
      mTable.AddChild( mDeleteButton, TableView::CellPosition( 0, 1 ) );
    }
    else
    {
      mTable.RemoveChildAt( TableView::CellPosition( 0, 1 ) );
    }
  }

  Popup CreatePopup( float width )
  {
    Popup popup = Popup::New();
    popup.SetParentOrigin( ParentOrigin::CENTER );
    popup.SetAnchorPoint( AnchorPoint::CENTER );
    popup.SetSize( width, 0.0f );
    popup.HideTail();
    popup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::HEIGHT );
    popup.SetSizeModeFactor( POPUP_SIZE_FACTOR_TO_PARENT );

    return popup;
  }

  bool OnDeleteButtonTouched( Actor actor, const TouchEvent& event )
  {
    if( event.points.size() > 0 &&
        TouchPoint::Up == event.points[0].state )
    {
      if( mField )
      {
        mField.SetProperty( TextField::Property::TEXT, "" );
      }

      mButtonLabel = "";
      if( mButton )
      {
        mButton.SetLabel( mButtonLabel );
      }
    }

    return true; // Consume events to avoid reaching OnPopupTouched()
  }

  void OnPopupOutsideTouched()
  {
    // Update the folder text
    if( mButton && mField )
    {
      Property::Value text = mField.GetProperty( TextField::Property::TEXT );
      mButtonLabel = text.Get< std::string >();
      mButton.SetLabel( mButtonLabel );
    }

    // Hide & discard the pop-up
    if( mPopup )
    {
      mPopup.Hide();
    }
    mField.Reset();
    mPopup.Reset();
  }

  bool OnPopupTouched( Actor actor, const TouchEvent& event )
  {
    // End edit mode for TextField if parent Popup touched.
    if(event.GetPointCount() > 0)
    {
      const TouchPoint& point = event.GetPoint(0);
      switch(point.state)
      {
        case TouchPoint::Down:
        {
          // Update the folder text and lose focus for Key events
          if( mButton && mField )
          {
            Property::Value text = mField.GetProperty( TextField::Property::TEXT );
            mButtonLabel = text.Get< std::string >();
            mButton.SetLabel( mButtonLabel );
            mField.ClearKeyInputFocus();
          }
          break;
        }
        default:
        {
          break;
        }
      } // end switch
    }

    return true;
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
  std::string mButtonLabel;

  // Pop-up contents
  TableView mTable;
  TextField mField;
  PushButton mDeleteButton;
  Popup mPopup;
};

void RunTest( Application& application )
{
  TextFieldExample test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DALI_DEMO_THEME_PATH );

  RunTest( application );

  return 0;
}
