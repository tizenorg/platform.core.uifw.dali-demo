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
#include<sstream>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/text-abstraction/text-abstraction.h>

// INTERNAL INCLUDES
#include "shared/multi-language-strings.h"
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace MultiLanguageStrings;

namespace
{

const char* const FOLDER_BACKGROUND_IMAGE = DALI_IMAGE_DIR "folder_appicon_empty_bg.png";
const char* const NEW_FOLDER_ICON_IMAGE = DALI_IMAGE_DIR "icon_add_folder.png";

const char* const ICON_FACEBOOK( DALI_IMAGE_DIR "gallery_icon_facebook_ef.png" );
const char* const ICON_ALARM( DALI_IMAGE_DIR "icon_alarm_1.png" );
const char* const ICON_CAMERA( DALI_IMAGE_DIR "icon_camera.png" );
const char* const ICON_PLANNER( DALI_IMAGE_DIR "icon_planner.png" );

const Size MAX_ITEM_SIZE = Size( 100.0f, 100.0f );
const Size TEXT_FIELD_MINIMUM_SIZE = Size( 150.0f, 30.0f );
const Size TEXT_FIELD_MAXIMUM_SIZE = Size( 250.0f, std::numeric_limits<float>::max() );
const Size FOLDER_EDIT_POPUP_MINIMUM_SIZE = Size( 300.0f, 400.0f );
const Size MAX_CHARACTERS_EXCEEDED_LABEL_SIZE = Size( 100.0f, std::numeric_limits<float>::max() );

const Rect<float> TEXT_FIELD_PADDING = Rect<float>( 10.0f, 10.0f, 25.0f, 15.0f );  // left, right, bottom, top
const Rect<float> ICONS_GRID_PADDING= Rect<float>( 10.0f, 10.0f, 10.0f, 10.0f );  // left, right, bottom, top
const Size MINI_ITEMS_GRID_PADDING = Size( 4.0f, 4.0f );

const float LABEL_FONT_SIZE = 12.0f;
const float MAX_CHARACTERS_EXCEEDED_FONT_SIZE = 12.0f;

const int MAX_FOLDER_LENGTH = 21;
const DemoHelper::ViewStyle VIEW_STYLE( 0.1f, 0.7f, 80.f, 4.f );
const Vector3 GRID_OFFSET = Vector3( -20.0f, -20.0f, 0.0f );
const float TEXT_OFFSET_FROM_TOP = 30.0f;
const float BORDER_WIDTH = 4.0f;
const float UNDERLINE_HEIGHT = 4.0f;

const unsigned int KEY_ZERO = 10;
const unsigned int KEY_ONE = 11;
const unsigned int KEY_F = 41;
const unsigned int KEY_H = 43;
const unsigned int KEY_V = 55;
const unsigned int KEY_M = 58;
const unsigned int KEY_L = 46;
const unsigned int KEY_P = 33;
const unsigned int KEY_S = 39;
const unsigned int KEY_PLUS = 21;
const unsigned int KEY_MINUS = 20;

const char* H_ALIGNMENT_STRING_TABLE[] =
{
  "BEGIN",
  "CENTER",
  "END"
};

const unsigned int H_ALIGNMENT_STRING_COUNT = sizeof( H_ALIGNMENT_STRING_TABLE ) / sizeof( H_ALIGNMENT_STRING_TABLE[0u] );

const char* V_ALIGNMENT_STRING_TABLE[] =
{
  "TOP",
  "CENTER",
  "BOTTOM"
};

// Allows toggling between various font sizes
const float POINT_SIZES[] =
{
    16.0f, 32.0f
};

const unsigned int POINT_SIZES_COUNT = sizeof( POINT_SIZES ) / sizeof( POINT_SIZES[0u] );

const unsigned int V_ALIGNMENT_STRING_COUNT = sizeof( V_ALIGNMENT_STRING_TABLE ) / sizeof( V_ALIGNMENT_STRING_TABLE[0u] );

const Size ACTIVATE_BUTTON_SIZE( 60.0f, 60.0f );

const unsigned int MAX_NUMBER_OF_FOLDERS = 1;

const unsigned int GRID_COLUMNS = 4;
const unsigned int GRID_ROWS = 1;

const unsigned int LAYOUT_ROWS = 2;
const unsigned int LAYOUT_COLUMNS = 1;

const unsigned int ITEM_EDIT_ROWS = 2;
const unsigned int ITEM_EDIT_COLUMNS = 3;

const Vector3 OFFSET_FROM_EDGE = Vector3( 20.0f, -20.0f, 0.0f );

} // unnamed namespace

