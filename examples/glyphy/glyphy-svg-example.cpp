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
#include <dali/dali.h>
using namespace Dali;

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

#include "glyphy-text-effect.h"
#include "glyphy-atlas.h"
#include "demo-svg.h"
#include "glyphy-demo/demo-buffer.h"

namespace
{
const char* SVG_IMAGES[] =
{
  DALI_IMAGE_DIR "baidu.svg",
  DALI_IMAGE_DIR "designmoo.svg",
  DALI_IMAGE_DIR "dropbox.svg",
  DALI_IMAGE_DIR "ebay.svg",
  DALI_IMAGE_DIR "facebook.svg",
  DALI_IMAGE_DIR "github.svg",
  DALI_IMAGE_DIR "gowalla.svg",
  DALI_IMAGE_DIR "icq.svg",
  DALI_IMAGE_DIR "mixx.svg",
  DALI_IMAGE_DIR "paypal.svg",
  DALI_IMAGE_DIR "ping.svg",
  DALI_IMAGE_DIR "playstation.svg",
  DALI_IMAGE_DIR "twitter.svg",
  DALI_IMAGE_DIR "youtube.svg"
};

const unsigned int NUM_SVG_IMAGES( sizeof( SVG_IMAGES ) / sizeof( SVG_IMAGES[0] ) );

}

class GlyphySVGController : public ConnectionTracker
{
public:

