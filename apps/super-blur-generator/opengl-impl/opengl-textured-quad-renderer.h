#ifndef SUPER_BLUR_GENERATOR_OPENGL_TEXTURED_QUAD_RENDERER_H_
#define SUPER_BLUR_GENERATOR_OPENGL_TEXTURED_QUAD_RENDERER_H_

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

#include "opengl-program.h"
#include "opengl-context.h"
#include "opengl-texture.h"
#include "opengl-framebuffer.h"

class OpenGLTexturedQuadRenderer
{
public:
  OpenGLTexturedQuadRenderer( OpenGLContext& context, OpenGLProgram& program, const OpenGLTexture& texture, float scale = 1.0f );
  ~OpenGLTexturedQuadRenderer();

private:
  OpenGLContext& mContext;
  const OpenGLTexture& mTexture;
  GLuint mVertexBuffer;
  float mScale;

};

#endif // SUPER_BLUR_GENERATOR_OPENGL_TEXTURED_QUAD_RENDERER_H_
