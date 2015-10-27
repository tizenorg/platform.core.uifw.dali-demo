/*
 * image region effect
 *
 * Copyright (c) 2000 - 2015 Samsung Electronics Co., Ltd. All rights reserved.
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

// CLASS HEADER
#include "image-region-effect.h"

using namespace Dali;

namespace ClusterHome3D
{

namespace ImageRegionEffect
{

const std::string TOP_LEFT_PROPERTY_NAME( "uTopLeft" );
const std::string BOTTOM_RIGHT_PROPERTY_NAME( "uBottomRight" );

ShaderEffect Create()
{
  const char* IMAGE_REGION_VERTEX_SHADER =
    "uniform mediump vec2 uTopLeft;\n"
    "uniform mediump vec2 uBottomRight;\n"
    "void main()\n"
    "{\n"
    "  mediump vec4 position = vec4(aPosition,1.0);\n"
    "  gl_Position = uMvpMatrix * position;\n"
    // The line below is doing the same as the following commented lines:
    //"  vec2 imageSize = sTextureRect.zw - sTextureRect.xy;\n"
    //"  vec2 topLeft = sTextureRect.xy + uTopLeft * imageSize;\n"
    //"  vec2 bottomRight = sTextureRect.xy + uBottomRight * imageSize;\n"
    //"  vec2 texCoord = (aTexCoord - sTextureRect.xy) / imageSize;\n"
    //"  vTexCoord = topLeft + texCoord * ( bottomRight - topLeft );\n"
    "  vTexCoord = sTextureRect.xy + uTopLeft * ( sTextureRect.zw - sTextureRect.xy ) + ( aTexCoord - sTextureRect.xy ) * ( uBottomRight - uTopLeft );\n"
    "}\n";

  ShaderEffect effect = ShaderEffect::New(IMAGE_REGION_VERTEX_SHADER, "");
  effect.SetUniform( TOP_LEFT_PROPERTY_NAME, Vector2( 0.f, 0.f ) );
  effect.SetUniform( BOTTOM_RIGHT_PROPERTY_NAME, Vector2( 1.f, 1.f ) );
  return effect;
}

void SetTopLeft(ShaderEffect effect, const Vector2& point)
{
  effect.SetUniform(TOP_LEFT_PROPERTY_NAME, point);
}

void SetBottomRight(ShaderEffect effect, const Vector2& point)
{
  effect.SetUniform(BOTTOM_RIGHT_PROPERTY_NAME, point);
}

const std::string& GetTopLeftPropertyName()
{
  return TOP_LEFT_PROPERTY_NAME;
}

const std::string& GetBottomRightPropertyName()
{
  return BOTTOM_RIGHT_PROPERTY_NAME;
}

} // namespace UnpremultiplyEffect

} // namespace ClusterHome3D

