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
using namespace Dali::Toolkit;

namespace
{
const char* PUSHBUTTON_BUTTON_IMAGE = DALI_IMAGE_DIR "button-up.9.png";
const char* CHECKBOX_UNCHECKED_IMAGE = DALI_IMAGE_DIR "checkbox-unchecked.png";
const char* CHECKBOX_CHECKED_IMAGE = DALI_IMAGE_DIR "checkbox-checked.png";

//const char *FONT_PATH = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
const float BACKGROUND_Z = -10.f;
const Vector4 BACKGROUND_COLOR( 1.0f, 1.0f, 1.0f, 1.0f );
const Vector4 POPUP_BACKGROUND_COLOUR( 1.0f, 1.0f, 1.0f, 0.15f );
const float MENU_OPTION_HEIGHT(140.0f);

const Vector4 TEXT_COLOR( 0.f, 0.f, 0.f, 1.f );
const float DEFAULT_SCALE = 20.f;
const float DEFAULT_BOLDNESS = 0.f;
const float DEFAULT_THICKNESS = 1.f;

const int GLYPHY_ATLAS_SIZE = 256;

const char*             DEFAULT_TEXT_STYLE_FONT_FAMILY("HelveticaNue");
const char*             DEFAULT_TEXT_STYLE_FONT_STYLE("Regular");
const PointSize         DEFAULT_TEXT_STYLE_POINT_SIZE( 10.f );
const TextStyle::Weight DEFAULT_TEXT_STYLE_WEIGHT(Dali::TextStyle::EXTRALIGHT);
const Vector4           DEFAULT_TEXT_STYLE_COLOR(0.0f, 0.0f, 0.0f, 1.0f);
static TextStyle defaultTextStyle;
static bool textStyleSet=false;

float ScalePointSize(int pointSize)
{
  Dali::Vector2 dpi = Dali::Stage::GetCurrent().GetDpi();
  float meanDpi = (dpi.height + dpi.width) * 0.5f;
  return pointSize *  220.0f / meanDpi;        // 220 is the default horizontal DPI defined in adaptor Application
}

TextStyle& GetDefaultTextStyle()
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

  // Used for slider construction
  struct SliderInfo
  {
    std::string label;
    float minValue;
    float maxValue;
    float defaultValue;
    bool showValueInPopup;
    int valuePrecision;
  };

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
    mStageBackground = Dali::Toolkit::CreateSolidColorActor( BACKGROUND_COLOR );
    mStageBackground.SetAnchorPoint( AnchorPoint::CENTER );
    mStageBackground.SetParentOrigin( ParentOrigin::CENTER );
    mStageBackground.SetSize( stageSize*1.1f );
    mStageBackground.SetZ( BACKGROUND_Z );
    stage.Add(mStageBackground);

    // Preprocess: calculate mesh vertices( tew triangles per charater to render )
    //             create glyphy atlas texture
    mAtlas = new GlyphyAtlas( GLYPHY_ATLAS_SIZE, GLYPHY_ATLAS_SIZE, 64, 8 );
    FT_Library ft_library;
    FT_Init_FreeType (&ft_library);
    FT_Face ft_face;
    FT_New_Memory_Face (ft_library, (const FT_Byte *) DroidSans, sizeof (DroidSans), 0, &ft_face);
    //FT_New_Face (ft_library, FONT_PATH, 0/*face_index*/, &ft_face);
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
    mScale = DEFAULT_SCALE;
    mActor.SetScale( mScale );
    mActor.SetCullFace( CullNone );
    mActor.SetColor( TEXT_COLOR );
    mActor.SetAnchorPoint( AnchorPoint::CENTER );
    mActor.SetParentOrigin( ParentOrigin::CENTER );
    stage.Add( mActor );

    // text rendering shader
    mBoldness = DEFAULT_BOLDNESS;
    mThickness = DEFAULT_THICKNESS;
    mShaderEffect = GlyphyTextEffect::New();
    Vector4 atlasInfo( static_cast<float>(mAtlas->GetTextureWidth()),
                       static_cast<float>(mAtlas->GetTextureHeight()),
                       static_cast<float>(mAtlas->GetItemWidth()),
                       static_cast<float>(mAtlas->GetItemHeightQuantum()));
    mShaderEffect.SetAtlasInfo( atlasInfo );
    mCustomMaterial.SetDiffuseTexture( mAtlas->GetAtlasTexture() );
    mActor.SetShaderEffect( mShaderEffect );

