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

#define TIZEN_WAYLAND

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <cstring>
#include <fstream>

#ifdef TIZEN_WAYLAND
#include <tbm_surface.h>
#endif

using namespace Dali;
using namespace Toolkit;

namespace
{
  const std::string RAW_FILENAME = DALI_IMAGE_DIR "background-3_720x1280.raw";
  const std::string TEST_RESULT = "/tmp/dali_testimage.raw";
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
tbm_surface_destroy(mSurface);
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
  }

  bool Tick()
  {
    Stage stage = Stage::GetCurrent();

    Any surface;
    mNativeImageSourcePtr = NativeImageSource::New( 720, 1280, NativeImageSource::COLOR_DEPTH_DEFAULT );

#ifdef TIZEN_WAYLAND
    SetPixelBufferIntoTBMsurface( mNativeImageSourcePtr->GetNativeImageSource(), RAW_FILENAME );
#endif
    mNativeImage = NativeImage::New( *mNativeImageSourcePtr );
    mImageView = ImageView::New( mNativeImage );
    mImageView.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mImageView.SetParentOrigin( ParentOrigin::CENTER );
    mImageView.SetAnchorPoint( AnchorPoint::CENTER );
    stage.Add( mImageView );

    Dali::Stage::GetCurrent().KeepRendering( 0.0f );

    std::ofstream out;
    out.open( TEST_RESULT.c_str(), std::ofstream::binary );
    if( out.is_open() == false )
    {
      return false;
    }

    std::vector< unsigned char > pixbuf;
    unsigned int width(0), height(0);
    Pixel::Format pixelFormat;
    if( mNativeImageSourcePtr->GetPixels(pixbuf, width, height, pixelFormat) )
    {
      out.write( (char*)&pixbuf[0], width*height );
    }

    out.close();

    mTimer.Stop();
    return false;
  }

  bool SetPixmap( Any source )
  {

    return true;
  }

#ifdef TIZEN_WAYLAND
  bool SetPixelBufferIntoTBMsurface( Any source, std::string filename )
  {
    mSurface = NULL;

    if( source.Empty() )
    {

      mSurface = tbm_surface_create( 720, 1280, TBM_FORMAT_ARGB8888 );
      if( mSurface == NULL )
      {
        return false;
      }
    }
    else
    {
      mSurface = AnyCast< tbm_surface_h >( source );
    }

    tbm_surface_info_s surface_info;
    if( tbm_surface_map( mSurface, TBM_SURF_OPTION_READ | TBM_SURF_OPTION_WRITE, &surface_info ) != TBM_SURFACE_ERROR_NONE )
    {
      return false;
    }

    int height = surface_info.height;
    int width = surface_info.width;
    unsigned char* bufferPtr = surface_info.planes[0].ptr;
    uint32_t stride = surface_info.planes[0].stride;

    int lineSize = 720 * 3;
    int size = lineSize * 1280;
    unsigned char* rawBufferPtr = new unsigned char[size];

    std::ifstream in;
    in.open( filename.c_str(), std::ifstream::binary );

    if( in.is_open() )
    {
      in.read( (char*) rawBufferPtr, size );

    // Todo. DDK for tizen 3.0 wayland have some issue. After fix it, the part below should be changed
      for( int r = 0; r < height; ++r )
      {
        for( int c = 0; c < width; ++c )
        {
          bufferPtr[(c*4+3)+r*stride] = 255;
          bufferPtr[(c*4+2)+r*stride] = rawBufferPtr[(c*3+0)+r*lineSize];
          bufferPtr[(c*4+1)+r*stride] = rawBufferPtr[(c*3+1)+r*lineSize];
          bufferPtr[(c*4+0)+r*stride] = rawBufferPtr[(c*3+2)+r*lineSize];
        }
      }

    }

    in.close();

    if( tbm_surface_unmap( mSurface ) != TBM_SURFACE_ERROR_NONE )
    {
      return false;
    }

    return true;
  }
#endif

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mTimer.Stop();
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
  Timer mTimer;
  NativeImageSourcePtr mNativeImageSourcePtr;
  NativeImage mNativeImage;
  ImageView mImageView;
#ifdef TIZEN_WAYLAND
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
