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
#include <fstream>
#include <tbm_surface.h>

using namespace Dali;
using namespace DaliToolkit;

namespace
{
  const std::string RAW_FILENAME = DALI_IMAGE_DIR "background-3_720x1280.raw";
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
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    mTextLabel = TextLabel::New( "Hello World" );
    mTextLabel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mTextLabel.SetName( "helloWorldLabel" );
    stage.Add( mTextLabel );

    stage.GetRootLayer().TouchedSignal().Connect( this, &NativeImageSourceController::OnTouch );

    mTimer = Timer::New(100);
    mTimer.TickSignal().Connect( this, &NativeImageSourceController::Tick );
    mTimer.Start();
  }

  bool Tick()
  {
    Any source;
    mNativeImageSourcePtr = NativeImageSource::New( 720, 1280, NativeImageSource::COLOR_DEPTH_24 );
    Any source = mNativeImageSourcePtr.GetNativeImageSource();
    SetPixelBufferIntoTBMsurface( source, RAW_FILENAME );

    mNativeImage = NativeImage::New( *mNativeImageSourcePtr );

    Stage::GetCurrent().Add( mNativeImage );

    PrintShaderCode( mNativeImage );
    mTimer.Stop();
    return false;
  }

  void SetPixelBufferIntoTBMsurface( Any& image, std::string filename )
  {
    tbm_surface_h surface = AnyCast< tbm_surface_h >( image );
    tbm_surface_info_s surface_info;

    if( tbm_surface_map( surface, TBM_SURF_OPTION_WRITE, &surface_info ) != TBM_SURFACE_ERROR_NONE )
    {
      mTextLabel.SetProperty( TextLabel::Property::TEXT, "Fail to map tbm_surface" );
      return;
    }

    int height = surface_info.height;
    int width = surface_info.width;
    unsigned char* bufferPtr = surface_info.planes[0].ptr;
    uint32_t stride = surface_info.planes[0].stride;

    int lineSize = 720 * 3;
    int size = lineSize * 1280;
    unsigned char* rawBufferPtr = new unsigned char[size];
    unsigned char* rawBufferLinePtr = NULL;

    ifstream in;
    in.open( filename, std::ifstream::binary );

    if( in.is_open() )
    {
      in.read( (char*) rawBufferPtr, size );
      rawBufferLinePtr = rawBufferPtr;

      for( int r = 0; r < height; ++r )
      {
        memcpy( bufferPtr, rawBufferLinePtr, lineSize);
        bufferPtr += (r*stride);
        rawLineBufferPtr += (r*lineSize);
      }
    }

    in.close();

    if( tbm_surface_unmap( surface ) != TBM_SURFACE_ERROR_NONE )
    {
      mTextLabel.SetProperty( TextLabel::Property::TEXT, "Fail to unmap tbm_surface" );
    }
  }

  void PrintShaderCode( Image& image )
  {
    NativeImage nativeImage = NativeImage::DownCast( image );
    mTextLabel.SetProperty( TextLabel::Property::TEXT, nativeImage.GetCustomVertexShaderCode() );
    mTextLabel.SetProperty( TextLabel::Property::TEXT, nativeImage.GetCustomFragmentShaderCode() );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
  TextLabel mTextLabel;
  Timer mTimer;
  NativeImageSourcePtr mNativeImageSourcePtr;
  NativeImage mNativeImage;
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
