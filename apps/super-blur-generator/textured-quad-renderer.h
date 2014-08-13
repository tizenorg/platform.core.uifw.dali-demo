/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

#ifndef SUPER_BLUR_GENERATOR_TEXTURED_QUAD_RENDERER_H_
#define SUPER_BLUR_GENERATOR_TEXTURED_QUAD_RENDERER_H_

#include "implementation-pointer.h"
#include "context.h"
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
  TexturedQuadRenderer( Context& context, Program& program, const Texture& texture );

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
