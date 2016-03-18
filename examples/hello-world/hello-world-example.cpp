/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <iostream>
#include <sstream>
#include "shared/view.h"
#include <vector>

using namespace Dali;
using Dali::Toolkit::TextLabel;

const char* const BASE_IMAGE_DIR = DEMO_IMAGE_DIR;
const int MAX_ACTORS = 8;

// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &HelloWorldController::Create );
  }

  ~HelloWorldController()
  {
    // Nothing to do here;
  }

  typedef Rect<float> Quad;

  struct QuickQuad
  {
      Quad geometry;
      float angle;
      bool clip;

      QuickQuad( Quad newGeometry, float newAngle, bool newClip )
      : geometry( newGeometry ),
        angle( newAngle ),
        clip( newClip )
      {
      }
  };


  typedef std::vector<QuickQuad> SceneGraph;

  SceneGraph mSceneGraph;

  std::vector<Actor> mActors;


  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

#if 0
    TextLabel textLabel = TextLabel::New( "Hello World" );
    textLabel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    textLabel.SetName( "helloWorldLabel" );
    stage.Add( textLabel );
#endif



    mSceneGraph.push_back( QuickQuad( Quad( 0.0f, 0.0f, 400.0f, 400.0f ), 0.0f, false ) );        // 0
    mSceneGraph.push_back( QuickQuad( Quad( 25.0f, 100.0f, 350.0f, 200.0f ), 0.0f, false ) );     // 1

    mSceneGraph.push_back( QuickQuad( Quad( 30.0f, 105.0f, 340.0f, 190.0f ), 0.0f, true ) );       // 2
    mSceneGraph.push_back( QuickQuad( Quad( -40.0f, -200.0f, 400.0f, 400.0f ), 0.0f, false ) );   // 3

#if 1
    mSceneGraph.push_back( QuickQuad( Quad( 35.0f, 110.0f - 80.0f, 162.5f, 180.0f ), 0.0f, true ) );      // 4
#endif
#if 1
    mSceneGraph.push_back( QuickQuad( Quad( 0.0f, 0.0f + 0.0f, 400.0f, 400.0f ), 0.0f, false ) );        // 5
#endif
#if 1
    mSceneGraph.push_back( QuickQuad( Quad( -116.25f, -200.0f, 400.0f, 100.0f ), 0.0f, false ) ); // 6
#endif
#if 1
    mSceneGraph.push_back( QuickQuad( Quad( 116.25f, 110.0f - 10.0f, 81.25f, 90.0f ), 0.0f, true ) );     // 7
#endif
#if 1
    mSceneGraph.push_back( QuickQuad( Quad( -81.25f, -245.0f, 400.0f, 50.0f ), 0.0f, false ) );   // 8
#endif

#if 0
    mSceneGraph.push_back( QuickQuad( Quad( 202.5f, 110.0f, 262.5f, 180.0f ), 0.0f, true ) );     // (2) 9
    mSceneGraph.push_back( QuickQuad( Quad( 0.0f, 0.0f, 400.0f, 140.0f ), 0.0f, false ) );        // (2) 10
#endif




    int i = 0;
    for( SceneGraph::iterator it = mSceneGraph.begin(); it != mSceneGraph.end(); ++it, ++i )
    {
      QuickQuad& renderer = *it;

      std::stringstream filename;
      filename << BASE_IMAGE_DIR << "gallery-small-";
      filename << ( i + 2 ) << ".jpg";

      Toolkit::ImageView actor = Toolkit::ImageView::New( filename.str() );

      if( i == 3 || i == 6 || i == 8 )
      {
        actor.SetAnchorPoint( AnchorPoint::CENTER );
        actor.SetParentOrigin( ParentOrigin::CENTER );
      }
      else
      {
        actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
        actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
      }
      actor.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
      actor.SetSize( renderer.geometry.width, renderer.geometry.height );
      actor.SetPosition( renderer.geometry.x, renderer.geometry.y );
      actor.SetOrientation( Dali::Degree( renderer.angle ), Vector3( 0.0f, 0.0f, 1.0f ) );

      std::stringstream name;
      name << "test-actor" << ( i + 1 );
      if( renderer.clip )
      {
        name << "-CLIPPING";
      }

      std::cout << "Created actor: " << name.str() << std::endl;

      mActors.push_back( actor );
      stage.Add( actor );

      actor.SetName( name.str() );
    }

#if 0
    KeyFrames keyFrames = KeyFrames::New();
    keyFrames.Add( 0.0f, 0.0f );
    keyFrames.Add( 1.0f, Vector3( 0.0f, 0.0f, 1.0f ) );
#endif

    Animation animation;

    animation = Animation::New( 32.0f );
    animation.AnimateBy( Property( mActors[3], Dali::Actor::Property::ORIENTATION ), Quaternion( Radian( M_PI * 2.0f ), Vector3::ZAXIS ) );
    animation.SetLooping( true );
    animation.Play();

    if( mActors.size() > 6 )
    {
      animation = Animation::New( 8.0f );
      animation.AnimateBy( Property( mActors[6], Dali::Actor::Property::ORIENTATION ), Quaternion( Radian( M_PI * 2.0f ), Vector3::ZAXIS ) );
      animation.SetLooping( true );
      animation.Play();
    }

    if( mActors.size() > 8 )
    {
      animation = Animation::New( 16.0f );
      animation.AnimateBy( Property( mActors[8], Dali::Actor::Property::ORIENTATION ), Quaternion( Radian( -M_PI * 2.0f ), Vector3::ZAXIS ) );
      animation.SetLooping( true );
      animation.Play();
    }

#if 0
    Toolkit::ImageView images[ MAX_ACTORS ];
    //images[ i++ ] = Toolkit::ImageView::New( DEMO_IMAGE_DIR "people-small-3.jpg" );

    float currentSize = 200.0f;
    float currentRotation = 0.0f;
    float offset = 0.0f;
    for( int i = 0; i < MAX_ACTORS; ++i )
    {

#if 1
      if( i == 3 )
      {
        currentSize = 120.0f;
      }
      else
      {
        currentSize = 200.0f;
      }
#endif


      std::stringstream filename;
      filename << BASE_IMAGE_DIR << "gallery-small-";
      filename << ( i + 2 ) << ".jpg";
      images[i] = Toolkit::ImageView::New( filename.str() );

      images[i].SetAnchorPoint( AnchorPoint::CENTER );
      images[i].SetParentOrigin( ParentOrigin::CENTER );
      images[i].SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
      images[i].SetSize( currentSize, currentSize );
      images[i].SetPosition( offset, offset );

#if 0
      if( i == 3 )
      {
        images[i].SetOrientation( Dali::Degree( 45.0f ), Vector3( 0.0f, 0.0f, 1.0f ) );
      }
#endif
      images[i].SetOrientation( Dali::Degree( currentRotation ), Vector3( 0.0f, 0.0f, 1.0f ) );


      std::stringstream name;
      name << "test-actor" << ( i + 1 );
      images[i].SetName( name.str() );

      std::cout << "Created actor: " << name.str() << std::endl;

      stage.Add( images[i] );

      offset += 40.0f;
      currentRotation += 90.0f / 2.0f;
    }
#endif


    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
};

void RunTest( Application& application )
{
  HelloWorldController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
