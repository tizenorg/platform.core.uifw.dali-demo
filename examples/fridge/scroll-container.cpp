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

#include "scroll-container.h"

#include <math.h>

using namespace Dali;
using namespace Dali::Toolkit;

ScrollContainer::ScrollContainer()
: mItemSize(Vector3::ZERO),
  mPadding(0.0f),
  mCurrentScrollPosition(0.0f),
  mItemCount(0),
  mScrollCount(0),
  mBaseItem(0),
  mFocusedItem(0),
  mScrollDisplacement(0.0f),
  mIsFocused(false)
{
}

ScrollContainer::~ScrollContainer()
{
}

void ScrollContainer::Initialize(Vector3 itemSize)
{
  mContainer = Actor::New();
  mItemRoot = Actor::New();
  mContainer.Add(mItemRoot);

  if ( !mPanGestureDetector )
  {
    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach(mItemRoot);
    mPanGestureDetector.DetectedSignal().Connect( this, &ScrollContainer::OnPan );
  }

  mItemSize = itemSize;
}

Vector3 ScrollContainer::GetItemPosition( unsigned int itemId, float scrollPosition )
{
  return Vector3(mItemSize.x * itemId + mPadding * (itemId + 1) + scrollPosition, 0.0, 0.0);
}

unsigned int ScrollContainer::GetFirstVisibleItemId()
{
  return floor(-1.0 * mCurrentScrollPosition / mItemSize.x);
}

unsigned int ScrollContainer::GetLastVisibleItemId()
{
  return ceil( (Stage::GetCurrent().GetSize().x - mCurrentScrollPosition) / mItemSize.x - 1 );
}

void ScrollContainer::AddItem(Actor item)
{
  item.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  item.SetParentOrigin( ParentOrigin::TOP_LEFT );

  item.SetSize(mItemSize);
  item.SetKeyboardFocusable(true);
  item.HoveredSignal().Connect( this, &ScrollContainer::OnHovered );
  item.SetPosition( GetItemPosition(mItemCount, mCurrentScrollPosition) );

  mItemRoot.Add( item );
  mItemList.push_back(item);
  mPanGestureDetector.Attach(item);
  mItemCount++;
}

void ScrollContainer::Show(float duration, float delay, float itemDelay)
{
  float progress = 0.0;
  if(mHideAnimation)
  {
    progress = mHideAnimation.GetCurrentProgress();
    if( progress > 0.0f )
      progress = 1.0f - progress;

    mHideAnimation.Stop();
  }

  Stage stage( Stage::GetCurrent() );
  const Vector2 stageSize = stage.GetSize();

  mItemRoot.SetPosition(Vector3::ZERO);
  unsigned int numberOfItemsVisibleOnScreen = stageSize.x / mItemSize.x;

  mShowAnimation = Animation::New(duration);

  KeyFrames keyframes = KeyFrames::New();
  keyframes.Add( 0.0f, mItemRoot.GetCurrentPosition().y);
  keyframes.Add( 1.0f, 0.0f );

  for( unsigned int i = 0; i < mItemList.size(); ++i )
  {
    float d = delay;
    if( i < numberOfItemsVisibleOnScreen )
    {
      d += itemDelay * i;
    }

    mShowAnimation.AnimateBetween( Property(mItemList[i], Actor::Property::POSITION_Y), keyframes, AlphaFunction::EASE_IN_OUT_SINE, TimePeriod(d, duration));
    mShowAnimation.AnimateTo(Property(mItemList[i], Actor::Property::COLOR_ALPHA), 1.0f, AlphaFunction::EASE_IN_OUT_SINE, TimePeriod(d, duration) );
  }

  mShowAnimation.PlayFrom(progress);
}

