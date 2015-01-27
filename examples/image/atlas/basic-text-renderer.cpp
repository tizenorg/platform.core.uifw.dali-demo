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

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Text;

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

    //BitmapImage bitmap = mFontClient.CreateBitmap( glyphs[0].fontId, glyphs[0].index );
    BitmapImage bitmap = CreateTestImage( Color::RED );

    MeshActor actor = MeshActor::New( CreateMesh( glyphs, positions, bitmap ) );
    actor.SetParentOrigin( AnchorPoint::TOP_LEFT );
    actor.SetAffectedByLighting( false );
    mActor = actor;
  }

  return mActor;
}

BasicTextRenderer::BasicTextRenderer()
{
  mFontClient = TextAbstraction::FontClient::Get();
}

BasicTextRenderer::~BasicTextRenderer()
{
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

      std::cout << "size " << width << "," << height << " position: " << position << std::endl;

      vertices[ i*4 + 0 ] = MeshData::Vertex( Vector3( position.x + 0.0f*width, position.y + 0.0f*height, 0.0f ), Vector2( 0.0f, 0.0f ), Vector3( 1.0f, 0.0f, 0.0f ) );
      vertices[ i*4 + 1 ] = MeshData::Vertex( Vector3( position.x + 1.0f*width, position.y + 0.0f*height, 0.0f ), Vector2( 1.0f, 0.0f ), Vector3( 1.0f, 1.0f, 0.0f ) );
      vertices[ i*4 + 2 ] = MeshData::Vertex( Vector3( position.x + 0.0f*width, position.y + 1.0f*height, 0.0f ), Vector2( 0.0f, 1.0f ), Vector3( 0.0f, 1.0f, 0.0f ) );
      vertices[ i*4 + 3 ] = MeshData::Vertex( Vector3( position.x + 1.0f*width, position.y + 1.0f*height, 0.0f ), Vector2( 1.0f, 1.0f ), Vector3( 0.0f, 0.0f, 1.0f ) );

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

BitmapImage BasicTextRenderer::CreateTestImage(const Vector4& color, bool border, const Vector4& borderColor, const unsigned int borderSize)
{
  const unsigned int bitmapWidth = borderSize * 2 + 2;
  BitmapImage image = BitmapImage::New( bitmapWidth, bitmapWidth, Pixel::RGBA8888 );

  // Create the image
  PixelBuffer* pixbuf = image.GetBuffer();
  Vector4 outerColor = color;
  if ( border )
  {
    outerColor = borderColor;
  }

  // Using a (2 + border) x (2 + border) image gives a better blend with the GL implementation
  // than a (1 + border) x (1 + border) image
  const unsigned int bitmapSize = bitmapWidth * bitmapWidth;
  const unsigned int topLeft = bitmapWidth * borderSize + borderSize;
  const unsigned int topRight = topLeft + 1;
  const unsigned int bottomLeft = bitmapWidth * (borderSize + 1) + borderSize;
  const unsigned int bottomRight = bottomLeft + 1;

  for( size_t i = 0; i < bitmapSize; ++i )
  {
    if( i == topLeft ||
        i == topRight ||
        i == bottomLeft ||
        i == bottomRight )
    {
      pixbuf[i*4+0] = 0xFF * color.r;
      pixbuf[i*4+1] = 0xFF * color.g;
      pixbuf[i*4+2] = 0xFF * color.b;
      pixbuf[i*4+3] = 0xFF * color.a;
    }
    else
    {
      pixbuf[i*4+0] = 0xFF * outerColor.r;
      pixbuf[i*4+1] = 0xFF * outerColor.g;
      pixbuf[i*4+2] = 0xFF * outerColor.b;
      pixbuf[i*4+3] = 0xFF * outerColor.a;
    }
  }

  image.Update();

  return image;
}

