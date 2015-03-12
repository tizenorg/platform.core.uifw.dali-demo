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

// INTERNAL INCLUDES
#include "vertical-layout.h"
#include "emoji-strings.h"

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/text-abstraction/text-abstraction.h>
#include <iostream>
#include <stdio.h>

//#define SHOW_UNICODE

namespace
{
  const static uint8_t U1 = 1u;
  const static uint8_t U2 = 2u;
  const static uint8_t U3 = 3u;
  const static uint8_t U4 = 4u;
  const static uint8_t U0 = 0u;
  const static uint8_t UTF8_LENGTH[256] = {
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, // lead byte = 0xxx xxxx (U+0000 - U+007F + some extended ascii characters)
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1,                                 //

    U2, U2, U2, U2, U2, U2, U2, U2, U2, U2, //
    U2, U2, U2, U2, U2, U2, U2, U2, U2, U2, // lead byte = 110x xxxx (U+0080 - U+07FF)
    U2, U2, U2, U2, U2, U2, U2, U2, U2, U2, //
    U2, U2,                                 //

    U3, U3, U3, U3, U3, U3, U3, U3, U3, U3, // lead byte = 1110 xxxx (U+0800 - U+FFFF)
    U3, U3, U3, U3, U3, U3,                 //

    U4, U4, U4, U4, U4, U4, U4, U4,         // lead byte = 1111 0xxx (U+10000 - U+1FFFFF)

    U0, U0, U0, U0,                         // Non valid.
    U0, U0, U0, U0,                         // Non valid.
  };
} // namespace

using namespace Dali;
using namespace Dali::Toolkit;
using namespace EmojiStrings;

// This example shows how to create and display Hello World! using a simple TextActor
//
class EmojiExample : public ConnectionTracker
{
public:

  typedef uint32_t SizeType;

  EmojiExample( Application& application )
  : mApplication( application )
  {
    std::cout << "EmojiExample::EmojiExample" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &EmojiExample::Create );
  }

  ~EmojiExample()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {

    mLayout = VerticalLayout::New();
    mLayout.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mLayout.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    Stage stage = Stage::GetCurrent();
    stage.Add( mLayout );
    stage.KeyEventSignal().Connect(this, &EmojiExample::OnKeyEvent);
    unsigned int index = 0;

    while ( EMOJIS[ index ].mUTF8[0] )
    {
      const Emoji& emoji = EMOJIS[index++];

#ifdef SHOW_UNICODE
      uint32_t unicode;
      char unicodeNumber[ 16 ];
      Utf8ToUtf32( ( const uint8_t* )emoji.mUTF8.c_str(), emoji.mUTF8.length(), &unicode );
      sprintf( reinterpret_cast< char* >( &unicodeNumber ), "U+%0x", unicode );
      std::string unicodeString( unicodeNumber);
      const std::string text = unicodeString + emoji.mUTF8 + emoji.mDescription;
#else
      const std::string text = emoji.mUTF8 + emoji.mDescription;
#endif

      TextLabel label = TextLabel::New();
      label.SetParentOrigin( ParentOrigin::TOP_CENTER );
      label.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      label.SetProperty( TextLabel::Property::MULTI_LINE, true );
      label.SetProperty( TextLabel::Property::TEXT, text );
      mLayout.AddLabel( label );
    }

    const Vector2& size = stage.GetSize();
    const float height = mLayout.GetHeightForWidth( size.width );
    mLayout.SetSize( Size( size.width, height ) );
    mLayout.TouchedSignal().Connect( this, &EmojiExample::OnTouchEvent );

  }

  bool OnTouchEvent( Actor actor, const TouchEvent& event )
  {
    if( 1u == event.GetPointCount() )
    {
      const TouchPoint::State state = event.GetPoint(0u).state;

      // Clamp to integer values; this is to reduce flicking due to pixel misalignment
      const float localPoint = static_cast<float>( static_cast<int>( event.GetPoint( 0 ).local.y ) );

      if( TouchPoint::Down == state )
      {
        mLastPoint = localPoint;
        mAnimation = Animation::New( 0.25f );
      }
      else if( TouchPoint::Motion == state )
      {
        if( mAnimation )
        {
          mAnimation.MoveBy( mLayout, Vector3( 0.f, localPoint - mLastPoint, 0.f ), AlphaFunctions::Linear );
          mAnimation.Play();
          mLastPoint = localPoint;
        }
      }
    }

    return true;
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

  uint32_t Utf8ToUtf32( const uint8_t* const utf8, uint32_t length, uint32_t* utf32 )
  {
  uint32_t numberOfCharacters = 0u;

  const uint8_t* begin = utf8;
  const uint8_t* end = utf8 + length;

  for( ; begin < end ; ++numberOfCharacters )
  {
    const uint8_t leadByte = *begin;

    switch( UTF8_LENGTH[leadByte] )
    {
      case U1:
      {
        *utf32++ = leadByte;
        begin++;
        break;
      }

      case U2:
      {
        uint32_t& code = *utf32++;
        code = leadByte & 0x1fu;
        begin++;
        code <<= 6u;
        code |= *begin++ & 0x3fu;
        break;
      }

      case U3:
      {
        uint32_t& code = *utf32++;
        code = leadByte & 0x0fu;
        begin++;
        code <<= 6u;
        code |= *begin++ & 0x3fu;
        code <<= 6u;
        code |= *begin++ & 0x3fu;
        break;
      }

      case U4:
      {
        uint32_t& code = *utf32++;
        code = leadByte & 0x07u;
        begin++;
        code <<= 6u;
        code |= *begin++ & 0x3fu;
        code <<= 6u;
        code |= *begin++ & 0x3fu;
        code <<= 6u;
        code |= *begin++ & 0x3fu;
        break;
      }
    }
  }

  return numberOfCharacters;
}

private:
  Application&  mApplication;
  VerticalLayout mLayout;
  Animation      mAnimation;
  float          mLastPoint;
};

void RunTest( Application& application )
{
  EmojiExample test( application );

  application.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
