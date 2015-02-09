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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <string.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace std;

namespace
{
/**
 * Returns a random number between f0 and f1
 */
inline float Range(float f0, float f1)
{
  return f0 + (rand() & 0xfff) * (f1-f0) * (1.0f/4095.0f);
}

// These values depend on the tile image used in tile
const float IMAGE_BORDER_LEFT = 2.0f;
const float IMAGE_BORDER_RIGHT = 2.0f;
const float IMAGE_BORDER_TOP = 2.0f;
const float IMAGE_BORDER_BOTTOM = 2.0f;
}

/**
 * This example shows how to use TableView UI control
 * There is two tables, one in middle, which can be modified and a toolbar style
 * table on bottom with some commands
 * Both tables have size constrained relative to a parent actor
 * that is rotated according to device orientation
 * Middle table has a topic cell on top that spans multiple columns and a
 * border cell on right side which spans multiple rows.
 * Title row and border column have fixed size
 * There are buttons to add and delete rows as well as add and delete columns
 * Bottom table has buttons for Adding actors to free cells and removing actors
 * You can remove added actors by clicking them
 * There's also buttons for resetting the middle table and quitting the example
 */
class TestController : public ConnectionTracker
{
public:

  /**
   * Constructor
   * @param application class, stored as reference
   */
  TestController( Application& application )
  : mApplication( application ),
    mOrientation( 0 )
  {
    // Connect to the Application's Init and orientation changed signal
    mApplication.InitSignal().Connect(this, &TestController::OnInit);
  }

  ~TestController()
  {
    // Nothing to do here; table gets deleted automatically and it deletes its children
  }

  /**
   * This method gets called once the main loop of applicatin is up and running
   */
  void OnInit( Application& app )
  {
    Vector2 stageSize = Stage::GetCurrent().GetSize();
    // root actor to handle rotation
    mRootActor = Actor::New();
    // anchor the root from its center point to the middle of its parent
    mRootActor.SetAnchorPoint( AnchorPoint::CENTER );
    mRootActor.SetParentOrigin( ParentOrigin::CENTER );
    // root size is same as stage
    mRootActor.SetSize( stageSize );
    // add root to stage
    Stage::GetCurrent().Add( mRootActor );
    // The Init signal is received once (only) during the Application lifetime
    // create a 5x5 table
    mTableView = TableView::New( 5, 5 );
    mTableView.SetSizePolicy( Toolkit::Control::Fixed, Toolkit::Control::Fixed );
    // anchor the table from its center point to the middle of its parent
    mTableView.SetAnchorPoint( AnchorPoint::CENTER );
    mTableView.SetParentOrigin( ParentOrigin::CENTER );
    // 2 pixels of padding
    mTableView.SetCellPadding( Size( 2.0f, 2.0f ) );
    // make the table relative to root size, 90% of width, 75% of height and 100% of depth
    mTableView.SetSize( stageSize * 0.75f );
    // initialize the cells
    SetInitialCells();

    // add table to stage
    mRootActor.Add( mTableView );

    // toolbar buttons on the bottom
    mButtons = TableView::New( 1, 4 );
    mButtons.SetSizePolicy( Toolkit::Control::Fixed, Toolkit::Control::Fixed );
    mButtons.SetRelativeHeight( 0, 0.95f ); // 95% of the table for first row
    // anchor the table from its bottom center point to the bottom center of its parent
    mButtons.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    mButtons.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    // 1 pixel padding
    mButtons.SetCellPadding( Size( 1.0f, 1.0f ) );
    // make the table 100% of root width, 10% of height, full depth
    mButtons.SetSize( stageSize.x, stageSize.y * 0.1f );
    // create action buttons
    mButtons.Add( CreateTile( "Add" ) );
    mButtons.Add( CreateTile( "Remove" ) );
    mButtons.Add( CreateTile( "Reset" ) );
    mButtons.Add( CreateTile( "Quit" ) );
    // add to stage
    mRootActor.Add( mButtons );

    Dali::Window winHandle = app.GetWindow();
    winHandle.AddAvailableOrientation( Dali::Window::PORTRAIT );
    winHandle.AddAvailableOrientation( Dali::Window::LANDSCAPE );
    winHandle.AddAvailableOrientation( Dali::Window::PORTRAIT_INVERSE  );
    winHandle.AddAvailableOrientation( Dali::Window::LANDSCAPE_INVERSE );

    app.GetOrientation().ChangedSignal().Connect( this, &TestController::OrientationChanged );
  }

