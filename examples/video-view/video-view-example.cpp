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
  //ToDo. This path will be changed after a video file for public sample app is supported.
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
      mIsPlay( true ),
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

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime

    mVideoView = Toolkit::VideoView::New( );
    mVideoView.SetParentOrigin( ParentOrigin::CENTER );
    mVideoView.SetAnchorPoint( AnchorPoint::CENTER );
    mVideoView.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    mVideoView.SetSize( INIT_WIDTH, INIT_HEIGHT );
    mVideoView.SetProperty( VideoView::Property::LOOPING, true );
    mVideoView.SetProperty( VideoView::Property::MUTED, false );
    mVideoView.SetProperty( VideoView::Property::VIDEO, PLAY_FILE );
    mVideoView.Play();

    Stage::GetCurrent().Add( mVideoView );

    mMenu = Layer::New();
    mMenu.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    mMenu.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mMenu.SetSize( 800, 120 );
    mVideoView.Add( mMenu );

    mStopButton = PushButton::New();
    mStopButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mStopButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mStopButton.SetName( "Stop" );
    mStopButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mStopButton.SetSizeScalePolicy( SizeScalePolicy::USE_SIZE_SET );
    mStopButton.SetSize( 100, 100 );
    mStopButton.SetPosition( 160, 10 );
    mStopButton.ClickedSignal().Connect( this, &VideoViewController::OnButtonClicked );

    mMuteButton = PushButton::New();
    mMuteButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mMuteButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mMuteButton.SetName( "Mute" );
    mMuteButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mMuteButton.SetSizeScalePolicy( SizeScalePolicy::USE_SIZE_SET );
    mMuteButton.SetSize( 100, 100 );
    mMuteButton.SetPosition( 270, 10 );
    mMuteButton.ClickedSignal().Connect( this, &VideoViewController::OnButtonClicked );

    mBackwardButton = PushButton::New();
    mBackwardButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mBackwardButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mBackwardButton.SetName( "Backward" );
    mBackwardButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mBackwardButton.SetSizeScalePolicy( SizeScalePolicy::USE_SIZE_SET );
    mBackwardButton.SetSize( 100, 100 );
    mBackwardButton.SetPosition( 380, 10 );
    mBackwardButton.ClickedSignal().Connect( this, &VideoViewController::OnButtonClicked );

    mMenu.Add( mStopButton );
    mMenu.Add( mMuteButton );
    mMenu.Add( mBackwardButton );

    ImageDimensions size( 100, 100 );

    mStopImage = ResourceImage::New( STOP_IMAGE, size );
    mResetImage = ResourceImage::New( RESET_IMAGE, size );
    mBackwardImage = ResourceImage::New( BACKWARD_IMAGE, size );
    mMuteOnImage = ResourceImage::New( MUTE_ON_IMAGE, size );
    mMuteOffImage = ResourceImage::New( MUTE_OFF_IMAGE, size );

    mStopButton.SetUnselectedImage( STOP_IMAGE );
    mBackwardButton.SetUnselectedImage( BACKWARD_IMAGE );
    mMuteButton.SetBackgroundImage( MUTE_OFF_IMAGE );
    mMuteButton.SetUnselectedImage( MUTE_ON_IMAGE );

    Stage::GetCurrent().KeyEventSignal().Connect(this, &VideoViewController::OnKeyEvent);
  }

  bool OnButtonClicked( Button button )
  {
    if( mStopButton.GetId() == button.GetId())
    {
      if( mIsStop )
      {
        mIsStop = false;
        mIsPlay = true;
        mVideoView.SetProperty( VideoView::Property::VIDEO, PLAY_FILE );
        mVideoView.Play();
        button.SetUnselectedImage( STOP_IMAGE );
      }
      else
      {
        mIsStop = true;
        mVideoView.Stop();
        button.SetUnselectedImage( RESET_IMAGE );
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
        mIsMuted = false;
        mVideoView.SetProperty( VideoView::Property::MUTED, false );
        button.SetUnselectedImage( MUTE_ON_IMAGE );
      }
      else
      {
        mIsMuted = true;
        mVideoView.SetProperty( VideoView::Property::MUTED, true );
        button.SetUnselectedImage( MUTE_OFF_IMAGE );
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

  Image mStopImage;
  Image mResetImage;
  Image mBackwardImage;
  Image mMuteOnImage;
  Image mMuteOffImage;

  bool mIsPlay;
  bool mIsStop;
  bool mIsMuted;

  PushButton mStopButton;
  PushButton mBackwardButton;
  PushButton mMuteButton;
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
