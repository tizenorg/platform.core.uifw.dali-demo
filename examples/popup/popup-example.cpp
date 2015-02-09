//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "../shared/view.h"

#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace std;
using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const char* BACKGROUND_IMAGE_PATH = DALI_IMAGE_DIR "background-gradient.jpg";
const char* TOOLBAR_IMAGE = DALI_IMAGE_DIR "top-bar.png";
const char* TOOLBAR_TITLE = "Popups";

const char* BUTTON_BACKGROUND_IMAGE_PATH = DALI_IMAGE_DIR "button-background.png";
const char* DUMMY_IMAGE_PATH = DALI_IMAGE_DIR "contacts-background.png";
const char* SMALL_IMAGE_PATH = DALI_IMAGE_DIR "gallery-medium-53.jpg";

const Vector3 MENU_BUTTON_SIZE = Vector3( 150.0f, 50.0f, 1.0f);               ///< Standard Menu Buttons.

const Vector4 BUTTON_IMAGE_BORDER( Vector4::ONE * 3.0f );

} // namespace

/**
 * This example shows how to use Popup UI control.
 * Popups are used to present Modal/Non-Modal dialog boxes.
 */
class PopupController : public ConnectionTracker
{
public:

  /**
   * Constructor
   * @param application class, stored as reference
   */
  PopupController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init and orientation changed signal
    mApplication.InitSignal().Connect(this, &PopupController::OnInit);
  }

  /**
   * This method gets called once the main loop of application is up and running
   */
  void OnInit(Application& app)
  {
    Stage stage = Dali::Stage::GetCurrent();

    stage.KeyEventSignal().Connect( this, &PopupController::OnKeyEvent );

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContents = DemoHelper::CreateView( app,
                                        mView,
                                        mToolBar,
                                        BACKGROUND_IMAGE_PATH,
                                        TOOLBAR_IMAGE,
                                        TOOLBAR_TITLE );

    mPopupLayer = Layer::New();
    mPopupLayer.SetParentOrigin( ParentOrigin::CENTER );
    mPopupLayer.SetAnchorPoint( AnchorPoint::CENTER );
    mContents.Add(mPopupLayer);

    {
      // 1a. Create Touch Me Button /////////////////////////////////////////////
      PushButton touchMe = CreateButton("Touch Me!");
      mContents.Add(touchMe);
      touchMe.SetSize( MENU_BUTTON_SIZE );
      touchMe.SetAnchorPoint( AnchorPoint::CENTER );
      touchMe.SetParentOrigin( ParentOrigin::CENTER );

      touchMe.ClickedSignal().Connect(this, &PopupController::OnTouchMeTouched);

      // 1b. Create Touch Me Popup
      mInfoPopup = CreatePopup("You touched me!");
      PushButton buttonClose = CreateButton("Close");
      mInfoPopup.AddButton(buttonClose);
      mPopupLayer.Add(mInfoPopup);
      buttonClose.ClickedSignal().Connect(this, &PopupController::OnPopupClose);
      mInfoPopup.OutsideTouchedSignal().Connect(this, &PopupController::OnPopupTouchedOutside);
      mInfoPopup.SetSizePolicy( Toolkit::Control::Fixed, Toolkit::Control::Fixed );
    }

    {
      // 2a. Create Quit Button /////////////////////////////////////////////////
      mQuitButton = CreateButton("Quit");
      mQuitButton.SetSize( MENU_BUTTON_SIZE );
      mQuitButton.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
      mQuitButton.SetParentOrigin( ParentOrigin::BOTTOM_RIGHT );

      mQuitButton.ClickedSignal().Connect(this, &PopupController::OnQuitTouched);

      mContents.Add(mQuitButton);

      // 2b. Create Quit Popup
      mQuitPopup = CreatePopup("Do you want to Quit?");

      PushButton buttonOk = CreateButton("Ok");
      mQuitPopup.AddButton(buttonOk);
      PushButton buttonCancel = CreateButton("Cancel");
      mQuitPopup.AddButton(buttonCancel);
      buttonOk.ClickedSignal().Connect(this, &PopupController::OnPopupOk);
      buttonCancel.ClickedSignal().Connect(this, &PopupController::OnPopupCancel);
      mQuitPopup.OutsideTouchedSignal().Connect(this, &PopupController::OnPopupTouchedOutside);
      mQuitPopup.SetSizePolicy( Toolkit::Control::Fixed, Toolkit::Control::Fixed );

      // Add some custom content to this.
      Image dummyImage = Image::New(DUMMY_IMAGE_PATH);
      ImageActor dummy = ImageActor::New(dummyImage);
      dummy.SetAnchorPoint( AnchorPoint::CENTER );
      dummy.SetParentOrigin( ParentOrigin::CENTER );

      mQuitPopup.Add(dummy);

      mPopupLayer.Add(mQuitPopup);
    }

    {
      // 3a. Create Small Image Button /////////////////////////////////////////////
      PushButton button = CreateButton( "Small Image" );
      mContents.Add(button);
      button.SetSize( MENU_BUTTON_SIZE );
      button.SetAnchorPoint( AnchorPoint::CENTER );
      button.SetParentOrigin( ParentOrigin::CENTER );
      button.SetPosition( 0.0f, -200.0f, 0.0f );

      button.ClickedSignal().Connect(this, &PopupController::OnSmallImageButtonTouched);

      // 3b. Create Small Image Popup
      mSmallImagePopup = CreatePopup("This is a long title with a small image");

      PushButton buttonOk = CreateButton( "Ok" );
      mSmallImagePopup.AddButton( buttonOk );
      PushButton buttonCancel = CreateButton( "Cancel" );
      mSmallImagePopup.AddButton( buttonCancel );
      buttonOk.ClickedSignal().Connect(this, &PopupController::OnSmallButtonPopupOk);
      buttonCancel.ClickedSignal().Connect(this, &PopupController::OnSmallButtonPopupCancel);
      mSmallImagePopup.OutsideTouchedSignal().Connect(this, &PopupController::OnSmallButtonPopupTouchedOutside);
      mSmallImagePopup.SetSizePolicy( Toolkit::Control::Fixed, Toolkit::Control::Fixed );

      // Add some custom content to this.
      Image dummyImage = Image::New( SMALL_IMAGE_PATH );
      ImageActor dummy = ImageActor::New( dummyImage );
      dummy.SetAnchorPoint( AnchorPoint::CENTER );
      dummy.SetParentOrigin( ParentOrigin::CENTER );

      mSmallImagePopup.Add( dummy );

      mPopupLayer.Add( mSmallImagePopup );
    }

    KeyboardFocusManager::Get().PreFocusChangeSignal().Connect( this, &PopupController::PreFocusChange );
  }

