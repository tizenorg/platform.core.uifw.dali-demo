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

#ifndef __SCROLL_CONTAINER_H__
#define __SCROLL_CONTAINER_H__

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

/**
 * Scroll container.
 *
 */
class ScrollContainer: public Dali::ConnectionTracker
{
public:

  /*
   * Constructor
   */
  ScrollContainer();

  /**
   * Virtual Destructor
   */
  virtual ~ScrollContainer();

  Dali::Actor GetContainer()
  {
    return mContainer;
  }

  Dali::Actor GetItemRoot()
  {
    return mItemRoot;
  }

  unsigned int GetFocusedItem()
  {
    return mFocusedItem;
  }

  Dali::Actor GetCurrentFocusedActor()
  {
    return mItemList[mFocusedItem];
  }

  bool IsFocused()
  {
    return mIsFocused;
  }

  int GetItemCount()
  {
    return mItemList.size();
  }

  void Initialize(Dali::Vector3 itemSize);

  Dali::Vector3 GetItemPosition( unsigned int itemId, float scrollPosition );
  unsigned int GetFirstVisibleItemId();
  unsigned int GetLastVisibleItemId();
  void AddItem( Dali::Actor item );

  void Show(float duration, float delay, float itemDelay);
  void Hide(float translation, float duration, float delay);
  void Expand(float translation, Dali::Vector3 targetViewSize, Dali::Vector3 targetItemSize, float duration, float delay);

  Dali::Actor FocusPrevious();
  Dali::Actor FocusNext();
  Dali::Actor Focus(unsigned int itemId, float duration, float delay);

  void Scroll(float amount, float duration, float itemDelay, unsigned int baseItem);
  void ClearScrollAnimation();

private:

  void OnPan( Dali::Actor actor, const Dali::PanGesture& gesture );
  bool OnHovered( Dali::Actor actor, const Dali::HoverEvent& event );

private:

  Dali::Actor mContainer;
  Dali::Actor mItemRoot;

  std::vector<Dali::Actor> mItemList;

  Dali::Animation mShowAnimation;
  Dali::Animation mHideAnimation;
  Dali::Animation mExpandAnimation;
  std::vector<Dali::Animation> mScrollAnimationList;

  Dali::PanGestureDetector mPanGestureDetector;

  Dali::Vector3 mItemSize;
  float mPadding;
  float mCurrentScrollPosition;
  int mItemCount;
  int mScrollCount;
  unsigned int mBaseItem;
  unsigned int mFocusedItem;
  float mScrollDisplacement;

  bool mIsFocused;
};

#endif // __SCROLL_CONTAINER_H__