void ScrollContainer::Hide(float translation, float duration, float delay)
{
  float progress = 0.0;
  if(mShowAnimation)
  {
    progress = mShowAnimation.GetCurrentProgress();
    if( progress > 0.0f )
      progress = 1.0f - progress;

    mShowAnimation.Stop();
  }

  mHideAnimation = Animation::New(duration);
  mHideAnimation.AnimateTo(Property(mItemRoot, Actor::Property::POSITION_Y), translation, AlphaFunction::EASE_IN_OUT_SINE, TimePeriod(delay, duration) );
  for( unsigned int i = 0; i < mItemList.size(); ++i )
  {
    mHideAnimation.AnimateTo(Property(mItemList[i], Actor::Property::COLOR_ALPHA), 0.0f, AlphaFunction::EASE_IN_OUT_SINE, TimePeriod(delay, duration) );
  }

  mHideAnimation.PlayFrom(progress);
}

void ScrollContainer::Expand(float translation, Vector3 targetViewSize, Vector3 targetItemSize, float duration, float delay)
{
  float progress = 0.0;
  if(mExpandAnimation)
  {
    progress = mExpandAnimation.GetCurrentProgress();
    if( progress > 0.0f )
      progress = 1.0f - progress;

    mExpandAnimation.Stop();
  }

  mExpandAnimation = Animation::New(duration + delay);

  Vector3 scale(targetItemSize.x / mItemSize.x, targetItemSize.y / mItemSize.y, 1.0f);
  for( unsigned int i = 0; i < mItemList.size(); ++i )
  {
    mExpandAnimation.AnimateTo(Property(mItemList[i], Actor::Property::SCALE), scale, AlphaFunction::EASE_IN_OUT_SINE, TimePeriod(delay, duration) );
  }

  mExpandAnimation.AnimateTo(Property(mItemRoot, Actor::Property::SIZE), targetViewSize, AlphaFunction::EASE_IN_OUT_SINE, TimePeriod(delay, duration) );
  mExpandAnimation.AnimateTo(Property(mItemRoot, Actor::Property::POSITION_Y), translation, AlphaFunction::EASE_IN_OUT_SINE, TimePeriod(delay, duration) );

  mExpandAnimation.PlayFrom(progress);
}

void ScrollContainer::Scroll(float amount, float duration, float itemDelay, unsigned int baseItem)
{
  // Clamp the target scroll position
  float tagetScrollPosition = mCurrentScrollPosition + amount;
  float totalItemSize = mItemList.size() * mItemSize.x;

  Stage stage( Stage::GetCurrent() );
  const Vector2 stageSize = stage.GetSize();
  float maxScrollPosition = stageSize.x - totalItemSize;
  if( tagetScrollPosition < maxScrollPosition)
  {
    tagetScrollPosition = maxScrollPosition;
  }
  if( tagetScrollPosition > 0.0f )
  {
    tagetScrollPosition = 0.0f;
  }

  ClearScrollAnimation();

  Animation animation = Animation::New(duration);
  mScrollAnimationList.push_back(animation);

  if(duration > 0)
  {
    for( unsigned int i = 0; i < mItemList.size(); ++i )
    {
      Vector3 targetPosition = GetItemPosition(i, tagetScrollPosition);

      float delay = 0.0f;
      if( amount < 0.0 && i >= baseItem )
      {
        delay = (i - baseItem) * itemDelay;
      }
      else if( amount > 0.0 && i <= baseItem )
      {
        delay = (baseItem- i)*itemDelay;
      }

      animation.AnimateTo(Property(mItemList[i], Actor::Property::POSITION), targetPosition, AlphaFunction::EASE_OUT_SINE, TimePeriod(delay, duration) );
    }

    mCurrentScrollPosition = tagetScrollPosition;
    animation.Play();
  }
  else
  {
    for( unsigned int i = 0; i < mItemList.size(); ++i )
    {
      Vector3 targetPosition = GetItemPosition(i, tagetScrollPosition);
      mItemList[i].SetPosition(targetPosition);
    }
    mCurrentScrollPosition = tagetScrollPosition;
  }

  mScrollCount++;
}

