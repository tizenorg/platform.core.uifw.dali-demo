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
  if( !mGrabCorner )
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    // Resize the center layout when the corner is grabbed
    //mGrabCorner = Actor::New();
    mGrabCorner = CreateSolidColorActor( Color::GREEN );
    mGrabCorner.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
    mGrabCorner.SetParentOrigin( ParentOrigin::BOTTOM_RIGHT );
    mGrabCorner.SetSize( stageSize.width*0.2f, stageSize.width*0.2f );
    mGrabCorner.SetZ(1.0f);
    Self().Add( mGrabCorner );

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach( mGrabCorner );
    //mPanGestureDetector.Attach( Self() );
    mPanGestureDetector.DetectedSignal().Connect( this, &CenterLayout::OnPan );
  }
}

void CenterLayout::OnRelayout( const Vector2& size, ActorSizeContainer& container )
{
  CustomActor self = Self();

  mLayoutSize = size;

  for( unsigned int i=0; i<self.GetChildCount(); ++i )
  {
    Dali::Toolkit::Control child = Dali::Toolkit::Control::DownCast( self.GetChildAt(i) );

    if( child )
    {
      child.SetParentOrigin( ParentOrigin::CENTER );
      child.SetAnchorPoint( AnchorPoint::CENTER );

      container.push_back( ActorSizePair( child, size ) );
    }
  }
}

void CenterLayout::OnPan( Actor actor, const PanGesture& gesture )
{
  std::cout << "gesture.displacement: " << gesture.displacement << std::endl;

  mLayoutSize.x += gesture.displacement.x;
  mLayoutSize.y += gesture.displacement.y;
  Self().SetSize( mLayoutSize );

  RelayoutRequest();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
