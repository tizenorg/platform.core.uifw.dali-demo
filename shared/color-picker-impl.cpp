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
#include "color-picker-impl.h"

// EXTERNAL INCLUDES
#include <string.h>

namespace DemoHelper
{

namespace Internal
{

namespace
{

// Type registration
BaseHandle Create()
{
  return DemoHelper::ColorPicker::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( DemoHelper::ColorPicker, Dali::Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( DemoHelper, ColorPicker, "colorPicker", VECTOR4, TEXT_COLOR )

DALI_SIGNAL_REGISTRATION( DemoHelper, ColorPicker, "colorPicker", SIGNAL_COLOR_PICKED )

DALI_TYPE_REGISTRATION_END()

} // unnamed namespace

DemoHelper::ColorPicker ColorPicker::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< ColorPicker > impl = new ColorPicker();

  // Pass ownership to CustomActor handle
  DemoHelper::ColorPicker handle( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

void ColorPicker::SetProperty( BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value )
{
  DemoHelper::ColorPicker colorPicker = DemoHelper::ColorPicker::DownCast( Dali::BaseHandle( object ) );

  if( colorPicker )
  {
    ColorPicker& impl = GetImpl( colorPicker );

    switch( index )
    {
      case DemoHelper::ColorPicker::Property::TEXT_COLOR:
      {
        const Vector4 textColor = value.Get< Vector4 >();
        impl.SetTextColor( textColor );
        break;
      }
    }
  }
}

Dali::Property::Value ColorPicker::GetProperty( BaseObject* object, Dali::Property::Index index )
{
  Property::Value value;

  DemoHelper::ColorPicker colorPicker = DemoHelper::ColorPicker::DownCast( Dali::BaseHandle( object ) );

  if( colorPicker )
  {
    ColorPicker& impl = GetImpl( colorPicker );

    switch( index )
    {
      case DemoHelper::ColorPicker::Property::TEXT_COLOR:
      {
        value = impl.GetTextColor();
        break;
      }
    }    
  }

  return value;
}

bool ColorPicker::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  DemoHelper::ColorPicker colorPicker = DemoHelper::ColorPicker::DownCast( handle );

  if( 0 == strcmp( signalName.c_str(), SIGNAL_COLOR_PICKED ) )
  {
    colorPicker.ColorPickedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void ColorPicker::SetTextColor( const Vector4& color )
{
  mTextColor = color;
}

const Vector4& ColorPicker::GetTextColor() const
{
  return mTextColor;
}

DemoHelper::ColorPicker::ColorPickedSignalType& ColorPicker::ColorPickedSignal()
{
  return mColorPickedSignal;
}

void ColorPicker::OnInitialize()
{
}

ColorPicker::ColorPicker()
: Control( ControlBehaviour( REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mTextColor( Color::WHITE )
{
}

ColorPicker::~ColorPicker()
{
}

} // namespace Internal

} // namespace DemoHelper