  /**
   * Sets the initial cells for the table
   */
  void SetInitialCells()
  {
    // add cells to table
    // the next code populated the table like below
    // +-------------------+-+ row
    // |  TableView demo   | |  0
    // +-------------------+ |
    // |+row|+col|+col|+col| |  1
    // +-------------------+ |
    // |+row|         |-row| |  2
    // +----|   New   |----+ |
    // |+row|         |-row| |  3
    // +-------------------+ |
    // |-col|-col|-col|-row| |  4
    // +-------------------+-+
    //    0    1    2    3  4  <- column
    // title cell first, spans 4 columns
    mTableView.AddChild( CreateTile( "TableView Demo" ), TableView::CellPosition( 0, 0, 1, 4 ) );
    // fixed height, 30 pixels
    mTableView.SetFixedHeight( 0, 30.0f );
    // right side border, top right and spans 5 rows
    mTableView.AddChild( CreateTile( "" ), TableView::CellPosition( 0, 4, 5, 1 ) );
    // fixed width, 15 pixels
    mTableView.SetFixedWidth( 4, 15.0f );

    // add the buttons to insert and delete rows and columns
    mTableView.AddChild( CreateTile( "+row" ), TableView::CellPosition( 1, 0 ) );
    mTableView.AddChild( CreateTile( "+col" ), TableView::CellPosition( 1, 1 ) );
    mTableView.AddChild( CreateTile( "+col" ), TableView::CellPosition( 1, 2 ) );
    mTableView.AddChild( CreateTile( "+col" ), TableView::CellPosition( 1, 3 ) );
    mTableView.AddChild( CreateTile( "+row" ), TableView::CellPosition( 2, 0 ) );
    mTableView.AddChild( CreateTile( "+row" ), TableView::CellPosition( 3, 0 ) );
    mTableView.AddChild( CreateTile( "-row" ), TableView::CellPosition( 2, 3 ) );
    mTableView.AddChild( CreateTile( "-row" ), TableView::CellPosition( 3, 3 ) );
    mTableView.AddChild( CreateTile( "-col" ), TableView::CellPosition( 4, 0 ) );
    mTableView.AddChild( CreateTile( "-col" ), TableView::CellPosition( 4, 1 ) );
    mTableView.AddChild( CreateTile( "-col" ), TableView::CellPosition( 4, 2 ) );
    mTableView.AddChild( CreateTile( "-row" ), TableView::CellPosition( 4, 3 ) );

    // put a removable spanning column in the middle
    mTableView.AddChild( CreateTile( "New" ), TableView::CellPosition( 2, 1, 2, 2 ) );

    // second and last row are 15% (insert and delete columns)
    mTableView.SetRelativeHeight( 1, 0.15f );
    mTableView.SetRelativeHeight( 4, 0.15f );

    // first and last column are 15% (insert and delete rows)
    mTableView.SetRelativeWidth( 0, 0.15f );
    mTableView.SetRelativeWidth( 3, 0.15f );
  }

  /**
   * Helper to create a tile for the table
   */
  Actor CreateTile( const string& title )
  {
    // create root actor for the tile
    Actor tile = Actor::New();
    tile.SetName( title );
    // anchor the tile from its center point to the middle of its parent
    tile.SetAnchorPoint( AnchorPoint::CENTER );
    tile.SetParentOrigin( ParentOrigin::CENTER );

    // create background image and an actor for it
    Image bg = Image::New(DALI_IMAGE_DIR "button-background.png");
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

    // connect to the touch events of the tile
    tile.TouchedSignal().Connect( this, &TestController::OnTilePressed );

    return tile;
  }

