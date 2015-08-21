/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <sstream>

// INTERNAL INCLUDES
#include "shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/images/atlas.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char * const BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-default.png" );
const char * const TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char * const APPLICATION_TITLE( "ScrollView" );
const char * const EFFECT_CAROUSEL_IMAGE( DALI_IMAGE_DIR "icon-scroll-view-carousel.png" );
const char * const EFFECT_CAROUSEL_IMAGE_SELECTED( DALI_IMAGE_DIR "icon-scroll-view-carousel-selected.png" );

const Vector3 ICON_SIZE(100.0f, 100.0f, 0.0f);

const char* EFFECT_MODE_NAME[] = {
    "PageCarousel",
    "PageCube",
    "PageSpiral",
    "PageWave"
};

const char * const IMAGE_PATHS[] = {
    DALI_IMAGE_DIR "gallery-medium-1.jpg",
    DALI_IMAGE_DIR "gallery-medium-2.jpg",
    DALI_IMAGE_DIR "gallery-medium-3.jpg",
    DALI_IMAGE_DIR "gallery-medium-4.jpg",
    DALI_IMAGE_DIR "gallery-medium-5.jpg",
    DALI_IMAGE_DIR "gallery-medium-6.jpg",
    DALI_IMAGE_DIR "gallery-medium-7.jpg",
    DALI_IMAGE_DIR "gallery-medium-8.jpg",
    DALI_IMAGE_DIR "gallery-medium-9.jpg",
    DALI_IMAGE_DIR "gallery-medium-10.jpg",
    DALI_IMAGE_DIR "gallery-medium-11.jpg",
    DALI_IMAGE_DIR "gallery-medium-12.jpg",
    DALI_IMAGE_DIR "gallery-medium-13.jpg",
    DALI_IMAGE_DIR "gallery-medium-14.jpg",
    DALI_IMAGE_DIR "gallery-medium-15.jpg",
    DALI_IMAGE_DIR "gallery-medium-16.jpg",
    DALI_IMAGE_DIR "gallery-medium-17.jpg",
    DALI_IMAGE_DIR "gallery-medium-18.jpg",
    DALI_IMAGE_DIR "gallery-medium-19.jpg",
    DALI_IMAGE_DIR "gallery-medium-20.jpg",
    DALI_IMAGE_DIR "gallery-medium-21.jpg",
    DALI_IMAGE_DIR "gallery-medium-22.jpg",
    DALI_IMAGE_DIR "gallery-medium-23.jpg",
    DALI_IMAGE_DIR "gallery-medium-24.jpg",
    DALI_IMAGE_DIR "gallery-medium-25.jpg",
    DALI_IMAGE_DIR "gallery-medium-26.jpg",
    DALI_IMAGE_DIR "gallery-medium-27.jpg",
    DALI_IMAGE_DIR "gallery-medium-28.jpg",
    DALI_IMAGE_DIR "gallery-medium-29.jpg",
    DALI_IMAGE_DIR "gallery-medium-30.jpg",
    DALI_IMAGE_DIR "gallery-medium-31.jpg",
    DALI_IMAGE_DIR "gallery-medium-32.jpg",
    DALI_IMAGE_DIR "gallery-medium-33.jpg",
    DALI_IMAGE_DIR "gallery-medium-34.jpg",
    DALI_IMAGE_DIR "gallery-medium-35.jpg",
    DALI_IMAGE_DIR "gallery-medium-36.jpg",
    DALI_IMAGE_DIR "gallery-medium-37.jpg",
    DALI_IMAGE_DIR "gallery-medium-38.jpg",
    DALI_IMAGE_DIR "gallery-medium-39.jpg",
    DALI_IMAGE_DIR "gallery-medium-40.jpg",
    DALI_IMAGE_DIR "gallery-medium-41.jpg",
    DALI_IMAGE_DIR "gallery-medium-42.jpg",
    DALI_IMAGE_DIR "gallery-medium-43.jpg",
    DALI_IMAGE_DIR "gallery-medium-44.jpg",
    DALI_IMAGE_DIR "gallery-medium-45.jpg",
    DALI_IMAGE_DIR "gallery-medium-46.jpg",
    DALI_IMAGE_DIR "gallery-medium-47.jpg",
    DALI_IMAGE_DIR "gallery-medium-48.jpg",
    DALI_IMAGE_DIR "gallery-medium-49.jpg",
    DALI_IMAGE_DIR "gallery-medium-50.jpg",
    DALI_IMAGE_DIR "gallery-medium-51.jpg",
    DALI_IMAGE_DIR "gallery-medium-52.jpg",
    DALI_IMAGE_DIR "gallery-medium-53.jpg",

    NULL
};

