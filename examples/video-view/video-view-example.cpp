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

#include "shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;

namespace
{
  //ToDo. This path will be changed after a video file for public sample app is supported.
  const int BACKWARD_POS( 5000 );
  const int INIT_WIDTH( 800 );
  const int INIT_HEIGHT( 400 );

  const char* const PLAY_FILE = DEMO_VIDEO_DIR "big_buck_bunny.mp4";
  const char* const PLAY_IMAGE = DEMO_IMAGE_DIR "icon-play.png";
  const char* const PAUSE_IMAGE = DEMO_IMAGE_DIR "application-icon-146.png";
  const char* const STOP_IMAGE = DEMO_IMAGE_DIR "icon-stop.png";
  const char* const RESET_IMAGE = DEMO_IMAGE_DIR "icon-reset.png";
  const char* const BACKWARD_IMAGE = DEMO_IMAGE_DIR "music-libray-arrow.png";
  const char* const LOOPING_ON_IMAGE = DEMO_IMAGE_DIR "spot_button_on.png";
  const char* const LOOPING_OFF_IMAGE = DEMO_IMAGE_DIR "spot_button_off.png";
  const char* const MUTE_ON_IMAGE = DEMO_IMAGE_DIR "application-icon-23.png";
  const char* const MUTE_OFF_IMAGE = DEMO_IMAGE_DIR "application-icon-116.png";

}  // namespace

class VideoViewController: public ConnectionTracker
{
 public:

  VideoViewController( Application& application )
    : mApplication( application ),
      mIsPlay( false ),
      mIsStop( false ),
      mIsMuted( false ),
      mIsLooping( true )
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
    // The Init signal is received once (only) during the Application lifetime

