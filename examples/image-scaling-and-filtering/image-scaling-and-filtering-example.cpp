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
  DALI_IMAGE_DIR "gallery-large-14.wbmp",
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
ImageAttributes::ScalingMode NextScalingMode( ImageAttributes::ScalingMode oldMode )
{
  ImageAttributes::ScalingMode newMode = ImageAttributes::ShrinkToFit;
  switch ( oldMode )
  {
    case ImageAttributes::ShrinkToFit:
      newMode = ImageAttributes::ScaleToFill;
      break;
    case ImageAttributes::ScaleToFill:
      newMode = ImageAttributes::FitWidth;
      break;
    case ImageAttributes::FitWidth:
      newMode = ImageAttributes::FitHeight;
      break;
    case ImageAttributes::FitHeight:
      newMode = ImageAttributes::ShrinkToFit;
      break;
  }
  return newMode;
}

/** Cycle through filter mode options. */
ImageAttributes::FilterMode NextFilterMode( ImageAttributes::FilterMode oldMode )
{
  ImageAttributes::FilterMode newMode = ImageAttributes::Box;

  switch ( oldMode )
  {
    case ImageAttributes::Box:
      newMode = ImageAttributes::Nearest;
      break;
    case ImageAttributes::Nearest:
      newMode = ImageAttributes::Linear;
      break;
    case ImageAttributes::Linear:
      newMode = ImageAttributes::BoxThenNearest;
      break;
    case ImageAttributes::BoxThenNearest:
      newMode = ImageAttributes::BoxThenLinear;
      break;
    case ImageAttributes::BoxThenLinear:
      newMode = ImageAttributes::NoFilter;
      break;
    case ImageAttributes::NoFilter:
      newMode = ImageAttributes::Box;
      break;
    case ImageAttributes::DontCare:
      newMode = ImageAttributes::Box;
      break;
  }
  return newMode;
}

const char* StringFromScalingMode( ImageAttributes::ScalingMode scalingMode )
{
  return scalingMode == ImageAttributes::ScaleToFill ? "ScaleToFill" : scalingMode == ImageAttributes::ShrinkToFit ? "ShrinkToFit" : scalingMode == ImageAttributes::FitWidth ? "FitWidth" : scalingMode == ImageAttributes::FitHeight ? "FitHeight" : "UnknownScalingMode";
}

const char* StringFromFilterMode( ImageAttributes::FilterMode filterMode )
{
  return filterMode == ImageAttributes::Box ? "Box" : filterMode == ImageAttributes::BoxThenNearest ? "BoxThenNearest" : filterMode == ImageAttributes::BoxThenLinear ? "BoxThenLinear" : filterMode == ImageAttributes::Nearest ? "Nearest" : filterMode == ImageAttributes::Linear ? "Linear" : filterMode == ImageAttributes::NoFilter ? "NoFilter" : filterMode == ImageAttributes::DontCare ? "DontCare" : "UnknownFilterMode";
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
    mFilterMode( ImageAttributes::BoxThenLinear
                 ),
    mScalingMode( ImageAttributes::ScaleToFill )
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

    //BufferImage desiredBackgrund = BufferImage::WHITE();
    mDesiredBox = ImageActor::New( BufferImage::WHITE() );
    stage.Add( mDesiredBox );
    mDesiredBox.SetSize( stage.GetSize() * mImageStageScale );
    mDesiredBox.SetParentOrigin( ParentOrigin::CENTER );
    mDesiredBox.SetAnchorPoint( AnchorPoint::CENTER );
    mDesiredBox.SetPosition( 0, 0, -1 );

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
    stage.GetRootLayer().TouchedSignal().Connect( this, &ImageScalingAndFilteringController::OnTouch );
    stage.KeyEventSignal().Connect( this, &ImageScalingAndFilteringController::OnKeyEvent );
  }

  void OnImageLoaded( ResourceImage image )
  {
      DALI_ASSERT_DEBUG( image == mNextImage );
      ImageAttributes postLoad_attribs = image.GetAttributes();


      std::cerr << "Loaded: image: " << image.GetWidth() << ", " << image.GetHeight() << ". attribs: " << postLoad_attribs.GetSize() << ". actor old size: " <<  mImageActor.GetSize().x << ", " << mImageActor.GetSize().y << std::endl;
      mImageActor.SetImage( image );
      //mImageActor.SetSize( Vector2( image.GetWidth(), image.GetHeight() ) );
      mImageActor.SetSize( postLoad_attribs.GetSize() );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    //mApplication.Quit();
    return true;
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
        /// BOOKMARK XXX
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
      //else if( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
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
    ImageAttributes attributes;
    attributes.SetSize( imageSize );
    attributes.SetFilterMode( mFilterMode );
    attributes.SetScalingMode( mScalingMode );

    std::cerr << "Loading image from dims " << ResourceImage::GetImageSize( path ) << " to screen dims " << imageSize << ", scaling: " << StringFromScalingMode( mScalingMode ) << ", filter: " << StringFromFilterMode( mFilterMode ) << ", from path " << path << std::endl;

    ResourceImage image = ResourceImage::New( path, attributes );
    image.LoadingFinishedSignal().Connect( this, &ImageScalingAndFilteringController::OnImageLoaded );

    //mImageActor.SetImage( image );
    //mImageActor.SetSize( imageSize );
    mNextImage = image;

    mDesiredBox.SetSize( stage.GetSize() * mImageStageScale );
  }

private:
  Application&  mApplication;
  Actor mDesiredBox;
  ImageActor mImageActor;
  ResourceImage mNextImage; //< Currently-loading image
  Vector2 mImageStageScale;
  //std::string mImagePath;
  int mCurrentPath;
  ImageAttributes::FilterMode mFilterMode;
  ImageAttributes::ScalingMode mScalingMode;
};

void RunTest( Application& application )
{
  ImageScalingAndFilteringController test( application );

  application.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
