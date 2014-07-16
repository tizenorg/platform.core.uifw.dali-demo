/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Demo

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

#include "../shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const char* BACKGROUND_IMAGE = DALI_IMAGE_DIR "background-gradient.jpg";
const char* TOOLBAR_IMAGE = DALI_IMAGE_DIR "top-bar.png";
const char* APPLICATION_TITLE = "Image Samplers";
const char* VIEW_SWAP_IMAGE = DALI_IMAGE_DIR "icon-change.png";

const Vector3 BACK_POINT( -0.5f,-0.5f, 0.0f );
const Vector3 LEFT_POINT(-0.5f, 0.5f, 0.0f );
const Vector3 RIGHT_POINT( 0.5f, -0.5f, 0.0f );
const Vector3 TOP_POINT( 0.5f, 0.5f, 0.0f );

}

static const float TEST_IMAGE_WIDTH  = 128.0f;
static const float TEST_IMAGE_HEIGHT = 128.0f;

static const char* TEST_IMAGE_FILENAME = DALI_IMAGE_DIR "blocks-ball.png";
static const char* TEST_IMAGE_FILENAME2 = DALI_IMAGE_DIR "gallery-small-42.jpg";

// This example shows how to create an image actor for displaying an image
//
class ImageActorExample : public ConnectionTracker
{
public:

  ImageActorExample( Application& application );

  ~ImageActorExample();

  void OnInit( Application& application );

  void OnKeyEvent( const KeyEvent& event );
  bool OnView( Toolkit::Button button );

private:

  enum Scenes
  {
    TEXTURE_FILTERING,
    WRAPPING_REPEAT,
    TEXT_FILTERING,

    MAX_SCENES
  };

  void ChangeView();
  void CreateCurrentView();
  void CreateTextureFilteringScene();
  void CreateRepeatScene();
  void CreateTextScene();
  void UnloadView();
  void GenMaterial();
  void ConstructJointMesh( MeshData& meshData, float sz );
  bool OnSliderValueChanged( Toolkit::Slider slider, float value );

private:

  Application&        mApplication;            ///< Application instance
  Toolkit::View       mView;                   ///< The View instance.
  Toolkit::ToolBar    mToolBar;                ///< The View's Toolbar.
  Layer               mContentLayer;           ///< Content layer

  int                 mCurrentScene;

  Mesh mMesh;

  ImageActor mImageActor, mImageActor2, mImageActor3, mImageActor4, mImageActor5, mImageActor6;
  MeshActor mMeshActor;

  Toolkit::Slider mSlider;

  TextView mTextView;

