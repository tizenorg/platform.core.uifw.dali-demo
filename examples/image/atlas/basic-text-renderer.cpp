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

// INTERNAL INCLUDES
#include "basic-text-renderer.h"
#include "basic-text-shader.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Text;

namespace
{

const std::size_t PADDING = 2; //< To avoid GL filtering artefacts

struct TextureCoordinates
{
  TextureCoordinates()
  : topLeft( 0.0f, 0.0f ),
    topRight( 1.0f, 0.0f ),
    bottomLeft( 0.0f, 1.0f ),
    bottomRight( 1.0f, 1.0f )
  {
  }

  Vector2 topLeft;
  Vector2 topRight;
  Vector2 bottomLeft;
  Vector2 bottomRight;
};

} // unnamed namespace

struct BasicTextRenderer::AtlasHelperGlyph
{
  AtlasHelperGlyph()
  : fontId( 0 ),
    index( 0 ),
    xOffset( 0 ),
    width( 0 ),
    height( 0 )
  {
  }

  AtlasHelperGlyph( FontId id,
                    GlyphIndex glyphIndex,
                    std::size_t offset,
                    std::size_t widthPixels,
                    std::size_t heightPixels )
  : fontId( id ),
    index( glyphIndex ),
    xOffset( offset ),
    width( widthPixels ),
    height( heightPixels )
  {
  }

  FontId fontId;
  GlyphIndex index;
  std::size_t xOffset;
  std::size_t width;
  std::size_t height;
  TextureCoordinates coords;
};

struct BasicTextRenderer::AtlasHelper
{
  AtlasHelper()
  : mWidth( 0.0f ),
    mHeight( 0.0f )
  {
    mFontClient = TextAbstraction::FontClient::Get();
  }

  void Reset()
  {
    mWidth = 0.0f;
    mHeight = 0.0f;
    mGlyphs.clear();
  }

  void Reserve( std::size_t size )
  {
    mGlyphs.reserve( size );
  }

  bool GlyphFound( FontId fontId, GlyphIndex index ) const
  {
    for( unsigned int i=0; i<mGlyphs.size(); ++i )
    {
      const AtlasHelperGlyph& glyph = mGlyphs[i];

      if( fontId == glyph.fontId &&
          index  == glyph.index )
      {
        return true;
      }
    }

    return false;
  }

  void AddGlyph( const GlyphInfo& glyphInfo )
  {
    mGlyphs.push_back( AtlasHelperGlyph( glyphInfo.fontId, glyphInfo.index, mWidth, glyphInfo.width, glyphInfo.height ) );

    mWidth += glyphInfo.width + PADDING;
    if( mHeight < glyphInfo.height + PADDING )
    {
      mHeight = glyphInfo.height + PADDING;
    }
  }

  Atlas CreateAtlas()
  {
    Atlas atlas = Atlas::New( mWidth, mHeight, Pixel::L8 );

    for( unsigned int i=0; i<mGlyphs.size(); ++i )
    {
      AtlasHelperGlyph& glyph = mGlyphs[i];
      BitmapImage bitmap = mFontClient.CreateBitmap( glyph.fontId, glyph.index );
      atlas.Upload( bitmap, glyph.xOffset, 0 );

      TextureCoordinates& coords = glyph.coords;
      coords.topLeft.x     = static_cast<float>(glyph.xOffset) / static_cast<float>(mWidth);
      coords.topLeft.y     = 0.0f;
      coords.topRight.x    = static_cast<float>(glyph.xOffset + glyph.width) / static_cast<float>(mWidth);
      coords.topRight.y    = 0.0f;
      coords.bottomLeft.x  = static_cast<float>(glyph.xOffset) / static_cast<float>(mWidth);
      coords.bottomLeft.y  = static_cast<float>(glyph.height) / static_cast<float>(mHeight);
      coords.bottomRight.x = static_cast<float>(glyph.xOffset + glyph.width) / static_cast<float>(mWidth);
      coords.bottomRight.y = static_cast<float>(glyph.height) / static_cast<float>(mHeight);
    }

    return atlas;
  }

  void GetTextureCoordinates( FontId fontId, GlyphIndex index, TextureCoordinates& coords )
  {
    for( unsigned int i=0; i<mGlyphs.size(); ++i )
    {
      const AtlasHelperGlyph& glyph = mGlyphs[i];

      if( fontId == glyph.fontId &&
          index  == glyph.index )
      {
        coords = glyph.coords;
        return;
      }
    }
  }

private: // Data

  std::size_t mWidth;
  std::size_t mHeight;

  std::vector<AtlasHelperGlyph> mGlyphs;

