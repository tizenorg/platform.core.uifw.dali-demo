//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

static const float TestImageWidth  = 256.0f;
static const float TestImageHeight = 256.0f;
static const char* TestImageFilename = DALI_IMAGE_DIR "gallery-medium-45.jpg";


// This example shows how to create an image actor for displaying an image
//
class ImageActorExample : public ConnectionTracker
{
public:

  ImageActorExample( Application& application );

  ~ImageActorExample();

  void OnInit( Application& application );

  void AnimationCompleted(Animation& source);

  void OnKeyEvent( const KeyEvent& event );

  bool OnTouchEvent(Actor actor, const TouchEvent& event);

  void OnPan( Actor actor, PanGesture panGesture );

  void OnHorizontalPan( Actor actor, PanGesture panGesture );

  void OnVerticalPan( Actor actor, PanGesture panGesture );

  void OnPinch( Actor actor, PinchGesture pinchGesture );

private:
  void CreateMesh();
  Actor CreatePad( Actor parent, std::string label );

  Application& mApplication;
  Actor      mRootActor;
  ImageActor mImageActor;

  Actor mPanPad;
  Actor mRotateAnyPad;
  Actor mRotateHorizontalPad;
  Actor mRotateVerticalPad;
  Actor mDepthPad;
  CameraActor mCameraActor;

  Animation            mAnimation;
  PanGestureDetector   mPanDetector;
  PanGestureDetector   mHorizontalPanDetector;
  PanGestureDetector   mVerticalPanDetector;
  PinchGestureDetector mPinchDetector;

  Mesh                 mMesh;
  bool mPanHandled;
};

ImageActorExample::ImageActorExample( Application& application )
: mApplication( application )
{
  // Connect to the Application's Init signal
  application.InitSignal().Connect( this, &ImageActorExample::OnInit );
}

ImageActorExample::~ImageActorExample()
{
  // Nothing to do here; mImageActor will clean-up automatically
}

namespace
{
inline MeshData::Vertex CreateVertex(float x, float y)
{
  MeshData::Vertex v;
  memset(&v, 0.0f, sizeof(v));
  v.x=x;
  v.y=y;
  return v;
}
}

void ImageActorExample::CreateMesh()
{
  MeshData::VertexContainer vertices;
  MeshData::FaceIndices faces;
  vertices.push_back(CreateVertex(-0.5f, -0.5f));
  vertices.push_back(CreateVertex( 0.5f, -0.5f));
  vertices.push_back(CreateVertex( 0.5f,  0.5f));
  vertices.push_back(CreateVertex(-0.5f,  0.5f));
  faces.push_back(0);   faces.push_back(1);
  faces.push_back(1);   faces.push_back(2);
  faces.push_back(2);   faces.push_back(3);
  faces.push_back(3);   faces.push_back(0);

  Material material = Material::New("default");
  material.SetDiffuseColor(Vector4(0.0f, 1.0f, 0.0f, 1.0f));
  material.SetAmbientColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));

  MeshData meshData;
  meshData.SetLineData( vertices, faces, material);
  meshData.SetHasTextureCoords(false);
  meshData.SetHasNormals(false);
  mMesh = Mesh::New( meshData );
}


Actor ImageActorExample::CreatePad( Actor parent, std::string label )
{
  MeshActor meshActor = MeshActor::New(mMesh);
  meshActor.SetAffectedByLighting(false);
  meshActor.SetParentOrigin(ParentOrigin::CENTER);
  Actor padActor = Actor::New();
  padActor.Add(meshActor);
  Constraint constraint = Constraint::New<Vector3>( Actor::SCALE, ParentSource( Actor::SIZE ), EqualToConstraint() );
  meshActor.ApplyConstraint(constraint);

  TextActor textActor = TextActor::New( label );
  textActor.SetParentOrigin( ParentOrigin::CENTER );
  padActor.Add( textActor );
  parent.Add(padActor);
  return padActor;
}

namespace {
struct HeightToRelativeWidthConstraint
{
  HeightToRelativeWidthConstraint(float xScale, float yScale)
  : mXScale( xScale ), mYScale( yScale )
  {
  }

  Vector3 operator()(const Vector3& current, const PropertyInput& property)
  {
    Vector3 size = property.GetVector3();
    return Vector3( size.x*mXScale, size.x*mXScale*mYScale, 1.0f );
  }
  float mXScale;
  float mYScale;
};

// Sets the height to a scale of the width of the parent
// sets the width to a scale of own height
struct WidthToRelativeXHeightConstraint
{
  WidthToRelativeXHeightConstraint(float xScale, float yScale)
  : mXScale( xScale ), mYScale( yScale )
  {
  }

