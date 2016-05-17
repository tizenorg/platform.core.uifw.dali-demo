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

#include "scene-graph-editor-impl.h"

#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/rendering/renderer.h>
#include <dali/public-api/object/type-registry-helper.h>

#include <sstream>
//todor
#include <iostream>


namespace Demo
{

namespace Internal
{

namespace
{

// Setup properties, signals and actions using the type-registry.

#if 1
DALI_TYPE_REGISTRATION_BEGIN( Demo::SceneGraphEditor, Dali::Toolkit::Control, NULL )

DALI_PROPERTY_REGISTRATION( Demo, SceneGraphEditor, "testProperty", INTEGER, TEST_PROPERTY )

DALI_TYPE_REGISTRATION_END()
#endif


//const char* MATERIAL_SAMPLE( DEMO_IMAGE_DIR "gallery-small-48.jpg" );

#define MAKE_SHADER(A)#A

const char* VERTEX_SHADER = MAKE_SHADER(
attribute mediump vec2    aPosition1;
attribute mediump vec2    aPosition2;
uniform   mediump mat4    uMvpMatrix;
uniform   mediump vec3    uSize;
uniform   mediump float   uMorphAmount;

void main()
{
  mediump vec2 morphPosition = mix(aPosition1, aPosition2, uMorphAmount);
  mediump vec4 vertexPosition = vec4(morphPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;
  gl_Position = vertexPosition;
}
);

const char* FRAGMENT_SHADER = MAKE_SHADER(
uniform lowp  vec4    uColor;
uniform sampler2D     sTexture;

void main()
{
  gl_FragColor = uColor;
}
);

} // Anonymous namespace

Demo::SceneGraphEditor Internal::SceneGraphEditor::New()
{
  Dali::IntrusivePtr< Internal::SceneGraphEditor > impl = new Internal::SceneGraphEditor();
  Demo::SceneGraphEditor handle = Demo::SceneGraphEditor( *impl );
  impl->Initialize();
  return handle;
}

static int g_nodeTag = 0;
Internal::SceneGraphEditor::SceneGraphEditor()
: Control( ControlBehaviour( ACTOR_BEHAVIOUR_NONE ) )
{
  mTree = CreateNode();
  mTree->SetLayer( true );
}

Internal::SceneGraphEditor::~SceneGraphEditor()
{
}

void SceneGraphEditor::SetProperty( Dali::BaseObject* object, Dali::Property::Index propertyIndex, const Dali::Property::Value& value )
{
#if 0
  Toolkit::Slider slider = Toolkit::Slider::DownCast( Dali::BaseHandle( object ) );

  if ( slider )
  {
    Slider& sliderImpl( GetImpl( slider ) );

    switch ( propertyIndex )
    {
      case Toolkit::Slider::Property::LOWER_BOUND:
      {
        sliderImpl.SetLowerBound( value.Get< float >() );
        break;
      }
    }
  }
#endif
}
Dali::Property::Value SceneGraphEditor::GetProperty( Dali::BaseObject* object, Dali::Property::Index propertyIndex )
{
  Dali::Property::Value value;
  value = 0;
#if 0
  Toolkit::Slider slider = Toolkit::Slider::DownCast( Dali::BaseHandle( object ) );

  if ( slider )
  {
    Slider& sliderImpl( GetImpl( slider ) );

    switch ( propertyIndex )
    {
      case Toolkit::Slider::Property::LOWER_BOUND:
      {
        value = sliderImpl.GetLowerBound();
        break;
      }
    }
  }
#endif
  return value;
}


#define DEFAULT_NODE_DEPTH_Y          85.0f
#define DEFAULT_NODE_SEPARATION_X     85.0f
#define NODE_SPLINE_SEGMENTS          24
#define SPLINE_CURVE_STRENGTH         0.8f

void SceneGraphEditor::DeployNode( NodeControl& node, Dali::Actor& parent, Dali::Vector2 parentPosition, int depth )
{
  //std::cout << "DeployNode" << std::endl;
  Dali::Toolkit::ImageView& imageView = node.GetGraphic();

  Dali::Vector2 newPosition( node.GetPosition() );
  if( depth != 0 )
  {
    // Do not draw a curve for the root node.
    DrawCurve( node.GetSplineData(), parentPosition, newPosition );
  }
  imageView.SetPosition( Dali::Vector3( newPosition ) );

  parent.Add( imageView );

  NodeControlContainer& children = node.GetChildren();
  NodeControlContainer::iterator endIter = children.end();

  for( NodeControlContainer::iterator iter = children.begin(); iter != endIter; ++iter )
  {
    // Recurse.
    DeployNode( **iter, parent, newPosition, depth + 1 );
  }
}

void SceneGraphEditor::DeployNodeTree()
{
  std::cout << "DeployNodeTree" << std::endl;

  // Recurse.
  DeployNode( *mTree, mNodeParent, Dali::Vector2::ZERO, 0 );
}

NodeControl* SceneGraphEditor::CreateNode()
{
  NodeControl* nodeControl = new NodeControl( g_nodeTag );
  mNodeLookup.insert( std::make_pair( g_nodeTag, nodeControl ) );
  g_nodeTag++;
  nodeControl->GetGraphic().TouchedSignal().Connect( this, &SceneGraphEditor::OnTouch );
  return nodeControl;
}

void SceneGraphEditor::OnInitialize()
{
  std::cout << "OnInitialize" << std::endl;
}

void SceneGraphEditor::Deploy()
{
  Self().TouchedSignal().Connect( this, &SceneGraphEditor::OnTouch );
  mNodeParent = Dali::Actor::New();
  mNodeParent.SetParentOrigin( Dali::ParentOrigin::CENTER );
  mNodeParent.SetAnchorPoint( Dali::AnchorPoint::CENTER );
  Self().Add( mNodeParent );

  Dali::Vector2 position( Dali::Vector2::ZERO );
  mTree->SetPosition( position );

  NodeControl* nodeControl = CreateNode();
  position.y += DEFAULT_NODE_DEPTH_Y;
  position.x -= DEFAULT_NODE_SEPARATION_X / 2.0f;
  nodeControl->SetPosition( position );
  mTree->GetChildren().push_back( nodeControl );

  nodeControl = CreateNode();
  position.x += DEFAULT_NODE_SEPARATION_X;
  nodeControl->SetPosition( position );
  mTree->GetChildren().push_back( nodeControl );

  NodeControl* subNodeControl = CreateNode();
  position.y += DEFAULT_NODE_DEPTH_Y;
  position.x -= DEFAULT_NODE_SEPARATION_X / 2.0f;
  subNodeControl->SetPosition( position );
  nodeControl->GetChildren().push_back( subNodeControl );

  subNodeControl = CreateNode();
  position.x += DEFAULT_NODE_SEPARATION_X;
  subNodeControl->SetPosition( position );
  nodeControl->GetChildren().push_back( subNodeControl );

  DeployNodeTree();
}

void SceneGraphEditor::DrawCurve( SplineData& splineData, Dali::Vector2 startPosition, Dali::Vector2 endPosition )
{
  int pointCount = NODE_SPLINE_SEGMENTS;

  Dali::Path path = Dali::Path::New();
  Dali::Property::Array points;
  points.Resize( 2 );
  points[0] = Dali::Vector3( startPosition );
  points[1] = Dali::Vector3( endPosition );
  path.SetProperty( Dali::Path::Property::POINTS, points );
  Dali::Property::Array controlPoints;
  controlPoints.Resize( 2 );
  // Generate control points.
  float curveStrength = SPLINE_CURVE_STRENGTH;
  controlPoints[0] = Dali::Vector3( startPosition.x, ( endPosition.y * curveStrength )   + ( ( 1.0f - curveStrength ) * startPosition.y ), 0.0f );
  controlPoints[1] = Dali::Vector3( endPosition.x,   ( startPosition.y * curveStrength ) + ( ( 1.0f - curveStrength ) * endPosition.y ),   0.0f );

  path.SetProperty( Dali::Path::Property::CONTROL_POINTS, controlPoints );

  // Create vertices.
  struct Vertex { Dali::Vector2 position; };
  Dali::Property::Map vertexFormat;
  Dali::Vector3 outPosition, tangent;
  Vertex* vertexData = new Vertex[ pointCount ];
  for( int i = 0; i < pointCount; ++i )
  {
    float p = (float)i / (float)( pointCount - 1 );
    path.Sample( p, outPosition, tangent );
    //std::cout << "PATH: p:" << p << "  pos:" << outPosition << "  tangent:" << tangent << std::endl;
    vertexData[i].position = outPosition.GetVectorXY();
  }

  vertexFormat["aPosition1"] = Dali::Property::VECTOR2;
  Dali::PropertyBuffer pentagonVertices = Dali::PropertyBuffer::New( vertexFormat );
  pentagonVertices.SetData( vertexData, pointCount );
  delete []vertexData;

  // Create indices.
  int lineCount = pointCount - 1;
  int indexCount = lineCount * 2;
  unsigned short* indexData = new unsigned short[ indexCount ];
  for( int i = 0; i < indexCount; ++i )
  {
    // This generates a linear line mesh. EG. 0,1, 1,2, 2,3 ... etc
    indexData[i] = ( i + 1 ) / 2;
  }

  // Create the geometry object.
  Dali::Geometry geometry = Dali::Geometry::New();
  geometry.AddVertexBuffer( pentagonVertices );
  geometry.SetIndexBuffer( indexData, indexCount );
  geometry.SetGeometryType( Dali::Geometry::LINES );

  if( !splineData.actor )
  {
    // First curve for this node, create required info.
    Dali::Shader shader = Dali::Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );

    splineData.actor = Dali::Actor::New();
    splineData.actor.SetParentOrigin( Dali::ParentOrigin::CENTER );
    splineData.actor.SetAnchorPoint( Dali::AnchorPoint::CENTER );
    splineData.actor.SetSize( 1.0f, 1.0f );

    splineData.renderer = Dali::Renderer::New( geometry, shader ); // todor: Note: I don't want to set geometry here
    splineData.renderer.SetProperty( Dali::Renderer::Property::DEPTH_INDEX, 0 );

    splineData.actor.AddRenderer( splineData.renderer );

    Self().Add( splineData.actor );
  }
  else
  {
    // Reuse existing renderer.
    splineData.renderer.SetGeometry( geometry );
  }

}

bool SceneGraphEditor::OnTouch( Dali::Actor actor, const Dali::TouchEvent& event )
{
  if( event.GetPointCount() > 0 )
  {
    const Dali::TouchPoint& point = event.GetPoint( 0 );
    std::cout << "Node: Touched: state:" << (int)point.state << std::endl;

    // Get NodeTag from actor:
    int propertyIndex = actor.GetPropertyIndex( "node-control-tag" );
    std::cout << "idx: " << propertyIndex << std::endl;
    NodeControl* nodeControl = NULL;
    if( propertyIndex != Dali::Property::INVALID_INDEX )
    {
      int nodeTag = actor.GetProperty( propertyIndex ).Get<int>();
      std::cout << "Touched: NodeTag: " << nodeTag << std::endl;
      // Get NodeControl from NodeTag
      NodeLookupContainer::iterator lookupIterator = mNodeLookup.find( nodeTag );

      if( lookupIterator != mNodeLookup.end() )
      {
        // We touched a node.
        nodeControl = lookupIterator->second;
        std::cout << "Touched: NodeControl: " << nodeControl->GetPosition() << std::endl;
      }
    }

    if( point.state == Dali::TouchPoint::Down ) // Commence dragging
    {
      mDragNode = nodeControl;
      mDragStart = Dali::Vector2( point.screen.x, point.screen.y );
      mDragStart -= actor.GetCurrentPosition().GetVectorXY();
      return true;
    }
    else if( point.state == Dali::TouchPoint::Motion )
    {
      std::cout << "todor: DRAG-CONT:" << Dali::Vector2( point.screen.x, point.screen.y ) << std::endl;
      if( mDragNode )
      {
        // We are currently dragging a node.
        mDragNode->SetPosition( Dali::Vector2( point.screen.x, point.screen.y ) - mDragStart );
        // Redraw all. TODO: This can be reduced to this node downwards.
        DeployNodeTree();
        return true;
      }
    }
    else if( point.state == Dali::TouchPoint::Leave )
    {
      mDragNode = NULL;
      return true;
    }
  }

  return false;
}


} // namespace Internal

} // namespace Demo