private:

  Actor PreFocusChange( Actor current, Actor proposed, Control::KeyboardFocusNavigationDirection direction )
  {
    if ( !current && !proposed && mQuitPopup.GetState() == Popup::POPUP_SHOW )
    {
      return mQuitPopup;
    }
    else if ( !current && !proposed && mQuitPopup.GetState() != Popup::POPUP_SHOW )
    {
      return mQuitButton;
    }

    return proposed;
  }

  /**
   * Creates a Button
   *
   * @param[in] text The text to appear on this button
   */
  PushButton CreateButton(const std::string& text)
  {
    // Create the button
    Image img = Image::New(BUTTON_BACKGROUND_IMAGE_PATH);
    ImageActor background = ImageActor::New(img);

    background.SetStyle(ImageActor::STYLE_NINE_PATCH);
    background.SetNinePatchBorder(BUTTON_IMAGE_BORDER);

    PushButton button = PushButton::New();
    button.SetBackgroundImage(background);

    // Add a text label to the button
    button.SetLabel(text);

    return button;
  }

  /**
   * Creates a Popup, dialogs are by default hidden.
   *
   * @param[in] title The title to appear on this buton
   */
  Popup CreatePopup(const std::string& title)
  {
    Stage stage = Dali::Stage::GetCurrent();

    Popup popup = Popup::New();
    popup.SetAnchorPoint( AnchorPoint::CENTER );
    popup.SetParentOrigin( ParentOrigin::CENTER );
    //popup.SetBackgroundImage(ImageActor::New(Image::New(BACKGROUND_IMAGE_PATH2)));

    // Add a title to the popup

    popup.SetTitle( title );

    return popup;
  }

  /**
   * Signal handler, called when the 'Close' button in the info-popup is touched.
   *
   * @param[in] button The button that was pressed.
   */
  bool OnPopupClose(Button button)
  {
    mInfoPopup.Hide();
    return true;
  }

  /**
   * Signal handler, called when the 'Ok' button in the quit-popup is touched.
   *
   * @param[in] button The button that was pressed.
   */
  bool OnPopupOk(Button button)
  {
    // quit the application
    mApplication.Quit();

    return true;
  }

  /**
   * Signal handler, called when the 'Cancel' button in the quit-popup is touched.
   *
   * @param[in] button The button that was pressed.
   */
  bool OnPopupCancel(Button button)
  {
    mQuitPopup.Hide();
    return true;
  }

  /**
   * Signal handler called when the user touches outside of Dialog.
   */
  void OnPopupTouchedOutside()
  {
    mInfoPopup.Hide();
    mQuitPopup.Hide();
  }

  /**
   * Signal handler, called when the 'Ok' button in the quit-popup is touched.
   *
   * @param[in] button The button that was pressed.
   */
  bool OnSmallButtonPopupOk(Button button)
  {
    mSmallImagePopup.Hide();
    return true;
  }

  /**
   * Signal handler, called when the 'Cancel' button in the quit-popup is touched.
   *
   * @param[in] button The button that was pressed.
   */
  bool OnSmallButtonPopupCancel(Button button)
  {
    mSmallImagePopup.Hide();
    return true;
  }

  /**
   * Signal handler called when the user touches outside of Dialog.
   */
  void OnSmallButtonPopupTouchedOutside()
  {
    mSmallImagePopup.Hide();
  }

  /**
   * Signal handler, called when the 'Touch Me!' button in the center of the screen is touched.
   *
   * @param[in] button The button that was pressed.
   */
  bool OnTouchMeTouched(Button button)
  {
    mInfoPopup.Show();
    return true;
  }

  /**
   * Signal handler, called when the 'Touch Me!' button in the center of the screen is touched.
   *
   * @param[in] button The button that was pressed.
   */
  bool OnSmallImageButtonTouched(Button button)
  {
    mSmallImagePopup.Show();
    return true;
  }

  /**
   * Signal handler, called when the 'Quit' button in the bottom right corner is touched.
   *
   * @param[in] button The button that was pressed.
   */
  bool OnQuitTouched(Button button)
  {
    mQuitPopup.Show();
    return true;
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mQuitPopup.Show();
      }
    }
  }

private:

  Application& mApplication;                ///< Application instance
  Toolkit::View mView;                      ///< The view of the app
  Layer mContents;                          ///< The main content layer
  Toolkit::ToolBar mToolBar;                ///< The View's Toolbar.

  PushButton mQuitButton;                   ///< Quit button
  Layer mPopupLayer;                        ///< Popup layer (modal popup is on this layer)
  Popup mInfoPopup;                         ///< Info Popup (opened when Press Me is touched)
  Popup mQuitPopup;                         ///< Quit popup (opened when user attempts to quit by pressing 'Quit')
  Popup mSmallImagePopup;                   ///< Popup to show small image
};

void RunTest(Application& app)
{
  PopupController test(app);

  app.MainLoop();
}

int main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv);

  RunTest(app);

  return 0;
}
