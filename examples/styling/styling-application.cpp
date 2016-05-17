/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
 */

/**
 * @file style-example.cpp
 * @brief Example of styling Toolkit controls.
 */

// Class include
#include "styling-application.h"

// External includes
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/slider/slider.h>
#include <dali-toolkit/devel-api/controls/popup/popup.h>
#include "image-channel-control.h"
#include <cstdio>
#include <sstream>

// Internal includes

using namespace Dali;
using namespace Dali::Toolkit;

namespace Demo
{

const char* StylingApplication::DEMO_THEME_ONE_PATH( DEMO_STYLE_DIR "style-example-theme-one.json" );
const char* StylingApplication::DEMO_THEME_TWO_PATH( DEMO_STYLE_DIR "style-example-theme-two.json" );
const char* StylingApplication::DEMO_THEME_THREE_PATH( DEMO_STYLE_DIR "style-example-theme-three.json" );

namespace
{
#define DP(x) x

const char* DEFAULT_CONTROL_AREA_IMAGE_PATH( DEMO_IMAGE_DIR "popup_button_background.9.png" );

const char* POPUP_CONTROL_OK_NAME( "PopupControlOk" );
const char* POPUP_CONTROL_CANCEL_NAME( "PopupControlCancel" );

const int NUMBER_OF_THEMES(3); // The default theme is considered.

const Vector4 BACKGROUND_COLOUR( 1.0f, 1.0f, 1.0f, 0.15f );

const char* const SMALL_IMAGE_1 = DEMO_IMAGE_DIR "gallery-small-14.jpg";
const char* const BIG_IMAGE_1   = DEMO_IMAGE_DIR "gallery-large-4.jpg";

const char* const SMALL_IMAGE_2 = DEMO_IMAGE_DIR "gallery-small-39.jpg";
const char* const BIG_IMAGE_2   = DEMO_IMAGE_DIR "gallery-large-7.jpg";

const char* const SMALL_IMAGE_3 = DEMO_IMAGE_DIR "gallery-small-20.jpg";
const char* const BIG_IMAGE_3   = DEMO_IMAGE_DIR "gallery-large-11.jpg";

// Layout
const int MARGIN_SIZE = 10;
const int TOP_MARGIN = 55;

const int RADIO_LABEL_THUMBNAIL_SIZE = 60;
const int RADIO_LABEL_THUMBNAIL_SIZE_SMALL = 40;
const int RADIO_IMAGE_SPACING = 8;
const int BUTTON_HEIGHT = 48;

Property::Index GetChannelProperty( int index )
{
  Property::Index channelIndex = Property::INVALID_INDEX;
  switch(index)
  {
    case 0: { channelIndex = ImageChannelControl::Property::RED_CHANNEL; break; }
    case 1: { channelIndex = ImageChannelControl::Property::GREEN_CHANNEL; break; }
    case 2: { channelIndex = ImageChannelControl::Property::BLUE_CHANNEL; break; }
  }
  return channelIndex;
}

} // anonymous namespace



StylingApplication::StylingApplication( Application& application )
: mApplication( application )
{
  application.InitSignal().Connect( this, &StylingApplication::Create );
}

StylingApplication::~StylingApplication()
{
}

void StylingApplication::Create( Application& application )
{
  Stage stage = Stage::GetCurrent();
  stage.KeyEventSignal().Connect(this, &StylingApplication::OnKeyEvent);
  stage.SetBackgroundColor( Vector4( 0.1f, 0.1f, 0.1f, 1.0f ) );

  mTitle = TextLabel::New( "Styling Example" );
  mTitle.SetName( "Title" );
  mTitle.SetStyleName("title");
  mTitle.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  mTitle.SetParentOrigin( ParentOrigin::TOP_CENTER );
  mTitle.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  mTitle.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
  mTitle.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  stage.Add( mTitle );

  // Content panes:
  TableView contentTable = TableView::New( 4, 1 );
  contentTable.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  contentTable.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
  contentTable.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  contentTable.SetParentOrigin( ParentOrigin::TOP_LEFT );
  contentTable.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE ) );

  // Assign all rows the size negotiation property of fitting to children
  for( unsigned int i = 0; i < contentTable.GetRows(); ++i )
  {
    contentTable.SetFitHeight(i);
  }
  contentTable.SetPosition( 0.0f, TOP_MARGIN );
  stage.Add( contentTable );

  // Buttons:

  TableView imageSelectLayout = TableView::New( 1, 2 );
  imageSelectLayout.SetName("ImageSelectLayout");

  imageSelectLayout.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  imageSelectLayout.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
  imageSelectLayout.SetAnchorPoint( AnchorPoint::CENTER );
  imageSelectLayout.SetParentOrigin( ParentOrigin::CENTER );
  imageSelectLayout.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE ) );

  imageSelectLayout.SetFitHeight( 0 );
  imageSelectLayout.SetFitWidth( 0 );

  contentTable.Add( imageSelectLayout );

  TableView radioButtonsLayout = TableView::New( 3, 2 );
  radioButtonsLayout.SetName("RadioButtonsLayout");
  radioButtonsLayout.SetFitHeight(0);
  radioButtonsLayout.SetFitHeight(1);
  radioButtonsLayout.SetFitHeight(2);
  radioButtonsLayout.SetFitWidth(0);
  radioButtonsLayout.SetFitWidth(1);
  radioButtonsLayout.SetCellPadding( Vector2( 6.0f, 0.0f ) );

  imageSelectLayout.AddChild( radioButtonsLayout, TableView::CellPosition(0, 0) );

  const char* images[] = { SMALL_IMAGE_1, SMALL_IMAGE_2, SMALL_IMAGE_3 };

  for( int i=0; i<3; ++i )
  {
    std::ostringstream thumbnailName; thumbnailName << "thumbnail" << i+1;
    ImageView image = ImageView::New( ResourceImage::New( images[i] ) );
    image.SetName( thumbnailName.str() );
    image.SetSize( DP(RADIO_LABEL_THUMBNAIL_SIZE), DP(RADIO_LABEL_THUMBNAIL_SIZE) );

    std::ostringstream label; label << (i+1);
    std::ostringstream radioButtonStyleName;
    radioButtonStyleName << "imageSelectButton" << i+1;
    mRadioButtons[i] = RadioButton::New( label.str() );
    mRadioButtons[i].SetName( radioButtonStyleName.str() );
    mRadioButtons[i].SetParentOrigin( ParentOrigin::TOP_LEFT );
    mRadioButtons[i].SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mRadioButtons[i].SetSelected( false );
    mRadioButtons[i].StateChangedSignal().Connect( this, &StylingApplication::OnButtonStateChange );

    radioButtonsLayout.AddChild( mRadioButtons[i], TableView::CellPosition( i, 0 ) );
    radioButtonsLayout.AddChild( image, TableView::CellPosition( i, 1 ) );
  }
  mRadioButtons[0].SetSelected( true );

  mImageChannelControl = ImageChannelControl::New( BIG_IMAGE_1 );
  mImageChannelControl.SetName("ImageChannelControl");
  mImageChannelControl.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  mImageChannelControl.SetSizeScalePolicy( SizeScalePolicy::FIT_WITH_ASPECT_RATIO );
  imageSelectLayout.AddChild( mImageChannelControl, TableView::CellPosition( 0, 1 ) );
  imageSelectLayout.SetCellAlignment( TableView::CellPosition( 0, 1 ), HorizontalAlignment::RIGHT, VerticalAlignment::CENTER );


  TableView channelSliderLayout = TableView::New( 3, 3 );
  channelSliderLayout.SetName("ChannelSliderLayout");

  // Contains a column of check buttons and a column of sliders for R/G/B
  channelSliderLayout.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  channelSliderLayout.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
  channelSliderLayout.SetAnchorPoint( AnchorPoint::CENTER );
  channelSliderLayout.SetParentOrigin( ParentOrigin::CENTER );
  channelSliderLayout.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE ) );

  channelSliderLayout.SetFitHeight( 0 );
  channelSliderLayout.SetFitHeight( 1 );
  channelSliderLayout.SetFitHeight( 2 );
  channelSliderLayout.SetFitWidth( 0 );
  channelSliderLayout.SetFitWidth( 1 );

  contentTable.Add( channelSliderLayout );
  const char *checkboxLabels[3] = {"R", "G", "B"};

  for( int i=0; i<3; ++i )
  {
    std::ostringstream checkBoxStyleName;
    checkBoxStyleName << "channelActiveCheckBox" << i+1;
    mCheckButtons[i] = CheckBoxButton::New();
    mCheckButtons[i].SetName( checkBoxStyleName.str() );
    mCheckButtons[i].SetParentOrigin( ParentOrigin::CENTER );
    mCheckButtons[i].SetAnchorPoint( AnchorPoint::CENTER );
    mCheckButtons[i].SetSelected( true );
    mCheckButtons[i].StateChangedSignal().Connect( this, &StylingApplication::OnCheckButtonChange );
    mCheckButtons[i].RegisterProperty( "channel", i, Property::READ_WRITE );

    channelSliderLayout.AddChild( mCheckButtons[i], TableView::CellPosition( i, 0 ) );
    channelSliderLayout.SetCellAlignment( TableView::CellPosition( i, 0 ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );

    TextLabel label = TextLabel::New( checkboxLabels[i] );
    std::ostringstream labelStyleName; labelStyleName << "colorLabel" << i+1;
    label.SetName( labelStyleName.str() );
    label.SetStyleName( labelStyleName.str() );
    label.SetParentOrigin( ParentOrigin::CENTER );
    label.SetAnchorPoint ( AnchorPoint::CENTER );
    label.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::WIDTH );
    label.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );

    channelSliderLayout.AddChild( label, TableView::CellPosition( i, 1 ) );
    channelSliderLayout.SetCellAlignment( TableView::CellPosition( i, 1 ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );

    mChannelSliders[i] = Slider::New();
    std::ostringstream sliderStyleName; sliderStyleName << "colorSlider" << i+1;
    mChannelSliders[i].SetName( sliderStyleName.str() );
    mChannelSliders[i].SetStyleName( sliderStyleName.str() );
    mChannelSliders[i].SetParentOrigin( ParentOrigin::CENTER );
    mChannelSliders[i].SetAnchorPoint ( AnchorPoint::CENTER );
    mChannelSliders[i].SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mChannelSliders[i].SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN , Dimension::HEIGHT );
    mChannelSliders[i].SetProperty( Slider::Property::LOWER_BOUND, 0.0f );
    mChannelSliders[i].SetProperty( Slider::Property::UPPER_BOUND, 100.0f );
    mChannelSliders[i].SetProperty( Slider::Property::VALUE_PRECISION, 0 );
    mChannelSliders[i].SetProperty( Slider::Property::VALUE, 100.0f );
    mChannelSliders[i].SetProperty( Slider::Property::ENABLED, true );
    mChannelSliders[i].SetProperty( Slider::Property::SHOW_POPUP, true );
    mChannelSliders[i].SetProperty( Slider::Property::SHOW_VALUE, true );

    mChannelSliders[i].RegisterProperty( "channel", i, Property::READ_WRITE );

    channelSliderLayout.AddChild( mChannelSliders[i], TableView::CellPosition( i, 2 ) );
    channelSliderLayout.SetCellAlignment( TableView::CellPosition( i, 2 ), HorizontalAlignment::RIGHT, VerticalAlignment::CENTER );

    mChannelSliders[i].ValueChangedSignal().Connect( this, &StylingApplication::OnSliderChanged );
  }

  mResetButton = PushButton::New();
  mResetButton.SetLabelText( "Reset" );
  mResetButton.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  mResetButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
  mResetButton.ClickedSignal().Connect( this, &StylingApplication::OnResetClicked );

  contentTable.Add( mResetButton );


  TableView themeButtonLayout = TableView::New( 1, 4 );
  themeButtonLayout.SetName("ThemeButtonsLayout");
  themeButtonLayout.SetCellPadding( Vector2( 6.0f, 0.0f ) );

  themeButtonLayout.SetAnchorPoint( AnchorPoint::CENTER );
  themeButtonLayout.SetParentOrigin( ParentOrigin::CENTER );
  themeButtonLayout.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  themeButtonLayout.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
  themeButtonLayout.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE ) );
  themeButtonLayout.SetFitHeight( 0 );

  TextLabel label = TextLabel::New( "Theme: ");
  label.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
  label.SetStyleName("themelabel");
  label.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  label.SetParentOrigin( ParentOrigin::TOP_CENTER );
  themeButtonLayout.AddChild( label, TableView::CellPosition( 0, 0 ) );
  themeButtonLayout.SetCellAlignment( TableView::CellPosition( 0, 0 ), HorizontalAlignment::LEFT, VerticalAlignment::CENTER );

  for( int i=0; i<3; ++i )
  {
    mThemeButtons[i] = PushButton::New();
    mThemeButtons[i].SetName("ThemeButton");
    mThemeButtons[i].SetStyleName("ThemeButton");
    mThemeButtons[i].SetParentOrigin( ParentOrigin::CENTER );
    mThemeButtons[i].SetAnchorPoint( ParentOrigin::CENTER );
    mThemeButtons[i].SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mThemeButtons[i].SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    mThemeButtons[i].RegisterProperty( "theme", i, Property::READ_WRITE );
    mThemeButtons[i].ClickedSignal().Connect( this, &StylingApplication::OnThemeButtonClicked );
    themeButtonLayout.AddChild( mThemeButtons[i], TableView::CellPosition( 0, 1+i ) );
  }
  mThemeButtons[0].SetLabelText( "Lite" ); // Lightweight changes on top of Dali
  mThemeButtons[1].SetLabelText( "App1" ); // Different application style
  mThemeButtons[2].SetLabelText( "App2" );

  contentTable.Add( themeButtonLayout );

}

