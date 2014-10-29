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
#include <dali-toolkit/dali-toolkit.h>

#include <string>
#include <cmath>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H

#include "jabberwocky.h"
#include "DroidSans.c"
#include "glyphy-text-effect.h"
#include "glyphy-atlas.h"
#include "glyphy-demo/demo-font.h"
#include "glyphy-demo/demo-buffer.h"

using namespace Dali;

namespace
{
const char* const PUSHBUTTON_PRESS_IMAGE = DALI_IMAGE_DIR "button-down.9.png";
const char* const PUSHBUTTON_DIM_IMAGE = DALI_IMAGE_DIR "button-disabled.9.png";
const char* const PUSHBUTTON_BUTTON_IMAGE = DALI_IMAGE_DIR "button-up.9.png";

//const char *FONT_PATH = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
const float BACKGROUND_Z = -10.f;
const Vector4 BACKGROUND_COLOR( 1.0f, 1.0f, 1.0f, 1.0f );

const Vector4 TEXT_COLOR( 0.f, 0.f, 0.f, 1.f );

const int GLYPHY_ATLAS_SIZE = 256;

const char*                   DEFAULT_TEXT_STYLE_FONT_FAMILY("HelveticaNue");
const char*                   DEFAULT_TEXT_STYLE_FONT_STYLE("Regular");
const Dali::PointSize         DEFAULT_TEXT_STYLE_POINT_SIZE( 30.f );
const Dali::TextStyle::Weight DEFAULT_TEXT_STYLE_WEIGHT(Dali::TextStyle::EXTRALIGHT);
const Dali::Vector4           DEFAULT_TEXT_STYLE_COLOR(0.0f, 0.0f, 0.0f, 1.0f);

static Dali::TextStyle defaultTextStyle;
static bool textStyleSet=false;

float ScalePointSize(int pointSize)
{
  Dali::Vector2 dpi = Dali::Stage::GetCurrent().GetDpi();
  float meanDpi = (dpi.height + dpi.width) * 0.5f;
  return pointSize * meanDpi / 220.0f;        // 220 is the default horizontal DPI defined in adaptor Application
}

Dali::TextStyle& GetDefaultTextStyle()
{
  if(!textStyleSet)
  {
    defaultTextStyle.SetFontName(DEFAULT_TEXT_STYLE_FONT_FAMILY);
    defaultTextStyle.SetFontStyle(DEFAULT_TEXT_STYLE_FONT_STYLE);
    defaultTextStyle.SetFontPointSize(Dali::PointSize(ScalePointSize(DEFAULT_TEXT_STYLE_POINT_SIZE)));
    defaultTextStyle.SetWeight(DEFAULT_TEXT_STYLE_WEIGHT);
    defaultTextStyle.SetTextColor(DEFAULT_TEXT_STYLE_COLOR);
    textStyleSet = true;
  }

  return defaultTextStyle;
}

float clamp (float value, float minValue, float maxValue)
{
  return value < minValue ? minValue : value > maxValue ? maxValue : value;
}

} // namespace

class GlyphyTextController : public ConnectionTracker
{
public:

