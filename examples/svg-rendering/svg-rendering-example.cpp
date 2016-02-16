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
#include <dali/devel-api/adaptor-framework/file-loader.h>

using namespace Dali;

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define NANOSVG_ALL_COLOR_KEYWORDS  // Include full list of color keywords.
#define NANOSVG_IMPLEMENTATION      // Expands implementation
#include "nanosvg.h"

#include "svg-atlas.h"
#include "svg-encoder.h"
#include "svg-actor.h"

namespace
{
const char*  const CHANGE_IMAGE_ICON(DEMO_IMAGE_DIR "icon-change.png");
const char * const CHANGE_IMAGE_ICON_SELECTED( DEMO_IMAGE_DIR "icon-change-selected.png" );

const unsigned int TEXTURE_ITEM_WIDTH( 64u );

const char* SVG_IMAGES[] =
{
    DEMO_IMAGE_DIR "Camera.svg",
    DEMO_IMAGE_DIR "Contacts.svg",
    DEMO_IMAGE_DIR "Crown_of_Italy.svg",
    DEMO_IMAGE_DIR "Flamingo.svg",
    DEMO_IMAGE_DIR "FoodChain.svg",
    DEMO_IMAGE_DIR "Kid1.svg",
    DEMO_IMAGE_DIR "Mail.svg",
    DEMO_IMAGE_DIR "Message.svg",
    DEMO_IMAGE_DIR "Phone.svg",
    DEMO_IMAGE_DIR "Settings.svg",
    DEMO_IMAGE_DIR "Tiger.svg",
    DEMO_IMAGE_DIR "Twitter.svg",
    DEMO_IMAGE_DIR "World.svg",
    DEMO_IMAGE_DIR "butterfly.svg",
    DEMO_IMAGE_DIR "car.svg",
    DEMO_IMAGE_DIR "drops.svg",
    DEMO_IMAGE_DIR "embrace.svg",
    DEMO_IMAGE_DIR "picasso.svg",
    DEMO_IMAGE_DIR "spring.svg",
    DEMO_IMAGE_DIR "spring_bicicleta.svg",
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
    stage.GetRootLayer().SetBehavior( Layer::LAYER_3D );
    Vector2 stageSize = stage.GetSize();
    stage.SetBackgroundColor(Color::WHITE);
    stage.KeyEventSignal().Connect(this, &GlyphySVGController::OnKeyEvent);

    // Background, for receiving gestures
    mStageBackground = Actor::New();
    mStageBackground.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mStageBackground.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mStageBackground.SetSize( stageSize.x, stageSize.y );
    stage.Add(mStageBackground);

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.DetectedSignal().Connect( this, &GlyphySVGController::OnPanGesture );
    mPanGestureDetector.Attach( mStageBackground );

    mPinchGestureDetector = PinchGestureDetector::New();
    mPinchGestureDetector.Attach( mStageBackground);
    mPinchGestureDetector.DetectedSignal().Connect(this, &GlyphySVGController::OnPinch);

    Toolkit::PushButton button = Toolkit::PushButton::New();
    button.SetButtonImage( ResourceImage::New( CHANGE_IMAGE_ICON ) );
    button.SetSelectedImage( ResourceImage::New( CHANGE_IMAGE_ICON_SELECTED ) );
    button.SetZ(1.f);
    button.SetScale(3.f);
    button.SetAnchorPoint( AnchorPoint::TOP_RIGHT );
    button.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    stage.Add( button );
    button.ClickedSignal().Connect( this, &GlyphySVGController::OnButtonClicked );
	
    OnButtonClicked(button);

  }

  bool OnButtonClicked(Toolkit::Button button)
  {
    if( mActor )
    {
      Stage::GetCurrent().Remove(mActor);
    }

    mActor = CreateMeshActorFromSVG(SVG_IMAGES[mIndex]);
    mScale = 1.f;
    mActor.SetScale( mScale );
    mActor.SetAnchorPoint( AnchorPoint::CENTER );
    mActor.SetParentOrigin( ParentOrigin::CENTER );
    Stage::GetCurrent().Add( mActor );

    mIndex = (mIndex+1) % NUM_SVG_IMAGES;
    return true;
  }