const char * const GetNextImagePath()
{
  static const char * const * imagePtr = &IMAGE_PATHS[0];

  if ( *(++imagePtr) == NULL )
  {
    imagePtr = &IMAGE_PATHS[0];
  }

  return *imagePtr;
}

const int PAGE_COLUMNS = 10;                                                ///< Number of Pages going across (columns)
const int PAGE_ROWS = 1;                                                    ///< Number of Pages going down (rows)
const int IMAGE_ROWS = 5;                                                   ///< Number of Images going down (rows) with a Page

const unsigned int IMAGE_THUMBNAIL_WIDTH  = 256;                            ///< Width of Thumbnail Image in texels
const unsigned int IMAGE_THUMBNAIL_HEIGHT = 256;                            ///< Height of Thumbnail Image in texels

const float SPIN_DURATION = 1.0f;                                           ///< Times to spin an Image by upon touching, each spin taking a second.

const float EFFECT_SNAP_DURATION(0.66f);                                    ///< Scroll Snap Duration for Effects
const float EFFECT_FLICK_DURATION(0.5f);                                    ///< Scroll Flick Duration for Effects


class AtlasGridManager
{
public:

  AtlasGridManager()
  :mFirstFreeCell(0),
   mCellSize(Dali::Vector2::ZERO),
   mRows(0),
   mColumns(0),
   mCellCount(0)
  {}

  AtlasGridManager( Dali::Vector2 cellSize, unsigned int rows, unsigned int columns, Pixel::Format format )
  :mFirstFreeCell(0),
   mCellSize(cellSize),
   mRows(rows),
   mColumns(columns),
   mCellCount( rows * columns )
  {
     //Initialize free list
     mCells.Resize( mCellCount );
     for( unsigned int i(0); i<mCellCount; ++i )
     {
        mCells[i] = i+1;
     }

     //Create the atlas
     mAtlas = Atlas::New( (unsigned int)cellSize.x * columns, (unsigned int)cellSize.y * rows, format );
  }

  ~AtlasGridManager()
  {}

  /**
   * @brief Add a new BufferImage to the atlas
   *
   * @param[in] image The image to be added
   * @param[out] The region in the atlas where the image has been added
   *
   * @return true if image correctly added to the atlas, false otherwise
   */
  bool AddImage( BufferImage image, Dali::Rect<int>& region)
  {
    if( AllocateNewImage( region ) )
    {
      return mAtlas.Upload( image, region.x, region.y );
    }
    else
    {
      //Atlas is full
      return false;
    }
  }

  /**
   * @brief Add a new ResourceImage to the atlas
   *
   * @param[in] imageUrl url of the image to be added
   * @param[out] The region in the atlas where the image has been added
   *
   * @return true if image correctly added to the atlas, false otherwise
   */
  bool AddImage( const char* imageUrl, Dali::Rect<int>& region)
  {
    if( AllocateNewImage( region ) )
    {
      return mAtlas.Upload( imageUrl, region.x, region.y );
    }
    else
    {
      //Atlas is full
      return false;
    }
  }

  /**
   * @brief Remove image at location specified by region
   *
   * @param[in] region The region in the atlas where the image resides
   */
  void RemoveImage( Dali::Rect<int> region )
  {
    //Get cell from region
    unsigned int column = region.x / mCellSize.x;
    unsigned int row = region.y / mCellSize.y;

    unsigned int cell = row * mColumns + column;
    mCells[cell] = mFirstFreeCell;
    mFirstFreeCell = cell;
  }

