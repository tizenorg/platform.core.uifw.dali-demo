#ifndef __DALI_DEMO_BASIC_TEXT_SHADER_H__
#define __DALI_DEMO_BASIC_TEXT_SHADER_H__

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

#include <dali/dali.h>

/**
 * @brief A basic shader for rendering glyphs in Pixel::L8 format.
 */
namespace BasicTextShader
{

/**
 * Create a basic text shader.
 * @return A handle to a newly allocated ShaderEffect
 */
static Dali::ShaderEffect New()
{
  std::string vertexShader = DALI_COMPOSE_SHADER(
      uniform mediump vec4 uTextureRect;\n
      void main()\n
      {\n
        gl_Position = uMvpMatrix * vec4( aPosition.xy, 0.0, 1.0 );\n
        vTexCoord = aTexCoord.xy;\n
      }\n
  );

  std::string fragmentShader = DALI_COMPOSE_SHADER(
      void main()\n
      {\n
        mediump vec4 color = texture2D( sTexture, vTexCoord );
        gl_FragColor = vec4(uColor.rgb, uColor.a*color.r);
      }\n
  );

  Dali::ShaderEffect shaderEffect = Dali::ShaderEffect::New( vertexShader, fragmentShader,
                                                             Dali::GeometryType( Dali::GEOMETRY_TYPE_TEXTURED_MESH ),
                                                             Dali::ShaderEffect::GeometryHints( Dali::ShaderEffect::HINT_NONE ) );
  return shaderEffect;
}

} // namespace BasicTextShader

#endif // __DALI_DEMO_BASIC_TEXT_SHADER_H__