  TextAbstraction::FontClient mFontClient;
};

Text::RendererPtr BasicTextRenderer::New()
{
  return Text::RendererPtr( new BasicTextRenderer() );
}

RenderableActor BasicTextRenderer::Render( Text::ViewInterface& view )
{
  Text::Length numberOfGlyphs = view.GetNumberOfGlyphs();

  if( numberOfGlyphs > 0 )
  {
    Vector<GlyphInfo> glyphs;
    glyphs.Resize( numberOfGlyphs );

    view.GetGlyphs( 0, &glyphs[0], numberOfGlyphs );

    std::vector<Vector2> positions;
    positions.resize( numberOfGlyphs );
    view.GetGlyphPositions( 0, &positions[0], numberOfGlyphs );

    Atlas atlas = CreateAtlas( glyphs );

    MeshActor actor = MeshActor::New( CreateMesh( glyphs, positions, atlas ) );
    actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
    actor.SetAffectedByLighting( false );

    ShaderEffect shader = BasicTextShader::New();
    actor.SetShaderEffect( shader );

    mActor = actor;
  }

  return mActor;
}

BasicTextRenderer::BasicTextRenderer()
{
  mAtlasHelper = new BasicTextRenderer::AtlasHelper();
}

BasicTextRenderer::~BasicTextRenderer()
{
  delete mAtlasHelper;
}

Atlas BasicTextRenderer::CreateAtlas( const Vector<GlyphInfo>& glyphs )
{
  // Clear previous atlas
  mAtlasHelper->Reset();
  mAtlasHelper->Reserve( glyphs.Count() );

  for( unsigned int i=0; i<glyphs.Count(); ++i )
  {
    float width  = glyphs[i].width;
    float height = glyphs[i].height;

    if( width > 0 &&
        height > 0 ) // skip whitespace
    {
      if( !mAtlasHelper->GlyphFound( glyphs[i].fontId, glyphs[i].index ) )
      {
        mAtlasHelper->AddGlyph( glyphs[i] );
      }
    }
  }

  // Uploads the bitmaps to Dali
  return mAtlasHelper->CreateAtlas();
}

Mesh BasicTextRenderer::CreateMesh( const Vector<GlyphInfo>& glyphs, const std::vector<Vector2>& positions, Image image )
{
  MeshData::VertexContainer vertices( 4 * glyphs.Count() ); // 1 quad per glyph

  MeshData::FaceIndices faces;
  faces.reserve( 6 * glyphs.Count() ); // 2 triangles per quad

  for( unsigned int i=0; i<glyphs.Count(); ++i )
  {
    float width  = glyphs[i].width;
    float height = glyphs[i].height;

    if( width > 0 &&
        height > 0 ) // skip whitespace
    {
      const Vector2& position = positions[i];

      TextureCoordinates coords;
      mAtlasHelper->GetTextureCoordinates( glyphs[i].fontId, glyphs[i].index, coords );

      vertices[ i*4 + 0 ] = MeshData::Vertex( Vector3( position.x + 0.0f*width, position.y + 0.0f*height, 0.0f ), coords.topLeft,     Vector3( 1.0f, 0.0f, 0.0f ) );
      vertices[ i*4 + 1 ] = MeshData::Vertex( Vector3( position.x + 1.0f*width, position.y + 0.0f*height, 0.0f ), coords.topRight,    Vector3( 1.0f, 1.0f, 0.0f ) );
      vertices[ i*4 + 2 ] = MeshData::Vertex( Vector3( position.x + 0.0f*width, position.y + 1.0f*height, 0.0f ), coords.bottomLeft,  Vector3( 0.0f, 1.0f, 0.0f ) );
      vertices[ i*4 + 3 ] = MeshData::Vertex( Vector3( position.x + 1.0f*width, position.y + 1.0f*height, 0.0f ), coords.bottomRight, Vector3( 0.0f, 0.0f, 1.0f ) );

      faces.push_back( i*4 + 0 ); faces.push_back( i*4 + 3 ); faces.push_back( i*4 + 1 );
      faces.push_back( i*4 + 0 ); faces.push_back( i*4 + 2 ); faces.push_back( i*4 + 3 );
    }
  }

  Material material = Material::New( "Material" );
  material.SetDiffuseTexture( image );

  // Create the mesh data from the vertices and faces
  MeshData meshData;
  meshData.SetHasColor( false );
  meshData.SetMaterial( material );
  meshData.SetVertices( vertices );
  meshData.SetFaceIndices( faces );

  // Create a mesh from the data
  Dali::Mesh mesh = Mesh::New( meshData );
  return mesh;
}