  /**
   * Get the Atlas
   */
  Dali::Atlas GetAtlas()
  {
     return mAtlas;
  }

private:

  /**
   * Helper function to allocate an image in the atlas
   */
  bool AllocateNewImage( Dali::Rect<int>& region )
  {
    if( mFirstFreeCell < mCellCount )
    {
      unsigned int allocatedCell = mFirstFreeCell;
      unsigned int row = allocatedCell / mColumns;
      unsigned int column = allocatedCell % mColumns;

      //Upload the image to the atlas
      region = Dali::Rect<int>(column * mCellSize.x,row * mCellSize.y,mCellSize.x, mCellSize.y);

      //Update first free cell
      mFirstFreeCell = mCells[allocatedCell];
      return true;
    }
    else
    {
      //Atlas is full
      return false;
    }
  }

  Dali::Atlas mAtlas;

  //Free list of cells
  Dali::Vector<unsigned int> mCells;
  unsigned int mFirstFreeCell;

  Dali::Vector2 mCellSize;
  unsigned int mRows;
  unsigned int mColumns;
  unsigned int mCellCount;
};

} // unnamed namespace

/**
 * This example shows how to do custom Scroll Effects
 */
class ExampleController : public ConnectionTracker
{
public:

  /**
   * Constructor
   * @param application class, stored as reference
   */
  ExampleController( Application& application, bool useAtlas )
  : mApplication( application ),
    mView(),
    mScrolling(false),
    mEffectMode(PageCarouselEffect),
    mUseAtlas(useAtlas),
    mAtlasManager(0)
  {
    // Connect to the Application's Init and orientation changed signal
    mApplication.InitSignal().Connect(this, &ExampleController::OnInit);
  }

  ~ExampleController()
  {
    delete mAtlasManager;
  }

  /**
   * This method gets called once the main loop of application is up and running
   */
  void OnInit(Application& app)
  {
    Stage stage = Dali::Stage::GetCurrent();
    stage.KeyEventSignal().Connect(this, &ExampleController::OnKeyEvent);

    if( mUseAtlas )
    {
      Vector2 stageSize = stage.GetSize();
      unsigned int columns = round(IMAGE_ROWS * (stageSize.x / stage.GetDpi().x) / (stageSize.y / stage.GetDpi().y)) * PAGE_COLUMNS;
      mAtlasManager = new AtlasGridManager( Vector2(IMAGE_THUMBNAIL_WIDTH,IMAGE_THUMBNAIL_HEIGHT), IMAGE_ROWS, columns , Pixel::RGB888 );
    }

    // Hide the indicator bar
    mApplication.GetWindow().ShowIndicator(Dali::Window::INVISIBLE);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( app,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            "" );

    mEffectIcon[ PageCarouselEffect ] = EFFECT_CAROUSEL_IMAGE;
    mEffectIconSelected[ PageCarouselEffect ] = EFFECT_CAROUSEL_IMAGE_SELECTED;
    mEffectIcon[ PageCubeEffect ]     = EFFECT_CAROUSEL_IMAGE;
    mEffectIconSelected[ PageCubeEffect ]     = EFFECT_CAROUSEL_IMAGE_SELECTED;
    mEffectIcon[ PageSpiralEffect ]   = EFFECT_CAROUSEL_IMAGE;
    mEffectIconSelected[ PageSpiralEffect ]   = EFFECT_CAROUSEL_IMAGE_SELECTED;
    mEffectIcon[ PageWaveEffect ]     = EFFECT_CAROUSEL_IMAGE;
    mEffectIconSelected[ PageWaveEffect ]     = EFFECT_CAROUSEL_IMAGE_SELECTED;

    // Create a effect change button. (right of toolbar)
    mEffectChangeButton = Toolkit::PushButton::New();
    mEffectChangeButton.ClickedSignal().Connect( this, &ExampleController::OnEffectTouched );
    mToolBar.AddControl( mEffectChangeButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    // Create the content layer.
    AddContentLayer();

    // Hack to force screen refresh.
    Animation animation = Animation::New(1.0f);
    animation.AnimateTo(Property(mContentLayer, Actor::Property::POSITION), Vector3::ZERO );
    animation.Play();
  }

private:

  /**
   * Adds content to the ContentLayer. This is everything we see
   * excluding the toolbar at the top.
   */
  void AddContentLayer()
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    mScrollView = ScrollView::New();
    mScrollView.SetAnchorPoint(AnchorPoint::CENTER);
    mScrollView.SetParentOrigin(ParentOrigin::CENTER);
    mContentLayer.Add( mScrollView );
    mScrollView.SetSize( stageSize );
    mScrollView.SetAxisAutoLock( true );
    mScrollView.SetAxisAutoLockGradient( 1.0f );

    mScrollView.ScrollStartedSignal().Connect( this, &ExampleController::OnScrollStarted );
    mScrollView.ScrollCompletedSignal().Connect( this, &ExampleController::OnScrollCompleted );

    for(int row = 0;row<PAGE_ROWS;row++)
    {
      for(int column = 0;column<PAGE_COLUMNS;column++)
      {
        Actor page = CreatePage();

        page.SetPosition( column * stageSize.x, row * stageSize.y );
        mScrollView.Add( page );

        mPages.push_back(page);
      }
    }

    Update();
  }

