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

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
  const char* DESKTOP_IMAGE( DEMO_IMAGE_DIR "woodEffect.jpg" );
  const Vector2 DESKTOP_SIZE( Vector2( 1440, 1600 ) );
  const Vector2 BOX_SIZE( Vector2(330.0f, 80.0f ) );
  const Vector2 SCROLLING_BOX_SIZE( Vector2(330.0f, 40.0f ) );
  const float MAX_OFFSCREEN_RENDERING_SIZE = 2048.f;
  const float SCREEN_BORDER = 5.0f; // Border around screen that Popups and handles will not exceed

  enum Labels
  {
    SMALL = 1u << 0,
    RTL = 1u << 1,
    LARGE = 1u << 2,
    RTL_LONG = 1u << 4,
    NONE = 1u << 6,
  };
}
/**
 * @brief The main class of the demo.
 */
class TextMessageFieldExample : public ConnectionTracker
{
public:

  TextMessageFieldExample( Application& application )
  : mApplication( application ),
    mTargetActorPosition(),
    mTargetActorSize()
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextMessageFieldExample::Create );
  }

  ~TextMessageFieldExample()
  {
    // Nothing to do here.
  }


  void CreateBox( const std::string& name, Actor& box, Actor parent, const Vector2& size )
  {
    box.SetName(name);
    box.SetAnchorPoint( AnchorPoint::CENTER );
    box.SetParentOrigin( ParentOrigin::CENTER );
    box.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    box.SetSize( size );
    parent.Add( box );

    Dali::Property::Map border;
    border.Insert( "rendererType",  "border" );
    border.Insert( "borderColor",  Color::WHITE );
    border.Insert( "borderSize",  1.f );
    box.SetProperty( Control::Property::BACKGROUND, border );
  }

  void CreateLabel( Actor& label, const std::string text, Actor parent, bool scrollOnStart, PushButton button )
  {
    label = TextLabel::New( text );
    label.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    label.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    label.SetPadding( Padding( 1.0f, 1.0f, 1.0f, 1.0f ) );
    label.SetAnchorPoint( AnchorPoint::CENTER );
    label.SetParentOrigin( ParentOrigin::CENTER );
    parent.Add( label );

    if ( scrollOnStart )
    {
      label.SetProperty(TextLabel::Property::ENABLE_AUTO_SCROLL, true);
    }

    button.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    button.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    button.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    parent.Add(button);
  }


  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();
    mStageSize = stage.GetSize();

    stage.KeyEventSignal().Connect(this, &TextMessageFieldExample::OnKeyEvent);

    // Create Root actor
    Actor rootActor = Actor::New();
    rootActor.SetName("rootActor");
    rootActor.SetResizePolicy( ResizePolicy::FIXED,  Dimension::ALL_DIMENSIONS );
    rootActor.SetSize( mStageSize );
    rootActor.SetAnchorPoint( AnchorPoint::TOP_LEFT );

    stage.Add( rootActor );

    const Size mTargetActorSize( mStageSize.width, DESKTOP_SIZE.height );

    // Create Desktop
    ImageView desktop = ImageView::New( DESKTOP_IMAGE );
    desktop.SetName("desktopActor");
    desktop.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    desktop.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    desktop.SetSize( mTargetActorSize );

    rootActor.Add( desktop ); // Add desktop (content) to offscreen actor

    // Create Boxes
    Control boxA = Control::New();
    Control boxB = Control::New();
    Control boxC = Control::New();
    Control boxD = Control::New();
    Control boxE = Control::New();

    CreateBox( "boxA", boxA, desktop, BOX_SIZE );
    boxA.SetPosition( 0.0f, -500.0f, 1.0f );

    // Create TextField
    TextField field = TextField::New();
    field.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    field.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    field.SetPadding( Padding( 1.0f, 1.0f, 1.0f, 1.0f ) );
    field.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    field.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "Enter Folder Name" );
    field.SetProperty( TextField::Property::DECORATION_BOUNDING_BOX, Rect<int>( SCREEN_BORDER, SCREEN_BORDER, mStageSize.width - SCREEN_BORDER*2, mStageSize.height - SCREEN_BORDER*2 ) );
    boxA.Add( field );

    CreateBox( "boxB", boxB, desktop, SCROLLING_BOX_SIZE );
    boxB.SetPosition( 0.0f, -400.0f, 1.0f );
    Toolkit::PushButton scrollLargeButton = Toolkit::PushButton::New();
    scrollLargeButton.ClickedSignal().Connect( this, &TextMessageFieldExample::OnButtonClickedLarge );
    CreateLabel( mLargeLabel, "A Quick Brown Fox Jumps Over The Lazy Dog", boxB, false ,scrollLargeButton );


    CreateBox( "boxC", boxC, desktop, SCROLLING_BOX_SIZE );
    boxC.SetPosition( 0.0f, -300.0f, 1.0f );
    Toolkit::PushButton scrollSmallButton = Toolkit::PushButton::New();
    scrollSmallButton.ClickedSignal().Connect( this, &TextMessageFieldExample::OnButtonClickedSmall );
    CreateLabel( mSmallLabel, "A Quick Brown Fox", boxC , true, scrollSmallButton );

    CreateBox( "boxD", boxD, desktop, SCROLLING_BOX_SIZE );
    boxD.SetPosition( 0.0f, -200.0f, 1.0f );
    Toolkit::PushButton scrollRtlButton = Toolkit::PushButton::New();
    scrollRtlButton.ClickedSignal().Connect( this, &TextMessageFieldExample::OnButtonClickedRtl );
    CreateLabel( mRtlLabel, "مرحبا بالعالم", boxD , true, scrollRtlButton );

    CreateBox( "boxE", boxE, desktop, SCROLLING_BOX_SIZE );
    boxE.SetPosition( 0.0f, -100.0f, 1.0f );
    Toolkit::PushButton scrollRtlLongButton = Toolkit::PushButton::New();
    scrollRtlLongButton.ClickedSignal().Connect( this, &TextMessageFieldExample::OnButtonClickedRtlLong );
    CreateLabel( mRtlLongLabel, " مرحبا بالعالم مرحبا بالعالم مرحبا بالعالم", boxE , false, scrollRtlLongButton );


    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.DetectedSignal().Connect(this, &TextMessageFieldExample::OnPanGesture );
    mPanGestureDetector.Attach( desktop );
  }

  void EnableScrolling( Labels labels )
    {
      Actor label;
      switch( labels )
      {
        case LARGE:
        {
          label = mLargeLabel;
          break;
        }
        case RTL:
        {
          label = mRtlLabel;
          break;
        }
        case SMALL:
        {
          label = mSmallLabel;
          break;
        }
        case RTL_LONG:
        {
          label = mRtlLongLabel;
          break;
        }
        case NONE:
        {
          return;
        }
      }

      if ( labels != NONE )
      {
        Property::Value value = label.GetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL);
        if (value.Get< bool >())
        {
          label.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, false  );
        }
        else
        {
          label.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true  );
        }
      }
    }

  bool OnButtonClickedSmall( Toolkit::Button button )
  {
    EnableScrolling( SMALL );
    return true;
  }

  bool OnButtonClickedLarge( Toolkit::Button button )
  {
    EnableScrolling( LARGE );
    return true;
  }

  bool OnButtonClickedRtl( Toolkit::Button button )
  {
    EnableScrolling( RTL );
    return true;
  }

  bool OnButtonClickedRtlLong( Toolkit::Button button )
  {
    EnableScrolling( RTL_LONG );
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

  TextLabel mLargeLabel;
  TextLabel mSmallLabel;
  TextLabel mRtlLabel;
  TextLabel mRtlLongLabel;
};

void RunTest( Application& application )
{
  TextMessageFieldExample test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );

  RunTest( application );

  return 0;
}
