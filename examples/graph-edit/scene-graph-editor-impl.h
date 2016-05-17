#ifndef DALI_DEMO_SCENE_GRAPH_EDITOR_IMPL_H
#define DALI_DEMO_SCENE_GRAPH_EDITOR_IMPL_H

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

#include <dali-toolkit/dali-toolkit.h>
#include "scene-graph-editor.h"
#include <vector>
#include <map>
#include <dali/devel-api/rendering/shader.h>
#include <dali/devel-api/rendering/material.h>
#include <dali/devel-api/rendering/geometry.h>
#include <dali/devel-api/rendering/renderer.h>
//todor
#include <iostream>


namespace Demo
{

namespace Internal
{

class SceneGraphEditor;
typedef Dali::IntrusivePtr< SceneGraphEditor > SceneGraphEditorPtr;


#define NODE_IMAGE_LAYER          DEMO_IMAGE_DIR "gallery-large-1.jpg"
#define NODE_IMAGE_NORMAL         DEMO_IMAGE_DIR "gallery-large-2.jpg"
#define DEFAULT_NODE_SIZE         Dali::Vector2( 50.0f, 50.0f )
class NodeControl;
typedef std::vector<NodeControl*> NodeControlContainer;
typedef std::map<int, NodeControl*> NodeLookupContainer;
//static int g_nodeTag = 0;

struct SplineData
{
    Dali::Material material;
    Dali::Actor actor;
    //Dali::Shader shader;
    //Dali::ResourceImage image;
    //Dali::Geometry geometry;
    //Dali::Renderer renderer;
};

class NodeControl : public Dali::ConnectionTracker
{
  public:

    NodeControl( int tag = 0 )
    : mIsLayer( false ),
      mInDrag( false )
    {
      mGraphic = Dali::Toolkit::ImageView::New();
      UpdateGraphic();

      mGraphic.SetAnchorPoint( Dali::AnchorPoint::CENTER );
      mGraphic.SetParentOrigin( Dali::ParentOrigin::CENTER );
      mGraphic.SetSize( DEFAULT_NODE_SIZE );

      Dali::Property::Value propertyValue = tag;
      mGraphic.RegisterProperty( "node-control-tag", propertyValue );
    }

    void UpdateGraphic()
    {
      if( mIsLayer )
      {
        mGraphic.SetImage( NODE_IMAGE_LAYER );
      }
      else
      {
        mGraphic.SetImage( NODE_IMAGE_NORMAL );
      }
    }

    virtual ~NodeControl()
    {

    }

    void SetLayer( bool enable )
    {
      mIsLayer = enable;
      UpdateGraphic();
    }

    NodeControlContainer& GetChildren()
    {
      return mChildren;
    }

    Dali::Toolkit::ImageView& GetGraphic()
    {
      return mGraphic;
    }

    void SetPosition( Dali::Vector2 position )
    {
      mPosition = position;
      mGraphic.SetPosition( Dali::Vector3( position ) );
    }

    Dali::Vector2 GetPosition()
    {
      return mPosition;
    }

    SplineData& GetSplineData()
    {
      return mSplineData;
    }

  private:

    bool mIsLayer;
    Dali::Vector2 mPosition;
    NodeControl* mParent;
    NodeControlContainer mChildren;
    Dali::Toolkit::ImageView mGraphic;
    SplineData mSplineData;

    bool mInDrag;
    Dali::Vector3 mDragStart;
    Dali::Animation mDragAnimation;

};


/********************************************************************************
 * Class to implement xxx
 */
// class SceneGraphEditor : public Dali::Toolkit::Internal::Control
class SceneGraphEditor : public Dali::Toolkit::Internal::Control
{
public:

  static Demo::SceneGraphEditor New();

  SceneGraphEditor();
  ~SceneGraphEditor();

  virtual void OnInitialize();