  /**
   * Updates the ScrollView and it's children based
   * on the current effect.
   */
  void Update()
  {
    std::stringstream ss(APPLICATION_TITLE);
    ss << APPLICATION_TITLE << ": " << EFFECT_MODE_NAME[mEffectMode];
    SetTitle(ss.str());

    mEffectChangeButton.SetUnselectedImage( mEffectIcon[ mEffectMode ] );
    mEffectChangeButton.SetSelectedImage( mEffectIconSelected[ mEffectMode ] );

    // remove old Effect if exists.
    if(mScrollViewEffect)
    {
      mScrollView.RemoveEffect(mScrollViewEffect);
    }

    // apply new Effect to ScrollView
    ApplyEffectToScrollView();
    unsigned int pageCount(0);
    for( std::vector< Actor >::iterator pageIter = mPages.begin(); pageIter != mPages.end(); ++pageIter)
    {
      Actor page = *pageIter;
      ApplyEffectToPage( page, pageCount++ );
    }
  }

  /**
   * Creates a page using a source of images.
   */
  Actor CreatePage()
  {
    Actor page = Actor::New();
    page.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    page.SetParentOrigin( ParentOrigin::CENTER );
    page.SetAnchorPoint( AnchorPoint::CENTER );

    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    const float margin = 10.0f;

    // Calculate the number of images going across (columns) within a page, according to the screen resolution and dpi.
    int imageColumns = round(IMAGE_ROWS * (stageSize.x / stage.GetDpi().x) / (stageSize.y / stage.GetDpi().y));
    const Vector3 imageSize((stageSize.x / imageColumns) - margin, (stageSize.y / IMAGE_ROWS) - margin, 0.0f);

    for(int row = 0;row<IMAGE_ROWS;row++)
    {
      for(int column = 0;column<imageColumns;column++)
      {
        ImageActor actor;
        if( mUseAtlas )
        {
          actor = CreateImageInAtlas( GetNextImagePath(), imageSize.x, imageSize.y  );
        }
        else
        {
          actor = CreateImage( GetNextImagePath(), imageSize.x, imageSize.y );
        }

        actor.SetParentOrigin( ParentOrigin::CENTER );
        actor.SetAnchorPoint( AnchorPoint::CENTER );

        Vector3 position( margin * 0.5f + (imageSize.x + margin) * column - stageSize.width * 0.5f,
                         margin * 0.5f + (imageSize.y + margin) * row - stageSize.height * 0.5f,
                          0.0f);
        actor.SetPosition( position + imageSize * 0.5f );
        actor.SetSize( imageSize );
        page.Add(actor);
      }
    }

    return page;
  }

