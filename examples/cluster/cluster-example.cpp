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

#include <sstream>
#include <iomanip>

#include "shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/shader-effects/motion-blur-effect.h>
#include <dali-toolkit/devel-api/controls/popup/popup.h>
#include <dali-toolkit/devel-api/shader-effects/carousel-effect.h>

#include "cluster.h"
#include "cluster-style.h"


using namespace Dali;
using namespace Dali::Demo;
using namespace Dali::Toolkit;
using namespace DemoHelper;

namespace // unnamed namespace
{

const char * const BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-default.png" );
const char * const TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char * const APPLICATION_TITLE( "Clusters" );
const char * const LAYOUT_NONE_IMAGE( DALI_IMAGE_DIR "icon-cluster-none.png" );
const char * const LAYOUT_NONE_IMAGE_SELECTED( DALI_IMAGE_DIR "icon-cluster-none-selected.png" );
const char * const LAYOUT_MOTION_BLUR_IMAGE( DALI_IMAGE_DIR "icon-cluster-wobble.png" );
const char * const LAYOUT_MOTION_BLUR_IMAGE_SELECTED( DALI_IMAGE_DIR "icon-cluster-wobble-selected.png" );
const char * const LAYOUT_CAROUSEL_IMAGE( DALI_IMAGE_DIR "icon-cluster-carousel.png" );
const char * const LAYOUT_CAROUSEL_IMAGE_SELECTED( DALI_IMAGE_DIR "icon-cluster-carousel-selected.png" );
const char * const LAYOUT_SPHERE_IMAGE( DALI_IMAGE_DIR "icon-cluster-sphere.png" );
const char * const LAYOUT_SPHERE_IMAGE_SELECTED( DALI_IMAGE_DIR "icon-cluster-sphere-selected.png" );

enum ClusterType
{
  PEOPLE,
  TODAY,
  PHONE,
  PICTURES,
  MUSIC,
  MAGAZINE,

