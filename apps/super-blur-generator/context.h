#ifndef SUPER_BLUR_GENERATOR_CONTEXT_H_
#define SUPER_BLUR_GENERATOR_CONTEXT_H_

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

#include <cstddef>
#include "implementation-pointer.h"

class OpenGLContext;

/**
 * Represents a rendering context
 */
class Context
{
public:
  /**
   * Constructor
   */
  Context();

  /**
   * Destructor
   */
  ~Context();
  void Prepare(std::size_t width, std::size_t height);

private:
  ImplementationPointer<OpenGLContext> mImpl;

public:
  OpenGLContext& GetImpl()
  {
    return *mImpl;
  }
  const OpenGLContext& GetImpl() const
  {
    return *mImpl;
  }
};

#endif // SUPER_BLUR_GENERATOR_CONTEXT_H_