    mVideoView = Toolkit::VideoView::New();
    mVideoView.SetParentOrigin( ParentOrigin::CENTER );
    mVideoView.SetAnchorPoint( AnchorPoint::CENTER );
    mVideoView.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL );
    mVideoView.SetSize( INIT_WIDTH, INIT_HEIGHT );
    mVideoView.SetLooping( true );
    mVideoView.SetUrl( PLAY_FILE );
    mVideoView.Play();

    Stage::GetCurrent().Add( mVideoView );

    mMenu = Layer::New();
    mMenu.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    mMenu.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mMenu.SetSize( 800, 120 );
    mVideoView.Add( mMenu );

    mPlayButton = PushButton::New();
    mPlayButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mPlayButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mPlayButton.SetName( "Play" );
    mPlayButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL );
    mPlayButton.SetSize( 100, 100 );
    mPlayButton.SetPosition( 10, 10 );
    mPlayButton.StateChangedSignal().Connect( this, &VideoViewController::OnButtonSelected );

    mStopButton = PushButton::New();
    mStopButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mStopButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mStopButton.SetName( "Stop" );
    mStopButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL );
    mStopButton.SetSize( 100, 100 );
    mStopButton.SetPosition( 120, 10 );
    mStopButton.StateChangedSignal().Connect( this, &VideoViewController::OnButtonSelected );

    mMuteButton = PushButton::New();
    mMuteButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mMuteButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mMuteButton.SetName( "Mute" );
    mMuteButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL );
    mMuteButton.SetSize( 100, 100 );
    mMuteButton.SetPosition( 230, 10 );
    mMuteButton.StateChangedSignal().Connect( this, &VideoViewController::OnButtonSelected );

    mLoopingButton = PushButton::New();
    mLoopingButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mLoopingButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mLoopingButton.SetName( "Looping" );
    mLoopingButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL );
    mLoopingButton.SetSize( 100, 100 );
    mLoopingButton.SetPosition( 340, 10 );
    mLoopingButton.StateChangedSignal().Connect( this, &VideoViewController::OnButtonSelected );

    mBackwardButton = PushButton::New();
    mBackwardButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mBackwardButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mBackwardButton.SetName( "Backward" );
    mBackwardButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL );
    mBackwardButton.SetSize( 100, 100 );
    mBackwardButton.SetPosition( 450, 10 );
    mBackwardButton.StateChangedSignal().Connect( this, &VideoViewController::OnButtonSelected );

    mMenu.Add( mPlayButton );
    mMenu.Add( mStopButton );
    mMenu.Add( mMuteButton );
    mMenu.Add( mLoopingButton );
    mMenu.Add( mBackwardButton );

    mPlayImage = ResourceImage::New( PLAY_IMAGE );
    mPauseImage = ResourceImage::New( PAUSE_IMAGE );
    mStopImage = ResourceImage::New( STOP_IMAGE );
    mResetImage = ResourceImage::New( RESET_IMAGE );
    mBackwardImage = ResourceImage::New( BACKWARD_IMAGE );
    mLoopingOnImage = ResourceImage::New( LOOPING_ON_IMAGE );
    mLoopingOffImage = ResourceImage::New( LOOPING_OFF_IMAGE );
    mMuteOnImage = ResourceImage::New( MUTE_ON_IMAGE );
    mMuteOffImage = ResourceImage::New( MUTE_OFF_IMAGE );

    mPlayButton.SetButtonImage( mPlayImage );
    mStopButton.SetButtonImage( mStopImage );
    mBackwardButton.SetButtonImage( mBackwardImage );
    mMuteButton.SetButtonImage( mMuteOnImage );
    mLoopingButton.SetButtonImage( mLoopingOffImage );

    Stage::GetCurrent().KeyEventSignal().Connect(this, &VideoViewController::OnKeyEvent);
  }

  bool OnButtonSelected( Button button )
  {
    PushButton pushButton = Button::DownCast( button );

    if( !pushButton && !button.IsSelected() )
    {
      return true;
    }

    if( mPlayButton.GetId() == pushButton.GetId())
    {
      if( mIsPlay )
      {
        mIsPlay = false;
        mVideoView.Pause();
        button.SetButtonImage( mPlayImage );
      }
      else
      {
        mIsPlay = true;
        mVideoView.Play();
        button.SetButtonImage( mPauseImage );
      }
    }
    else if( mStopButton.GetId() == pushButton.GetId())
    {
      if( mIsStop )
      {
        mIsStop = false;
        mIsPlay = true;
        mVideoView.SetProperty( VideoView::VIDEO, PLAY_FILE );
        mVideoView.Play();
        mPlayButton.SetButtonImage( mPauseImage );
        button.SetButtonImage( mStopImage );
      }
      else
      {
        mIsStop = true;
        mVideoView.Stop();
        button.SetButtonImage( mResetImage );
      }
    }
    else if( mBackwardButton.GetId() == pushButton.GetId())
    {
      mVideoView.Backward( BACKWARD_POS );
    }
    else if( mMuteButton.GetId() == pushButton.GetId())
    {
      if( mIsMuted )
      {
        mIsMuted = false;
        mVideoView.SetProperty( VideoView::MUTE, false );
        button.SetButtonImage( mMuteOnImage );
      }
      else
      {
        mIsMuted = true;
        mVideoView.SetProperty( VideoView::MUTE, true );
        button.SetButtonImage( mMuteOffImage );
      }
    }
    else if( mLoopingButton.GetId() == pushButton.GetId())
    {
      if( mIsLooping )
      {
        mIsLooping = false;
        mVideoView.SetProperty( VideoView::LOOPING, false );
        button.SetButtonImage( mLoopingOnImage );
      }
      else
      {
        mIsLooping = true;
        mVideoView.SetProperty( VideoView::LOOPING, true );
        button.SetButtonImage( mLoopingOffImage );
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

  Image mPlayImage;
  Image mPauseImage;
  Image mStopImage;
  Image mResetImage;
  Image mBackwardImage;
  Image mLoopingOnImage;
  Image mLoopingOffImage;
  Image mMuteOnImage;
  Image mMuteOffImage;

  bool mIsPlay;
  bool mIsStop;
  bool mIsMuted;
  bool mIsLooping;

  PushButton mPlayButton; //play & pause
  PushButton mStopButton;
  PushButton mBackwardButton;
  PushButton mMuteButton;
  PushButton mLoopingButton;
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
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );

  RunTest( application );

  return 0;
}
