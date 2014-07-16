/*
Copyright (c) 2000-2014 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Demo

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

#include "../shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <sstream>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

// CONSTANTS

// Application constants
const char* BACKGROUND_IMAGE = DALI_IMAGE_DIR "background-gradient.jpg";

// Toolbar constants
const char* TOOLBAR_IMAGE = DALI_IMAGE_DIR "top-bar.png";
const char* APPLICATION_TITLE = "Image Samplers";
const char* FILTERMODE_TITLE = "Texture filter modes";
const char* WRAP_TITLE = "Texture wrap modes";

// Toolbar icons
const char* VIEW_SWAP_IMAGE = DALI_IMAGE_DIR "icon-change.png";
const char* MENU_ICON_IMAGE = DALI_IMAGE_DIR "icon-menu.png";
const char* HELP_ICON_IMAGE = DALI_IMAGE_DIR "icon-help.png";

const float TOOLBAR_BUTTON_SIZE = 48.0f;

// Image filter scene
const char* BUTTON_BACKGROUND_IMAGE = DALI_IMAGE_DIR "button-background.png";

const char* ITEM_BORDER_IMAGE_PATH( DALI_IMAGE_DIR "frame-128x128.png" );
const Vector3 ITEM_BORDER_MARGIN_SIZE(24, 24, 0);

const float ITEM_IMAGE_BORDER_LEFT   = 13.0f;
const float ITEM_IMAGE_BORDER_RIGHT  = 13.0f;
const float ITEM_IMAGE_BORDER_TOP    = 13.0f;
const float ITEM_IMAGE_BORDER_BOTTOM = 13.0f;

const float MENU_OPTION_HEIGHT = 50.0f;

const char* FILTER_HELP_TEXT = "Displays images using texture filtering. The labels under the images indicate which modes are in effect (Minify, Magnify). "
                               "When a filter mode has minify as None then the GL system default mode with mip mapping will be used. This will result in a black texture.\n\n"
                               "Images can be scaled using the bottom slider and cycled using the swap image toolbar button. "
                               "Cyling through images will also display a text example.";


// Image filtering tests
const int NUM_IMAGE_FILTER_TESTS = 9;

const char* TEST_NAMES[] = { "Nearest, Nearest",
                             "Nearest, Linear",
                             "Linear, Linear",
                             "Linear, Nearest",
                             "Default, Default",
                             "None, None",
                             "Default, None",
                             "None, Default",
                             "Unspecified" };

const unsigned int NUM_TEST_NAMES = sizeof( TEST_NAMES ) / sizeof( TEST_NAMES[0] );

const FilterMode::Type MIN_FILTER_MODES[] = { FilterMode::NEAREST,
                                              FilterMode::NEAREST,
                                              FilterMode::LINEAR,
                                              FilterMode::LINEAR,
                                              FilterMode::DEFAULT,
                                              FilterMode::NONE,
                                              FilterMode::DEFAULT,
                                              FilterMode::NONE };

const FilterMode::Type MAG_FILTER_MODES[] = { FilterMode::NEAREST,
                                              FilterMode::LINEAR,
                                              FilterMode::LINEAR,
                                              FilterMode::NEAREST,
                                              FilterMode::DEFAULT,
                                              FilterMode::NONE,
                                              FilterMode::NONE,
                                              FilterMode::DEFAULT };

const unsigned int NUM_FILTER_MODES = sizeof( MIN_FILTER_MODES ) / sizeof( MIN_FILTER_MODES[0] );

const char* TEST_IMAGES[] = { DALI_IMAGE_DIR "blocks-ball.png",
                              DALI_IMAGE_DIR "icon-item-view-layout-depth.png",
                              DALI_IMAGE_DIR "blocks-brick-3.png",
                              DALI_IMAGE_DIR "gallery-medium-10.jpg",
                              DALI_IMAGE_DIR "gallery-small-9.jpg",
                              DALI_IMAGE_DIR "gallery-small-47.jpg" };

const unsigned int NUM_TEST_IMAGES = sizeof( TEST_IMAGES ) / sizeof( TEST_IMAGES[0] );

// Wrap mode scene
const Vector3 BACK_POINT( -0.5f,-0.5f, 0.0f );
const Vector3 LEFT_POINT(-0.5f, 0.5f, 0.0f );
const Vector3 RIGHT_POINT( 0.5f, -0.5f, 0.0f );
const Vector3 TOP_POINT( 0.5f, 0.5f, 0.0f );

const float TEST_IMAGE_WIDTH  = 128.0f;
const float TEST_IMAGE_HEIGHT = 128.0f;

const char* TEST_IMAGE_FILENAME2 = DALI_IMAGE_DIR "gallery-small-42.jpg";

} // namespace


// CONSTRAINTS

/**
 * Constraint to resize and image with a border
 */
struct BorderSizeConstraintFunction
{
  /**
   * @brief Construct the constraint
   *
   * @param[in] itemBorderMarginSize The extra size to expand parent size by
   */
  BorderSizeConstraintFunction( const Vector3& itemBorderMarginSize )
    : mItemBorderMarginSize( itemBorderMarginSize )
  {
  }

