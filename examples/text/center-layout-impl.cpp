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
#include "center-layout-impl.h"

// INTERNAL INCLUDES
#include "center-layout.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

Toolkit::CenterLayout CenterLayout::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< CenterLayout > impl = new CenterLayout();

  // Pass ownership to CustomActor handle
  Toolkit::CenterLayout handle( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

CenterLayout::CenterLayout()
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_NONE ) )
{
}

CenterLayout::~CenterLayout()
{
}

void CenterLayout::OnInitialize()
{
}

void CenterLayout::OnRelayout( const Vector2& size, ActorSizeContainer& container )
{
  if( Self().GetChildCount() > 0 )
  {
    Dali::Toolkit::Control child = Dali::Toolkit::Control::DownCast( Self().GetChildAt(0) );

    if( child )
    {
      child.SetY( -0.5f );
      child.SetParentOrigin( ParentOrigin::CENTER );
      child.SetAnchorPoint( AnchorPoint::CENTER );

      Vector3 naturalSize = child.GetNaturalSize();

      container.push_back( ActorSizePair(child, Vector2( std::min(naturalSize.width, size.width), naturalSize.height )) );
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
