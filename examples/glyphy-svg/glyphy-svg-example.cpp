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

#include <stdio.h>
#include <string.h>
#include <math.h>
#define NANOSVG_ALL_COLOR_KEYWORDS  // Include full list of color keywords.
#define NANOSVG_IMPLEMENTATION      // Expands implementation
#include "nanosvg.h"

#include "glyphy-svg-effect.h"
#include "glyphy-atlas.h"
#include "demo-svg.h"
#include "demo-buffer.h"

namespace
{
const char* SVG_IMAGES[] =
{
  DALI_IMAGE_DIR "Olive_Wreath_clip_art.svg",
  DALI_IMAGE_DIR "Sepia_Film_Strip_clip_art.svg",
  DALI_IMAGE_DIR "core_check_bg_stroke.svg",
  DALI_IMAGE_DIR "core_check_icon.svg",
  DALI_IMAGE_DIR "aquiarius.svg",
  DALI_IMAGE_DIR "Sign_No_Smoking.svg",
  DALI_IMAGE_DIR "Blue_Yin_Yang_clip_art.svg",
  DALI_IMAGE_DIR "Kotik_Cannabis_clip_art.svg",
  DALI_IMAGE_DIR "drawing.svg"
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
    mAtlas = new GlyphyAtlas( 2048, 1024, 64, 8 );
    mBuffer = demo_buffer_create ();
    glyphy_point_t top_left = {0, 0};
    demo_buffer_move_to (mBuffer, &top_left);
    mSVG = demo_svg_create( mAtlas );

    NSVGimage* image;
    Dali::Vector<float> shapeInfo;
    for( unsigned int i = 0; i < NUM_SVG_IMAGES; i++ )
    {
      image = nsvgParseFromFile(SVG_IMAGES[i], "px", 96);
      for (NSVGshape* shape = image->shapes; shape != NULL; shape = shape->next)
      {
        float opacity = pack_opacity( (shape->fill).color, (shape->stroke).color );
        float fillColor = pack_rgb( (shape->fill).color );
        float strokeColor = pack_rgb( (shape->stroke).color);
        float strokeWidth = shape->strokeWidth;
        shapeInfo.PushBack(  strokeWidth );
        shapeInfo.PushBack(  fillColor );
        shapeInfo.PushBack(  strokeColor );
        shapeInfo.PushBack(  opacity );

        demo_buffer_add_svg_shape(mBuffer, shape, mSVG);

      }
      top_left.y -= image->height;
      demo_buffer_move_to (mBuffer, &top_left);
      nsvgDelete(image);
    }

    // create mesh actor with buffer->vertices
    mCustomMaterial = Material::New("CustomMaterial");
    MeshData meshData;
    ConstructMesh(meshData, mBuffer->vertices, shapeInfo);
    mActor = MeshActor::New( Mesh::New(meshData) );
    mScale = 1.f;
    mActor.SetScale( mScale, -mScale,1.f );
    mActor.SetCullFace( CullNone );
    mActor.SetColor( Vector4( 0.f, 0.f, 0.f, 1.f ) );
    mActor.SetAnchorPoint( AnchorPoint::CENTER );
    mActor.SetParentOrigin( ParentOrigin::CENTER );
    stage.Add( mActor );

    mShaderEffect = GlyphySVGEffect::New();
    Vector4 atlasInfo( static_cast<float>(mAtlas->GetTextureWidth()),
                       static_cast<float>(mAtlas->GetTextureHeight()),
                       static_cast<float>(mAtlas->GetItemWidth()),
                       static_cast<float>(mAtlas->GetItemHeightQuantum()));
    mShaderEffect.SetAtlasInfo( atlasInfo );
    mCustomMaterial.SetDiffuseTexture( mAtlas->GetAtlasTexture() );
    mShaderEffect.ConstraintStrokeScale( Source( mActor, Actor::Property::SCALE_X ) );
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

  float pack_opacity(unsigned int fillColor, unsigned int strokeColor)
  {
    unsigned int opacity = (fillColor >> 24) | ( (strokeColor >> 24) << 8 );
    return opacity & 0x0000FFFF;
  }

  Vector2 unpack_opacity( float f )
  {
    unsigned int F = int(f);
    unsigned int fillOpacity =  F % 256u;
    F = (F - fillOpacity)/256u;
    unsigned int strokeOpacity = F % 256u;
    return Vector2( fillOpacity, strokeOpacity  )/255.0;
  }

  float pack_rgb(unsigned int color)
  {
    return color & 0x00FFFFFF;
  }

  Vector3 unpack_rgb(float f)
  {
    unsigned int F = f;

    printf( "F: 0x%8.8XL\n", *(int*)&F ) ;

    unsigned int r = F % 256u;
    F = (F - r)/256u;
    unsigned int g = F % 256u;
    F = (F - g)/256u;
    unsigned int b = F % 256u;
    return Vector3(r,g,b);
  }

  Vector4 RGBA_Color( unsigned int color )
  {
    unsigned int r = color & 0xFF;
    unsigned int g = (color >> 8) & 0xFF;
    unsigned int b = (color >> 16)& 0xFF;
    unsigned int a = (color >> 24)& 0xFF;
    return Vector4( r, g, b, a);
  }

  void OnPanGesture( Actor actor, const PanGesture& gesture )
  {
    if( gesture.state == Gesture::Continuing )
    {
      mActor.TranslateBy(Vector3(gesture.displacement));
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
      else
      {
        const char* keyName = event.keyPressedName.c_str();
        if( strcmp(keyName, "Left") == 0 )
        {
          mScale /= 1.1f;
          mActor.SetScale( mScale, -mScale, 1.f );
        }
        else if( strcmp(keyName, "Right") == 0 )
        {
          mScale *= 1.1f;
          mActor.SetScale( mScale, -mScale, 1.f );
        }
    }
  }
  }

  void AddVertex(MeshData::VertexContainer& vertices,
                 float x, float y, float z,
                 float u, float v,
                 float nX, float nY, float nZ)
  {
    MeshData::Vertex meshVertex;
    meshVertex.x = x;
    meshVertex.y = y;
    meshVertex.z = z;
    meshVertex.u = u;
    meshVertex.v = v;
    meshVertex.nX = nX;
    meshVertex.nY = nY;
    meshVertex.nZ = nZ;

    vertices.push_back(meshVertex);
  }

  void AddTriangle(MeshData::FaceIndices& faces,
      size_t v0, size_t v1, size_t v2)
  {
    faces.push_back(v0);
    faces.push_back(v2);
    faces.push_back(v1);
  }

  void ConstructMesh( MeshData& meshData,
                      std::vector<glyph_vertex_t>* glyphVertices,
                      const Dali::Vector<float>& shapeInfo )
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
    float centerX = (minX+maxX)/2.f;
    float centerY = (minY+maxY)/2.f;

    // four values per pixel, need to use vertex position and vertex texture!!
    unsigned int shapeIdx;
    for(unsigned int index = 0; index < numTriangles; index++)
    {
      shapeIdx = ( idx / 6 ) * 4;
      AddVertex( vertices,
                 glyphVertices->at(idx).x - centerX, glyphVertices->at(idx).y - centerY, shapeInfo[shapeIdx],
                 glyphVertices->at(idx).g16hi, glyphVertices->at(idx).g16lo,
                 shapeInfo[shapeIdx+1], shapeInfo[shapeIdx+2], shapeInfo[shapeIdx+3]);
      AddVertex( vertices,
                 glyphVertices->at(idx+1).x - centerX, glyphVertices->at(idx+1).y - centerY,shapeInfo[shapeIdx],
                 glyphVertices->at(idx+1).g16hi, glyphVertices->at(idx+1).g16lo,
                 shapeInfo[shapeIdx+1], shapeInfo[shapeIdx+2], shapeInfo[shapeIdx+3]);
      AddVertex( vertices,
                 glyphVertices->at(idx+2).x - centerX, glyphVertices->at(idx+2).y - centerY,shapeInfo[shapeIdx],
                 glyphVertices->at(idx+2).g16hi, glyphVertices->at(idx+2).g16lo,
                 shapeInfo[shapeIdx+1], shapeInfo[shapeIdx+2], shapeInfo[shapeIdx+3]);
      AddTriangle( faces, idx, idx+1, idx+2 );
      idx += 3;
    }

    meshData.SetFaceIndices( faces );
    meshData.SetVertices(vertices);
    meshData.SetMaterial( mCustomMaterial);
    meshData.SetHasNormals(true);
    meshData.SetHasTextureCoords(true);
  }

private:

  Application&     mApplication;

  GlyphySVGEffect mShaderEffect;
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
