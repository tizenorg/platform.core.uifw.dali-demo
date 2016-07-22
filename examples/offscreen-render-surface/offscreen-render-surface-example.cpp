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

#include <dali/public-api/dali-core.h>
#include <dali/public-api/common/stage.h>
#include <dali/devel-api/adaptor-framework/singleton-service.h>
#include <dali/integration-api/adaptors/adaptor.h>
#include <dali/integration-api/adaptors/native-source-render-surface.h>
#include <dali/integration-api/adaptors/native-source-render-surface-factory.h>
#include <dali/integration-api/adaptors/trigger-event-factory.h>
#include <dali-toolkit/dali-toolkit.h>
#include <cstring>
#include <tbm_surface.h>
#include <Ecore.h>
#include <Ecore_Wayland.h>

using namespace Dali;
using Dali::Toolkit::TextLabel;

namespace
{
const int MAX_NUM_TBM = 1000;

const char* IMAGE_PATH[] = {
    DEMO_IMAGE_DIR "blocks-ball.png",
    DEMO_IMAGE_DIR "gallery-small-23.jpg",
    DEMO_IMAGE_DIR "selection-popup-bg.2.9.png",
    DEMO_IMAGE_DIR "heartsframe.9.png",
};

const unsigned int NUM_IMAGES = sizeof(IMAGE_PATH) / sizeof(char*);

const unsigned int COLUMNS = 3;
const unsigned int ROWS = 4;

const unsigned int WIDTH = 400;
const unsigned int HEIGHT = 800;

}

struct OffScreenBuffer
{
  OffScreenBuffer()
  : mCurrentIndex( -1 )
  {
  }

  ~OffScreenBuffer()
  {
    tbm_surface_destroy( mConsumeSurface );
  }

  bool Copy( tbm_surface_h curSurface )
  {
    mCurrentIndex++;

    if( mCurrentIndex == MAX_NUM_TBM-1 )
    {
      return false;
    }

    tbm_surface_info_s info, newInfo;

    if( tbm_surface_map( curSurface, TBM_SURF_OPTION_READ, &info ) != TBM_SURFACE_ERROR_NONE )
    {
      return false;
    }

    if( mConsumeSurface )
    {
      tbm_surface_destroy( mConsumeSurface );
    }
    mConsumeSurface = tbm_surface_create( info.width, info.height, info.format );

    if( mConsumeSurface == NULL )
    {
      return false;
    }

    if( tbm_surface_map( mConsumeSurface, TBM_SURF_OPTION_READ|TBM_SURF_OPTION_WRITE, &newInfo ) != TBM_SURFACE_ERROR_NONE )
    {
      return false;
    }

    for(unsigned int i = 0; i < info.num_planes; ++i )
    {
      memcpy( newInfo.planes[i].ptr, info.planes[i].ptr, info.planes[i].size );
    }

    if( tbm_surface_unmap( curSurface ) != TBM_SURFACE_ERROR_NONE )
    {
      return false;
    }

    if( tbm_surface_unmap( mConsumeSurface ) != TBM_SURFACE_ERROR_NONE )
    {
      return false;
    }

    return true;
  }

  tbm_surface_h mConsumeSurface;
  int mCurrentIndex;

};

class OffScreenRender : public ConnectionTracker
{
public:
  OffScreenRender( OffScreenBuffer* buffer )
  : mNativeSourceRenderSurface(NULL)
  , mAdaptor(NULL)
  , mRenderNotification(NULL)
  , mBuffer( buffer )
  , mFinished( false )
  {
    mSingletonService = SingletonService::New();

    mNativeSourceRenderSurface = CreateNativeSourceSurface( PositionSize( 0, 0, WIDTH, HEIGHT ), "TestNativeSourceRenderSurface", true );

    Any nativeWindow = ecore_wl_window_new(NULL, 0, 0, WIDTH, HEIGHT, 0);
    mAdaptor = &Adaptor::New( nativeWindow, *mNativeSourceRenderSurface, Configuration::APPLICATION_DOES_NOT_HANDLE_CONTEXT_LOSS );

    TriggerEventFactory triggerEventFactory;
    mRenderNotification = triggerEventFactory.CreateTriggerEvent(
                                      MakeCallback(this, &OffScreenRender::OnPostRender),
                                      TriggerEventInterface::KEEP_ALIVE_AFTER_TRIGGER );

    mNativeSourceRenderSurface->SetRenderNotification(mRenderNotification);

  }

