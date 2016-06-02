#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/rendering/renderer.h>
#include <dali/public-api/object/property-map.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/control-renderer.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>
#include <dali-toolkit/public-api/controls/slider/slider.h>
#include <dali/public-api/actors/actor-enumerations.h>

#include <iostream> //TODO: TMP

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
  //Button image urls
  const char* BUTTON_IMAGE_URL[] =
  {
    DEMO_IMAGE_DIR "sphere.png",
    DEMO_IMAGE_DIR "cone.png",
    DEMO_IMAGE_DIR "conical-frustrum.png",
    DEMO_IMAGE_DIR "cylinder.png",
    DEMO_IMAGE_DIR "cube.png",
    DEMO_IMAGE_DIR "bevelled-cube.png",
    DEMO_IMAGE_DIR "octahedron.png"
  };

  //Shape property defaults
  const int DEFAULT_SLICES = 32;
  const int DEFAULT_STACKS = 32;
  const float DEFAULT_SCALE_HEIGHT = 16.0f;
  const float DEFAULT_SCALE_BOTTOM_RADIUS = 8.0f;
  const float DEFAULT_SCALE_TOP_RADIUS = 4.0f;
  const float DEFAULT_SCALE_RADIUS = 8.0f;
  const float DEFAULT_BEVEL_PERCENTAGE = 0.3f;
  const float DEFAULT_BEVEL_SMOOTHNESS = 0.0f;

  //Used to the control rate of rotation when panning an object.
  const float X_ROTATION_DISPLACEMENT_FACTOR = 60.0f;
  const float Y_ROTATION_DISPLACEMENT_FACTOR = 60.0f;

  const int NUM_MODELS = 7; //Total number of possible base shapes.
  const int MAX_PROPERTIES = 3; //Maximum number of properties a shape may require. (For displaying sliders.)

} //End namespace

class PrimitiveShapesController : public ConnectionTracker
{
public:

