#ifndef __FRIDGE_APP_H__
#define __FRIDGE_APP_H__

//
// Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/builder/builder.h>
#include "scroll-container.h"

class FridgeApp: public Dali::ConnectionTracker
{
public:

  FridgeApp( Dali::Application application );

  ~FridgeApp();

  void OnInitialize( Dali::Application& application );
  void OnTerminate( Dali::Application& application );
  void OnPause( Dali::Application& application );
  void OnResume( Dali::Application& application );

  void MainLoop();

private:

  void CreateBackground();
  void CreatePosters();
  void CreateMenu();
  void CreateTopPanel();

  Dali::Actor CreateMenuItem(std::string iconName, std::string title);
  void CreateMenuBackground();

  void ShowTopPanel(float duration, float delay);
  void HideTopPanel(float duration, float delay);
  void ShowMenuBackground(float duration, float delay);
  void HideMenuBackground(float duration, float delay);
  void ShowMenu();
  void HideMenu();

  void OnKeyboardEvent(const Dali::KeyEvent& event);
  Dali::Actor OnKeyboardPreFocusChange( Dali::Actor current, Dali::Actor proposed, Dali::Toolkit::Control::KeyboardFocus::Direction direction );

  bool OnArrowButtonTouched(Dali::Actor indicator, const Dali::TouchEvent& touchEvent);

  void OnQuit( Dali::PropertyNotification& source );

private:

  Dali::Application mApplication;
  Dali::Toolkit::Builder  mBuilder;

  ScrollContainer mPosters;
  ScrollContainer mMenu;
  Dali::Toolkit::ImageView mTopPanel;
  Dali::Toolkit::ImageView mMenuBackground;
  Dali::Toolkit::ImageView mArrowButton;

  bool mIsMenuVisible;
  bool mIsMenuBackgroundVisible;
  bool mIsFocusInPosters;
  Dali::Vector3 mPosterItemSize;
  Dali::Vector3 mMenuItemSize;

  Dali::Animation mTopPanelShowAnimation;
  Dali::Animation mTopPanelHideAnimation;
  Dali::Animation mMenuShowAnimation;
  Dali::Animation mMenuHideAnimation;
  Dali::Animation mMenuBackgroundShowAnimation;
  Dali::Animation mMenuBackgroundHideAnimation;
};

#endif // __FRIDGE_APP_H__