  Material mCustomMaterial;

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

void ImageActorExample::OnInit( Application& application )
{
  // The Init signal is received once (only) during the Application lifetime

  Stage stage = Stage::GetCurrent();

  // Creates a default view with a default tool bar.
  // The view is added to the stage.
  mContentLayer = DemoHelper::CreateView( application,
                                          mView,
                                          mToolBar,
                                          BACKGROUND_IMAGE,
                                          TOOLBAR_IMAGE,
                                          APPLICATION_TITLE );

  // Creates view change button.
  Image image = Image::New( VIEW_SWAP_IMAGE );
  Toolkit::PushButton viewButton = Toolkit::PushButton::New();
  viewButton.SetBackgroundImage( image );
  // Connects the view change button clicked signal to the OnView method.
  viewButton.ClickedSignal().Connect( this, &ImageActorExample::OnView );
  mToolBar.AddControl( viewButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

  // For exit behavior
  stage.KeyEventSignal().Connect( this, &ImageActorExample::OnKeyEvent );

  GenMaterial();
  MeshData meshData;
  ConstructJointMesh( meshData, 300 );
  mMesh = Mesh::New( meshData );

  mCurrentScene = 0;
  CreateCurrentView();
}

static void AddVertex(MeshData::VertexContainer& vertices, Vector3 V, Vector2 UV)
{
  MeshData::Vertex meshVertex;
  meshVertex.x = V.x;
  meshVertex.y = V.y;
  meshVertex.z = V.z;
  meshVertex.u = UV.x;
  meshVertex.v = UV.y;
  vertices.push_back(meshVertex);
}

static void SetNormal(MeshData::VertexContainer& verts, size_t vertIdx, Vector3 normal)
{
  verts[vertIdx].nX = normal.x;
  verts[vertIdx].nY = normal.y;
  verts[vertIdx].nZ = normal.z;
}

static void CopyVertex(MeshData::Vertex& vert, Vector3& vector)
{
  vector.x = vert.x;
  vector.y = vert.y;
  vector.z = vert.z;
}

void AddTriangle(MeshData::VertexContainer& verts,
                 MeshData::FaceIndices& faces,
                 size_t v0, size_t v1, size_t v2)
{
  faces.push_back(v0);
  faces.push_back(v1);
  faces.push_back(v2);

  // Calculate normal...
  Vector3 vert0, vert1, vert2;
  CopyVertex(verts[v0], vert0);
  CopyVertex(verts[v1], vert1);
  CopyVertex(verts[v2], vert2);
  Vector3 e0 = vert1 - vert0;
  Vector3 e1 = vert2 - vert1;
  Vector3 normal = e0.Cross(e1);
  normal.Normalize();
  SetNormal(verts, v0, normal);
  SetNormal(verts, v1, normal);
  SetNormal(verts, v2, normal);
}

void ImageActorExample::ConstructJointMesh( MeshData& meshData, float sz )
{
  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones(0);

  const float UV_SCALE = 2.0f;

  // back
  AddVertex(vertices, BACK_POINT*sz, Vector2(0.0f, 0.0f)); // 0a  0
  AddVertex(vertices, BACK_POINT*sz, Vector2(0.0f, 0.0f)); // 0b  1
  AddVertex(vertices, BACK_POINT*sz, Vector2(0.0f, 0.0f)); // 0c  2

  // left
  AddVertex(vertices, LEFT_POINT*sz, Vector2(0.0f, 1.0f * UV_SCALE));  // 1a  3
  AddVertex(vertices, LEFT_POINT*sz, Vector2(0.0f, 1.0f * UV_SCALE));  // 1b  4
  AddVertex(vertices, LEFT_POINT*sz, Vector2(0.0f, 1.0f * UV_SCALE));  // 1c  5

  // right
  AddVertex(vertices, RIGHT_POINT*sz, Vector2(1.0f * UV_SCALE, 0.0f));  // 2a  6
  AddVertex(vertices, RIGHT_POINT*sz, Vector2(1.0f * UV_SCALE, 0.0f));  // 2b  7
  AddVertex(vertices, RIGHT_POINT*sz, Vector2(1.0f * UV_SCALE, 0.0f));  // 2c  8

  // top
  AddVertex(vertices, TOP_POINT*sz, Vector2(1.0f * UV_SCALE, 1.0f * UV_SCALE)); // 3a  9
  AddVertex(vertices, TOP_POINT*sz, Vector2(1.0f * UV_SCALE, 1.0f * UV_SCALE)); // 3b 10
  AddVertex(vertices, TOP_POINT*sz, Vector2(1.0f * UV_SCALE, 1.0f * UV_SCALE)); // 3c 11

  AddTriangle(vertices, faces,  0, 6,  3); // 0, 2, 1  back, right, left (ac)
  AddTriangle(vertices, faces,  1, 9,  7); // 0, 3, 2  back, top , right (ac)
  AddTriangle(vertices, faces,  2, 4, 10); // 0, 1, 3  back, left, top   (ac)
  AddTriangle(vertices, faces, 11, 5,  8); // 3, 1, 2  top, left, right  (ac)

  meshData.SetData(vertices, faces, bones, mCustomMaterial);
  meshData.SetHasColor(false);
  meshData.SetHasNormals(true);
  meshData.SetHasTextureCoords(true);
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

bool ImageActorExample::OnView( Toolkit::Button button )
{
  ChangeView();
  return true;
}

void ImageActorExample::ChangeView()
{
  ++mCurrentScene;
  if( mCurrentScene >= MAX_SCENES )
  {
    mCurrentScene = 0;
  }

  UnloadView();
  CreateCurrentView();
}

void ImageActorExample::CreateCurrentView()
{
  switch( mCurrentScene )
  {
    case TEXTURE_FILTERING:
    {
      CreateTextureFilteringScene();
      break;
    }
    case WRAPPING_REPEAT:
    {
      CreateRepeatScene();
      break;
    }
    case TEXT_FILTERING:
    default:
    {
      CreateTextScene();
      break;
    }
  }
}

void ImageActorExample::CreateTextureFilteringScene()
{
  Stage stage = Stage::GetCurrent();

  {
    // Create the image
    Image image = Image::New( TEST_IMAGE_FILENAME );
    mImageActor = ImageActor::New( image );

    // Set the image size
    mImageActor.SetSize( TEST_IMAGE_WIDTH, TEST_IMAGE_HEIGHT);

    // Center the image
    mImageActor.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mImageActor.SetAnchorPoint( AnchorPoint::CENTER );
    mImageActor.SetPosition( -50.0f, 200.0f );

    // Display the actor on the stage
    stage.Add( mImageActor );
  }

  {
    // Create the image top right
    Image image = Image::New( TEST_IMAGE_FILENAME );
    mImageActor6 = ImageActor::New( image );
    mImageActor6.SetFilterMode( FilterMode::NEAREST, FilterMode::LINEAR );

    // Set the image size
    mImageActor6.SetSize( TEST_IMAGE_WIDTH, TEST_IMAGE_HEIGHT);

    // Center the image
    mImageActor6.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mImageActor6.SetAnchorPoint( AnchorPoint::CENTER );
    mImageActor6.SetPosition( 50.0f, 200.0f );

    // Display the actor on the stage
    stage.Add( mImageActor6 );
  }

  {
    // Bottom image left
    Image image = Image::New( TEST_IMAGE_FILENAME );
    mImageActor2 = ImageActor::New( image );
    mImageActor2.SetFilterMode( FilterMode::NEAREST, FilterMode::NEAREST );

    // Set the image size
    mImageActor2.SetSize( TEST_IMAGE_WIDTH, TEST_IMAGE_HEIGHT);

    // Center the image
    mImageActor2.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    mImageActor2.SetAnchorPoint( AnchorPoint::CENTER );
    mImageActor2.SetPosition( -50.0f, -200.0f );

    // Display the actor on the stage
    stage.Add( mImageActor2 );
  }

  {
    // Bottom image right
    Image image = Image::New( TEST_IMAGE_FILENAME );
    mImageActor5 = ImageActor::New( image );
    mImageActor5.SetFilterMode( FilterMode::LINEAR, FilterMode::NEAREST );

    // Set the image size
    mImageActor5.SetSize( TEST_IMAGE_WIDTH, TEST_IMAGE_HEIGHT);

    // Center the image
    mImageActor5.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    mImageActor5.SetAnchorPoint( AnchorPoint::CENTER );
    mImageActor5.SetPosition( 50.0f, -200.0f );

    // Display the actor on the stage
    stage.Add( mImageActor5 );
  }

  {
    // Left image
    Image image = Image::New( TEST_IMAGE_FILENAME );
    mImageActor3 = ImageActor::New( image );

    // Set the image size
    mImageActor3.SetSize( TEST_IMAGE_WIDTH, TEST_IMAGE_HEIGHT);

    // Center the image
    mImageActor3.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mImageActor3.SetAnchorPoint( AnchorPoint::CENTER );
    mImageActor3.SetPosition( -50.0f, 300.0f );

    // Display the actor on the stage
    stage.Add( mImageActor3 );
  }

  {
    // Right image
    Image image = Image::New( TEST_IMAGE_FILENAME );
    mImageActor4 = ImageActor::New( image );
    mImageActor4.SetFilterMode( FilterMode::LINEAR, FilterMode::LINEAR );

    // Set the image size
    mImageActor4.SetSize( TEST_IMAGE_WIDTH, TEST_IMAGE_HEIGHT);

    // Center the image
    mImageActor4.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mImageActor4.SetAnchorPoint( AnchorPoint::CENTER );
    mImageActor4.SetPosition( 50.0f, 300.0f );

    // Display the actor on the stage
    stage.Add( mImageActor4 );
  }

  // Slider
  mSlider = Toolkit::Slider::New();
  mSlider.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
  mSlider.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  mSlider.SetSize( stage.GetSize().x, 48.0f );
  mSlider.ValueChangedSignal().Connect( this, &ImageActorExample::OnSliderValueChanged );

  mSlider.SetProperty( Slider::LOWER_BOUND_PROPERTY, 0.0f );
  mSlider.SetProperty( Slider::UPPER_BOUND_PROPERTY, 10.0f );
  mSlider.SetProperty( Slider::SHOW_VALUE_PROPERTY, true );
  mSlider.SetProperty( Slider::VALUE_PRECISION_PROPERTY, 2 );
  mSlider.SetProperty( Slider::VALUE_PROPERTY, 1.0f );
  mSlider.SetProperty( Slider::SHOW_POPUP_PROPERTY, false );

  stage.Add( mSlider );
}

bool ImageActorExample::OnSliderValueChanged( Toolkit::Slider slider, float value )
{
  mImageActor.SetScale( value );
  mImageActor2.SetScale( value );
  mImageActor3.SetScale( value );
  mImageActor4.SetScale( value );
  mImageActor5.SetScale( value );
  mImageActor6.SetScale( value );

  return true;
}

void ImageActorExample::GenMaterial()
{
  mCustomMaterial = Material::New("CustomMaterial");
  mCustomMaterial.SetOpacity(1.0f);
  mCustomMaterial.SetDiffuseColor(Color::WHITE);
  mCustomMaterial.SetAmbientColor(Vector4(0.0, 0.1, 0.1, 1.0));
  mCustomMaterial.SetMapU( Material::MAPPING_MODE_WRAP );
  mCustomMaterial.SetMapV( Material::MAPPING_MODE_WRAP );

  Image image = Image::New( TEST_IMAGE_FILENAME2 );

  mCustomMaterial.SetDiffuseTexture(image);
}

void ImageActorExample::CreateRepeatScene()
{
  Stage stage = Stage::GetCurrent();

  mMeshActor = MeshActor::New(mMesh);

  // Set the image size
  mMeshActor.SetScale( 2.0f );

  // Center the image
  mMeshActor.SetParentOrigin( ParentOrigin::CENTER );
  mMeshActor.SetAnchorPoint( AnchorPoint::CENTER );
  mMeshActor.SetAffectedByLighting(false);

  // Display the actor on the stage
  stage.Add( mMeshActor );
}

void ImageActorExample::CreateTextScene()
{
  Stage stage = Stage::GetCurrent();

  std::string title( "Hello Test!" );
  mTextView = TextView::New();
  mTextView.SetText( title );
  Dali::Font font = Dali::Font::New();
  mTextView.SetSize( font.MeasureText( title ) );
  mTextView.SetColor( Color::BLUE );
  mTextView.SetParentOrigin( ParentOrigin::CENTER );
  mTextView.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( mTextView );
}

void ImageActorExample::UnloadView()
{
  UnparentAndReset( mImageActor );
  UnparentAndReset( mImageActor2 );
  UnparentAndReset( mImageActor3 );
  UnparentAndReset( mImageActor4 );
  UnparentAndReset( mImageActor5 );
  UnparentAndReset( mImageActor6 );
  UnparentAndReset( mMeshActor );
  UnparentAndReset( mTextView );
  UnparentAndReset( mSlider );
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

