/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define NANOSVG_ALL_COLOR_KEYWORDS  // Include full list of color keywords.
#define NANOSVG_IMPLEMENTATION      // Expands implementation
#include "nanosvg.h"

#include "glyphy-atlas.h"
#include "demo-svg.h"
#include "demo-buffer.h"
#include "svg-actor.h"

namespace
{
const char*  const CHANGE_IMAGE_ICON(DALI_IMAGE_DIR "icon-change.png");
const char * const CHANGE_IMAGE_ICON_SELECTED( DALI_IMAGE_DIR "icon-change-selected.png" );

const unsigned int TEXTURE_ITEM_WIDTH( 64u );

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
  DALI_IMAGE_DIR "drops.svg",
  DALI_IMAGE_DIR "embrace.svg",
  DALI_IMAGE_DIR "car.svg",
  DALI_IMAGE_DIR "Tiger.svg",
  DALI_IMAGE_DIR "Crown_of_Italy.svg",
  DALI_IMAGE_DIR "picasso.svg",
  DALI_IMAGE_DIR "b8.svg",
  DALI_IMAGE_DIR "Flamingo.svg",
  DALI_IMAGE_DIR "FoodChain.svg"
};

const unsigned int NUM_SVG_IMAGES( sizeof( SVG_IMAGES ) / sizeof( SVG_IMAGES[0] ) );

/**
 * structure of the vertex in the mesh
 */
struct Vertex
{
  Vector3 position;
  Vector2 textureCoord;
  Vector3 normal;

  Vertex()
  {}

