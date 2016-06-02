#ifndef __DALI_DEMO_HELPER_INTERNAL_COLOR_PICKER_H__
#define __DALI_DEMO_HELPER_INTERNAL_COLOR_PICKER_H__

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
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

// INTERNAL INCLUDES
#include "color-picker.h"

using namespace Dali;

namespace DemoHelper
{

namespace Internal
{

/**
 * @brief ColorPicker implementation class.
 */
class ColorPicker : public Toolkit::Internal::Control
{
public:

  /**
   * @copydoc DemoHelper::ColorPicker::New()
   */
  static DemoHelper::ColorPicker New();

  // Properties

  /**
   * @brief Called when a property of an object of this type is set.
   *
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value );

  /**
   * @brief Called to retrieve a property of an object of this type.
   *
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Dali::Property::Value GetProperty( BaseObject* object, Dali::Property::Index index );

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

  void SetTextColor( const Vector4& color );
  const Vector4& GetTextColor() const;

  /**
   * @copydoc DemoHelper::ColorPicker::ColorPickedSignal()
   */
  DemoHelper::ColorPicker::ColorPickedSignalType& ColorPickedSignal();

private: // From Control

  /**
   * @copydoc Control::OnInitialize()
   */
  virtual void OnInitialize();

private: // Implementation

  /**
   * Construct a new ColorPicker.
   */
  ColorPicker();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ColorPicker();

  // Undefined copy constructor and assignment operators
  ColorPicker(const ColorPicker&);
  ColorPicker& operator=(const ColorPicker& rhs);

private: // Data
  Vector4 mTextColor;

  // Signals
  DemoHelper::ColorPicker::ColorPickedSignalType mColorPickedSignal;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline DemoHelper::Internal::ColorPicker& GetImpl( DemoHelper::ColorPicker& colorPicker )
{
  DALI_ASSERT_ALWAYS( colorPicker );

  Dali::RefObject& handle = colorPicker.GetImplementation();

  return static_cast<DemoHelper::Internal::ColorPicker&>( handle );
}

inline const DemoHelper::Internal::ColorPicker& GetImpl( const DemoHelper::ColorPicker& colorPicker )
{
  DALI_ASSERT_ALWAYS( colorPicker );

  const Dali::RefObject& handle = colorPicker.GetImplementation();

  return static_cast<const DemoHelper::Internal::ColorPicker&>( handle );
}

} // namespace DemoHelper

#endif // __DALI_DEMO_HELPER_INTERNAL_COLOR_PICKER_H__