  CLUSTER_COUNT
};

const char* PEOPLE_IMAGE_PATHS[] =   { DALI_IMAGE_DIR "people-medium-1.jpg",
                                       DALI_IMAGE_DIR "people-medium-2.jpg",
                                       DALI_IMAGE_DIR "people-medium-3.jpg",
                                       DALI_IMAGE_DIR "people-medium-4.jpg",
                                       DALI_IMAGE_DIR "people-medium-5.jpg",
                                       DALI_IMAGE_DIR "people-medium-6.jpg",
                                       DALI_IMAGE_DIR "people-medium-7.jpg",
                                       DALI_IMAGE_DIR "people-medium-8.jpg",
                                       NULL };

const char* TODAY_IMAGE_PATHS[] =    { DALI_IMAGE_DIR "gallery-medium-1.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-2.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-3.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-4.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-5.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-6.jpg",
                                       NULL };

const char* PHONE_IMAGE_PATHS[] =    { DALI_IMAGE_DIR "gallery-medium-7.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-8.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-9.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-10.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-11.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-12.jpg",
                                       NULL };

const char* PICTURES_IMAGE_PATHS[] = { DALI_IMAGE_DIR "gallery-medium-13.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-14.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-15.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-16.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-17.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-18.jpg",
                                       NULL };

const char* MUSIC_IMAGE_PATHS[] =    { DALI_IMAGE_DIR "gallery-medium-19.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-20.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-21.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-22.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-23.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-24.jpg",
                                       NULL };

const char* MAGAZINE_IMAGE_PATHS[] = { DALI_IMAGE_DIR "gallery-medium-25.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-26.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-27.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-28.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-29.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-30.jpg",
                                       NULL };

const char **IMAGE_GROUPS[] = {PEOPLE_IMAGE_PATHS,
                               TODAY_IMAGE_PATHS,
                               PHONE_IMAGE_PATHS,
                               PICTURES_IMAGE_PATHS,
                               MUSIC_IMAGE_PATHS,
                               MAGAZINE_IMAGE_PATHS,
                               NULL};

const float CLUSTER_IMAGE_THUMBNAIL_WIDTH  = 256.0f;
const float CLUSTER_IMAGE_THUMBNAIL_HEIGHT = 256.0f;

const char* CLUSTER_SHADOW_IMAGE_PATH = DALI_IMAGE_DIR "cluster-image-shadow.png";
const char* CLUSTER_BORDER_IMAGE_PATH = DALI_IMAGE_DIR "cluster-image-frame.png";
const char* CLUSTER_BACKGROUND_IMAGE_PATH = DALI_IMAGE_DIR "cluster-background.png";

const float CLUSTER_IMAGE_BORDER_INDENT = 14.0f;            ///< Indent of border in pixels.
const float CLUSTER_IMAGE_BORDER_WIDTH = 128;               ///< Width of border in pixels.
const float CLUSTER_IMAGE_BORDER_HEIGHT = 128;              ///< Height of border in pixels.

const Vector4 CLUSTER_IMAGE_BORDER_ABSOLUTE( 16.0f, 16.0f, 16.0f, 16.0f ); // Border dimensions in absolute pixel coordinates.

// These values depend on the border image
const float CLUSTER_RELATIVE_SIZE = 0.65f;                  ///< Cluster size relative to screen width

const float CLUSTER_GROUP_DELAY_TOP = 0.25f;                ///< Delay for top Clusters in seconds.
const float CLUSTER_GROUP_DELAY_BOTTOM = 0.0f;              ///< Delay for bottom Clusters in seconds.

const float CLUSTER_COLUMN_INDENT = 0.1f;                   ///< Left Indentation in screen coordinates.
const float CLUSTER_ROW_INDENT = 0.13f;                     ///< Top Indentation in screen coordinates.

const float UI_MARGIN = 10.0f;                              ///< Screen Margin for placement of UI buttons

const float CAROUSEL_EFFECT_RADIUS = 500.0f;                ///< In Carousel Effect mode: Radius of carousel (Z peak depth)
const float CAROUSEL_EFFECT_ANGLE_SWEEP = 90.0f;            ///< In Carousel Effect mode: Angle sweep from left to right of screen
const float SPHERE_EFFECT_RADIUS = 1000.0f;                 ///< In Sphere Effect mode: Radius of sphere carousel (Z peak depth)
const float SPHERE_EFFECT_POSITION_Z = -700.0f;             ///< In Sphere Effect mode: Z position alter (as carousel is coming out to screen we move back)
const float SPHERE_EFFECT_ANGLE_SWEEP = 90.0f;              ///< In Sphere Effect mode: Angle sweep from edge to opposite side of circle.
const float SPHERE_EFFECT_VERTICAL_DOMAIN = 0.15f;          ///< In Sphere Effect mode: How much the user can pan in the vertical axis. (in stageHeights)

/**
 * List of effect types that user can select through.
 */
enum ExampleEffectType
{
  NO_EFFECT = 0,
  MOTION_BLUR_EFFECT,
  CAROUSEL_EFFECT,
  SPHERE_EFFECT,
  TOTAL_EFFECTS,
};

/**
 * List of effect type names that appear on the Effect button.
 */
const char* EXAMPLE_EFFECT_LABEL[] = { "None",
                                       "Motion Blur",
                                       "Carousel",
                                       "Sphere",
                                     };

/**
 * CarouselEffectOrientationConstraint
 * Based on the View orientation i.e. portrait (0 degrees), landscape (90 degrees) etc.
 * carousel shader effect should bend differently (as a function of this orientation),
 * as shader effect is applied to the screen coordinates.
 */
struct CarouselEffectOrientationConstraint
{
  /**
   * Constructor
   * @param[in] angleSweep The amount of degrees to rotate by per pixel.
   */
  CarouselEffectOrientationConstraint( const Vector2 angleSweep )
  : mAngleSweep( angleSweep )
  {
  }

  /**
   * @param[in] current The object's current property value
   * @return The object's new property value
   */
  void operator()( Vector2& current, const PropertyInputContainer& inputs )
  {
    Vector3 axis;
    Radian angle;
    inputs[0]->GetQuaternion().ToAxisAngle( axis, angle );

    current.x = cosf(angle);
    current.y = sinf(angle);

    current *= mAngleSweep;
  }

  Vector2 mAngleSweep;

};

/**
 * SphereEffectOffsetConstraint
 *
 * Sets SphereEffect's center to be a function of the
 * screen orientation (portrait or landscape).
 */
struct SphereEffectOffsetConstraint
{
  /**
   * @param[in] stageSize The stage size (not subject to orientation)
   * @param[in] center Shear Center position based on initial orientation.
   */
  SphereEffectOffsetConstraint(float offset)
  : mOffset(offset)
  {
  }

