#ifndef __DALI_DEMO_GLYPHY_SVG_EFFECT_H__
#define __DALI_DEMO_GLYPHY_SVG_EFFECT_H__

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

// INTERNEL INCLUDES
#include <dali/dali.h>

class GlyphySVGEffect : public Dali::ShaderEffect
{
public:

  GlyphySVGEffect();
	~GlyphySVGEffect();

	static GlyphySVGEffect New();

	//at->tex_w, at->tex_h, at->item_w, at->item_h_q
	void SetAtlasInfo( const Dali::Vector4& atlasInfo );

	void ConstraintStrokeScale( Dali::ConstraintSource actorScale );

private: // Not intended for application developer
	GlyphySVGEffect( ShaderEffect handle );

}; // class GlyphySVGEffect

#endif // __DALI_DEMO_GLYPHY_SVG_EFFECT_H__