    //////////// ---- add demo controlling for moving, scalling and text styling ---- /////////////
    mOptionsButton = CreatePushButton("Options");
    mOptionsButton.SetParentOrigin( ParentOrigin::BOTTOM_RIGHT );
    mOptionsButton.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
    mOptionsButton.SetSize( length*0.2f, length*0.05f );
    stage.Add( mOptionsButton );

    mResetButton = CreatePushButton("Reset");
    mResetButton.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    mResetButton.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    mResetButton.SetSize( length*0.2f, length*0.05f );
    stage.Add( mResetButton );

    CreateOptionPanel();
    mOptionsPopup.SetOpacity(0.f);
    mShowOptions = false;
    mFocusableRows = 3;
    mFocusedRow = -1;

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.DetectedSignal().Connect( this, &GlyphyTextController::OnPanGesture );
    mPanGestureDetector.Attach( mActor );
    mPanGestureDetector.Attach( mStageBackground );

    mPinchGestureDetector = PinchGestureDetector::New();
    mPinchGestureDetector.Attach( mActor );
    mPinchGestureDetector.Attach( mStageBackground );
    mPinchGestureDetector.DetectedSignal().Connect(this, &GlyphyTextController::OnPinch);

    mTapGestureDetector = TapGestureDetector::New();
    mTapGestureDetector.Attach( mActor );
    mTapGestureDetector.Attach( mStageBackground );
    mTapGestureDetector.Attach( mOptionsButton );
    mTapGestureDetector.Attach( mResetButton );
    mTapGestureDetector.Attach( mOptionsPopup );
    mTapGestureDetector.DetectedSignal().Connect( this, &GlyphyTextController::OnTap );

    KeyboardFocusManager::Get().PreFocusChangeSignal().Connect( this, &GlyphyTextController::OnKeyboardPreFocusChange );
    mResetButtonSeleted = false;
  }

