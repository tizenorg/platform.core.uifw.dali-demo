/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/dali-toolkit.h>
#include "shared/view.h"
#include "scene-graph-editor.h"
#include "scene-graph-editor-impl.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Demo;

namespace
{
#if 0
const char* TEST_OUTER_RING_FILENAME = DEMO_IMAGE_DIR "layer2.png"; // Image to be masked
const char* TEST_INNER_RING_FILENAME = DEMO_IMAGE_DIR "layer1.png"; // Image to be masked
const char* TEST_MENU_FILENAME = DEMO_IMAGE_DIR "layer3.png"; // Image to be masked
const char* TEST_DIAL_FILENAME = DEMO_IMAGE_DIR "layer4.png"; // Image to be masked
#endif
const char* TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" ); // Background for toolbar
const char* APPLICATION_TITLE( "Radial Menu" );
const char * const PLAY_ICON( DEMO_IMAGE_DIR "icon-play.png" );
const char * const PLAY_ICON_SELECTED( DEMO_IMAGE_DIR "icon-play-selected.png" );
const char * const STOP_ICON( DEMO_IMAGE_DIR "icon-stop.png" );
const char * const STOP_ICON_SELECTED( DEMO_IMAGE_DIR "icon-stop-selected.png" );
}


/********************************************************************************
 * Application controller class
 */

// This example shows how to create a mesh actor for use as a stencil buffer
class SceneGraphEditExample : public ConnectionTracker
{
public:
  /**
   * Constructor
   * @param[in] app The application handle
   */
  SceneGraphEditExample( Application app );

  /**
   * Destructor
   */
  ~SceneGraphEditExample();

private:

  /**
   * Initialization signal handler - all actor initialization should happen here
   * @param[in] app The application handle
   */
  void OnInit(Application& app);

  /**
   * Create a sweep view with the given image and parameters
   */
  SceneGraphEditor CreateSweepView( std::string imageName, Degree initial, Degree final );

  /**
   * Start the sweep animation on the menu
   */
  void StartAnimation();

  /**
   * Play or pause the animation when the button is clicked
   */
  bool OnButtonClicked( Toolkit::Button button );

  /**
   * Update the state flag and change the button icon when the animation is finished
   */
  void OnAnimationFinished( Animation& source );

  /**
   * Main key event handler
   *
   * @param[in] event The key event to respond to
   */
  void OnKeyEvent(const KeyEvent& event);

private: // Member variables
  enum AnimState
  {
    STOPPED,
    PAUSED,
    PLAYING
  };

  Application     mApplication; ///< The application handle
  Toolkit::Control mView;        ///< The toolbar view
  Layer           mContents;    ///< The toolbar contents pane
  ImageView       mImageView;  ///< Image view shown by stencil mask
  Animation       mAnimation;
  AnimState       mAnimationState;

  Toolkit::PushButton mPlayStopButton;
  ImageView       mDialView;

  SceneGraphEditor mEditor;

  //SceneGraphEditor mSceneGraphEditor1;
  //SceneGraphEditor mSceneGraphEditor2;
  //SceneGraphEditor mSceneGraphEditor3;
};

SceneGraphEditExample::SceneGraphEditExample(Application app)
: mApplication( app ),
  mAnimationState(STOPPED)
{
  // Connect to the Application's Init signal
  app.InitSignal().Connect(this, &SceneGraphEditExample::OnInit);
}

SceneGraphEditExample::~SceneGraphEditExample()
{
  // Nothing to do here; actor handles will clean up themselves.
}