  /**
   * @param[in] current The current center
   * @param[in] propertyViewSize The current view size
   * @return vector to provide SphereShaderEffect
   */
  void operator()( float& current, const PropertyInputContainer& /* inputs */ )
  {
    current += mOffset;
  }

  float mOffset;
};

/**
 * ClusterInfo struct
 *
 * Contains information about each cluster in mClusterInfo list.
 */
struct ClusterInfo
{
  /**
   * Constructor
   *
   * @param[in] cluster The cluster instance
   * @param[in] index The cluster's index (starting from 0 for the first cluster)
   * @param[in] position The cluster's original position
   * @param[in] size The cluster's original size
   */
  ClusterInfo(Cluster cluster, int index, const Vector3& position, const Vector3& size)
  : mCluster(cluster),
    mIndex(index),
    mPosition(position),
    mSize(size)
  {

  }

  /**
   * Copy constructor
   *
   * @param[in] rhs The ClusterInfo struct to be copied.
   */
  ClusterInfo( const ClusterInfo& rhs )
  : mCluster(rhs.mCluster),
    mIndex(rhs.mIndex),
    mPosition(rhs.mPosition),
    mSize(rhs.mSize)
  {

  }

  /**
   * Assignment operator
   */
  ClusterInfo& operator=( const ClusterInfo& rhs )
  {
    if( this != &rhs )
    {
      mCluster = rhs.mCluster;
      mIndex = rhs.mIndex;
      mPosition = rhs.mPosition;
      mSize = rhs.mSize;
    }
    return *this;
  }


  Cluster mCluster;             ///< Cluster instance
  int mIndex;                   ///< Cluster index
  Vector3 mPosition;            ///< Cluster original position
  Vector3 mSize;                ///< Cluster original size
  Constraint mEffectConstraint; ///< Cluster constraint
};

/**
 * Shrinks Actor based on parent's aspect ratio.
 */
struct ShrinkConstraint
{
  /**
   * Constructor
   */
  ShrinkConstraint()
  {
  }

  /**
   * @param[in] current The object's current scale value
   * @param[in] parentScale The parent's scale
   * @return The object's new scale value
   */
  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& parentScale)
  {
    return Vector3( parentScale.GetVector3().x / parentScale.GetVector3().y, 1.0f, 1.0f );
  }
};

struct ButtonImages
{
  std::string mButtonImage;
  std::string mSelectedImage;
};


} // unnamed namespace

/**
 * Custom position and size of shadow image
 */
namespace ShadowProperty
{
const Vector3 ANCHOR_POINT      (0.54f, 0.6f, 0.5f);
const Vector3 RELATIVE_POSITION (0.0f, 0.0f, -0.1f);
const Vector3 SIZE_SCALE        (1.4f, 1.4f, 1.0f);
}

// This example shows how to use Cluster UI control
//
class ClusterController : public ConnectionTracker
{
public:

  /**
   * Constructor
   * @param application class, stored as reference
   */
  ClusterController(Application &app)
  : mApplication(app),
    mClusterCount(0),
    mExampleEffect(NO_EFFECT)
  {
    // Connect to the Application's Init signal
    app.InitSignal().Connect(this, &ClusterController::Create);
  }

  ~ClusterController()
  {
    // Nothing to do here; everything gets deleted automatically
  }

  /**
   * This method gets called once the main loop of application is up and running
   */
  void Create(Application& application)
  {
    Stage::GetCurrent().KeyEventSignal().Connect(this, &ClusterController::OnKeyEvent);

    // The Init signal is received once (only) during the Application lifetime

    // Hide the indicator bar
    application.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            "" );

    mContentLayer.SetProperty(Layer::Property::BEHAVIOR, "Dali::Layer::LAYER_3D");

    // Create a effect toggle button. (right of toolbar)
    mLayoutButtonImages[ NO_EFFECT ].mButtonImage = LAYOUT_NONE_IMAGE;
    mLayoutButtonImages[ NO_EFFECT ].mSelectedImage = LAYOUT_NONE_IMAGE_SELECTED;
    mLayoutButtonImages[ MOTION_BLUR_EFFECT ].mButtonImage = LAYOUT_MOTION_BLUR_IMAGE;
    mLayoutButtonImages[ MOTION_BLUR_EFFECT ].mSelectedImage = LAYOUT_MOTION_BLUR_IMAGE_SELECTED;
    mLayoutButtonImages[ CAROUSEL_EFFECT ].mButtonImage = LAYOUT_CAROUSEL_IMAGE;
    mLayoutButtonImages[ CAROUSEL_EFFECT ].mSelectedImage = LAYOUT_CAROUSEL_IMAGE_SELECTED;
    mLayoutButtonImages[ SPHERE_EFFECT ].mButtonImage = LAYOUT_SPHERE_IMAGE;
    mLayoutButtonImages[ SPHERE_EFFECT ].mSelectedImage = LAYOUT_SPHERE_IMAGE_SELECTED;