  /**
   * [ScrollView]
   * Applies effect to scrollView
   */
  void ApplyEffectToScrollView()
  {
    bool wrap(true);
    bool snap(true);

    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    RulerPtr rulerX = CreateRuler(snap ? stageSize.width : 0.0f);
    RulerPtr rulerY = new DefaultRuler;
    rulerX->SetDomain(RulerDomain(0.0f, stageSize.x * PAGE_COLUMNS, !wrap));
    rulerY->Disable();

    Dali::Path path = Dali::Path::New();
    Dali::Property::Array points;
        points.Resize(3);
    Dali::Property::Array controlPoints;
        controlPoints.Resize(4);
    Vector3 forward;
    if( mEffectMode == PageCarouselEffect)
    {

      points[0] = Vector3( stageSize.x*0.75, 0.0f,  -stageSize.x*0.75f);
      points[1] = Vector3( 0.0f, 0.0f, 0.0f );
      points[2] = Vector3( -stageSize.x*0.75f, 0.0f,  -stageSize.x*0.75f);
      path.SetProperty( Path::Property::POINTS, points );

      controlPoints[0] = Vector3( stageSize.x*0.5f, 0.0f, 0.0f );
      controlPoints[1] = Vector3( stageSize.x*0.5f, 0.0f, 0.0f );
      controlPoints[2] = Vector3(-stageSize.x*0.5f, 0.0f, 0.0f );
      controlPoints[3] = Vector3(-stageSize.x*0.5f, 0.0f, 0.0f );
      path.SetProperty( Path::Property::CONTROL_POINTS, controlPoints );

      forward = Vector3::ZERO;
    }
    else if( mEffectMode == PageCubeEffect)
    {
      points[0] = Vector3( stageSize.x*0.5, 0.0f,  stageSize.x*0.5f);
      points[1] = Vector3( 0.0f, 0.0f, 0.0f );
      points[2] = Vector3( -stageSize.x*0.5f, 0.0f, stageSize.x*0.5f);
      path.SetProperty( Path::Property::POINTS, points );

      controlPoints[0] = Vector3( stageSize.x*0.5f, 0.0f, stageSize.x*0.3f );
      controlPoints[1] = Vector3( stageSize.x*0.3f, 0.0f, 0.0f );
      controlPoints[2] = Vector3(-stageSize.x*0.3f, 0.0f, 0.0f );
      controlPoints[3] = Vector3(-stageSize.x*0.5f, 0.0f,  stageSize.x*0.3f );
      path.SetProperty( Path::Property::CONTROL_POINTS, controlPoints );

      forward = Vector3(-1.0f,0.0f,0.0f);
    }
    else if( mEffectMode == PageSpiralEffect)
    {
      points[0] = Vector3( stageSize.x*0.5, 0.0f,  -stageSize.x*0.5f);
      points[1] = Vector3( 0.0f, 0.0f, 0.0f );
      points[2] = Vector3( -stageSize.x*0.5f, 0.0f, -stageSize.x*0.5f);
      path.SetProperty( Path::Property::POINTS, points );

      controlPoints[0] = Vector3( stageSize.x*0.5f, 0.0f, 0.0f );
      controlPoints[1] = Vector3( stageSize.x*0.5f, 0.0f, 0.0f );
      controlPoints[2] = Vector3(-stageSize.x*0.5f, 0.0f, 0.0f );
      controlPoints[3] = Vector3(-stageSize.x*0.5f, 0.0f, 0.0f );
      path.SetProperty( Path::Property::CONTROL_POINTS, controlPoints );

      forward = Vector3(-1.0f,0.0f,0.0f);
    }
    else if( mEffectMode == PageWaveEffect)
    {
      points[0] = Vector3( stageSize.x, 0.0f,  -stageSize.x);
      points[1] = Vector3( 0.0f, 0.0f, 0.0f );
      points[2] = Vector3( -stageSize.x, 0.0f, -stageSize.x);
      path.SetProperty( Path::Property::POINTS, points );

      controlPoints[0] = Vector3( 0.0f, 0.0f, -stageSize.x );
      controlPoints[1] = Vector3( stageSize.x*0.5f, 0.0f, 0.0f );
      controlPoints[2] = Vector3( -stageSize.x*0.5f, 0.0f, 0.0f);
      controlPoints[3] = Vector3(0.0f, 0.0f,-stageSize.x  );
      path.SetProperty( Path::Property::CONTROL_POINTS, controlPoints );

      forward = Vector3(-1.0f,0.0f,0.0f);
    }

    mScrollViewEffect = ScrollViewPagePathEffect::New(path, forward,Toolkit::ScrollView::Property::SCROLL_FINAL_X, Vector3(stageSize.x,stageSize.y,0.0f),PAGE_COLUMNS);
    mScrollView.SetScrollSnapDuration(EFFECT_SNAP_DURATION);
    mScrollView.SetScrollFlickDuration(EFFECT_FLICK_DURATION);
    mScrollView.SetScrollSnapAlphaFunction(AlphaFunction::EASE_OUT);
    mScrollView.SetScrollFlickAlphaFunction(AlphaFunction::EASE_OUT);
    mScrollView.RemoveConstraintsFromChildren();

    rulerX = CreateRuler(snap ? stageSize.width * 0.5f : 0.0f);
    if( wrap )
    {
      rulerX->SetDomain(RulerDomain(0.0f, stageSize.x * 0.5f * PAGE_COLUMNS, !wrap));
    }
    else
    {
      rulerX->SetDomain(RulerDomain(0.0f, stageSize.x*0.5f* (PAGE_COLUMNS+1), !wrap));
    }

    unsigned int currentPage = mScrollView.GetCurrentPage();
    if( mScrollViewEffect )
    {
      mScrollView.ApplyEffect(mScrollViewEffect);
    }

    mScrollView.SetWrapMode(wrap);
    mScrollView.SetRulerX( rulerX );
    mScrollView.SetRulerY( rulerY );

    mScrollView.ScrollTo( currentPage, 0.0f );
  }

