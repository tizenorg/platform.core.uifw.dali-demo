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
 *
 */

// CLASS HEADER
#include "circular-number-impl.h"

#include <stdio.h>
#include <iostream>

#include <dali/integration-api/debug.h>
#include <dali-toolkit/dali-toolkit.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

#define TIMER_INTERVAL 33

const char* NUM_IMAGES[MAX_NUM] = {
  DEMO_IMAGE_DIR "0.png",
  DEMO_IMAGE_DIR "1.png",
  DEMO_IMAGE_DIR "2.png",
  DEMO_IMAGE_DIR "3.png",
  DEMO_IMAGE_DIR "4.png",
  DEMO_IMAGE_DIR "5.png",
  DEMO_IMAGE_DIR "6.png",
  DEMO_IMAGE_DIR "7.png",
  DEMO_IMAGE_DIR "8.png",
  DEMO_IMAGE_DIR "9.png"
};

BaseHandle Create()
{
  // empty handle as we cannot create CircularNumber (but type registered for clicked signal)
  return BaseHandle();
}

TypeRegistration typeRegistration(typeid(Toolkit::CircularNumber), typeid(Toolkit::Control), Create);

} // unnamed namespace

Dali::Toolkit::CircularNumber CircularNumber::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<CircularNumber> internalCircularNumber = new CircularNumber();

  // Pass ownership to CustomActor
  Dali::Toolkit::CircularNumber CircularNumber(*internalCircularNumber);

  // Second-phase init of the implementation
  // This can only be done after the control has been made...
  internalCircularNumber->Initialize();

  return CircularNumber;
}


CircularNumber::CircularNumber()
: Control(ControlBehaviour(REQUIRES_TOUCH_EVENTS | REQUIRES_STYLE_CHANGE_SIGNALS))
, mTargetNumber( 0u )
, mCurrentNumber( 0.f )
, mCurrentImage( 0 )
{
}

CircularNumber::~CircularNumber()
{
}

bool CircularNumber::OnTick()
{
  bool keepTick = false;
  bool ignoreUpdate = false;

  Actor self = Self();

  // Timer is stopped if decreased duration is less than 0
  mEffectDuration -= mEffectInterval;
  if( mEffectDuration < 0.f )
  {
    mCurrentNumber = mTargetNumber;
    keepTick = false;
  }
  else
  {
    float updated = mCurrentNumber + mDeltaNumber;
    if ( int(updated) == int(mCurrentNumber) )
    {
      ignoreUpdate = true;
    }
    mCurrentNumber = updated;

    keepTick = true;
  }

  std::stringstream ss;
  ss << int(mCurrentNumber);
  std::string strNumber = ss.str();

  if( !ignoreUpdate )
  {
    int strLen    = strNumber.length();
    int vectorLen = mActors.size();
    int diffLen = strLen - vectorLen;

    if( diffLen > 0 )
    {
      for( int i = 0; i < diffLen; i++ )
      {
        ImageActor actor = ImageActor::New( mNumImages[0] );
        actor.SetParentOrigin( ParentOrigin::CENTER );
        actor.SetAnchorPoint( AnchorPoint::CENTER );
        self.Add( actor );

        mActors.push_back( actor );
      }
    }
    else if( diffLen < 0 )
    {
      diffLen = -diffLen;

      for( int i = 0; i < diffLen; i++ )
      {
        ImageActor actor = mActors.front();
        if( actor )
        {
          actor.Unparent();
          actor.Reset();
        }
        mActors.erase( mActors.begin() );
      }
    }

    float sumOfLen = 0.f;
    for( int i = 0; i < strLen-1; i++ )
    {
      sumOfLen += mNumImageSizes[i].GetWidth();
    }

    float xPos = sumOfLen * -0.5f;
    //OfLen[%.2f]\n", sumOfLen);
    for( int i = 0; i < strLen; i++ )
    {
      int num = strNumber[i] - '0';
      mActors[i].SetImage( mNumImages[num] );
      mActors[i].SetX( xPos );
      xPos += mNumImageSizes[i].GetWidth();
    }
  }

  return keepTick;
}

void CircularNumber::ChangeNumber( unsigned int num, float duration )
{
  mTargetNumber = num;

  printf("mTargetNumber[%d]\n", mTargetNumber);

  mEffectDuration = duration;
  mEffectInterval = float(TIMER_INTERVAL) * 0.001f;

  mDeltaNumber = ( mTargetNumber - mCurrentNumber ) / ( mEffectDuration / mEffectInterval );

  if ( mEffectDuration < mEffectInterval )
  {
    mEffectInterval = 0.f;
  }

  // stop previous timer and start new timer
  if( mTimer )
  {
    mTimer.TickSignal().Disconnect( this, &CircularNumber::OnTick );
    mTimer.Stop();
    mTimer.Reset();
  }
  mTimer = Timer::New( int(mEffectInterval * 1000.f) );
  mTimer.TickSignal().Connect( this, &CircularNumber::OnTick );
  mTimer.Start();

  //printf( "Start interval[%.3f]\n", mEffectInterval );
}

bool CircularNumber::OnTouchEvent(const TouchEvent& event)
{
  return false;
}

void CircularNumber::OnInitialize()
{
  Actor self = Self();

  for( int i = 0; i < MAX_NUM; i++ )
  {
    mNumImages[i]     = ResourceImage::New( NUM_IMAGES[i], ResourceImage::IMMEDIATE, Image::NEVER );
    mNumImageSizes[i] = ResourceImage::GetImageSize( NUM_IMAGES[i] );
  }

  ImageActor actor = ImageActor::New( mNumImages[mCurrentImage] );
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetAnchorPoint( AnchorPoint::CENTER );
  self.Add( actor );

  mActors.push_back( actor );
}

bool CircularNumber::OnTouched(Actor actor, const TouchEvent& event)
{
  TouchPoint touchPoint = event.GetPoint(0);

  if (TouchPoint::Down == touchPoint.state)
  {
  }

  return true;
}

void CircularNumber::OnPan(Actor actor, PanGesture gesture)
{
}

void CircularNumber::OnControlStageConnection()
{
}

void CircularNumber::OnControlStageDisconnection()
{
}

void CircularNumber::OnControlChildAdd( Actor& child )
{
}

void CircularNumber::OnControlChildRemove( Actor& child )
{
}

void CircularNumber::OnControlSizeSet(const Vector3& targetSize)
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