  /**
   * @brief Function operator
   *
   * @param[in] current The current size of the actor
   * @param[in] parentSize The size of the parent
   * @return Return the expanded size
   */
  Vector3 operator()(const Vector3&       current,
                     const PropertyInput& parentSize)
  {
    return parentSize.GetVector3() + mItemBorderMarginSize;
  }

private:

  Vector3 mItemBorderMarginSize;    ///< Expand the size by this amount
};

/**
 * @brief Adjust the size of the actor so that it is essentially clipped to it's parent's dimensions
 *
 * @param[in] current The current height of the actor
 * @param[in] layerWorldPositionYProperty The world Y position of the layer to clip against
 * @param[in] layerHeightProperty The height of the layer to clip against
 * @param[in] parentWorldPositionYProperty The world Y position of this actor's parent
 * @param[in] parentHeightProperty The height of this actor's parent
 */
float StencilUnionHeightConstraint(const float& current,
                                   const PropertyInput& layerWorldPositionYProperty,
                                   const PropertyInput& layerHeightProperty,
                                   const PropertyInput& parentWorldPositionYProperty,
                                   const PropertyInput& parentHeightProperty )
{
  float layerWorldPositionY = layerWorldPositionYProperty.GetFloat();
  float layerHeight = layerHeightProperty.GetFloat();
  float parentWorldPositionY = parentWorldPositionYProperty.GetFloat();
  float parentHeight = parentHeightProperty.GetFloat();

  float layerTopY = layerWorldPositionY - layerHeight * 0.5f;
  float layerBottomY = layerWorldPositionY + layerHeight * 0.5f;

  float topY = parentWorldPositionY - parentHeight * 0.5f;
  float bottomY = parentWorldPositionY + parentHeight * 0.5f;

  float height = parentHeight;

  // Clip against the layer
  if( topY < layerTopY )
  {
    float diff = fabs(topY) - fabs(layerTopY);

    height -= diff;

    if( height <= 0.0f )
    {
      height = 1.0f;       // A stencil with zero height stops the whole layer stenciling
    }
  }
  else if( bottomY > layerBottomY )
  {
    float diff = fabs(bottomY) - fabs(layerBottomY);

    height -= diff;

    if( height <= 0.0f )
    {
      height = 1.0f;      // A stencil with zero height stops the whole layer stenciling
    }
  }

  return height;
}

/**
 * @brief Adjust the position Y of the actor so that it is essentially clipped to it's parent's dimensions
 *
 * @param[in] current The current height of the actor
 * @param[in] layerWorldPositionYProperty The world Y position of the layer to clip against
 * @param[in] layerHeightProperty The height of the layer to clip against
 * @param[in] parentWorldPositionYProperty The world Y position of this actor's parent
 * @param[in] parentHeightProperty The height of this actor's parent
 */
float StencilUnionYConstraint(const float& current,
                 const PropertyInput& layerWorldPositionYProperty,
                 const PropertyInput& layerHeightProperty,
                 const PropertyInput& parentWorldPositionYProperty,
                 const PropertyInput& parentHeightProperty )
{
  float layerWorldPositionY = layerWorldPositionYProperty.GetFloat();
  float layerHeight = layerHeightProperty.GetFloat();
  float parentWorldPositionY = parentWorldPositionYProperty.GetFloat();
  float parentHeight = parentHeightProperty.GetFloat();

  float layerTopY = layerWorldPositionY - layerHeight * 0.5f;
  float layerBottomY = layerWorldPositionY + layerHeight * 0.5f;

  float topY = parentWorldPositionY - parentHeight * 0.5f;
  float bottomY = parentWorldPositionY + parentHeight * 0.5f;

  float diff = 0.0f;

  // Clip against the layer
  if( topY < layerTopY )
  {
    diff = fabs(topY) - fabs(layerTopY);
  }
  else if( bottomY > layerBottomY + parentHeight )
  {
    diff = fabs(topY) + fabs(layerTopY);
  }

  return diff;
}

// CLASS DECLARATION

/**
 * This example shows the effect of different texture filtering and wrap modes
 */
class ImageActorExample : public ConnectionTracker, public ItemFactory
{
public:

  /**
   * @brief Constructor
   *
   * @param[in] application The application instance
   */
  ImageActorExample( Application& application );

  /**
   * @brief Destructor
   */
  ~ImageActorExample();

  /**
   * @brief Initialistion function. Called on app startup.
   *
   * @param[in] application The application instance
   */
  void OnInit( Application& application );

  /**
   * @brief Handler for key event callback
   *
   * @param[in] event The key event that was raised
   */
  void OnKeyEvent( const KeyEvent& event );

  /**
   * @brief Handler for when the image change button is pressed
   *
   * @param[in] button The button that raised the event
   * @return Return if the event was handled or not
   */
  bool OnImageChange( Button button );

  /**
   * @brief Handler for when the menu button is pressed
   *
   * @param[in] button The button that raised the event
   * @return Return if the event was handled or not
   */
  bool OnMenu( Button button );

  /**
   * @brief Handler for when the help button is pressed
   *
   * @param[in] button The button that raised the event
   * @return Return if the event was handled or not
   */
  bool OnHelp( Button button );