  /**
   * Creates a Ruler that snaps to a specified grid size.
   * If that grid size is 0.0 then this ruler does not
   * snap.
   *
   * @param[in] gridSize (optional) The grid size for the ruler,
   * (Default = 0.0 i.e. no snapping)
   * @return The ruler is returned.
   */
  RulerPtr CreateRuler(float gridSize = 0.0f)
  {
    if(gridSize <= Math::MACHINE_EPSILON_0)
    {
        return new DefaultRuler();
    }
    return new FixedRuler(gridSize);
  }
  // end switch
  /**
    * [Page]
    * Applies effect to the pages within scroll view.
    *
    * @param[in] page The page Actor to apply effect to.
    */
   void ApplyEffectToPage(Actor page, unsigned int pageOrder )
   {
     page.RemoveConstraints();
     page.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

     ScrollViewPagePathEffect effect = ScrollViewPagePathEffect::DownCast( mScrollViewEffect );
     effect.ApplyToPage( page, pageOrder );
   }

  /**
   * Creates an Image in the atlas(Helper)
   *
   * @param[in] filename the path of the image.
   * @param[in] width the width of the image in texels
   * @param[in] height the height of the image in texels.
   */
  ImageActor CreateImageInAtlas( const std::string& filename, unsigned int width = IMAGE_THUMBNAIL_WIDTH, unsigned int height = IMAGE_THUMBNAIL_HEIGHT )
  {
    ImageActor actor;
    Dali::Rect<int> region;
    if( mAtlasManager->AddImage( filename.c_str(), region ) )
    {
      actor = ImageActor::New(mAtlasManager->GetAtlas(), region );
      actor.SetName( filename );
      actor.SetParentOrigin(ParentOrigin::CENTER);
      actor.SetAnchorPoint(AnchorPoint::CENTER);

      actor.TouchedSignal().Connect( this, &ExampleController::OnTouchImage );
    }

    return actor;
  }

  /**
   * Creates an Image (Helper)
   *
   * @param[in] filename the path of the image.
   * @param[in] width the width of the image in texels
   * @param[in] height the height of the image in texels.
   */
  ImageActor CreateImage( const std::string& filename, unsigned int width = IMAGE_THUMBNAIL_WIDTH, unsigned int height = IMAGE_THUMBNAIL_HEIGHT )
  {
    Image img = ResourceImage::New(filename, ImageDimensions( width, height ), Dali::FittingMode::SCALE_TO_FILL, Dali::SamplingMode::BOX_THEN_LINEAR );
    ImageActor actor = ImageActor::New( img );
    actor.SetName( filename );
    actor.SetParentOrigin(ParentOrigin::CENTER);
    actor.SetAnchorPoint(AnchorPoint::CENTER);

    actor.TouchedSignal().Connect( this, &ExampleController::OnTouchImage );
    return actor;
  }