void ScrollContainer::ClearScrollAnimation()
{
  for( unsigned int i = 0; i < mScrollAnimationList.size(); ++i )
  {
    if(mScrollAnimationList[i])
    {
      mScrollAnimationList[i].Stop();
      mScrollAnimationList[i].Clear();
    }
  }

  mScrollAnimationList.clear();
  mScrollCount = 0;
}

Actor ScrollContainer::FocusPrevious()
{
  if( mFocusedItem < GetFirstVisibleItemId() || mFocusedItem > GetLastVisibleItemId() )
  {
    mFocusedItem = GetFirstVisibleItemId();
  }
  else if( mFocusedItem > 0 )
  {
    mFocusedItem--;
  }

  return Focus(mFocusedItem, 0.2f, 0.0f);
}

Actor ScrollContainer::FocusNext()
{
  if( mFocusedItem < GetFirstVisibleItemId() || mFocusedItem > GetLastVisibleItemId() )
  {
    mFocusedItem = GetFirstVisibleItemId();
  }
  else if( mFocusedItem < mItemList.size() - 1)
  {
    mFocusedItem++;
  }
  return Focus(mFocusedItem, 0.2f, 0.0f);
}

Actor ScrollContainer::Focus(unsigned int itemId, float duration, float delay)
{
  if( itemId < 0 )
  {
    itemId = 0;
  }
  else if( itemId >= mItemList.size() )
  {
    itemId = mItemList.size() - 1;
  }

  mIsFocused = true;
  mFocusedItem = itemId;
  Vector3 itemPosition = GetItemPosition(mFocusedItem, mCurrentScrollPosition);
  const Vector2 stageSize = Stage::GetCurrent().GetSize();
  if( itemPosition.x < 0.0f )
  {
    Scroll( -itemPosition.x, 0.3f, 0.05f, itemId + 1 );
  }
  else if( itemPosition.x + mItemSize.x > stageSize.x )
  {
    float amount = mItemSize.x - (stageSize.x - itemPosition.x);
    Scroll( -amount, 0.3f, 0.05f, itemId - 1 );
  }
  return mItemList[mFocusedItem];
}

void ScrollContainer::OnPan( Actor actor, const PanGesture& panGesture )
{
  switch( panGesture.state )
  {
    case Gesture::Started:
    {
      bool found = false;
      for( unsigned int i = 0; i < mItemList.size(); ++i )
      {
        if( actor == mItemList[i] )
        {
          mBaseItem = i;
          found = true;
          break;
        }
      }
      if( found == false )
      {
        mBaseItem = mFocusedItem;
      }
      mScrollDisplacement = 0.0;
      break;
    }
    case Gesture::Continuing:
    {
      mScrollDisplacement = panGesture.displacement.x;
      break;
    }
    case Gesture::Finished:
    case Gesture::Cancelled:
    {
      float absScrollDistance = mScrollDisplacement;
      if(absScrollDistance < 0)
        absScrollDistance = 0 - absScrollDistance;

      float scrollSpeed = panGesture.velocity.x * panGesture.velocity.x + panGesture.velocity.y * panGesture.velocity.y;
      float maxScrollSpeed = 40.0;  // TBD
      if(scrollSpeed > maxScrollSpeed)
        scrollSpeed = maxScrollSpeed;

      if (absScrollDistance > 1.0 && scrollSpeed > 0.05) // Threshold TBD
      {
        Stage stage( Stage::GetCurrent() );
        const Vector2 stageSize = stage.GetSize();

        if(mScrollDisplacement > 0.0) // scroll constant distance in constant speed.
        {
          Scroll(stageSize.x * 0.3f, 0.5f, 0.05f, GetFirstVisibleItemId());
        }
        else
        {
          Scroll(-stageSize.x * 0.3, 0.5, 0.05, GetLastVisibleItemId());
        }
      }
      break;
    }
    default:
      break;
  }
}

bool ScrollContainer::OnHovered( Actor actor, const HoverEvent& event )
{
  return true;
}
