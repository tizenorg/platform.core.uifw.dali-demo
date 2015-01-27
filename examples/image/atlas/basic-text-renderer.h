#ifndef __DALI_DEMO_BASIC_TEXT_RENDERER_H__
#define __DALI_DEMO_BASIC_TEXT_RENDERER_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
 *
 */

#include <vector>

#include <dali-toolkit/dali-toolkit.h>

/**
 * @brief Basic implementation of Text::Renderer.
 */
class BasicTextRenderer : public Dali::Toolkit::Text::Renderer
{
public:

  /**
   * @brief Create the renderer.
   */
  static Dali::Toolkit::Text::RendererPtr New();

  /**
   * @brief Render the glyphs from a ViewInterface.
   *
   * @param[in] view The interface to a view.
   * @return The Renderable actor used to position the text.
   */
  virtual Dali::RenderableActor Render( Dali::Toolkit::Text::ViewInterface& view );

protected:

  /**
   * @brief Constructor.
   */
  BasicTextRenderer();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~BasicTextRenderer();

private:

  // TODO
  Dali::Mesh CreateMesh( const Dali::Vector<Dali::Toolkit::Text::GlyphInfo>& glyphs, const std::vector<Dali::Vector2>& positions, Dali::Image image );
  Dali::Atlas CreateAtlas( const Dali::Vector<Dali::Toolkit::Text::GlyphInfo>& glyphs );

  // Undefined
  BasicTextRenderer( const BasicTextRenderer& handle );

  // Undefined
  BasicTextRenderer& operator=( const BasicTextRenderer& handle );

private:

  Dali::RenderableActor mActor;

  Dali::TextAbstraction::FontClient mFontClient;

  struct AtlasHelper;
  struct AtlasHelperGlyph;
  AtlasHelper* mAtlasHelper;
};


#endif // __DALI_DEMO_BASIC_TEXT_RENDERER_H__
