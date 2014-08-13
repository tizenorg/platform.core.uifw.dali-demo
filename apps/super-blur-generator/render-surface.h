#ifndef SUPER_BLUR_GENERATOR_RENDER_SURFACE_H_
#define SUPER_BLUR_GENERATOR_RENDER_SURFACE_H_

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

#include "implementation-pointer.h"
#include "context.h"

class OpenGLRenderSurface;

/**
 * Represents a render surface
 */
class RenderSurface
{
public:
  /**
   * Constructor
   */
  RenderSurface( Context& context,
                 unsigned int width,
                 unsigned int height);

  /**
   * Destructor
   */
  ~RenderSurface();

  /**
   * Read the pixel data from the render surface
   * @return On success, a pointer to the pixel data. The client should delete[] this data when
   * finished with it.
   */
  unsigned char* ReadPixelData();

private:
  ImplementationPointer<OpenGLRenderSurface> mImpl;

public:
  OpenGLRenderSurface& GetImpl()
  {
    return *mImpl;
  }
  const OpenGLRenderSurface& GetImpl() const
  {
    return *mImpl;
  }
};

#endif // SUPER_BLUR_GENERATOR_RENDER_SURFACE_H_
