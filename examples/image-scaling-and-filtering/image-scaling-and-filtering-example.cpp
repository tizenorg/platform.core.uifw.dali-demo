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
#include <iostream>

using namespace Dali;

namespace
{
const char * const IMAGE = DALI_IMAGE_DIR "gallery-large-14.wbmp";

const char* IMAGE_PATHS[] =
{
  // Worst case for aliasing in downscaling, 2k x 2k 1 bit per pixel dithered
  // black and white image:
  DALI_IMAGE_DIR "gallery-large-14.wbmp",
  // Variety of sizes, shapes and formats:
  DALI_IMAGE_DIR "animation-list.png",
  DALI_IMAGE_DIR "layer1.png",
  DALI_IMAGE_DIR "layer2.png",
  DALI_IMAGE_DIR "music-libray-main-screen.png",
  DALI_IMAGE_DIR "music-libray-record-cover.png",
  DALI_IMAGE_DIR "contacts-background.png",
  DALI_IMAGE_DIR "portrait_screen_primitive_shapes.gif",
  DALI_IMAGE_DIR "landscape_screen_primitive_shapes.gif",
  DALI_IMAGE_DIR "square_primitive_shapes.bmp",
  DALI_IMAGE_DIR "dali-logo.png",
  DALI_IMAGE_DIR "com.samsung.dali-demo.ico",
  DALI_IMAGE_DIR "gallery-large-14.jpg",
  DALI_IMAGE_DIR "book-landscape-cover.jpg",
  DALI_IMAGE_DIR "book-portrait-p1.jpg",
  DALI_IMAGE_DIR "book-landscape-cover-back.jpg",
  DALI_IMAGE_DIR "background-1.jpg",
  DALI_IMAGE_DIR "background-blocks.jpg",
  DALI_IMAGE_DIR "background-magnifier.jpg",
  DALI_IMAGE_DIR "gallery-large-14.jpg",
  NULL
};
const int NUM_IMAGE_PATHS = sizeof(IMAGE_PATHS) / sizeof(IMAGE_PATHS[0]) - 1u;

/** Cycle the scaling mode options. */
FittingMode::Type NextScalingMode( FittingMode::Type oldMode )
{
  FittingMode::Type newMode = FittingMode::SHRINK_TO_FIT;
  switch ( oldMode )
  {
    case FittingMode::SHRINK_TO_FIT:
      newMode = FittingMode::SCALE_TO_FILL;
      break;
    case FittingMode::SCALE_TO_FILL:
      newMode = FittingMode::FIT_WIDTH;
      break;
    case FittingMode::FIT_WIDTH:
      newMode = FittingMode::FIT_HEIGHT;
      break;
    case FittingMode::FIT_HEIGHT:
      newMode = FittingMode::SHRINK_TO_FIT;
      break;
  }
  return newMode;
}

/** Cycle through filter mode options. */
SamplingMode::Type NextFilterMode( SamplingMode::Type oldMode )
{
  SamplingMode::Type newMode = SamplingMode::BOX;

  switch ( oldMode )
  {
    case SamplingMode::BOX:
      newMode = SamplingMode::NEAREST;
      break;
    case SamplingMode::NEAREST:
      newMode = SamplingMode::LINEAR;
      break;
    case SamplingMode::LINEAR:
      newMode = SamplingMode::BOX_THEN_NEAREST;
      break;
    case SamplingMode::BOX_THEN_NEAREST:
      newMode = SamplingMode::BOX_THEN_LINEAR;
      break;
    case SamplingMode::BOX_THEN_LINEAR:
      newMode = SamplingMode::NO_FILTER;
      break;
    case SamplingMode::NO_FILTER:
      newMode = SamplingMode::BOX;
      break;
    case SamplingMode::DONT_CARE:
      newMode = SamplingMode::BOX;
      break;
  }
  return newMode;
}

const char* StringFromScalingMode( FittingMode::Type scalingMode )
{
  return scalingMode == FittingMode::SCALE_TO_FILL ? "SCALE_TO_FILL" : scalingMode == FittingMode::SHRINK_TO_FIT ? "SHRINK_TO_FIT" : scalingMode == FittingMode::FIT_WIDTH ? "FIT_WIDTH" : scalingMode == FittingMode::FIT_HEIGHT ? "FIT_HEIGHT" : "UnknownScalingMode";
}

const char* StringFromFilterMode( SamplingMode::Type filterMode )
{
  return filterMode == SamplingMode::BOX ? "BOX" : filterMode == SamplingMode::BOX_THEN_NEAREST ? "BOX_THEN_NEAREST" : filterMode == SamplingMode::BOX_THEN_LINEAR ? "BOX_THEN_LINEAR" : filterMode == SamplingMode::NEAREST ? "NEAREST" : filterMode == SamplingMode::LINEAR ? "LINEAR" : filterMode == SamplingMode::NO_FILTER ? "NO_FILTER" : filterMode == SamplingMode::DONT_CARE ? "DONT_CARE" : "UnknownFilterMode";
}

}

