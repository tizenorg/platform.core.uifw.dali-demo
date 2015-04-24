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
#include <dali/public-api/text-abstraction/text-abstraction.h>

// INTERNAL INCLUDES
#include "shared/multi-language-strings.h"
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace MultiLanguageStrings;

namespace
{

const char* const TEXTFIELD_CONTAINER_BACKGROUND_IMAGE = DALI_IMAGE_DIR "folder-viewBG.png";

const char* const BACKGROUND_IMAGE = DALI_IMAGE_DIR "woodColourBG.png";

const char* const NEW_FOLDER_ICON_IMAGE = DALI_IMAGE_DIR "new-folder-icon.png";

const char* const FOLDER_BG_ICON_IMAGE = DALI_IMAGE_DIR "folderBG.png";

const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" ); // Background for toolbar
const char* APPLICATION_TITLE( "Text Field" );

const Size MAX_ITEM_SIZE = Size( 100.0f, 80.0f );
const Size TEXT_FIELD_SIZE = Size(150.0f, 30.0f );

const float LABEL_FONT_SIZE = 12.0f;
const float MAX_CHARACTERS_EXCEEDED_FONT_SIZE = 12.0f;

const int MAX_FOLDER_LENGTH = 17;
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

const unsigned int V_ALIGNMENT_STRING_COUNT = sizeof( V_ALIGNMENT_STRING_TABLE ) / sizeof( V_ALIGNMENT_STRING_TABLE[0u] );

const Size ACTIVATE_BUTTON_SIZE( 60.0f, 60.0f );

const unsigned int NUMBER_OF_ITEMS = 4;

const unsigned int GRID_COLUMNS = 4;
const unsigned int GRID_ROWS = 1;

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

  void SetUpLayer( Layer& layer, Actor& parent )
  {
    if (!layer)
    {
      layer = Layer::New();
      layer.SetAnchorPoint(Dali::AnchorPoint::CENTER);
      layer.SetParentOrigin(Dali::ParentOrigin::CENTER);
      layer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      layer.SetTouchConsumed( true ); // Touch Events will be blocked past this Layer.
      parent.Add( mLayer );
    }
  }
  void OnOutsidePopupTouched()
  {
    mTextFieldPopup.SetState( Popup::POPUP_HIDE, 0.35f  );
  }

  void OnPopupHideFinished()
  {
    mActivationButton.SetOpacity(1.0f);
    Animation opacityAnimation = Animation::New(.2f);
    const float ALPHA_TARGET = 0.0f;
    opacityAnimation.AnimateTo(Property( mOutsideArea, Actor::Property::COLOR_ALPHA ), ALPHA_TARGET );
    opacityAnimation.FinishedSignal().Connect( this, &TextFieldExample::OnPopupOpacityAnimationFinished );
    opacityAnimation.Play();
  }

  void OnPopupOpacityAnimationFinished( Animation&)
  {
    UnparentAndReset( mTextFieldPopup );
    UnparentAndReset( mLayer );
    UnparentAndReset( mOutsideArea );
  }

  void OnEnlargeAnimationFinished( Animation&)
  {
  }

  void OnTapGesture( Actor actor, const TapGesture& gesture )
  {
    if ( actor == mOutsideArea )
    {
      mTapDetector.Detach( mTextFieldPopup );
      mTapDetector.Detach( mOutsideArea );
      mTextFieldPopup.SetState( Popup::POPUP_HIDE, 0.35f  );
    }
    else
    {
      OpenFolderForEdit( actor );
    }

  }