    mLayoutButton = Toolkit::PushButton::New();
    mLayoutButton.ClickedSignal().Connect( this, &ClusterController::OnEffectTouched );
    mToolBar.AddControl( mLayoutButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    // create and setup the scroll view...
    mScrollView = ScrollView::New();
    mScrollView.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

    // anchor the scroll view from its center point to the middle of its parent
    mScrollView.SetAnchorPoint(AnchorPoint::CENTER);
    mScrollView.SetParentOrigin(ParentOrigin::CENTER);

    // Scale ScrollView to fit parent (mContentLayer)
    mScrollView.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

    // Add the scroll view to the content layer
    mContentLayer.Add(mScrollView);

    // Create the image border shared by all the cluster image actors
    mClusterBorderImage = ResourceImage::New(CLUSTER_BORDER_IMAGE_PATH);

    AddCluster( PEOPLE,   ClusterStyleStandard::New(ClusterStyleStandard::ClusterStyle1) );
    AddCluster( TODAY,    ClusterStyleStandard::New(ClusterStyleStandard::ClusterStyle2) );
    AddCluster( PHONE,    ClusterStyleStandard::New(ClusterStyleStandard::ClusterStyle3) );
    AddCluster( PICTURES, ClusterStyleStandard::New(ClusterStyleStandard::ClusterStyle4) );
    AddCluster( MUSIC,    ClusterStyleStandard::New(ClusterStyleStandard::ClusterStyle2) );
    AddCluster( MAGAZINE, ClusterStyleStandard::New(ClusterStyleStandard::ClusterStyle3) );

    SetEffect(MOTION_BLUR_EFFECT);
  }

  /**
   * Helper to create the cluster actors
   */
  Cluster CreateClusterActor(ClusterType clusterType, ClusterStyle style, Vector3& clusterSize)
  {
    // Create the cluster actor with the given cluster style
    Cluster clusterActor = Cluster::New(style);
    clusterActor.SetParentOrigin(ParentOrigin::CENTER);
    clusterActor.SetAnchorPoint(AnchorPoint::CENTER);

    Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
    float minStageDimension = std::min(stageSize.x, stageSize.y);

    clusterSize.x = minStageDimension * CLUSTER_RELATIVE_SIZE;
    clusterSize.y = minStageDimension * CLUSTER_RELATIVE_SIZE;

    clusterActor.SetSize( clusterSize );

    DALI_ASSERT_ALWAYS(clusterType < CLUSTER_COUNT);
    const char **paths = IMAGE_GROUPS[clusterType];
    DALI_ASSERT_ALWAYS(paths);

    // Add a background image to the cluster, limiting the loaded size by
    // fitting it inside a quarter of the stage area with the conservative BOX
    // filter mode:
    Image bg = ResourceImage::New( CLUSTER_BACKGROUND_IMAGE_PATH, Dali::ImageDimensions( stageSize.x * 0.5f, stageSize.y * 0.5f ), Dali::FittingMode::SHRINK_TO_FIT, Dali::SamplingMode::BOX );
    Control clusterControl = Control::DownCast( clusterActor );
    clusterControl.SetBackgroundImage( bg );

    // Add actors (pictures) as the children of the cluster
    for (unsigned int i = 0; (i < style.GetMaximumNumberOfChildren()) && (*paths); i++, paths++)
    {
      clusterActor.AddChild(CreateClusterPictureActor(clusterType, *paths), i);
    }

    return clusterActor;
  }

