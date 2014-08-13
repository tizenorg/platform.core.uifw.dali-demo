#ifndef OPENGL_FRAMEBUFFER_H
#define OPENGL_FRAMEBUFFER_H

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

#include "opengl-texture.h"

class OpenGLFramebuffer : public OpenGLTexture
{
public:
  OpenGLFramebuffer(int width, int height);
  ~OpenGLFramebuffer();

  /**
   * Prepare the frame buffer for rendering
   */
  bool Prepare();

  void Invalidate();

private:
  int mWidth;
  int mHeight;
  GLuint mFrameBufferName;
};

#endif // OPENGL_FRAMEBUFFER_H
