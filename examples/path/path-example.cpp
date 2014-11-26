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

    Dali::TextActor forwardLabel = TextActor::New("Forward Vector");
    forwardLabel.SetPosition( 10.0f, stage.GetSize().y - 60.0f, 0.0f );
    forwardLabel.SetColor( Vector4(0.0f,0.0f,0.0f,1.0f));
    forwardLabel.SetAnchorPoint( AnchorPoint::CENTER_LEFT);
    mContentLayer.Add( forwardLabel );

    //TextInput

    Dali::Layer textInputLayer = Dali::Layer::New();

    textInputLayer.SetSize( 400.0f, 30.0f, 0.0 );
    textInputLayer.SetPosition( 0.0f, stage.GetSize().y - 30.0f, 0.0f );
    textInputLayer.SetAnchorPoint( AnchorPoint::TOP_LEFT);
    textInputLayer.SetParentOrigin( ParentOrigin::TOP_LEFT);
    stage.Add( textInputLayer );

    Dali::TextActor label = TextActor::New("X:");
    label.SetPosition( 10.0f, 0.0f, 0.0f );
    label.SetColor( Vector4(0.0f,0.0f,0.0f,1.0f));
    label.SetAnchorPoint( AnchorPoint::CENTER_LEFT);
    textInputLayer.Add( label );


    TextStyle style;
    style.SetTextColor( Vector4( 0.0f, 0.0f ,0.0f, 1.0f ));

    mTextInput[0] = TextInput::New();
    mTextInput[0].SetInitialText("1.0");
    mTextInput[0].SetColor( Vector4(0.0f,0.0f,0.0f,1.0f));
    mTextInput[0].SetAnchorPoint( AnchorPoint::CENTER_LEFT);
    mTextInput[0].SetParentOrigin( ParentOrigin::CENTER_RIGHT);
    mTextInput[0].SetPosition( 10.0f, 0.0f, 0.0f );
    mTextInput[0].SetSize( 70.0f, 0.0f, 0.0f );
    mTextInput[0].SetTextAlignment(Alignment::HorizontalCenter );
    mTextInput[0].SetMaxCharacterLength( 5 );
    mTextInput[0].SetNumberOfLinesLimit(1);
    mTextInput[0].ApplyStyleToAll( style );
    mTextInput[0].SetProperty( mTextInput[0].GetPropertyIndex("cursor-color"), Vector4(0.0f,0.0f,0.0f,1.0f) );
    mTextInput[0].SetBackgroundColor( Vector4(0.8f,1.0f,0.8f, 0.4f));
    mTextInput[0].InputFinishedSignal().Connect(this, &PathController::OnTextInputEnd);
    mTextInput[0].SetEditOnTouch();
    label.Add( mTextInput[0]);

    label = TextActor::New("Y:");
    label.SetPosition( 160.0f,0.0f, 0.0f );
    label.SetColor( Vector4(0.0f,0.0f,0.0f,1.0f));
    label.SetAnchorPoint( AnchorPoint::CENTER_LEFT);
    textInputLayer.Add( label );

    mTextInput[1] = TextInput::New();
    mTextInput[1].SetInitialText("0.0");
    mTextInput[1].SetColor( Vector4(0.0f,0.0f,0.0f,1.0f));
    mTextInput[1].SetAnchorPoint( AnchorPoint::CENTER_LEFT);
    mTextInput[1].SetParentOrigin( ParentOrigin::CENTER_RIGHT);
    mTextInput[1].SetPosition( 10.0f, 0.0f, 0.0f );
    mTextInput[1].SetSize( 70.0f, 0.0f, 0.0f );
    mTextInput[1].SetTextAlignment(Alignment::HorizontalCenter );
    mTextInput[1].SetMaxCharacterLength( 5 );
    mTextInput[1].SetNumberOfLinesLimit(1);
    mTextInput[1].ApplyStyleToAll( style );
    mTextInput[1].SetProperty( mTextInput[1].GetPropertyIndex("cursor-color"), Vector4(0.0f,0.0f,0.0f,1.0f) );
    mTextInput[1].SetBackgroundColor( Vector4(0.8f,1.0f,0.8f, 0.4f));
    mTextInput[1].InputFinishedSignal().Connect(this, &PathController::OnTextInputEnd);
    label.Add( mTextInput[1]);

    label = TextActor::New("Z:");
    label.SetPosition( 310.0f, 0.0f, 0.0f );
    label.SetColor( Vector4(0.0f,0.0f,0.0f,1.0f));
    label.SetAnchorPoint( AnchorPoint::CENTER_LEFT);
    textInputLayer.Add( label );

    mTextInput[2] = TextInput::New();
    mTextInput[2].SetInitialText("0.0");
    mTextInput[2].SetColor( Vector4(0.0f,0.0f,0.0f,1.0f));
    mTextInput[2].SetAnchorPoint( AnchorPoint::CENTER_LEFT);
    mTextInput[2].SetParentOrigin( ParentOrigin::CENTER_RIGHT);
    mTextInput[2].SetPosition( 10.0f, 0.0f, 0.0f );
    mTextInput[2].SetSize( 70.0f, 0.0f, 0.0f );
    mTextInput[2].SetTextAlignment(Alignment::HorizontalCenter );
    mTextInput[2].SetMaxCharacterLength( 5 );
    mTextInput[2].SetNumberOfLinesLimit(1);
    mTextInput[2].ApplyStyleToAll( style );
    mTextInput[2].SetProperty( mTextInput[2].GetPropertyIndex("cursor-color"), Vector4(0.0f,0.0f,0.0f,1.0f) );
    mTextInput[2].SetBackgroundColor( Vector4(0.8f,1.0f,0.8f, 0.4f));
    mTextInput[2].InputFinishedSignal().Connect(this, &PathController::OnTextInputEnd);
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

    ////Path
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

    ////Segments connecting interpolation points and control points
    int baseVertex = vVertex.size();

    size_t pointCount = mPath.GetPointCount();
    size_t controlPointIndex = 0;
    for( size_t i(0); i<pointCount; ++i )
    {
      vVertex.push_back( MeshData::Vertex(mPath.GetPoint(i),Vector2::ZERO, Vector3::ZERO ) );
      if( i<pointCount-1)
      {
        vVertex.push_back( MeshData::Vertex(mPath.GetControlPoint(controlPointIndex),Vector2::ZERO, Vector3::ZERO ));
        vVertex.push_back( MeshData::Vertex(mPath.GetControlPoint(controlPointIndex+1),Vector2::ZERO, Vector3::ZERO ));
      }
      controlPointIndex += 2;
    }

    size_t segments = 2*(pointCount-2)+2;
    int index0=baseVertex;
    int index1=baseVertex+1;
    for( size_t i(0); i<segments; ++i )
    {
      vIndex.push_back(index0);
      vIndex.push_back(index1);

      index0+=1;
      index1+=1;

      if( i % 2 == 0 )
      {
        index0 += 1;
        index1 += 1;
      }
    }


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

    for( size_t i(0); i<pointCount; ++i )
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
      mKnot[i].SetSize( 20.0f, 20.0f );

      mKnot[i].TouchedSignal().Connect(this, &PathController::OnTouchPoint);
      mContentLayer.Add(mKnot[i] );
    }

    //Control points
    size_t controlPointCount=2*(pointCount-1);
    for( size_t i(0); i<controlPointCount; ++i )
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
      mControlPoint[i].SetSize( 20.0f, 20.0f );

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
      else if(!mDragActor && point.state==TouchPoint::Down ) // Add new point
      {
        const TouchPoint& point = event.GetPoint(0);
        Vector3 newPoint = Vector3(point.screen.x, point.screen.y, 0.0f);

        size_t pointCount = mPath.GetPointCount();
        Vector3 lastPoint = mPath.GetPoint( pointCount-1);
        mPath.AddPoint( newPoint );

        Vector3 displacement = (newPoint-lastPoint)/8;

        mPath.AddControlPoint( lastPoint + displacement );
        mPath.AddControlPoint( newPoint - displacement);

        DrawPath( 200u );
        CreateAnimation();
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


  void OnTextInputEnd( TextInput textInput)
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
  ImageActor mKnot[50];
  ImageActor mControlPoint[50];
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