  ~OffScreenRender()
  {
    mAdaptor->Stop();
    mSingletonService.UnregisterAll();

    delete mRenderNotification;
    mRenderNotification = NULL;

    delete mAdaptor;
    mAdaptor = NULL;

    delete mNativeSourceRenderSurface;
    mNativeSourceRenderSurface = NULL;

    ecore_wl_shutdown();
  }

  void Run()
  {
    mAdaptor->Start();
    mAdaptor->NotifySceneCreated();

    Prepare();
    mTimer = Timer::New(500);
    mTimer.TickSignal().Connect( this, &OffScreenRender::RenderFinished);
    mTimer.Start();

    ecore_main_loop_begin();
  }

  void Prepare()
  {
    Toolkit::TableView table = Toolkit::TableView::New( ROWS, COLUMNS );
    table.SetAnchorPoint( AnchorPoint::CENTER );
    table.SetParentOrigin( ParentOrigin::CENTER );
    table.SetResizePolicy( ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::ALL_DIMENSIONS );
    Vector3 offset( -50.0f, -350.0f, 0.0f );
    table.SetSizeModeFactor( offset );

    Stage::GetCurrent().Add( table );

    for( unsigned int y = 0; y < ROWS; ++y )
    {
      for( unsigned int x = 0; x < COLUMNS; ++x )
      {
        mImageViews[x][y] = Toolkit::ImageView::New( IMAGE_PATH[ 0 ] );
        mImageViews[x][y].SetParentOrigin( ParentOrigin::CENTER );
        mImageViews[x][y].SetAnchorPoint( AnchorPoint::CENTER );
        mImageViews[x][y].SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

        table.AddChild( mImageViews[x][y], Toolkit::TableView::CellPosition( y, x ) );
      }
    }
  }

  void OnPostRender()
  {
    mAdaptor->ReleaseSurfaceLock();
    mBuffer->Copy( mNativeSourceRenderSurface->GetDrawable());

    mNativeSourceRenderSurface->ReleaseNativeSource();
    mFinished = true;
  }

  bool RenderFinished()
  {
    mTimer.Stop();
    ecore_main_loop_quit();
    return mFinished;
  }

private:
  SingletonService mSingletonService;
  NativeSourceRenderSurface* mNativeSourceRenderSurface;
  Adaptor* mAdaptor;
  TriggerEventInterface* mRenderNotification;
  OffScreenBuffer* mBuffer;

  Toolkit::ImageView mImageViews[ COLUMNS ][ ROWS ];
  bool mFinished;
  Timer mTimer;
};

class OffScreenController : public ConnectionTracker
{
public:

  OffScreenController( Application& application, OffScreenBuffer* buffer )
  : mApplication( application )
  , mBuffer( buffer )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &OffScreenController::Create );
  }

  ~OffScreenController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    mTextLabel = TextLabel::New( "Hello World" );
    mTextLabel.SetParentOrigin( ParentOrigin::CENTER );
    mTextLabel.SetAnchorPoint( AnchorPoint::CENTER );
    mTextLabel.SetName( "helloWorldLabel" );
    stage.Add( mTextLabel );

    mTimer = Timer::New( 1000 );
    mTimer.TickSignal().Connect( this, &OffScreenController::SetOffscreen);
    mTimer.Start();

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchSignal().Connect( this, &OffScreenController::OnTouch );
  }

  bool SetOffscreen()
  {
    Stage::GetCurrent().Remove( mTextLabel );

    NativeImageSourcePtr nativeImageSource = NativeImageSource::New( mBuffer->mConsumeSurface );
    NativeImage image = NativeImage::New( *nativeImageSource );
    Toolkit::ImageView imageView = Toolkit::ImageView::New( image );
    imageView.SetParentOrigin( ParentOrigin::CENTER );
    imageView.SetAnchorPoint( AnchorPoint::CENTER );
    imageView.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

    Stage::GetCurrent().Add( imageView );
    mTimer.Stop();

    return false;
  }

  bool OnTouch( Actor actor, const TouchData& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
  TextLabel mTextLabel;
  Timer mTimer;
  OffScreenBuffer* mBuffer;
};

// Entry point for Linux & Tizen applications
//
int DALI_EXPORT_API main( int argc, char **argv )
{
  OffScreenBuffer buffer;

  {
    OffScreenRender render( &buffer );
    render.Run();
  }

  Application application = Application::New( &argc, &argv );
  OffScreenController test( application, &buffer );
  application.MainLoop();

  return 0;
}
