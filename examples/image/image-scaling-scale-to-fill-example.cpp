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
const char* APPLICATION_TITLE( "Image Scaling: ScaleToFill" );
const char* TOGGLE_SCALING_IMAGE( DALI_IMAGE_DIR "icon-change.png" );

/** The allowed widths of the grid. These are cycled-through on button-press. */
const unsigned GRID_WIDTHS[] = {
    9, ///~@ToDo: <----------------[Temp
    3, 4, 5, 6, 7 };
const unsigned NUM_GRID_WIDTHS = sizeof(GRID_WIDTHS) / sizeof(GRID_WIDTHS[0]);
const unsigned GRID_HEIGHT = 10;

/** The space between the edge of a grid cell and the image embedded within it.
 */
const unsigned GRID_CELL_PADDING = 4;

/** The target image sizes in grid cells. */
const Vector2 IMAGE_SIZES[] = {
 Vector2( 1, 2 ),
 Vector2( 1, 1 ),
 Vector2( 2, 1 ),
 Vector2( 3, 1 ),
 //Vector2( 1, 2 ),
 Vector2( 2, 2 ),
 Vector2( 2, 3 ),
 Vector2( 3, 2 )
};
const unsigned NUM_IMAGE_SIZES = sizeof(IMAGE_SIZES) / sizeof(IMAGE_SIZES[0]);

/** Images to load into the grid. These are mostly large and non-square to
 *  show the scaling. */