// This example shows the load-time image scaling and filtering features.
//
class ImageScalingAndFilteringController : public ConnectionTracker
{
public:

  ImageScalingAndFilteringController( Application& application )
  : mApplication( application ),
    mImageStageScale( 0.5f, 0.5f ),
    mCurrentPath( 0 ),
    mFilterMode( SamplingMode::BOX_THEN_LINEAR
                 ),
    mScalingMode( FittingMode::SCALE_TO_FILL )
  {
    std::cerr << "ImageScalingAndFilteringController::ImageScalingAndFilteringController" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ImageScalingAndFilteringController::Create );
  }

  ~ImageScalingAndFilteringController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    std::cerr << "ImageScalingAndFilteringController::Create" << std::endl;

    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    // Make a red pixel for the desired box, a green one for the desired height and a blue one for the desired width:
    BufferImage desiredBackground = BufferImage::WHITE();
    PixelBuffer* const redPixel = desiredBackground.GetBuffer();
    redPixel[0] = 0xff;
    redPixel[1] = 0x00;
    redPixel[2] = 0x00;

    BufferImage heightBackground = BufferImage::WHITE();
    PixelBuffer* const greenPixel = heightBackground.GetBuffer();
    greenPixel[0] = 0x00;
    greenPixel[1] = 0xff;
    greenPixel[2] = 0x00;

    BufferImage widthBackground = BufferImage::WHITE();
    PixelBuffer* const bluePixel = widthBackground.GetBuffer();
    bluePixel[0] = 0x00;
    bluePixel[1] = 0x00;
    bluePixel[2] = 0xff;

    mHeightBox = ImageActor::New( heightBackground );
    stage.Add( mHeightBox );

    mWidthBox = ImageActor::New( widthBackground );
    stage.Add( mWidthBox );

    mDesiredBox = ImageActor::New( desiredBackground );
    stage.Add( mDesiredBox );

    mDesiredBox.SetSize( stage.GetSize() * mImageStageScale );
    mDesiredBox.SetParentOrigin( ParentOrigin::CENTER );
    mDesiredBox.SetAnchorPoint( AnchorPoint::CENTER );
    mDesiredBox.SetPosition( 0, 0, -1 );

    mHeightBox.SetSize( stage.GetSize().width,  (stage.GetSize() * mImageStageScale).height );
    mHeightBox.SetParentOrigin( ParentOrigin::CENTER );
    mHeightBox.SetAnchorPoint( AnchorPoint::CENTER );
    mHeightBox.SetPosition( 0, 0, -1 );

    mWidthBox.SetSize( (stage.GetSize() * mImageStageScale).width, stage.GetSize().height );
    mWidthBox.SetParentOrigin( ParentOrigin::CENTER );
    mWidthBox.SetAnchorPoint( AnchorPoint::CENTER );
    mWidthBox.SetPosition( 0, 0, -1 );

    // Initialize the actor
    mImageActor = ImageActor::New();

    // Reposition the actor
    mImageActor.SetParentOrigin( ParentOrigin::CENTER );
    //mImageActor.SetSizeMode( SIZE_EQUAL_TO_PARENT );
    mImageActor.SetAnchorPoint( AnchorPoint::CENTER );

        // Display the actor on the stage
    stage.Add( mImageActor );

    //mImageActor.SetImage( image );
    mImageActor.SetSize( stage.GetSize() * mImageStageScale );

    ResizeImage();

    // Tie-in input event handlers:
    stage.KeyEventSignal().Connect( this, &ImageScalingAndFilteringController::OnKeyEvent );
  }

  void OnImageLoaded( ResourceImage image )
  {
      DALI_ASSERT_DEBUG( image == mNextImage );

      std::cerr << "Loaded: image: " << image.GetWidth() << ", " << image.GetHeight() << "." << std::endl;
      mImageActor.SetImage( image );
      mImageActor.SetSize( Size( image.GetWidth(), image.GetHeight() ) ); //< This will work once the core and adaptor are enhanced to store the post-load dimensions of images in those images.
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    std::cerr << " ================ KEY ============== \n";
    std::cerr << event.keyPressedName << " - " << event.keyPressed << " - " << event.keyCode << " - " << event.keyModifier << " - " << event.time << " - " << event.state << "\n";

    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
      else if ( event.keyPressedName == "Right" )
      {
        mImageStageScale.x = std::max( 0.05f, std::min( 1.0f, mImageStageScale.x * 1.1f ) );
      }
      else if ( event.keyPressedName == "Left" )
      {
        mImageStageScale.x = std::max( 0.05f, mImageStageScale.x * 0.9f );
      }
      else if ( event.keyPressedName == "Up" )
      {
        mImageStageScale.y = std::max( 0.05f, std::min( 1.0f, mImageStageScale.y * 1.1f ) );
      }
      else if ( event.keyPressedName == "Down" )
      {
        mImageStageScale.y = std::max( 0.05f, mImageStageScale.y * 0.9f );
      }
      else if ( event.keyPressedName == "o" )
      {
        mImageStageScale.x = std::max( 0.05f, mImageStageScale.x * 0.9f );
        mImageStageScale.y = std::max( 0.05f, mImageStageScale.y * 0.9f );
      }
      else if ( event.keyPressedName == "p" )
      {
        mImageStageScale.x = std::max( 0.05f, std::min( 1.0f, mImageStageScale.x * 1.1f ) );
        mImageStageScale.y = std::max( 0.05f, std::min( 1.0f, mImageStageScale.y * 1.1f ) );
      }
      else if ( event.keyPressedName == "n" )
      {
        mCurrentPath = mCurrentPath + 1;
        mCurrentPath = mCurrentPath <  NUM_IMAGE_PATHS ? mCurrentPath : 0;
      }
      else if ( event.keyPressedName == "b" )
      {
        mCurrentPath = mCurrentPath - 1;
        mCurrentPath = mCurrentPath >= 0 ? mCurrentPath : NUM_IMAGE_PATHS - 1;
      }
      // Cycle filter and scaling modes:
      else if ( event.keyPressedName == "f" )
      {
        mFilterMode = NextFilterMode( mFilterMode );
      }
      // Cycle filter and scaling modes:
      else if ( event.keyPressedName == "s" )
      {
        mScalingMode = NextScalingMode( mScalingMode );
      }
      else
      {
        return;
      }

      ResizeImage();
    }
    else if( event.state == KeyEvent::Up )
    {
    }
  }