/**
 * @brief The main class of the demo.
 * Demo shows a Text field appearing in a container after the start button is pressed.
 * Touching outside of the text field and it's container hides them until the button is pressed again.
 */
class TextFieldExample : public ConnectionTracker
{
public:

  struct ItemData
  {
    std::string itemName;
    TextLabel textLabel;
    ImageActor container;
  };

  TextFieldExample( Application& application )
  : mApplication( application ),
    mLanguageId( 0u ),
    mAlignment( 0u ),
    mPointSize( 0u ),
    mItems(),
    mNotifyingMaxCharactersExceeded( false )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextFieldExample::Create );
  }

  ~TextFieldExample()
  {
    // Nothing to do here.
  }

  void OnTapGesture( Actor actor, const TapGesture& gesture )
  {
    OpenFolderForEdit( actor );
  }

  void OnPopupHideFinished()
  {
    UnparentAndReset( mActiveTextField );
    UnparentAndReset( mEditItemPopup );
    UnparentAndReset( mOutsideArea );
    UnparentAndReset( mMaxCharactersExceededMessage );

    mNotifyingMaxCharactersExceeded = false;

    if ( mItems.size() < MAX_NUMBER_OF_FOLDERS )
    {
      mNewFolderButton.SetOpacity(1.0f);
    }
  }

  void OpenFolderForEdit( Actor& actor )
  {
    // Search for actor from the collection of items
    for( std::vector< ItemData >::iterator iter = mItems.begin(); iter != mItems.end(); ++iter )
    {
      if ( (*iter).container == actor )
      {
        EditFolderName( iter );
      }
    }
  }

  void SetUpTextFieldUnderline( ImageActor& imageActor, Actor& parent )
  {
    if ( !imageActor )
    {
      imageActor = Dali::Toolkit::CreateSolidColorActor(Color::BLACK);
      imageActor.SetOpacity( 0.8f );
      imageActor.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      imageActor.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
      imageActor.SetResizePolicy( ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::WIDTH );
      imageActor.SetResizePolicy( ResizePolicy::FIXED, Dimension::HEIGHT );
      imageActor.SetSize( Size( 0.0f, UNDERLINE_HEIGHT ) );

      if ( parent )
      {
        parent.Add( imageActor );
      }
    }
  }

  void SetUpNewFolderButton( PushButton& button, Actor& parent )
  {
    button = PushButton::New();
    ImageActor folderButton = ImageActor::New( ResourceImage::New( NEW_FOLDER_ICON_IMAGE )  );
    folderButton.SetColor( Color::BLACK );
    button.SetButtonImage( folderButton );
    button.SetParentOrigin( ParentOrigin::CENTER );
    button.SetSize( ACTIVATE_BUTTON_SIZE );
    button.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    button.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    button.SetPosition( OFFSET_FROM_EDGE );
    parent.Add( button );
    button.ClickedSignal().Connect( this, &TextFieldExample::OnActivateButtonClicked );
  }

  void SetUpFolderEditPopup( Popup& popup, TableView& layout )
  {
    if ( !popup )
    {
      layout = TableView::New( LAYOUT_ROWS, LAYOUT_COLUMNS );
      layout.SetAnchorPoint( AnchorPoint::CENTER );
      layout.SetParentOrigin( ParentOrigin::CENTER );
      layout.SetMinimumSize( FOLDER_EDIT_POPUP_MINIMUM_SIZE );
      layout.SetName("FolderEditLayout");

      popup = Popup::New();
      popup.SetParentOrigin( ParentOrigin::CENTER );
      popup.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );
      popup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::WIDTH );
      popup.SetSizeModeFactor( Vector3( 0.75f, 1.0f, 1.0f ) );
      popup.SetName("FolderEditPopup");

      popup.HiddenSignal().Connect( this, &TextFieldExample::OnPopupHideFinished );
      popup.OutsideTouchedSignal().Connect( this, &TextFieldExample::OnOutsidePopupTouched );
      popup.Add( layout );
    }
  }

  void OnOutsidePopupTouched()
  {
    mEditItemPopup.Hide();
  }

  void TextFieldEditingEnded( Control control )
  {
    Property::Value textFieldText = control.GetProperty( TextLabel::Property::TEXT );
    mActiveItem->itemName = textFieldText.Get< std::string >();
    mActiveItem->textLabel.SetProperty( TextLabel::Property::TEXT, mActiveItem->itemName );
    UnparentAndReset( mMaxCharactersExceededMessage );
    mNotifyingMaxCharactersExceeded = false;
  }

  void TextFieldMaxCharactersReached ( TextField textField )
  {
    if ( !mNotifyingMaxCharactersExceeded )
    {
      mNotifyingMaxCharactersExceeded = true;

      mMaxCharactersExceededMessage = TextLabel::New();

      std::ostringstream os;
      os << mActiveTextField.GetProperty( TextField::Property::MAX_LENGTH ); ;

      const std::string maxCharactersMessage = "Up to " +  os.str() + " characters available";

      mMaxCharactersExceededMessage.SetProperty( TextLabel::Property::TEXT, maxCharactersMessage );
      mMaxCharactersExceededMessage.SetProperty( TextLabel::Property::TEXT_COLOR, Color::BLACK );
      mMaxCharactersExceededMessage.SetBackgroundColor( Color::WHITE ); // todo Does not show background
      mMaxCharactersExceededMessage.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
      mMaxCharactersExceededMessage.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
      mMaxCharactersExceededMessage.SetProperty( TextLabel::Property::MULTI_LINE, true );
      mMaxCharactersExceededMessage.SetProperty( TextLabel::Property::POINT_SIZE, MAX_CHARACTERS_EXCEEDED_FONT_SIZE );
      mMaxCharactersExceededMessage.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
      mMaxCharactersExceededMessage.SetMaximumSize( MAX_CHARACTERS_EXCEEDED_LABEL_SIZE );
      mWallpaper.Add ( mMaxCharactersExceededMessage );
    }
  }

  void SetUpTextField( TextField& field, Size stageSize, std::string& text )
  {
    if ( !field )
    {
      field = TextField::New();

      float pointSize = POINT_SIZES[ mPointSize ];
      field.SetProperty( TextField::Property::POINT_SIZE, pointSize );

      if ( text != "")
      {
        field.SetProperty( TextField::Property::TEXT, text );
      }
      else
      {
        field.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "Unnamed" );
        field.SetProperty( TextField::Property::PLACEHOLDER_TEXT_FOCUSED, "Enter folder name." );
        field.SetProperty( TextField::Property::PLACEHOLDER_TEXT_COLOR, Color::WHITE);
      }
      field.SetMinimumSize( TEXT_FIELD_MINIMUM_SIZE );
      field.SetMaximumSize( TEXT_FIELD_MAXIMUM_SIZE );
      field.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      field.SetProperty( TextField::Property::DECORATION_BOUNDING_BOX, Rect<int>( BORDER_WIDTH, BORDER_WIDTH, stageSize.width - BORDER_WIDTH*2, stageSize.height - BORDER_WIDTH*2 ) );
      field.SetProperty( TextField::Property::MAX_LENGTH, MAX_FOLDER_LENGTH );
      field.SetProperty( TextLabel::Property::TEXT_COLOR, Color::WHITE ); // todo Does not show as white when editing
      field.KeyInputFocusLostSignal().Connect( this, &TextFieldExample::TextFieldEditingEnded );
      field.MaxLengthReachedSignal().Connect( this, &TextFieldExample::TextFieldMaxCharactersReached );
    }
  }

  bool OnActivateButtonClicked( Button button )
  {
    CreateNewFolderAndAddToGrid( button );

    return true;
  }

  void SetUpGrid( Actor& parent )
  {
    mGrid = TableView::New( GRID_ROWS , GRID_COLUMNS ); // 1 Row, 4 Columns
    mGrid.SetResizePolicy( ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::WIDTH );
    mGrid.SetSizeModeFactor( Vector3( GRID_OFFSET ) );
    mGrid.SetFitHeight( 0 );
    mGrid.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mGrid.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mGrid.SetPosition( 0.0f, VIEW_STYLE.mToolBarHeight*1.3 );
    parent.Add( mGrid );
  }

  void CreateNewFolderAndAddToGrid( Button& buttonToHide )
  {
    if ( mItems.size() < MAX_NUMBER_OF_FOLDERS )
    {
      buttonToHide.SetOpacity(0.0f);
      TextLabel newLabel = TextLabel::New();
      ImageActor folder = ImageActor::New( ResourceImage::New( FOLDER_BACKGROUND_IMAGE ) );
      folder.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
      folder.SetSize( MAX_ITEM_SIZE );
      folder.Add( newLabel );

      TableView gridOfItemsWithinFolder = TableView::New( ITEM_EDIT_ROWS, ITEM_EDIT_COLUMNS );
      gridOfItemsWithinFolder.SetAnchorPoint( AnchorPoint::CENTER );
      gridOfItemsWithinFolder.SetParentOrigin( ParentOrigin::CENTER );
      gridOfItemsWithinFolder.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      gridOfItemsWithinFolder.SetSizeModeFactor( Vector3( 0.85, 0.85, 1.0 ) );
      gridOfItemsWithinFolder.SetCellPadding( MINI_ITEMS_GRID_PADDING );
      folder.Add( gridOfItemsWithinFolder );

      // Create Icons to display whilst editting item name
      ImageActor facebookIcon = ImageActor::New( ResourceImage::New( ICON_FACEBOOK ) );
      ImageActor facebookIcon2 = ImageActor::New( ResourceImage::New( ICON_PLANNER ) );
      ImageActor facebookIcon3 = ImageActor::New( ResourceImage::New( ICON_CAMERA ) );

      SetUpIconInGrid( gridOfItemsWithinFolder, facebookIcon, TableView::CellPosition( 1, 0 ) );
      SetUpIconInGrid( gridOfItemsWithinFolder, facebookIcon2, TableView::CellPosition( 1, 1 ) );
      SetUpIconInGrid( gridOfItemsWithinFolder, facebookIcon3, TableView::CellPosition( 1, 2 ) );

      ItemData newItem;
      newItem.itemName = ""; // Initially set the name to empty so placeholder text used
      newItem.textLabel = newLabel;
      newItem.container = folder;
      newLabel.SetBackgroundColor(Vector4( 0.0f, .0f, 0.0f, 1.0f));
      newLabel.SetProperty( TextLabel::Property::TEXT, newItem.itemName );
      newLabel.SetProperty( TextLabel::Property::TEXT_COLOR, Color::BLACK );
      newLabel.SetProperty( TextLabel::Property::MULTI_LINE, true );
      newLabel.SetProperty( TextLabel::Property::POINT_SIZE, LABEL_FONT_SIZE );
      newLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
      newLabel.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
      newLabel.SetAnchorPoint( AnchorPoint::TOP_CENTER );

      mGrid.Add( folder );
      mTapDetector.Attach( folder );

      // Push new item into end of vector and start edit mode on that item
      mItems.push_back( newItem );
      EditFolderName( mItems.end()-1 );
    }
  }

  void SetUpIconInGrid( TableView& targetTableView, ImageActor& iconImage, TableView::CellPosition cellPosition )
  {
      iconImage.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      iconImage.SetSizeScalePolicy( SizeScalePolicy::FIT_WITH_ASPECT_RATIO );
      targetTableView.AddChild( iconImage, cellPosition );
  }

  void EditFolderName( std::vector<TextFieldExample::ItemData>::iterator item)
  {
    // container to hold text field
    SetUpFolderEditPopup( mEditItemPopup, mEditItemPopupLayout );

    // Setup and add TextField to Popup for folder name edit
    SetUpTextField( mActiveTextField, mStageSize, (*item).itemName );
    Alignment textFieldAlignment = Alignment::New(  Alignment::HorizontalCenter, Alignment::VerticalCenter );
    textFieldAlignment.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );
    textFieldAlignment.SetMinimumSize( Size( FOLDER_EDIT_POPUP_MINIMUM_SIZE.width, TEXT_FIELD_MINIMUM_SIZE.height ) );
    textFieldAlignment.Actor::SetPadding( TEXT_FIELD_PADDING );
    textFieldAlignment.Add ( mActiveTextField );
    mEditItemPopupLayout.AddChild( textFieldAlignment, TableView::CellPosition( 0, 0 ) );
    mEditItemPopupLayout.SetFitWidth( 0 );
    mEditItemPopupLayout.SetFitHeight( 0 );

    // Add a grid of items to the Folder edit popup
    mPopupIconGrid = TableView::New( ITEM_EDIT_ROWS, ITEM_EDIT_COLUMNS );
    mPopupIconGrid.Actor::SetPadding( ICONS_GRID_PADDING );
    mPopupIconGrid.SetAnchorPoint( AnchorPoint::CENTER );
    mPopupIconGrid.SetParentOrigin( ParentOrigin::CENTER );
    mPopupIconGrid.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mPopupIconGrid.SetSizeModeFactor( Vector3( 0.95, 0.95, 1.0 ) );
    mEditItemPopupLayout.AddChild( mPopupIconGrid, TableView::CellPosition ( 1, 0) );
    // Create Icons to display whilst editting item name
    ImageActor facebookIcon = ImageActor::New( ResourceImage::New( ICON_FACEBOOK ) );
    ImageActor facebookIcon2 = ImageActor::New( ResourceImage::New( ICON_PLANNER ) );
    ImageActor facebookIcon3 = ImageActor::New( ResourceImage::New( ICON_CAMERA ) );
    // todo Loop for all icons.
    SetUpIconInGrid( mPopupIconGrid, facebookIcon, TableView::CellPosition( 0, 0 ) );
    SetUpIconInGrid( mPopupIconGrid, facebookIcon2, TableView::CellPosition( 0, 1 ) );
    SetUpIconInGrid( mPopupIconGrid, facebookIcon3, TableView::CellPosition( 0, 2 ) );

    // Add Underline effect to TextField
    ImageActor fieldUnderline;
    SetUpTextFieldUnderline( fieldUnderline, mActiveTextField );

    mNewFolderButton.SetOpacity( 0.0f );
    mActiveItem = item;
    mEditItemPopup.Show();
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    DemoHelper::RequestThemeChange();

    Stage stage = Stage::GetCurrent();

    mTapDetector = TapGestureDetector::New();
    mTapDetector.DetectedSignal().Connect(this, &TextFieldExample::OnTapGesture);

    stage.KeyEventSignal().Connect(this, &TextFieldExample::OnKeyEvent);
    mStageSize = stage.GetSize();

    mWallpaper = Dali::Toolkit::CreateSolidColorActor(Color::WHITE);
    mWallpaper.SetAnchorPoint( AnchorPoint::CENTER );
    mWallpaper.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

    stage.Add( mWallpaper );

    // button to show text field
    SetUpNewFolderButton( mNewFolderButton, mWallpaper );

    SetUpGrid( mWallpaper );
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
        mTapDetector.DetectedSignal().Disconnect(this, &TextFieldExample::OnTapGesture );
        mApplication.Quit();
      }
      else if( event.IsCtrlModifier() && mActiveTextField )
      {
        switch( event.keyCode )
        {
          // Select rendering back-end
          case KEY_ZERO: // fall through
          case KEY_ONE:
          {
            mActiveTextField.SetProperty( TextField::Property::RENDERING_BACKEND, event.keyCode - 10 );
            break;
          }
          case KEY_H: // Horizontal alignment
          {
            if( ++mAlignment >= H_ALIGNMENT_STRING_COUNT )
            {
              mAlignment = 0u;
            }

            mActiveTextField.SetProperty( TextField::Property::HORIZONTAL_ALIGNMENT, H_ALIGNMENT_STRING_TABLE[ mAlignment ] );
            break;
          }
          case KEY_V: // Vertical alignment
          {
            if( ++mAlignment >= V_ALIGNMENT_STRING_COUNT )
            {
              mAlignment = 0u;
            }

            mActiveTextField.SetProperty( TextField::Property::VERTICAL_ALIGNMENT, V_ALIGNMENT_STRING_TABLE[ mAlignment ] );
            break;
          }
          case KEY_L: // Language
          {
            const Language& language = LANGUAGES[ mLanguageId ];

            mActiveTextField.SetProperty( TextField::Property::TEXT , language.text );

            if( ++mLanguageId >= NUMBER_OF_LANGUAGES )
            {
              mLanguageId = 0u;
            }
            break;
          }
          case KEY_P: // Text Point size
          {
            if( ++mPointSize >= POINT_SIZES_COUNT )
            {
              mPointSize = 0u;
            }
            break;
          }
          case KEY_S: // Shadow color
          {
            if( Color::BLACK == mActiveTextField.GetProperty<Vector4>( TextField::Property::SHADOW_COLOR ) )
            {
              mActiveTextField.SetProperty( TextField::Property::SHADOW_COLOR, Color::RED );
            }
            else
            {
              mActiveTextField.SetProperty( TextField::Property::SHADOW_COLOR, Color::BLACK );
            }
            break;
          }
          case KEY_PLUS: // Increase shadow offset
          {
            mActiveTextField.SetProperty( TextField::Property::SHADOW_OFFSET, mActiveTextField.GetProperty<Vector2>( TextField::Property::SHADOW_OFFSET ) + Vector2( 1.0f, 1.0f ) );
            break;
          }
          case KEY_MINUS: // Decrease shadow offset
          {
            mActiveTextField.SetProperty( TextField::Property::SHADOW_OFFSET, mActiveTextField.GetProperty<Vector2>( TextField::Property::SHADOW_OFFSET ) - Vector2( 1.0f, 1.0f ) );
            break;
          }
        }
      }
    }
  }

private:

  Application& mApplication;

  TapGestureDetector mTapGestureDetector;

  unsigned int mLanguageId;
  unsigned int mAlignment;
  unsigned int mPointSize;
  Size mStageSize;
  ImageActor mWallpaper;
  Popup mEditItemPopup;
  TextField mActiveTextField;
  TableView mEditItemPopupLayout;
  TableView mPopupIconGrid;
  TableView mGrid;
  ImageActor mOutsideArea;
  PushButton mNewFolderButton;
  TapGestureDetector mTapDetector;
  TextLabel mMaxCharactersExceededMessage;

  std::vector<TextFieldExample::ItemData>::iterator mActiveItem;
  std::vector<TextFieldExample::ItemData> mItems;

  bool mNotifyingMaxCharactersExceeded:1;
};

void RunTest( Application& application )
{
  TextFieldExample test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
