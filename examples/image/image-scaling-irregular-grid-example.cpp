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

/**
 * @file image-scaling-irregular-grid-example.cpp
 * @brief Demonstrates how to use image scaling modes when loading images.
 *
 * If an image is going to be drawn on-screen at a lower resolution than it is
 * stored at on-disk, the scaling feature of the image loader can be used to
 * reduce the image to save memory, improve performance, and potentially display
 * a better small version of the image than if the default size were loaded.
 *
 * This demo defaults to the ScaleToFill mode of ImageAttributes which makes
 * sure that every pixel in the loaded image is filled with a source colour
 * from the image's central region while losing the minimum number of pixels
 * from its periphery.
 * It is the best option for producing thumbnails of input images that have
 * diverse aspect ratios.
 *
 * The other four scaling modes of dali can be cycled-through for the whole
 * grid  using the button in the top-right of the toolbar.
 * A single image can be cycled by clicking the image directly.
 */
// INTERNAL INCLUDES
#include "../shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

// EXTERNAL INCLUDES
#include <algorithm>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-4.jpg" );
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char* APPLICATION_TITLE( "Image Scaling Modes" );
const char* TOGGLE_SCALING_IMAGE( DALI_IMAGE_DIR "icon-change.png" );

/** The width of the grid in whole grid cells. */
const unsigned GRID_WIDTH = 9;
/** Limit the grid to be no higher than this in units of a cell. */
const unsigned GRID_MAX_HEIGHT = 450;

/** The space between the edge of a grid cell and the image embedded within it. */
const unsigned GRID_CELL_PADDING = 4;

/** The aspect ratio of cells in the image grid. */
const float CELL_ASPECT_RATIO = 1.33333333333333333333f;

const ImageAttributes::ScalingMode DEFAULT_SCALING_MODE = ImageAttributes::ScaleToFill;

/** The number of times to spin an image on touching, each spin taking a second.*/
const float SPIN_DURATION = 1.0f;

/** The target image sizes in grid cells. */
const Vector2 IMAGE_SIZES[] = {
 Vector2( 1, 1 ),
 Vector2( 2, 1 ),
 Vector2( 3, 1 ),
 Vector2( 1, 2 ),
 Vector2( 1, 3 ),
 Vector2( 2, 3 ),
 Vector2( 3, 2 ),
 // Large, tall configuration:
 Vector2( GRID_WIDTH / 2, GRID_WIDTH + GRID_WIDTH / 2 ),
 // Large, square-ish images to show shrink-to-fit well with wide and tall images:
 Vector2( GRID_WIDTH / 2, GRID_WIDTH / 2.0f * CELL_ASPECT_RATIO + 0.5f ),
 Vector2( GRID_WIDTH - 2, (GRID_WIDTH - 2) * CELL_ASPECT_RATIO + 0.5f ),
};
const unsigned NUM_IMAGE_SIZES = sizeof(IMAGE_SIZES) / sizeof(IMAGE_SIZES[0]);

/** Images to load into the grid. These are mostly large and non-square to
 *  show the scaling. */
