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

using namespace Dali;
using namespace Toolkit;

namespace
{
  const int BACKWARD_POS( 5000 );
  const int INIT_WIDTH( 600 );
  const int INIT_HEIGHT( 400 );

  const char* const PLAY_FILE = DEMO_VIDEO_DIR "big_buck_bunny.mp4";
  const char* const PLAY_IMAGE = DEMO_IMAGE_DIR "icon-play.png";
  const char* const PAUSE_IMAGE = DEMO_IMAGE_DIR "application-icon-146.png";
  const char* const STOP_IMAGE = DEMO_IMAGE_DIR "icon-stop.png";
  const char* const RESET_IMAGE = DEMO_IMAGE_DIR "icon-reset.png";
  const char* const BACKWARD_IMAGE = DEMO_IMAGE_DIR "icon-cluster-wobble.png";
  const char* const MUTE_ON_IMAGE = DEMO_IMAGE_DIR "application-icon-116-g.png";
  const char* const MUTE_OFF_IMAGE = DEMO_IMAGE_DIR "application-icon-116.png";
  const char* const FULL_ON_IMAGE = DEMO_IMAGE_DIR "application-icon-116-g.png";
  const char* const FULL_OFF_IMAGE = DEMO_IMAGE_DIR "application-icon-116-g.png";

const char* DEFAULT_FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uColor;\n
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
  }\n
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  precision mediump float;
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uColor;\n
  \n
  void main()\n
  {\n
    vec2 st = vTexCoord.st;\n
    vec3 irgb = texture2D( sTexture, st ).rgb;\n
    vec3 negative = vec3( 1., 1., 1. ) - irgb;\n
    gl_FragColor = vec4( mix( irgb, negative, 1.0), 1. ) * uColor;\n
  }\n
);

}  // namespace

class VideoViewController: public ConnectionTracker
{
 public:

  VideoViewController( Application& application )
    : mApplication( application ),
      mIsPlay( false ),
      mIsStop( false ),
      mIsMuted( false ),
      mIsFullScreen( false ),
      mSetCustomShader( false ),
      mScale( 1.f )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &VideoViewController::Create );
  }

  ~VideoViewController()
  {
    mVideoView.Stop();
  }

  void Create( Application& application )
  {
    mStageSize = Stage::GetCurrent().GetSize();

    mVideoView = Toolkit::VideoView::New();
    Stage::GetCurrent().Add( mVideoView );
    mVideoView.SetParentOrigin( ParentOrigin::CENTER );
    mVideoView.SetAnchorPoint( AnchorPoint::CENTER );
    mVideoView.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    mVideoView.SetSize( INIT_WIDTH, INIT_HEIGHT );
    mVideoView.SetProperty( VideoView::Property::LOOPING, true );
    mVideoView.SetProperty( VideoView::Property::MUTED, false );
    mVideoView.SetProperty( VideoView::Property::VIDEO, PLAY_FILE );

    mMenu = Layer::New();
    mMenu.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    mMenu.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    mMenu.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mMenu.SetSize( INIT_WIDTH, 120 );
    mVideoView.Add( mMenu );

    mPlayPauseButton = PushButton::New();
    mPlayPauseButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mPlayPauseButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mPlayPauseButton.SetName( "PlayPause" );
    mPlayPauseButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mPlayPauseButton.SetSize( 100, 100 );
    mPlayPauseButton.SetPosition( 50, 10 );
    mPlayPauseButton.ClickedSignal().Connect( this, &VideoViewController::OnButtonClicked );

    mStopResetButton = PushButton::New();
    mStopResetButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mStopResetButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mStopResetButton.SetName( "StopReset" );
    mStopResetButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mStopResetButton.SetSize( 100, 100 );
    mStopResetButton.SetPosition( 160, 10 );
    mStopResetButton.ClickedSignal().Connect( this, &VideoViewController::OnButtonClicked );

    mMuteButton = PushButton::New();
    mMuteButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mMuteButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mMuteButton.SetName( "Mute" );
    mMuteButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mMuteButton.SetSize( 100, 100 );
    mMuteButton.SetPosition( 270, 10 );
    mMuteButton.ClickedSignal().Connect( this, &VideoViewController::OnButtonClicked );

    mBackwardButton = PushButton::New();
    mBackwardButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mBackwardButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mBackwardButton.SetName( "Backward" );
    mBackwardButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mBackwardButton.SetSize( 100, 100 );
    mBackwardButton.SetPosition( 380, 10 );
    mBackwardButton.ClickedSignal().Connect( this, &VideoViewController::OnButtonClicked );

    mMenu.Add( mPlayPauseButton );
    mMenu.Add( mStopResetButton );
    mMenu.Add( mBackwardButton );
    mMenu.Add( mMuteButton );

    mPlayPauseButton.SetUnselectedImage( PLAY_IMAGE );
    mPlayPauseButton.SetSelectedImage( PLAY_IMAGE );
    mStopResetButton.SetUnselectedImage( STOP_IMAGE );
    mStopResetButton.SetSelectedImage( STOP_IMAGE );
    mBackwardButton.SetUnselectedImage( BACKWARD_IMAGE );
    mBackwardButton.SetSelectedImage( BACKWARD_IMAGE );
    mMuteButton.SetUnselectedImage( MUTE_ON_IMAGE );
    mMuteButton.SetSelectedImage( MUTE_ON_IMAGE );

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach( mVideoView );
    mPanGestureDetector.DetectedSignal().Connect( this, &VideoViewController::OnPan );

    mPinchGestureDetector = PinchGestureDetector::New();
    mPinchGestureDetector.Attach( mVideoView );
    mPinchGestureDetector.DetectedSignal().Connect( this, &VideoViewController::OnPinch );

    mTapGestureDetector = TapGestureDetector::New();
    mTapGestureDetector.Attach( mVideoView );
    mTapGestureDetector.DetectedSignal().Connect( this, &VideoViewController::OnTap );

    mRotationAnimation = Animation::New(2.f);
    mRotationAnimation.AnimateBy( Property(mVideoView, Actor::Property::ORIENTATION), Quaternion(Degree(0.f), Degree(360.f), Degree(0.f)) );
    mRotationAnimation.SetLooping(false);

    Stage::GetCurrent().KeyEventSignal().Connect(this, &VideoViewController::OnKeyEvent);

    Property::Map customShader;
    customShader.Insert( "fragmentShader", FRAGMENT_SHADER );
    mCustomShader.Insert( "rendererType", "image" );
    mCustomShader.Insert( "shader", customShader );

    Property::Map defaultShader;
    customShader.Insert( "fragmentShader", DEFAULT_FRAGMENT_SHADER );
    mDefaultShader.Insert( "rendererType", "image" );
    mDefaultShader.Insert( "shader", defaultShader );
  }

  bool OnButtonClicked( Button button )
  {

    if( mPlayPauseButton.GetId() == button.GetId())
    {
       if( mIsPlay )
      {
        mPlayPauseButton.SetUnselectedImage( PLAY_IMAGE );
        mPlayPauseButton.SetSelectedImage( PLAY_IMAGE );
        mIsPlay = false;
        mVideoView.Pause();
      }
      else
      {
        if( mIsStop )
        {
          return false;
        }

        mPlayPauseButton.SetUnselectedImage( PAUSE_IMAGE );
        mPlayPauseButton.SetSelectedImage( PAUSE_IMAGE );
        mIsPlay = true;
        mVideoView.Play();
      }
    }
    else if( mStopResetButton.GetId() == button.GetId())
    {
      if( mIsStop )
      {
        mPlayPauseButton.SetUnselectedImage( PAUSE_IMAGE );
        mPlayPauseButton.SetSelectedImage( PAUSE_IMAGE );
        mStopResetButton.SetUnselectedImage( STOP_IMAGE );
        mStopResetButton.SetSelectedImage( STOP_IMAGE );
        mIsStop = false;
        mIsPlay = true;
        mVideoView.SetProperty( VideoView::Property::VIDEO, PLAY_FILE );
        mVideoView.Play();
      }
      else
      {
        mPlayPauseButton.SetUnselectedImage( PLAY_IMAGE );
        mPlayPauseButton.SetSelectedImage( PLAY_IMAGE );
        mStopResetButton.SetUnselectedImage( RESET_IMAGE );
        mStopResetButton.SetSelectedImage( RESET_IMAGE );
        mIsStop = true;
        mVideoView.Stop();
      }
    }
    else if( mBackwardButton.GetId() == button.GetId())
    {
      mVideoView.Backward( BACKWARD_POS );
    }
    else if( mMuteButton.GetId() == button.GetId())
    {
      if( mIsMuted )
      {
        mMuteButton.SetUnselectedImage( MUTE_ON_IMAGE );
        mMuteButton.SetSelectedImage( MUTE_ON_IMAGE );
        mIsMuted = false;
        mVideoView.SetProperty( VideoView::Property::MUTED, false );
      }
      else
      {
        mMuteButton.SetUnselectedImage( MUTE_OFF_IMAGE );
        mMuteButton.SetSelectedImage( MUTE_OFF_IMAGE );
        mIsMuted = true;
        mVideoView.SetProperty( VideoView::Property::MUTED, true );
      }
    }

    return true;
  }

  void OnPan( Actor actor, const PanGesture& gesture )
  {
    if( !mIsFullScreen && gesture.state == Gesture::Continuing )
    {
      mVideoView.TranslateBy( Vector3( gesture.displacement ) );
    }
  }

  void OnPinch( Actor actor, const PinchGesture& gesture )
  {
    if( gesture.state == Gesture::Started )
    {
      mPinchStartScale = mScale;
    }

    if( gesture.state == Gesture::Finished )
    {
      mScale = mPinchStartScale * gesture.scale;
      if( mScale > 1.f )
      {
        mVideoView.SetSize( mStageSize.x, mStageSize.y );
        mMenu.SetSize( mStageSize.x, 120 );
        mIsFullScreen = true;
      }
      else
      {
        mVideoView.SetSize( INIT_WIDTH, INIT_HEIGHT );
        mMenu.SetSize( INIT_WIDTH, 120 );
        mIsFullScreen = false;
      }
    }
  }

  void OnTap( Actor actor, const TapGesture& tapGesture )
  {
    if( !mIsFullScreen )
    {
      mRotationAnimation.Play();

      if( mSetCustomShader )
      {
        mSetCustomShader = false;
        mVideoView.SetProperty( VideoView::Property::VIDEO, mDefaultShader );
      }
      else
      {
        mSetCustomShader = true;
        mVideoView.SetProperty( VideoView::Property::VIDEO, mCustomShader );
      }
    }
  }

private:

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

private:
  Application&  mApplication;
  VideoView mVideoView;
  Layer mMenu;
  Vector2 mStageSize;

  bool mIsPlay;
  bool mIsStop;
  bool mIsMuted;
  bool mIsFullScreen;
  bool mSetCustomShader;

  PushButton mPlayPauseButton;
  PushButton mStopResetButton;
  PushButton mBackwardButton;
  PushButton mMuteButton;

  PanGestureDetector mPanGestureDetector;
  PinchGestureDetector mPinchGestureDetector;
  TapGestureDetector mTapGestureDetector;
  float mScale;
  float mPinchStartScale;

  Animation mRotationAnimation;
  Property::Map mCustomShader;
  Property::Map mDefaultShader;

//  player_h mPlayer;
};

void RunTest( Application& application )
{
  VideoViewController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int DALI_EXPORT_API main( int argc, char **argv )
{
//  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH, Application::TRANSPARENT );
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );

  RunTest( application );

  return 0;
}
