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

/**
 * @file message-field-example.cpp
 * @brief Basic usage of TextField control
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/text-abstraction/text-abstraction.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
  const char* DESKTOP_IMAGE( DALI_IMAGE_DIR "woodEffect.jpg" );
  const Vector2 DESKTOP_SIZE( Vector2( 1440, 1600 ) );
  const Vector2 PHOTOBOX_SIZE( Vector2(330.0f, 80.0f ) );
  const float MAX_OFFSCREEN_RENDERING_SIZE = 2048.f;
}
/**
 * @brief The main class of the demo.
 */
class MessageFieldExample : public ConnectionTracker
{
public:

  MessageFieldExample( Application& application )
  : mApplication( application ),
    mTargetActorPosition(),
    mTargetActorSize()
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &MessageFieldExample::Create );
  }

  ~MessageFieldExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();
    mStageSize = stage.GetSize();

    stage.KeyEventSignal().Connect(this, &MessageFieldExample::OnKeyEvent);

    // Create Root actor
    Actor rootActor = Actor::New();
    rootActor.SetName("rootActor");
    rootActor.SetSize( mStageSize );
    rootActor.SetAnchorPoint( AnchorPoint::TOP_LEFT );

    stage.Add( rootActor );

    const Size mTargetActorSize( mStageSize.width, DESKTOP_SIZE.height );

    // Create Desktop
    ResourceImage backgroundImage = ResourceImage::New( DESKTOP_IMAGE );
    ImageActor desktop = ImageActor::New( backgroundImage );
    desktop.SetName("desktopActor");
    desktop.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    desktop.SetSize( mTargetActorSize );

    rootActor.Add( desktop ); // Add desktop (content) to offscreen actor

    // Create Photo Box A
    ImageActor photoBoxA = CreateSolidColorActor( Vector4(0,0,0,0), true, Color::WHITE, 1 );
    photoBoxA.SetName("photoBoxA");
    photoBoxA.SetAnchorPoint( AnchorPoint::CENTER );
    photoBoxA.SetParentOrigin( ParentOrigin::CENTER );
    photoBoxA.SetSize( PHOTOBOX_SIZE );
    photoBoxA.SetPosition( 0.0f, -500.0f, 1.0f );
    desktop.Add( photoBoxA );

    // Create TextField
    TextField field = TextField::New();
    field.SetSize ( PHOTOBOX_SIZE );
    field.SetZ( 1.0f );
    field.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    field.SetProperty( TextField::Property::TEXT, "Enter Title name"  );
    photoBoxA.Add( field );

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.DetectedSignal().Connect(this, &MessageFieldExample::OnPanGesture );
    mPanGestureDetector.Attach( desktop );
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

  void OnPanGesture( Actor actor, const PanGesture& gesture )
  {
    if( gesture.state == Gesture::Continuing )
    {
      Vector2 position = Vector2( gesture.displacement );
      mTargetActorPosition.y = mTargetActorPosition.y + position.y;
      mTargetActorPosition.y = std::min( mTargetActorPosition.y, -mTargetActorSize.height );
      mTargetActorPosition.y = std::max( mTargetActorPosition.y, ( mTargetActorSize.height - mStageSize.height*0.25f ) );
      actor.SetPosition( 0.0f, mTargetActorPosition.y );
    }
  }

private:

  Application& mApplication;
  PanGestureDetector mPanGestureDetector;

  Vector2 mTargetActorPosition;
  Vector2 mTargetActorSize;
  Vector2 mStageSize;
};

void RunTest( Application& application )
{
  MessageFieldExample test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
