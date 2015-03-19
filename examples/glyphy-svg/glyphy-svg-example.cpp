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
#include <dali-toolkit/public-api/controls/buttons/push-button.h>
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
const char* PUSHBUTTON_BUTTON_IMAGE = DALI_IMAGE_DIR "icon-change.png";

const char* SVG_IMAGES[] =
{
  DALI_IMAGE_DIR "Camera.svg",
  DALI_IMAGE_DIR "Contacts.svg",
  DALI_IMAGE_DIR "FaceBook.svg",
  DALI_IMAGE_DIR "Mail.svg",
  DALI_IMAGE_DIR "Message.svg",
  DALI_IMAGE_DIR "Phone.svg",
  DALI_IMAGE_DIR "Settings.svg",
  DALI_IMAGE_DIR "Twitter.svg",
  DALI_IMAGE_DIR "World.svg",
  DALI_IMAGE_DIR "Kid1.svg",
  DALI_IMAGE_DIR "aquiarius.svg"
};

const unsigned int NUM_SVG_IMAGES( sizeof( SVG_IMAGES ) / sizeof( SVG_IMAGES[0] ) );
}

class GlyphySVGController : public ConnectionTracker
{
public:

  GlyphySVGController( Application& application )
  : mApplication( application ),
    mIndex( 0 )
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

    mActor = CreateMeshActorFromSVG(SVG_IMAGES[mIndex]);
    mScale = 1.f;
    mActor.SetScale( mScale, -mScale,1.f );
    stage.Add( mActor );
    mIndex++;

    Toolkit::PushButton button = Toolkit::PushButton::New();
    button.SetButtonImage( ResourceImage::New( PUSHBUTTON_BUTTON_IMAGE ) );
    button.SetZ(1.f);
    button.SetScale(2.f);
    button.SetAnchorPoint( AnchorPoint::TOP_RIGHT );
    button.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    stage.Add( button );

    button.ClickedSignal().Connect( this, &GlyphySVGController::OnButtonClicked );

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

  bool OnButtonClicked(Toolkit::Button button)
  {
    Stage::GetCurrent().Remove(mActor);
    mActor = CreateMeshActorFromSVG(SVG_IMAGES[mIndex]);
    mScale = 1.f;
    mActor.SetScale( mScale, -mScale,1.f );
    Stage::GetCurrent().Add( mActor );
    mIndex++;

    mIndex %= NUM_SVG_IMAGES;

    return true;
  }

