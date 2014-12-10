//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES

// #include <cstdint>
namespace std
{
  typedef unsigned char uint8_t;
}

#include <sstream>
#include <iomanip>

// INTERNAL INCLUDES
#include "../shared/view.h"

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/integration-api/debug.h>

using namespace Dali;

namespace
{
  const char* const BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-default.png" );
  const char* const TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
  const char* const QUIT_IMAGE( DALI_IMAGE_DIR "icon-cluster-none.png" );
//  const char* APPLICATION_TITLE( "Save Pixmap To JPEG & PNG" );
  // Filenames of images saved to the filesystem:
  const char* const SAVED_PNG_FROM_24_BIT = "/tmp/pixmap-to-compressed-file-example.24bit.png";
  const char* const SAVED_PNG_FROM_32_BIT = "/tmp/pixmap-to-compressed-file-example.32bit.png";
  const char* const SAVED_JPG_FROM_24_BIT = "/tmp/pixmap-to-compressed-file-example.24bit.jpg";
  const char* const SAVED_JPG_FROM_32_BIT = "/tmp/pixmap-to-compressed-file-example.32bit.jpg";

  PixmapImagePtr CreatePixmapFrameBuffer(unsigned width, unsigned height, PixmapImage::ColorDepth depth, FrameBufferImage& framebuffer)
  {
    // Create the PixmapImage object with a fresh internal pixmap:
    PixmapImagePtr PixmapImage = PixmapImage::New(width, height, depth );

    // Bind a framebuffer object to the PixmapImage so we can make it the
    // target of rendering operations:
    framebuffer = FrameBufferImage::New( *PixmapImage );

    return PixmapImage;
  }

  extern "C" void EndTiming(int clock_ok, const timespec& start)
  {
    timespec end;
    clock_ok |= clock_gettime(CLOCK_MONOTONIC, &end);
    if(!clock_ok)
    {
      float delta_millis = (end.tv_sec  - start.tv_sec) / 1000.0  +
                           (end.tv_nsec - start.tv_nsec) / 1.e6;
      (void)delta_millis;
    }
  }

}

/*****************************************************************************/

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
  // These values depend on the tile image used in tile
  const float IMAGE_BORDER_LEFT = 2.0f;
  const float IMAGE_BORDER_RIGHT = 2.0f;
  const float IMAGE_BORDER_TOP = 2.0f;
  const float IMAGE_BORDER_BOTTOM = 2.0f;

  /** Create a labeled button for embedding in a grid. */
  Actor CreateTile( const char * const  title )
  {
    // create root actor for the tile
    Actor tile = Actor::New();
    tile.SetName( title );
    // anchor the tile from its center point to the middle of its parent
    tile.SetAnchorPoint( AnchorPoint::CENTER );
    tile.SetParentOrigin( ParentOrigin::CENTER );

    // create background image and an actor for it
    Image bg = Image::New(DALI_IMAGE_DIR "table-tile.png");
    ImageActor image = ImageActor::New( bg );
    // anchor image from its center point to the middle of its parent
    image.SetAnchorPoint( AnchorPoint::CENTER );
    image.SetParentOrigin( ParentOrigin::CENTER );
    // make the image 100% of tile
    image.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    // move image back to get text appear in front
    image.SetZ( -1 );
    // make image nine patch to avoid stretching borders
    image.SetStyle( ImageActor::STYLE_NINE_PATCH );
    image.SetNinePatchBorder( Vector4( IMAGE_BORDER_LEFT, IMAGE_BORDER_TOP, IMAGE_BORDER_RIGHT, IMAGE_BORDER_BOTTOM ) );
    // add image a child of tile
    tile.Add( image );

    // create the text actor
    TextView text = TextView::New( title );
    // anchor text from its center point to the middle of its parent
    text.SetAnchorPoint( AnchorPoint::CENTER );
    text.SetParentOrigin( ParentOrigin::CENTER );
    // align text to the middle
    text.SetTextAlignment( Alignment::Type( Alignment::HorizontalCenter | Alignment::VerticalCenter ) );
    text.SetColor( Color::WHITE );
    // move text a bit forwards
    text.SetZ( 1 );
    // make the text 90% of tile
    text.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), RelativeToConstraint( Vector3( 0.9f, 0.9f, 0.9f ) ) ) );
    tile.Add( text );

    return tile;
  }
}

/**
 * This example shows how a scene can be rendered into egl NativeImage
 * off-screen frame buffers and then saved to a file in a compressed
 * image format.
 *
 * To see logging output on a tizen device, in a second terminal connected to
 * it, run:
 *          dlogutil PIXMAP_TO_COMPRESSED:V DALI:V
 */
class TestApp : public ConnectionTracker, public Toolkit::ItemFactory
{
public:

  TestApp( Application &application )
    : mApplication( application ),
      mPixmapImage(0),
      mPixmapImage24(0),
      mPngFilename(SAVED_PNG_FROM_32_BIT),
      mJpgFilename(SAVED_JPG_FROM_32_BIT)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TestApp::Create );
    mApplication.TerminateSignal().Connect( this, &TestApp::Terminate );
  }

  ~TestApp()
  {
  }

