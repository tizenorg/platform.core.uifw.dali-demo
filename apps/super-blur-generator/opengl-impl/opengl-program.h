#ifndef SUPER_BLUR_GENERATOR_OPENGL_PROGRAM_H_
#define SUPER_BLUR_GENERATOR_OPENGL_PROGRAM_H_

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
 */

#include <stdio.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <string>
#include <utility>

#include "opengl-context.h"
#include "unordered-map.h"

class OpenGLProgram
{
public:
  OpenGLProgram( OpenGLContext& context,
                 const std::string& prefix,
                 const std::string& vertexShaderFile,
                 const std::string& fragmentShaderFile );

  ~OpenGLProgram();

  void Activate();

  void SetFloatUniform( const std::string& name, float value );

  void SetVector2Uniform( const std::string& name, float value1, float value2 );

  GLuint GetAttributeLocation( const std::string& attributeName );

private:
  bool Initialize( const std::string& prefix,
                   const std::string& vertexShaderFile,
                   const std::string& fragmentShaderFile );
  void Terminate();
  GLuint LoadShader( GLenum type, const std::string& source );

private:
  OpenGLContext& mContext;
  GLuint mProgramObject;

  typedef UnorderedMap<std::string, GLint>::Type UniformsMap;
  UniformsMap mUniformLocations;
};

#endif // SUPER_BLUR_GENERATOR_OPENGL_PROGRAM_H_