  /**
   * Helper to create the picture actors in the cluster
   */
  Actor CreateClusterPictureActor(ClusterType clusterType, const std::string& imagePath)
  {
    // Create a picture for this cluster image
    // actor|->shadow
    //      |->image
    //      |->border
    Actor actor = Actor::New();
    actor.SetSize(CLUSTER_IMAGE_THUMBNAIL_WIDTH, CLUSTER_IMAGE_THUMBNAIL_HEIGHT);
    actor.SetParentOrigin( ParentOrigin::CENTER );
    actor.SetAnchorPoint( AnchorPoint::CENTER );

    // Load the thumbnail at quarter of screen width or standard size if that is smaller:
    Size stageQuarter = Stage::GetCurrent().GetSize() * 0.25f;
    const ImageDimensions requestedDims = ImageDimensions( std::min( stageQuarter.x, CLUSTER_IMAGE_THUMBNAIL_WIDTH ), std::min( stageQuarter.y, CLUSTER_IMAGE_THUMBNAIL_HEIGHT ) );

    // Add a shadow image child actor
    Image shadowImage = ResourceImage::New( CLUSTER_SHADOW_IMAGE_PATH, requestedDims, Dali::FittingMode::SHRINK_TO_FIT, Dali::SamplingMode::BOX );
    ImageActor shadowActor = ImageActor::New(shadowImage);

    // Shadow is not exactly located on the center of the image, so it is moved to a little
    // upper-left side of the image relatively using customised AnchorPoint.
    shadowActor.SetParentOrigin(ShadowProperty::ANCHOR_POINT);
    shadowActor.SetAnchorPoint(AnchorPoint::CENTER);
    shadowActor.SetPosition(Vector3(0.0f, 0.0f, -1.0f));

    // Apply size-relative mode to auto-size the image shadow
    shadowActor.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    shadowActor.SetSizeModeFactor( ShadowProperty::SIZE_SCALE );
    actor.Add( shadowActor );

    // Add a picture image actor to actor (with equal size to the parent).
    Image image = ResourceImage::New( imagePath, requestedDims, Dali::FittingMode::SHRINK_TO_FIT, Dali::SamplingMode::BOX );
    ImageActor imageActor = ImageActor::New( image );
    imageActor.SetParentOrigin( ParentOrigin::CENTER );
    imageActor.SetAnchorPoint( AnchorPoint::CENTER );
    imageActor.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    actor.Add( imageActor );

    // Add a border image child actor (with a fixed size offset from parent).
    ImageActor borderActor = ImageActor::New( mClusterBorderImage );
    borderActor.SetParentOrigin( ParentOrigin::CENTER );
    borderActor.SetAnchorPoint( AnchorPoint::CENTER );
    borderActor.SetStyle( ImageActor::STYLE_NINE_PATCH );
    borderActor.SetNinePatchBorder( CLUSTER_IMAGE_BORDER_ABSOLUTE );
    borderActor.SetPosition( Vector3( 0.0f, 0.0f, 1.0f ) );
    borderActor.SetResizePolicy( ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::ALL_DIMENSIONS );
    borderActor.SetSizeModeFactor( Vector3( CLUSTER_IMAGE_BORDER_INDENT - 1.0f, CLUSTER_IMAGE_BORDER_INDENT - 1.0f, 0.0f ) * 2.0f );
    actor.Add( borderActor );

    return actor;
  }


  /**
   * Adds a Cluster to the ScrollView
   *
   * @param[in] clusterType The type of cluster (determines the image content)
   * @param[in] style The style to be used for this cluster.
   */
  void AddCluster(ClusterType clusterType, ClusterStyle style)
  {
    Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();

    int column = mClusterCount>>1;
    int row = mClusterCount&1;

    float minStageDimension = std::min(stageSize.x, stageSize.y);
    float clusterRightShift = 1.0f - CLUSTER_COLUMN_INDENT * 2.0f;
    Vector3 clusterPosition = Vector3(CLUSTER_COLUMN_INDENT * stageSize.width + row * (clusterRightShift * stageSize.width - minStageDimension * CLUSTER_RELATIVE_SIZE),
                                      CLUSTER_ROW_INDENT * stageSize.height + row * (clusterRightShift * stageSize.height - minStageDimension * CLUSTER_RELATIVE_SIZE), 0.0f);

    Actor pageView = Actor::New();
    mScrollView.Add(pageView);
    pageView.SetParentOrigin(ParentOrigin::CENTER);
    pageView.SetPosition(Vector3(stageSize.width * column, 0.0f, 0.0f));
    pageView.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

    // Create cluster actors and add them to scroll view
    Vector3 clusterSize;
    Cluster cluster = CreateClusterActor( clusterType, style, clusterSize );
    cluster.SetParentOrigin(ParentOrigin::TOP_LEFT);
    cluster.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    cluster.SetPosition( clusterPosition );

    pageView.Add(cluster);

    mClusterInfo.push_back( ClusterInfo( cluster, mClusterCount, clusterPosition, clusterSize ) );

    mClusterCount++;
  }

