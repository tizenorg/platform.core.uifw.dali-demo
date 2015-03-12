#ifndef __DALI_DEMO_EMOJI_STRINGS_H__
#define __DALI_DEMO_EMOJI_STRINGS_H__

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
#include <string>

namespace EmojiStrings
{
  struct Emoji
  {
    std::string mUTF8;
    std::string mDescription;
  };

  const Emoji EMOJIS[] = {

    { "\xF0\x9F\x98\x81","grinning face with smiling eyes" },
    { "\xF0\x9F\x98\x82","face with tears of joy" },
    { "\xF0\x9F\x98\x85","smiling face with open mouth and cold sweat" },
    { "\xF0\x9F\x98\x89","winking face" },
    { "\xF0\x9F\x98\x8A","smiling face with smiling eyes" },
    { "\xF0\x9F\x98\x8B","face savouring delicious food" },
    { "\xF0\x9F\x98\x8C","relieved face" },
    { "\xF0\x9F\x98\x8D","smiling face with heart-shaped eyes" },
    { "\xF0\x9F\x98\x8F","smirking face" },
    { "\xF0\x9F\x98\x98","face throwing a kiss" },
    { "\xF0\x9F\x98\x9A","kissing face with closed eyes" },
    { "\xF0\x9F\x98\xA0","angry face" },
    { "\xF0\x9F\x99\x88","see-no-evil monkey" },
    { "\0", "\0" }

  };

} // EmojiStrings

#endif // __DALI_DEMO_EMOJI_STRINGS_H__