const char* IMAGE_PATHS[] = {

  DALI_IMAGE_DIR "square.gif",
  //DALI_IMAGE_DIR "square.gif",

  DALI_IMAGE_DIR "tall.gif",
  //DALI_IMAGE_DIR "tall.gif",

  DALI_IMAGE_DIR "wide.gif",
  //DALI_IMAGE_DIR "wide.gif",

  // Images from other demos that are tall, wide or just large:

  DALI_IMAGE_DIR "gallery-large-1.jpg",
  DALI_IMAGE_DIR "gallery-large-2.jpg",
  DALI_IMAGE_DIR "gallery-large-3.jpg",
  DALI_IMAGE_DIR "gallery-large-4.jpg",
  DALI_IMAGE_DIR "gallery-large-5.jpg",
  DALI_IMAGE_DIR "gallery-large-6.jpg",
  DALI_IMAGE_DIR "gallery-large-7.jpg",
  DALI_IMAGE_DIR "gallery-large-8.jpg",
  DALI_IMAGE_DIR "gallery-large-9.jpg",
  DALI_IMAGE_DIR "gallery-large-10.jpg",
  DALI_IMAGE_DIR "gallery-large-11.jpg",
  DALI_IMAGE_DIR "gallery-large-12.jpg",
  DALI_IMAGE_DIR "gallery-large-13.jpg",
  DALI_IMAGE_DIR "gallery-large-14.jpg",
  DALI_IMAGE_DIR "gallery-large-15.jpg",
  DALI_IMAGE_DIR "gallery-large-16.jpg",
  DALI_IMAGE_DIR "gallery-large-17.jpg",
  DALI_IMAGE_DIR "gallery-large-18.jpg",
  DALI_IMAGE_DIR "gallery-large-19.jpg",
  DALI_IMAGE_DIR "gallery-large-20.jpg",
  DALI_IMAGE_DIR "gallery-large-21.jpg",

  DALI_IMAGE_DIR "background-1.jpg",
  DALI_IMAGE_DIR "background-2.jpg",
  DALI_IMAGE_DIR "background-3.jpg",
  DALI_IMAGE_DIR "background-4.jpg",
  DALI_IMAGE_DIR "background-5.jpg",
  DALI_IMAGE_DIR "background-blocks.jpg",
  DALI_IMAGE_DIR "background-gradient.jpg",
  DALI_IMAGE_DIR "background-magnifier.jpg",

  DALI_IMAGE_DIR "background-1.jpg",
  DALI_IMAGE_DIR "background-2.jpg",
  DALI_IMAGE_DIR "background-3.jpg",
  DALI_IMAGE_DIR "background-4.jpg",
  DALI_IMAGE_DIR "background-5.jpg",
  DALI_IMAGE_DIR "background-blocks.jpg",
  DALI_IMAGE_DIR "background-magnifier.jpg",

  DALI_IMAGE_DIR "book-landscape-cover-back.jpg",
  DALI_IMAGE_DIR "book-landscape-cover.jpg",
  DALI_IMAGE_DIR "book-landscape-p1.jpg",
  DALI_IMAGE_DIR "book-landscape-p2.jpg",
  DALI_IMAGE_DIR "book-landscape-p3.jpg",
  DALI_IMAGE_DIR "book-landscape-p4.jpg",
  DALI_IMAGE_DIR "book-landscape-p5.jpg",
  DALI_IMAGE_DIR "book-landscape-p6.jpg",
  DALI_IMAGE_DIR "book-landscape-p7.jpg",
  DALI_IMAGE_DIR "book-landscape-p8.jpg",

  DALI_IMAGE_DIR "book-portrait-cover.jpg",
  DALI_IMAGE_DIR "book-portrait-p1.jpg",
  DALI_IMAGE_DIR "book-portrait-p2.jpg",
  DALI_IMAGE_DIR "book-portrait-p3.jpg",
  DALI_IMAGE_DIR "book-portrait-p4.jpg",
  DALI_IMAGE_DIR "book-portrait-p5.jpg",
  DALI_IMAGE_DIR "book-portrait-p6.jpg",
  DALI_IMAGE_DIR "book-portrait-p7.jpg",
  DALI_IMAGE_DIR "book-portrait-p8.jpg",

  ///@ToDo: Reenable PNGs when the PNG loader works better with scaling.
  // A large, 3840 * 2160 pixel image:
  // --  DALI_IMAGE_DIR "wide.4k.png",
  // Artificial images that are labeled with their aspect ratios and contain
  // many squares to allow aspect ratio preservation to be visually inspected:
// --  DALI_IMAGE_DIR "tall.png",
// --  DALI_IMAGE_DIR "square.png",
// --  DALI_IMAGE_DIR "wide.png",
// --  DALI_IMAGE_DIR "blocks-paddle.png",
// --  DALI_IMAGE_DIR "brick-wall.jpg",
// --  DALI_IMAGE_DIR "dali-logo.png",
// --  DALI_IMAGE_DIR "layer1.png",
  NULL
};
const unsigned NUM_IMAGE_PATHS = sizeof(IMAGE_PATHS) / sizeof(IMAGE_PATHS[0]) - 1u;

inline unsigned min( unsigned a, unsigned b )
{
  return a < b ? a : b;
}

inline unsigned max( unsigned a, unsigned b )
{
  return a > b ? a : b;
}

