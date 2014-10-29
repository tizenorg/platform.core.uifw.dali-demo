#ifndef __DALI_DEMO_GLYPHY_TEXT_EFFECT_H__
#define __DALI_DEMO_GLYPHY_TEXT_EFFECT_H__

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
 *
 */

// INTERNEL INCLUDES
#include <dali/dali.h>

class GlyphyTextEffect : public Dali::ShaderEffect
{
public:

	GlyphyTextEffect();
	~GlyphyTextEffect();

	static GlyphyTextEffect New();

	//at->tex_w, at->tex_h, at->item_w, at->item_h_q
	void SetAtlasInfo( const Dali::Vector4& atlasInfo );

	void SetContrast( float contrast );

	void SetGammaAdjust( float gammaAdjust );

	void SetOutlineThickness( float outlineThickness );

	void ToggleOutline( bool outline );

	void SetBoldness( float boldness );

	void ToggleDebugMode( bool debug );

private: // Not intended for application developer
  GlyphyTextEffect( ShaderEffect handle );

/*private:

  float mContrast;
  float mGammaAdjist;
  float mOutlineThickness;
  float mBoldness;

  bool  mIsDebugMode;
  bool  mIsOutline;*/

}; // class GlyphyTextEffect

#endif // __DALI_DEMO_GLYPHY_TEXT_EFFECT_H__