  void Deploy();
  void DeployNodeTree();
  void DeployNode( NodeControl& node, Dali::Actor& parent, Dali::Vector2 parentPosition, int depth );
  NodeControl* CreateNode();
  //bool OnTouchNode( Dali::Actor actor, const Dali::TouchEvent& event );
  bool OnTouch( Dali::Actor actor, const Dali::TouchEvent& event );
  void DrawCurve( SplineData& splineData, Dali::Vector2 startPosition, Dali::Vector2 endPosition );

  static void SetProperty( Dali::BaseObject* object, Dali::Property::Index propertyIndex, const Dali::Property::Value& value );
  static Dali::Property::Value GetProperty( Dali::BaseObject* object, Dali::Property::Index propertyIndex );



#if 0
  void SetDuration(float duration);
  void SetEasingFunction( Dali::AlphaFunction easingFunction );

  void SetDiameter(float diameter);
  void SetInitialAngle( Dali::Radian initialAngle);
  void SetFinalAngle( Dali::Radian finalAngle);
  void SetInitialSector( Dali::Radian initialSector);
  void SetFinalSector( Dali::Radian finalSector);
  void SetInitialActorAngle( Dali::Radian initialAngle );
  void SetFinalActorAngle( Dali::Radian finalAngle );

  float GetDuration( );
  float GetDiameter( );
  Dali::Radian GetInitialAngle( );
  Dali::Radian GetFinalAngle( );
  Dali::Radian GetInitialSector( );
  Dali::Radian GetFinalSector( );
  Dali::Radian GetInitialActorAngle( );
  Dali::Radian GetFinalActorAngle( );

  void RotateActorsWithStencil(bool rotate);

  void Add( Dali::Actor actor );

  void Activate( Dali::Animation anim = Dali::Animation(), float offsetTime=0, float duration=2.0f );

  void Deactivate();
#endif

private:

  /**
   * Create the stencil mask
   */
  //void CreateStencil(Dali::Radian initialSector );

private:

  //todor vars
  NodeControl*          mTree;
  Dali::Vector2         mDragStart;
  NodeControl*          mDragNode;
  NodeLookupContainer   mNodeLookup;
  Dali::Actor           mNodeParent;

  //todor spline
  Dali::Image    mImage;
  Dali::Shader   mShader;
  Dali::Material mMaterial;
  Dali::Geometry mGeometry;
  Dali::Renderer mRenderer;
  Dali::Actor    mMeshActor;

  //todor del

#if 0
  Dali::Layer           mLayer;
  Dali::Animation       mAnim;
  float                 mDuration;
  float                 mDiameter;
  Dali::Radian          mInitialAngle;
  Dali::Radian          mFinalAngle;
  Dali::Radian          mInitialSector;
  Dali::Radian          mFinalSector;
  Dali::Radian          mInitialActorAngle;
  Dali::Radian          mFinalActorAngle;
  Dali::AlphaFunction   mEasingFunction;
  Dali::Actor           mStencilActor;       ///< Stencil actor which generates mask
  Dali::Property::Index mStartAngleIndex;    ///< Index of start-angle property
  Dali::Property::Index mRotationAngleIndex; ///< Index of rotation-angle property
  bool                  mRotateActorsWithStencil:1;
  bool                  mRotateActors;
#endif
};


} // namespace Internal


inline Internal::SceneGraphEditor& GetImpl( Demo::SceneGraphEditor& object )
{
  DALI_ASSERT_ALWAYS( object );
  Dali::RefObject& handle = object.GetImplementation();
  return static_cast< Internal::SceneGraphEditor& >( handle );
}

inline const Internal::SceneGraphEditor& GetImpl( const Demo::SceneGraphEditor& object )
{
  DALI_ASSERT_ALWAYS( object );
  const Dali::RefObject& handle = object.GetImplementation();
  return static_cast< const Internal::SceneGraphEditor& >( handle );
}


} // namespace Demo


#endif // DALI_DEMO_RADIAL_SWEEP_VIEW_IMPL_H
