#ifndef __DALI_DEMO_SVG_ACTOR_H__
#define __DALI_DEMO_SVG_ACTOR_H__

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

#include <dali/public-api/actors/actor.h>
#include <dali/public-api/images/buffer-image.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>

#include <dali/devel-api/rendering/renderer.h>
#include <dali/devel-api/rendering/shader.h>

using namespace Dali;

class SVGActor : public RefObject
{
public:
  struct Vertex
  {
    Vector2      position;
    Vector2      glyphyInfo;
    Vector4      fillColor;
    Vector4      strokeColor;
    Vector3      gradientParamX;
    Vector3      gradientParamY;
    float        strokeWidth;
    float        gradientIndex;

    Vertex( Vector2 position, Vector2 glyphyInfo,
            Vector4 fillColor, Vector4 strokeColor,
            Vector3 gradientParamX, Vector3 gradientParamY,
            float strokeWidth, float gradientIndex )
    : position( position ), glyphyInfo( glyphyInfo ),
      fillColor( fillColor ), strokeColor( strokeColor ),
      gradientParamX( gradientParamX ), gradientParamY( gradientParamY ),
      strokeWidth( strokeWidth ), gradientIndex( gradientIndex )
    {
    }

  };

  SVGActor(){};

  ~SVGActor(){};

  void Initialize();

  void SetVertices( std::vector<Vertex>* vertexData );

  void SetGlyphyAtlas( BufferImage glyphyAtlas );

  void SetGradientLookupTexture( BufferImage lookupTexture );

  void SetAtlasInfo( const Vector4& atlasInfo );

  void SetGradientCount( int count );

  void ConstraintStrokeScale( );

  Actor GetMeshActor();

private:

  Shader CreateShader();

private:
  Actor           mActor;
  Renderer        mRenderer;
  Geometry        mGeometry;
  Material        mMaterial;

  // uniform properties
  Property::Index mIndexAtlasInfo;
  Property::Index mIndexStrokeScale;
  Property::Index mIndexGradientCount;
};

#endif // __DALI_DEMO_SVG_ACTOR_H__
