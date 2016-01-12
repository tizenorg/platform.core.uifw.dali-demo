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
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>

#if DALI_WAYLAND
#include <tbm_surface.h>
#include <player.h>
#include <dali/devel-api/threading/mutex.h>
#endif

using namespace Dali;
using namespace Toolkit;

namespace
{
  const std::string VIDEO_FILENAME = DALI_IMAGE_DIR "videofile.mp4";

  struct VideoFrame
  {
    tbm_surface_h surface;
    Dali::Mutex mutex;
    std::list< media_packet_h > packetList;
    media_packet_h packet;
  };

  VideoFrame gVideoFrame;

  static void media_packet_video_decoded_cb( media_packet_h packet, void* user_data )
  {
    VideoFrame *frame = (VideoFrame*)user_data;
    if( frame == NULL )
    {
      return;
    }

    Dali::Mutex::ScopedLock lock( frame->mutex );
    frame->packetList.push_back( packet );

    return;
  }
}

// This example shows how to create and display an image using a NativeImageSource with tbm_surface
//
class NativeImageSourceController : public ConnectionTracker
{
public:

  NativeImageSourceController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &NativeImageSourceController::Create );
  }

  ~NativeImageSourceController()
  {
    // Nothing to do here;
    mVideoTimer.Stop();
    player_destroy( mPlayer );
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    stage.GetRootLayer().TouchedSignal().Connect( this, &NativeImageSourceController::OnTouch );

    mTimer = Timer::New(100);
    mTimer.TickSignal().Connect( this, &NativeImageSourceController::Tick );
    mTimer.Start();

    mVideoTimer = Timer::New(1000);
    mVideoTimer.TickSignal().Connect( this, &NativeImageSourceController::FrameTick );

// player
    player_create( &mPlayer );

    player_set_media_packet_video_frame_decoded_cb( mPlayer, media_packet_video_decoded_cb, &gVideoFrame );

    player_set_display( mPlayer, PLAYER_DISPLAY_TYPE_NONE, NULL );

    player_set_uri( mPlayer, VIDEO_FILENAME.c_str() );

    player_prepare( mPlayer );
  }

  bool Tick()
  {
    Stage stage = Stage::GetCurrent();

    Any surface;
    mNativeImageSourcePtr = NativeImageSource::New( surface );

#if DALI_WAYLAND
#endif
    mNativeImage = NativeImage::New( *mNativeImageSourcePtr );
    mImageView = ImageView::New( mNativeImage );
    mImageView.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mImageView.SetParentOrigin( ParentOrigin::CENTER );
    mImageView.SetAnchorPoint( AnchorPoint::CENTER );
    stage.Add( mImageView );

    gVideoFrame.packet = NULL;
    player_start( mPlayer );
    mVideoTimer.SetInterval( 16 );

// ToDo: fix an empty image file issue

    mTimer.Stop();
    return false;
  }

  bool FrameTick()
  {
    Dali::Mutex::ScopedLock lock( gVideoFrame.mutex );

    if( gVideoFrame.packet )
    {
      if( media_packet_destroy( gVideoFrame.packet ) != MEDIA_PACKET_ERROR_NONE )
      {
        mVideoTimer.Stop();
        return false;
      }
    }

    gVideoFrame.packet = gVideoFrame.packetList.front();

    if( gVideoFrame.packet == NULL || gVideoFrame.packetList.empty())
    {
      return true;
    }

    gVideoFrame.packetList.pop_front();

    if( media_packet_get_tbm_surface( gVideoFrame.packet, &gVideoFrame.surface ) != MEDIA_PACKET_ERROR_NONE )
    {
      media_packet_destroy( gVideoFrame.packet );
      mVideoTimer.Stop();
      return false;
    }

    mNativeImageSourcePtr->SetNativeImageSource( gVideoFrame.surface );
    mNativeImage = NativeImage::New( *mNativeImageSourcePtr );
    mImageView.SetImage( mNativeImage );
    Dali::Stage::GetCurrent().KeepRendering( 0.0f );

    return true;
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mTimer.Stop();
    mVideoTimer.Stop();
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
  Timer mTimer;
  Timer mVideoTimer;
  NativeImageSourcePtr mNativeImageSourcePtr;
  NativeImage mNativeImage;
  ImageView mImageView;
  player_h mPlayer;
#if DALI_WAYLAND
  tbm_surface_h mSurface;
#endif
};

void RunTest( Application& application )
{
  NativeImageSourceController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
