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
  const std::string VIDEOURL( "/media/videofile.mp4" );
}  // namespace

class VideoViewController: public ConnectionTracker
{
 public:

  VideoViewController( Application& application )
    : mApplication( application )
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
    mVideoView.SetSize( 800, 400 );
    mVideoView.SetLooping( true );
    mVideoView.SetUrl( VIDEOURL );
    mVideoView.Play();

    Stage::GetCurrent().Add( mVideoView );

    mPlayButton = PushButton::New();
    mPlayButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL );
    mPlayButton.StateChangedSign
    mVideoView.Add( mMenu );
    Stage::GetCurrent().KeyEventSignal().Connect(this, &VideoViewController::OnKeyEvent);
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
  VideoView        mVideoView;
  Layer mMenu;
  PushButton mPlayButton;
  PushButton mActionModeButton;
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
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );

  RunTest( application );

  return 0;
}
