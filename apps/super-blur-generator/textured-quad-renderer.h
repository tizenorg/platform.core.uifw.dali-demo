#ifndef SUPER_BLUR_GENERATOR_TEXTURED_QUAD_RENDERER_H_
#define SUPER_BLUR_GENERATOR_TEXTURED_QUAD_RENDERER_H_

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
#include "framebuffer.h"
#include "texture.h"
#include "program.h"

class OpenGLTexturedQuadRenderer;

class TexturedQuadRenderer
{
public:

  /**
   * Constructor
   * @param[in] context The gl context
   * @param[in] program The gl shader program to use
   * @param[in] image to upload to texture
   */
  TexturedQuadRenderer( Context& context, Program& program, const Texture& texture, float scale=1.0f );
  TexturedQuadRenderer( Context& context, Program& program, const Framebuffer& framebuffer, float scale=1.0f );

  /**
   * Destructor
   */
  ~TexturedQuadRenderer();

private:
  ImplementationPointer<OpenGLTexturedQuadRenderer> mImpl;

public:
  OpenGLTexturedQuadRenderer& GetImpl()
  {
    return *mImpl;
  }
  const OpenGLTexturedQuadRenderer& GetImpl() const
  {
    return *mImpl;
  }
};

#endif // SUPER_BLUR_GENERATOR_TEXTURED_QUAD_RENDERER_H_