  GlyphySVGController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &GlyphySVGController::Create );
  }

  ~GlyphySVGController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();
    stage.SetBackgroundColor(Color::WHITE);
    stage.KeyEventSignal().Connect(this, &GlyphySVGController::OnKeyEvent);

    // Load
    mAtlas = new GlyphyAtlas( 512, 512, 64, 8 );
    mBuffer = demo_buffer_create ();
    glyphy_point_t top_left = {0, 0};
    demo_buffer_move_to (mBuffer, &top_left);
    mSVG = demo_svg_create( mAtlas );

    NSVGimage* image;
    for( unsigned int i = 0; i < NUM_SVG_IMAGES; i++ )
    {
      image = nsvgParseFromFile(SVG_IMAGES[i], "px", 92);
      demo_buffer_add_svg (mBuffer, image, mSVG);
      top_left.y -= image->height;
      demo_buffer_move_to (mBuffer, &top_left);
      nsvgDelete(image);
    }

    // create mesh actor with buffer->vertices
    mCustomMaterial = Material::New("CustomMaterial");
    MeshData meshData;
    ConstructMesh(meshData, mBuffer->vertices);
    mActor = MeshActor::New( Mesh::New(meshData) );

    mActor.SetAffectedByLighting( false );
    mScale = 2.f;
    mActor.SetScale( 2.f, -2.f,1.f );
    mActor.SetCullFace( CullNone );
    mActor.SetColor( Vector4( 0.f, 0.f, 0.f, 1.f ) );
    mActor.SetAnchorPoint( AnchorPoint::CENTER );
    mActor.SetParentOrigin( ParentOrigin::CENTER );
    stage.Add( mActor );

    mShaderEffect = GlyphyTextEffect::New();
    Vector4 atlasInfo( static_cast<float>(mAtlas->GetTextureWidth()),
                       static_cast<float>(mAtlas->GetTextureHeight()),
                       static_cast<float>(mAtlas->GetItemWidth()),
                       static_cast<float>(mAtlas->GetItemHeightQuantum()));
    mShaderEffect.SetAtlasInfo( atlasInfo );
    mCustomMaterial.SetDiffuseTexture( mAtlas->GetAtlasTexture() );
    mActor.SetShaderEffect( mShaderEffect );

    // Background, for receiving gestures
    mStageBackground = Actor::New();
    mStageBackground.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mStageBackground.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mStageBackground.SetSize( stageSize.x, stageSize.y );
    stage.Add(mStageBackground);

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.DetectedSignal().Connect( this, &GlyphySVGController::OnPanGesture );
    mPanGestureDetector.Attach( mActor );
    mPanGestureDetector.Attach( mStageBackground );

    mPinchGestureDetector = PinchGestureDetector::New();
    mPinchGestureDetector.Attach( mActor );
    mPinchGestureDetector.Attach( mStageBackground);
    mPinchGestureDetector.DetectedSignal().Connect(this, &GlyphySVGController::OnPinch);
  }

  void OnPanGesture( Actor actor, const PanGesture& gesture )
  {
    if( gesture.state == Gesture::Continuing )
    {
      mActor.MoveBy(Vector3(gesture.displacement));
    }
  }

  void OnPinch(Actor actor, const PinchGesture& gesture)
  {
    if (gesture.state == Gesture::Started)
    {
      mScaleAtPinchStart = mScale;
    }
    mScale = mScaleAtPinchStart * gesture.scale;
    mActor.SetScale( mScale, -mScale, 1.f );
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApplication.Quit();
      }
    }
  }

  void AddVertex(MeshData::VertexContainer& vertices, float x, float y, float u, float v)
  {
    MeshData::Vertex meshVertex;
    meshVertex.x = x;
    meshVertex.y = y;
    meshVertex.u = u;
    meshVertex.v = v;
    vertices.push_back(meshVertex);
  }

  void AddTriangle(MeshData::FaceIndices& faces,
      size_t v0, size_t v1, size_t v2)
  {
    faces.push_back(v0);
    faces.push_back(v2);
    faces.push_back(v1);
  }

  void ConstructMesh( MeshData& meshData, std::vector<glyph_vertex_t>* glyphVertices )
  {
    MeshData::VertexContainer    vertices;
    MeshData::FaceIndices        faces;

    unsigned int numTriangles = glyphVertices->size () / 3;
    unsigned int idx = 0;

    float minX = 1000.f, minY=1000.f, maxX=-1000.f, maxY=-1000.f;
    for(unsigned int index = idx; index < glyphVertices->size() ; index++)
    {
      if(glyphVertices->at(index).x < minX ) minX = glyphVertices->at(index).x;
      if(glyphVertices->at(index).x > maxX ) maxX = glyphVertices->at(index).x;
      if(glyphVertices->at(index).y < minY ) minY = glyphVertices->at(index).y;
      if(glyphVertices->at(index).y > maxY ) maxY = glyphVertices->at(index).y;
    }
    std::cout<<"width: "<<maxX<<" "<<minX<<"   "<<"height: "<<maxY<<" "<<minY<<std::endl;
    float centerX = (minX+maxX)/2.f;
    float centerY = (minY+maxY)/2.f;

    // four values per pixel, need to use vertex position and vertex texture!!
    for(unsigned int index = 0; index < numTriangles; index++)
    {
      AddVertex( vertices, glyphVertices->at(idx).x - centerX, glyphVertices->at(idx).y - centerY, glyphVertices->at(idx).g16hi, glyphVertices->at(idx).g16lo );
      AddVertex( vertices, glyphVertices->at(idx+1).x - centerX, glyphVertices->at(idx+1).y - centerY, glyphVertices->at(idx+1).g16hi, glyphVertices->at(idx+1).g16lo );
      AddVertex( vertices, glyphVertices->at(idx+2).x - centerX, glyphVertices->at(idx+2).y - centerY, glyphVertices->at(idx+2).g16hi, glyphVertices->at(idx+2).g16lo );
      AddTriangle( faces, idx, idx+1, idx+2 );
      idx += 3;
    }

    meshData.SetFaceIndices( faces );
    meshData.SetVertices(vertices);
    meshData.SetMaterial( mCustomMaterial);
    meshData.SetHasColor(false);
    meshData.SetHasTextureCoords(true);
  }

private:

  Application&     mApplication;

  GlyphyTextEffect mShaderEffect;
  GlyphyAtlas*     mAtlas;
  demo_buffer_t*   mBuffer;
  demo_svg_t*      mSVG;

  Material         mCustomMaterial;
  MeshActor        mActor;
  Actor            mStageBackground;

  PanGestureDetector    mPanGestureDetector;

  PinchGestureDetector  mPinchGestureDetector;
  float mScale;
  float mScaleAtPinchStart;
};

void RunTest( Application& application )
{
  GlyphySVGController test( application );

  application.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
