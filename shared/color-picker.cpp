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
#include "color-picker.h"

// INTERNAL INCLUDES
#include "color-picker-impl.h"

namespace DemoHelper
{

DemoHelper::ColorPicker ColorPicker::New()
{
  return ColorPicker();
}

ColorPicker::ColorPicker()
{
}

ColorPicker::ColorPicker( const ColorPicker& handle )
: Control( handle )
{
}

ColorPicker& ColorPicker::operator=( const ColorPicker& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

ColorPicker::~ColorPicker()
{
}

ColorPicker ColorPicker::DownCast( BaseHandle handle )
{
  return Control::DownCast<ColorPicker, Internal::ColorPicker>(handle);
}

ColorPicker::ColorPickedSignalType& ColorPicker::ColorPickedSignal()
{
  return DemoHelper::GetImpl( *this ).ColorPickedSignal();
}

ColorPicker::ColorPicker( Internal::ColorPicker& implementation )
: Control(implementation)
{
}

ColorPicker::ColorPicker( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::ColorPicker>( internal );
}

} // namespace DemoHelper
