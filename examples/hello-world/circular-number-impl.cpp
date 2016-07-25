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

#include <iostream>
#include <stdio.h>
#include <math.h>

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

const float PI = 3.141592f;

} // unnamed namespace

Dali::Toolkit::CircularNumber CircularNumber::New( bool reverse )
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<CircularNumber> internalCircularNumber = new CircularNumber();

  // Pass ownership to CustomActor
  Dali::Toolkit::CircularNumber CircularNumber(*internalCircularNumber);

  // Second-phase init of the implementation
  // This can only be done after the control has been made...
  internalCircularNumber->Initialize();
  if( reverse )
  {
    internalCircularNumber->SetReverse( reverse );
  }

  return CircularNumber;
}

CircularNumber::CircularNumber()
: Control(ControlBehaviour(REQUIRES_TOUCH_EVENTS | REQUIRES_STYLE_CHANGE_SIGNALS))
, mTargetNumber( 0u )
, mTargetRadius( 30.f )
, mCurrentNumber( 0.f )
, mCurrentRadius( 0.f )
, mDeltaNumber( 0.f )
, mDeltaRadius( 0.f )
, mEffectDuration( 0.f )
, mEffectInterval( 0.f )
, mReverse( false )
{
}

CircularNumber::~CircularNumber()
{
}

void CircularNumber::Update( unsigned int num, float radius, float duration )
{
  mTargetNumber = num;
  mTargetRadius = radius;

  printf( "mTargetNumber[%d] mTargetRadius[%.2f]\n", mTargetNumber, mTargetRadius );

  mEffectDuration = duration;
  mEffectInterval = float(TIMER_INTERVAL) * 0.001f;

  if( mEffectDuration == 0.f )
  {
    mDeltaNumber = mTargetNumber - mCurrentNumber;
    mDeltaRadius = mTargetRadius - mCurrentRadius;
  }
  else
  {
    mDeltaNumber = ( mTargetNumber - mCurrentNumber ) / ( mEffectDuration / mEffectInterval );
    mDeltaRadius = ( mTargetRadius - mCurrentRadius ) / ( mEffectDuration / mEffectInterval );
  }

  if ( mEffectDuration < mEffectInterval )
  {
    mEffectInterval = 0.f;
  }

  if (mEffectInterval == 0)
  {
    OnTick();
  }
  else
  {
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
  }
}

void CircularNumber::SetReverse( bool reverse )
{
  mReverse = reverse;

  Update( mTargetNumber, mTargetRadius, 0.f );
}

ShaderEffect CircularNumber::NewReverseEffect()
{
  std::string REVERSE_FSHADER = DALI_COMPOSE_SHADER(
    void main()                                                              \n
    {                                                                        \n
      mediump vec2 reverseCoord = vec2(1.0 - vTexCoord.x, 1.0 - vTexCoord.y);              \n
      gl_FragColor = texture2D(sTexture, reverseCoord) * uColor;             \n
    }                                                                        \n
  );

  // Create the implementation, temporarily owned on stack,
  Dali::ShaderEffect shaderEffect =  Dali::ShaderEffect::New(
    "",
    REVERSE_FSHADER,
    ShaderEffect::GeometryHints( ShaderEffect::HINT_GRID ) );

  return shaderEffect;
}

bool CircularNumber::OnTick()
{
  bool keepTick = false;
  bool ignoreUpdate = false;

  Actor self = Self();

  // Timer is stopped if decreased duration is less than 0
  mEffectDuration -= mEffectInterval;
  if( mEffectDuration <= 0.f )
  {
    mCurrentNumber = mTargetNumber;
    mCurrentRadius = mTargetRadius;
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

    mCurrentRadius += mDeltaRadius;
    keepTick = true;
  }

  std::stringstream ss;
  ss << int(mCurrentNumber);
  mStrNumber = ss.str();

  if( !ignoreUpdate )
  {
    int strLen    = mStrNumber.length();
    int vectorLen = mActors.size();
    int diffLen = strLen - vectorLen;

    // if number of actors needs to be increased
    if( diffLen > 0 )
    {
      for( int i = 0; i < diffLen; i++ )
      {
        ImageActor actor = ImageActor::New( mNumImages[0] );
        actor.SetParentOrigin( ParentOrigin::CENTER );
        actor.SetAnchorPoint( AnchorPoint::CENTER );
        self.Add( actor );

        Constraint cstrColor = Constraint::New<Vector4>( actor, Actor::Property::COLOR, EqualToConstraint() );
        cstrColor.AddSource( ParentSource( Actor::Property::COLOR ) );
        cstrColor.Apply();

        mActors.push_back( actor );
      }
    }
    // if number of actors needs to be decreased
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

    RelayoutNumbers();
  }

  return keepTick;
}

void CircularNumber::RelayoutNumbers()
{
  // Linear
#if 0
  float sumOfLen = 0.f;
  int   strLen   = mStrNumber.length();

  for( int i = 0; i < strLen-1; i++ )
  {
    sumOfLen += mNumImageSizes[i].GetWidth();
  }

  float xPos = sumOfLen * -0.5f;
  for( int i = 0; i < strLen; i++ )
  {
    int num = mStrNumber[i] - '0';
    mActors[i].SetImage( mNumImages[num] );
    mActors[i].SetX( xPos );
    xPos += mNumImageSizes[i].GetWidth();
  }
#endif

  // Circular
#if 1
  float sumOfAngle = 0.f;
  int   strLen   = mStrNumber.length();

  for( int i = 0; i < strLen-1; i++ )
  {
    sumOfAngle += atanf(mNumImageSizes[i].GetWidth() / mCurrentRadius) / PI * 180.f;
  }

  float sign = mReverse ? -1.f : 1.f;
  float angle = (sumOfAngle * -0.5f) * sign;
  for( int i = 0; i < strLen; i++ )
  {
    int num = mStrNumber[i] - '0';
    mActors[i].SetImage( mNumImages[num] );

    float anchorHeight = 1.f + mCurrentRadius / mNumImageSizes[i].GetHeight();
    mActors[i].SetAnchorPoint( Vector3(0.5f, anchorHeight, 0.5f) );

    //printf( "num[%d] anchorHeight[%.2f] angle[%.2f]\n", num, anchorHeight, angle );
    mActors[i].SetOrientation( Degree(angle), Vector3::ZAXIS );
    angle += (atanf(mNumImageSizes[i].GetWidth() / mCurrentRadius) / PI * 180.f) * sign;

    if( mReverse )
    {
      mActors[i].SetShaderEffect( mReverseShaderEffect );
    }
    else
    {
      mActors[i].RemoveShaderEffect();
    }
  }
#endif
}

bool CircularNumber::OnTouchEvent(const TouchEvent& event)
{
  return false;
}

void CircularNumber::OnInitialize()
{
  Actor self = Self();

  mReverseShaderEffect = NewReverseEffect();

  for( int i = 0; i < MAX_NUM; i++ )
  {
    mNumImages[i]     = ResourceImage::New( NUM_IMAGES[i], ResourceImage::IMMEDIATE, Image::NEVER );
    mNumImageSizes[i] = ResourceImage::GetImageSize( NUM_IMAGES[i] );
  }

  Update( mTargetNumber, mTargetRadius, 0.f );
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