public: // From ItemFactory

  virtual unsigned int GetNumberOfItems()
  {
    return 200;
  }

  virtual Actor NewItem(unsigned int itemId)
  {
    Actor actor;

    if (itemId < GetNumberOfItems())
    {
      std::stringstream str;
      str << "a_00" << std::setfill('0') << std::setw(2) << itemId % 34 << ".jpg";

      Image image = Image::New( std::string( DALI_IMAGE_DIR ) + str.str() );
      actor = ImageActor::New(image);
    }

    return actor;
  }

void BuildToolbar(Toolkit::ToolBar& toolBar)
{
  // quit button:
  Image image = Image::New( QUIT_IMAGE );
  Toolkit::PushButton quitButton = Toolkit::PushButton::New();
  quitButton.SetBackgroundImage( image );
  quitButton.ClickedSignal().Connect( this, &TestApp::OnQuitButtonClicked );
  toolBar.AddControl( quitButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalLeft );
}

TableView BuildButtons()
{
  // toolbar buttons on the bottom
  TableView buttons = TableView::New(1, 4);
  buttons.SetSizePolicy( Toolkit::Control::Fixed, Toolkit::Control::Fixed );
  buttons.SetRelativeHeight(0, 0.95f); // 95% of the table for first row
  // anchor the table from its bottom center point to the bottom center of its parent
  buttons.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  buttons.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
  // 1 pixel padding
  buttons.SetCellPadding( Size( 1.0f, 1.0f ) );
  // make the table 100% of root width, 10% of height, full depth
  buttons.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), RelativeToConstraint( Vector3( 1.0f, 0.1f, 1.0f ) ) ) );

  // create action buttons:

  // Buttons controlling framebuffer bit depthe:
  Actor jpegButton32 = CreateTile("Render\n32 bit");
  jpegButton32.TouchedSignal().Connect(this, &TestApp::OnRender32);
  buttons.Add(jpegButton32);

  Actor pngButton24 = CreateTile("Render\n24 bit");
  pngButton24.TouchedSignal().Connect(this, &TestApp::OnRender24);
  buttons.Add(pngButton24);

  // Button to save a 24 bpp image:
  Actor jpegButton24 = CreateTile("Save\nJPEG");
  jpegButton24.TouchedSignal().Connect(this, &TestApp::OnSavePixmapToJpeg);
  buttons.Add(jpegButton24);

  Actor pngButton32 = CreateTile("Save\nPNG");
  pngButton32.TouchedSignal().Connect(this, &TestApp::OnSavePixmapToPng);
  buttons.Add(pngButton32);


  return buttons;
}

