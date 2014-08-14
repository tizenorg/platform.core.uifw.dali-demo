#ifndef SUPER_BLUR_GENERATOR_PROGRAM_H_
#define SUPER_BLUR_GENERATOR_PROGRAM_H_

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

#include <string>

#include "implementation-pointer.h"
#include "context.h"

class OpenGLProgram;

/**
 * Represents a render surface
 */
class Program
{
public:
  /**
   * Constructor
   */
  Program( Context& context,
           const std::string& prefix,
           const std::string& vertexShaderFile,
           const std::string& fragmentShaderFile );

  /**
   * Destructor
   */
  ~Program();

  /**
   * Activate this shader
   */
  void Activate();

  /**
   * Set uniform with a float value
   */
  void SetFloatUniform( const std::string& name, float value );

  /**
   * Set uniform with 2 float values
   */
  void SetVector2Uniform( const std::string& name, float value1, float value2 );

private:
  ImplementationPointer<OpenGLProgram> mImpl;

public:
  OpenGLProgram& GetImpl()
  {
    return *mImpl;
  }
  const OpenGLProgram& GetImpl() const
  {
    return *mImpl;
  }
};

#endif // SUPER_BLUR_GENERATOR_PROGRAM_H_