  MeshActor CreateMeshActorFromSVG( const char* svgFile )
  {
    // Load
    GlyphyAtlas* glyphyAtlas = new GlyphyAtlas( 512, 512, 64, 8 );
    demo_buffer_t* demoBuffer = demo_buffer_create ();
    glyphy_point_t top_left = {0, 0};
    demo_buffer_move_to (demoBuffer, &top_left);
    demo_svg_t*  mSVG = demo_svg_create( glyphyAtlas );

    NSVGimage* image = nsvgParseFromFile(svgFile, "px", 96);

    Dali::Vector<float> shapeInfo; // fill_color, stroke_color, fill_gradient ect.

    //fill_linear_gradient
    unsigned int gradientCount = 0;
    Dali::Vector<float> gradientInfo;
    Dali::Vector<int> nstops;
    Dali::Vector< NSVGgradientStop*> stops;

    for (NSVGshape* shape = image->shapes; shape != NULL; shape = shape->next)
    {
      demo_buffer_add_svg_shape(demoBuffer, shape, mSVG);

      unsigned int fill_color = (shape->fill).color;
      char type = (shape->fill).type;
      unsigned int gradientIndex = 0;
      if( type == 0 )//fill_none
      {
        fill_color = 0x00000000;
      }
     /* else if( type == 1 )//fill_solid_color
      {
        fill_color = (shape->fill).color;
      }*/
      else if( type == 2 || type == 3) // fill_gradient
      {
        NSVGgradient* gradient = (shape->fill).gradient;
        pack_gradient_info( gradientInfo, type, gradient->xform );

        nstops.PushBack( gradient->nstops );
        stops.PushBack( gradient->stops );
        gradientCount++;
        gradientIndex = gradientCount;
      }
      float opacity = pack_opacity( gradientIndex, fill_color, (shape->stroke).color, shape->opacity );
      float fillColor = pack_rgb( fill_color );
      float strokeColor = pack_rgb( (shape->stroke).color);
      float strokeWidth = shape->strokeWidth;
      shapeInfo.PushBack(  strokeWidth );
      shapeInfo.PushBack(  fillColor );
      shapeInfo.PushBack(  strokeColor );
      shapeInfo.PushBack(  opacity );


    }

    // create mesh actor with buffer->vertices
    Material customMaterial = Material::New("CustomMaterial");
    MeshData meshData;
    ConstructMesh(meshData, demoBuffer->vertices, shapeInfo);
    meshData.SetMaterial( customMaterial);
    MeshActor svgActor = MeshActor::New( Mesh::New(meshData) );
    svgActor.SetCullFace( CullNone );
    svgActor.SetColor( Vector4( 0.f, 0.f, 0.f, 1.f ) );
    svgActor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    svgActor.SetParentOrigin( Vector3(0.05f,0.1f,0.5f) );

    GlyphySVGEffect shaderEffect = GlyphySVGEffect::New(gradientCount);
    Vector4 atlasInfo( static_cast<float>(glyphyAtlas->GetTextureWidth()),
                       static_cast<float>(glyphyAtlas->GetTextureHeight()),
                       static_cast<float>(glyphyAtlas->GetItemWidth()),
                       static_cast<float>(glyphyAtlas->GetItemHeightQuantum()));
    shaderEffect.SetAtlasInfo( atlasInfo );
    customMaterial.SetDiffuseTexture( glyphyAtlas->GetAtlasTexture() );

    shaderEffect.ConstraintStrokeScale( Source( svgActor, Actor::Property::SCALE_X ) );

    if( gradientCount > 0 )
    {
      BufferImage lookupTexture = generate_lookup_texture( gradientCount, nstops,stops );
      shaderEffect.SetEffectImage( lookupTexture );
      shaderEffect.SetGradientInfo(gradientInfo);
    }

    svgActor.SetShaderEffect( shaderEffect );

    nsvgDelete(image);

    return svgActor;
  }

