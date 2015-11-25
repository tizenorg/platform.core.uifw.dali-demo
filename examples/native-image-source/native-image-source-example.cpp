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
  const std::string RAW_FILENAME = DALI_IMAGE_DIR "background-3_720x1280RGB.raw";
  const std::string TEST_RESULT = "/tmp/rbackground-3_720x1280RGB.raw";
  const std::string TEST_RESULTPNG = "/tmp/dali-image.png";
  const int WIDTH = 720;
  const int HEIGHT = 1280;
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
    mNativeImageSourcePtr = NativeImageSource::New( WIDTH, HEIGHT, NativeImageSource::COLOR_DEPTH_DEFAULT );

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

    WriteRGBImage( TEST_RESULT );

// ToDo: fix an empty image file issue
    mNativeImageSourcePtr->EncodeToFile( TEST_RESULTPNG );

    mTimer.Stop();
    return false;
  }

  bool SetPixmap( Any source )
  {

    return true;
  }

  bool WriteRGBImage( std::string filename )
  {
    std::ofstream out;
    out.open( filename.c_str(), std::ofstream::binary );
    if( out.is_open() == false )
    {
      return false;
    }

    std::vector< unsigned char > pixBuf;
    std::vector< unsigned char > pixRGBBuf;
    unsigned int width(0), height(0);
    Pixel::Format pixelFormat;

    size_t rgbaLineSize;
    size_t rgbLineSize;
    size_t rgbaOffset;
    size_t rgbOffset;

    if( mNativeImageSourcePtr->GetPixels(pixBuf, width, height, pixelFormat) )
    {
      rgbaLineSize = width*4;
      rgbLineSize = width*3;

      switch( pixelFormat )
      {
        case Pixel::RGBA8888:
        {
          pixRGBBuf.resize( rgbLineSize*height );
          for( unsigned int r = 0; r < height; ++r )
          {
            for( unsigned int c = 0; c < width; ++c )
            {
              rgbaOffset = c*4 + r*rgbaLineSize;
              rgbOffset = c*3 + r*rgbLineSize;
              pixRGBBuf[rgbOffset + 0] = pixBuf[rgbaOffset + 2];
              pixRGBBuf[rgbOffset + 1] = pixBuf[rgbaOffset + 1];
              pixRGBBuf[rgbOffset + 2] = pixBuf[rgbaOffset + 0];
            }
          }
          out.write( (char*)&pixRGBBuf[0], rgbLineSize*height );
          out.close();
          break;
        }
        case Pixel::RGB888:
        {
          out.write( (char*)&pixBuf[0], rgbLineSize*height );
          out.close();
          break;
        }
        default:
          return false;
      }
    }

    return true;
  }

#ifdef TIZEN_WAYLAND
  bool SetPixelBufferIntoTBMsurface( Any source, std::string filename )
  {
    mSurface = NULL;

    if( source.Empty() )
    {

      mSurface = tbm_surface_create( WIDTH, HEIGHT, TBM_FORMAT_RGBA8888 );
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

    if( surface_info.format != TBM_FORMAT_RGBA8888 )
    {
      return false;
    }

    int height = surface_info.height;
    int width = surface_info.width;
    unsigned char* bufferPtr = surface_info.planes[0].ptr;
    uint32_t stride = surface_info.planes[0].stride;

    size_t lineSize = WIDTH * 3;
    size_t size = lineSize * HEIGHT;
    unsigned char* rawBufferPtr = new unsigned char[size];
    size_t tbmOffset;
    size_t fileOffset;

    std::ifstream in;
    in.open( filename.c_str(), std::ifstream::binary );

    if( in.is_open() )
    {
      in.read( (char*) rawBufferPtr, size );

      for( int r = 0; r < height; ++r )
      {
        for( int c = 0; c < width; ++c )
        {
          tbmOffset = c*4 + r*stride;
          fileOffset = c*3 + r*lineSize;
          bufferPtr[tbmOffset+3] = rawBufferPtr[fileOffset+0];
          bufferPtr[tbmOffset+2] = rawBufferPtr[fileOffset+1];
          bufferPtr[tbmOffset+1] = rawBufferPtr[fileOffset+2];
          bufferPtr[tbmOffset+0] = 255;
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
