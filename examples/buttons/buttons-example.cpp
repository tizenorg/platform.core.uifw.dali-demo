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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;

namespace
{
const char* const SMALL_IMAGE_1_DIR = DALI_IMAGE_DIR "buttons-small-image-1.png";
const char* const BIG_IMAGE_1_DIR = DALI_IMAGE_DIR "buttons-big-image-1.png";

const char* const SMALL_IMAGE_2_DIR = DALI_IMAGE_DIR "buttons-small-image-2.png";
const char* const BIG_IMAGE_2_DIR = DALI_IMAGE_DIR "buttons-big-image-2.png";

const char* const PUSHBUTTON_PRESS_IMAGE_DIR = DALI_IMAGE_DIR "buttons-press-image.png";
const char* const PUSHBUTTON_DIM_IMAGE_DIR = DALI_IMAGE_DIR "buttons-dim-image.png";
const char* const PUSHBUTTON_BUTTON_IMAGE_DIR = DALI_IMAGE_DIR "buttons-button-image.png";

const char* const CHECKBOX_UNCHECKED_IMAGE_DIR = DALI_IMAGE_DIR "buttons-checkbox-unchecked.png";
const char* const CHECKBOX_CHECKED_IMAGE_DIR = DALI_IMAGE_DIR "buttons-checkbox-checked.png";
}

// This example shows how to create and use different buttons.
//

class ButtonsController: public ConnectionTracker
{
 public:

