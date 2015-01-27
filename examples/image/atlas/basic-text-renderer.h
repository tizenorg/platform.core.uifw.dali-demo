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

// EXTERNAL INCLUDES
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

  /**
   * @brief Helper method to create an Atlas, uploading the necessary glyph bitmaps
   *
   * @param[in] glyphs The glyphs to upload.
   */
  Dali::Atlas CreateAtlas( const Dali::Vector<Dali::Toolkit::Text::GlyphInfo>& glyphs );

  /**
   * @brief Helper method to create a mesh with one quad per glyph.
   *
   * @param[in] glyphs The glyphs to display.
   * @param[in] positions The 2D positions of the glyphs.
   * @param[in] image The material uses this as a diffuse texture.
   */
  Dali::Mesh CreateMesh( const Dali::Vector<Dali::Toolkit::Text::GlyphInfo>& glyphs, const std::vector<Dali::Vector2>& positions, Dali::Image image );

  // Undefined
  BasicTextRenderer( const BasicTextRenderer& handle );

  // Undefined
  BasicTextRenderer& operator=( const BasicTextRenderer& handle );

private:

  Dali::RenderableActor mActor; ///< The actor which renders the text

  Dali::TextAbstraction::FontClient mFontClient; ///< Used to get glyph bitmaps

  struct AtlasHelper;
  struct AtlasHelperGlyph;
  AtlasHelper* mAtlasHelper; ///< A helper class for storing atlas positions etc.
};


#endif // __DALI_DEMO_BASIC_TEXT_RENDERER_H__
