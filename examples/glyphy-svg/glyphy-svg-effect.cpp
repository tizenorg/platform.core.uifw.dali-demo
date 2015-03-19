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
#include "glyphy-svg-effect.h"

// EXTERNAL HEADERS
#include <sstream>

#include "glyphy-common-glsl.h"
#include "glyphy-sdf-glsl.h"
#include "demo-atlas-glsl.h"
#include "demo-fshader-glsl.h"
#include "demo-vshader-glsl.h"

namespace
{
const std::string enableExtensionString("#extension GL_OES_standard_derivatives : enable\nprecision highp float;\nprecision highp int;\n");
} // namespace

GlyphySVGEffect::GlyphySVGEffect()
{
}

GlyphySVGEffect::GlyphySVGEffect( Dali::ShaderEffect handle )
: Dali::ShaderEffect( handle )
{
}

GlyphySVGEffect::~GlyphySVGEffect()
{
}

GlyphySVGEffect GlyphySVGEffect::New( unsigned int gradientCount )
{
  std::string glyphyCommonShaderSource( glyphy_common_glsl );
  std::string glyphySdfShaderSource( glyphy_sdf_glsl  );
  std::string vertexShaderSource( demo_vshader_glsl );
  std::string atlasGLSL( demo_atlas_glsl );
  std::string fragmentShaderSource( demo_fshader_glsl );

  std::ostringstream vertexShaderStringStream;
  std::ostringstream fragmentShaderStringStream;

  int count = gradientCount > 0 ? gradientCount*2 : 2;
  vertexShaderStringStream<< "#define GRADIENT_INFO_COUNT "<< count << "\n"<<vertexShaderSource;

  fragmentShaderStringStream<<atlasGLSL
                            <<glyphyCommonShaderSource
                            <<glyphySdfShaderSource
                            <<fragmentShaderSource;
  Dali::ShaderEffect shaderEffectCustom =
		  Dali::ShaderEffect::NewWithPrefix(enableExtensionString, vertexShaderStringStream.str(),
				                            enableExtensionString, fragmentShaderStringStream.str(),
                                            Dali::GeometryType( Dali::GEOMETRY_TYPE_TEXTURED_MESH),
                                            Dali::ShaderEffect::GeometryHints( Dali::ShaderEffect::HINT_BLENDING ));

  GlyphySVGEffect handle( shaderEffectCustom );

  handle.SetUniform( "u_atlas_info", Dali::Vector4( 1024.f, 1024.f, 64.f, 8.f ));
  handle.SetUniform( "u_scale", 1.f );
  handle.mGradientInfoCount = gradientCount*6;
  return handle;
}

void GlyphySVGEffect::SetAtlasInfo( const Dali::Vector4& atlasInfo )
{
  SetUniform( "u_atlas_info", atlasInfo );
}

void GlyphySVGEffect::ConstraintStrokeScale( Dali::ConstraintSource actorScale )
{
  Dali::Handle handle = *this;
  Dali::Constraint strokeScaleConstraint = Dali::Constraint::New<float>(handle, GetPropertyIndex( "u_scale" ), Dali::EqualToConstraint());
  strokeScaleConstraint.AddSource(actorScale);
  strokeScaleConstraint.Apply();
}

void GlyphySVGEffect::SetGradientInfo( const Dali::Vector<float>& gradientInfo )
{
  DALI_ASSERT_ALWAYS( mGradientInfoCount == gradientInfo.Size() );
  for( unsigned int i=0; i<mGradientInfoCount; i=i+3 )
  {
    std::ostringstream oss;
    oss<< "u_gradient_info["<< i/3 << "]";
    Dali::Vector3 gradient(gradientInfo[i],gradientInfo[i+1],gradientInfo[i+2]);
    SetUniform( oss.str(), gradient );
  }
}
