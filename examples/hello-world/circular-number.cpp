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

#include "circular-number.h"

// INTERNAL INCLUDES

#include "circular-number-impl.h"

namespace Dali
{

namespace Toolkit
{

const float PI = 3.141592f;

CircularNumber::CircularNumber()
{
}

CircularNumber::CircularNumber( const CircularNumber& CircularNumber )
: Control( CircularNumber )
{
}

CircularNumber CircularNumber::New()
{
  return Internal::CircularNumber::New();
}

CircularNumber& CircularNumber::operator=( const CircularNumber& CircularNumber )
{
  if( &CircularNumber != this )
  {
    Control::operator=( CircularNumber );
  }
  return *this;
}

CircularNumber::~CircularNumber()
{
}

CircularNumber CircularNumber::DownCast( BaseHandle handle )
{
  return Control::DownCast<CircularNumber, Internal::CircularNumber>(handle);
}

CircularNumber::CircularNumber( Internal::CircularNumber& implementation )
: Control( implementation )
{
}

CircularNumber::CircularNumber( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::CircularNumber>(internal);
}

void CircularNumber::ChangeNumber( unsigned int num, float duration )
{
  Dali::Toolkit::GetImplementation(*this).ChangeNumber( num, duration );
}

} // namespace Toolkit

} // namespace Dali
