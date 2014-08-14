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

/**
 * SuperBlurGerator creates blur levels from an image file to be used with
 * the SuperBlurView
 */

#include "program.h"

#include "opengl-impl/opengl-context.h"
#include "opengl-impl/opengl-program.h"

Program::Program( Context& context,
                  const std::string& prefix,
                  const std::string& vertexShaderFile,
                  const std::string& fragmentShaderFile )
: mImpl( new OpenGLProgram( context.GetImpl(),
                            prefix,
                            vertexShaderFile,
                            fragmentShaderFile ) )
{
}

Program::~Program()
{
}

/**
 * Activate this shader
 */
void Program::Activate()
{
  GetImpl().Activate();
}

/**
 * Set uniform with a float value
 */
void Program::SetFloatUniform( const std::string& name, float value )
{
  GetImpl().SetFloatUniform( name, value );
}

/**
 * Set uniform with 2 float values
 */
void Program::SetVector2Uniform( const std::string& name, float value1, float value2 )
{
  GetImpl().SetVector2Uniform( name, value1, value2 );
}