  Vector3 operator()(const Vector3& current, const PropertyInput& property)
  {
    Vector3 size = property.GetVector3();
    return Vector3( size.x*mXScale*mYScale, size.x*mYScale, 1.0f );
  }
  float mXScale;
  float mYScale;
};

}

void ImageActorExample::OnInit( Application& application )
{
  // The Init signal is received once (only) during the Application lifetime
  Stage stage = Stage::GetCurrent();

  mRootActor = Actor::New();
  CreateMesh();

  mPanPad = CreatePad(stage.GetRootLayer(), "Camera");
  mRotateAnyPad = CreatePad(stage.GetRootLayer(), "Rotate");
  mDepthPad = CreatePad(stage.GetRootLayer(), "Depth");
  mRotateHorizontalPad = CreatePad(mRotateAnyPad, "");
  mRotateVerticalPad = CreatePad(mRotateAnyPad, "");

  Layer rootLayer = stage.GetRootLayer();
  Constraint constraint = Constraint::New<Vector3>( Actor::SIZE, Source( rootLayer, Actor::SIZE ),
                                                    HeightToRelativeWidthConstraint( 0.25f, 1.0f ) );
  mPanPad.ApplyConstraint(constraint);
  mRotateAnyPad.ApplyConstraint(constraint);
  mRotateHorizontalPad.ApplyConstraint(Constraint::New<Vector3>( Actor::SIZE, Source( rootLayer, Actor::SIZE ),
                                                            HeightToRelativeWidthConstraint( 0.25f, 0.333f )));
  mRotateVerticalPad.ApplyConstraint(Constraint::New<Vector3>( Actor::SIZE, Source( rootLayer, Actor::SIZE ),
                                                               WidthToRelativeXHeightConstraint( 0.33f, 0.25f )));

  mDepthPad.ApplyConstraint(constraint);
  mPanPad.SetParentOrigin(Vector3(0.25f, 1.0f, 0.5f));
  mPanPad.SetAnchorPoint(AnchorPoint::BOTTOM_CENTER);
  mRotateAnyPad.SetParentOrigin(Vector3(0.5f, 1.0f, 0.5f));
  mRotateAnyPad.SetAnchorPoint(AnchorPoint::BOTTOM_CENTER);
  mRotateVerticalPad.SetParentOrigin(ParentOrigin::CENTER);
  mRotateHorizontalPad.SetParentOrigin(ParentOrigin::CENTER);
  mDepthPad.SetParentOrigin(Vector3(0.75f, 1.0f, 0.5f));
  mDepthPad.SetAnchorPoint(AnchorPoint::BOTTOM_CENTER);

  // Create the image
  Image image = Image::New( TestImageFilename );
  mImageActor = ImageActor::New( image );

  TextStyle style;
  style.SetFontName( "DejaVu-Sans-Book" );
  style.SetFontStyle( "regular" );
  style.SetFontPointSize(PointSize(64.0f));
  style.SetShadow( true, Vector4(0.4f, 0.4f, 0.4f, 0.7f), Vector2(5.0f, 5.0f) );
  style.SetItalics( true, Degree( 45.0f ) );
  TextActorParameters params( style, TextActorParameters::FONT_DETECTION_OFF );
  TextActor textActor = TextActor::New( "LexM", params );
  textActor.SetColor(Vector4(1.0f, 0.0f, 1.0f, 1.0f));
  textActor.SetParentOrigin( ParentOrigin::CENTER );
  textActor.SetAnchorPoint( AnchorPoint::CENTER );

  style.SetItalics(false, Degree( 0.0f ));
  params = TextActorParameters( style, TextActorParameters::FONT_DETECTION_OFF );
  TextActor textActor2 = TextActor::New( "Text", params );
  textActor2.SetParentOrigin( ParentOrigin::CENTER );
  textActor2.SetAnchorPoint( AnchorPoint::CENTER );

  // Set the image size
  mImageActor.SetSize( TestImageWidth, TestImageHeight );

  // Center the image
  mImageActor.SetParentOrigin( ParentOrigin::CENTER );
  mImageActor.SetAnchorPoint( AnchorPoint::CENTER );

  mRootActor.Add( mImageActor );
  mImageActor.Add( textActor );
  mImageActor.Add( textActor2 );
  textActor.SetZ(1.0f);
  textActor2.SetZ(1.0f);

  mRootActor.SetSize( TestImageWidth*1.75f, TestImageHeight*1.75f );

  mRootActor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mRootActor.SetAnchorPoint( AnchorPoint::CENTER );
  mRootActor.SetPosition( Vector3(stage.GetSize() * 0.5f) );
  stage.Add( mRootActor );

  mPanDetector = PanGestureDetector::New();
  mPanDetector.Attach( stage.GetRootLayer() );
  mPanDetector.Attach( mPanPad );
  mPanDetector.Attach( mRotateAnyPad );
  mPanDetector.Attach( mDepthPad );
  mPanDetector.DetectedSignal().Connect( this, &ImageActorExample::OnPan );

  mVerticalPanDetector = PanGestureDetector::New();
  mVerticalPanDetector.AddDirection( Degree( 90 ), Degree(20) );
  mVerticalPanDetector.Attach( mRotateVerticalPad );
  mVerticalPanDetector.DetectedSignal().Connect( this, &ImageActorExample::OnVerticalPan);

  mHorizontalPanDetector = PanGestureDetector::New();
  mHorizontalPanDetector.AddDirection( Degree( 0 ), Degree(20) );
  mHorizontalPanDetector.Attach( mRotateHorizontalPad );
  mHorizontalPanDetector.DetectedSignal().Connect( this, &ImageActorExample::OnHorizontalPan);

  // For exit behavior
  stage.KeyEventSignal().Connect( this, &ImageActorExample::OnKeyEvent );

  RenderTask renderTask = Stage::GetCurrent().GetRenderTaskList().CreateTask();
  mCameraActor = CameraActor::New();
  mCameraActor.SetType(Dali::Camera::FREE_LOOK);
  mCameraActor.SetRotation(Quaternion(Math::PI, Vector3::YAXIS));
  mCameraActor.SetParentOrigin(ParentOrigin::CENTER);
  stage.Add(mCameraActor);
  renderTask.SetCameraActor( mCameraActor );
  renderTask.SetSourceActor( mRootActor );
  renderTask.SetExclusive(true);

  // To keep dali running constantly:
  TextActor a = TextActor::New("a");
  stage.Add(a);
  a.SetParentOrigin( ParentOrigin::TOP_LEFT );
  a.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
  Animation anim = Animation::New(1);
  anim.SetLooping(true);
  anim.RotateBy(a, Radian(1), Vector3::YAXIS );
  anim.Play();
}

