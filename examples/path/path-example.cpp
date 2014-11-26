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

/**
 *  Example description
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <stdlib.h> //atof

// INTERNAL INCLUDES
#include "../shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;


namespace
{
const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-default.png" );
const char* ACTOR_IMAGE( DALI_IMAGE_DIR "dali-logo.png" );
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char* APPLICATION_TITLE( "Path Example" );

}; //Unnamed namespace

/**
 * @brief The main class of the demo.
 */
class PathController : public ConnectionTracker
{
public:

  PathController( Application& application )
  : mApplication( application )
  {
    std::cout << "PathController::PathController" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &PathController::Create );
  }

  ~PathController()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    std::cout << "PathController::Create" << std::endl;

    // Get a handle to the stage:
    Stage stage = Stage::GetCurrent();

    // Connect to input event signals:
    stage.KeyEventSignal().Connect(this, &PathController::OnKeyEvent);

    // Hide the indicator bar
    mApplication.GetWindow().ShowIndicator(Dali::Window::INVISIBLE);

    // Create a default view with a default tool bar:
    mContentLayer = DemoHelper::CreateView( mApplication,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            "" );

    mContentLayer.TouchedSignal().Connect(this, &PathController::OnTouchLayer);

    //Title
    mTitleActor = TextView::New();
    // Add title to the tool bar.
    mToolBar.AddControl( mTitleActor, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Alignment::HorizontalCenter );
    Font font = Font::New();
    mTitleActor.SetText( APPLICATION_TITLE );
    mTitleActor.SetSize( font.MeasureText( APPLICATION_TITLE ) );
    mTitleActor.SetStyleToCurrentText(DemoHelper::GetDefaultTextStyle());

    //PAth
    mPath = Dali::Path::New();
    mPath.AddPoint( Vector3( 10.0f, stage.GetSize().y*0.5f, 0.0f ));
    mPath.AddPoint( Vector3( stage.GetSize().x*0.5f, stage.GetSize().y*0.3f, 0.0f ));
    mPath.AddPoint( Vector3( stage.GetSize().x-10.0f, stage.GetSize().y*0.5f, 0.0f ));
    mPath.GenerateControlPoints(0.25f);

    DrawPath( 200u );

    //Actor
    Image img = Image::New( ACTOR_IMAGE );
    mActor = ImageActor::New( img );
    mActor.SetPosition( Vector3( 10.0f, stage.GetSize().y*0.5f, 0.0f ) );
    mActor.SetAnchorPoint( AnchorPoint::CENTER );
    mActor.SetSize( 100, 50, 1 );
    stage.Add( mActor );

    mForward = Vector3::XAXIS;
    CreateAnimation();

    //TextInput
    Dali::TextActor label = TextActor::New("X:");
    label.SetPosition( 10.0f, stage.GetSize().y - 30.0f, 0.0f );
    label.SetColor( Vector4(0.0f,0.0f,0.0f,1.0f));
    label.SetAnchorPoint( AnchorPoint::CENTER_LEFT);
    mContentLayer.Add( label );

    mTextInput[0] = TextInput::New();
    mTextInput[0].SetInitialText("1.0");
    mTextInput[0].SetColor( Vector4(0.0f,0.0f,0.0f,1.0f));
    mTextInput[0].SetAnchorPoint( AnchorPoint::CENTER_LEFT);
    mTextInput[0].SetParentOrigin( ParentOrigin::CENTER_RIGHT);
    mTextInput[0].SetPosition( 10.0f, 0.0f, 0.0f );
    mTextInput[0].SetMaxCharacterLength( 4 );
    mTextInput[0].SetNumberOfLinesLimit(1);
    mTextInput[0].InputFinishedSignal().Connect(this, &PathController::OnTextInput);
    label.Add( mTextInput[0]);

    label = TextActor::New("Y:");
    label.SetPosition( 160.0f, stage.GetSize().y - 30.0f, 0.0f );
    label.SetColor( Vector4(0.0f,0.0f,0.0f,1.0f));
    label.SetAnchorPoint( AnchorPoint::CENTER_LEFT);
    mContentLayer.Add( label );

    mTextInput[1] = TextInput::New();
    mTextInput[1].SetInitialText("0.0");
    mTextInput[1].SetColor( Vector4(0.0f,0.0f,0.0f,1.0f));
    mTextInput[1].SetAnchorPoint( AnchorPoint::CENTER_LEFT);
    mTextInput[1].SetParentOrigin( ParentOrigin::CENTER_RIGHT);
    mTextInput[1].SetPosition( 10.0f, 0.0f, 0.0f );
    mTextInput[1].SetMaxCharacterLength( 4 );
    mTextInput[1].SetNumberOfLinesLimit(1);
    mTextInput[1].InputFinishedSignal().Connect(this, &PathController::OnTextInput);
    label.Add( mTextInput[1]);

    label = TextActor::New("Z:");
    label.SetPosition( 310.0f, stage.GetSize().y - 30.0f, 0.0f );
    label.SetColor( Vector4(0.0f,0.0f,0.0f,1.0f));
    label.SetAnchorPoint( AnchorPoint::CENTER_LEFT);
    mContentLayer.Add( label );

    mTextInput[2] = TextInput::New();
    mTextInput[2].SetInitialText("0.0");
    mTextInput[2].SetColor( Vector4(0.0f,0.0f,0.0f,1.0f));
    mTextInput[2].SetAnchorPoint( AnchorPoint::CENTER_LEFT);
    mTextInput[2].SetParentOrigin( ParentOrigin::CENTER_RIGHT);
    mTextInput[2].SetPosition( 10.0f, 0.0f, 0.0f );
    mTextInput[2].SetMaxCharacterLength( 4 );
    mTextInput[2].SetNumberOfLinesLimit(1);
    mTextInput[2].InputFinishedSignal().Connect(this, &PathController::OnTextInput);
    label.Add( mTextInput[2]);

  }

  void DrawPath( unsigned int resolution )
  {
    Stage stage = Dali::Stage::GetCurrent();

    std::vector<Dali::MeshData::Vertex> vVertex( resolution + 1 );

    size_t lineCount( resolution);
    std::vector<unsigned short> vIndex(2*lineCount);
    Vector3 tangent;
    Vector3 position;
    float delta = 1.0f / (float)resolution;

    //Vertex position
    for( unsigned int i(0); i<=resolution; ++i )
    {
      mPath.Sample( i*delta, position, tangent );

      vVertex[i].x = position.x;
      vVertex[i].y = position.y;
      vVertex[i].z = position.z;
    }

    //Indices
    size_t nIndex = 0;
    for( unsigned int i(0); i<lineCount; ++i )
    {
      vIndex[nIndex] = i;
      vIndex[nIndex+1] = i + 1;


      nIndex += 2;
    }

    //Segments connecting interpolation points and control points
    int baseVertex = vVertex.size();
    vVertex.push_back( MeshData::Vertex(mPath.GetPoint(0),Vector2::ZERO, Vector3::ZERO ) );
    vVertex.push_back( MeshData::Vertex(mPath.GetControlPoint(0),Vector2::ZERO, Vector3::ZERO ));
    vVertex.push_back( MeshData::Vertex(mPath.GetControlPoint(1),Vector2::ZERO, Vector3::ZERO ));
    vVertex.push_back( MeshData::Vertex(mPath.GetPoint(1),Vector2::ZERO, Vector3::ZERO ));
    vVertex.push_back( MeshData::Vertex(mPath.GetControlPoint(2),Vector2::ZERO, Vector3::ZERO ));
    vVertex.push_back( MeshData::Vertex(mPath.GetControlPoint(3),Vector2::ZERO, Vector3::ZERO ));
    vVertex.push_back( MeshData::Vertex(mPath.GetPoint(2),Vector2::ZERO, Vector3::ZERO ));

    //P0 to CP0
    vIndex.push_back( baseVertex );
    vIndex.push_back( baseVertex + 1 );
    //CP1 to P1
    vIndex.push_back( baseVertex + 2 );
    vIndex.push_back( baseVertex + 3 );
    //P1 to CP2
    vIndex.push_back( baseVertex + 3);
    vIndex.push_back( baseVertex + 4);
    //CP3 to P2
    vIndex.push_back( baseVertex + 5);
    vIndex.push_back( baseVertex + 6);


    //Material
    Dali::Material material = Material::New("LineMaterial");
    material.SetDiffuseColor( Vector4(0.0f,0.0f,0.0f,1.0f));

    MeshData meshData;
    meshData.SetLineData( vVertex, vIndex, material );
    Dali::Mesh mesh = Dali::Mesh::New( meshData );

    if( mMesh )
    {
      stage.Remove( mMesh );
    }

    mMesh = Dali::MeshActor::New( mesh );
    mMesh.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mMesh.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mMesh.SetAffectedByLighting(false);

    stage.Add( mMesh );

    //Interpolation points
    for( int i(0); i<3; ++i )
    {
      if( mKnot[i])
        mContentLayer.Remove( mKnot[i]);

      std::string name( "Knot_");
      name.push_back( i + 48 );

      mKnot[i] = Toolkit::CreateSolidColorActor(Vector4(0.0f,0.0f,0.0f,1.0f) );
      mKnot[i].SetName( name );
      mKnot[i].SetParentOrigin( ParentOrigin::TOP_LEFT);
      mKnot[i].SetAnchorPoint( AnchorPoint::CENTER );
      mKnot[i].SetPosition( mPath.GetPoint(i) );
      mKnot[i].SetSize( 10.0f, 10.0f );

      mKnot[i].TouchedSignal().Connect(this, &PathController::OnTouchPoint);
      mContentLayer.Add(mKnot[i] );
    }

    //Control points
    for( int i(0); i<4; ++i )
    {
      if( mControlPoint[i])
        mContentLayer.Remove( mControlPoint[i]);

      std::string name( "ControlPoint_");
      name.push_back( i + 48 );

      mControlPoint[i] = Toolkit::CreateSolidColorActor(Vector4(1.0f,0.0f,0.0f,1.0f) );
      mControlPoint[i].SetName( name );
      mControlPoint[i].SetParentOrigin( ParentOrigin::TOP_LEFT);
      mControlPoint[i].SetAnchorPoint( AnchorPoint::CENTER );
      mControlPoint[i].SetPosition( mPath.GetControlPoint(i) );
      mControlPoint[i].SetSize( 10.0f, 10.0f );

      mControlPoint[i].TouchedSignal().Connect(this, &PathController::OnTouchPoint);
      mContentLayer.Add(mControlPoint[i] );
    }
  }

  bool OnTouchPoint(Actor actor, const TouchEvent& event)
  {
    if(event.GetPointCount()>0)
    {
      const TouchPoint& point = event.GetPoint(0);
      if(point.state==TouchPoint::Down) // Commence dragging
      {
        mDragActor = actor;
      }
    }
    return false;
  }

  bool OnTouchLayer(Actor actor, const TouchEvent& event)
  {
    if(event.GetPointCount()>0)
    {
      const TouchPoint& point = event.GetPoint(0);
      if(point.state==TouchPoint::Up) // Stop dragging
      {
        mDragActor.Reset();
      }
      else
      {
        if( mDragActor )
        {
          const TouchPoint& point = event.GetPoint(0);
          Vector3 newPosition = Vector3(point.screen.x, point.screen.y, 0.0f);

          std::string actorName(mDragActor.GetName());

          if( actorName.compare(0, 5, "Knot_") == 0)
          {
             int index = actorName[5] - 48;
             mPath.GetPoint(index) = newPosition;
          }
          else
          {
            int index = actorName[13] - 48;
            mPath.GetControlPoint(index) = newPosition;
          }

          DrawPath( 200u );
          CreateAnimation();
        }
      }
    }
    return false;
  }

 /**
  * Main key event handler.
  * Quit on escape key.
  */
  void OnKeyEvent(const KeyEvent& event)
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE ) ||
          IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }


  void OnTextInput( TextInput textInput)
  {



    mForward.x = (float)atof( mTextInput[0].GetText().c_str() );
    mForward.y = (float)atof( mTextInput[1].GetText().c_str() );
    mForward.z = (float)atof( mTextInput[2].GetText().c_str() );


    CreateAnimation();


  }

  void CreateAnimation()
  {
    if( !mAnimation )
    {
      mAnimation = Animation::New( 2.0f );
    }
    else
    {
      mAnimation.Pause();
      mAnimation.Clear();
      mActor.SetRotation( Quaternion() );
    }

    mAnimation.Animate( mActor, mPath, mForward );
    mAnimation.SetLooping( true );
    mAnimation.Play();

  }

private:
  Application&  mApplication;

  Layer mContentLayer;                ///< The content layer (contains non gui chrome actors)
  Toolkit::View mView;                ///< The View instance.
  Toolkit::ToolBar mToolBar;          ///< The View's Toolbar.
  TextView mTitleActor;               ///< The Toolbar's Title.
  Path     mPath;
  MeshActor mMesh;
  ImageActor mKnot[3];
  ImageActor mControlPoint[4];
  Actor mDragActor;

  ImageActor mActor;
  Animation  mAnimation;

  TextInput mTextInput[3];
  Vector3   mForward;

};

void RunTest( Application& application )
{
  PathController test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
