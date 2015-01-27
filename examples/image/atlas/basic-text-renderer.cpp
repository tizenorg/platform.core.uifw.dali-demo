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

    BitmapImage bitmap = mFontClient.CreateBitmap( glyphs[0].fontId, glyphs[0].index );

    MeshActor actor = MeshActor::New( CreateMesh( bitmap ) );
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

Mesh BasicTextRenderer::CreateMesh( Image image )
{
  float widthHack( 100.0f );
  float heightHack( 100.0f );

  // Create vertices and specify their color
  MeshData::VertexContainer vertices(4);
  vertices[ 0 ] = MeshData::Vertex( Vector3(  0.0f * widthHack, 0.0f * heightHack, 0.0f ), Vector2(0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f) );
  vertices[ 1 ] = MeshData::Vertex( Vector3(  1.0f * widthHack, 0.0f * heightHack, 0.0f ), Vector2(1.0f, 0.0f), Vector3(1.0f, 1.0f, 0.0f) );
  vertices[ 2 ] = MeshData::Vertex( Vector3(  0.0f * widthHack, 1.0f * heightHack, 0.0f ), Vector2(0.0f, 1.0f), Vector3(0.0f,1.0f,0.0f) );
  vertices[ 3 ] = MeshData::Vertex( Vector3(  1.0f * widthHack, 1.0f * heightHack, 0.0f ), Vector2(1.0f, 1.0f), Vector3(0.0f,0.0f,1.0f) );

  // Specify all the faces
  MeshData::FaceIndices faces;
  faces.reserve( 6 ); // 2 triangles in Quad
  faces.push_back( 0 ); faces.push_back( 3 ); faces.push_back( 1 );
  faces.push_back( 0 ); faces.push_back( 2 ); faces.push_back( 3 );

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