void ImageActorExample::OnPan( Actor actor, PanGesture panGesture )
{
  if( actor == mRotateAnyPad )
  {
    if( panGesture.displacement != Vector2::ZERO && (panGesture.state == Gesture::Started ||panGesture.state == Gesture::Continuing ))
    {
      Vector3 size = actor.GetCurrentSize();
      Vector2 a = panGesture.position - Vector2(size*0.5f);
      Vector2 b = a - panGesture.displacement;

      // Other pan
      a.Normalize();
      b.Normalize();
      float dot = a.x*b.x + a.y*b.y;
      if( -1.0f <= dot && dot <= 1.0f )
      {
        float angle = acosf(dot);

        // Determine direction:
        Vector2 c = b-a;
        if( (a.x > 0 && c.y > 0) || (a.x < 0 && c.y < 0))
        {
          angle = -angle;
        }

        mImageActor.RotateBy(Radian(angle), Vector3::ZAXIS);
      }
    }
  }
  else if( actor == mDepthPad )
  {
    mImageActor.MoveBy(Vector3(0.0f, 0.0f, panGesture.displacement.y));
  }
  else if( actor == mPanPad )
  {
    // Move the default camera
    mCameraActor.MoveBy( Vector3( panGesture.displacement) );
  }
  else
  {
    mImageActor.MoveBy( Vector3( panGesture.displacement ) );
  }
}

void ImageActorExample::OnHorizontalPan( Actor actor, PanGesture panGesture )
{
  mImageActor.RotateBy( Degree( panGesture.displacement.x) , Vector3::YAXIS );
}

void ImageActorExample::OnVerticalPan( Actor actor, PanGesture panGesture )
{
  mImageActor.RotateBy( Degree( panGesture.displacement.y), Vector3::XAXIS );
}


void ImageActorExample::OnPinch( Actor actor, PinchGesture pinchGesture )
{
}

void ImageActorExample::AnimationCompleted(Animation& source)
{
  mAnimation.Reset();
}

/**
 * Main key event handler
 */
void ImageActorExample::OnKeyEvent(const KeyEvent& event)
{
  if(event.state == KeyEvent::Down)
  {
    if( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
    {
      mApplication.Quit();
    }
  }
}

void RunTest( Application& application )
{
  ImageActorExample test( application );

  application.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main(int argc, char **argv)
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
