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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

// This example shows how to create and display Hello World! using a simple TextActor
//
class TextureAtlasExample : public ConnectionTracker
{
public:

  TextureAtlasExample( Application& application )
  : mApplication( application ),
    mTimerSlot( this ),
    mFlash( false )
  {
    std::cout << "TextureAtlasExample::TextureAtlasExample" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextureAtlasExample::Create );
  }

  ~TextureAtlasExample()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {

    TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
    fontClient.SetDpi( 96, 96 );

    TextLabel label = TextLabel::New();
    label.SetRenderer( Text::AtlasRenderer::New() );
    label.SetProperty( TextLabel::PROPERTY_TEXT, "Text Renderer using Atlas" );
    label.SetParentOrigin( Vector3( 0.8f, 0.52f, 0.5f ) );

    Stage stage = Stage::GetCurrent();
    stage.Add( label );

    mTimer = Dali::Timer::New( 400.0f );
    mTimer.TickSignal().Connect( mTimerSlot, &TextureAtlasExample::OnTick );
    mTimer.Start();
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  bool OnTick()
  {
    Stage stage = Stage::GetCurrent();
    if ( !mFlash )
    {
      mLabel = TextLabel::New();
      mLabel.SetRenderer( Text::AtlasRenderer::New() );
      mLabel.SetProperty( TextLabel::PROPERTY_TEXT, "***Flashing Text:123***" );
      mLabel.SetParentOrigin( Vector3( 0.82f, 1.0f, 0.5f ) );
      stage.Add( mLabel );
    }
    else
    {
      stage.Remove( mLabel );
    }
    mFlash ^= 1;
    return true;
  }

private:
  Application&  mApplication;
  TextLabel mLabel;
  SlotDelegate< TextureAtlasExample > mTimerSlot;
  Dali::Timer mTimer;
  bool mFlash;
};

void RunTest( Application& application )
{
  TextureAtlasExample test( application );

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
