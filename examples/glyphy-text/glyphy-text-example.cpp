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

#include <string>
#include <sstream>
#include <cmath>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include "shared/view.h"

#include "glyphy-text-effect.h"
#include "glyphy-atlas.h"
#include "glyphy-demo/demo-font.h"
#include "glyphy-demo/demo-buffer.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char *englishText =
"\n"
"`Twas brillig, and the slithy toves\n"
"  Did gyre and gimble in the wabe:\n"
"All mimsy were the borogoves,\n"
"  And the mome raths outgrabe.\n";

const char *koreanText =
"괜  찬  타\n"
"수부룩이 내려오는 눈발속에서는\n"
"까투리 매추래기 새끼들도 깃들이어 오는 소리\n";

const char *chineseText =
"细雨斜风作晓寒，淡烟疏柳媚晴滩。\n"
"入淮清洛渐漫漫。\n";

const char* PUSHBUTTON_BUTTON_IMAGE = DALI_IMAGE_DIR "button-up.9.png";
const char* CHECKBOX_UNCHECKED_IMAGE = DALI_IMAGE_DIR "checkbox-unchecked.png";
const char* CHECKBOX_CHECKED_IMAGE = DALI_IMAGE_DIR "checkbox-checked.png";

const char *ENGLISH_FONT_PATH = "/usr/share/fonts/SamsungSans_Rg.ttf";
const char *CHINESE_FONT_PATH = DALI_MODEL_DIR "simhei.ttf";
const char *KOREAN_FONT_PATH = "/usr/share/fallback_fonts/TizenSansKoreanRegular.ttf";
const char* FONT_NAME = "HelveticaNue";
const float MENU_OPTION_HEIGHT(140.0f);
const float INCH_TO_POINTS( 72.f );

const Vector4 TEXT_COLOR( 0.f, 0.f, 0.f, 1.f );
const float DEFAULT_SIZE = 30.f;
const float DEFAULT_BOLDNESS = 0.f;
const float DEFAULT_THICKNESS = 1.f;

