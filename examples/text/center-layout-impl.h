#ifndef __DALI_DEMO_CENTER_LAYOUT_IMPL_H__
#define __DALI_DEMO_CENTER_LAYOUT_IMPL_H__

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

// INTERNAL INCLUDES
#include "center-layout.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class CenterLayout : public Control
{
public:
  /**
   * @copydoc Dali::Toollkit::TextLabel::New()
   */
  static Toolkit::CenterLayout New();

  CenterLayout();

  virtual ~CenterLayout();

  /**
   * @copydoc Control::OnInitialize()
   */
  virtual void OnInitialize();

  // Size negotiation methods inherited from Internal::Control

  /**
   * @copydoc Control::OnRelayout()
   */
  virtual void OnRelayout( const Vector2& size, ActorSizeContainer& container );

private:

  void OnPan( Actor actor, const PanGesture& gesture );

  // Undefined copy constructor and assignment operators
  CenterLayout(const CenterLayout&);
  CenterLayout& operator=(const CenterLayout& rhs);

private:

  Vector3 mLayoutSize;

  Actor mGrabCorner;

  PanGestureDetector mPanGestureDetector;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_DEMO_CENTER_LAYOUT_IMPL_H__