  void OpenFolderForEdit( Actor& actor )
  {
    // Search for actor from the collection of items
    for( std::vector< ItemData >::iterator iter = mItems.begin(); iter != mItems.end(); ++iter )
    {
      if ( (*iter).container == actor )
      {
        EditItemInGrid( iter );
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
      imageActor.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      imageActor.SetResizePolicy( ResizePolicy::FIXED, Dimension::HEIGHT );
      imageActor.SetSize( Size( 0.0f, UNDERLINE_HEIGHT ) );
      if ( parent )
      {
      parent.Add( imageActor );
      }
    }
  }

  void SetUpOutsideTouchArea( ImageActor& imageActor, Actor& parent )
  {
    if ( !imageActor )
    {
      imageActor = Dali::Toolkit::CreateSolidColorActor(Color::BLACK);
      const float ALPHA_TARGET = 0.6f;
      imageActor.SetOpacity(0.0f);
      Animation opacityAnimation = Animation::New(.2f);
      opacityAnimation.AnimateTo(Property( imageActor, Actor::Property::COLOR_ALPHA ), ALPHA_TARGET );
      opacityAnimation.Play();
      imageActor.SetAnchorPoint( AnchorPoint::CENTER );
      imageActor.SetParentOrigin( ParentOrigin::CENTER );
      imageActor.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      imageActor.SetZ( -1.0f );
      parent.Add( imageActor );
    }
  }

  void SetUpActivationButton( PushButton& button, Actor& parent )
  {
    button = PushButton::New();
    button.SetButtonImage( ResourceImage::New( NEW_FOLDER_ICON_IMAGE ) );
    button.SetParentOrigin( ParentOrigin::CENTER );
    button.SetSize( ACTIVATE_BUTTON_SIZE );
    button.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    button.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    button.SetPosition( OFFSET_FROM_EDGE );
    parent.Add( button );
    button.ClickedSignal().Connect( this, &TextFieldExample::OnActivateButtonClicked );
  }

  void SetUpTextFieldPopup( Popup& textFieldPopup )
  {
    if ( !textFieldPopup )
    {
      textFieldPopup =  Popup::New();
      textFieldPopup.SetParentOrigin( ParentOrigin::CENTER );
      textFieldPopup.SetSize(250.0f, 400.0f);
      ImageActor popupBackground = ImageActor::New( NinePatchImage::New( TEXTFIELD_CONTAINER_BACKGROUND_IMAGE ) );
      popupBackground.SetZ( -0.1f);
      textFieldPopup.HiddenSignal().Connect( this, &TextFieldExample::OnPopupHideFinished );
      //textFieldPopup.OutsideTouchedSignal().Connect( this, &TextFieldExample::OnOutsidePopupTouched );
      textFieldPopup.SetBackgroundImage( popupBackground );
    }
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
      mMaxCharactersExceededMessage.SetProperty( TextLabel::Property::TEXT,"Max Characters Exceeded" );
      mMaxCharactersExceededMessage.SetProperty( TextLabel::Property::TEXT_COLOR, Color::BLACK );
      mMaxCharactersExceededMessage.SetBackgroundColor( Color::WHITE );  // todo Does not show back ground
      mMaxCharactersExceededMessage.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
      mMaxCharactersExceededMessage.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
      mMaxCharactersExceededMessage.SetProperty( TextLabel::Property::MULTI_LINE, true );
      mMaxCharactersExceededMessage.SetProperty( TextLabel::Property::POINT_SIZE, MAX_CHARACTERS_EXCEEDED_FONT_SIZE );
      mMaxCharactersExceededMessage.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
      mMaxCharactersExceededMessage.SetSize( Size( 100.0f, 80.0f ) );
      mView.Add ( mMaxCharactersExceededMessage );
    }
  }