  /**
   * Touch signal callback
   * Handling of the tile press events
   */
  bool OnTilePressed( Actor actor, const TouchEvent& event )
  {
    // by default not consumed
    bool consumed = false;
    // only act on the up event
    const TouchPoint& point = event.GetPoint( 0 );
    if( TouchPoint::Up == point.state )
    {
      if( actor.GetName() == "Quit" )
      {
        // quit the application
        mApplication.Quit();
        consumed = true;
      }
      else if( actor.GetName() == "Reset" )
      {
        // resize to 0x0
        mTableView.Resize( 0, 0 );
        // add back the initial cells, adding cells to empty table increases its size
        // to required size
        SetInitialCells();
        consumed = true;
      }
      else if( actor.GetName() == "+row" )
      {
        // insert a row
        // find the pressed actor from the table
        TableView::CellPosition position;
        if( mTableView.FindChildPosition( actor, position ) )
        {
          mTableView.InsertRow( position.rowIndex );
        }
        // add buttons to insert and delete row to the new row
        mTableView.AddChild( CreateTile( "+row" ), TableView::CellPosition( position.rowIndex, 0 ) );
        mTableView.AddChild( CreateTile( "-row" ), TableView::CellPosition( position.rowIndex, mTableView.GetColumns() - 2 ) );
      }
      else if( actor.GetName() == "+col" )
      {
        // insert a column
        // find the pressed actor from the table
        TableView::CellPosition position;
        if( mTableView.FindChildPosition( actor, position ) )
        {
          mTableView.InsertColumn( position.columnIndex );
        }
        // add buttons to insert and delete column to the new column
        mTableView.AddChild( CreateTile( "+col" ), TableView::CellPosition( 1, position.columnIndex ) );
        mTableView.AddChild( CreateTile( "-col" ), TableView::CellPosition( mTableView.GetRows() - 1, position.columnIndex ) );
      }
      else if( actor.GetName() == "-row" )
      {
        // delete a row
        // find the pressed actor from the table
        TableView::CellPosition position;
        if( mTableView.FindChildPosition( actor, position ) )
        {
          mTableView.DeleteRow( position.rowIndex );
        }
      }
      else if( actor.GetName() == "-col" )
      {
        // delete a column
        // find the pressed actor from the table
        TableView::CellPosition position;
        if( mTableView.FindChildPosition( actor, position ) )
        {
          mTableView.DeleteColumn( position.columnIndex );
        }
      }
      else if( actor.GetName() == "Add" )
      {
        Actor tile = CreateTile( "New" );
        // random position on sides of the stage
        const Vector2 stage = Stage::GetCurrent().GetSize();
        const float randomX = Range( 0.0f, stage.width );
        const float randomY = Range( 0.0f, stage.height );
        float offSetX = 0.0f;
        float offSetY = 0.0f;
        if( randomX > (0.8f * stage.width ) )
        {
          offSetX = 0.8f * stage.width;
        }
        if( randomY > (0.8f * stage.height ) )
        {
          offSetY = 0.8f * stage.height;
        }
        // set actor position
        tile.SetPosition( offSetX + 0.2f * randomX, offSetY + 0.2f * randomY, 0.0f );
        // add the actor to the first free slot of the table
        // note we're using Actor::Add here so cannot specify location
        mTableView.Add( tile );
        consumed = true;
      }
      else if( actor.GetName() == "New" )
      {
        // remove the actor from table through Actor::Remove
        mTableView.Remove( actor );
        consumed = true;
      }
      else if( actor.GetName() == "Remove" )
      {
        // check if we find the child
        Actor child = mTableView.FindChildByName( "New" );
        if( child )
        {
          // try finding it from the table
          TableView::CellPosition position;
          if( mTableView.FindChildPosition( child, position ) )
          {
            // remove child at this position
            mTableView.RemoveChildAt( position );
          }
          else
          {
            // remove child through Actor::Remove
            mTableView.Remove( child );
          }
        }
        consumed = true;
      }
      else if( actor.GetParent() == mTableView )
      {
        // just consume
        consumed = true;
      }
      // if event was consumed, animate the tile press
      if( consumed )
      {
        // animation moves actor back and forwards
        mPressedAnimation = Animation::New( 0.5f );
        mPressedAnimation.SetEndAction( Animation::Discard );
        mPressedAnimation.MoveBy( actor, Vector3( 0, 0, -25.0f ), AlphaFunctions::EaseInOut, 0.0f, 0.25f );
        mPressedAnimation.MoveBy( actor, Vector3( 0, 0, 25.0f ), AlphaFunctions::EaseInOut, 0.25f, 0.25f );
        mPressedAnimation.Play();
      }
    }
  return consumed;
  }

  /**
   * Orientation changed signal callback
   * @param orientation
   */
  void OrientationChanged( Orientation orientation )
  {
    const int angle = orientation.GetDegrees();
    // if orientation really changed
    if( mOrientation != angle )
    {
      // remember orientation
      mOrientation = angle;

      // Get current stage size
      Vector2 stageSize = Stage::GetCurrent().GetSize();
      Vector3 targetSize(stageSize);
      // if landscape, swap width and height
      if( angle == 90 ||
          angle == 270 )
      {
        targetSize = Vector3( stageSize.height, stageSize.width, 0.0f );
      }

      // animate rotation and resizing of root actor, tables rotate and resize with it
      mRotateAnimation = Animation::New( 2.0f );
      mRotateAnimation.SetEndAction( Animation::Bake );
      mRotateAnimation.RotateTo( mRootActor, Degree( -angle ), Vector3::ZAXIS, AlphaFunctions::EaseOut );
      mRotateAnimation.Resize( mRootActor, targetSize );
      mRotateAnimation.Play();
    }
  }

private:

  Application& mApplication;
  int          mOrientation;
  Actor        mRootActor;
  Animation    mRotateAnimation;
  TableView    mTableView;
  TableView    mButtons;
  Animation    mPressedAnimation;

};

void RunTest(Application& app)
{
  TestController test(app);

  app.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv);

  RunTest(app);

  return 0;
}
