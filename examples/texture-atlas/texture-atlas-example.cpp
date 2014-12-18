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

namespace
{

//const char* BRICK_IMAGE = DALI_IMAGE_DIR "blocks-brick-1.png";

}  // namespace

using namespace Dali;
using namespace Dali::Toolkit;

// This example shows how to create and display Hello World! using a simple TextActor
//
class TextureAtlasExample : public ConnectionTracker
{
public:

  TextureAtlasExample( Application& application )
  : mApplication( application )
  {

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

    // Get a handle to the stage
    mStage = Stage::GetCurrent();

    // Respond to a click anywhere on the stage
    mStage.GetRootLayer().TouchedSignal().Connect( this, &TextureAtlasExample::OnTouch );

    mAtlas = TextureAtlas::New( 1024u, 1024u );
    //mAtlas.ShowBorders( true );
    mAtlas.SetSpacing( 4u );

    ImageActor actor = ImageActor::New( mAtlas );
    actor.SetParentOrigin( Vector3( 0.5f, 0.3f, 0.5f ) );
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetSize(400.0f, 400.0f );
    mStage.Add( actor );

    mAtlas.Add( 256u, 256u );
    mAtlas.Add( 320u, 128u );
    mAtlas.Add( 64u, 512u );
    mAtlas.Add( 128u, 64u );

    mAtlas.Add( 512u, 64u );
    mAtlas.Add( 64u, 64u );
    mAtlas.Add( 192u, 256u );

    mAtlas.Add( 128u, 128u );
    mAtlas.Add( 128u, 128u );
    mAtlas.Add( 256u, 192u );

    mAtlas.Add( 64u, 64u );
    mAtlas.Add( 64u, 64u );
    mAtlas.Add( 64u, 64u );
    mAtlas.Add( 64u, 64u );

    mAtlas.Add( 64u, 64u );
    mAtlas.Add( 64u, 64u );
    mAtlas.Add( 64u, 64u );
    mAtlas.Add( 64u, 64u );

    mAtlas.Add( 64u, 64u );
    mAtlas.Add( 64u, 64u );
    mAtlas.Add( 64u, 64u );
    mAtlas.Add( 64u, 64u );


    mAtlas.Add( 128u, 128u );
    mAtlas.Add( 256u, 64u );
    mAtlas.Add( 512u, 64u );
    mAtlas.Add( 128u, 128u );
    mAtlas.Add( 128u, 128u );
    mAtlas.Add( 128u, 128u );
    mAtlas.Add( 128u, 128u );
    mAtlas.Add( 256u, 256u );
    mAtlas.Add( 256u, 128u );
    mAtlas.Add( 8u, 960u );
    mAtlas.Add( 8u, 768u );

    for ( int i=0; i<1024; i++)
    {
      mAtlas.Add( 32u, 32u );
    }

    /*

    mAtlas.Add( 128u, 16u );
    mAtlas.Add( 640u, 48u );

    for ( int i=0; i<8; i++)
    {
      mAtlas.Add( 32u, 32u );
    }

  */

    TextActor textActor = TextActor::New( "Select Texture to Add / Remove from Atlas" );
    textActor.SetSize( 384.0f, 32.0f );
    textActor.SetParentOrigin( Vector3( 0.5f, 0.57f, 0.5f ) );
    mStage.Add( textActor );
    TextActor textActor2 = TextActor::New( "Current Atlas Size: 1024x1024" );
    textActor2.SetSize( 192.0f, 16.0f );
    textActor2.SetParentOrigin( Vector3( 0.5f, 0.015f, 0.5f ) );
    textActor2.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mStage.Add( textActor2 );


  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
  Stage mStage;
  TextureAtlas mAtlas;
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
