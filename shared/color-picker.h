#ifndef __DALI_DEMO_HELPER_COLOR_PICKER_H__
#define __DALI_DEMO_HELPER_COLOR_PICKER_H__

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
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;

namespace DemoHelper
{

namespace Internal
{
class ColorPicker;
}

/**
 * @brief ColorPicker is a control to select a color.
 *
 * @code
 * void OnColorPicked( DemoHelper::ColorPicker picker, const Vector4& color )
 * {
 *   cout << color << endl;
 * }
 * @endcode
 *
 * @code
 * void CreateColorPicker()
 * {
 *   DemoHelper::ColorPicker colorPicker = DemoHelper::ColorPicker::New();
 *   Stage::GetCurrent().Add( colorPicker );
 *
 *   colorPicker.ColorPickedSignal().Connect( this, &OnColorPicked );
 * }
 * @endcode
 */
class ColorPicker : public Toolkit::Control
{
public:

  /**
   * @brief The start and end property ranges for this control.
    */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Toolkit::Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices.
  };

  /**
   * @brief An enumeration of properties belonging to the ColorPicker class.
   */
  struct Property
  {
    enum
    {
      TEXT_COLOR = PROPERTY_START_INDEX, ///< name "textColor", The text color, type VECTOR4
    };
  };

  // Type Defs

  /// @brief Color picked signal type.
  typedef Signal<void ( ColorPicker, const Vector4& ) > ColorPickedSignalType;

  /**
   * @brief Create a ColorPicker control.
   * @return A handle to the ColorPicker control.
   */
  static DemoHelper::ColorPicker New();

  /**
   * @brief Creates an empty ColorPicker handle.
   */
  ColorPicker();

  /**
   * @brief Copy constructor.
   * @param[in] handle The handle to copy from.
  */
  ColorPicker( const ColorPicker& handle );

  /**
   * @brief Assignment operator.
   * @param[in] handle The handle to copy from.
   * @return A reference to this.
   */
  ColorPicker& operator=( const ColorPicker& handle );

  /**
   * @brief Destructor.
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
   ~ColorPicker();

  /**
   * @brief Downcast a handle to ColorPicker.
   *
   * If the BaseHandle points is a ColorPicker the downcast returns a valid handle.
   * If not the returned handle is left empty.
   *
   * @param[in] handle Handle to an object.
   * @return handle to a ColorPicker or an empty handle.
   */
  static ColorPicker DownCast( BaseHandle handle );

  // Signals

  /**
   * @brief This signal is emitted when a color is picked.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( ColorPicker colorPicker, const Vector4& color );
   * @endcode
   * @return The signal to connect to.
   */
  ColorPickedSignalType& ColorPickedSignal();

public: // not intended for application developers
  ColorPicker(Internal::ColorPicker&);
  explicit ColorPicker(Dali::Internal::CustomActor*);
};

} // namespace DemoHelper

#endif // __DALI_DEMO_HELPER_COLOR_PICKER_H__