  Vertex( const Vector3& position, const Vector2& textureCoord, const Vector3& normal  )
  : position( position ),
    textureCoord( textureCoord ),
    normal( normal )
  {}
};

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

    Toolkit::PushButton button = Toolkit::PushButton::New();
    button.SetButtonImage( ResourceImage::New( CHANGE_IMAGE_ICON ) );
    button.SetSelectedImage( ResourceImage::New( CHANGE_IMAGE_ICON_SELECTED ) );
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

    mActor = CreateMeshActorFromSVG(SVG_IMAGES[mIndex]);
    mScale = 1.f;
    mActor.SetScale( mScale, -mScale, 1.f );
    mActor.SetColor( Vector4( 0.f, 0.f, 0.f, 1.f ) );
    mActor.SetAnchorPoint( AnchorPoint::CENTER );
    mActor.SetParentOrigin( Vector3(0.05f,0.1f,0.5f) );
    stage.Add( mActor );
    mIndex++;

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
    mActor.SetColor( Vector4( 0.f, 0.f, 0.f, 1.f ) );
    mActor.SetAnchorPoint( AnchorPoint::CENTER );
    mActor.SetParentOrigin( Vector3(0.05f,0.1f,0.5f) );
    Stage::GetCurrent().Add( mActor );
    mIndex++;

    mIndex %= NUM_SVG_IMAGES;

    return true;
  }

  Actor CreateMeshActorFromSVG( const char* svgFile )
  {
    // Load
    GlyphyAtlas* glyphyAtlas = new GlyphyAtlas( 1024,512, TEXTURE_ITEM_WIDTH, 8 );
    demo_buffer_t* demoBuffer = demo_buffer_create ();
    glyphy_point_t top_left = {0, 0};
    demo_buffer_move_to (demoBuffer, &top_left);
    demo_svg_t*  mSVG = demo_svg_create( glyphyAtlas );

    NSVGimage* image = nsvgParseFromFile(svgFile, "px", 96);

    std::vector<SVGActor::Vertex>* vertexData = new std::vector<SVGActor::Vertex>;
    unsigned int shapeCount = 0;
    for (NSVGshape* shape = image->shapes; shape != NULL; shape = shape->next)
    {
      shapeCount++;
    }
    vertexData->reserve( shapeCount*4u );

    //fill_gradient
    int gradientCount = 0;
    Dali::Vector<int> nstops;
    Dali::Vector< NSVGgradientStop*> stops;

    unsigned int shapeIdx = 0;
    for (NSVGshape* shape = image->shapes; shape != NULL; shape = shape->next)
    {
      demo_buffer_add_svg_shape(demoBuffer, shape, mSVG);

      Vector4 fillColor = Vector4::ZERO;

      char type = (shape->fill).type;
      if( type == 1 )//fill_solid
      {
        fillColor = Hex2RGBA( (shape->fill).color, shape->opacity );
      }

      int gradientIndex = 0;
      Vector3 gradientParamX = Vector3::ZERO;
      Vector3 gradientParamY = Vector3::ZERO;
      if( type == 2 || type == 3) // fill_gradient
      {
        fillColor = Hex2RGBA( (shape->fill).color, shape->opacity );
        NSVGgradient* gradient = (shape->fill).gradient;

        if( type == 3 ) // radient gradient
        {
          gradientParamX = Vector3( (gradient->xform)[0],(gradient->xform)[2], (gradient->xform)[4] );
        }
        gradientParamY = Vector3( (gradient->xform)[1],(gradient->xform)[3], (gradient->xform)[5] );

        nstops.PushBack( gradient->nstops );
        stops.PushBack( gradient->stops );
        gradientCount++;
        gradientIndex = gradientCount;
      }

      Vector4 strokeColor( 0.f, 0.f, 0.f, 0.f );
      if( (shape->stroke).type != 0 )
      {
        strokeColor= Hex2RGBA( (shape->stroke).color, shape->opacity );
      }

      if( demoBuffer->vertices->size() == (shapeIdx+1)*4 )
      {
        for( unsigned int i = 0; i < 4u; i++ )
        {
          unsigned int idx = shapeIdx*4u + i;
          vertexData->push_back(SVGActor::Vertex( Vector2( (demoBuffer->vertices)->at(idx).x, (demoBuffer->vertices)->at(idx).y ),
              Vector2( (demoBuffer->vertices)->at(idx).g16hi, (demoBuffer->vertices)->at(idx).g16lo ),
              fillColor, strokeColor,
              gradientParamX, gradientParamY,
              shape->strokeWidth, gradientIndex) );
        }
        shapeIdx++;
      }
    }

    IntrusivePtr<SVGActor> svgActor = new SVGActor();
    svgActor->Initialize();
    svgActor->SetVertices( vertexData );

    if( gradientCount > 0 )
    {
      svgActor->SetGradientCount( gradientCount );

      std::vector<glyphy_rgba_t> gradient_tex_data;
      gradient_tex_data.resize( gradientCount* TEXTURE_ITEM_WIDTH);
      generate_gradient_texture(gradient_tex_data, gradientCount, nstops,stops );

      unsigned int tex_pos_x, tex_pos_y;
      glyphyAtlas->AtlasAlloc( &gradient_tex_data[0], gradientCount* TEXTURE_ITEM_WIDTH, &tex_pos_x, &tex_pos_y, false );
      Vector2 pos((tex_pos_x-0.5f)/static_cast<float>(glyphyAtlas->GetTextureWidth()), (tex_pos_y-0.5f)/static_cast<float>(glyphyAtlas->GetTextureHeight()));
      svgActor->SetGradientTexturePos( pos );
    }

    svgActor->SetGlyphyAtlas( glyphyAtlas->GetAtlasTexture() );

    Vector4 atlasInfo( static_cast<float>(glyphyAtlas->GetTextureWidth()),
                       static_cast<float>(glyphyAtlas->GetTextureHeight()),
                       static_cast<float>(glyphyAtlas->GetItemWidth()),
                       static_cast<float>(glyphyAtlas->GetItemHeightQuantum()));
    svgActor->SetAtlasInfo( atlasInfo );

    svgActor->ConstraintStrokeScale();
    glyphy_extents_t extents = demoBuffer->ink_extents;
    svgActor->GetMeshActor().SetSize( extents.max_x - extents.min_x, extents.max_y - extents.min_y );

    nsvgDelete(image);
    return svgActor->GetMeshActor();
  }

  Vector4 Hex2RGBA( unsigned int color, float shapeOpacity = 1.f )
  {
    float r = static_cast<float>(color & 0xFF) / 255.f;
    float g = static_cast<float>( (color >> 8) & 0xFF ) / 255.f;
    float b = static_cast<float>( (color >> 16)& 0xFF ) / 255.f;
    float a = static_cast<float>( (color >> 24)& 0xFF ) / 255.f;
    return Vector4( r,g,b,a*shapeOpacity );
  }

  void generate_gradient_texture( std::vector<glyphy_rgba_t>& gradient_tex,
                                  int numGradient,
                                  const Dali::Vector<int>& nstops,
                                  const Dali::Vector< NSVGgradientStop*>& stops )
    {
      int resolution = TEXTURE_ITEM_WIDTH;
      float length = static_cast<float>(resolution);

      // ToDo: currently only handling spread == pad;
      //       needs to add support for spread == repeated / reflected
      for( int n = 0; n < numGradient; n++ )
      {
        int offset = n*resolution;
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
            int index = offset + j;
            gradient_tex[index].r = color.r;
            gradient_tex[index].g = color.g;
            gradient_tex[index].b = color.b;
            gradient_tex[index].a = color.a;
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
            int index = offset + j;
            gradient_tex[index].r = color.r;
            gradient_tex[index].g = color.g;
            gradient_tex[index].b = color.b;
            gradient_tex[index].a = color.a;
          }
        }

        for( int i = 0; i<numStops-1; i++ )
        {
          Vector4 startColor = RGBA_Color( curStops[i].color );
          Vector4 endColor = RGBA_Color( curStops[i+1].color );
          int start = floorf(curStops[i].offset * length + 0.5f );
          int end = floorf(curStops[i+1].offset * length + 0.5f);
          for( int j = start; j< end; j++)
          {
            float ratio = static_cast<float>(j-start)/static_cast<float>(end-start);
            Vector4 currentColor = startColor * (1.f-ratio) + endColor * ratio;
            int index = offset + j;
            gradient_tex[index].r = currentColor.r;
            gradient_tex[index].g = currentColor.g;
            gradient_tex[index].b = currentColor.b;
            gradient_tex[index].a = currentColor.a;
          }
        }
      }
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

private:

  Application&     mApplication;

  unsigned int     mIndex;
  Actor            mActor;
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