  /**
   * When scroll starts (i.e. user starts to drag scrollview),
   * note this state (mScrolling = true)
   * @param[in] position Current Scroll Position
   */
  void OnScrollStarted( const Vector2& position )
  {
    mScrolling = true;
  }

  /**
   * When scroll starts (i.e. user stops dragging scrollview, and scrollview has snapped to destination),
   * note this state (mScrolling = false)
   * @param[in] position Current Scroll Position
   */
  void OnScrollCompleted( const Vector2& position )
  {
    mScrolling = false;
  }

  /**
   * Upon Touching an image (Release), make it spin
   * (provided we're not scrolling).
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
        // Spin the Image a few times.
        Animation animation = Animation::New(SPIN_DURATION);
        animation.AnimateBy( Property( actor, Actor::Property::ORIENTATION ), Quaternion( Radian( Degree(360.0f * SPIN_DURATION) ), Vector3::XAXIS ), AlphaFunction::EASE_OUT );
        animation.Play();
      }
    }
    return false;
  }

  /**
   * Signal handler, called when the 'Effect' button has been touched.
   *
   * @param[in] button The button that was pressed.
   */
  bool OnEffectTouched(Button button)
  {
    mEffectMode = static_cast<EffectMode>((static_cast<int>(mEffectMode) + 1) % static_cast<int>(Total));
    Update();
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
      mTitleActor = DemoHelper::CreateToolBarLabel( "" );
      // Add title to the tool bar.
      mToolBar.AddControl( mTitleActor, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Alignment::HorizontalCenter );
    }

    mTitleActor.SetProperty( Toolkit::TextLabel::Property::TEXT, title );
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApplication.Quit();
      }
    }
  }

private:

  Application& mApplication;                            ///< Application instance
  Toolkit::Control mView;                               ///< The View instance.
  Toolkit::ToolBar mToolBar;                            ///< The View's Toolbar.
  TextLabel mTitleActor;                                ///< The Toolbar's Title.
  Layer mContentLayer;                                  ///< The content layer (contains game actors)
  ScrollView mScrollView;                               ///< ScrollView UI Component
  bool mScrolling;                                      ///< ScrollView scrolling state (true = scrolling, false = stationary)
  ScrollViewEffect mScrollViewEffect;                   ///< ScrollView Effect instance.
  std::vector< Actor > mPages;                          ///< Keeps track of all the pages for applying effects.



  /**
   * Enumeration of different effects this scrollview can operate under.
   */
  enum EffectMode
  {
    PageCarouselEffect,                                 ///< Page carousel effect
    PageCubeEffect,                                     ///< Page cube effect
    PageSpiralEffect,                                   ///< Page spiral effect
    PageWaveEffect,                                     ///< Page wave effect

    Total
  };

  EffectMode mEffectMode;                               ///< Current Effect mode

  std::string mEffectIcon[Total];                       ///< Icons for the effect button
  std::string mEffectIconSelected[Total];               ///< Icons for the effect button when its selected
  Toolkit::PushButton mEffectChangeButton;              ///< Effect Change Button

  bool mUseAtlas;                                       ///< Whether to use atlas for images or not
  AtlasGridManager*  mAtlasManager;                     ///< Atlas manager
};

int main(int argc, char **argv)
{
  bool useAtlas = false;
  for( int i(1) ; i < argc; ++i )
  {
    std::string arg( argv[i] );
    if( arg.compare("--use-atlas") == 0)
    {
      useAtlas = true;
    }
  }
  Application app = Application::New(&argc, &argv, DALI_DEMO_THEME_PATH );
  ExampleController test(app, useAtlas );
  app.MainLoop();
  return 0;
}
