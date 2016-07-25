#ifndef __DALI_TOOLKIT_CIRCULARNUMBER_H__
#define __DALI_TOOLKIT_CIRCULARNUMBER_H__

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

/**
 * @addtogroup CAPI_DALI_TOOLKIT_CIRCULARNUMBER_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Internal
{
class CustomActor;
}

namespace Toolkit
{

namespace Internal
{
class CircularNumber;
}

/**
 * @brief CircularNumber control renders curved number
 */
class DALI_IMPORT_API CircularNumber : public Control
{
public:

  /**
   * @brief Create an uninitialized CircularNumber.
   */
  CircularNumber();

  /**
   * @brief Copy constructor.
   */
  CircularNumber( const CircularNumber& CircularNumber );

  /**
   * @brief Create an initialized CircularNumber.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static CircularNumber New();

  /**
   * @brief Assignment operator.
   */
  CircularNumber& operator=( const CircularNumber& CircularNumber );

  /**
   * @brief Downcast an Object handle to CircularNumber.
   *
   * If handle points to a CircularNumber the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return handle to a CircularNumber or an uninitialized handle
   */
  static CircularNumber DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~CircularNumber();

public:

  /**
   * @brief Set number to be updated
   *
   * @param[in] num Number to be updated
   * @param[in] radius Radius of the circle
   * @param[in] duration Duration for effect to update number
   */
  void SetNumber( unsigned int num, float radius, float duration = 0.f );

  /**
   * @brief Set radius of the circle
   *
   * @param[in] radius Radius of the circle
   */
  void SetRadius( float radius );

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  CircularNumber( Internal::CircularNumber& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  CircularNumber( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_CIRCULARNUMBER_H__
