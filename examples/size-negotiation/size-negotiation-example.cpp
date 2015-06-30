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

#include "shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/popup/popup.h>

using namespace Dali;

using Dali::Toolkit::TextLabel;

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

const char* const TOOLBAR_TITLE = "Negotiate Size";
const int TOOLBAR_HEIGHT = 62;

const char* MENU_ICON_IMAGE = DALI_IMAGE_DIR "icon-cluster-none.png";
const char* MENU_ICON_IMAGE_SELECTED = DALI_IMAGE_DIR "icon-cluster-none-selected.png";

const char* const POPUPS_MENU_ID = "POPUPS_MENU";
const char* const TABLEVIEW_MENU_ID = "TABLEVIEW_MENU";

const char* const POPUP_BUTTON_EMPTY_ID = "POPUP_BUTTON_EMPTY";
const char* const POPUP_BUTTON_TITLE_ID = "POPUP_BUTTON_TITLE";
const char* const POPUP_BUTTON_BUTTONS_1_ID = "POPUP_BUTTON_BUTTONS_1";
const char* const POPUP_BUTTON_BUTTONS_2_ID = "POPUP_BUTTON_BUTTONS_2";
const char* const POPUP_BUTTON_TITLE_BUTTONS_ID = "POPUP_BUTTON_TITLE_BUTTONS";
const char* const POPUP_BUTTON_CONTENT_TEXT_ID = "POPUP_BUTTON_CONTENT_TEXT";
const char* const POPUP_BUTTON_CONTENT_IMAGE_ID = "POPUP_BUTTON_CONTENT_IMAGE";
const char* const POPUP_BUTTON_CONTENT_IMAGE_SCALE_ID = "POPUP_BUTTON_CONTENT_IMAGE_SCALE";
const char* const POPUP_BUTTON_CONTENT_IMAGE_FIT_ID = "POPUP_BUTTON_CONTENT_IMAGE_FIT";
const char* const POPUP_BUTTON_CONTENT_IMAGE_FILL_ID = "POPUP_BUTTON_CONTENT_IMAGE_FILL";
const char* const POPUP_BUTTON_TITLE_CONTENT_TEXT_ID = "POPUP_BUTTON_TITLE_CONTENT_TEXT";
const char* const POPUP_BUTTON_TITLE_CONTENT_TEXT_BUTTONS_ID = "POPUP_BUTTON_TITLE_CONTENT_TEXT_BUTTONS";
const char* const POPUP_BUTTON_COMPLEX_ID = "POPUP_BUTTON_COMPLEX";

const char* const TABLEVIEW_BUTTON_EMPTY_ID = "TABLEVIEW_BUTTON_EMPTY";
const char* const TABLEVIEW_BUTTON_1CELL_ID = "TABLEVIEW_BUTTON_1CELL";
const char* const TABLEVIEW_BUTTON_3CELL_ID = "TABLEVIEW_BUTTON_3CELL";
const char* const TABLEVIEW_BUTTON_3X3CELL_ID = "TABLEVIEW_BUTTON_3X3CELL";
const char* const TABLEVIEW_BUTTON_FIXED1_ID = "TABLEVIEW_BUTTON_FIXED1";
const char* const TABLEVIEW_BUTTON_FIXED2_ID = "TABLEVIEW_BUTTON_FIXED2";
const char* const TABLEVIEW_BUTTON_FIT1_ID = "TABLEVIEW_BUTTON_FIT1";
const char* const TABLEVIEW_BUTTON_FIT2_ID = "TABLEVIEW_BUTTON_FIT2";
const char* const TABLEVIEW_BUTTON_NATURAL1_ID = "TABLEVIEW_BUTTON_NATURAL1";
const char* const TABLEVIEW_BUTTON_NATURAL2_ID = "TABLEVIEW_BUTTON_NATURAL2";
const char* const TABLEVIEW_BUTTON_NATURAL3_ID = "TABLEVIEW_BUTTON_NATURAL3";

const char* const OKAY_BUTTON_ID = "OKAY_BUTTON";
const char* const CANCEL_BUTTON_ID = "CANCEL_BUTTON";