Popup StylingApplication::CreateResetPopup()
{
  Stage stage = Stage::GetCurrent();

  Popup popup= Popup::New();
  popup.SetName("ResetPopup");
  popup.SetStyleName("ResetPopup");
  popup.SetParentOrigin( ParentOrigin::CENTER );
  popup.SetAnchorPoint( AnchorPoint::CENTER );
  popup.SetSize( stage.GetSize().width * 0.75f, 0.0f );
  popup.SetProperty( Popup::Property::TAIL_VISIBILITY, false );
  popup.OutsideTouchedSignal().Connect( this, &StylingApplication::HidePopup );
  popup.HiddenSignal().Connect( this, &StylingApplication::PopupHidden );

  popup.SetTitle( CreateTitle( "Reset channels" ) );

  TextLabel text = TextLabel::New( "This will reset the channel data to full value. Are you sure?" );
  text.SetName( "PopupContentText" );
  text.SetStyleName( "popupBody" );
  text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  text.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
  text.SetProperty( TextLabel::Property::MULTI_LINE, true );
  text.SetPadding( Padding( 10.0f, 10.0f, 20.0f, 0.0f ) );
  popup.SetContent( text );

  ImageView footer = ImageView::New( DEFAULT_CONTROL_AREA_IMAGE_PATH );
  footer.SetName( "PopupFooter" );
  footer.SetStyleName( "PopupFooter" );
  footer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  footer.SetResizePolicy( ResizePolicy::FIXED, Dimension::HEIGHT );
  footer.SetSize( 0.0f, 80.0f );
  footer.SetAnchorPoint( AnchorPoint::CENTER );
  footer.SetParentOrigin( ParentOrigin::CENTER );

  TableView footerLayout = TableView::New( 1, 2 );
  footerLayout.SetParentOrigin( ParentOrigin::CENTER );
  footerLayout.SetAnchorPoint ( AnchorPoint::CENTER );
  footerLayout.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

  PushButton okayButton = PushButton::New();
  okayButton.SetName( POPUP_CONTROL_OK_NAME );
  okayButton.SetStyleName( POPUP_CONTROL_OK_NAME );
  okayButton.SetLabelText( "Ok!" );
  okayButton.ClickedSignal().Connect( this, &StylingApplication::OnReset );
  okayButton.SetParentOrigin( ParentOrigin::CENTER );
  okayButton.SetAnchorPoint( AnchorPoint::CENTER );
  okayButton.SetResizePolicy( ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::ALL_DIMENSIONS );
  okayButton.SetSizeModeFactor( Vector3( -20.0f, -20.0f, 0.0f ) );

  PushButton cancelButton = PushButton::New();
  cancelButton.SetName( POPUP_CONTROL_CANCEL_NAME );
  cancelButton.SetStyleName( POPUP_CONTROL_CANCEL_NAME );
  cancelButton.SetLabelText( "Cancel" );
  cancelButton.ClickedSignal().Connect( this, &StylingApplication::OnResetCancelled );
  cancelButton.SetParentOrigin( ParentOrigin::CENTER );
  cancelButton.SetAnchorPoint( AnchorPoint::CENTER );
  cancelButton.SetResizePolicy( ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::ALL_DIMENSIONS );
  cancelButton.SetSizeModeFactor( Vector3( -20.0f, -20.0f, 0.0f ) );

  footerLayout.SetCellAlignment( TableView::CellPosition( 0, 0 ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );
  footerLayout.SetCellAlignment( TableView::CellPosition( 0, 1 ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );
  footerLayout.AddChild( okayButton, TableView::CellPosition( 0, 0 ) );
  footerLayout.AddChild( cancelButton, TableView::CellPosition( 0, 1 ) );
  footerLayout.SetCellPadding( Size( 10.f, 10.f ) );
  footer.Add( footerLayout );

  popup.SetFooter( footer );
  return popup;
}

TextLabel StylingApplication::CreateTitle( std::string title )
{
  TextLabel titleActor = TextLabel::New( title );
  titleActor.SetName( "titleActor" );
  titleActor.SetStyleName( "popupTitle" );
  titleActor.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  titleActor.SetProperty( TextLabel::Property::MULTI_LINE, false );

  return titleActor;
}

bool StylingApplication::OnButtonStateChange( Button button )
{
  // Todo: save / restore slider states per image

  if( mImageChannelControl )
  {
    if( mRadioButtons[0].IsSelected() )
    {
      mImageChannelControl.SetImage( BIG_IMAGE_1 );
    }
    else if( mRadioButtons[1].IsSelected() )
    {
      mImageChannelControl.SetImage( BIG_IMAGE_2 );
    }
    else if( mRadioButtons[2].IsSelected() )
    {
      mImageChannelControl.SetImage( BIG_IMAGE_3 );
    }
  }
  return true;
}

bool StylingApplication::OnCheckButtonChange( Button button )
{
  Property::Index index = button.GetPropertyIndex("channel");
  if( index != Property::INVALID_INDEX )
  {
    int channel = button.GetProperty<int>( index );
    float value = mChannelSliders[channel].GetProperty<float>( Slider::Property::VALUE );
    if( !button.IsSelected() )
    {
      // "Turn off" the channel's contribution
      value = 0.0f;
    }
    Property::Index channelIndex = GetChannelProperty( channel );
    mImageChannelControl.SetProperty(channelIndex, value/100.0f);
  }
  return true;
}

bool StylingApplication::OnThemeButtonClicked( Button button )
{
  int theme = button.GetProperty<int>(button.GetPropertyIndex("theme"));
  const char* themePath=NULL;

  switch( theme )
  {
    case 0:
    {
      themePath = DEMO_THEME_ONE_PATH;
      break;
    }
    case 1:
    {
      themePath = DEMO_THEME_TWO_PATH;
      break;
    }
    case 2:
    {
      themePath = DEMO_THEME_THREE_PATH;
      break;
    }
  }
  StyleManager::Get().ApplyTheme( themePath );
  return true;
}

bool StylingApplication::OnResetClicked( Button button )
{
  if( ! mResetPopup )
  {
    mResetPopup = CreateResetPopup ();
  }

  Stage::GetCurrent().Add( mResetPopup );

  mResetPopup.SetDisplayState( Popup::SHOWN );
  return true;
}

bool StylingApplication::OnReset( Button button )
{
  // todo: Reset the sliders for this image
  for( int i=0; i<3; ++i )
  {
    mChannelSliders[i].SetProperty( Slider::Property::VALUE, 100.0f );
  }
  HidePopup();
  return true;
}

bool StylingApplication::OnResetCancelled( Button button )
{
  HidePopup();
  return true;
}

bool StylingApplication::OnSliderChanged( Slider slider, float value )
{
  // todo - change color channel's saturation
  Property::Index index = slider.GetPropertyIndex("channel");
  if( index != Property::INVALID_INDEX )
  {
    int channel = slider.GetProperty<int>( index );
    if( mCheckButtons[channel].IsSelected() )
    {
      Property::Index channelIndex = GetChannelProperty( channel );
      mImageChannelControl.SetProperty(channelIndex, value/100.0f);
    }
  }
  return true;
}

void StylingApplication::HidePopup()
{
  if( mResetPopup )
  {
    mResetPopup.SetDisplayState( Popup::HIDDEN );
  }
}

void StylingApplication::PopupHidden()
{
  if( mResetPopup )
  {
    // Clear down resources
    mResetPopup.Unparent();
    mResetPopup.Reset();
  }
}

void StylingApplication::OnKeyEvent( const KeyEvent& keyEvent )
{
  static int keyPressed = 0;

  if( keyEvent.state == KeyEvent::Down)
  {
    if( keyPressed == 0 ) // Is this the first down event?
    {
      printf("Key pressed: %s %d\n", keyEvent.keyPressedName.c_str(), keyEvent.keyCode );

      if( IsKey( keyEvent, DALI_KEY_ESCAPE) || IsKey( keyEvent, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
      else if( keyEvent.keyPressedName.compare("Return") == 0 )
      {
        mCurrentTheme++;
        mCurrentTheme %= NUMBER_OF_THEMES;

        StyleManager styleManager = StyleManager::Get();
        switch( mCurrentTheme )
        {
          case 0:
          {
            styleManager.ApplyTheme( DEMO_THEME_ONE_PATH );
            printf("Changing to theme One\n");
            break;
          }
          case 1:
          {
            styleManager.ApplyTheme( DEMO_THEME_TWO_PATH );
            printf("Changing to theme Two\n");
            break;
          }
          case 2:
          {
            styleManager.ApplyDefaultTheme();
            printf("Changing to Toolkit theme\n");
            break;
          }
        }
      }
    }
    keyPressed = 1;
  }
  else if( keyEvent.state == KeyEvent::Up )
  {
    keyPressed = 0;
  }
}

} // namespace Demo
