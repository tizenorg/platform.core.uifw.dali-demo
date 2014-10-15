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

#include "../shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;

// Define this so that it is interchangeable
// "DP" stands for Device independent Pixels
#define DP(x) DemoHelper::ScalePointSize(x)

struct ButtonItem
{
  const char* name;
  const char* text;
};


namespace
{

const char* const BACKGROUND_IMAGE = DALI_IMAGE_DIR "background-gradient.jpg";
const char* const TOOLBAR_IMAGE = DALI_IMAGE_DIR "top-bar.png";

const char* const TOOLBAR_TITLE = "Size Negotiation";
const int TOOLBAR_HEIGHT = 62;

const char* const PUSHBUTTON_BUTTON_IMAGE = DALI_IMAGE_DIR "button-up.9.png";
const char* const PUSHBUTTON_PRESS_IMAGE = DALI_IMAGE_DIR "button-down.9.png";

const char* const POPUP_BUTTON_ID = "POPUP_BUTTON";
const char* const OKAY_BUTTON_ID = "OKAY_BUTTON";
const char* const CANCEL_BUTTON_ID = "CANCEL_BUTTON";

const int POPUP_HEIGHT = 600;

const char* const CONTENT_TEXT = "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
const char* const IMAGE1 = DALI_IMAGE_DIR "gallery-medium-5.jpg";
const char* const IMAGE2 = DALI_IMAGE_DIR "gallery-medium-12.jpg";


const ButtonItem BUTTON_ITEMS[] = {
    { POPUP_BUTTON_ID, "Popup!" }
};

const int BUTTON_ITEMS_COUNT = sizeof( BUTTON_ITEMS ) / sizeof( BUTTON_ITEMS[0] );

}  // namespace



/**
 * This example shows the usage of size negotiation.
 */
class SizeNegotiationController: public ConnectionTracker, public Toolkit::ItemFactory
{
public:

