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
// CLASS HEADER
#include "glyphy-text-effect.h"

// EXTERNAL HEADERS
#include <sstream>

#include "glyphy-src/glyphy-common-glsl.h"
#include "glyphy-src/glyphy-sdf-glsl.h"
#include "glyphy-demo/demo-atlas-glsl.h"
#include "glyphy-demo/demo-fshader-glsl.h"
#include "glyphy-demo/demo-vshader-glsl.h"

namespace
{

template <typename T>
T clamp (T v, T m, T M)
{
  return v < m ? m : v > M ? M : v;
}

#define MAKE_STRING(A)#A

//const std::string ATLAS_INFO_PROPERTY_NAME("uAtlasInfo");

const std::string glslHeaderString = MAKE_STRING(
precision highp float;\n
precision highp int;\n
);

} // namespace

GlyphyTextEffect::GlyphyTextEffect()
/*: mContrast(1.f),
  mGammaAdjust(1.f),
  mOutlineThickness(1.f)
  mBoldness(1.f),
  mIsDebugMode(false),
  mIsOutline(false)*/
{
}

GlyphyTextEffect::GlyphyTextEffect( Dali::ShaderEffect handle )
: Dali::ShaderEffect( handle )
{
}

GlyphyTextEffect::~GlyphyTextEffect()
{
}

GlyphyTextEffect GlyphyTextEffect::New()
{
  std::string glyphyCommonShaderSource( glyphy_common_glsl );
  std::string glyphySdfShaderSource( glyphy_sdf_glsl  );
  std::string vertexShaderSource( demo_vshader_glsl );
  std::string atlasGLSL( demo_atlas_glsl );
  std::string fragmentShaderSource( demo_fshader_glsl );

  std::ostringstream vertexShaderStringStream;
  std::ostringstream fragmentShaderStringStream;

  vertexShaderStringStream//<<"#extension GL_OES_standard_derivatives : enable\n"
                          <<glslHeaderString
                          <<vertexShaderSource;
  fragmentShaderStringStream//<<"#extension GL_OES_standard_derivatives : enable\n"
                            <<glslHeaderString
                            <<atlasGLSL
                            <<glyphyCommonShaderSource
                           <<"#define GLYPHY_SDF_PSEUDO_DISTANCE 1\n"
                            <<glyphySdfShaderSource
                            <<fragmentShaderSource;
  Dali::ShaderEffect shaderEffectCustom =
		  Dali::ShaderEffect::New(vertexShaderStringStream.str(), fragmentShaderStringStream.str(),
                                  Dali::GeometryType( Dali::GEOMETRY_TYPE_TEXTURED_MESH),
                                  Dali::ShaderEffect::GeometryHints( Dali::ShaderEffect::HINT_BLENDING ));

  //std::cout<<vertexShaderStringStream.str()<<std::endl;
  //std::cout<<fragmentShaderStringStream.str()<<std::endl;

  GlyphyTextEffect handle( shaderEffectCustom );

  handle.SetUniform( "u_atlas_info", Dali::Vector4( 2048.f, 1024.f, 64.f, 8.f ));
  handle.SetUniform( "u_contrast", 1.f );
  handle.SetUniform( "u_gamma_adjust", 1.f );
  handle.SetUniform( "u_outline_thickness", 1.f );
  handle.SetUniform( "u_outline", 0.f );
  handle.SetUniform( "u_boldness", 0.f );

  return handle;
}

void GlyphyTextEffect::SetAtlasInfo( const Dali::Vector4& atlasInfo )
{
  SetUniform( "u_atlas_info", atlasInfo );
}

void GlyphyTextEffect::SetContrast( float contrast )
{
  SetUniform( "u_contrast", contrast);
}

void GlyphyTextEffect::SetGammaAdjust( float gammaAdjust )
{
  SetUniform( "u_gamma_adjust", gammaAdjust);
}

void GlyphyTextEffect::SetOutlineThickness( float outlineThickness )
{
  SetUniform( "u_outline_thickness", outlineThickness );
}

void GlyphyTextEffect::ToggleOutline( bool outline  )
{
  SetUniform( "u_outline", outline ? 1.f : 0.f );
}

void GlyphyTextEffect::SetBoldness( float boldness )
{
  SetUniform( "u_boldness", boldness );
}