private:

  void CreateOptionPanel()
  {
    Stage stage = Stage::GetCurrent();
    const float popupWidth = std::min(stage.GetSize().x * 0.5f, MENU_OPTION_HEIGHT*3.f);
    Vector2 tableSize = Vector2( popupWidth, MENU_OPTION_HEIGHT * 3.5f );

    // Add controls to the popup panel
    mOptionsPopup = CreateSolidColorActor( Vector4( 0.f, 0.25f, 0.35f, 1.f ) );
    mOptionsPopup.SetParentOrigin( ParentOrigin::BOTTOM_RIGHT );
    mOptionsPopup.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
    mOptionsPopup.SetSize(tableSize.x, tableSize.y + 15.f);
    mOptionsPopup.SetZ(1.f);
    stage.Add(mOptionsPopup);

    const int numControls = 4;
    mOptionsTable = TableView::New( numControls, 1 );
    mOptionsTable.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mOptionsTable.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mOptionsTable.SetSize( tableSize );
    mOptionsPopup.Add( mOptionsTable );

    int optionIndex = 0;
    // Add a scale slider
    {
      SliderInfo sliderInfo;
      sliderInfo.label = std::string("Scale");
      sliderInfo.minValue = 1.f;
      sliderInfo.maxValue = 1000.f;
      sliderInfo.valuePrecision = 0.f;
      sliderInfo.showValueInPopup = false;
      sliderInfo.defaultValue = DEFAULT_SCALE;

      mScaleSlider  = CreateSlider(sliderInfo, popupWidth );
      mScaleSlider.SetKeyboardFocusable( true );
      mOptionsTable.AddChild( mScaleSlider, TableView::CellPosition( optionIndex, 0 ) );
      ++optionIndex;
    }

    // Add a boldness slider
    {
      SliderInfo sliderInfo;
      sliderInfo.label = std::string("Boldness");
      sliderInfo.minValue = -0.2f;
      sliderInfo.maxValue = 0.7f;
      sliderInfo.valuePrecision = 1.f;
      sliderInfo.showValueInPopup = false;
      sliderInfo.defaultValue = DEFAULT_BOLDNESS;

      mBoldnessSlider  = CreateSlider(sliderInfo, popupWidth );
      mBoldnessSlider.SetKeyboardFocusable( true );
      mOptionsTable.AddChild( mBoldnessSlider, TableView::CellPosition( optionIndex, 0 ) );
      ++optionIndex;
    }

    //Add a outline checkbox
    {
      mOutlineCheckBox = CheckBoxButton::New();
      mOutlineCheckBox.SetName( "checkbox1" );
      mOutlineCheckBox.SetParentOrigin( ParentOrigin::TOP_LEFT );
      mOutlineCheckBox.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      mOutlineCheckBox.SetBackgroundImage( Image::New( CHECKBOX_UNCHECKED_IMAGE ) );
      mOutlineCheckBox.SetCheckedImage( Image::New( CHECKBOX_CHECKED_IMAGE ) );
      mOutlineCheckBox.SetY(20.f);
      mOutlineCheckBox.SetZ(2.f);
      mOutlineCheckBox.SetSize( 40, 40 );

      TextView label = Toolkit::TextView::New( "Outline" );
      label.SetParentOrigin( ParentOrigin::CENTER_LEFT );
      label.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
      TextStyle textStyle = GetDefaultTextStyle();
      textStyle.SetTextColor(Color::WHITE);
      label.SetStyleToCurrentText( textStyle );
      label.SetZ(1.5f);
      label.SetX(80.f);
      mOutlineCheckBox.Add( label );

      ImageActor bg = CreateSolidColorActor( Vector4( 0.f, 0.25f, 0.35f, 1.f ) );
      bg.SetName("OutlineCheckbox");
      bg.Add(mOutlineCheckBox);
      mOutlineCheckBox.ToggledSignal().Connect( this, &GlyphyTextController::OnCheckBoxesToggled );
      bg.SetKeyboardFocusable( true );
      mOutlineCheckBox.SetKeyboardFocusable( true );
      mOptionsTable.AddChild( bg, TableView::CellPosition( optionIndex, 0 ) );
      mOptionsTable.SetFixedHeight(optionIndex, MENU_OPTION_HEIGHT*0.5f);
      ++optionIndex;
    }

    // Add a outlineThichness slider
    {
      SliderInfo sliderInfo;
      sliderInfo.label = std::string("Thickness");
      sliderInfo.minValue = 0.5f;
      sliderInfo.maxValue = 3.f;
      sliderInfo.valuePrecision = 1.f;
      sliderInfo.showValueInPopup = false;
      sliderInfo.defaultValue = DEFAULT_THICKNESS;

      mThicknessSlider  = CreateSlider(sliderInfo, popupWidth );
      mThicknessSlider.SetKeyboardFocusable( true );
      mThicknessSlider.SetProperty( Toolkit:: Slider::ENABLED_PROPERTY, false );
      mOptionsTable.AddChild( mThicknessSlider, TableView::CellPosition( optionIndex, 0 ) );
      ++optionIndex;
    }

  }

  Slider CreateSlider( const SliderInfo& info, float sliderWidth )
  {
    // Create slider
    Slider slider = Slider::New();
    slider.SetProperty( Slider::LOWER_BOUND_PROPERTY, info.minValue );
    slider.SetProperty( Slider::UPPER_BOUND_PROPERTY, info.maxValue );
    slider.SetProperty( Slider::VALUE_PRECISION_PROPERTY, info.valuePrecision );
    slider.SetProperty( Slider::SHOW_POPUP_PROPERTY, info.showValueInPopup );
    slider.SetProperty( Slider::SHOW_VALUE_PROPERTY, info.showValueInPopup );
   // Configure slider
    slider.SetName( info.label );
    slider.SetProperty( Slider::VALUE_PROPERTY, info.defaultValue );

    // Bottom grey bar
    ImageActor bar = CreateSolidColorActor( Vector4( 1.0f, 1.0f, 1.0f, 0.8f ) );
    bar.SetSize( 0.0f, 2.0f );
    bar.ApplyConstraint( Constraint::New<float>( Actor::SIZE_WIDTH, ParentSource( Actor::SIZE_WIDTH ), EqualToConstraint() ) );
    bar.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    bar.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    bar.SetZ( 1.5f );
    bar.SetY(10.f);
    slider.Add( bar );

    // Label
    if( info.label != String::EMPTY )
    {
      TextView text = TextView::New( info.label );
      text.SetAnchorPoint( ParentOrigin::BOTTOM_CENTER );
      text.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
      text.SetTextAlignment( Alignment::HorizontalLeft );
      TextStyle textStyle = GetDefaultTextStyle();
      textStyle.SetTextColor(Color::WHITE);
      text.SetStyleToCurrentText( textStyle );
      text.SetZ( 1.5f );
      slider.Add( text );
      slider.SetName(info.label);
    }

    slider.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    slider.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    slider.ValueChangedSignal().Connect( this, &GlyphyTextController::OnSliderValueChanged );
    slider.SetZ(2.f);
    return slider;
  }

  PushButton CreatePushButton(std::string label)
  {
    PushButton button = Toolkit::PushButton::New();
    button.SetToggleButton( false );
    button.SetButtonImage( Dali::Image::New( PUSHBUTTON_BUTTON_IMAGE ) );
    TextView buttonLabel = TextView::New();
    buttonLabel.SetText(  label );
    buttonLabel.SetStyleToCurrentText(GetDefaultTextStyle());
    button.SetLabelText( buttonLabel);
    button.SetZ(1.f);

    return button;
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
      mPinchCenter = gesture.screenCenterPoint;
    }

    mScale = clamp(mScaleAtPinchStart * gesture.scale, 1.f, 1000.f);
    mActor.SetScale( mScale );
  }

  void OnTap( Actor actor, const TapGesture& tapGesture )
  {
    if(actor == mOptionsButton)
    {
      ShowOptionPanel();
    }
    else if( actor == mResetButton )
    {
      Reset();
    }
    else if( actor == mActor || actor == mStageBackground)
    {
      HideOptionPanel();
    }
  }

  void Reset()
  {
    mScale = DEFAULT_SCALE;
    mScaleSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, mScale );
    mBoldness = DEFAULT_BOLDNESS;
    mBoldnessSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, mBoldness );
    mOutlineCheckBox.SetChecked(false);
    mThickness = DEFAULT_THICKNESS;
    mThicknessSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, mThickness );
    mActor.SetPosition(0.f,0.f,0.f);
  }

  void ShowOptionPanel()
  {
    mOptionsPopup.SetOpacity(1.f);
    mOptionsButton.SetOpacity(0.f);
    mScaleSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, mScale );
    mShowOptions = true;
    mResetButtonSeleted = false;
    mFocusedRow = -1;
  }

  void HideOptionPanel()
  {
    mOptionsPopup.SetOpacity(0.f);
    mOptionsButton.SetOpacity(1.f);
    mShowOptions = false;
  }

  bool OnSliderValueChanged( Toolkit::Slider slider, float value )
  {
    if( slider == mScaleSlider )
    {
      mScale = value;
      mActor.SetScale( mScale );
    }
    else if( slider == mBoldnessSlider )
    {
      mBoldness = value;
      mShaderEffect.SetBoldness( mBoldness );
    }
    else if( slider == mThicknessSlider )
    {
      mThickness = value;
      mShaderEffect.SetOutlineThickness( mThickness );
    }
    return true;
  }

  bool OnCheckBoxesToggled( Toolkit::Button button, bool state )
  {
    mShaderEffect.ToggleOutline( state );
    mThicknessSlider.SetProperty( Toolkit:: Slider::ENABLED_PROPERTY, state );
    mFocusableRows = state ? 4 : 3;
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

    float minX = 1000.f, minY=1000.f, maxX=-1000.f, maxY=-1000.f;
    for(unsigned int index = 0; index < glyphVertices->size (); index++)
    {
      if(glyphVertices->at(index).x < minX ) minX = glyphVertices->at(index).x;
      if(glyphVertices->at(index).x > maxX ) maxX = glyphVertices->at(index).x;
      if(glyphVertices->at(index).y < minY ) minY = glyphVertices->at(index).y;
      if(glyphVertices->at(index).y > maxY ) maxY = glyphVertices->at(index).y;
    }
    float centerX = (minX+maxX)/2.f;
    float centerY = (minY+maxY)/2.f;

    unsigned int numTriangles = glyphVertices->size () / 3;
    unsigned int idx = 0;
    // four values per pixel, need to use vertex position and vertex texture!!
    for(unsigned int index = 0; index < numTriangles; index++)
    {
      AddVertex( vertices, glyphVertices->at(idx).x - centerX, glyphVertices->at(idx).y - centerY, glyphVertices->at(idx).g16hi, glyphVertices->at(idx).g16lo );
      AddVertex( vertices, glyphVertices->at(idx+1).x - centerX, glyphVertices->at(idx+1).y - centerY, glyphVertices->at(idx+1).g16hi, glyphVertices->at(idx+1).g16lo );
      AddVertex( vertices, glyphVertices->at(idx+2).x - centerX, glyphVertices->at(idx+2).y - centerY, glyphVertices->at(idx+2).g16hi, glyphVertices->at(idx+2).g16lo );
      AddTriangle( faces, idx, idx+1, idx+2 );
      idx += 3;
    }

    meshData.SetData(vertices, faces, bones, mCustomMaterial);
    meshData.SetHasColor(false);
    meshData.SetHasTextureCoords(true);
  }

  Actor OnKeyboardPreFocusChange( Actor current, Actor proposed, Control::KeyboardFocusNavigationDirection direction )
  {
    if(!mShowOptions)
    {
      if ( direction == Control::Left)
      {
        mResetButtonSeleted = true;
        return mResetButton;
      }
      else
      {
        mResetButtonSeleted = false;
        return mOptionsButton;
      }
    }
    else if ( direction == Control::Down || (!current && !proposed) )
    {
      mFocusedRow = (mFocusedRow+1)%mFocusableRows;
      return mOptionsTable.GetChildAt(TableView::CellPosition( mFocusedRow, 0 ));
    }
    else if( direction == Control::Up )
    {
      mFocusedRow = (mFocusedRow-1+mFocusableRows)%mFocusableRows;
      return mOptionsTable.GetChildAt(TableView::CellPosition( mFocusedRow, 0 ));
    }

    return current;
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
        if( mShowOptions )
        {
          HideOptionPanel();
        }
        else
        {
          Die( "normal quit" );
        }
      }
      else
      {
        std::string keyName = event.keyPressedName;
        if( keyName == "Return" )
        {
          if( !mShowOptions && !mResetButtonSeleted)
          {
            ShowOptionPanel();
          }
          else if( !mShowOptions && mResetButtonSeleted )
          {
            Reset();
          }
          else if( mFocusedRow == 2 )
          {
            mOutlineCheckBox.SetChecked(!mOutlineCheckBox.IsChecked());
          }
        }
        else if ( mShowOptions && ( keyName == "Left" || keyName == "Right" ) )
        {
          int step = keyName == "Left" ? -1 : 1;
          switch( mFocusedRow )
          {
            case 0:
            {
              mScale += sqrt(mScale)*step;
              mScale = clamp(mScale, 1.f, 1000.f);
              mScaleSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, mScale );
              break;
            }
            case 1:
            {
              mBoldness += 0.05*step;
              mBoldness = clamp(mBoldness, -0.2f, 0.7f);
              mBoldnessSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, mBoldness );
              break;
            }
            case 3:
            {
              mThickness += 0.1*step;
              mThickness = clamp(mThickness, 0.5f, 3.f);
              mThicknessSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, mThickness );
              break;
            }
            default:
            {
              break;
            }
          }
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
  ImageActor       mStageBackground;

  PanGestureDetector    mPanGestureDetector;
  PinchGestureDetector  mPinchGestureDetector;
  TapGestureDetector    mTapGestureDetector;
  float                 mScaleAtPinchStart;
  Vector2               mPinchCenter;

  float   mScale;
  Slider  mScaleSlider;
  float   mBoldness;
  Slider  mBoldnessSlider;
  float   mThickness;
  Slider  mThicknessSlider;

  PushButton     mOptionsButton;
  Actor          mOptionsPopup;
  TableView      mOptionsTable;
  CheckBoxButton mOutlineCheckBox;
  bool           mShowOptions;

  PushButton     mResetButton;
  bool           mResetButtonSeleted;

  int            mFocusedRow;
  int            mFocusableRows;

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