const int GLYPHY_ATLAS_WIDTH = 256;
const int GLYPHY_ATLAS_HEIGHT = 512;

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
  : mApplication( application ),
    mFontSize( 0.f )
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

    stage.SetBackgroundColor(Color::WHITE);
    stage.KeyEventSignal().Connect(this, &GlyphyTextController::OnKeyEvent);

    // Background, for receiving gestures
    mStageBackground = Actor::New();
    mStageBackground.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mStageBackground.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mStageBackground.SetSize( stageSize.x, stageSize.y );
    stage.Add(mStageBackground);

    // Preprocess: calculate mesh vertices( tew triangles per charater to render )
    //             create glyphy atlas texture
    mAtlas = new GlyphyAtlas( GLYPHY_ATLAS_WIDTH, GLYPHY_ATLAS_HEIGHT, 64, 8 );
    FT_Library ft_library;
    FT_Init_FreeType (&ft_library);
    FT_Face ft_face_english, ft_face_korean, ft_face_chinese;
    FT_New_Face (ft_library, ENGLISH_FONT_PATH, 0/*face_index*/, &ft_face_english);
    FT_New_Face (ft_library, KOREAN_FONT_PATH, 0/*face_index*/, &ft_face_korean);
    FT_New_Face (ft_library, CHINESE_FONT_PATH, 0/*face_index*/, &ft_face_chinese);
    if(!ft_face_english || !ft_face_korean || !ft_face_chinese)
    {
      Die("Failed to open font file");
    }
    mBuffer = demo_buffer_create ();
    glyphy_point_t top_left = {0, 0};
    demo_buffer_move_to (mBuffer, &top_left);
    mFont = demo_font_create (ft_face_english, mAtlas);
    demo_buffer_add_text (mBuffer, englishText, mFont, 1, 0.1);
    demo_font_set_face( mFont, ft_face_korean );
    demo_buffer_add_text (mBuffer, koreanText, mFont, 1, 0.2);
    demo_font_set_face( mFont, ft_face_chinese );
    demo_buffer_add_text (mBuffer, chineseText, mFont, 1, 0.2);

    std::cout<<"Welcome to GLyphy demo !!!"<<std::endl;

    // create mesh actor with buffer->vertices
    MeshData meshData;
    mCustomMaterial = Material::New("CustomMaterial");
    ConstructMesh(meshData, mBuffer->vertices);
    mActor = MeshActor::New( Mesh::New( meshData ) );
    mActor.SetAffectedByLighting( false );
    mSize = DEFAULT_SIZE;
    mActor.SetScale( mSize );
    mActor.SetCullFace( CullNone );
    mActor.SetColor( TEXT_COLOR );
    mActor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mActor.SetParentOrigin( ParentOrigin::TOP_LEFT );
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

    Layer textViewLayer = Layer::New();
    textViewLayer.SetClipping(true);
    textViewLayer.SetClippingBox(0, stageSize.y * 0.5f+5.f, stageSize.x, stageSize.y * 0.5f );
    textViewLayer.SetSize(stageSize.x, stageSize.y);
    textViewLayer.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    textViewLayer.SetParentOrigin( ParentOrigin::CENTER_LEFT);
    stage.Add(textViewLayer);
    CreateTextView( "Regular",TEXT_COLOR );
    mTextView.SetAnchorPoint( AnchorPoint::CENTER );
    mTextView.SetParentOrigin( ParentOrigin::TOP_LEFT);
    mTextView.SetScale( DEFAULT_SIZE / mFontSize );
    mTextView.SetPosition( Vector3(mCompensateMovement) * DEFAULT_SIZE / mFontSize  );
    textViewLayer.Add(mTextView);

    mMovement = Vector2::ZERO;
    //////////// ---- add demo controlling for moving, scalling and text styling ---- /////////////
    Layer controlLayer = Layer::New();
    controlLayer.SetAnchorPoint( AnchorPoint::CENTER );
    controlLayer.SetParentOrigin( ParentOrigin::CENTER);
    controlLayer.SetSize(stageSize.x, stageSize.y);
    controlLayer.RaiseToTop();
    stage.Add(controlLayer);

    ImageActor bar = CreateSolidColorActor( Vector4( 0.5f, 0.0f, 0.0f, 1.f ) );
    bar.SetSize( stageSize.x, 10.0f );
    bar.SetAnchorPoint( AnchorPoint::CENTER );
    bar.SetParentOrigin( ParentOrigin::CENTER );
    bar.SetZ( 2.f );
    bar.SetY(0);
    controlLayer.Add( bar );

    mOptionsButton = CreatePushButton("Options");
    mOptionsButton.SetParentOrigin( ParentOrigin::BOTTOM_RIGHT );
    mOptionsButton.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
    mOptionsButton.SetSize( length*0.2f, length*0.05f );
    controlLayer.Add( mOptionsButton );

    mResetButton = CreatePushButton("Reset");
    mResetButton.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    mResetButton.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    mResetButton.SetSize( length*0.2f, length*0.05f );
    controlLayer.Add( mResetButton );

    CreateOptionPanel();
    controlLayer.Add(mOptionsPopup);
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
    mPinchGestureDetector.Attach( mStageBackground);
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
    mOptionsPopup.SetZ(3.f);

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
      sliderInfo.label = std::string("Font Size");
      sliderInfo.minValue = 2.f;
      sliderInfo.maxValue = 500.f;
      sliderInfo.valuePrecision = 0.f;
      sliderInfo.showValueInPopup = false;
      sliderInfo.defaultValue = DEFAULT_SIZE;

      mSizeSlider  = CreateSlider(sliderInfo, popupWidth );
      mSizeSlider.SetKeyboardFocusable( true );
      mOptionsTable.AddChild( mSizeSlider, TableView::CellPosition( optionIndex, 0 ) );
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
      mOutlineCheckBox.SetZ(4.f);
      mOutlineCheckBox.SetSize( 40, 40 );

      TextView label = Toolkit::TextView::New( "Outline" );
      label.SetParentOrigin( ParentOrigin::CENTER_LEFT );
      label.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
      TextStyle textStyle = DemoHelper::GetDefaultTextStyle();
      textStyle.SetTextColor(Color::WHITE);
      label.SetStyleToCurrentText( textStyle );
      label.SetZ(1.f);
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
    bar.SetZ( 1.f );
    bar.SetY(10.f);
    slider.Add( bar );

    // Label
    if( info.label != String::EMPTY )
    {
      TextView text = TextView::New( info.label );
      text.SetAnchorPoint( ParentOrigin::BOTTOM_CENTER );
      text.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
      text.SetTextAlignment( Alignment::HorizontalLeft );
      TextStyle textStyle = DemoHelper::GetDefaultTextStyle();
      textStyle.SetTextColor(Color::WHITE);
      text.SetStyleToCurrentText( textStyle );
      text.SetZ( 1.f );
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
    buttonLabel.SetStyleToCurrentText(DemoHelper::GetDefaultTextStyle());
    button.SetLabelText( buttonLabel);
    button.SetZ(3.f);

    return button;
  }

  void OnPanGesture( Actor actor, const PanGesture& gesture )
  {
    if( gesture.state == Gesture::Continuing )
    {
      mMovement += gesture.displacement;
      mActor.MoveBy(Vector3(gesture.displacement));
      mTextView.MoveBy(Vector3(gesture.displacement));
    }
  }

  void OnPinch(Actor actor, const PinchGesture& gesture)
  {
    if (gesture.state == Gesture::Started)
    {
      mSizeAtPinchStart = mSize;
      mPinchCenter = gesture.screenCenterPoint;
    }

    float size = clamp(mSizeAtPinchStart * gesture.scale, 2.f, 500.f);
    mActor.SetScale( size );

    mTextView.SetScale( size / mFontSize );
    mTextView.SetPosition( Vector3(mCompensateMovement) * size / mFontSize  );
    mTextView.MoveBy(Vector3(mMovement));
    mSize = size;
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
    else if( actor == mActor || actor == mTextView || actor == mStageBackground)
    {
      HideOptionPanel();
    }
  }

  void Reset()
  {
    mSize = DEFAULT_SIZE;
    mSizeSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, mSize );
    mBoldness = DEFAULT_BOLDNESS;
    mBoldnessSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, mBoldness );
    mOutlineCheckBox.SetChecked(false);
    mThickness = DEFAULT_THICKNESS;
    mThicknessSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, mThickness );

    mActor.SetPosition(0.f,0.f,0.f);
    mTextView.SetPosition( Vector3(mCompensateMovement) * DEFAULT_SIZE / mFontSize );
    mMovement = Vector2::ZERO;
  }

  void ShowOptionPanel()
  {
    mOptionsPopup.SetOpacity(1.f);
    mOptionsButton.SetOpacity(0.f);
    mSizeSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, mSize );
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
    if( slider == mSizeSlider )
    {
      mActor.SetScale( value );
      mTextView.SetScale( value / mFontSize );
      mTextView.SetPosition( Vector3(mCompensateMovement) * value / mFontSize  );
      mTextView.MoveBy(Vector3(mMovement));
      mSize = value;
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

  void ConstructMesh( MeshData& meshData, std::vector<glyph_vertex_t>* glyphVertices )
  {
    MeshData::VertexContainer    vertices;
    MeshData::FaceIndices        faces;
    BoneContainer                bones(0);

    /*float minX = 1000.f, minY=1000.f, maxX=-1000.f, maxY=-1000.f;
    for(unsigned int index = 0; index < glyphVertices->size (); index++)
    {
      if(glyphVertices->at(index).x < minX ) minX = glyphVertices->at(index).x;
      if(glyphVertices->at(index).x > maxX ) maxX = glyphVertices->at(index).x;
      if(glyphVertices->at(index).y < minY ) minY = glyphVertices->at(index).y;
      if(glyphVertices->at(index).y > maxY ) maxY = glyphVertices->at(index).y;
    }
    std::cout<<"width: "<<maxX<<" "<<minX<<"   "<<"height: "<<maxY<<" "<<minY<<std::endl;
    float centerX = (minX+maxX)/2.f;
    float centerY = (minY+maxY)/2.f;*/

    unsigned int numTriangles = glyphVertices->size () / 3;
    unsigned int idx = 0;
    // four values per pixel, need to use vertex position and vertex texture!!
    for(unsigned int index = 0; index < numTriangles; index++)
    {
      //AddVertex( vertices, glyphVertices->at(idx).x - centerX, glyphVertices->at(idx).y - centerY, glyphVertices->at(idx).g16hi, glyphVertices->at(idx).g16lo );
      //AddVertex( vertices, glyphVertices->at(idx+1).x - centerX, glyphVertices->at(idx+1).y - centerY, glyphVertices->at(idx+1).g16hi, glyphVertices->at(idx+1).g16lo );
      //AddVertex( vertices, glyphVertices->at(idx+2).x - centerX, glyphVertices->at(idx+2).y - centerY, glyphVertices->at(idx+2).g16hi, glyphVertices->at(idx+2).g16lo );
      AddVertex( vertices, glyphVertices->at(idx).x, glyphVertices->at(idx).y, glyphVertices->at(idx).g16hi, glyphVertices->at(idx).g16lo );
      AddVertex( vertices, glyphVertices->at(idx+1).x, glyphVertices->at(idx+1).y, glyphVertices->at(idx+1).g16hi, glyphVertices->at(idx+1).g16lo );
      AddVertex( vertices, glyphVertices->at(idx+2).x, glyphVertices->at(idx+2).y, glyphVertices->at(idx+2).g16hi, glyphVertices->at(idx+2).g16lo );
      AddTriangle( faces, idx, idx+1, idx+2 );
      idx += 3;
    }

    meshData.SetData(vertices, faces, bones, mCustomMaterial);
    meshData.SetHasColor(false);
    meshData.SetHasTextureCoords(true);
  }

  void CreateTextView( const std::string& style, const Vector4& textColor)
  //                       bool shadow = false, const Vector4& shadowColor = Color::BLACK, const Vector2& shadowOffset = Vector2::ZERO, const float shadowSize = 0.0f )
  {
    mTextStyle.SetFontName( FONT_NAME );
    mTextStyle.SetFontStyle( style );
    mTextStyle.SetTextColor( textColor );
    mTextStyle.SetWeight(  Dali::TextStyle::REGULAR );
    const Vector2& dpi = Dali::Stage::GetCurrent().GetDpi();
    const float meanDpi = (dpi.height + dpi.width) / 2.0f;
    mFontSize = 128*meanDpi / INCH_TO_POINTS;
    mTextStyle.SetFontPointSize( PointSize( 128 ) );

    std::ostringstream textStream;
    textStream<<englishText<<"\n"<<koreanText<<"\n"<<chineseText;

    std::string text = textStream.str();
    mTextView = TextView::New( text );
    mTextView.SetSnapshotModeEnabled( false );
    mTextView.SetColorMode( USE_OWN_COLOR );
    mTextView.SetStyleToCurrentText( mTextStyle);

    TextView::TextLayoutInfo textLayoutInfo;
    mTextView.GetTextLayoutInfo(textLayoutInfo);
    mCompensateMovement = textLayoutInfo.mTextSize*0.5f;
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
      std::string keyName = event.keyPressedName;
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK )  || keyName == "XF86Back")
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
        if( keyName == "Return" || keyName == "KP_Enter")
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
              mSize += sqrt(mSize)*step;
              mSize = clamp(mSize, 2.f, 500.f);
              mSizeSlider.SetProperty( Toolkit:: Slider::VALUE_PROPERTY, mSize );
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
  Actor            mStageBackground;

  PanGestureDetector    mPanGestureDetector;
  PinchGestureDetector  mPinchGestureDetector;
  TapGestureDetector    mTapGestureDetector;
  float                 mSizeAtPinchStart;
  Vector2               mMovement;
  Vector2               mPinchCenter;

  float   mSize;
  Slider  mSizeSlider;
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

  TextView       mTextView;
  TextStyle      mTextStyle;
  Vector2        mCompensateMovement;
  float          mFontSize;
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
