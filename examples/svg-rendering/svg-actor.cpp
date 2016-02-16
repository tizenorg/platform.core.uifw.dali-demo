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

//CLASS HEADER
#include "svg-actor.h"

// EXTERNAL HEADERS
#include <sstream>

// INTERNAL INCLUDES
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/property-map.h>

// LOCAL INCLUDES
#define MAKE_STRING(A)#A
#include "glyphy-common-glsl.h"
#include "winding-glsl.h"
#include "glyphy-sdf-glsl.h"
#include "svg-fshader-glsl.h"
#include "svg-vshader-glsl.h"

Shader SVGActor::CreateShader()
{
  std::ostringstream fragmentShaderStringStream;
  fragmentShaderStringStream<<"#extension GL_OES_standard_derivatives : enable\n"
                            <<"precision highp float;\n"
                            <<"precision highp int;\n"
                            <<"#define GLYPHY_INFINITY 1e5\n"
                            <<"#define GLYPHY_EPSILON  1e-5\n"
                            <<"#define GLYPHY_MAX_D 0.75\n"
                            <<glyphyCommonShaderSource
                            <<WindingShaderSource
                            <<glyphySdfShaderSource
                            <<fragmentShaderSource;

  Shader shader = Shader::New( vertexShaderSource,
                               fragmentShaderStringStream.str() );

  return shader;
}

void SVGActor::Initialize()
{
  mActor = Actor::New();
  mGeometry = Geometry::New();
  mMaterial = Material::New( CreateShader() );

  mRenderer = Renderer::New( mGeometry, mMaterial );
  mActor.AddRenderer( mRenderer );

  // register uniforms
  mIndexAtlasInfo = mMaterial.RegisterProperty( "uAtlasInfo", Vector4( 1024.f, 1024.f, 64.f, 8.f ) );
  mIndexStrokeScale = mActor.RegisterProperty( "uStrokeScale", 1.f );
  mIndexGradientTexturePos = mMaterial.RegisterProperty( "uGradientTexturePos", Vector2::ZERO );
  mIndexCenter = mGeometry.RegisterProperty( "uCenter", Vector2::ZERO );
}

void SVGActor::SetVertices( std::vector<Vertex>* vertexData )
{
  Property::Map vertexFormat;
  vertexFormat["aPosition"] = Property::VECTOR2;
  vertexFormat["aGlyphyInfo"] = Property::VECTOR2;
  vertexFormat["aFillColor"] = Property::VECTOR4;
  vertexFormat["aStrokeColor"] = Property::VECTOR4;
  vertexFormat["aGradientParamX"] = Property::VECTOR3;
  vertexFormat["aGradientParamY"] = Property::VECTOR3;
  vertexFormat["aStrokeWidth"] = Property::FLOAT;
  vertexFormat["aGradientIndex"] = Property::FLOAT;

  unsigned int vertexCount = vertexData->size();
  PropertyBuffer vertices = PropertyBuffer::New( vertexFormat, vertexCount );
  vertices.SetData( &(vertexData->at(0)) );

  unsigned int indexCount = vertexCount * 3u / 2u;
  Vector<unsigned int> indexData;
  indexData.Reserve( indexCount );
  for( unsigned int i = 0; i < vertexCount; i+=4u )
  {
    indexData.PushBack( i );
    indexData.PushBack( i+1 );
    indexData.PushBack( i+2 );
    indexData.PushBack( i+1 );
    indexData.PushBack( i+2 );
    indexData.PushBack( i+3 );
  }

  Property::Map indexFormat;
  indexFormat["indices"] = Property::INTEGER;
  PropertyBuffer indices = PropertyBuffer::New( indexFormat, indexCount  );
  indices.SetData( &indexData[0] );

  mGeometry.AddVertexBuffer( vertices );
  mGeometry.SetIndexBuffer( indices );
}

void SVGActor::SetSvgAtlas( BufferImage glyphyAtlas )
{
  mMaterial.AddTexture( glyphyAtlas, "sSvgAtlas" );
}

void SVGActor::SetGradientTexturePos( const Vector2& gradientTexPos )
{
  mMaterial.SetProperty( mIndexGradientTexturePos, gradientTexPos );
}

void SVGActor::SetAtlasInfo( const Vector4& atlasInfo )
{
  mMaterial.SetProperty( mIndexAtlasInfo, atlasInfo );
}

void SVGActor::SetCenter(const Vector2& center)
{
  mGeometry.SetProperty( mIndexCenter, center );
}

void SVGActor::ConstraintStrokeScale( )
{
  Constraint strokeScaleConstraint = Constraint::New<float>(mActor, mIndexStrokeScale, Dali::EqualToConstraint());
  strokeScaleConstraint.AddSource( LocalSource( Actor::Property::SCALE_X ) );
  strokeScaleConstraint.Apply();
}

Actor SVGActor::GetMeshActor()
{
  return mActor;
}