void SceneGraphEditExample::OnInit(Application& app)
{
  Stage stage = Dali::Stage::GetCurrent();

  // The Init signal is received once (only) during the Application lifetime
  stage.KeyEventSignal().Connect(this, &SceneGraphEditExample::OnKeyEvent);

  // Create toolbar & view
  Toolkit::ToolBar toolBar;
  mContents = DemoHelper::CreateView( mApplication,
                                      mView,
                                      toolBar,
                                      "",
                                      TOOLBAR_IMAGE,
                                      APPLICATION_TITLE );

  mPlayStopButton = Toolkit::PushButton::New();
  mPlayStopButton.SetUnselectedImage( STOP_ICON );
  mPlayStopButton.SetSelectedImage( STOP_ICON_SELECTED );

  mPlayStopButton.ClickedSignal().Connect( this, &SceneGraphEditExample::OnButtonClicked );

  toolBar.AddControl( mPlayStopButton,
                      DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage,
                      Toolkit::Alignment::HorizontalRight,
                      DemoHelper::DEFAULT_PLAY_PADDING );



  mEditor = SceneGraphEditor::New();
  mEditor.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  mEditor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mEditor.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  stage.Add( mEditor );
  mEditor.Deploy();

#if 0
  Path path = Path::New();
  Dali::Property::Array points;
      points.Resize(3);
  //Dali::Property::Array controlPoints;
  //    controlPoints.Resize(4);
  //Vector3 forward;
  //if( mEffectMode == PageCarouselEffect)
  {
    Vector2 stageSize = Stage::GetCurrent().GetSize();
    points[0] = Vector3( stageSize.x*0.75, 0.0f,  -stageSize.x*0.75f);
    points[1] = Vector3( 0.0f, 0.0f, 0.0f );
    points[2] = Vector3( -stageSize.x*0.75f, 0.0f,  -stageSize.x*0.75f);
    path.SetProperty( Path::Property::POINTS, points );

#if 0
    controlPoints[0] = Vector3( stageSize.x*0.5f, 0.0f, 0.0f );
    controlPoints[1] = Vector3( stageSize.x*0.5f, 0.0f, 0.0f );
    controlPoints[2] = Vector3(-stageSize.x*0.5f, 0.0f, 0.0f );
    controlPoints[3] = Vector3(-stageSize.x*0.5f, 0.0f, 0.0f );
    path.SetProperty( Path::Property::CONTROL_POINTS, controlPoints );

    forward = Vector3::ZERO;
#endif
  }
  path.GenerateControlPoints( 0.25f );

#if 0
  * @SINCE_1_0.0
  * @param[in]  progress  A floating point value between 0.0 and 1.0.
  * @param[out] position The interpolated position at that progress.
  * @param[out] tangent The interpolated tangent at that progress.
  */
 void Sample( float progress, Vector3& position, Vector3& tangent ) const;
#endif
  Vector3 position;
  Vector3 tangent;
  for( int i = 0; i <= 10; ++i )
  {
    float p = (float)i / 10.0f;
    path.Sample( p, position, tangent );
    std::cout << "PATH: p:" << p << "  pos:" << position << "  tangent:" << tangent << std::endl;
  }
#endif










#if 0
  const ImageDimensions intImgSize = ResourceImage::GetImageSize(TEST_OUTER_RING_FILENAME);
  Vector2 imgSize = Vector2( intImgSize.GetWidth(), intImgSize.GetHeight() );
  Vector2 stageSize = stage.GetSize();
  float scale = stageSize.width / imgSize.width;
  float availableHeight = stageSize.height - DemoHelper::DEFAULT_VIEW_STYLE.mToolBarHeight * 2.0f;
  if(availableHeight <= stageSize.width)
  {
    scale = availableHeight / imgSize.width;
  }

  mSceneGraphEditor1 = CreateSweepView( TEST_OUTER_RING_FILENAME, Degree(-90.0f), Degree(-90.0f));
  mSceneGraphEditor2 = CreateSweepView( TEST_INNER_RING_FILENAME, Degree(90.0f),  Degree(0.0f));
  mSceneGraphEditor3 = CreateSweepView( TEST_MENU_FILENAME, Degree(100.0f), Degree(0.0f));
  mSceneGraphEditor3.SetInitialActorAngle(Degree(-110));
  mSceneGraphEditor3.SetFinalActorAngle(Degree(0));

  Image dial = ResourceImage::New( TEST_DIAL_FILENAME );
  mDialView = ImageView::New( dial );
  mDialView.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
  mDialView.SetParentOrigin( ParentOrigin::CENTER );
  mDialView.SetScale(scale);
  Layer dialLayer = Layer::New();

  dialLayer.Add( mDialView );
  dialLayer.SetParentOrigin( ParentOrigin::CENTER );
  dialLayer.SetSize(stage.GetSize());
  mContents.Add(dialLayer);

  mSceneGraphEditor1.SetScale(scale);
  mSceneGraphEditor2.SetScale(scale);
  mSceneGraphEditor3.SetScale(scale);

  StartAnimation();
#endif
}