private:
  void ResizeImage()
  {
    const char * const path = IMAGE_PATHS[mCurrentPath];

    Stage stage = Stage::GetCurrent();
    Size imageSize = stage.GetSize() * mImageStageScale;
    const ImageDimensions imageSizeInt = ImageDimensions::FromFloatArray( &imageSize.x );

    //std::cerr << "Loading image from dims " << ResourceImage::GetImageSize( path ) << " to screen dims " << imageSize << ", scaling: " << StringFromScalingMode( mScalingMode ) << ", filter: " << StringFromFilterMode( mFilterMode ) << ", from path " << path << std::endl;

    ResourceImage image = ResourceImage::New( path, imageSizeInt, mScalingMode, mFilterMode );
    image.LoadingFinishedSignal().Connect( this, &ImageScalingAndFilteringController::OnImageLoaded );

    mNextImage = image;

    mDesiredBox.SetSize( stage.GetSize() * mImageStageScale );
    mHeightBox.SetSize( stage.GetSize().width,  (stage.GetSize() * mImageStageScale).height );
    mWidthBox.SetSize( (stage.GetSize() * mImageStageScale).width, stage.GetSize().height );
  }

private:
  Application&  mApplication;
  Actor mDesiredBox; //< Red background to show requested image size.
  Actor mHeightBox;  //< Green background to show requested image height.
  Actor mWidthBox;   //< Blue background to show requested image width.
  ImageActor mImageActor;
  ResourceImage mNextImage; //< Currently-loading image
  Vector2 mImageStageScale;
  int mCurrentPath;
  SamplingMode::Type mFilterMode;
  FittingMode::Type mScalingMode;
};

void RunTest( Application& application )
{
  ImageScalingAndFilteringController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
