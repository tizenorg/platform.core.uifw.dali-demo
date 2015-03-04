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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;

// This example shows how to create and display Hello World! using a simple TextActor
//
class EmoticonController : public ConnectionTracker
{
public:

  typedef uint32_t SizeType;

  EmoticonController( Application& application )
  : mApplication( application )
  {
    std::cout << "EmoticonController::EmoticonController" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &EmoticonController::Create );
  }

  ~EmoticonController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {

    TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
    TextAbstraction::GlyphInfo glyphInfo;
    Stage stage = Stage::GetCurrent();

    if ( !fontClient.IsScalable("TizenColorEmoji", "Regular" ) )
    {
      Dali::Vector< SizeType > sizes;
      fontClient.GetFixedSizes( "TizenColorEmoji", "Regular", sizes );
      if ( sizes.Size() )
      {
        glyphInfo.fontId = fontClient.GetFontId( "TizenColorEmoji", "Regular", sizes[ 0 ] );
      }
    }

    glyphInfo.index = fontClient.GetGlyphIndex( glyphInfo.fontId, 0x1F601 );

    if ( fontClient.GetGlyphMetrics( &glyphInfo, 1 ) )
    {
      BitmapImage bitmap = fontClient.CreateBitmap( glyphInfo.fontId, glyphInfo.index );
      ImageActor actor = ImageActor::New( fontClient.CreateBitmap( glyphInfo.fontId, glyphInfo.index ) );
      actor.SetParentOrigin( ParentOrigin::CENTER );
      Stage::GetCurrent().Add( actor );
    }
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
};

void RunTest( Application& application )
{
  EmoticonController test( application );

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
