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
using namespace Toolkit;

namespace
{
  const int BACKWARD_POS( 5000 );
  const int INIT_WIDTH( 800 );
  const int INIT_HEIGHT( 400 );

  const char* const PLAY_FILE = DEMO_VIDEO_DIR "big_buck_bunny.mp4";
  const char* const PLAY_IMAGE = DEMO_IMAGE_DIR "icon-play.png";
  const char* const PAUSE_IMAGE = DEMO_IMAGE_DIR "application-icon-146.png";
  const char* const STOP_IMAGE = DEMO_IMAGE_DIR "icon-stop.png";
  const char* const RESET_IMAGE = DEMO_IMAGE_DIR "icon-reset.png";
  const char* const BACKWARD_IMAGE = DEMO_IMAGE_DIR "icon-cluster-wobble.png";
  const char* const MUTE_ON_IMAGE = DEMO_IMAGE_DIR "icon-effects-off.png";
  const char* const MUTE_OFF_IMAGE = DEMO_IMAGE_DIR "application-icon-116.png";

}  // namespace

class VideoViewController: public ConnectionTracker
{
 public:

  VideoViewController( Application& application )
    : mApplication( application ),
      mIsPlay( false ),
      mIsStop( false ),
      mIsMuted( false )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &VideoViewController::Create );
  }

  ~VideoViewController()
  {
    mVideoView.Stop();
  }

/*
  void VideoPlay()
  {
    int width, height;
    ecore_wl_screen_size_get( &width, &height );
//    player_set_display_mode( mPlayer, PLAYER_DISPLAY_MODE_FULL_SCREEN );
    player_set_ecore_wl_display( mPlayer, PLAYER_DISPLAY_TYPE_OVERLAY, ewindow, 0, 0, width, height );
//    player_set_display_rotation( mPlayer, PLAYER_DISPLAY_ROTATION_90 );
    player_set_uri( mPlayer, PLAY_FILE );
    player_prepare( mPlayer );
    player_start( mPlayer );
  }
*/
  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime

    mVideoView = Toolkit::VideoView::New();
    mVideoView.SetParentOrigin( ParentOrigin::CENTER );
    mVideoView.SetAnchorPoint( AnchorPoint::CENTER );
    mVideoView.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    mVideoView.SetSize( INIT_WIDTH, INIT_HEIGHT );
    mVideoView.SetProperty( VideoView::Property::LOOPING, true );
    mVideoView.SetProperty( VideoView::Property::MUTED, false );
    mVideoView.SetProperty( VideoView::Property::VIDEO, PLAY_FILE );

    Stage::GetCurrent().Add( mVideoView );

    mMenu = Layer::New();
    mMenu.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    mMenu.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    mMenu.SetSize( 800, 120 );
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

    Stage::GetCurrent().KeyEventSignal().Connect(this, &VideoViewController::OnKeyEvent);
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
        mPlayPauseButton.SetUnselectedImage( PAUSE_IMAGE );
        mPlayPauseButton.SetSelectedImage( PAUSE_IMAGE );
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

  bool mIsPlay;
  bool mIsStop;
  bool mIsMuted;

  PushButton mPlayPauseButton;
  PushButton mStopResetButton;
  PushButton mBackwardButton;
  PushButton mMuteButton;

  PushButton mScreenModeButton;
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
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH, Application::TRANSPARENT );
//  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );

  RunTest( application );

  return 0;
}