void SceneGraphEditExample::StartAnimation()
{
#if 0
  mDialView.SetOpacity(0.0f);
  mSceneGraphEditor1.SetOpacity(0.0f);
  mAnimation = Animation::New(6.0f);
  mSceneGraphEditor1.Activate(mAnimation, 0.0f, 3.0f);
  mSceneGraphEditor2.Activate(mAnimation, 1.5f, 3.0f);
  mSceneGraphEditor3.Activate(mAnimation, 3.0f, 3.0f);
  mAnimation.AnimateTo( Property( mDialView, Actor::Property::COLOR_ALPHA ), 1.0f, AlphaFunction::EASE_IN, TimePeriod( 0.0f, 0.8f ) );
  mAnimation.AnimateTo( Property( mSceneGraphEditor1, Actor::Property::COLOR_ALPHA ), 1.0f, AlphaFunction::EASE_IN, TimePeriod( 0.0f, 0.5f ) );
  mAnimation.FinishedSignal().Connect( this, &SceneGraphEditExample::OnAnimationFinished );

  mAnimationState = PLAYING;
  mAnimation.Play();
#endif
}

bool SceneGraphEditExample::OnButtonClicked( Toolkit::Button button )
{
#if 0
  switch( mAnimationState )
  {
    case PLAYING:
    {
      mAnimation.Pause();
      mAnimationState = PAUSED;
      mPlayStopButton.SetUnselectedImage( PLAY_ICON );
      mPlayStopButton.SetSelectedImage( PLAY_ICON_SELECTED );
    }
    break;

    case PAUSED:
    {
      mAnimation.Play();
      mAnimationState = PLAYING;
      mPlayStopButton.SetUnselectedImage( STOP_ICON );
      mPlayStopButton.SetSelectedImage( STOP_ICON_SELECTED );
    }
    break;

    case STOPPED:
    {
      mPlayStopButton.SetUnselectedImage( STOP_ICON );
      mPlayStopButton.SetSelectedImage( STOP_ICON_SELECTED );
      mSceneGraphEditor1.Deactivate();
      mSceneGraphEditor2.Deactivate();
      mSceneGraphEditor3.Deactivate();
      StartAnimation();
    }
  }
#endif
  return false;
}

void SceneGraphEditExample::OnAnimationFinished( Animation& source )
{
  mAnimationState = STOPPED;
  mPlayStopButton.SetUnselectedImage( PLAY_ICON );
  mPlayStopButton.SetSelectedImage( PLAY_ICON_SELECTED );
}

SceneGraphEditor SceneGraphEditExample::CreateSweepView( std::string imageName,
                                                    Degree initialAngle,
                                                    Degree finalAngle)
{
#if 0
  // Create the image
  Image image = ResourceImage::New(imageName);
  mImageView = ImageView::New(image);
  mImageView.SetParentOrigin(ParentOrigin::CENTER);
  mImageView.SetAnchorPoint(AnchorPoint::CENTER);
  mImageView.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

  // Create the stencil
  const ImageDimensions imageSize = ResourceImage::GetImageSize(imageName);
  float diameter = std::max(imageSize.GetWidth(), imageSize.GetHeight());
  SceneGraphEditor radialSweepView = SceneGraphEditor::New();
  radialSweepView.SetDiameter( diameter );
  radialSweepView.SetInitialAngle( initialAngle );
  radialSweepView.SetFinalAngle( finalAngle );
  radialSweepView.SetInitialSector( Degree(0.0f) );
  radialSweepView.SetFinalSector( Degree(359.999f) );
  radialSweepView.SetSize( Stage::GetCurrent().GetSize());
  radialSweepView.SetEasingFunction( Dali::AlphaFunction::EASE_IN_OUT );
  radialSweepView.SetParentOrigin( ParentOrigin::CENTER );
  mContents.Add(radialSweepView);
  radialSweepView.Add( mImageView );
  mImageView.SetParentOrigin( ParentOrigin::CENTER );
#endif
  return SceneGraphEditor();
}


void SceneGraphEditExample::OnKeyEvent( const KeyEvent& event )
{
  if(event.state == KeyEvent::Down)
  {
    if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
    {
      mApplication.Quit();
    }
  }
}

void RunTest( Application app )
{
  SceneGraphEditExample test( app );

  app.MainLoop();
}

// Entry point for Linux & Tizen applications
int main( int argc, char **argv )
{
  Application app = Application::New( &argc, &argv, DEMO_THEME_PATH );

  RunTest( app );

  return 0;
}