  float pack_opacity(unsigned int gradientIndex, unsigned int fillColor, unsigned int strokeColor, float shapeOpacity)
  {
    float shape_opacity = Clamp(shapeOpacity, 0.f, 1.f);
    unsigned int fillOpacity = static_cast<unsigned int>( static_cast<float>(fillColor >> 24) * shape_opacity );
    unsigned int strokeOpacity = static_cast<unsigned int>( static_cast<float>( strokeColor >> 24 ) * shape_opacity );
    unsigned int opacity = fillOpacity | ( strokeOpacity << 8 ) | ( gradientIndex << 16 );
    return opacity & 0x00FFFFFF;
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

  void pack_gradient_info( Dali::Vector<float>& gradientInfo, char type, float* xform )
  {
    if( type == 3 ) // radial gradient
    {
      gradientInfo.PushBack(xform[0]);
      gradientInfo.PushBack(xform[2]);
      gradientInfo.PushBack(xform[4]);
    }
    else
    {
      gradientInfo.PushBack(0.f);
      gradientInfo.PushBack(0.f);
      gradientInfo.PushBack(0.f);
    }
    gradientInfo.PushBack(xform[1]);
    gradientInfo.PushBack(xform[3]);
    gradientInfo.PushBack(xform[5]);
  }

  BufferImage generate_lookup_texture( int numGradient,
                                       const Dali::Vector<int>& nstops,
                                       const Dali::Vector< NSVGgradientStop*>& stops )
  {
    int resolution = 128;
    float length = static_cast<float>(resolution);
    BufferImage colorLookup = BufferImage::New( resolution, numGradient, Pixel::RGBA8888 );
    unsigned int bytesPerPixel = Pixel::GetBytesPerPixel(Pixel::RGBA8888);
    PixelBuffer* pixbuf = colorLookup.GetBuffer();

    // ToDo: currently only handling spread == pad;
    //       needs to add support for spread == repeated / reflected
    int numPixelPerRow = resolution*bytesPerPixel;
    for( int n = 0; n < numGradient; n++ )
    {
      int offset = n*numPixelPerRow;
      int numStops = nstops[n];
      NSVGgradientStop* curStops = stops[n];

      // spead == pad, spread the first stop color to  0.0 ~ first stop
      if( curStops[0].offset > 0.f )
      {
        Vector4 color = RGBA_Color( curStops[0].color );
        int start = 0;
        int end = floorf(curStops[0].offset * length + 0.5f);
        for( int j = start; j< end; j++)
        {
          int index = offset + j*bytesPerPixel;
          pixbuf[index] = color.r;
          pixbuf[index+1] = color.g;
          pixbuf[index+2] = color.b;
          pixbuf[index+3] = color.a;
        }
      }

      // spread == pad, spread the last stop color to lastStop ~ 1.0
      if( curStops[numStops-1].offset < 1.f)
      {
        Vector4 color = RGBA_Color( curStops[numStops-1].color );
        int start = floorf(curStops[numStops-1].offset * length + 0.5f);
        int end = resolution;
        for( int j = start; j< end; j++)
        {
          int index = offset + j*bytesPerPixel;
          pixbuf[index] = color.r;
          pixbuf[index+1] = color.g;
          pixbuf[index+2] = color.b;
          pixbuf[index+3] = color.a;
        }
      }

      for( int i = 0; i<numStops-1; i++ )
      {
        Vector4 startColor = RGBA_Color( curStops[i].color );
        Vector4 endColor = RGBA_Color( curStops[i+1].color );
        int start = floorf(curStops[i].offset * 128.f + 0.5f );
        int end = floorf(curStops[i+1].offset * 128.f + 0.5f);
        for( int j = start; j< end; j++)
        {
          float ratio = static_cast<float>(j-start)/static_cast<float>(end-start);
          Vector4 currentColor = startColor * (1.f-ratio) + endColor * ratio;
          int index = offset + j*bytesPerPixel;
          pixbuf[index] = currentColor.r;
          pixbuf[index+1] = currentColor.g;
          pixbuf[index+2] = currentColor.b;
          pixbuf[index+3] = currentColor.a;
        }
      }
    }
    colorLookup.Update();
    return colorLookup;
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

    // four values per pixel, need to use vertex position and vertex texture!!
    unsigned int shapeIdx;
    for(unsigned int index = 0; index < numTriangles; index++)
    {
      shapeIdx = ( idx / 6 ) * 4;

      AddVertex( vertices,
                 glyphVertices->at(idx).x , glyphVertices->at(idx).y, shapeInfo[shapeIdx],
                 glyphVertices->at(idx).g16hi, glyphVertices->at(idx).g16lo,
                 shapeInfo[shapeIdx+1], shapeInfo[shapeIdx+2], shapeInfo[shapeIdx+3]);
      AddVertex( vertices,
                 glyphVertices->at(idx+1).x, glyphVertices->at(idx+1).y,shapeInfo[shapeIdx],
                 glyphVertices->at(idx+1).g16hi, glyphVertices->at(idx+1).g16lo,
                 shapeInfo[shapeIdx+1], shapeInfo[shapeIdx+2], shapeInfo[shapeIdx+3]);
      AddVertex( vertices,
                 glyphVertices->at(idx+2).x, glyphVertices->at(idx+2).y,shapeInfo[shapeIdx],
                 glyphVertices->at(idx+2).g16hi, glyphVertices->at(idx+2).g16lo,
                 shapeInfo[shapeIdx+1], shapeInfo[shapeIdx+2], shapeInfo[shapeIdx+3]);
      AddTriangle( faces, idx, idx+1, idx+2 );
      idx += 3;
    }

    meshData.SetFaceIndices( faces );
    meshData.SetVertices(vertices);
    meshData.SetHasNormals(true);
    meshData.SetHasTextureCoords(true);
  }

private:

  Application&     mApplication;

  unsigned int     mIndex;
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