/** Cycle the scaling mode options. */
ImageAttributes::ScalingMode NextMode( const ImageAttributes::ScalingMode oldMode )
{
  ImageAttributes::ScalingMode newMode = ImageAttributes::ShrinkToFit;
  if(oldMode == ImageAttributes::ShrinkToFit) newMode = ImageAttributes::ScaleToFill;
  if(oldMode == ImageAttributes::ScaleToFill) newMode = ImageAttributes::FitWidth;
  if(oldMode == ImageAttributes::FitWidth) newMode = ImageAttributes::FitHeight;
  return newMode;
}



/**
 * Bundle an image path with the rectangle to pack it into.
 * */
struct ImageConfiguration
{
  ImageConfiguration( const char * const path, const Vector2 dimensions ) :
    path( path ),
    dimensions( dimensions )
  {}
  const char * path;
  Vector2 dimensions;
};

/**
 * Post-layout image data.
 */
struct PositionedImage
{
  PositionedImage(ImageConfiguration& configuration, unsigned cellX, unsigned cellY, Vector2 imageGridDims) :
    configuration( configuration ),
    cellX( cellX ),
    cellY( cellY ),
    imageGridDims( imageGridDims )
  {}

  ImageConfiguration configuration;
  unsigned cellX;
  unsigned cellY;
  Vector2 imageGridDims;
};

/** A set of bools addressable over a 2D grid. */
class GridFlags
{
public:
  GridFlags( unsigned width, unsigned height ) :  mCells( width * height ), mWidth( width ), mHeight( height ), mFirstRow( 0 ), mHighestUsedRow( 0 )
  {
    fprintf(stderr, "Grid created with dimensions: (%u, %u).\n", mWidth, mHeight );
  }

  void Set( const unsigned x, const unsigned y )
  {
    const unsigned index = CellIndex( x, y );
    mCells[index] += 1u; ///< += To allow a debug check of the number of times a cell is set.
    if( mCells[index] > 1u )
    {
      fprintf(stderr, " - Cell(%u,%u) = %u - ", x, y, unsigned(mCells[index]) );
    }
    mHighestUsedRow = max( mHighestUsedRow, y );
  }

  bool Get( unsigned x, unsigned y ) const
  {
    return mCells[ CellIndex( x, y ) ] != 0;
  }

  unsigned GetHighestUsedRow() const
  {
    return mHighestUsedRow;
  }

  /**
   * Try to find space in a grid of cells for the region requested.
   */
  bool AllocateRegion( const Vector2& region, unsigned& outCellX, unsigned& outCellY, Vector2& outRegion )
  {
    const unsigned regionWidth = (region.x + 0.5f);
    const unsigned regionHeight = (region.y + 0.5f);
    fprintf( stderr, "Allocation requested for region (%u, %u). Result: ", regionWidth, regionHeight );
    unsigned bestRegionWidth = 0;
    unsigned bestRegionHeight = 0;
    unsigned bestCellX = 0;
    unsigned bestCellY = 0;

    // Look for a non-set cell:
    for( unsigned y = mFirstRow; y < mHeight; ++y )
    {
      for( unsigned x = 0; x < mWidth; ++x )
      {
        if ( !Get( x, y) )
        {
          // Look for clear grid cells under the desired region:

          const unsigned clampedRegionHeight = min( regionHeight, mHeight - y);
          const unsigned clampedRegionWidth = min( regionWidth, mWidth - x);
          const unsigned regionLimitY = y + clampedRegionHeight;
          const unsigned regionLimitX = x + clampedRegionWidth;

          for( unsigned regionY = y; regionY < regionLimitY; ++regionY )
          {
            for( unsigned regionX = x; regionX < regionLimitX; ++regionX )
            {
              if( Get( regionX, regionY ) )
              {
                // The region of clear cells is not big enough but remember it
                // anyway in case there is no region that fits:
                const unsigned clearRegionWidth = regionX - x;
                const unsigned clearRegionHeight = (regionY + 1) - y;
                if( clearRegionWidth * clearRegionHeight > bestRegionWidth * bestRegionHeight )
                {
                  bestCellX = x;
                  bestCellY = y;
                  bestRegionWidth = clearRegionWidth;
                  bestRegionHeight = clearRegionHeight;
                }
                goto whole_region_not_found;
              }
            }
          }

          // Found a big-enough region was found, greedily allocate it:
          if( clampedRegionHeight == regionHeight && clampedRegionWidth == regionWidth )
          {
            bestCellX = x;
            bestCellY = y;
            bestRegionWidth  = min( mWidth - x,  regionWidth);
            bestRegionHeight = min( mHeight - y, regionHeight);
            // End early:
            x = mWidth;
            y = mHeight;
          }
whole_region_not_found:
          continue;
        }
      }
    }

    // Allocate and return the best cell region found:

    if( bestRegionWidth == 0 || bestRegionHeight == 0 )
    {
      fputs( "false.\n", stderr );
      return false;
    }

    // Allocate the found region:
    fprintf( stderr, " - bestCellX = %u, bestCellY = %u, bestRegionWidth = %u, bestRegionHeight = %u - ", bestCellX, bestCellY, bestRegionWidth, bestRegionHeight );
    for( unsigned y = bestCellY; y < bestCellY + bestRegionHeight; ++y )
    {
      for( unsigned x = bestCellX; x < bestCellX + bestRegionWidth; ++x )
      {
        fprintf( stderr, "Set(%u,%u), ", x, y );
        Set( x, y );
      }
    }

    outCellX = bestCellX;
    outCellY = bestCellY;
    outRegion = Vector2( bestRegionWidth, bestRegionHeight );
    fputs( "true.\n", stderr );
    return true;
  }

