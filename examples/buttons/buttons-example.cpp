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

namespace
{
// Used to produce visually same dimensions on desktop and device builds
float ScalePointSize( int pointSize )
{
  Dali::Vector2 dpi = Dali::Stage::GetCurrent().GetDpi();
  float meanDpi = (dpi.height + dpi.width) * 0.5f;
  return pointSize * meanDpi / 220.0f;
}

} // namespace

// Define this so that it is interchangeable
// "DP" stands for Device independent Pixels
#define DP(x) ScalePointSize(x)


namespace
{

const char* const BACKGROUND_IMAGE = DALI_IMAGE_DIR "background-gradient.jpg";
const char* const TOOLBAR_IMAGE = DALI_IMAGE_DIR "top-bar.png";

const char* const TOOLBAR_TITLE = "Buttons";

const char* const SMALL_IMAGE_1 = DALI_IMAGE_DIR "gallery-small-14.jpg";
const char* const BIG_IMAGE_1 = DALI_IMAGE_DIR "gallery-large-4.jpg";

const char* const SMALL_IMAGE_2 = DALI_IMAGE_DIR "gallery-small-20.jpg";
const char* const BIG_IMAGE_2 = DALI_IMAGE_DIR "gallery-large-11.jpg";

const char* const SMALL_IMAGE_3 = DALI_IMAGE_DIR "gallery-small-25.jpg";
const char* const BIG_IMAGE_3 = DALI_IMAGE_DIR "gallery-large-13.jpg";

const char* const ENABLED_IMAGE = DALI_IMAGE_DIR "item-select-check.png";

const char* const PUSHBUTTON_PRESS_IMAGE = DALI_IMAGE_DIR "button-down.9.png";
const char* const PUSHBUTTON_DIM_IMAGE = DALI_IMAGE_DIR "button-disabled.9.png";
const char* const PUSHBUTTON_BUTTON_IMAGE = DALI_IMAGE_DIR "button-up.9.png";

const char* const CHECKBOX_UNCHECKED_IMAGE = DALI_IMAGE_DIR "checkbox-unchecked.png";
const char* const CHECKBOX_CHECKED_IMAGE = DALI_IMAGE_DIR "checkbox-checked.png";

const Vector4 BACKGROUND_COLOUR( 1.0f, 1.0f, 1.0f, 0.15f );

}  // namespace

/** This example shows how to create and use different buttons.
 *
 * 1. First group of radio buttons with image actor labels selects an image to load
 * 2. A push button loads the selected thumbnail image into the larger image pane
 * 3. Second group of radio buttons with a table view label containing a text view and image view, and a normal text view.
 *    Selecting one of these will enable/disable the image loading push button
 * 4. A group of check boxes
 */
class ButtonsController: public ConnectionTracker
{
 public:

  ButtonsController( Application& application )
    : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ButtonsController::Create );
  }

  ~ButtonsController()
  {
    // Nothing to do here
  }

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime

    // Respond to key events
    Stage::GetCurrent().KeyEventSignal().Connect(this, &ButtonsController::OnKeyEvent);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            TOOLBAR_TITLE );
    const int MARGIN_SIZE = 10;
    const int TOP_MARGIN = 85;

    int yPos = TOP_MARGIN + MARGIN_SIZE;

    const int group2Height = 238;

    // Image selector radio group
    Actor radioGroup2Background = Toolkit::CreateSolidColorActor( BACKGROUND_COLOUR );
    radioGroup2Background.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    radioGroup2Background.SetParentOrigin( ParentOrigin::TOP_LEFT );
    radioGroup2Background.SetPosition( DP(MARGIN_SIZE), DP(yPos) );
    radioGroup2Background.SetSize( DP(348), DP(group2Height) );
    mContentLayer.Add( radioGroup2Background );

    Actor radioButtonsGroup2 = Actor::New();
    radioButtonsGroup2.SetParentOrigin( ParentOrigin::TOP_LEFT );
    radioButtonsGroup2.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    radioButtonsGroup2.SetPosition( DP(MARGIN_SIZE), DP(MARGIN_SIZE) );
    radioButtonsGroup2.SetSize( DP(100), DP(160) );

    radioGroup2Background.Add( radioButtonsGroup2 );

    const int RADIO_IMAGE_HEIGHT = 48;
    const int RADIO_IMAGE_SPACING = 8;

    int radioY = 0;

    // Radio 1
    {
      ImageActor imageActor = ImageActor::New( Image::New( SMALL_IMAGE_1 ) );
      imageActor.SetSize( DP(RADIO_IMAGE_HEIGHT), DP(RADIO_IMAGE_HEIGHT) );
      mRadioButtonImage1 = Dali::Toolkit::RadioButton::New( imageActor );
      mRadioButtonImage1.SetParentOrigin( ParentOrigin::TOP_LEFT );
      mRadioButtonImage1.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      mRadioButtonImage1.SetPosition( 0, DP(radioY) );
      mRadioButtonImage1.SetActive( true );

      radioButtonsGroup2.Add( mRadioButtonImage1 );
    }

    // Radio 2
    {
      radioY += RADIO_IMAGE_HEIGHT + RADIO_IMAGE_SPACING;

      ImageActor imageActor = ImageActor::New( Image::New( SMALL_IMAGE_2 ) );
      imageActor.SetSize( DP(RADIO_IMAGE_HEIGHT), DP(RADIO_IMAGE_HEIGHT) );

      mRadioButtonImage2 = Dali::Toolkit::RadioButton::New( imageActor );
      mRadioButtonImage2.SetParentOrigin( ParentOrigin::TOP_LEFT );
      mRadioButtonImage2.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      mRadioButtonImage2.SetPosition( 0, DP(radioY) );

      radioButtonsGroup2.Add( mRadioButtonImage2 );
    }

    // Radio 3
    {
      radioY += RADIO_IMAGE_HEIGHT + RADIO_IMAGE_SPACING;

      ImageActor imageActor = ImageActor::New( Image::New( SMALL_IMAGE_3 ) );
      imageActor.SetSize( DP(RADIO_IMAGE_HEIGHT), DP(RADIO_IMAGE_HEIGHT) );

      mRadioButtonImage3 = Dali::Toolkit::RadioButton::New( imageActor );
      mRadioButtonImage3.SetParentOrigin( ParentOrigin::TOP_LEFT );
      mRadioButtonImage3.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      mRadioButtonImage3.SetPosition( 0, DP(radioY) );

      radioButtonsGroup2.Add( mRadioButtonImage3 );
    }

    // Create select button
    mUpdateButton = Toolkit::PushButton::New();
    mUpdateButton.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    mUpdateButton.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mUpdateButton.SetPosition( 0, DP(MARGIN_SIZE) );
    mUpdateButton.SetLabelText("Select");
    mUpdateButton.SetSize( DP(100), DP(RADIO_IMAGE_HEIGHT) );

    mUpdateButton.SetPressedImage( Dali::Image::New( PUSHBUTTON_PRESS_IMAGE ) );
    mUpdateButton.SetDimmedImage( Dali::Image::New( PUSHBUTTON_DIM_IMAGE ) );
    mUpdateButton.SetButtonImage( Dali::Image::New( PUSHBUTTON_BUTTON_IMAGE ) );

    mUpdateButton.ClickedSignal().Connect( this, &ButtonsController::OnButtonClicked );

    radioButtonsGroup2.Add(mUpdateButton);

    // ImageActor to display selected image
    mBigImage1 = Image::New( BIG_IMAGE_1 );
    mBigImage2 = Image::New( BIG_IMAGE_2 );
    mBigImage3 = Image::New( BIG_IMAGE_3 );

    mImage = ImageActor::New( mBigImage1 );
    mImage.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    mImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mImage.SetPosition( DP(MARGIN_SIZE), 0 );
    mImage.SetSize( DP(218), DP(218) );
    radioButtonsGroup2.Add( mImage );

    // The enable/disable radio group
    yPos += group2Height + MARGIN_SIZE;

    const int group1Height = 120;

    Actor radioGroup1Background = Toolkit::CreateSolidColorActor( BACKGROUND_COLOUR );
    radioGroup1Background.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    radioGroup1Background.SetParentOrigin( ParentOrigin::TOP_LEFT );
    radioGroup1Background.SetPosition( DP(MARGIN_SIZE), DP(yPos) );
    radioGroup1Background.SetSize( DP(348), DP(group1Height) );
    mContentLayer.Add( radioGroup1Background );

    // Radio group
    Actor radioButtonsGroup1 = Actor::New();
    radioButtonsGroup1.SetParentOrigin( ParentOrigin::TOP_LEFT );
    radioButtonsGroup1.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    radioButtonsGroup1.SetPosition( DP(MARGIN_SIZE), DP(MARGIN_SIZE) );

    radioGroup1Background.Add( radioButtonsGroup1 );

    // First radio button
    {
      Toolkit::TableView tableView = Toolkit::TableView::New( 1, 2 );
      tableView.SetSize( DP(260), DP(RADIO_IMAGE_HEIGHT) );

      Toolkit::TextView textView = Toolkit::TextView::New( "Select enabled" );
      Toolkit::Alignment alignment = Toolkit::Alignment::New( Toolkit::Alignment::HorizontalLeft );
      alignment.Add( textView );
      tableView.AddChild( alignment, Toolkit::TableView::CellPosition( 0, 0 ) );

      ImageActor imageActor = ImageActor::New( Image::New( ENABLED_IMAGE ) );
      imageActor.SetSize( DP(RADIO_IMAGE_HEIGHT), DP(RADIO_IMAGE_HEIGHT) );
      tableView.AddChild( imageActor, Toolkit::TableView::CellPosition( 0, 1 ) );
      tableView.SetFixedWidth( 1, DP(RADIO_IMAGE_HEIGHT) );

      Toolkit::RadioButton radioButton = Dali::Toolkit::RadioButton::New( tableView );
      radioButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
      radioButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      radioButton.SetPosition( 0, 0 );
      radioButton.SetActive( true );

      radioButton.ClickedSignal().Connect( this, &ButtonsController::SetPushButton );

      radioButtonsGroup1.Add( radioButton );
    }

    // Second radio button
    {
      Toolkit::RadioButton radioButton = Dali::Toolkit::RadioButton::New( "Select disabled" );
      radioButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
      radioButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      radioButton.SetPosition( 0, DP(50) );

      radioButton.ClickedSignal().Connect( this, &ButtonsController::SetDimButton );

      radioButtonsGroup1.Add( radioButton );
    }

    // CheckBoxes
    yPos += group1Height + MARGIN_SIZE;

    Actor checkBoxBackground = Toolkit::CreateSolidColorActor( BACKGROUND_COLOUR );
    checkBoxBackground.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    checkBoxBackground.SetParentOrigin( ParentOrigin::TOP_LEFT );
    checkBoxBackground.SetPosition( DP(MARGIN_SIZE), DP(yPos) );
    checkBoxBackground.SetSize( DP(430), DP(190) );
    mContentLayer.Add( checkBoxBackground );

    Dali::Image unchecked = Dali::Image::New( CHECKBOX_UNCHECKED_IMAGE );
    Dali::Image checked = Dali::Image::New( CHECKBOX_CHECKED_IMAGE );

    int checkYPos = MARGIN_SIZE;

    mCheckBox1 = Dali::Toolkit::CheckBoxButton::New();

    mCheckBox1.SetPosition( DP(MARGIN_SIZE), DP(checkYPos) );
    mCheckBox1.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mCheckBox1.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mCheckBox1.SetBackgroundImage( unchecked );
    mCheckBox1.SetCheckedImage( checked );
    mCheckBox1.SetSize( DP(48), DP(48) );
    mCheckBox1.ClickedSignal().Connect( this, &ButtonsController::OnCheckBoxesClicked );

    mCheckBox1State = Toolkit::TextView::New( "CheckBox1 is unchecked" );

    mCheckBox1State.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mCheckBox1State.SetPosition( DP(80), DP(checkYPos) );

    checkYPos += 60;

    mCheckBox2 = Dali::Toolkit::CheckBoxButton::New();

    mCheckBox2.SetPosition( DP(MARGIN_SIZE), DP(checkYPos) );
    mCheckBox2.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mCheckBox2.SetBackgroundImage( unchecked );
    mCheckBox2.SetCheckedImage( checked );
    mCheckBox2.SetSize( DP(48), DP(48) );
    mCheckBox2.SetChecked( true );
    mCheckBox2.ClickedSignal().Connect( this, &ButtonsController::OnCheckBoxesClicked );

    mCheckBox2State = Toolkit::TextView::New( "CheckBox2 is checked" );
    mCheckBox2State.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mCheckBox2State.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mCheckBox2State.SetPosition( DP(80), DP(checkYPos) );

    checkYPos += 60;

    mCheckBox3 = Dali::Toolkit::CheckBoxButton::New();

    mCheckBox3.SetPosition( DP(MARGIN_SIZE), DP(checkYPos) );
    mCheckBox3.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mCheckBox3.SetBackgroundImage( unchecked );
    mCheckBox3.SetCheckedImage( checked );
    mCheckBox3.SetSize( DP(48), DP(48) );
    mCheckBox3.ClickedSignal().Connect( this, &ButtonsController::OnCheckBoxesClicked );

    mCheckBox3State = Toolkit::TextView::New( "CheckBox3 is unchecked" );

    mCheckBox3State.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mCheckBox3State.SetPosition( DP(80), DP(checkYPos) );

    checkBoxBackground.Add( mCheckBox1 );
    checkBoxBackground.Add( mCheckBox2 );
    checkBoxBackground.Add( mCheckBox3 );

    checkBoxBackground.Add( mCheckBox1State );
    checkBoxBackground.Add( mCheckBox2State );
    checkBoxBackground.Add( mCheckBox3State );
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK) )
      {
        // Exit application when click back or escape.
        mApplication.Quit();
      }
    }
  }

  bool SetPushButton(Toolkit::Button button)
  {
    mUpdateButton.SetDimmed(false);
    return true;
  }

  bool SetDimButton(Toolkit::Button button)
  {
    mUpdateButton.SetDimmed(true);
    return true;
  }

  bool OnButtonClicked(Toolkit::Button button)
  {
    if( mRadioButtonImage1.IsActive() )
    {
      mImage.SetImage(mBigImage1);
    }
    else if( mRadioButtonImage2.IsActive() )
    {
      mImage.SetImage(mBigImage2);
    }
    else if( mRadioButtonImage3.IsActive() )
    {
      mImage.SetImage(mBigImage3);
    }
    return true;
  }

  bool OnCheckBoxesClicked(Toolkit::Button button)
  {
    if( mCheckBox1.IsChecked() )
    {
      mCheckBox1State.SetText("CheckBox1 is checked");
    }
    else
    {
      mCheckBox1State.SetText("CheckBox1 is unchecked");
    }

    if( mCheckBox2.IsChecked() )
    {
      mCheckBox2State.SetText("CheckBox2 is checked");
    }
    else
    {
      mCheckBox2State.SetText("CheckBox2 is unchecked");
    }

    if( mCheckBox3.IsChecked() )
    {
      mCheckBox3State.SetText("CheckBox3 is checked");
    }
    else
    {
      mCheckBox3State.SetText("CheckBox3 is unchecked");
    }

    return true;
  }

 private:

  Application&      mApplication;
  Toolkit::View     mView;                              ///< The View instance.
  Toolkit::ToolBar  mToolBar;                           ///< The View's Toolbar.
  Layer             mContentLayer;                      ///< Content layer

  Toolkit::RadioButton mRadioButtonImage1;
  Toolkit::RadioButton mRadioButtonImage2;
  Toolkit::RadioButton mRadioButtonImage3;

  Toolkit::PushButton mUpdateButton;

  Image mBigImage1;
  Image mBigImage2;
  Image mBigImage3;
  ImageActor mImage;

  Toolkit::CheckBoxButton mCheckBox1;
  Toolkit::CheckBoxButton mCheckBox2;
  Toolkit::CheckBoxButton mCheckBox3;

  Toolkit::TextView mCheckBox1State;
  Toolkit::TextView mCheckBox2State;
  Toolkit::TextView mCheckBox3State;
};

void RunTest(Application& application)
{
  ButtonsController test(application);

  application.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main(int argc, char **argv)
{
  Application application = Application::New(&argc, &argv);

  RunTest(application);

  return 0;
}