  PrimitiveShapesController( Application& application )
  : mApplication( application ),
    mRotation( Vector2( 0.0f, 0.0f ) ),
    mColor( Vector4( 0.3f, 0.7f, 1.0f, 1.0f ) )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &PrimitiveShapesController::Create );
  }

  ~PrimitiveShapesController()
  {
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    //Set up 3D layer to place UI on.
    Layer layer = Layer::New();
    layer.SetParentOrigin( ParentOrigin::CENTER );
    layer.SetAnchorPoint( AnchorPoint::CENTER );
    layer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    layer.SetBehavior( Layer::LAYER_3D );
    stage.Add( layer );

    //Set up model selection buttons.
    SetupButtons( layer );

    //Set up model parameter sliders.
    SetupSliders( layer );

    //Set up 3D model.
    SetupModel( layer );
  }

  void SetupButtons( Layer layer )
  {
    float padding = 10.0f;

    FlexContainer buttonContainer = FlexContainer::New();
    buttonContainer.SetParentOrigin( ParentOrigin::TOP_CENTER );
    buttonContainer.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    buttonContainer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    buttonContainer.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::HEIGHT );
    buttonContainer.SetSizeModeFactor( Vector3( 0.0, 0.3, 0.0 ) );
    buttonContainer.SetPadding( Padding( padding, padding, padding, padding ) );

    buttonContainer.SetProperty( FlexContainer::Property::FLEX_DIRECTION, FlexContainer::ROW );
    buttonContainer.SetProperty( FlexContainer::Property::FLEX_WRAP, FlexContainer::WRAP );

    layer.Add( buttonContainer );

    for( int i = 0; i < NUM_MODELS; i++ )
    {
      buttonContainer.Add( CreateButton( i ) );
    }
  }

  Actor CreateButton( int modelNumber )
  {
    float padding = 5.0f;

    PushButton button = Toolkit::PushButton::New();
    button.SetParentOrigin( ParentOrigin::CENTER );
    button.SetAnchorPoint( AnchorPoint::CENTER );
    button.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    button.SetPadding( Padding( padding, padding, padding, padding ) );
    button.SetProperty( Button::Property::UNSELECTED_STATE_IMAGE, Property::Value( DEMO_IMAGE_DIR "temporary.png" ) );
    button.SetProperty( Button::Property::SELECTED_STATE_IMAGE, Property::Value( BUTTON_IMAGE_URL[modelNumber] ) );
    button.RegisterProperty( "modelNumber", Property::Value( modelNumber ) );
    button.ClickedSignal().Connect( this, &PrimitiveShapesController::OnChangeShapeClicked );

    return button;
  }

  void SetupSliders( Layer layer )
  {
    //Create table to hold sliders and their corresponding labels.
    mSliderTable = Toolkit::TableView::New( 3, 2 );
    mSliderTable.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    mSliderTable.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    mSliderTable.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mSliderTable.SetSizeModeFactor( Vector3( 0.9, 0.3, 0.0 ) );
    mSliderTable.SetFitWidth( 0 );  //Label column should fit to natural size of label.
    mSliderTable.SetRelativeWidth( 1, 1.0f );  //Slider column should fill remaining space.
    mSliderTable.SetCellPadding( Vector2( 10.0f, 0.0f ) );
    layer.Add( mSliderTable );

    //Set up sliders to adjust model properties, and place labels next to them.
    for( int i = 0; i < MAX_PROPERTIES; i++ )
    {
      //Create slider
      Slider slider = Slider::New();
      slider.SetParentOrigin( ParentOrigin::CENTER );
      slider.SetAnchorPoint( AnchorPoint::CENTER );
      slider.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      slider.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      slider.ValueChangedSignal().Connect( this, &PrimitiveShapesController::OnSliderValueChanged );
      mSliders.push_back( slider );

      //Setup slider cell properties
      mSliderTable.AddChild( slider, TableView::CellPosition( i, 1 ) );
      mSliderTable.SetCellAlignment( TableView::CellPosition( i, 1 ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );

      //Create slider label
      TextLabel sliderLabel = TextLabel::New( "Label" );
      sliderLabel.SetParentOrigin( ParentOrigin::CENTER );
      sliderLabel.SetAnchorPoint( AnchorPoint::CENTER );
      sliderLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
      mSliderLabels.push_back( sliderLabel );

      //Setup slider label cell properties
      mSliderTable.AddChild( sliderLabel, TableView::CellPosition( i, 0 ) );
      mSliderTable.SetCellAlignment( TableView::CellPosition( i, 0 ), HorizontalAlignment::LEFT, VerticalAlignment::CENTER );
    }
  }

  //Sets up the slider at the given index for the supplied property, and labels it appropriately.
  void SetupSlider( int sliderIndex, float lowerBound, float upperBound, float startPoint,
                    std::string rendererPropertyLabel )
  {
    //Set up the slider itself.
    mSliders.at( sliderIndex ).RegisterProperty( "rendererProperty", Property::Value( rendererPropertyLabel ), Property::READ_WRITE );
    mSliders.at( sliderIndex ).SetProperty( Slider::Property::LOWER_BOUND, Property::Value( lowerBound ) );
    mSliders.at( sliderIndex ).SetProperty( Slider::Property::UPPER_BOUND, Property::Value( upperBound ) );
    mSliders.at( sliderIndex ).SetProperty( Slider::Property::VALUE, Property::Value( startPoint ) );
    mSliders.at( sliderIndex ).SetVisible( true );

    //Label the slider with the property.
    //We reset the TextLabel to force a relayout of the table.
    mSliderTable.RemoveChildAt( TableView::CellPosition(sliderIndex, 0) );

    TextLabel sliderLabel = TextLabel::New( rendererPropertyLabel );
    sliderLabel.SetParentOrigin( ParentOrigin::CENTER );
    sliderLabel.SetAnchorPoint( AnchorPoint::CENTER );
    sliderLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

    mSliderTable.AddChild( sliderLabel, TableView::CellPosition( sliderIndex, 0 ) );
    mSliderTable.SetCellAlignment( TableView::CellPosition( sliderIndex, 0 ), HorizontalAlignment::LEFT, VerticalAlignment::CENTER );

    mSliderLabels.at( sliderIndex ).SetVisible( true );
    mSliderLabels.at( sliderIndex) = sliderLabel;
  }

  //Setup snapping to integer values between the two given values.
  void SetupMarks( Slider& slider, int lower, int upper )
  {
    Property::Array marks;

    for( int mark = lower; mark <= upper; mark++ )
    {
      marks.PushBack( Property::Value( mark ) );
    }

    slider.SetProperty( Slider::Property::MARKS, Property::Value( marks ) );
    slider.SetProperty( Slider::Property::SNAP_TO_MARKS, Property::Value( true ) );
  }

  void SetupModel( Layer layer )
  {
    //Create a container to house the renderer-holding actor, to provide a constant hitbox.
    Actor container = Actor::New();
    container.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    container.SetSizeModeFactor( Vector3( 0.9, 0.30, 0.0 ) );
    container.SetParentOrigin( ParentOrigin::CENTER );
    container.SetAnchorPoint( AnchorPoint::CENTER );
    layer.Add( container );

    //Create actor to house the primitive shape renderer.
    mModelActor = Actor::New();
    mModelActor.SetParentOrigin( ParentOrigin::CENTER );
    mModelActor.SetAnchorPoint( AnchorPoint::CENTER);
    mModelActor.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    container.Add( mModelActor );

    //Load default shape.
    LoadCube();

    //Make actor spin to demonstrate 3D.
    mRotationAnimation = Animation::New(15.f);
    mRotationAnimation.AnimateBy( Property( mModelActor, Actor::Property::ORIENTATION ),
                                 Quaternion( Degree( 0.0f ), Degree( 360.0f ), Degree( 0.0f ) ) );
    mRotationAnimation.SetLooping(true);
    mRotationAnimation.Play();

    //Attach gesture detector to pan models when rotated.
    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach( container );
    mPanGestureDetector.DetectedSignal().Connect( this, &PrimitiveShapesController::OnPan );
  }

  void ChangeModel()
  {
    //If a model has been loaded, remove it first.
    if( mShapeRenderer )
    {
      mShapeRenderer.SetOffStage( mModelActor );
    }

    //Load the new model and set it on stage.
    mShapeRenderer = Toolkit::RendererFactory::Get().GetControlRenderer( mRendererMap );
    mShapeRenderer.SetOnStage( mModelActor );
  }

  void LoadCube()
  {
    //Set up renderer properties.
    mRendererMap.Clear();
    mRendererMap[ "rendererType" ] = "primitive";
    mRendererMap[ "color"        ] = mColor;
    mRendererMap[ "shape"        ] = "cube";

    //Set up sliders.
    for( unsigned i = 0; i < mSliders.size(); i++ )
    {
      //We don't use any sliders for properties.
      mSliders.at( i ).SetVisible( false );
      mSliderLabels.at( i ).SetVisible( false );
    }

    ChangeModel();
  }

  void LoadOctahedron()
  {
    //Set up renderer properties.
    mRendererMap.Clear();
    mRendererMap[ "rendererType" ] = "primitive";
    mRendererMap[ "color"        ] = mColor;
    mRendererMap[ "shape"        ] = "octahedron";

    //Set up sliders.
    for( unsigned i = 0; i < mSliders.size(); i++ )
    {
      //We don't use any sliders for properties.
      mSliders.at( i ).SetVisible( false );
      mSliderLabels.at( i ).SetVisible( false );
    }

    ChangeModel();
  }

  void LoadSphere()
  {
    //Set up renderer properties.
    mRendererMap.Clear();
    mRendererMap[ "rendererType" ] = "primitive";
    mRendererMap[ "color"        ] = mColor;
    mRendererMap[ "shape"        ] = "sphere";
    mRendererMap[ "slices"       ] = DEFAULT_SLICES;
    mRendererMap[ "stacks"       ] = DEFAULT_STACKS;

    //Set up sliders.
    for( unsigned i = 0; i < mSliders.size(); i++ )
    {
      if( i == 0 )
      {
        int lowerBound = 3;
        int upperBound = 16;

        SetupSlider( i, lowerBound, upperBound, DEFAULT_STACKS, "slices" );
        SetupMarks( mSliders.at( i ), lowerBound, upperBound );
      }
      else if( i == 1 )
      {
        int lowerBound = 2;
        int upperBound = 16;

        SetupSlider( i, lowerBound, upperBound, DEFAULT_STACKS, "stacks" );
        SetupMarks( mSliders.at( i ), lowerBound, upperBound );
      }
      else
      {
        //We don't use any more sliders for properties.
        mSliders.at( i ).SetVisible( false );
        mSliderLabels.at( i ).SetVisible( false );
      }
    }

    ChangeModel();
  }

  void LoadCone()
  {
    //Set up renderer properties.
    mRendererMap.Clear();
    mRendererMap[ "rendererType"      ] = "primitive";
    mRendererMap[ "color"             ] = mColor;
    mRendererMap[ "shape"             ] = "cone";
    mRendererMap[ "scaleHeight"       ] = DEFAULT_SCALE_HEIGHT;
    mRendererMap[ "scaleBottomRadius" ] = DEFAULT_SCALE_BOTTOM_RADIUS;
    mRendererMap[ "slices"            ] = DEFAULT_SLICES;

    //Set up sliders.
    for( unsigned i = 0; i < mSliders.size(); i++ )
    {
      if( i == 0 )
      {
        SetupSlider( i, 1.0f, 32.0f, DEFAULT_SCALE_HEIGHT, "scaleHeight" );
        mSliders.at( i ).SetProperty( Slider::Property::MARKS, Property::Value( 0 ) ); //Remove marks
      }
      else if( i == 1 )
      {
        SetupSlider( i, 1.0f, 32.0f, DEFAULT_SCALE_BOTTOM_RADIUS, "scaleBottomRadius" );
        mSliders.at( i ).SetProperty( Slider::Property::MARKS, Property::Value( 0 ) ); //Remove marks
      }
      else if( i == 2 )
      {
        int lowerBound = 3;
        int upperBound = 16;

        SetupSlider( i, lowerBound, upperBound, DEFAULT_SLICES, "slices" );
        SetupMarks( mSliders.at( i ), lowerBound, upperBound );
      }
      else
      {
        //We don't use any more sliders for properties.
        mSliders.at( i ).SetVisible( false );
        mSliderLabels.at( i ).SetVisible( false );
      }
    }

    ChangeModel();
  }

  void LoadConicalFrustrum()
  {
    //Set up renderer properties.
    mRendererMap.Clear();
    mRendererMap[ "rendererType"      ] = "primitive";
    mRendererMap[ "color"             ] = mColor;
    mRendererMap[ "shape"             ] = "conicalFrustrum";
    mRendererMap[ "scaleTopRadius"    ] = DEFAULT_SCALE_TOP_RADIUS;
    mRendererMap[ "scaleBottomRadius" ] = DEFAULT_SCALE_BOTTOM_RADIUS;
    mRendererMap[ "scaleHeight"       ] = DEFAULT_SCALE_HEIGHT;
    mRendererMap[ "slices"            ] = DEFAULT_SLICES;

    //Set up sliders.
    for( unsigned i = 0; i < mSliders.size(); i++ )
    {
      if( i == 0 )
      {
        SetupSlider( i, 1.0f, 32.0f, DEFAULT_SCALE_HEIGHT, "scaleHeight" );
        mSliders.at( i ).SetProperty( Slider::Property::MARKS, Property::Value( 0 ) ); //Remove marks
      }
      else if( i == 1 )
      {
        SetupSlider( i, 0.0f, 32.0f, DEFAULT_SCALE_BOTTOM_RADIUS, "scaleBottomRadius" );
        mSliders.at( i ).SetProperty( Slider::Property::MARKS, Property::Value( 0 ) ); //Remove marks
      }
      else if( i == 2 )
      {
        SetupSlider( i, 0.0f, 32.0f, DEFAULT_SCALE_TOP_RADIUS, "scaleTopRadius" );
        mSliders.at( i ).SetProperty( Slider::Property::MARKS, Property::Value( 0 ) ); //Remove marks
      }
      else
      {
        //We don't use any more sliders for properties.
        mSliders.at( i ).SetVisible( false );
        mSliderLabels.at( i ).SetVisible( false );
      }
    }

    ChangeModel();
  }

  void LoadCylinder()
  {
    //Set up renderer properties.
    mRendererMap.Clear();
    mRendererMap[ "rendererType" ] = "primitive";
    mRendererMap[ "color"        ] = mColor;
    mRendererMap[ "shape"        ] = "cylinder";
    mRendererMap[ "scaleHeight"  ] = DEFAULT_SCALE_HEIGHT;
    mRendererMap[ "scaleRadius"  ] = DEFAULT_SCALE_RADIUS;
    mRendererMap[ "slices"       ] = DEFAULT_SLICES;

    //Set up sliders.
    for( unsigned i = 0; i < mSliders.size(); i++ )
    {
      if( i == 0 )
      {
        SetupSlider( i, 1.0f, 32.0f, DEFAULT_SCALE_HEIGHT, "scaleHeight" );
        mSliders.at( i ).SetProperty( Slider::Property::MARKS, Property::Value( 0 ) ); //Remove marks
      }
      else if( i == 1 )
      {
        SetupSlider( i, 1.0f, 32.0f, DEFAULT_SCALE_RADIUS, "scaleRadius" );
        mSliders.at( i ).SetProperty( Slider::Property::MARKS, Property::Value( 0 ) ); //Remove marks
      }
      else if( i == 2 )
      {
        int lowerBound = 3;
        int upperBound = 16;

        SetupSlider( i, lowerBound, upperBound, DEFAULT_SLICES, "slices" );
        SetupMarks( mSliders.at( i ), lowerBound, upperBound );
      }
      else
      {
        //We don't use any more sliders for properties.
        mSliders.at( i ).SetVisible( false );
        mSliderLabels.at( i ).SetVisible( false );
      }
    }

    ChangeModel();
  }

  void LoadBevelledCube()
  {
    //Set up renderer properties.
    mRendererMap.Clear();
    mRendererMap[ "rendererType"    ] = "primitive";
    mRendererMap[ "color"           ] = mColor;
    mRendererMap[ "shape"           ] = "bevelledCube";
    mRendererMap[ "bevelPercentage" ] = DEFAULT_BEVEL_PERCENTAGE;
    mRendererMap[ "bevelSmoothness" ] = DEFAULT_BEVEL_SMOOTHNESS;

    //Set up sliders.
    for( unsigned i = 0; i < mSliders.size(); i++ )
    {
      if( i == 0 )
      {
        SetupSlider( i, 0.0f, 1.0f, DEFAULT_BEVEL_PERCENTAGE, "bevelPercentage" );
        mSliders.at( i ).SetProperty( Slider::Property::MARKS, Property::Value( 0 ) ); //Remove marks
      }
      else if( i == 1 )
      {
        SetupSlider( i, 0.0f, 1.0f, DEFAULT_BEVEL_SMOOTHNESS, "bevelSmoothness" );
        mSliders.at( i ).SetProperty( Slider::Property::MARKS, Property::Value( 0 ) ); //Remove marks
      }
      else
      {
        //We don't use any more sliders for properties.
        mSliders.at( i ).SetVisible( false );
        mSliderLabels.at( i ).SetVisible( false );
      }
    }

    ChangeModel();
  }

  //Switch to the selected shape.
  bool OnChangeShapeClicked( Button button )
  {
    int modelNumber;
    button.GetProperty( button.GetPropertyIndex( "modelNumber" ) ).Get( modelNumber );

    switch( modelNumber )
    {
      case 0:
      {
        LoadSphere();
        break;
      }
      case 1:
      {
        LoadCone();
        break;
      }
      case 2:
      {
        LoadConicalFrustrum();
        break;
      }
      case 3:
      {
        LoadCylinder();
        break;
      }
      case 4:
      {
        LoadCube();
        break;
      }
      case 5:
      {
        LoadBevelledCube();
        break;
      }
      case 6:
      {
        LoadOctahedron();
        break;
      }
      default:
      {
        break;
      }
    }

    return true;
  }

  bool OnSliderValueChanged( Slider slider, float value )
  {
    std::string rendererPropertyLabel;
    slider.GetProperty( slider.GetPropertyIndex( "rendererProperty" ) ).Get( rendererPropertyLabel );
    mRendererMap[ rendererPropertyLabel ] = value;

    ChangeModel();

    return true;
  }

  void OnPan( Actor actor, const PanGesture& gesture )
  {
    switch( gesture.state )
    {
      case Gesture::Started:
      {
        //Pause animation, as the gesture will be used to manually rotate the model
        mRotationAnimation.Pause();

        break;
      }
      case Gesture::Continuing:
      {
        //Rotate based off the gesture.
        mRotation.x -= gesture.displacement.y / X_ROTATION_DISPLACEMENT_FACTOR; // Y displacement rotates around X axis
        mRotation.y += gesture.displacement.x / Y_ROTATION_DISPLACEMENT_FACTOR; // X displacement rotates around Y axis
        Quaternion rotation = Quaternion( Radian( mRotation.x ), Vector3::XAXIS) *
                              Quaternion( Radian( mRotation.y ), Vector3::YAXIS);

        mModelActor.SetOrientation( rotation );

        break;
      }
      case Gesture::Finished:
      {
        //Return to automatic animation
        mRotationAnimation.Play();

        break;
      }
      case Gesture::Cancelled:
      {
        //Return to automatic animation
        mRotationAnimation.Play();

        break;
      }
      default:
      {
        break;
      }
    }
  }

private:
  Application&  mApplication;

  //Holds the sliders and their labels that each shape needs to access.
  std::vector<Slider> mSliders;
  std::vector<TextLabel> mSliderLabels;

  //A table to layout the sliders next to their labels.
  TableView mSliderTable;

  //For creating the 3D shape.
  Property::Map mRendererMap;
  ControlRenderer mShapeRenderer;
  Actor mModelActor;

  //Used to manually rotate the model through panning gestures.
  PanGestureDetector mPanGestureDetector;
  Animation mRotationAnimation;
  Vector2 mRotation;

  //Color to set all shapes.
  Vector4 mColor;
};

void RunTest( Application& application )
{
  PrimitiveShapesController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
