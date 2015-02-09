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
#include "../shared/view.h"

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

//const char *ENGLISH_FONT_PATH = "/usr/share/fonts/SamsungSans_Rg.ttf";
const char *ENGLISH_FONT_PATH = "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf";
const char *CHINESE_FONT_PATH = DALI_MODEL_DIR "simhei.ttf";
//const char *KOREAN_FONT_PATH = "/usr/share/fallback_fonts/TizenSansKoreanRegular.ttf";
const char *KOREAN_FONT_PATH = "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf";
const float MENU_OPTION_HEIGHT(140.0f);
const float INCH_TO_POINTS( 72.f );

const Vector4 TEXT_COLOR( 0.f, 0.f, 0.f, 1.f );
const float DEFAULT_SIZE = 30.f;
const float DEFAULT_BOLDNESS = 0.f;
const float DEFAULT_THICKNESS = 1.f;

const int GLYPHY_ATLAS_WIDTH = 256;
const int GLYPHY_ATLAS_HEIGHT = 512;

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

    stage.SetBackgroundColor(Color::WHITE);

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

    Layer glyphyTextLayer = Layer::New();
    glyphyTextLayer.SetClipping(true);
    glyphyTextLayer.SetClippingBox(0, 0, stageSize.x, stageSize.y * 0.5f );
    glyphyTextLayer.SetSize(stageSize.x, stageSize.y);
    glyphyTextLayer.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    glyphyTextLayer.SetParentOrigin( ParentOrigin::TOP_LEFT);
    stage.Add(glyphyTextLayer);
    glyphyTextLayer.Add( mActor );

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
  }

private:

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

    unsigned int numTriangles = glyphVertices->size () / 3;
    unsigned int idx = 0;
    // four values per pixel, need to use vertex position and vertex texture!!
    for(unsigned int index = 0; index < numTriangles; index++)
    {
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

private:

  Application&     mApplication;

  GlyphyTextEffect mShaderEffect;
  GlyphyAtlas*     mAtlas;
  demo_buffer_t*   mBuffer;
  demo_font_t*     mFont;

  Material         mCustomMaterial;
  MeshActor        mActor;

  float   mSize;
  float   mBoldness;
  float   mThickness;

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