  /**
   * @brief Handler for when the help dialog OK button is pressed
   *
   * @param[in] button The button that raised the event
   * @return Return if the event was handled or not
   */
  bool OnFilterHelpOK( Button button );

  /**
   * @brief Handler for when the menu option 0 button is pressed
   *
   * @param[in] button The button that raised the event
   * @return Return if the event was handled or not
   */
  bool OnMenu0Select( Button button );

  /**
   * @brief Handler for when the menu option 1 button is pressed
   *
   * @param[in] button The button that raised the event
   * @return Return if the event was handled or not
   */
  bool OnMenu1Select( Button button );

public: // From ItemFactory

  /**
   * @brief Return the number of items to display in the item view
   *
   * @return Return the number of items to display
   */
  virtual unsigned int GetNumberOfItems();

  /**
   * @brief Create a new item to populate the item view with
   *
   * @param[in] itemId The index of the item to create
   * @return Return the created actor for the given ID
   */
  virtual Actor NewItem(unsigned int itemId);

private:

  /**
   * @brief Create the scene for texture filtering
   */
  void CreateTextureFilteringScene();

  /**
   * @brief Create the scene for texture wrapping
   */
  void CreateTextureWrapScene();

  /**
   * @brief Unload the current view
   */
  void UnloadView();

  /**
   * @brief Create the material for the given image name
   *
   * @param[in] imageName The image filename to load for the material
   */
  void CreateMaterial( const std::string& imageName );

  /**
   * @brief Build a mesh
   *
   * @param[out] meshData The constructed mesh data
   * @param[in] scale The scale to apply to the vertices
   */
  void ConstructJointMesh( MeshData& meshData, float scale );

  /**
   * @brief Handler for scale slider value changed
   *
   * @param[in] slider The slider that raised the event
   * @param[in] value The current value of the slider
   * @return Return if the event was handled or not
   */
  bool OnSliderValueChanged( Slider slider, float value );

  /**
   * @brief Show the navigation menu
   */
  void ShowMenu();

  /**
   * @brief Hide the navigation menu
   */
  void HideMenu();

  /**
   * @brief Hide the help menu
   */
  void HideHelp();

private:

  Application&  mApplication;            ///< Application instance
  View          mView;                   ///< The View instance
  ToolBar       mToolBar;                ///< The View's Toolbar
  Layer         mContentLayer;           ///< Content layer
  TextView      mTitle;                  ///< The toolbar's title
  Popup         mMenu;                   ///< The navigation menu

  Layer         mItemViewLayer;          ///< The layer to apply stenciling of the item view to
  ItemView      mItemView;               ///< ItemView to hold test images
  TableView     mImageLayoutTable;       ///< Table view to layout the filtering scene

  PushButton    mImageSwitcherButton;    ///< Button to change images
  PushButton    mFilterHelpButton;       ///< Button to open help menu
  Popup         mFilteringHelp;          ///< The help popup

  Image         mBorderImage;            ///< The image for the border of the item view's items

  Mesh          mMesh;                   ///< Mesh to apply wrap textures to
  MeshActor     mMeshActor;              ///< Mesh actor for wrap mode test
  Material      mCustomMaterial;         ///< Material to apply for wrap mode test

  float         mImageScale;             ///< The current scale of filtering textures
  unsigned int  mCurrentImage;           ///< The index of the current image to display
  bool          mMenuShown;              ///< If the navigation menu is currently being displayed or not
};

// CLASS DEFINITION

ImageActorExample::ImageActorExample( Application& application )
: mApplication( application ),
  mImageScale( 1.0f ),
  mCurrentImage( 0 ),
  mMenuShown( false )
{
  // Connect to the Application's Init signal
  application.InitSignal().Connect( this, &ImageActorExample::OnInit );
}

ImageActorExample::~ImageActorExample()
{
  // Nothing to do here
}

