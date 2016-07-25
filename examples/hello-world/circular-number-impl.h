#ifndef __DALI_TOOLKIT_INTERNAL_CIRCULARNUMBER_H__
#define __DALI_TOOLKIT_INTERNAL_CIRCULARNUMBER_H__

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

// EXTERNAL INCLUDES
#include <vector>
#include <string>
#include <sstream>

// INTERNAL INCLUDES
#include <dali/dali.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

#include "circular-number.h"

#define MAX_NUM 10

namespace Dali
{

namespace Toolkit
{

class CircularNumber;

namespace Internal
{

/**
 * CircularNumber is the base class implementation for all CircularNumbers.
 */
class CircularNumber : public Control
{
public:

  /**
   * Create a new PushButton.
   * @return A smart-pointer to the newly allocated PushButton.
   */
  static Dali::Toolkit::CircularNumber New();

protected:

  /**
   * Construct a new CircularNumber.
   */
  CircularNumber();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~CircularNumber();

private:

  /**
   * Touch event handler to rotate the CircularNumber
   *
   * @param[in] actor   actor which consumes pan gesture
   * @param[in] event   information of touch event
   * @return which indicates if touch event is consumed
   */
  bool OnTouched(Actor actor, const TouchEvent& event);

  /**
   * Pan gesture handler to animate rotation of the CircularNumber
   *
   * @param[in] actor   actor which consumes pan gesture
   * @param[in] gesture handle of pan gesture
   */
  void OnPan(Actor actor, PanGesture gesture);

public:

  /**
   * @brief Called if number needs to be updated
   */
  bool OnTick();

  /**
   * @copydoc Toolkit::CircularNumber::SetNumber()
   */
  void SetNumber( unsigned int num, float radius, float duration );

  /**
   * @copydoc Toolkit::CircularNumber::SetRadius()
   */
  void SetRadius( float radius );

private:

  /**
   * @brief Relayout number actors
   */
  void RelayoutNumbers();

protected: // From CustomActorImpl

  /**
   * @copydoc Dali::CustomActorImpl::OnTouchEvent( const TouchEvent& event )
   */
  virtual bool OnTouchEvent( const TouchEvent& event );

private: // From Control

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Toolkit::Control::OnControlStageConnection()
   */
  virtual void OnControlStageConnection();

  /**
   * @copydoc Toolkit::Control::OnControlStageDisconnection()
   */
  virtual void OnControlStageDisconnection();

  /**
   * @copydoc Toolkit::Control::OnControlChildAdd()
   */
  virtual void OnControlChildAdd( Actor& child );

  /**
   * @copydoc Toolkit::Control::OnControlChildRemove( Actor& child )
   */
  virtual void OnControlChildRemove( Actor& child );

  /**
   * @copydoc Toolkit::Control::OnControlSizeSet( const Vector3& targetSize )
   */
  virtual void OnControlSizeSet( const Vector3& targetSize );

private:

  // Undefined
  CircularNumber( const CircularNumber& );

  // Undefined
  CircularNumber& operator = ( const CircularNumber& );

protected: // Signals

private:
  unsigned int  mTargetNumber;          ///< target number to be updated
  float         mTargetRadius;          ///< target radius to be updated
  float         mCurrentNumber;         ///< current number
  float         mCurrentRadius;         ///< current radius
  float         mDeltaNumber;           ///< delta number
  float         mDeltaRadius;           ///< delta radius
  float         mEffectDuration;        ///< effect duration
  float         mEffectInterval;        ///< effect interval

  std::string   mStrNumber;

  ResourceImage   mNumImages[MAX_NUM];
  ImageDimensions mNumImageSizes[MAX_NUM];

  std::vector<ImageActor> mActors;

  float         mRadius;                ///< radius of circle layout

  Timer         mTimer;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::CircularNumber& GetImplementation( Toolkit::CircularNumber& CircularNumber )
{
  DALI_ASSERT_ALWAYS( CircularNumber );

  Dali::RefObject& handle = CircularNumber.GetImplementation();

  return static_cast<Toolkit::Internal::CircularNumber&>( handle );
}

inline const Toolkit::Internal::CircularNumber& GetImplementation( const Toolkit::CircularNumber& CircularNumber )
{
  DALI_ASSERT_ALWAYS( CircularNumber );

  const Dali::RefObject& handle = CircularNumber.GetImplementation();

  return static_cast<const Toolkit::Internal::CircularNumber&>( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_CIRCULARNUMBER_H__

