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

#include "textured-quad-renderer.h"

#include "opengl-impl/opengl-textured-quad-renderer.h"

TexturedQuadRenderer::TexturedQuadRenderer( Context& context, Program& program, const Texture& texture, float scale )
: mImpl( new OpenGLTexturedQuadRenderer( context.GetImpl(), program.GetImpl(), texture.GetImpl(), scale ) )
{
}

TexturedQuadRenderer::TexturedQuadRenderer( Context& context, Program& program, const Framebuffer& framebuffer, float scale )
: mImpl( new OpenGLTexturedQuadRenderer( context.GetImpl(), program.GetImpl(), framebuffer.GetImpl(), scale ) )
{
}

TexturedQuadRenderer::~TexturedQuadRenderer()
{
}