  void SetUpTextField( TextField& field, Size stageSize, Actor& parent, std::string& text )
  {
    if ( !field )
    {
      field = TextField::New();
      field.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      field.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
      field.SetSize( TEXT_FIELD_SIZE );
      field.SetParentOrigin( ParentOrigin::TOP_CENTER );
      field.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      field.SetPosition(0.0f, TEXT_OFFSET_FROM_TOP );

      if ( text != "")
      {
        field.SetProperty( TextField::Property::TEXT, text );
      }
      else
      {
        field.SetProperty( TextField::Property::PLACEHOLDER_TEXT_FOCUSED, "Enter folder name." );
        field.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "Unnamed" );
      }

      field.SetProperty( TextField::Property::DECORATION_BOUNDING_BOX, Rect<int>( BORDER_WIDTH, BORDER_WIDTH, stageSize.width - BORDER_WIDTH*2, stageSize.height - BORDER_WIDTH*2 ) );
      field.SetProperty( TextField::Property::MAX_LENGTH, MAX_FOLDER_LENGTH );
      field.SetProperty( TextLabel::Property::TEXT_COLOR, Color::WHITE ); // Bug
      field.KeyInputFocusLostSignal().Connect( this, &TextFieldExample::TextFieldEditingEnded );
      field.MaxLengthReachedSignal().Connect( this, &TextFieldExample::TextFieldMaxCharactersReached );

      parent.Add( field );
    }
  }

  void EnlargeAnimation( Control& control, Animation& animation, Vector3 destinationPosition = Vector3::ZERO )
  {
    Vector3 fullSize(1.0f, 1.0f, 1.0f);

    animation.AnimateTo(Property(control, Actor::Property::SCALE ), fullSize, AlphaFunction::EASE_IN_OUT );
    animation.AnimateTo(Property(control, Actor::Property::POSITION ), destinationPosition, AlphaFunction::EASE_OUT );
    animation.FinishedSignal().Connect( this, &TextFieldExample::OnEnlargeAnimationFinished );
    animation.Play();
  }

  bool OnActivateButtonClicked( Button button )
  {
    CreateNewItemAndAddToGrid( button );

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

  void CreateNewItemAndAddToGrid( Button& buttonToHide )
  {
    if ( mItems.size() < NUMBER_OF_ITEMS )
    {
      buttonToHide.SetOpacity(0.0f);
      TextLabel newLabel = TextLabel::New();
      ImageActor labelContainer = ImageActor::New( NinePatchImage::New( FOLDER_BG_ICON_IMAGE ) );

      labelContainer.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
      labelContainer.SetSize( MAX_ITEM_SIZE );
      labelContainer.Add( newLabel );

      ItemData newItem;
      newItem.itemName = ""; // Initially set the name to empty so placeholder text used
      newItem.textLabel = newLabel;
      newItem.container = labelContainer;
      newLabel.SetBackgroundColor(Vector4( 0.0f, .0f, 0.0f, 1.0f));
      newLabel.SetProperty( TextLabel::Property::TEXT, newItem.itemName );
      newLabel.SetProperty( TextLabel::Property::TEXT_COLOR, Color::WHITE );
      newLabel.SetProperty( TextLabel::Property::MULTI_LINE, true );
      newLabel.SetProperty( TextLabel::Property::POINT_SIZE, LABEL_FONT_SIZE );
      newLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
      newLabel.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
      newLabel.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
      newLabel.SetZ(1.0f);

      mGrid.Add( labelContainer );
      mTapDetector.Attach( labelContainer );


      // Push new item into end of vector and start edit mode on that item
      mItems.push_back( newItem );
      EditItemInGrid( mItems.end()-1 );
    }
  }

  void EditItemInGrid( std::vector<TextFieldExample::ItemData>::iterator item)
  {
    // Create layer for dimmed background image and to parent popup
    SetUpLayer( mLayer, mView );
    mLayer.RaiseToTop(); // Can be done on demand but this demo has nothing else to raise above
    // touch area to close folder when touched
    SetUpOutsideTouchArea( mOutsideArea, mLayer );

    // container to hold text field
    SetUpTextFieldPopup( mTextFieldPopup );

    TextField textField;
    SetUpTextField( textField, mStageSize, mTextFieldPopup, (*item).itemName );

    ImageActor fieldUnderline;
    SetUpTextFieldUnderline( fieldUnderline, textField );

    mActiveItem = item;
    mLayer.Add( mTextFieldPopup );
    mTextFieldPopup.SetState( Popup::POPUP_SHOW, 0.35f );
    mTapDetector.Attach(mOutsideArea);
    mTapDetector.Attach(mTextFieldPopup);
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


    // Create toolbar & view
    Toolkit::ToolBar toolBar;
    mViewLayer = DemoHelper::CreateView( mApplication,
                                         mView,
                                         toolBar,
                                         BACKGROUND_IMAGE,
                                         TOOLBAR_IMAGE,
                                         APPLICATION_TITLE,
                                         VIEW_STYLE);


    // button to show text field
    SetUpActivationButton( mActivationButton, mView );

    SetUpGrid( mView );
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
      else if( event.IsCtrlModifier() )
      {
        switch( event.keyCode )
        {
          // Select rendering back-end
          case KEY_ZERO: // fall through
          case KEY_ONE:
          {
            //mTextField.SetProperty( TextField::Property::RENDERING_BACKEND, event.keyCode - 10 );
            break;
          }
          case KEY_H: // Horizontal alignment
          {
            if( ++mAlignment >= H_ALIGNMENT_STRING_COUNT )
            {
              mAlignment = 0u;
            }

            //mTextField.SetProperty( TextField::Property::HORIZONTAL_ALIGNMENT, H_ALIGNMENT_STRING_TABLE[ mAlignment ] );
            break;
          }
          case KEY_V: // Vertical alignment
          {
            if( ++mAlignment >= V_ALIGNMENT_STRING_COUNT )
            {
              mAlignment = 0u;
            }

            //mTextField.SetProperty( TextField::Property::VERTICAL_ALIGNMENT, V_ALIGNMENT_STRING_TABLE[ mAlignment ] );
            break;
          }
          case KEY_L: // Language
          {
            //const Language& language = LANGUAGES[ mLanguageId ];

            //mTextField.SetProperty( TextField::Property::TEXT, language.text );

            if( ++mLanguageId >= NUMBER_OF_LANGUAGES )
            {
              mLanguageId = 0u;
            }
            break;
          }
          case KEY_S: // Shadow color
          {
//            if( Color::BLACK == mTextField.GetProperty<Vector4>( TextField::Property::SHADOW_COLOR ) )
//            {
//              //mTextField.SetProperty( TextField::Property::SHADOW_COLOR, Color::RED );
//            }
//            else
//            {
//              //mTextField.SetProperty( TextField::Property::SHADOW_COLOR, Color::BLACK );
//            }
            break;
          }
          case KEY_PLUS: // Increase shadow offset
          {
            //mTextField.SetProperty( TextField::Property::SHADOW_OFFSET, mTextField.GetProperty<Vector2>( TextField::Property::SHADOW_OFFSET ) + Vector2( 1.0f, 1.0f ) );
            break;
          }
          case KEY_MINUS: // Decrease shadow offset
          {
            //mTextField.SetProperty( TextField::Property::SHADOW_OFFSET, mTextField.GetProperty<Vector2>( TextField::Property::SHADOW_OFFSET ) - Vector2( 1.0f, 1.0f ) );
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
  Size mStageSize;
  Layer mViewLayer;
  Toolkit::Control mView;
  Popup mTextFieldPopup;
  Layer mLayer;
  TableView mGrid;
  ImageActor mOutsideArea;
  PushButton mActivationButton;
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
