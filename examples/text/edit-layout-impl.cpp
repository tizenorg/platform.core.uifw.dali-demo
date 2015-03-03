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

// CLASS HEADER
#include "edit-layout-impl.h"

// INTERNAL INCLUDES
#include "edit-layout.h"

namespace
{
}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

Toolkit::EditLayout EditLayout::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< EditLayout > impl = new EditLayout();

  // Pass ownership to CustomActor handle
  Toolkit::EditLayout handle( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

EditLayout::EditLayout()
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_NONE ) )
{
}

EditLayout::~EditLayout()
{
}

void EditLayout::OnInitialize()
{
}

float EditLayout::GetHeightForWidth( float width )
{
  float height( 0.0f );

  CustomActor self = Self();

  if( self.GetChildCount() > 0 )
  {
    Dali::Toolkit::Control child = Dali::Toolkit::Control::DownCast( self.GetChildAt(0) );

    if( child )
    {
      height = child.GetHeightForWidth( width );
    }
  }

  return height;
}

void EditLayout::OnRelayout( const Vector2& size, ActorSizeContainer& container )
{
  CustomActor self = Self();

  if( self.GetChildCount() > 0 )
  {
    Dali::Toolkit::Control child = Dali::Toolkit::Control::DownCast( self.GetChildAt(0) );

    if( child )
    {
      child.SetParentOrigin( ParentOrigin::CENTER );
      child.SetAnchorPoint( AnchorPoint::CENTER );

      container.push_back( ActorSizePair( child, size ) );
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