  /**
   * Sets motion blur effect to a cluster and all its children
   *
   * @param[in] actor Cluster control to which the effect will be applied
   */
  void SetMotionBlurEffect( Actor actor )
  {

    // only do something if the actor and effect are valid
    if( actor )
    {
      // first remove from this actor
      ImageActor imageActor = ImageActor::DownCast( actor );
      if( imageActor )
      {
        ShaderEffect shaderEffect = Toolkit::CreateMotionBlurEffect();
        shaderEffect.SetUniform("uSpeedScalingFactor",0.1f);

        Dali::Property::Index uModelProperty = shaderEffect.GetPropertyIndex( "uModelLastFrame" );
        Constraint constraint = Constraint::New<Matrix>( shaderEffect, uModelProperty, EqualToConstraint() );
        constraint.AddSource( Source( imageActor , Actor::Property::WORLD_MATRIX ) );
        constraint.Apply();
        imageActor.SetShaderEffect( shaderEffect );
      }
      // then all children recursively
      const unsigned int count = actor.GetChildCount();
      for( unsigned int index = 0; index < count; ++index )
      {
        Actor child( actor.GetChildAt( index ) );
        SetMotionBlurEffect( child );
      }
    }
  }

  /**
   * Resets ScrollView and Clusters settings
   * to reflect the new ExampleEffectType
   *
   * TODO: Add animation transition to fade out of existing effect,
   * and into new effect.
   *
   * @param[in] type The desired effect to switch to.
   */
  void SetEffect(ExampleEffectType type)
  {
    Vector2 stageSize(Dali::Stage::GetCurrent().GetSize());

    mExampleEffect = type;

    std::stringstream ss(APPLICATION_TITLE);
    ss << APPLICATION_TITLE << ": " << EXAMPLE_EFFECT_LABEL[mExampleEffect];
    SetTitle(ss.str());

    // Set up default ruler settings (fixed in horizontal, disabled in vertical)
    RulerPtr rulerX;
    rulerX = new FixedRuler(stageSize.x);
    int columns = (mClusterCount + 1) >> 1;
    rulerX->SetDomain(RulerDomain(0.0f, stageSize.x * columns, true));
    mScrollView.SetRulerX(rulerX);

    RulerPtr rulerY = new DefaultRuler();
    rulerY->Disable();
    mScrollView.SetRulerY(rulerY);

    mScrollView.SetActorAutoSnap(false);

    // Remove all shader-effects from mScrollView and it's children (the clusters)
    mScrollView.SetPosition(Vector3::ZERO);

    mLayoutButton.SetUnselectedImage( mLayoutButtonImages[ type ].mButtonImage );
    mLayoutButton.SetSelectedImage( mLayoutButtonImages[ type ].mSelectedImage );

    for( std::vector<ClusterInfo>::iterator i = mClusterInfo.begin(); i != mClusterInfo.end(); ++i )
    {
      Cluster cluster = i->mCluster;
      RemoveShaderEffectRecursively( cluster );
      if( i->mEffectConstraint )
      {
        i->mEffectConstraint.Remove();
        i->mEffectConstraint.Reset();
      }
    }

    // Apply new shader-effects.
    // Move Y to origin incase we came from an effect where user could free pan in y axis.
    const Vector2 currentScrollPosition(mScrollView.GetCurrentScrollPosition());
    mScrollView.ScrollTo(Vector2(currentScrollPosition.x, 0.0f));

    switch(type)
    {
      case NO_EFFECT:
      {
        break;
      }

      case MOTION_BLUR_EFFECT:
      {
        for( std::vector<ClusterInfo>::iterator i = mClusterInfo.begin(); i != mClusterInfo.end(); ++i )
        {
          SetMotionBlurEffect( i->mCluster );
        }
        break;
      }

      case CAROUSEL_EFFECT:
      {
        // Apply Carousel Shader Effect to scrollView
        ShaderEffect shaderEffect = Toolkit::CreateCarouselEffect();
        shaderEffect.SetUniform( "uRadius", -CAROUSEL_EFFECT_RADIUS );
        // dont apply shader effect to scrollview as it might override internal shaders for bounce effect etc
        for( std::vector<ClusterInfo>::iterator i = mClusterInfo.begin(); i != mClusterInfo.end(); ++i )
        {
          Cluster cluster = i->mCluster;
          SetShaderEffectRecursively( cluster, shaderEffect );
        }
        mScrollView.SetPosition( Vector3( 0.0f, 0.0f, CAROUSEL_EFFECT_RADIUS ) );

        const Vector2 angleSweep( CAROUSEL_EFFECT_ANGLE_SWEEP / stageSize.width,
                                  CAROUSEL_EFFECT_ANGLE_SWEEP / stageSize.width );

        Property::Index anglePerUnit = shaderEffect.GetPropertyIndex( "uAnglePerUnit" );
        Constraint constraint = Constraint::New<Vector2>( shaderEffect, anglePerUnit, CarouselEffectOrientationConstraint( angleSweep ) );
        constraint.AddSource( Source(mView, Actor::Property::ORIENTATION) );
        constraint.Apply();

        break;
      }

      case SPHERE_EFFECT:
      {
        // Change ruler to free panning...
        RulerPtr rulerX;
        rulerX = new DefaultRuler();
        int columns = (mClusterCount + 1) >> 1;
        rulerX->SetDomain(RulerDomain(0.0f, stageSize.x * columns, true));
        mScrollView.SetRulerX(rulerX);

        RulerPtr rulerY = new DefaultRuler();
        rulerY->SetDomain(RulerDomain( -stageSize.y * SPHERE_EFFECT_VERTICAL_DOMAIN, stageSize.y * (1.0f + SPHERE_EFFECT_VERTICAL_DOMAIN), true));
        mScrollView.SetRulerY(rulerY);

        // Apply Carousel Shader Effect to scrollView (Spherical style)
        ShaderEffect shaderEffect = Toolkit::CreateCarouselEffect();

        shaderEffect.SetUniform( "uRadius", SPHERE_EFFECT_RADIUS );
        shaderEffect.SetUniform( "uAnglePerUnit", Vector2( SPHERE_EFFECT_ANGLE_SWEEP / stageSize.y, SPHERE_EFFECT_ANGLE_SWEEP / stageSize.y ) );
        // dont apply shader effect to scrollview as it might override internal shaders for bounce effect etc
        for( std::vector<ClusterInfo>::iterator i = mClusterInfo.begin(); i != mClusterInfo.end(); ++i )
        {
          Cluster cluster = i->mCluster;
          i->mEffectConstraint = Constraint::New<float>( cluster, Actor::Property::POSITION_Z, SphereEffectOffsetConstraint( SPHERE_EFFECT_POSITION_Z ) );
          i->mEffectConstraint.SetRemoveAction(Constraint::Discard);
          SetShaderEffectRecursively( cluster, shaderEffect );
          i->mEffectConstraint.Apply();
        }
        break;
      }

      default:
        break;
    }

  }