const char* const CONTENT_TEXT = "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
const char* const IMAGE1 = DALI_IMAGE_DIR "gallery-medium-5.jpg";
const char* const IMAGE2 = DALI_IMAGE_DIR "background-magnifier.jpg";
const char* const CHECKBOX_UNCHECKED_IMAGE = DALI_IMAGE_DIR "checkbox-unselected.png";
const char* const CHECKBOX_CHECKED_IMAGE = DALI_IMAGE_DIR "checkbox-selected.png";

const ButtonItem MENU_ITEMS[] = {
    { POPUPS_MENU_ID,        "Popups" },
    { TABLEVIEW_MENU_ID,     "TableView" }
};

const unsigned int MENU_ITEMS_COUNT = sizeof( MENU_ITEMS ) / sizeof( MENU_ITEMS[0] );

const ButtonItem POPUP_BUTTON_ITEMS[] = {
    { POPUP_BUTTON_COMPLEX_ID,                    "Complex" },
    { POPUP_BUTTON_EMPTY_ID,                      "Empty" },
    { POPUP_BUTTON_TITLE_ID,                      "Title" },
    { POPUP_BUTTON_BUTTONS_1_ID,                  "1 Button" },
    { POPUP_BUTTON_BUTTONS_2_ID,                  "2 Buttons" },
    { POPUP_BUTTON_TITLE_BUTTONS_ID,              "Title & Buttons" },
    { POPUP_BUTTON_CONTENT_TEXT_ID,               "Text" },
    { POPUP_BUTTON_CONTENT_IMAGE_ID,              "Image" },
    { POPUP_BUTTON_CONTENT_IMAGE_SCALE_ID,        "Image Scale" },
    { POPUP_BUTTON_CONTENT_IMAGE_FIT_ID,          "Image Fit" },
    { POPUP_BUTTON_CONTENT_IMAGE_FILL_ID,         "Image Fill" },
    { POPUP_BUTTON_TITLE_CONTENT_TEXT_ID,         "Title Text" },
    { POPUP_BUTTON_TITLE_CONTENT_TEXT_BUTTONS_ID, "Title, text, buttons" }

};

const int POPUP_BUTTON_ITEMS_COUNT = sizeof( POPUP_BUTTON_ITEMS ) / sizeof( POPUP_BUTTON_ITEMS[0] );

const ButtonItem TABLEVIEW_BUTTON_ITEMS[] = {
    { TABLEVIEW_BUTTON_EMPTY_ID,                    "Empty" },
    { TABLEVIEW_BUTTON_1CELL_ID,                    "1 Cell" },
    { TABLEVIEW_BUTTON_3CELL_ID,                    "3 Cell" },
    { TABLEVIEW_BUTTON_3X3CELL_ID,                  "3x3 Cells" },
    { TABLEVIEW_BUTTON_FIXED1_ID,                   "Fixed 1" },
    { TABLEVIEW_BUTTON_FIXED2_ID,                   "Fixed 2" },
    { TABLEVIEW_BUTTON_FIT1_ID,                     "Fit Top Bottom" },
    { TABLEVIEW_BUTTON_FIT2_ID,                     "Fit Middle" },
    { TABLEVIEW_BUTTON_NATURAL1_ID,                 "Natural 1" },
    { TABLEVIEW_BUTTON_NATURAL2_ID,                 "Natural 2" },
    { TABLEVIEW_BUTTON_NATURAL3_ID,                 "Natural 3" },
};

const unsigned int TABLEVIEW_BUTTON_ITEMS_COUNT = sizeof( TABLEVIEW_BUTTON_ITEMS ) / sizeof( TABLEVIEW_BUTTON_ITEMS[0] );

}  // namespace



/**
 * This example shows the usage of size negotiation.
 */
class SizeNegotiationController: public ConnectionTracker, public Toolkit::ItemFactory
{
public:

  SizeNegotiationController( Application& application )
    : mApplication( application ),
      mMenuShown( false ),
      mDemoState( POPUP )
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
                                            std::string("") );

    mTitleActor = DemoHelper::CreateToolBarLabel( "CUSTOM_TOOLBAR_TITLE" );

    SetTitle();

    // Create menu button
    Toolkit::PushButton viewButton = Toolkit::PushButton::New();
    viewButton.SetButtonImage( ResourceImage::New( MENU_ICON_IMAGE ) );
    viewButton.SetSelectedImage( ResourceImage::New( MENU_ICON_IMAGE_SELECTED ) );
    viewButton.ClickedSignal().Connect( this, &SizeNegotiationController::OnMenu );
    mToolBar.AddControl( viewButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalLeft, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    // Add title to the tool bar.
    const float padding( DemoHelper::DEFAULT_VIEW_STYLE.mToolBarPadding );
    mToolBar.AddControl( mTitleActor, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::Padding( padding, padding, padding, padding ) );

    mItemView = Toolkit::ItemView::New( *this );
    mItemView.SetParentOrigin( ParentOrigin::CENTER );
    mItemView.SetAnchorPoint( AnchorPoint::CENTER );
    mItemView.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

    // Use a grid layout for tests
    Vector2 stageSize = stage.GetSize();
    Toolkit::ItemLayoutPtr gridLayout = Toolkit::DefaultItemLayout::New( Toolkit::DefaultItemLayout::LIST );
    Vector3 itemSize;
    gridLayout->GetItemSize( 0, Vector3( stageSize ), itemSize );
    itemSize.height = stageSize.y / 10;
    gridLayout->SetItemSize( itemSize );
    mItemView.AddLayout( *gridLayout );

    mItemView.ActivateLayout( 0, Vector3(stageSize.x, stageSize.y, stageSize.x), 0.0f );

    mContentLayer.Add( mItemView );
  }

  void SetTitle()
  {
    std::string subTitle = "";

    switch( mDemoState )
    {
      case POPUP:
      {
        subTitle = "Popups";
        break;
      }

      case TABLEVIEW:
      {
        subTitle = "TableView";
        break;
      }

      default:
      {
        break;
      }
    }

    mTitleActor.SetProperty( Toolkit::TextLabel::Property::TEXT, std::string( std::string( TOOLBAR_TITLE ) + ": " + subTitle ) );
  }

  bool OnMenu( Toolkit::Button button )
  {
    ShowMenu();
    return true;
  }

  void ShowMenu()
  {
    Stage stage = Stage::GetCurrent();
    const float popupWidth = stage.GetSize().x * 0.5f;

    mMenu = Toolkit::Popup::New();
    mMenu.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mMenu.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mMenu.HideTail();
    mMenu.OutsideTouchedSignal().Connect( this, &SizeNegotiationController::HideMenu );
    mMenu.SetSize( popupWidth, 0.0f );
    mMenu.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );

    Toolkit::TableView tableView = Toolkit::TableView::New( 0, 0 );
    tableView.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    tableView.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    mMenu.Add( tableView );

    for( unsigned int i = 0; i < MENU_ITEMS_COUNT; ++i )
    {
      Toolkit::PushButton menuButton = Toolkit::PushButton::New();
      menuButton.SetName( MENU_ITEMS[ i ].name );
      menuButton.SetLabel( MENU_ITEMS[ i ].text );
      menuButton.SetButtonImage( Actor() );
      menuButton.SetSelectedImage( Actor() );
      menuButton.ClickedSignal().Connect( this, &SizeNegotiationController::OnMenuSelect );

      tableView.Add( menuButton );
      tableView.SetFitHeight( i );
    }

    // Show the menu
    mMenu.Show();
    mMenuShown = true;
  }

  void HideMenu()
  {
    if( mMenu )
    {
      mMenu.Hide();
      mMenu.Reset();
    }

    mMenuShown = false;
  }

  bool OnMenuSelect( Toolkit::Button button )
  {
    bool refresh = false;

    if( button.GetName() == POPUPS_MENU_ID )
    {
      if( mDemoState != POPUP )
      {
        refresh = true;
        mDemoState = POPUP;
      }
    }
    else if( button.GetName() == TABLEVIEW_MENU_ID )
    {
      if( mDemoState != TABLEVIEW )
      {
        refresh = true;
        mDemoState = TABLEVIEW;
      }
    }

    if( refresh )
    {
      SetTitle();

      mItemView.Refresh();
    }

    HideMenu();
    return true;
  }

  Toolkit::Popup CreatePopup()
  {
    Stage stage = Stage::GetCurrent();
    const float POPUP_WIDTH_DP = stage.GetSize().width * 0.75f;

    Toolkit::Popup popup = Toolkit::Popup::New();
    popup.SetName( "POPUP" );
    popup.SetParentOrigin( ParentOrigin::CENTER );
    popup.SetAnchorPoint( AnchorPoint::CENTER );
    popup.SetSize( POPUP_WIDTH_DP, 0.0f );
    popup.HideTail();

    popup.OutsideTouchedSignal().Connect( this, &SizeNegotiationController::OnPopupOutsideTouched );

    return popup;
  }

  bool OnButtonClicked( Toolkit::Button button )
  {
    if( button.GetName() == POPUP_BUTTON_EMPTY_ID )
    {
      mPopup = CreatePopup();

      mPopup.Show();
    }
    else if( button.GetName() == POPUP_BUTTON_TITLE_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetTitle( "Popup!" );

      mPopup.Show();
    }
    else if( button.GetName() == POPUP_BUTTON_BUTTONS_1_ID )
    {
      mPopup = CreatePopup();

      Toolkit::PushButton okayButton = Toolkit::PushButton::New();
      okayButton.SetName( OKAY_BUTTON_ID );
      okayButton.SetLabel( "OK!" );

      okayButton.ClickedSignal().Connect( this, &SizeNegotiationController::OnButtonClicked );

      mPopup.AddButton( okayButton );

      mPopup.Show();
    }
    else if( button.GetName() == POPUP_BUTTON_BUTTONS_2_ID )
    {
      mPopup = CreatePopup();

      Toolkit::PushButton cancelButton = Toolkit::PushButton::New();
      cancelButton.SetName( CANCEL_BUTTON_ID );
      cancelButton.SetLabel( "Cancel" );

      cancelButton.ClickedSignal().Connect( this, &SizeNegotiationController::OnButtonClicked );

      mPopup.AddButton( cancelButton );

      Toolkit::PushButton okayButton = Toolkit::PushButton::New();
      okayButton.SetName( OKAY_BUTTON_ID );
      okayButton.SetLabel( "OK!" );

      okayButton.ClickedSignal().Connect( this, &SizeNegotiationController::OnButtonClicked );

      mPopup.AddButton( okayButton );

      mPopup.Show();
    }
    else if( button.GetName() == POPUP_BUTTON_TITLE_BUTTONS_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetTitle( "Popup!" );

      Toolkit::PushButton cancelButton = Toolkit::PushButton::New();
      cancelButton.SetName( CANCEL_BUTTON_ID );
      cancelButton.SetLabel( "Cancel" );

      cancelButton.ClickedSignal().Connect( this, &SizeNegotiationController::OnButtonClicked );

      mPopup.AddButton( cancelButton );

      Toolkit::PushButton okayButton = Toolkit::PushButton::New();
      okayButton.SetName( OKAY_BUTTON_ID );
      okayButton.SetLabel( "OK!" );

      okayButton.ClickedSignal().Connect( this, &SizeNegotiationController::OnButtonClicked );

      mPopup.AddButton( okayButton );

      mPopup.Show();
    }
    else if( button.GetName() == POPUP_BUTTON_CONTENT_TEXT_ID )
    {
      mPopup = CreatePopup();

      TextLabel text = TextLabel::New( CONTENT_TEXT );
      text.SetName( "POPUP_CONTENT_TEXT" );
      text.SetProperty( TextLabel::Property::MULTI_LINE, true );
      text.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
      text.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
      text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      text.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
      text.SetPadding( Padding( 20.0f, 20.0f, 20.0f, 20.0f ) );

      mPopup.Add( text );

      mPopup.Show();
    }
    else if( button.GetName() == POPUP_BUTTON_CONTENT_IMAGE_ID )
    {
      mPopup = CreatePopup();

      ImageActor image = ImageActor::New( ResourceImage::New( IMAGE2 ) );
      image.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      image.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
      image.SetPadding( Padding( 20.0f, 20.0f, 20.0f, 20.0f ) );

      mPopup.Add( image );

      mPopup.Show();
    }
    else if( button.GetName() == POPUP_BUTTON_CONTENT_IMAGE_SCALE_ID )
    {
      mPopup = CreatePopup();

      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );

      ImageActor image = ImageActor::New( ResourceImage::New( IMAGE2 ) );
      image.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

      mPopup.Add( image );

      mPopup.Show();
    }
    else if( button.GetName() == POPUP_BUTTON_CONTENT_IMAGE_FIT_ID )
    {
      mPopup = CreatePopup();

      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );

      ImageActor image = ImageActor::New( ResourceImage::New( IMAGE2 ) );
      image.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      image.SetSizeScalePolicy( SizeScalePolicy::FIT_WITH_ASPECT_RATIO );

      mPopup.Add( image );

      mPopup.Show();
    }
    else if( button.GetName() == POPUP_BUTTON_CONTENT_IMAGE_FILL_ID )
    {
      mPopup = CreatePopup();

      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );

      ImageActor image = ImageActor::New( ResourceImage::New( IMAGE2 ) );
      image.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      image.SetSizeScalePolicy( SizeScalePolicy::FILL_WITH_ASPECT_RATIO );

      mPopup.Add( image );

      mPopup.Show();
    }
    else if( button.GetName() == POPUP_BUTTON_TITLE_CONTENT_TEXT_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetTitle( "Popup!" );

      Toolkit::TextLabel text = Toolkit::TextLabel::New( CONTENT_TEXT );
      text.SetName( "POPUP_CONTENT_TEXT" );
      text.SetProperty( TextLabel::Property::MULTI_LINE, true );
      text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      text.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
      text.SetPadding( Padding( 20.0f, 20.0f, 20.0f, 20.0f ) );

      mPopup.Add( text );

      mPopup.Show();
    }
    else if( button.GetName() == POPUP_BUTTON_TITLE_CONTENT_TEXT_BUTTONS_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetTitle( "Popup!" );

      Toolkit::TextLabel text = Toolkit::TextLabel::New( CONTENT_TEXT );
      text.SetName( "POPUP_CONTENT_TEXT" );
      text.SetProperty( TextLabel::Property::MULTI_LINE, true );
      text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      text.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
      text.SetPadding( Padding( 10.0f, 10.0f, 20.0f, 0.0f ) );

      mPopup.Add( text );

      Toolkit::PushButton cancelButton = Toolkit::PushButton::New();
      cancelButton.SetName( CANCEL_BUTTON_ID );
      cancelButton.SetLabel( "Cancel" );

      cancelButton.ClickedSignal().Connect( this, &SizeNegotiationController::OnButtonClicked );

      mPopup.AddButton( cancelButton );

      Toolkit::PushButton okayButton = Toolkit::PushButton::New();
      okayButton.SetName( OKAY_BUTTON_ID );
      okayButton.SetLabel( "OK!" );

      okayButton.ClickedSignal().Connect( this, &SizeNegotiationController::OnButtonClicked );

      mPopup.AddButton( okayButton );

      mPopup.Show();
    }
    else if( button.GetName() == POPUP_BUTTON_COMPLEX_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetTitle( "Warning" );

      // Content
      Toolkit::TableView content = Toolkit::TableView::New( 2, 2 );
      content.SetName( "COMPLEX_TABLEVIEW" );
      content.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      content.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      content.SetFitHeight( 0 );
      content.SetFitHeight( 1 );
      content.SetPadding( Padding( 20.0f, 20.0f, 20.0f, 0.0f ) );

      // Text
      {
        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Do you really want to quit?" );
        text.SetProperty( Toolkit::TextLabel::Property::MULTI_LINE, true );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        text.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );

        content.AddChild( text, Toolkit::TableView::CellPosition( 0, 0 ) );
      }

      // Image
      {
        ImageActor image = ImageActor::New( ResourceImage::New( IMAGE1 ) );
        image.SetName( "COMPLEX_IMAGE" );
        image.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        image.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
        image.SetPadding( Padding( 20.0f, 0.0f, 0.0f, 0.0f ) );
        content.AddChild( image, Toolkit::TableView::CellPosition( 0, 1 ) );
      }

      // Text 2
      {
        Toolkit::TableView root = Toolkit::TableView::New( 1, 2 );
        root.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        root.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
        root.SetFitHeight( 0 );
        root.SetFitWidth( 0 );
        root.SetPadding( Padding( 0.0f, 0.0f, 0.0f, 20.0f ) );

        Toolkit::CheckBoxButton checkBox = Toolkit::CheckBoxButton::New();
        checkBox.SetSize( 48, 48 );

        root.AddChild( checkBox, Toolkit::TableView::CellPosition( 0, 0 ) );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Don't show again" );
        Actor textActor = text;
        textActor.SetPadding( Padding( 20.0f, 0.0f, 0.0f, 10.0f ) );

        root.AddChild( text, Toolkit::TableView::CellPosition( 0, 1 ) );

        content.AddChild( root, Toolkit::TableView::CellPosition( 1, 0 ) );
      }

      mPopup.Add( content );

      // Buttons
      Toolkit::PushButton cancelButton = Toolkit::PushButton::New();
      cancelButton.SetName( CANCEL_BUTTON_ID );
      cancelButton.SetLabel( "Cancel" );

      cancelButton.ClickedSignal().Connect( this, &SizeNegotiationController::OnButtonClicked );

      mPopup.AddButton( cancelButton );

      Toolkit::PushButton okayButton = Toolkit::PushButton::New();
      okayButton.SetName( OKAY_BUTTON_ID );
      okayButton.SetLabel( "OK!" );

      okayButton.ClickedSignal().Connect( this, &SizeNegotiationController::OnButtonClicked );

      mPopup.AddButton( okayButton );

      mPopup.Show();
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_EMPTY_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );


      Toolkit::TableView table = Toolkit::TableView::New( 0, 0 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

      mPopup.Add( table );

      mPopup.Show();
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_1CELL_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );


      Toolkit::TableView table = Toolkit::TableView::New( 0, 0 );
      table.SetName( "TABLEVIEW_BUTTON_1CELL_ID" );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

      Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
      backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      table.Add( backing );

      mPopup.Add( table );

      mPopup.Show();
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_3CELL_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );


      Toolkit::TableView table = Toolkit::TableView::New( 0, 0 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.Add( backing );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.Add( backing );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.Add( backing );
      }

      mPopup.Add( table );

      mPopup.Show();
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_3X3CELL_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );


      Toolkit::TableView table = Toolkit::TableView::New( 3, 3 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

      // Column 0
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 0, 0 ) );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 1, 0 ) );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 2, 0 ) );
      }

      // Column 1
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 0.0f, 1.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 0, 1 ) );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 1, 1 ) );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 0.0f, 0.0f, 1.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 2, 1 ) );
      }

      // Column 2
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 0, 2 ) );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 0.5f, 0.5f, 0.5f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 1, 2 ) );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 0.5f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 2, 2 ) );
      }

      mPopup.Add( table );

      mPopup.Show();
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_FIXED1_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );


      Toolkit::TableView table = Toolkit::TableView::New( 3, 1 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      table.SetFixedHeight( 0, 50.0f );

      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fixed" );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
        backing.Add( text );
        table.Add( backing );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fill" );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
        backing.Add( text );
        table.Add( backing );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fill" );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
        backing.Add( text );
        table.Add( backing );
      }

      mPopup.Add( table );

      mPopup.Show();
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_FIXED2_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );


      Toolkit::TableView table = Toolkit::TableView::New( 3, 1 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      table.SetFixedHeight( 0, 50.0f );
      table.SetFixedHeight( 2, 50.0f );

      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fixed" );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );
        table.Add( backing );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fill" );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );
        table.Add( backing );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fixed" );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );
        table.Add( backing );
      }

      mPopup.Add( table );

      mPopup.Show();
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_FIT1_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );


      Toolkit::TableView table = Toolkit::TableView::New( 3, 1 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      table.SetFitHeight( 0 );
      table.SetFitHeight( 2 );

      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        backing.SetSize( 0.0f, 100.0f );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fit" );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fill" );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        backing.SetSize( 0.0f, 100.0f );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fit" );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }

      mPopup.Add( table );

      mPopup.Show();
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_FIT2_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );

      Toolkit::TableView table = Toolkit::TableView::New( 3, 1 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      table.SetFitHeight( 1 );

      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fill" );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        backing.SetSize( 0.0f, 200.0f );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fit" );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        backing.Add( text );

        table.Add( backing );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fill" );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }

      mPopup.Add( table );

      mPopup.Show();
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_NATURAL1_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::WIDTH );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 1.0f, 1.0f ) );
      mPopup.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );

      Toolkit::TableView table = Toolkit::TableView::New( 3, 1 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      table.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      table.SetFitHeight( 0 );
      table.SetFitHeight( 1 );
      table.SetFitHeight( 2 );

      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        backing.SetSize( 0.0f, 100.0f );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fit" );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        backing.SetSize( 0.0f, 200.0f );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fit" );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        backing.SetSize( 0.0f, 300.0f );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fit" );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }

      mPopup.Add( table );

      mPopup.Show();
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_NATURAL2_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::WIDTH );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 1.0f, 1.0f ) );
      mPopup.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );

      Toolkit::TableView table = Toolkit::TableView::New( 3, 1 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      table.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      table.SetFitHeight( 0 );
      table.SetFitHeight( 1 );

      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        backing.SetResizePolicy( ResizePolicy::FIXED, Dimension::HEIGHT );
        backing.SetSize( 0.0f, 100.0f );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fit" );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        backing.SetSize( 0.0f, 200.0f );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fit" );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }

      mPopup.Add( table );

      mPopup.Show();
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_NATURAL3_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::WIDTH );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 1.0f, 1.0f ) );
      mPopup.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );

      Toolkit::TableView table = Toolkit::TableView::New( 3, 1 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      table.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      table.SetFixedHeight( 0, 20.0f );
      table.SetFitHeight( 1 );

      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fixed" );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }
      {
        Actor backing = Toolkit::CreateSolidColorActor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        backing.SetSize( 0.0f, 200.0f );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fit" );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }

      mPopup.Add( table );

      mPopup.Show();
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
    switch( mDemoState )
    {
      case POPUP:
      {
        return POPUP_BUTTON_ITEMS_COUNT;
      }

      case TABLEVIEW:
      {
        return TABLEVIEW_BUTTON_ITEMS_COUNT;
      }

      default:
      {
        break;
      }
    }

    return 0;
  }

  /**
   * @brief Create a new item to populate the item view with
   *
   * @param[in] itemId The index of the item to create
   * @return Return the created actor for the given ID
   */
  virtual Actor NewItem(unsigned int itemId)
  {
    const ButtonItem* buttonDataArray = NULL;
    switch( mDemoState )
    {
      case POPUP:
      {
        buttonDataArray = POPUP_BUTTON_ITEMS;
        break;
      }

      case TABLEVIEW:
      {
        buttonDataArray = TABLEVIEW_BUTTON_ITEMS;
        break;
      }

      default:
      {
        break;
      }
    }

    if( buttonDataArray )
    {
      Toolkit::PushButton popupButton = Toolkit::PushButton::New();
      popupButton.SetName( buttonDataArray[ itemId ].name );
      popupButton.SetLabel( buttonDataArray[ itemId ].text );
      popupButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

      popupButton.ClickedSignal().Connect( this, &SizeNegotiationController::OnButtonClicked );

      return popupButton;
    }

    return Actor();
  }

private:

  enum DemoState
  {
    POPUP,
    TABLEVIEW
  };

  Application&      mApplication;
  Toolkit::Control  mView;                   ///< The View instance.
  Toolkit::ToolBar  mToolBar;                ///< The View's Toolbar.
  Layer             mContentLayer;           ///< Content layer

  Toolkit::TextLabel mTitleActor;             ///< Title text

  Toolkit::Popup    mMenu;                   ///< The navigation menu
  bool              mMenuShown;              ///< If the navigation menu is currently being displayed or not

  Toolkit::Popup    mPopup;

  Toolkit::ItemView mItemView;               ///< ItemView to hold test images

  DemoState mDemoState;
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
  Application application = Application::New( &argc, &argv, DALI_DEMO_THEME_PATH );

  RunTest( application );

  return 0;
}