  ButtonsController(Application& application)
    : mApplication(application)
  {
    std::cout << "ButtonsController::ButtonsController" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ButtonsController::Create);
  }

  ~ButtonsController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime

  void Create(Application& application)
  {
    std::cout << "ButtonsController::Create" << std::endl;

    /* radio buttons group 1 */
    Actor radioButtonsGroup1 = Actor::New();

    Toolkit::RadioButton radioButton1 = Dali::Toolkit::RadioButton::New("PushButton");
    radioButton1.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    radioButton1.SetPosition(10, 10);
    radioButton1.SetActive(true);

    radioButton1.ClickedSignal().Connect(this, &ButtonsController::SetPushButton);

    Toolkit::RadioButton radioButton2 = Dali::Toolkit::RadioButton::New("DimmedButton");
    radioButton2.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    radioButton2.SetPosition(10, 60);

    radioButton1.ClickedSignal().Connect(this, &ButtonsController::SetDimButton);

    radioButtonsGroup1.Add(radioButton1);
    radioButtonsGroup1.Add(radioButton2);

    Stage::GetCurrent().Add(radioButtonsGroup1);
    /* radio buttons group 1 */

    /* radio buttons group 2 */
    Actor radioButtonsGroup2 = Actor::New();

    Actor radioButton3Label = Actor::New();
    radioButton3Label.Add(ImageActor::New(Image::New(SMALL_IMAGE_1_DIR)));

    radioButton3 = Dali::Toolkit::RadioButton::New(radioButton3Label);
    radioButton3.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    radioButton3.SetPosition(220, 10);
    radioButton3.SetActive(true);

    Actor radioButton4Label = Actor::New();
    radioButton4Label.Add(ImageActor::New(Image::New(SMALL_IMAGE_2_DIR)));

    radioButton4 = Dali::Toolkit::RadioButton::New(radioButton4Label);
    radioButton4.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    radioButton4.SetPosition(220, 60);

    radioButtonsGroup2.Add(radioButton3);
    radioButtonsGroup2.Add(radioButton4);

    Stage::GetCurrent().Add(radioButtonsGroup2);
    /* radio buttons group 2 */

    /* Button */
    mUpdateButton = Toolkit::PushButton::New();
    mUpdateButton.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    mUpdateButton.SetPosition(10, 150);
    mUpdateButton.SetLabelText("Update image");
    mUpdateButton.SetSize(150, 60);

    Dali::Image pressImage = Dali::Image::New(PUSHBUTTON_PRESS_IMAGE_DIR);
    Dali::Image dimImage = Dali::Image::New(PUSHBUTTON_DIM_IMAGE_DIR);
    Dali::Image buttonImage = Dali::Image::New(PUSHBUTTON_BUTTON_IMAGE_DIR);

    mUpdateButton.SetPressedImage(pressImage);
    mUpdateButton.SetDimmedImage(dimImage);
    mUpdateButton.SetButtonImage(buttonImage);

    mUpdateButton.ClickedSignal().Connect(this, &ButtonsController::OnButtonClicked);

    Stage::GetCurrent().Add(mUpdateButton);
    /* Button */

    /* Image Actor */
    mBigImage1 = Image::New(BIG_IMAGE_1_DIR);
    mBigImage2 = Image::New(BIG_IMAGE_2_DIR);

    mImage = ImageActor::New(mBigImage1);
    mImage.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    mImage.SetPosition(180, 150);

    Stage::GetCurrent().Add(mImage);
    /* Image Actor */

    /* CheckBoxes */
    Dali::Image unchecked = Dali::Image::New(CHECKBOX_UNCHECKED_IMAGE_DIR);
    Dali::Image checked = Dali::Image::New(CHECKBOX_CHECKED_IMAGE_DIR);

    CheckBox1 = Dali::Toolkit::CheckBoxButton::New();

    CheckBox1.SetPosition(10, 270);
    CheckBox1.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    CheckBox1.SetBackgroundImage(unchecked);
    CheckBox1.SetCheckedImage(checked);
    CheckBox1.SetSize(unchecked.GetWidth(), unchecked.GetHeight());
    CheckBox1.ClickedSignal().Connect(this, &ButtonsController::OnCheckBoxesClicked);

    CheckBox2 = Dali::Toolkit::CheckBoxButton::New();

    CheckBox2.SetPosition(10, 330);
    CheckBox2.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    CheckBox2.SetBackgroundImage(unchecked);
    CheckBox2.SetCheckedImage(checked);
    CheckBox2.SetSize(unchecked.GetWidth(), unchecked.GetHeight());
    CheckBox2.SetChecked(true);
    CheckBox2.ClickedSignal().Connect(this, &ButtonsController::OnCheckBoxesClicked);

    CheckBox3 = Dali::Toolkit::CheckBoxButton::New();

    CheckBox3.SetPosition(10, 390);
    CheckBox3.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    CheckBox3.SetBackgroundImage(unchecked);
    CheckBox3.SetCheckedImage(checked);
    CheckBox3.SetSize(unchecked.GetWidth(), unchecked.GetHeight());
    CheckBox3.ClickedSignal().Connect(this, &ButtonsController::OnCheckBoxesClicked);

    Stage::GetCurrent().Add(CheckBox1);
    Stage::GetCurrent().Add(CheckBox2);
    Stage::GetCurrent().Add(CheckBox3);
    /* CheckBoxes */

    /* CheckBoxes States */
    CheckBox1State = Toolkit::TextView::New("CheckBox1 is unchecked");

    CheckBox1State.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    CheckBox1State.SetPosition(60, 270);

    CheckBox2State = Toolkit::TextView::New("CheckBox2 is checked");

    CheckBox2State.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    CheckBox2State.SetPosition(60, 330);

    CheckBox3State = Toolkit::TextView::New("CheckBox3 is unchecked");

    CheckBox3State.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    CheckBox3State.SetPosition(60, 390);

    Stage::GetCurrent().Add(CheckBox1State);
    Stage::GetCurrent().Add(CheckBox2State);
    Stage::GetCurrent().Add(CheckBox3State);
    /* CheckBoxes States */

    // Respond to a click back
    Stage::GetCurrent().KeyEventSignal().Connect(this, &ButtonsController::OnKeyEvent);
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK) )
      {
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
    if( radioButton3.IsActive() )
    {
      mImage.SetImage(mBigImage1);
    }
    else if( radioButton4.IsActive() )
    {
      mImage.SetImage(mBigImage2);
    }
    return true;
  }

  bool OnCheckBoxesClicked(Toolkit::Button button)
  {
    if( CheckBox1.IsChecked() )
    {
      CheckBox1State.SetText("CheckBox1 is checked");
    }
    else
    {
      CheckBox1State.SetText("CheckBox1 is unchecked");
    }

    if( CheckBox2.IsChecked() )
    {
      CheckBox2State.SetText("CheckBox2 is checked");
    }
    else
    {
      CheckBox2State.SetText("CheckBox2 is unchecked");
    }

    if( CheckBox3.IsChecked() )
    {
      CheckBox3State.SetText("CheckBox3 is checked");
    }
    else
    {
      CheckBox3State.SetText("CheckBox3 is unchecked");
    }

    return true;
  }

 private:
  Application& mApplication;

  Toolkit::RadioButton radioButton3;
  Toolkit::RadioButton radioButton4;

  Toolkit::PushButton mUpdateButton;

  Image mBigImage1;
  Image mBigImage2;
  ImageActor mImage;

  Toolkit::CheckBoxButton CheckBox1;
  Toolkit::CheckBoxButton CheckBox2;
  Toolkit::CheckBoxButton CheckBox3;

  Toolkit::TextView CheckBox1State;
  Toolkit::TextView CheckBox2State;
  Toolkit::TextView CheckBox3State;
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