  /**
   * Signal handler, called when quit button is pressed
   */
  bool OnEffectTouched( Toolkit::Button button )
  {
    // quit the application
    SetEffect(static_cast<ExampleEffectType>( (mExampleEffect + 1) % TOTAL_EFFECTS) );
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

    mTitleActor.SetProperty( TextLabel::Property::TEXT, title );
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

  Application&               mApplication;                       ///< Application instance
  Toolkit::Control           mView;                              ///< The View instance.
  Toolkit::ToolBar           mToolBar;                           ///< The View's Toolbar.
  TextLabel                  mTitleActor;                        ///< The Toolbar's Title.

  Layer                      mContentLayer;                      ///< Content layer (scrolling cluster content)

  ScrollView                 mScrollView;                        ///< The ScrollView container for all clusters
  Image                      mClusterBorderImage;                ///< The border frame that appears on each image

  std::vector<ClusterInfo>   mClusterInfo;                       ///< Keeps track of each cluster's information.
  int                        mClusterCount;                      ///< Current number of clusters in use
  ExampleEffectType          mExampleEffect;                     ///< Current example effect.

  Toolkit::PushButton        mLayoutButton;                      ///< The layout button
  ButtonImages               mLayoutButtonImages[TOTAL_EFFECTS]; ///< Image when no layout
};

// Entry point for Linux & Tizen applications
//
int main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv, DALI_DEMO_THEME_PATH);
  ClusterController test(app);
  app.MainLoop();

  return 0;
}