public:

  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    // Hide the indicator bar
    application.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

    mContents = DemoHelper::CreateView( application,
                                        mView,
                                        mToolBar,
                                        BACKGROUND_IMAGE,
                                        TOOLBAR_IMAGE,
                                        "" );
    assert(mContents);
    if(!mContents){
      exit(-1);
    }
    BuildToolbar(mToolBar);

    // Create a layer above the scene for the buttons to sit in:
    Dali::Layer guiLayer = Dali::Layer::New();
    guiLayer.SetAnchorPoint( Dali::AnchorPoint::CENTER );
    guiLayer.SetParentOrigin( Dali::ParentOrigin::CENTER );
    guiLayer.ApplyConstraint( Dali::Constraint::New<Dali::Vector3>( Dali::Actor::SIZE, Dali::ParentSource( Dali::Actor::SIZE ), Dali::EqualToConstraint() ) );
    mView.AddContentLayer( guiLayer );
    guiLayer.RaiseToTop();
    guiLayer.Add(BuildButtons());

    // Setup render tasks with frame buffers
    RenderTaskList taskList = stage.GetRenderTaskList();
    RenderTask firstTask = taskList.GetTask( 0u );

    mPixmapImage = CreatePixmapFrameBuffer(stageSize.width, stageSize.height, PixmapImage::COLOR_DEPTH_32, mFrameBuffer1);
    mPixmapImage24 = CreatePixmapFrameBuffer(stageSize.width, stageSize.height, PixmapImage::COLOR_DEPTH_24, mFrameBuffer24);
    mCurrentPixmapImage = mPixmapImage;

    // first task to render the view to the FrameBuffer
    firstTask.SetSourceActor( mView );
    firstTask.SetScreenToFrameBufferFunction( RenderTask::FULLSCREEN_FRAMEBUFFER_FUNCTION );
    firstTask.SetClearColor( Vector4( 0, 0, 0, 0 ) );
    firstTask.SetClearEnabled( true );
    firstTask.SetTargetFrameBuffer( mFrameBuffer1 );

    // second task to render ImageActor with FrameBuffer to the screen
    mImageActor1 = ImageActor::New( mFrameBuffer1 );
    mImageActor1.SetParentOrigin( ParentOrigin::CENTER );
    mImageActor1.SetSize( stageSize.x, stageSize.y );
    mImageActor1.ScaleBy( Vector3(1.0f, -1.0f, 1.0f) );

    stage.Add( mImageActor1 ); // Not part of view

    mImageActor24 = ImageActor::New( mFrameBuffer24 );
    mImageActor24.SetParentOrigin(ParentOrigin::CENTER);
    mImageActor24.SetSize( stageSize.x, stageSize.y );
    mImageActor24.ScaleBy( Vector3(1.0f, -1.0f, 1.0f) );

    mSecondTask = taskList.CreateTask();
    mSecondTask.SetSourceActor( mImageActor1 );
    mSecondTask.SetInputEnabled( false );

    // Create an ItemView
    mContents.Add(CreateItemView(stageSize.width, stageSize.height));
  }

  bool OnRender24( Actor actor, const TouchEvent& event )
  {
    const TouchPoint& point = event.GetPoint( 0 );
    if( TouchPoint::Up == point.state )
    {
      Stage stage = Stage::GetCurrent();
      RenderTaskList taskList = stage.GetRenderTaskList();
      RenderTask firstTask = taskList.GetTask( 0u );
      firstTask.SetTargetFrameBuffer( mFrameBuffer24 );
      mSecondTask.SetSourceActor( mImageActor24 );
      stage.Remove( mImageActor1 ); // Not part of view
      stage.Add( mImageActor24 ); // Not part of view
      mCurrentPixmapImage = mPixmapImage24;
      mPngFilename = SAVED_PNG_FROM_24_BIT;
      mJpgFilename = SAVED_JPG_FROM_24_BIT;

      return true;
    }
    return false;
  }

  bool OnRender32( Actor actor, const TouchEvent& event )
  {
    const TouchPoint& point = event.GetPoint( 0 );
    if( TouchPoint::Up == point.state )
    {
      Stage stage = Stage::GetCurrent();
      RenderTaskList taskList = stage.GetRenderTaskList();
      RenderTask firstTask = taskList.GetTask( 0u );
      firstTask.SetTargetFrameBuffer( mFrameBuffer1 );
      mSecondTask.SetSourceActor( mImageActor1 );
      stage.Remove( mImageActor24 ); // Not part of view
      stage.Add( mImageActor1 ); // Not part of view
      mCurrentPixmapImage = mPixmapImage;
      mPngFilename = SAVED_PNG_FROM_32_BIT;
      mJpgFilename = SAVED_JPG_FROM_32_BIT;

      return true;
    }
    return false;
  }

  bool OnSavePixmapToJpeg( Actor actor, const TouchEvent& event )
  {
    const TouchPoint& point = event.GetPoint( 0 );
    if( TouchPoint::Up == point.state )
    {
      timespec start;
      PixmapImage * const pmi = &(*mCurrentPixmapImage);
      int clock_ok = clock_gettime(CLOCK_MONOTONIC, &start);
      const bool encodeRes = pmi->EncodeToFile(mJpgFilename);
      EndTiming(clock_ok, start);
      return encodeRes;
    }
    return false;
  }

  bool OnSavePixmapToPng( Actor actor, const TouchEvent& event )
  {
    const TouchPoint& point = event.GetPoint( 0 );
    if( TouchPoint::Up == point.state )
    {
      timespec start;
      PixmapImage * const pmi = &(*mCurrentPixmapImage);
      int clock_ok = clock_gettime(CLOCK_MONOTONIC, &start);
      const bool encodeRes = pmi->EncodeToFile(mPngFilename);
      EndTiming(clock_ok, start);
      return encodeRes;
    }
    return false;
  }

  void Terminate( Application& application )
  {
  }

  Toolkit::ItemView CreateItemView( float width, float height )
  {
    // Create the itemview
    Toolkit::ItemView itemView = Toolkit::ItemView::New(*this);
    itemView.SetParentOrigin(ParentOrigin::TOP_CENTER);
    itemView.SetAnchorPoint(AnchorPoint::TOP_CENTER);

    // Activate a spiral layout
    Toolkit::SpiralLayoutPtr spiral = Toolkit::SpiralLayout::New();
    itemView.AddLayout(*spiral);
    itemView.ActivateLayout( 0u, Vector3(width, height, width), 0.0f );

    return itemView;
  }

  bool OnQuitButtonClicked( Toolkit::Button button )
  {
    mApplication.Quit();
    return true;
  }

private:

  Application& mApplication;
  Toolkit::View mView;
  Toolkit::ToolBar           mToolBar;                           ///< The View's Toolbar.
  Layer         mContents;
  RenderTask mSecondTask;
  // 32-bit offscreen framebuffer:
  FrameBufferImage mFrameBuffer1;
  PixmapImagePtr mPixmapImage;
  ImageActor mImageActor1;
  // 24-bit off-screen framebuffer:
  FrameBufferImage mFrameBuffer24;
  PixmapImagePtr mPixmapImage24;
  ImageActor mImageActor24;
  // Alias for the current render target:
  PixmapImagePtr mCurrentPixmapImage;
  // Current filenames for saving images to:
  const char * mPngFilename;
  const char * mJpgFilename;
};


/*****************************************************************************/
void RunTest( Application& application )
{
  TestApp theApp( application );
  application.MainLoop();
}

/*****************************************************************************/
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
