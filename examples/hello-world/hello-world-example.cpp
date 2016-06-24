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

#include <dali-toolkit/dali-toolkit.h>
#include <cstdio>
using namespace Dali;
using Dali::Toolkit::TextLabel;

namespace
{
Image LoadImageSynchronously(const std::string& path, ImageDimensions dimensions = ImageDimensions(0, 0), FittingMode::Type fittingMode = FittingMode::DEFAULT)
{
  EncodedBufferImage image;
  FILE *fp = fopen(path.c_str(), "r");
  if( fp )
  {
    fseek(fp, 0L, SEEK_END);
    long int fileSize = ftell(fp);
    if (fileSize == -1L) {
      fclose(fp);
      return image;
    }
    fseek(fp, 0L, SEEK_SET);
    uint8_t* memblock = new uint8_t[fileSize];
    size_t read = fread(memblock, sizeof(uint8_t), fileSize, fp);
    fclose(fp);
    if(read)
    {
      image = EncodedBufferImage::New(memblock, fileSize, dimensions, fittingMode, SamplingMode::DEFAULT, Image::UNUSED);
    }
    delete[] memblock;
  }
  return image;
}

const int kWidth = 360;
const int kHeight = 640;
}


// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application ),
    mIndex(0)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &HelloWorldController::Create );
  }

  ~HelloWorldController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    mImageView = Toolkit::ImageView::New();
    mImageView.SetParentOrigin( ParentOrigin::CENTER);
    mImageView.SetAnchorPoint( AnchorPoint::CENTER);
    stage.Add( mImageView );
    mImageView.SetSize( kWidth, kHeight, 1.0f );
    mImageView.SetPosition( 0.0f, 0.0, 0.0f );
    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchSignal().Connect( this, &HelloWorldController::OnTouch );

  }

  bool OnTouch( Actor actor, const TouchData& touch )
  {
    if ( touch.GetState(0) == PointState::DOWN )
    {
      // quit the application
      //mApplication.Quit();
      char cpath[256];
      sprintf(cpath, "/tmp/bg/gallery-%d.jpg", mIndex);
      std::string path(cpath);
      ImageDimensions dimensions( kWidth, kHeight );
      Image image = LoadImageSynchronously(path, dimensions, FittingMode::SCALE_TO_FILL );
      mImageView.SetImage( image );
      mIndex ++;
    }
    return true;
  }

private:
  Application&            mApplication;
  Toolkit::ImageView      mImageView;
  int mIndex;
};

void RunTest( Application& application )
{
  HelloWorldController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