const char* IMAGE_PATHS[] = {

  // A large, 3840 * 2160 pixel image:
  DALI_IMAGE_DIR "wide.4k.png",

  // A 1440 * 2560 pixel image with clear central square and lots of filamentous detail that will alias:
  DALI_IMAGE_DIR "portrait_screen_filaments.1440w.png",

  // Artificial images that are labeled with their aspect ratios and contain
  // many squares to allow aspect ratio preservation to be visually inspected:
  DALI_IMAGE_DIR "tall.png",
  DALI_IMAGE_DIR "square.png",
  DALI_IMAGE_DIR "wide.png",

  DALI_IMAGE_DIR "square.gif",
  DALI_IMAGE_DIR "square.gif",
  DALI_IMAGE_DIR "square.gif",
  DALI_IMAGE_DIR "square.gif",
  DALI_IMAGE_DIR "square.gif",
  DALI_IMAGE_DIR "square.gif",
  DALI_IMAGE_DIR "square.gif",
  DALI_IMAGE_DIR "square.gif",
  DALI_IMAGE_DIR "square.gif",
  DALI_IMAGE_DIR "square.gif",

  DALI_IMAGE_DIR "tall.gif",
  DALI_IMAGE_DIR "tall.gif",
  DALI_IMAGE_DIR "tall.gif",
  DALI_IMAGE_DIR "tall.gif",
  DALI_IMAGE_DIR "tall.gif",

  DALI_IMAGE_DIR "wide.gif",
  DALI_IMAGE_DIR "wide.gif",
  DALI_IMAGE_DIR "wide.gif",
  DALI_IMAGE_DIR "wide.gif",
  DALI_IMAGE_DIR "wide.gif",

  // Images from other demos that are tall, wide or just large:

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
  DALI_IMAGE_DIR "gallery-large-1.jpg",
  DALI_IMAGE_DIR "gallery-large-20.jpg",
  DALI_IMAGE_DIR "gallery-large-21.jpg",
  DALI_IMAGE_DIR "gallery-large-2.jpg",
  DALI_IMAGE_DIR "gallery-large-3.jpg",
  DALI_IMAGE_DIR "gallery-large-4.jpg",
  DALI_IMAGE_DIR "gallery-large-5.jpg",
  DALI_IMAGE_DIR "gallery-large-6.jpg",
  DALI_IMAGE_DIR "gallery-large-7.jpg",
  DALI_IMAGE_DIR "gallery-large-8.jpg",
  DALI_IMAGE_DIR "gallery-large-9.jpg",

  DALI_IMAGE_DIR "background-1.jpg",
  DALI_IMAGE_DIR "background-2.jpg",
  DALI_IMAGE_DIR "background-3.jpg",
  DALI_IMAGE_DIR "background-4.jpg",
  DALI_IMAGE_DIR "background-5.jpg",
  DALI_IMAGE_DIR "background-blocks.jpg",
  DALI_IMAGE_DIR "background-gradient.jpg",
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

  DALI_IMAGE_DIR "book-portrait-p1.jpg",
  DALI_IMAGE_DIR "book-portrait-p2.jpg",
  DALI_IMAGE_DIR "book-portrait-p3.jpg",
  DALI_IMAGE_DIR "book-portrait-p4.jpg",
  DALI_IMAGE_DIR "book-portrait-p5.jpg",
  DALI_IMAGE_DIR "book-portrait-cover.jpg",
  DALI_IMAGE_DIR "book-portrait-p6.jpg",
  DALI_IMAGE_DIR "book-portrait-p7.jpg",
  DALI_IMAGE_DIR "book-portrait-p8.jpg",

  DALI_IMAGE_DIR "blocks-paddle.png",
  DALI_IMAGE_DIR "blocks-paddle.png",

  DALI_IMAGE_DIR "brick-wall.jpg",
  DALI_IMAGE_DIR "dali-logo.png",

  DALI_IMAGE_DIR "image-with-border-1.jpg",
  DALI_IMAGE_DIR "image-with-border-2.jpg",
  DALI_IMAGE_DIR "image-with-border-3.jpg",
  DALI_IMAGE_DIR "image-with-border-4.jpg",
  DALI_IMAGE_DIR "image-with-border-5.jpg",

  DALI_IMAGE_DIR "layer1.png",

  DALI_IMAGE_DIR "people-medium-10.jpg",
  DALI_IMAGE_DIR "people-medium-2.jpg",
  DALI_IMAGE_DIR "people-medium-6.jpg",
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

/** A set of bools addressable over a 2D grid. */
class GridFlags
{
public:
  GridFlags( unsigned width, unsigned height ) : mWidth( width ), mHeight( height ), mFirstRow( 0 ), mCells( width * height )
  {
    fprintf(stderr, "Grid created with dimensions: (%u, %u).\n", mWidth, mHeight );
  }

  void Set( unsigned x, unsigned y )
  {
    const unsigned index = CellIndex( x, y );
    mCells[index] += 1u; ///< += To allow a debug check of the number of times a cell is set.
    if( mCells[index] > 1u )
    {
      fprintf(stderr, " - Cell(%u,%u) = %u - ", x, y, unsigned(mCells[index]) );
    }
  }

  bool Get( unsigned x, unsigned y ) const
  {
    return mCells[ CellIndex( x, y ) ] != 0;
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
          for( unsigned regionY = y; regionY < min( y + regionHeight, mHeight ); ++regionY )
          {
            for( unsigned regionX = x; regionX < min( x + regionWidth, mWidth ); ++regionX )
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

          // Found a region so greedily allocate it and return it:
          bestCellX = x;
          bestCellY = y;
          bestRegionWidth  = min( mWidth - x,  regionWidth);
          bestRegionHeight = min( mHeight - y, regionHeight);
          // End early:
          x = mWidth;
          y = mHeight;
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

  const unsigned mWidth;
  const unsigned mHeight;
  unsigned mFirstRow;
  std::vector<unsigned char> mCells;
};

/**
 * Creates an Image (Helper)
 *
 * @param[in] filename The path of the image.
 * @param[in] width The width of the image in pixels.
 * @param[in] height The height of the image in pixels.
 * @param[in] scalingMode The mode to use when scaling the image to fit the desired dimensions.
 */
ImageActor CreateImage(const std::string& filename, unsigned int width, unsigned int height, ImageAttributes::ScalingMode scalingMode )
{
  ImageAttributes attributes;

  attributes.SetSize( width, height );
  attributes.SetScalingMode( scalingMode );
  Image img = Image::New( filename, attributes );
  ImageActor actor = ImageActor::New( img );
  actor.SetName( filename );
  actor.SetParentOrigin(ParentOrigin::CENTER);
  actor.SetAnchorPoint(AnchorPoint::CENTER);

  //actor.TouchedSignal().Connect( this, touchHandler ); ///@ToDo <--------------------------------------------------------------<<<
  return actor;
}

/**
 * Build a field of images scaled into a variety of shapes from very wide,
 * through square, to very tall. The images are direct children of the Dali::Actor
 * returned.
 **/
Actor BuildImageField( const Vector2 fieldSize, const unsigned gridWidth, const unsigned gridHeight )
{
  Actor gridActor = Actor::New();
  gridActor.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
  gridActor.SetParentOrigin( ParentOrigin::CENTER );
  gridActor.SetAnchorPoint( AnchorPoint::CENTER );

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

  // Work out the constants of the grid and cell dimensions and positions:
  const Vector2 cellSize = fieldSize / Vector2( gridWidth, gridHeight );
  const Vector2 gridOrigin = -(fieldSize * 0.5f);

  // Place the images in the grid:

  std::vector<ImageConfiguration>::iterator config, end;
  GridFlags grid( gridWidth, gridHeight );

  for( config = configurations.begin(), end = configurations.end(); config != end; ++config )
  {
    unsigned cellX, cellY;
    Vector2 imageGridDims;

    // Allocate a region of the grid for the image:
    bool allocated = grid.AllocateRegion(config->dimensions, cellX, cellY, imageGridDims );
    if( !allocated )
    {
      fprintf( stderr, "Failed to allocate image in grid with dims (%f, %f) and path: %s.\n", config->dimensions.x, config->dimensions.y, config->path );
      continue;
    }

    const Vector2 imageSize = imageGridDims * cellSize - Vector2( GRID_CELL_PADDING * 2, GRID_CELL_PADDING * 2 );

    ImageActor image = CreateImage( config->path, imageSize.x, imageSize.y, ImageAttributes::ScaleToFill );

    image.SetParentOrigin( ParentOrigin::CENTER );
    image.SetAnchorPoint( AnchorPoint::CENTER );

    const Vector2 imageRegionCorner = gridOrigin + cellSize * Vector2( cellX, cellY );
    const Vector2 imagePosition = imageRegionCorner + Vector2( GRID_CELL_PADDING , GRID_CELL_PADDING ) + imageSize * 0.5f;
    image.SetPosition( Vector3( imagePosition.x, imagePosition.y, 0 ) );//position + imageSize * 0.5f );
    image.SetSize( imageSize );
    gridActor.Add( image );
  }

  DALI_ASSERT_DEBUG( grid.DebugCheckGridValid() && "Cells were set more than once, indicating erroneous overlap in placing images on the grid." );

  // Add a text label:
  TextActor textActor = TextActor::New( APPLICATION_TITLE );
  textActor.SetParentOrigin( ParentOrigin::CENTER );
  textActor.SetPosition( Vector3( 0, 0, fieldSize.x / 1.7f ) );
  gridActor.Add( textActor );

  return gridActor;
}

}

/**
 * @brief Demonstrate how to use the ScaleToFill image scaling mode when loading
 * images.
 *
 * If the rectangle on-screen that an image is to occupy contains fewer pixels
 * than the image to be loaded into it, the scaling feature of the image loader
 * can be used to reduce the image to save memory, improve performance, and
 * potentially display a better small version of the image than if the default
 * size were loaded and downsampled at render time.
 * The ScaleToFill mode of ImageAttributes makes sure that every pixel in the
 * loaded image is filled with a source colour from the image's central region
 * while losing the minimum number of pixels from its periphery.
 * It is the best option for producing thumbnails of input images that have
 * diverse aspect ratios.
 *
 * This demo uses ImageAttributes::ScaleToFill to load a grid of thumbnails.
 */
class ImageScalingScaleToFillController : public ConnectionTracker
{
public:

  ImageScalingScaleToFillController( Application& application )
  : mApplication( application ),
    mScrolling( false )
  {
    std::cout << "ImageScalingScaleToFillController::ImageScalingScaleToFillController" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ImageScalingScaleToFillController::Create );
  }

  ~ImageScalingScaleToFillController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    std::cout << "ImageScalingScaleToFillController::Create" << std::endl;

    // Get a handle to the stage:
    Stage stage = Stage::GetCurrent();

    // Connect to input event signals:
    stage.KeyEventSignal().Connect(this, &ImageScalingScaleToFillController::OnKeyEvent);
    stage.GetRootLayer().TouchedSignal().Connect( this, &ImageScalingScaleToFillController::OnTouch );

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
    toggleScalingButton.ClickedSignal().Connect( this, &ImageScalingScaleToFillController::OnToggleScalingTouched );
    mToolBar.AddControl( toggleScalingButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    // Build the main content of the widow:
    PopulateContentLayer();
  }

  void PopulateContentLayer()
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    mScrollView = ScrollView::New();

    mScrollView.ScrollStartedSignal().Connect( this, &ImageScalingScaleToFillController::OnScrollStarted );
    mScrollView.ScrollCompletedSignal().Connect( this, &ImageScalingScaleToFillController::OnScrollCompleted );

    mScrollView.EnableScrollComponent( Scrollable::VerticalScrollBar );
    mScrollView.EnableScrollComponent( Scrollable::HorizontalScrollBar );

    mScrollView.SetAnchorPoint(AnchorPoint::CENTER);
    mScrollView.SetParentOrigin(ParentOrigin::CENTER);

    mScrollView.SetSize( stageSize );
    mScrollView.SetAxisAutoLock( true );
    mScrollView.SetAxisAutoLockGradient( 1.0f );

    // Restrict scrolling to mostly vertical only, but with some horizontal wiggle-room:

    RulerPtr rulerX = new FixedRuler( stageSize.width );
    rulerX->SetDomain( RulerDomain( stageSize.width * -0.125f, stageSize.width * 1.125f ) );
    mScrollView.SetRulerX ( rulerX );

    RulerPtr rulerY = new FixedRuler( stageSize.height );
    rulerY->SetDomain( RulerDomain( stageSize.height * -2, stageSize.height * 3 ) );
    mScrollView.SetRulerY ( rulerY );


    mContentLayer.Add( mScrollView );

    Actor imageField = BuildImageField( stageSize, GRID_WIDTHS[0], GRID_HEIGHT );

    mScrollView.Add( imageField );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    return true;
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
    // Rebuild stage with scaling on or off.
    ///@ToDo: <----------------------------------------------------------------------------------------------------------<<<
    return true;
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
   * note this state (mScrolling = false)
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
  ScrollView mScrollView;             ///< ScrollView UI Component
  bool mScrolling;                   ///< ScrollView scrolling state (true = scrolling, false = stationary)
  ScrollViewEffect mScrollViewEffect; ///< ScrollView Effect instance.
  Actor mScrollViewContainer;         ///< Container Sprite within ScrollView.
};

void RunTest( Application& application )
{
  ImageScalingScaleToFillController test( application );

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