  SizeNegotiationController( Application& application )
    : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &SizeNegotiationController::Create );
  }

  ~SizeNegotiationController()
  {
    // Nothing to do here
  }

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime

    Stage stage = Stage::GetCurrent();

    // Respond to key events
    stage.KeyEventSignal().Connect(this, &SizeNegotiationController::OnKeyEvent);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            TOOLBAR_TITLE );

    mItemView = Toolkit::ItemView::New( *this );
    mItemView.SetParentOrigin( ParentOrigin::CENTER );
    mItemView.SetAnchorPoint( AnchorPoint::CENTER );
    mItemView.SetResizePolicy( Actor::FillToParent, Actor::FillToParent );

    // Use a grid layout for tests
    Toolkit::GridLayoutPtr gridLayout = Toolkit::GridLayout::New();
    gridLayout->SetNumberOfColumns( 2 );
    gridLayout->SetTopMargin( DP(TOOLBAR_HEIGHT) + DP(20.0f) );
    gridLayout->SetBottomMargin( DP(100.0f) );
    gridLayout->SetRowSpacing( DP(45.0f) );
    mItemView.AddLayout( *gridLayout );

    Vector2 stageSize = Stage::GetCurrent().GetSize();
    float layoutWidth = Toolkit::IsHorizontal( gridLayout->GetOrientation() ) ? stageSize.height : stageSize.width;
    float gridItemSize = ( layoutWidth / gridLayout->GetNumberOfColumns() ) * 0.5f;
    gridLayout->SetScrollSpeedFactor( gridLayout->GetNumberOfColumns() / gridItemSize * 0.5f );

    mItemView.ActivateLayout( 0, Vector3(stageSize.x, stageSize.y, stageSize.x), 0.0f );

    mContentLayer.Add( mItemView );

    {
      // Popup
      const float POPUP_WIDTH_DP = stage.GetSize().width * 0.75f;

      mPopup = Toolkit::Popup::New();
      mPopup.SetName( "POPUP" );
      mPopup.SetParentOrigin( ParentOrigin::CENTER );
      mPopup.SetAnchorPoint( AnchorPoint::CENTER );
      mPopup.SetSize( POPUP_WIDTH_DP, DP( POPUP_HEIGHT ) );
      mPopup.HideTail();
      mPopup.SetTitle( "Popup!" );

      mPopup.OutsideTouchedSignal().Connect( this, &SizeNegotiationController::OnPopupOutsideTouched );

      mContentLayer.Add( mPopup );

      Toolkit::PushButton cancelButton = Toolkit::PushButton::New();
      cancelButton.SetName( CANCEL_BUTTON_ID );
      cancelButton.SetLabelText( "Cancel" );
      cancelButton.SetPressedImage( Dali::Image::New( PUSHBUTTON_PRESS_IMAGE ) );
      cancelButton.SetButtonImage( Dali::Image::New( PUSHBUTTON_BUTTON_IMAGE ) );

      cancelButton.ClickedSignal().Connect( this, &SizeNegotiationController::OnButtonClicked );

      mPopup.AddButton( cancelButton );

      Toolkit::PushButton okayButton = Toolkit::PushButton::New();
      okayButton.SetName( OKAY_BUTTON_ID );
      okayButton.SetLabelText( "OK!" );
      okayButton.SetPressedImage( Dali::Image::New( PUSHBUTTON_PRESS_IMAGE ) );
      okayButton.SetButtonImage( Dali::Image::New( PUSHBUTTON_BUTTON_IMAGE ) );

      okayButton.ClickedSignal().Connect( this, &SizeNegotiationController::OnButtonClicked );

      mPopup.AddButton( okayButton );

      // Popup content
      Toolkit::TableView contentTable = Toolkit::TableView::New( 3, 2 );
      contentTable.SetName( "CONTENT_TABLE" );
      contentTable.SetSize( POPUP_WIDTH_DP, DP( POPUP_HEIGHT * 0.75f ) );
      contentTable.SetFixedHeight( 1, DP(8.0f) );

      // Text
      Toolkit::TextView contentText = Toolkit::TextView::New( CONTENT_TEXT );
      contentText.SetName( "CONTENT_TEXT" );

      Dali::TextStyle textStyle;
      textStyle.SetFontPointSize( Dali::PointSize( DP( 5.0f ) ) );
      contentText.SetStyleToCurrentText( textStyle );

      contentText.SetSize( 0.0f, DP( POPUP_HEIGHT * 0.5f ) );
      contentText.SetMultilinePolicy( Toolkit::TextView::SplitByWord );
      contentText.SetWidthExceedPolicy( Toolkit::TextView::Split );
      contentText.SetHeightExceedPolicy( Toolkit::TextView::EllipsizeEnd );

      contentTable.AddChild( contentText, Toolkit::TableView::CellPosition( 0, 0, 1, 2 ) );

      // Image 1
      Image image1 = Image::New( IMAGE1 );
      ImageActor image1Actor = ImageActor::New( image1 );
      image1Actor.SetName( "IMAGE1_ACTOR" );
      contentTable.AddChild( image1Actor, Toolkit::TableView::CellPosition( 2, 0 ) );

      // Image 2
      Image image2 = Image::New( IMAGE2 );
      ImageActor image2Actor = ImageActor::New( image2 );
      image1Actor.SetName( "IMAGE2_ACTOR" );
      contentTable.AddChild( image2Actor, Toolkit::TableView::CellPosition( 2, 1 ) );

      mPopup.Add( contentTable );
    }
  }

  bool OnButtonClicked( Toolkit::Button button )
  {
    if( button.GetName() == POPUP_BUTTON_ID )
    {
      if( mPopup )
      {
        mPopup.Show();
      }
    }
    else if( button.GetName() == OKAY_BUTTON_ID || button.GetName() == CANCEL_BUTTON_ID )
    {
      if( mPopup )
      {
        mPopup.Hide();
      }
    }

    return true;
  }

  void OnPopupOutsideTouched()
  {
    if( mPopup )
    {
      mPopup.Hide();
    }
  }

  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        // Exit application when click back or escape.
        mApplication.Quit();
      }
    }
  }

public: // From ItemFactory

  /**
   * @brief Return the number of items to display in the item view
   *
   * @return Return the number of items to display
   */
  virtual unsigned int GetNumberOfItems()
  {
    return BUTTON_ITEMS_COUNT;
  }

  /**
   * @brief Create a new item to populate the item view with
   *
   * @param[in] itemId The index of the item to create
   * @return Return the created actor for the given ID
   */
  virtual Actor NewItem(unsigned int itemId)
  {
    Toolkit::PushButton popupButton = Toolkit::PushButton::New();
    popupButton.SetName( BUTTON_ITEMS[itemId].name );
    popupButton.SetLabelText( BUTTON_ITEMS[itemId].text );
    popupButton.SetResizePolicy( Actor::UseNaturalSize, Actor::UseNaturalSize );

    popupButton.SetPressedImage( Dali::Image::New( PUSHBUTTON_PRESS_IMAGE ) );
    popupButton.SetButtonImage( Dali::Image::New( PUSHBUTTON_BUTTON_IMAGE ) );

    popupButton.ClickedSignal().Connect( this, &SizeNegotiationController::OnButtonClicked );

    return popupButton;
  }

private:

  Application&      mApplication;
  Toolkit::View     mView;                   ///< The View instance.
  Toolkit::ToolBar  mToolBar;                ///< The View's Toolbar.
  Layer             mContentLayer;           ///< Content layer

  Toolkit::Popup    mPopup;

  Toolkit::ItemView mItemView;               ///< ItemView to hold test images
};

void RunTest( Application& application )
{
  SizeNegotiationController test( application );

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