void ImageActorExample::OnInit( Application& application )
{
  // The Init signal is received once (only) during the Application lifetime

  Stage stage = Stage::GetCurrent();

  // Create a border image shared by all the item actors
  mBorderImage = Image::New( ITEM_BORDER_IMAGE_PATH );

  // Creates a default view with a default tool bar.
  // The view is added to the stage.
  mContentLayer = DemoHelper::CreateView( application,
                                          mView,
                                          mToolBar,
                                          BACKGROUND_IMAGE,
                                          TOOLBAR_IMAGE,
                                          APPLICATION_TITLE );

  // Store a reference to the toolbar's title textview
  mTitle = TextView::DownCast( mToolBar.FindChildByName( "ToolbarTitle" ) );

  // Create toolbar buttons
  Actor buttonContainer = Actor::New();

  // Creates help button
  {
    Image image = Image::New( HELP_ICON_IMAGE );
    mFilterHelpButton = PushButton::New();
    mFilterHelpButton.SetParentOrigin( ParentOrigin::CENTER );
    mFilterHelpButton.SetAnchorPoint( AnchorPoint::CENTER );
    mFilterHelpButton.SetBackgroundImage( image );
    mFilterHelpButton.SetPosition( -15.0f, 0.0f );
    mFilterHelpButton.SetSize( TOOLBAR_BUTTON_SIZE, TOOLBAR_BUTTON_SIZE );
    mFilterHelpButton.ClickedSignal().Connect( this, &ImageActorExample::OnHelp );
    buttonContainer.Add( mFilterHelpButton );
  }

  // Creates view change button
  {
    Image image = Image::New( VIEW_SWAP_IMAGE );
    mImageSwitcherButton = PushButton::New();
    mImageSwitcherButton.SetParentOrigin( ParentOrigin::CENTER_LEFT );
    mImageSwitcherButton.SetAnchorPoint( AnchorPoint::CENTER_RIGHT );
    mImageSwitcherButton.SetPosition( -35.0f, 0.0f );
    mImageSwitcherButton.SetSize( TOOLBAR_BUTTON_SIZE, TOOLBAR_BUTTON_SIZE );
    mImageSwitcherButton.SetBackgroundImage( image );
    mImageSwitcherButton.ClickedSignal().Connect( this, &ImageActorExample::OnImageChange );
    buttonContainer.Add( mImageSwitcherButton );
  }

  mToolBar.AddControl( buttonContainer, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

  // Creates menu button
  {
    Image image = Image::New( MENU_ICON_IMAGE );
    PushButton viewButton = PushButton::New();
    viewButton.SetBackgroundImage( image );
    viewButton.SetSize( TOOLBAR_BUTTON_SIZE, TOOLBAR_BUTTON_SIZE );
    viewButton.ClickedSignal().Connect( this, &ImageActorExample::OnMenu );
    mToolBar.AddControl( viewButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Alignment::HorizontalLeft, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );
  }

  // For exit behavior
  stage.KeyEventSignal().Connect( this, &ImageActorExample::OnKeyEvent );

  // Create data for texture wrap scene
  CreateMaterial( TEST_IMAGE_FILENAME2 );
  MeshData meshData;
  ConstructJointMesh( meshData, 300 );
  mMesh = Mesh::New( meshData );

  // Load the initial scene
  CreateTextureFilteringScene();
}

static void AddVertex(MeshData::VertexContainer& vertices, Vector3 V, Vector2 UV)
{
  MeshData::Vertex meshVertex;
  meshVertex.x = V.x;
  meshVertex.y = V.y;
  meshVertex.z = V.z;
  meshVertex.u = UV.x;
  meshVertex.v = UV.y;
  vertices.push_back(meshVertex);
}

static void SetNormal(MeshData::VertexContainer& verts, size_t vertIdx, Vector3 normal)
{
  verts[vertIdx].nX = normal.x;
  verts[vertIdx].nY = normal.y;
  verts[vertIdx].nZ = normal.z;
}

static void CopyVertex(MeshData::Vertex& vert, Vector3& vector)
{
  vector.x = vert.x;
  vector.y = vert.y;
  vector.z = vert.z;
}

void AddTriangle(MeshData::VertexContainer& verts,
                 MeshData::FaceIndices& faces,
                 size_t v0, size_t v1, size_t v2)
{
  faces.push_back(v0);
  faces.push_back(v1);
  faces.push_back(v2);

  // Calculate normal...
  Vector3 vert0, vert1, vert2;
  CopyVertex(verts[v0], vert0);
  CopyVertex(verts[v1], vert1);
  CopyVertex(verts[v2], vert2);
  Vector3 e0 = vert1 - vert0;
  Vector3 e1 = vert2 - vert1;
  Vector3 normal = e0.Cross(e1);
  normal.Normalize();
  SetNormal(verts, v0, normal);
  SetNormal(verts, v1, normal);
  SetNormal(verts, v2, normal);
}

void ImageActorExample::ConstructJointMesh( MeshData& meshData, float scale )
{
  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones(0);

  const float UV_SCALE = 2.0f;

  // back
  AddVertex(vertices, BACK_POINT * scale, Vector2(0.0f, 0.0f)); // 0a  0
  AddVertex(vertices, BACK_POINT * scale, Vector2(0.0f, 0.0f)); // 0b  1
  AddVertex(vertices, BACK_POINT * scale, Vector2(0.0f, 0.0f)); // 0c  2

  // left
  AddVertex(vertices, LEFT_POINT * scale, Vector2(0.0f, 1.0f * UV_SCALE));  // 1a  3
  AddVertex(vertices, LEFT_POINT * scale, Vector2(0.0f, 1.0f * UV_SCALE));  // 1b  4
  AddVertex(vertices, LEFT_POINT * scale, Vector2(0.0f, 1.0f * UV_SCALE));  // 1c  5

  // right
  AddVertex(vertices, RIGHT_POINT * scale, Vector2(1.0f * UV_SCALE, 0.0f));  // 2a  6
  AddVertex(vertices, RIGHT_POINT * scale, Vector2(1.0f * UV_SCALE, 0.0f));  // 2b  7
  AddVertex(vertices, RIGHT_POINT * scale, Vector2(1.0f * UV_SCALE, 0.0f));  // 2c  8

  // top
  AddVertex(vertices, TOP_POINT * scale, Vector2(1.0f * UV_SCALE, 1.0f * UV_SCALE)); // 3a  9
  AddVertex(vertices, TOP_POINT * scale, Vector2(1.0f * UV_SCALE, 1.0f * UV_SCALE)); // 3b 10
  AddVertex(vertices, TOP_POINT * scale, Vector2(1.0f * UV_SCALE, 1.0f * UV_SCALE)); // 3c 11

  AddTriangle(vertices, faces,  0, 6,  3); // 0, 2, 1  back, right, left (ac)
  AddTriangle(vertices, faces,  1, 9,  7); // 0, 3, 2  back, top , right (ac)
  AddTriangle(vertices, faces,  2, 4, 10); // 0, 1, 3  back, left, top   (ac)
  AddTriangle(vertices, faces, 11, 5,  8); // 3, 1, 2  top, left, right  (ac)

  meshData.SetData(vertices, faces, bones, mCustomMaterial);
  meshData.SetHasColor(false);
  meshData.SetHasNormals(true);
  meshData.SetHasTextureCoords(true);
}

void ImageActorExample::OnKeyEvent(const KeyEvent& event)
{
  if( event.state == KeyEvent::Down )
  {
    if( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
    {
      mApplication.Quit();
    }
  }
}

bool ImageActorExample::OnImageChange( Button button )
{
  mCurrentImage++;
  mCurrentImage %= NUM_TEST_IMAGES + 1; // +1 for text

  ItemRange range( 0, 0 );
  mItemView.GetItemsRange( range );

  for( unsigned int i = range.begin; i < range.end; ++i )
  {
    std::stringstream ss;
    ss << "Image" << i;
    ImageActor imageActor = ImageActor::DownCast( mItemViewLayer.FindChildByName( ss.str() ) );

    std::stringstream ss2;
    ss2 << "Text" << i;
    Actor textActor = mItemViewLayer.FindChildByName( ss2.str() );
    if( imageActor && textActor )
    {
      if( mCurrentImage < NUM_TEST_IMAGES )
      {
        Image image = Image::New( TEST_IMAGES[ mCurrentImage ] );
        imageActor.SetImage( image );

        imageActor.SetVisible( true );
        textActor.SetVisible( false );      }
      else
      {
        imageActor.SetVisible( false );
        textActor.SetVisible( true );
      }
    }
  }

  return true;
}

bool ImageActorExample::OnMenu( Button button )
{
  ShowMenu();
  return true;
}

bool ImageActorExample::OnHelp( Button button )
{
  Stage stage = Stage::GetCurrent();
  const float popupWidth = stage.GetSize().x * 0.9f;
  const float popupHeight = stage.GetSize().y * 0.5f;

  // Create popup
  mFilteringHelp = Popup::New();
  mFilteringHelp.SetParentOrigin( ParentOrigin::CENTER );
  mFilteringHelp.SetAnchorPoint( AnchorPoint::CENTER );
  mFilteringHelp.SetTitle( "Help" );
  mFilteringHelp.HideTail();
  mFilteringHelp.OutsideTouchedSignal().Connect( this, &ImageActorExample::HideHelp );
  stage.Add( mFilteringHelp );

  // OK button
  PushButton okButton = PushButton::New();
  okButton.SetLabelText( "OK" );
  Image backgroundImage = Image::New( BUTTON_BACKGROUND_IMAGE );
  okButton.SetBackgroundImage( backgroundImage );
  okButton.SetSize( 200.0f, 0.0f );
  okButton.ClickedSignal().Connect( this, &ImageActorExample::OnFilterHelpOK );
  mFilteringHelp.AddButton( okButton );

  // Table view to organise information
  TableView tableView = TableView::New( 2, 0 );
  tableView.SetSize( Vector2( popupWidth, popupHeight ) );
  mFilteringHelp.Add( tableView );

  // The help text
  TextView text = TextView::New();
  text.SetParentOrigin( ParentOrigin::TOP_CENTER );
  text.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  text.SetMultilinePolicy( TextView::SplitByWord );
  text.SetSize( popupWidth, popupHeight );
  text.SetTextAlignment( Alignment::Type( Alignment::VerticalTop | Alignment::HorizontalCenter ) );
  text.SetText( FILTER_HELP_TEXT );

  TextStyle textTextStyle;
  textTextStyle.SetFontPointSize( PointSize( 6.0f ) );
  text.SetStyleToCurrentText(textTextStyle);

  tableView.SetFixedHeight( 0, 8.0f );
  tableView.AddChild( text, TableView::CellPosition( 1, 0 ) );
  tableView.SetRelativeHeight( 1, 1.0f );

  // Show the popup
  mFilteringHelp.Show();

  return true;
}

bool ImageActorExample::OnFilterHelpOK( Button button )
{
  HideHelp();
  return true;
}

void ImageActorExample::HideHelp()
{
  if( mFilteringHelp )
  {
    mFilteringHelp.Hide();
    mFilteringHelp.Reset();
  }
}

void ImageActorExample::CreateTextureFilteringScene()
{
  Stage stage = Stage::GetCurrent();

  // Configure toolbar
  mImageSwitcherButton.SetVisible( true );
  mFilterHelpButton.SetVisible( true );

  if( mTitle )
  {
    std::stringstream ss;
    ss << FILTERMODE_TITLE;

    mTitle.SetText( ss.str() );
    mTitle.SetStyleToCurrentText( DemoHelper::GetDefaultTextStyle() );
  }

  // Layout table
  mImageLayoutTable = TableView::New( 3, 1 );
  mImageLayoutTable.SetParentOrigin( ParentOrigin::CENTER );
  mImageLayoutTable.SetAnchorPoint( AnchorPoint::CENTER );

  const DemoHelper::ViewStyle& style = DemoHelper::DEFAULT_VIEW_STYLE;
  Dali::Vector2 dpi = stage.GetDpi();

  mImageLayoutTable.SetFixedHeight( 0, style.mToolBarHeight * dpi.y / style.mDpi );
  mImageLayoutTable.SetRelativeHeight( 1, 1.0f );
  mImageLayoutTable.SetFixedHeight( 2, 100 );
  stage.Add( mImageLayoutTable );

  // Create the item view actor for tests
  mItemView = ItemView::New( *this );
  mItemView.SetParentOrigin( ParentOrigin::CENTER );
  mItemView.SetAnchorPoint( AnchorPoint::CENTER );
  mItemView.ApplyConstraint( Constraint::New< Vector3 >( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );

  mItemViewLayer = Layer::New();
  mItemViewLayer.SetParentOrigin( ParentOrigin::CENTER );
  mItemViewLayer.SetAnchorPoint( AnchorPoint::CENTER );
  mItemViewLayer.RaiseToTop();
  mItemViewLayer.Add( mItemView );

  // Stencil the whole itemview so that it is clipped
  Actor stencilActor = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
  stencilActor.SetParentOrigin( ParentOrigin::CENTER );
  stencilActor.SetAnchorPoint( AnchorPoint::CENTER );
  stencilActor.SetDrawMode( DrawMode::STENCIL );
  stencilActor.ApplyConstraint( Constraint::New< Vector3 >( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
  mItemView.Add( stencilActor );

  mImageLayoutTable.AddChild( mItemViewLayer, TableView::CellPosition( 1, 0 ) );

  // Use a grid layout for tests
  GridLayoutPtr gridLayout = GridLayout::New();
  gridLayout->SetNumberOfColumns( 2 );
  gridLayout->SetTopMargin( 20.0f );
  gridLayout->SetBottomMargin( 100.0f );
  gridLayout->SetRowSpacing( 45.0f );
  mItemView.AddLayout( *gridLayout );

  Vector2 stageSize = Stage::GetCurrent().GetSize();
  float layoutWidth = Toolkit::IsHorizontal( gridLayout->GetOrientation() ) ? stageSize.height : stageSize.width;
  float gridItemSize = ( layoutWidth / gridLayout->GetNumberOfColumns() ) * 0.5f;
  gridLayout->SetScrollSpeedFactor( gridLayout->GetNumberOfColumns() / gridItemSize * 0.5f );

  mItemView.ActivateLayout( 0, Vector3(stageSize.x, stageSize.y, stageSize.x), 0.0f );

  // Scale slider
  Slider slider = Slider::New();
  slider.SetParentOrigin( ParentOrigin::CENTER );
  slider.SetAnchorPoint( AnchorPoint::CENTER );
  slider.ValueChangedSignal().Connect( this, &ImageActorExample::OnSliderValueChanged );

  slider.SetProperty( Slider::LOWER_BOUND_PROPERTY, 0.0f );
  slider.SetProperty( Slider::UPPER_BOUND_PROPERTY, 10.0f );
  slider.SetProperty( Slider::SHOW_VALUE_PROPERTY, true );
  slider.SetProperty( Slider::VALUE_PRECISION_PROPERTY, 2 );
  slider.SetProperty( Slider::VALUE_PROPERTY, mImageScale );
  slider.SetProperty( Slider::SHOW_POPUP_PROPERTY, false );

  // Label for the slider
  TextView sliderLabel = TextView::New();
  sliderLabel.SetParentOrigin( ParentOrigin::CENTER );
  sliderLabel.SetAnchorPoint( AnchorPoint::CENTER );
  std::string labelText = "Scale:";
  sliderLabel.SetText( labelText );

  TableView sliderTable = TableView::New( 1, 2 );
  sliderTable.SetParentOrigin( ParentOrigin::CENTER );
  sliderTable.SetAnchorPoint( AnchorPoint::CENTER );
  sliderTable.SetCellPadding( Size( 10.0f, 10.0f ) );

  Dali::Font font = Dali::Font::New();
  sliderTable.SetFixedWidth( 0, font.MeasureText( labelText ).width );
  sliderTable.SetRelativeWidth( 1, 1.0f );
  sliderTable.ApplyConstraint( Constraint::New< Vector3 >( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );

  Actor sliderLabelContainer = Actor::New();
  sliderLabelContainer.Add( sliderLabel );
  sliderTable.AddChild( sliderLabelContainer, TableView::CellPosition( 0, 0 ) );

  Actor sliderContainer = Actor::New();
  sliderContainer.Add( slider );
  sliderTable.AddChild( sliderContainer, TableView::CellPosition( 0, 1 ) );

  Actor sliderBackgroundActor = Toolkit::CreateSolidColorActor( Vector4( 0.0f, 0.0f, 0.0f, 0.5f ) );
  sliderBackgroundActor.Add( sliderTable );

  mImageLayoutTable.AddChild( sliderBackgroundActor, TableView::CellPosition( 2, 0 ) );
}

bool ImageActorExample::OnSliderValueChanged( Slider slider, float value )
{
  mImageScale = value;

  ItemRange range( 0, 0 );
  mItemView.GetItemsRange( range );

  for( unsigned int i = range.begin; i < range.end; ++i )
  {
    std::stringstream ss;
    ss << "Image" << i;
    Actor imageActor = mItemViewLayer.FindChildByName( ss.str() );
    if( imageActor )
    {
      imageActor.SetScale( mImageScale );
    }

    std::stringstream ss2;
    ss2 << "Text" << i;
    TextView textActor = TextView::DownCast( mItemViewLayer.FindChildByName( ss2.str() ) );
    if( textActor )
    {
      TextStyle textTextStyle;
      float pointSize = 12.0f * mImageScale;
      if( pointSize < 4.0f )  // Hard limit on how small text can be
      {
        pointSize = 4.0f;
      }

      textTextStyle.SetFontPointSize( PointSize( pointSize ) );
      textActor.SetStyleToCurrentText( textTextStyle );
    }
  }

  return true;
}

void ImageActorExample::CreateMaterial( const std::string& imageName )
{
  mCustomMaterial = Material::New("CustomMaterial");
  mCustomMaterial.SetOpacity(1.0f);
  mCustomMaterial.SetDiffuseColor(Color::WHITE);
  mCustomMaterial.SetAmbientColor(Vector4(0.0, 0.1, 0.1, 1.0));
  mCustomMaterial.SetMapU( Material::MAPPING_MODE_WRAP );
  mCustomMaterial.SetMapV( Material::MAPPING_MODE_WRAP );

  Image image = Image::New( imageName );

  mCustomMaterial.SetDiffuseTexture( image );
}

void ImageActorExample::CreateTextureWrapScene()
{
  Stage stage = Stage::GetCurrent();

  mImageSwitcherButton.SetVisible( false );
  mFilterHelpButton.SetVisible( false );

  if( mTitle )
  {
    std::stringstream ss;
    ss << WRAP_TITLE;

    mTitle.SetText( ss.str() );
    mTitle.SetStyleToCurrentText( DemoHelper::GetDefaultTextStyle() );
  }

  mMeshActor = MeshActor::New(mMesh);

  // Set the image size
  mMeshActor.SetScale( 2.0f );

  // Center the image
  mMeshActor.SetParentOrigin( ParentOrigin::CENTER );
  mMeshActor.SetAnchorPoint( AnchorPoint::CENTER );
  mMeshActor.SetAffectedByLighting(false);

  // Display the actor on the stage
  stage.Add( mMeshActor );
}

void ImageActorExample::UnloadView()
{
  // Filter scene
  mItemViewLayer.Reset();
  mItemView.Reset();
  UnparentAndReset( mImageLayoutTable );

  // Wrap scene
  UnparentAndReset( mMeshActor );
}

unsigned int ImageActorExample::GetNumberOfItems()
{
  return NUM_IMAGE_FILTER_TESTS;
}

Actor ImageActorExample::NewItem(unsigned int itemId)
{
  FilterMode::Type minFilterMode = MIN_FILTER_MODES[ itemId % NUM_FILTER_MODES ];
  FilterMode::Type magFilterMode = MAG_FILTER_MODES[ itemId % NUM_FILTER_MODES ];

  // Create an image actor for this item
  Image image = Image::New( TEST_IMAGES[ 0 ] );
  ImageActor actor = ImageActor::New( image );
  std::stringstream ss;
  ss << "Image" << itemId;
  actor.SetName( ss.str() );
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetAnchorPoint( AnchorPoint::CENTER );
  actor.SetScale( mImageScale );

  if( itemId < NUM_FILTER_MODES )
  {
    actor.SetFilterMode( minFilterMode, magFilterMode );
  }

  // Create a textview to test
  TextView text = TextView::New();
  std::stringstream ss2;
  ss2 << "Text" << itemId;
  text.SetName( ss2.str() );
  text.SetParentOrigin( ParentOrigin::CENTER );
  text.SetAnchorPoint( AnchorPoint::CENTER );
  text.SetText( "TESTING!" );

  TextStyle textTextStyle;
  textTextStyle.SetFontPointSize( PointSize( 12.0f ) );
  text.SetStyleToCurrentText(textTextStyle);
  text.SetVisible( false );

  // A layer is needed for stenciling
  Layer container = Layer::New();
  container.SetParentOrigin( ParentOrigin::CENTER );
  container.SetAnchorPoint( AnchorPoint::CENTER );

  // Add a border image child actor
  ImageActor borderActor = ImageActor::New( mBorderImage );
  borderActor.SetParentOrigin( ParentOrigin::CENTER );
  borderActor.SetAnchorPoint( AnchorPoint::CENTER );
  borderActor.SetPosition( 0.0f, 0.0f, 1.0f );
  borderActor.SetStyle( ImageActor::STYLE_NINE_PATCH );
  borderActor.SetNinePatchBorder( Vector4( ITEM_IMAGE_BORDER_LEFT, ITEM_IMAGE_BORDER_TOP, ITEM_IMAGE_BORDER_RIGHT, ITEM_IMAGE_BORDER_BOTTOM ) );
  borderActor.SetColorMode( USE_OWN_MULTIPLY_PARENT_COLOR ); // darken with parent image-actor

  Constraint constraint = Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), BorderSizeConstraintFunction( ITEM_BORDER_MARGIN_SIZE ) );
  borderActor.ApplyConstraint(constraint);

  container.Add( actor );
  container.Add( text );
  container.Add( borderActor );

  // Stencil out the actor
  Actor stencilActor = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
  stencilActor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  stencilActor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  stencilActor.SetDrawMode( DrawMode::STENCIL );
  stencilActor.ApplyConstraint( Constraint::New< Vector3 >( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
  container.Add( stencilActor );

  Actor itemActor = Actor::New();
  itemActor.SetParentOrigin( ParentOrigin::CENTER );
  itemActor.SetAnchorPoint( AnchorPoint::CENTER );

  container.ApplyConstraint( Constraint::New< Vector3 >( Actor::SIZE, Source( itemActor, Actor::SIZE ), EqualToConstraint() ) );
  container.ApplyConstraint( Constraint::New< Vector3 >( Actor::POSITION, Source( itemActor, Actor::POSITION ), EqualToConstraint() ) );

  // Make the stencil actor resize so as to form a union with the mItemViewLayer stencil
  stencilActor.ApplyConstraint( Constraint::New< float >( Actor::SIZE_HEIGHT,
                                                          Source( mItemViewLayer, Actor::WORLD_POSITION_Y ),
                                                          Source( mItemViewLayer, Actor::SIZE_HEIGHT ),
                                                          ParentSource( Actor::WORLD_POSITION_Y ),
                                                          ParentSource( Actor::SIZE_HEIGHT ),
                                                          StencilUnionHeightConstraint ) );
  stencilActor.ApplyConstraint( Constraint::New< float >( Actor::POSITION_Y,
                                                          Source( mItemViewLayer, Actor::WORLD_POSITION_Y ),
                                                          Source( mItemViewLayer, Actor::SIZE_HEIGHT ),
                                                          ParentSource( Actor::WORLD_POSITION_Y ),
                                                          ParentSource( Actor::SIZE_HEIGHT ),
                                                          StencilUnionYConstraint ) );

  mItemViewLayer.Add( container );

  // Name label below the test
  TextView label = TextView::New();
  label.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
  label.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  std::string labelText = TEST_NAMES[ itemId % NUM_TEST_NAMES ];
  label.SetText( labelText );

  TextStyle textStyle;
  textStyle.SetFontPointSize( PointSize( 6.0f ) );
  label.SetStyleToCurrentText(textStyle);

  itemActor.Add( label );

  return itemActor;
}

void ImageActorExample::ShowMenu()
{
  Stage stage = Stage::GetCurrent();
  const float popupWidth = stage.GetSize().x * 0.5f;

  mMenu = Popup::New();
  mMenu.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mMenu.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mMenu.HideTail();
  mMenu.OutsideTouchedSignal().Connect( this, &ImageActorExample::HideMenu );
  stage.Add( mMenu );

  TableView tableView = TableView::New( 0, 0 );
  Vector2 tableSize = Vector2( popupWidth, MENU_OPTION_HEIGHT * 2 );
  tableView.SetSize( tableSize );
  mMenu.Add( tableView );

  PushButton menu0Button = PushButton::New();
  menu0Button.SetLabelText( FILTERMODE_TITLE );
  menu0Button.ClickedSignal().Connect( this, &ImageActorExample::OnMenu0Select );

  Actor lineActor = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 1.0f, 1.0f, 0.5f ) );
  lineActor.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
  lineActor.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  lineActor.SetSize( 0.0f, 2.0f );
  lineActor.ApplyConstraint( Constraint::New< float >( Actor::SIZE_WIDTH, ParentSource( Actor::SIZE_WIDTH ), EqualToConstraint() ) );

  menu0Button.Add( lineActor );

  tableView.AddChild( menu0Button, TableView::CellPosition( 0, 0 ) );
  tableView.SetRelativeHeight( 0, 0.5f );

  PushButton menu1Button = PushButton::New();
  menu1Button.SetLabelText( WRAP_TITLE );
  menu1Button.ClickedSignal().Connect( this, &ImageActorExample::OnMenu1Select );

  tableView.AddChild( menu1Button, TableView::CellPosition( 1, 0 ) );
  tableView.SetRelativeHeight( 0, 0.5f );

  mMenu.Show();
  mMenuShown = true;
}

void ImageActorExample::HideMenu()
{
  if( mMenu )
  {
    mMenu.Hide();
    mMenu.Reset();
  }

  mMenuShown = false;
}

bool ImageActorExample::OnMenu0Select( Button button )
{
  UnloadView();
  CreateTextureFilteringScene();
  HideMenu();
  return true;
}

bool ImageActorExample::OnMenu1Select( Button button )
{
  UnloadView();
  CreateTextureWrapScene();
  HideMenu();
  return true;
}

void RunTest( Application& application )
{
  ImageActorExample test( application );

  application.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main(int argc, char **argv)
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}