  /** @return True if every cell was set one or zero times, else false. */
  bool DebugCheckGridValid()
  {
    for( unsigned cell = 0; cell < mWidth * mHeight; ++cell )
    {
      if( mCells[cell] > 1 )
      {
        return false;
      }
    }
    return true;
  }

private:
  unsigned CellIndex( unsigned x, unsigned y ) const
  {
    const unsigned offset = mWidth * y + x;
    assert( offset < mCells.size() && "Out of range access to grid." );
    return offset;
  }

  std::vector<unsigned char> mCells;
  const unsigned mWidth;
  const unsigned mHeight;
  unsigned mFirstRow;
  unsigned mHighestUsedRow;

};

/**
 * Creates an Image
 *
 * @param[in] filename The path of the image.
 * @param[in] width The width of the image in pixels.
 * @param[in] height The height of the image in pixels.
 * @param[in] scalingMode The mode to use when scaling the image to fit the desired dimensions.
 */
Image CreateImage(const std::string& filename, unsigned int width, unsigned int height, ImageAttributes::ScalingMode scalingMode )
{
  fprintf( stderr, "CreateImage(%s, %u, %u, scalingMode=%u)\n", filename.c_str(), width, height, unsigned(scalingMode) );
  ImageAttributes attributes;

  attributes.SetSize( width, height );
  attributes.SetScalingMode( scalingMode );
  Image image = Image::New( filename, attributes );
  return image;
}

/**
 * Creates an ImageActor
 *
 * @param[in] filename The path of the image.
 * @param[in] width The width of the image in pixels.
 * @param[in] height The height of the image in pixels.
 * @param[in] scalingMode The mode to use when scaling the image to fit the desired dimensions.
 */
ImageActor CreateImageActor(const std::string& filename, unsigned int width, unsigned int height, ImageAttributes::ScalingMode scalingMode )
{
  Image img = CreateImage( filename, width, height, scalingMode );
  ImageActor actor = ImageActor::New( img );
  actor.SetName( filename );
  actor.SetParentOrigin(ParentOrigin::CENTER);
  actor.SetAnchorPoint(AnchorPoint::CENTER);

  return actor;
}

}

/**
 * @brief The main class of the demo.
 */
class ImageScalingIrregularGridController : public ConnectionTracker
{
public:

  ImageScalingIrregularGridController( Application& application )
  : mApplication( application ),
    mScrolling( false )
  {
    std::cout << "ImageScalingScaleToFillController::ImageScalingScaleToFillController" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ImageScalingIrregularGridController::Create );
  }

  ~ImageScalingIrregularGridController()
  {
    // Nothing to do here.
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    std::cout << "ImageScalingScaleToFillController::Create" << std::endl;

    // Get a handle to the stage:
    Stage stage = Stage::GetCurrent();

    // Connect to input event signals:
    stage.KeyEventSignal().Connect(this, &ImageScalingIrregularGridController::OnKeyEvent);

    // Hide the indicator bar
    mApplication.GetWindow().ShowIndicator(false);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.

    mContentLayer = DemoHelper::CreateView( mApplication,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            "" );

    // Create an image scaling toggle button. (right of toolbar)
    Image toggleScalingImage = Image::New( TOGGLE_SCALING_IMAGE );
    Toolkit::PushButton toggleScalingButton = Toolkit::PushButton::New();
    toggleScalingButton.SetBackgroundImage( toggleScalingImage );
    toggleScalingButton.ClickedSignal().Connect( this, &ImageScalingIrregularGridController::OnToggleScalingTouched );
    mToolBar.AddControl( toggleScalingButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    SetTitle( APPLICATION_TITLE );

    // Build the main content of the widow:
    PopulateContentLayer( DEFAULT_SCALING_MODE );
  }

  void PopulateContentLayer( const ImageAttributes::ScalingMode scalingMode )
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    float fieldHeight;
    Actor imageField = BuildImageField( stageSize.x, GRID_WIDTH, GRID_MAX_HEIGHT, scalingMode, fieldHeight );

    mScrollView = ScrollView::New();

    mScrollView.ScrollStartedSignal().Connect( this, &ImageScalingIrregularGridController::OnScrollStarted );
    mScrollView.ScrollCompletedSignal().Connect( this, &ImageScalingIrregularGridController::OnScrollCompleted );

    mScrollView.EnableScrollComponent( Scrollable::VerticalScrollBar );
    mScrollView.EnableScrollComponent( Scrollable::HorizontalScrollBar );

    mScrollView.SetAnchorPoint(AnchorPoint::CENTER);
    mScrollView.SetParentOrigin(ParentOrigin::CENTER);

    mScrollView.SetSize( stageSize );//Vector2( stageSize.width, fieldHeight ) );//stageSize );
    mScrollView.SetAxisAutoLock( true );
    mScrollView.SetAxisAutoLockGradient( 1.0f );

    // Restrict scrolling to mostly vertical only, but with some horizontal wiggle-room:

    RulerPtr rulerX = new FixedRuler( stageSize.width ); //< Pull the view back to the grid's centre-line when touch is release using a snapping ruler.
    rulerX->SetDomain( RulerDomain( stageSize.width * -0.125f, stageSize.width * 1.125f ) ); //< Scroll slightly left/right of image field.
    mScrollView.SetRulerX ( rulerX );

    RulerPtr rulerY = new DefaultRuler(); //stageSize.height ); //< Snap in multiples of a screen / stage height
    rulerY->SetDomain( RulerDomain( - fieldHeight * 0.5f + stageSize.height * 0.5f - GRID_CELL_PADDING, fieldHeight * 0.5f + stageSize.height * 0.5f + GRID_CELL_PADDING ) );
    mScrollView.SetRulerY ( rulerY );

    mContentLayer.Add( mScrollView );
    mScrollView.Add( imageField );
    mGridActor = imageField;
  }

  /**
   * Build a field of images scaled into a variety of shapes from very wide,
   * through square, to very tall. The images are direct children of the Dali::Actor
   * returned.
   **/
  Actor BuildImageField( const float fieldWidth,
                           const unsigned gridWidth,
                           const unsigned maxGridHeight,
                           ImageAttributes::ScalingMode scalingMode,
                           float & outFieldHeight )
  {
    // Generate the list of image configurations to be fitted into the field:
    std::vector<ImageConfiguration> configurations;
    configurations.reserve( NUM_IMAGE_PATHS * NUM_IMAGE_SIZES );
    for( unsigned imageIndex = 0; imageIndex < NUM_IMAGE_PATHS; ++imageIndex )
    {
      for( unsigned dimensionsIndex = 0; dimensionsIndex < NUM_IMAGE_SIZES; ++ dimensionsIndex )
      {
        configurations.push_back( ImageConfiguration( IMAGE_PATHS[imageIndex], IMAGE_SIZES[dimensionsIndex] ) );
      }
    }
    // Stir-up the list to get some nice irregularity in the generated field:
    std::random_shuffle( configurations.begin(), configurations.end() );
    std::random_shuffle( configurations.begin(), configurations.end() );

    // Place the images in the grid:

    std::vector<ImageConfiguration>::iterator config, end;
    GridFlags grid( gridWidth, maxGridHeight );
    std::vector<PositionedImage> placedImages;

    for( config = configurations.begin(), end = configurations.end(); config != end; ++config )
    {
      unsigned cellX, cellY;
      Vector2 imageGridDims;

      // Allocate a region of the grid for the image:
      bool allocated = grid.AllocateRegion( config->dimensions, cellX, cellY, imageGridDims );
      if( !allocated )
      {
        fprintf( stderr, "Failed to allocate image in grid with dims (%f, %f) and path: %s.\n", config->dimensions.x, config->dimensions.y, config->path );
        continue;
      }

      placedImages.push_back( PositionedImage( *config, cellX, cellY, imageGridDims ) );
    }
    DALI_ASSERT_DEBUG( grid.DebugCheckGridValid() && "Cells were set more than once, indicating erroneous overlap in placing images on the grid." );
    const unsigned actualGridHeight = grid.GetHighestUsedRow() + 1;

    // Take the images images in the grid and turn their logical locations into
    // coordinates in a frame defined by a parent actor:

    Actor gridActor = Actor::New();
    gridActor.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    gridActor.SetParentOrigin( ParentOrigin::CENTER );
    gridActor.SetAnchorPoint( AnchorPoint::CENTER );

    // Work out the constants of the grid and cell dimensions and positions:
    const float cellWidth = fieldWidth / gridWidth;
    const float cellHeight = cellWidth / CELL_ASPECT_RATIO;
    const Vector2 cellSize = Vector2( cellWidth, cellHeight );
    outFieldHeight = actualGridHeight * cellHeight;
    const Vector2 gridOrigin = Vector2( -fieldWidth * 0.5f, -outFieldHeight * 0.5 );

    // Build the image actors in their right locations in their parent's frame:
    for( std::vector<PositionedImage>::const_iterator i = placedImages.begin(), end = placedImages.end(); i != end; ++i )
    {
      const PositionedImage& imageSource = *i;
      const Vector2 imageSize = imageSource.imageGridDims * cellSize - Vector2( GRID_CELL_PADDING * 2, GRID_CELL_PADDING * 2 );
      const Vector2 imageRegionCorner = gridOrigin + cellSize * Vector2( imageSource.cellX, imageSource.cellY );
      const Vector2 imagePosition = imageRegionCorner + Vector2( GRID_CELL_PADDING , GRID_CELL_PADDING ) + imageSize * 0.5f;

      ImageActor image = CreateImageActor( imageSource.configuration.path, imageSize.x, imageSize.y, scalingMode );
      image.SetPosition( Vector3( imagePosition.x, imagePosition.y, 0 ) );
      image.SetSize( imageSize );
      image.TouchedSignal().Connect( this, &ImageScalingIrregularGridController::OnTouchImage );
      mScalingModes[image.GetId()] = scalingMode;
      mSizes[image.GetId()] = imageSize;

      gridActor.Add( image );
    }

    return gridActor;
  }

 /**
  * Upon Touching an image (Release), change its scaling mode and make it spin, provided we're not scrolling.
  * @param[in] actor The actor touched
  * @param[in] event The TouchEvent.
  */
  bool OnTouchImage( Actor actor, const TouchEvent& event )
  {
    if( (event.points.size() > 0) && (!mScrolling) )
    {
      TouchPoint point = event.points[0];
      if(point.state == TouchPoint::Up)
      {
        // Spin the image a few times:
        Animation animation = Animation::New(SPIN_DURATION);
        animation.RotateBy( actor, Degree(360.0f * SPIN_DURATION), Vector3::XAXIS, AlphaFunctions::EaseOut);
        animation.Play();

        // Change the scaling mode:
        const unsigned id = actor.GetId();
        ImageAttributes::ScalingMode newMode = NextMode( mScalingModes[id] );
        const Vector2 imageSize = mSizes[actor.GetId()];

        ImageActor imageActor = ImageActor::DownCast( actor );
        Image oldImage = imageActor.GetImage();
        Image newImage = CreateImage( oldImage.GetFilename(), imageSize.width + 0.5f, imageSize.height + 0.5f, newMode );
        imageActor.SetImage( newImage );
        mScalingModes[id] = newMode;
      }
    }
    return false;
  }

 /**
  * Main key event handler.
  * Quit on escape key.
  */
  void OnKeyEvent(const KeyEvent& event)
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE )
          || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

 /**
  * Signal handler, called when the 'Scaling' button has been touched.
  *
  * @param[in] button The button that was pressed.
  */
  bool OnToggleScalingTouched( Button button )
  {
    const unsigned numChildren = mGridActor.GetChildCount();
    fprintf(stderr, "OnToggleScalingTouched() - %u children.\n", numChildren );
    for( unsigned i = 0; i < numChildren; ++i )
    {
      ImageActor gridImageActor = ImageActor::DownCast( mGridActor.GetChildAt( i ) );
      fprintf(stderr, "OnToggleScalingTouched() - Child %u - ", i );
      if( gridImageActor )
      {
        // Cycle the scaling mode options:
        const Vector2 imageSize = mSizes[gridImageActor.GetId()];
        ImageAttributes::ScalingMode newMode = NextMode( mScalingModes[gridImageActor.GetId()] );
        Image oldImage = gridImageActor.GetImage();
        Image newImage = CreateImage( oldImage.GetFilename(), imageSize.width, imageSize.height, newMode );
        gridImageActor.SetImage( newImage );

        mScalingModes[gridImageActor.GetId()] = newMode;
        fprintf(stderr, "set new image, mode %u, for %s.\n", unsigned(newMode), oldImage.GetFilename().c_str() );

        SetTitle( std::string( newMode == ImageAttributes::ShrinkToFit ? "ShrinkToFit" : newMode == ImageAttributes::ScaleToFill ?  "ScaleToFill" : newMode == ImageAttributes::FitWidth ? "FitWidth" : "FitHeight" ) );
      }
    }
    return true;
  }

  /**
   * Sets/Updates the title of the View
   * @param[in] title The new title for the view.
   */
  void SetTitle(const std::string& title)
  {
    if(!mTitleActor)
    {
      mTitleActor = TextView::New();
      // Add title to the tool bar.
      mToolBar.AddControl( mTitleActor, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Alignment::HorizontalCenter );
    }

    Font font = Font::New();
    mTitleActor.SetText( title );
    mTitleActor.SetSize( font.MeasureText( title ) );
    mTitleActor.SetStyleToCurrentText(DemoHelper::GetDefaultTextStyle());
  }

  /**
   * When scroll starts (i.e. user starts to drag scrollview),
   * note this state (mScrolling = true)
   * @param[in] position Current Scroll Position
   */
  void OnScrollStarted( const Vector3& position )
  {
    mScrolling = true;
  }

  /**
   * When scroll starts (i.e. user stops dragging scrollview, and scrollview has snapped to destination),
   * note this state (mScrolling = false).
   * @param[in] position Current Scroll Position
   */
  void OnScrollCompleted( const Vector3& position )
  {
    mScrolling = false;
  }

private:
  Application&  mApplication;

  Layer mContentLayer;                ///< The content layer (contains non gui chrome actors)
  Toolkit::View mView;                ///< The View instance.
  Toolkit::ToolBar mToolBar;          ///< The View's Toolbar.
  TextView mTitleActor;               ///< The Toolbar's Title.
  Actor mGridActor;                   ///< The container for the grid of images
  ScrollView mScrollView;             ///< ScrollView UI Component
  bool mScrolling;                    ///< ScrollView scrolling state (true = scrolling, false = stationary)
  ScrollViewEffect mScrollViewEffect; ///< ScrollView Effect instance.
  Actor mScrollViewContainer;         ///< Container Sprite within ScrollView.
  std::map<unsigned, ImageAttributes::ScalingMode> mScalingModes; ///< Stores the current scaling mode of each image, keyed by image actor id.
  std::map<unsigned, Vector2> mSizes; ///< Stores the current size of each image, keyed by image actor id.

};

void RunTest( Application& application )
{
  ImageScalingIrregularGridController test( application );

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