	GlyphyTextController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &GlyphyTextController::Create );
  }

  ~GlyphyTextController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();
    float length = std::max(stageSize.x, stageSize.y);

    stage.KeyEventSignal().Connect(this, &GlyphyTextController::OnKeyEvent);

    // Background
    ImageActor background = Dali::Toolkit::CreateSolidColorActor( BACKGROUND_COLOR );
    background.SetAnchorPoint( AnchorPoint::CENTER );
    background.SetParentOrigin( ParentOrigin::CENTER );
    background.SetSize( stageSize*1.1f );
    background.SetZ( BACKGROUND_Z );
    stage.Add(background);

    // Preprocess: calculate mesh vertices( tew triangles per charater to render )
    //             create glyphy atlas texture
    mAtlas = new GlyphyAtlas( GLYPHY_ATLAS_SIZE, GLYPHY_ATLAS_SIZE, 64, 8 );
	FT_Library ft_library;
    FT_Init_FreeType (&ft_library);
    FT_Face ft_face;
    FT_New_Memory_Face (ft_library, (const FT_Byte *) DroidSans, sizeof (DroidSans), 0, &ft_face);
   // FT_New_Face (ft_library, FONT_PATH, 0/*face_index*/, &ft_face);
    if(!ft_face)
    {
      Die("Failed to open font file");
    }
    mFont = demo_font_create (ft_face, mAtlas);
    mBuffer = demo_buffer_create ();
    glyphy_point_t top_left = {0, 0};
    demo_buffer_move_to (mBuffer, &top_left);
    const char *text = jabberwocky;
    demo_buffer_add_text (mBuffer, text, mFont, 1);

    std::cout<<"Welcome to GLyphy demo !!!"<<std::endl;

    // create mesh actor with buffer->vertices
    MeshData meshData;
    mCustomMaterial = Material::New("CustomMaterial");
    ConstructMesh(meshData, mBuffer->vertices);
    mActor = MeshActor::New( Mesh::New( meshData ) );
    mActor.SetAffectedByLighting( false );
    mActor.SetScale(30.f);
    mActor.SetCullFace( CullNone );
    mActor.SetColor( TEXT_COLOR );
    stage.Add( mActor );

    // text rendering shader
    mShaderEffect = GlyphyTextEffect::New();
    Vector4 atlasInfo( static_cast<float>(mAtlas->GetTextureWidth()),
                       static_cast<float>(mAtlas->GetTextureHeight()),
                       static_cast<float>(mAtlas->GetItemWidth()),
                       static_cast<float>(mAtlas->GetItemHeightQuantum()));
    mShaderEffect.SetAtlasInfo( atlasInfo );
    mCustomMaterial.SetDiffuseTexture( mAtlas->GetAtlasTexture() );
    mActor.SetShaderEffect( mShaderEffect );


    //////////// ---- add demo controlling for moving, scalling and text styling ---- /////////////

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.DetectedSignal().Connect( this, &GlyphyTextController::OnPanGesture );
    mPanGestureDetector.Attach( mActor );
    mPanGestureDetector.Attach( background );

    mSlider = Toolkit::Slider::New();
    mSlider.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    mSlider.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    mSlider.SetX(length*0.05f);
    mSlider.SetSize( stageSize.x*0.8f, length*0.05f );
    mSlider.SetProperty(  Toolkit::Slider::LOWER_BOUND_PROPERTY, 1.0f );
    mSlider.SetProperty(  Toolkit::Slider::UPPER_BOUND_PROPERTY, 100.0f );
    mSlider.SetProperty(  Toolkit::Slider::SHOW_POPUP_PROPERTY, false );
    mSlider.SetProperty(  Toolkit::Slider::SHOW_VALUE_PROPERTY, false );
    mSlider.SetProperty(  Toolkit::Slider::VALUE_PRECISION_PROPERTY, 0 );
    mSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, 3.0f );
    mSlider.ValueChangedSignal().Connect( this, &GlyphyTextController::OnSliderValueChanged );
    mSliderValue = 3.f;
    stage.Add( mSlider );

    mScaleLabel = CreateTextView( "Scale" );
    mOutlineLable = CreateTextView( "Outline OFF" );
    mOutlineThicknessLable = CreateTextView( "Outline Thickness" );;
    mBoldnessLable = CreateTextView( "Boldness" );;

    Toolkit::PushButton toggleButton1 = CreatePushButton();
    toggleButton1.SetParentOrigin( ParentOrigin::TOP_LEFT );
    toggleButton1.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    toggleButton1.SetLabelText( mOutlineLable );
    toggleButton1.SetSize( length*0.1f, length*0.05f );
    toggleButton1.SetZ(1.f);
    toggleButton1.ToggledSignal().Connect( this, &GlyphyTextController::OnButtonToggled );
    stage.Add( toggleButton1 );

    Toolkit::PushButton toggleButton2 = CreatePushButton();
    toggleButton2.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    toggleButton2.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    toggleButton2.SetLabelText( mScaleLabel );
    toggleButton2.SetSize( length*0.1f, length*0.05f );
    toggleButton2.SetZ(1.f);
    toggleButton2.ToggledSignal().Connect( this, &GlyphyTextController::OnButtonPushed );
    stage.Add( toggleButton2 );

    mScale = 30.f;
    mGamma = 1.f;
    mContrast = 1.f;
    mOutlineThickness = 1.f;
    mBoldness = 0.f;
    mSliderChoice = 0;

    Actor toggleBackground1 = Toolkit::CreateSolidColorActor( Vector4( 0.f, 1.f, 1.f, 0.8f ) );
    toggleBackground1.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    toggleBackground1.SetParentOrigin( ParentOrigin::TOP_LEFT );
    toggleBackground1.SetSize( length*0.1f, length*0.05f );
    stage.Add( toggleBackground1 );

    Actor toggleBackground2 = Toolkit::CreateSolidColorActor( Vector4( 0.f, 1.f, 1.f, 0.8f ) );
    toggleBackground2.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    toggleBackground2.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    toggleBackground2.SetSize( length*0.1f, length*0.05f );
    stage.Add( toggleBackground2 );
  }