  Actor CreateMeshActorFromSVG( const char* svgFile )
  {
    // Load
    IntrusivePtr<SvgEncoder> svgEncoder = new SvgEncoder();

    Dali::Vector<char> fileContent;
    if( FileLoader::ReadFile(svgFile, fileContent, FileLoader::TEXT) == 0)
    {
      return Actor::New();
    }

    NSVGimage* image = nsvgParse(&fileContent[0], "px", 96);

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
      svgEncoder->AddSvgShape( shape );

      Vector4 fillColor = Vector4::ZERO;

      char type = (shape->fill).type;
      if( type != 0 )//fill_solid or fill_gradient
      {
        fillColor = Hex2RGBA( (shape->fill).color, shape->opacity ) / 255.f;
      }

      int gradientIndex = 0;
      Vector3 gradientParamX = Vector3::ZERO;
      Vector3 gradientParamY = Vector3::ZERO;
      if( type == 2 || type == 3) // fill_gradient
      {
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

      Vector4 strokeColor = Vector4::ZERO;
      if( (shape->stroke).type != 0 )
      {
        strokeColor= Hex2RGBA( (shape->stroke).color, shape->opacity ) / 255.f;
      }

      const SvgEncoder::VertexContainer& vertices = svgEncoder->GetVertices();
      if( vertices.size() == (shapeIdx+1)*4 )
      {
        for( unsigned int i = 0; i < 4u; i++ )
        {
          unsigned int idx = shapeIdx*4u + i;
          vertexData->push_back(SVGActor::Vertex( Vector2( vertices[idx].x, vertices[idx].y ),
              Vector2( vertices[idx].g16hi, vertices[idx].g16lo ),
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

    SvgAtlasPtr encodedAtlas = svgEncoder->GetAtlas();

    if( gradientCount > 0 )
    {
      std::vector<glyphy_rgba_t> gradient_tex_data;
      gradient_tex_data.resize( gradientCount* TEXTURE_ITEM_WIDTH);
      generate_gradient_texture(gradient_tex_data, gradientCount, nstops,stops );

      unsigned int tex_pos_x, tex_pos_y;
      encodedAtlas->AtlasAlloc( &gradient_tex_data[0], gradientCount* TEXTURE_ITEM_WIDTH, &tex_pos_x, &tex_pos_y, false );
      Vector2 pos((tex_pos_x-0.5f)/static_cast<float>(encodedAtlas->GetTextureWidth()), (tex_pos_y-0.5f)/static_cast<float>(encodedAtlas->GetTextureHeight()));
      svgActor->SetGradientTexturePos( pos );
    }

    svgActor->SetSvgAtlas( encodedAtlas->GetAtlasTexture() );

    Vector4 atlasInfo( static_cast<float>(encodedAtlas->GetTextureWidth()),
                       static_cast<float>(encodedAtlas->GetTextureHeight()),
                       static_cast<float>(encodedAtlas->GetItemWidth()),
                       static_cast<float>(encodedAtlas->GetItemHeightQuantum()));
    svgActor->SetAtlasInfo( atlasInfo );

    svgActor->ConstraintStrokeScale();

    const glyphy_extents_t& extents = svgEncoder->GetExtents();
    Vector2 center((extents.max_x + extents.min_x)/2.f, (extents.max_y + extents.min_y)/2.f);
    svgActor->SetCenter(center);
    svgActor->GetMeshActor().SetSize( extents.max_x - extents.min_x, extents.max_y - extents.min_y );

    nsvgDelete(image);

    return svgActor->GetMeshActor();
  }

  Vector4 Hex2RGBA( unsigned int color, float shapeOpacity = 1.f )
  {
    float r = static_cast<float>(color & 0xFF);
    float g = static_cast<float>( (color >> 8) & 0xFF );
    float b = static_cast<float>( (color >> 16)& 0xFF );
    float a = static_cast<float>( (color >> 24)& 0xFF );
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
        Vector4 color = Hex2RGBA( curStops[0].color );
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
        Vector4 color = Hex2RGBA( curStops[numStops-1].color );
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
        Vector4 startColor = Hex2RGBA( curStops[i].color );
        Vector4 endColor = Hex2RGBA( curStops[i+1].color );
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
    mActor.SetScale( mScale );
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
          mActor.SetScale( mScale );
        }
        else if( strcmp(keyName, "Right") == 0 )
        {
          mScale *= 1.1f;
          mActor.SetScale( mScale );
        }
      }
    }
  }

private:

  Application&     mApplication;
  Actor            mActor;
  Actor            mStageBackground;

  PanGestureDetector    mPanGestureDetector;
  PinchGestureDetector  mPinchGestureDetector;

  float         mScale;
  float         mScaleAtPinchStart;
  unsigned int  mIndex;
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