private:

  void OnPanGesture( Actor actor, PanGesture gesture )
  {
    if( gesture.state == Gesture::Continuing )
    {
      mActor.MoveBy(Vector3(gesture.displacement));
    }
  }

  bool OnSliderValueChanged( Toolkit::Slider slider, float value )
  {
    mSliderValue = value;
	switch(mSliderChoice)
    {
      case 0:
      {
    	  mScale =  value * value;
    	  mActor.SetScale( mScale );
		  break;
	  }
	  case 1:
	  {
		  mOutlineThickness = clamp( value / 20.f, 0.5f, 3.f );
          mShaderEffect.SetOutlineThickness( mOutlineThickness );
		  break;
	  }
	  case 2:
	  {
		  mBoldness = clamp( value / 100 - 0.2f, -0.2f, 0.7f);
		  mShaderEffect.SetBoldness( mBoldness );
		  break;
	  }
	  case 3:
	  {
		  mGamma = clamp( value / 10.f, 0.1f, 10.f );
		  mShaderEffect.SetGammaAdjust( mGamma );
		  break;
	  }
	  case 4:
	  {
		  mContrast = clamp( value / 10.f, 0.1f, 10.f );
          mShaderEffect.SetContrast( mContrast );
		  break;
	  }
	  default:
	  {
		  break;
	  }
	}
    return true;
  }

  bool OnButtonToggled( Toolkit::Button button, bool state )
  {
    Toolkit::PushButton pushButton = Toolkit::PushButton::DownCast( button );
    if( pushButton )
    {
      if( state )
      {
        mOutlineLable.SetText( "Outline ON" );
      }
      else
      {
        mOutlineLable.SetText( "Outline OFF" );
      }
      mShaderEffect.ToggleOutline( state );
    }

    return true;
  }

  bool OnButtonPushed( Toolkit::Button button, bool state )
  {
	mSliderChoice = (mSliderChoice + 1) % 3;
    Toolkit::PushButton pushButton = Toolkit::PushButton::DownCast( button );

    switch(mSliderChoice)
    {
      case 0:
      {
    	pushButton.SetLabelText(mScaleLabel );
    	mSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, std::sqrt(mScale) );
    	break;
      }
      case 1:
      {
        pushButton.SetLabelText( mOutlineThicknessLable );
        mSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, mOutlineThickness*20.f );
        break;
      }
      case 2:
      {
        pushButton.SetLabelText( mBoldnessLable );
        mSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, mBoldness*100.f + 20.f );
        break;
      }
      case 3:
      {
        pushButton.SetLabelText( "gamma" );
        mSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, mGamma*10.f );
        break;
      }
      case 4:
      {
        pushButton.SetLabelText( "contrast" );
        mSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, mContrast * 10.f);
        break;
      }
      default:
      {
        break;
      }
    }

    return true;
  }

  bool Die( const std::string& message )
  {
    std::cout<<message<<std::endl;
	delete mAtlas;
	demo_buffer_destroy(mBuffer);
	demo_font_destroy(mFont);
    mApplication.Quit();
    return true;
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
    faces.push_back(v1);
    faces.push_back(v2);
  }

  void GenMaterial()
  {
    mCustomMaterial = Material::New("CustomMaterial");
    mCustomMaterial.SetOpacity(1.0f);
    mCustomMaterial.SetMapU( Material::MAPPING_MODE_DECAL );
    mCustomMaterial.SetMapV( Material::MAPPING_MODE_DECAL );
  }

  void ConstructMesh( MeshData& meshData, std::vector<glyph_vertex_t>* glyphVertices )
  {
    MeshData::VertexContainer    vertices;
    MeshData::FaceIndices        faces;
    BoneContainer                bones(0);

    unsigned int numTriangles = glyphVertices->size () / 3;
    unsigned int idx = 0;
    // four values per pixel, need to use vertex position and vertex texture!!
    for(unsigned int index = 0; index < numTriangles; index++)
    {
      AddVertex( vertices, glyphVertices->at(idx).x, glyphVertices->at(idx).y, glyphVertices->at(idx).g16hi, glyphVertices->at(idx).g16lo );
      AddVertex( vertices, glyphVertices->at(idx+1).x, glyphVertices->at(idx+1).y, glyphVertices->at(idx+1).g16hi, glyphVertices->at(idx+1).g16lo );
      AddVertex( vertices, glyphVertices->at(idx+2).x, glyphVertices->at(idx+2).y, glyphVertices->at(idx+2).g16hi, glyphVertices->at(idx+2).g16lo );
      AddTriangle( faces, idx, idx+1,idx+2 );
      idx += 3;
    }

    meshData.SetData(vertices, faces, bones, mCustomMaterial);
    meshData.SetHasColor(false);
    meshData.SetHasTextureCoords(true);
  }

  Toolkit::PushButton CreatePushButton()
  {
    Toolkit::PushButton toggleButton = Toolkit::PushButton::New();
    toggleButton.SetToggleButton( true );
    toggleButton.SetPressedImage( Dali::Image::New( PUSHBUTTON_PRESS_IMAGE ) );
    toggleButton.SetDimmedImage( Dali::Image::New( PUSHBUTTON_DIM_IMAGE ) );
    toggleButton.SetButtonImage( Dali::Image::New( PUSHBUTTON_BUTTON_IMAGE ) );

    return toggleButton;
  }

  Toolkit::TextView CreateTextView(const std::string& text)
  {
    Dali::Toolkit::TextView textView = Dali::Toolkit::TextView::New();
    textView.SetText( text );
    textView.SetStyleToCurrentText(GetDefaultTextStyle());

    return textView;
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        Die( "normal quit" );
      }
      else
      {
        std::string keyName = event.keyPressedName;
        if (keyName == "Left")
        {
            mSliderValue += 1.f;
            mSliderValue = clamp(mSliderValue, 1.f, 100.f);
        	mSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, mSliderValue );
        }
        else if (keyName == "Right")
        {
            mSliderValue -= 1.f;
            mSliderValue = clamp(mSliderValue, 1.f, 100.f);
        	mSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, mSliderValue );
        }
      }
    }
  }

private:
  Application&     mApplication;

  GlyphyTextEffect mShaderEffect;
  GlyphyAtlas*     mAtlas;
  demo_buffer_t*   mBuffer;
  demo_font_t*     mFont;

  Material         mCustomMaterial;
  MeshActor        mActor;

  PanGestureDetector  mPanGestureDetector;

  Toolkit::TextView mScaleLabel;
  Toolkit::TextView mOutlineLable;
  Toolkit::TextView mOutlineThicknessLable;
  Toolkit::TextView mBoldnessLable;

  float mGamma;
  float mContrast;
  float mOutlineThickness;
  float mBoldness;
  float mScale;

  int   mSliderChoice;

  Toolkit::Slider  mSlider;
  float            mSliderValue;

};

void RunTest( Application& application )
{
  GlyphyTextController test( application );

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
