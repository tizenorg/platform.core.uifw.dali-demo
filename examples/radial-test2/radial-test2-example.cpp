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
#include <dali-toolkit/dali-toolkit.h>
#include "../shared/view.h"

#include <sstream>

//#define ENABLE_TRANSITION_EFFECT

#define TEST_INCREASE_ANGLE
//#define TEST_INCREASE_STEP

#if defined (TEST_INCREASE_ANGLE)
#define TEST_CASE_1
#elif defined (TEST_INCREASE_STEP)
#define TEST_CASE_1
#else
#define TEST_CASE_1
#endif

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* TEST_OUTER_RING_FILENAME = DALI_IMAGE_DIR "layer8.png"; // Image to be masked
const char* TEST_BG_FILENAME = DALI_IMAGE_DIR "layer11.png"; // Image to be masked
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" ); // Background for toolbar
const char* APPLICATION_TITLE( "Radial Test 2" );

#ifdef ENABLE_TRANSITION_EFFECT
const char* TEST_ICON_MASK_FILENAME = DALI_IMAGE_DIR "layer5.png"; // Image to be masked
const char* TEST_ICON1_FILENAME = DALI_IMAGE_DIR "layer5-1.png"; // Image to be masked
const char* TEST_ICON2_FILENAME = DALI_IMAGE_DIR "layer5-2.png"; // Image to be masked

const char* STENCIL_FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
	void main()
	{
        mediump vec4 color = texture2D(sTexture, vTexCoord);
		if (color.a != 0.0)
		{
			gl_FragColor = color;
		}
		else
		{
			discard;
		}
	}
);
#endif

const char* BG_FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
    void main()
    {
        mediump vec4 color = texture2D(sTexture, vTexCoord);
        gl_FragColor = color * vec4(0.0, 0.0, 0.0, 1.0);
    }
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
	void main()
	{
        mediump vec4 color = texture2D(sTexture, vTexCoord);
		gl_FragColor = color * vec4(0.0, 0.7, 0.2, 1.0);
	}
);

TextStyle GetTextStyle(float size, bool bBold, Vector4 color)
{
  TextStyle textStyle;
  textStyle.SetFontName("HelveticaNeue");
  textStyle.SetFontStyle("Regular");
  textStyle.SetFontPointSize( Dali::PointSize(DemoHelper::ScalePointSize(size)));
  textStyle.SetTextColor(color);
  if (bBold)
  {
	  textStyle.SetWeight(Dali::TextStyle::BOLD);
  }
  else
  {
	  textStyle.SetWeight(Dali::TextStyle::REGULAR);
  }
  return textStyle;
}
}

/********************************************************************************
 * Application controller class
 */

// This example shows how to create a mesh actor for use as a stencil buffer
class RadialTest2Example : public ConnectionTracker
{
public:
  /**
   * Constructor
   * @param[in] app The application handle
   */
  RadialTest2Example(Application app);

  /**
   * Destructor
   */
  ~RadialTest2Example();

private:

  /**
   * Initialization signal handler - all actor initialization should happen here
   * @param[in] app The application handle
   */
  void OnInit(Application& app);

  /**
   * Create a sweep view with the given image and parameters
   */
  Toolkit::RadialSweepView CreateSweepView( std::string imageName );

  void AddCircleView(void);
  void AddRadialView(void);

  void StartAnimation1();
  void StartAnimation2();
  void StartAnimation3();

  void OnAnimationFinished1( Animation& source );
  void OnAnimationFinished2( Animation& source );
  void OnAnimationFinished3( Animation& source );

  bool OnButtonClicked( Toolkit::Button button );

  bool OnAnimationTick(void);

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
  Toolkit::View   mView;        ///< The toolbar view
  Layer           mContents;    ///< The toolbar contents pane
  ImageActor      mImageActor;  ///< Image actor shown by stencil mask
  Animation       mAnimation1;
  Animation       mAnimation2;
  AnimState       mAnimationState;

  ImageActor      mIconActor1;
  ImageActor      mIconActor2;
  ImageActor      mStencilMaskActor;
  ImageActor      mInnerCircleActor;
  ImageActor      mBgActor1;
  ImageActor      mBgActor2;
  MeshActor       mMeshActor;
  Toolkit::RadialSweepView mRadialSweepView1;

  Toolkit::TextView   mTitleTextView;
  Toolkit::TextView   mTextView1;
  Toolkit::TextView   mTextView2;
  Toolkit::TextView   mTextView3;

  Timer		mAnimationTimer;

  int		mPedometerNumber;
  int		mCalorie;
  float		mDistance;
};

RadialTest2Example::RadialTest2Example(Application app)
: mApplication( app ),
  mAnimationState(STOPPED)
{
  // Connect to the Application's Init signal
  app.InitSignal().Connect(this, &RadialTest2Example::OnInit);
}

RadialTest2Example::~RadialTest2Example()
{
  // Nothing to do here; actor handles will clean up themselves.
}

void RadialTest2Example::OnInit(Application& app)
{
  Stage stage = Dali::Stage::GetCurrent();

  stage.SetBackgroundColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

  // The Init signal is received once (only) during the Application lifetime
  stage.KeyEventSignal().Connect(this, &RadialTest2Example::OnKeyEvent);

  // Create toolbar & view
  Toolkit::ToolBar toolBar;
  mContents = DemoHelper::CreateView( mApplication,
                                      mView,
                                      toolBar,
                                      "",
                                      TOOLBAR_IMAGE,
                                      APPLICATION_TITLE );

#ifdef ENABLE_TRANSITION_EFFECT
  Vector2 stageSize = stage.GetSize();
  float minStageDimension = std::min(stageSize.width, stageSize.height);

  if(stageSize.height <= stageSize.width)
  {
    minStageDimension -= DemoHelper::DEFAULT_VIEW_STYLE.mToolBarHeight * 2.0f;
  }

  Vector2 iconSize = Image::GetImageSize(TEST_ICON1_FILENAME);

  Image icon1 = Image::New(TEST_ICON1_FILENAME);
  mIconActor1 = ImageActor::New(icon1);
  mIconActor1.SetSize(iconSize.width, iconSize.height);
  mIconActor1.SetScale(minStageDimension / iconSize.width);
  mIconActor1.SetParentOrigin(ParentOrigin::CENTER);
  mIconActor1.SetAnchorPoint(AnchorPoint::BOTTOM_CENTER);
  mContents.Add(mIconActor1);

  Image icon2 = Image::New(TEST_ICON2_FILENAME);
  mIconActor2 = ImageActor::New(icon2);
  mIconActor2.SetSize(iconSize.width, iconSize.height);
  mIconActor2.SetScale(minStageDimension / iconSize.width);
  mIconActor2.SetParentOrigin(ParentOrigin::CENTER);
  mIconActor2.SetAnchorPoint(AnchorPoint::TOP_CENTER);
  mContents.Add(mIconActor2);

  ShaderEffect maskShader = ShaderEffect::New("", STENCIL_FRAGMENT_SHADER);

  Image stencilMask = Image::New(TEST_ICON_MASK_FILENAME);
  mStencilMaskActor = ImageActor::New(stencilMask);
  mStencilMaskActor.SetScale(minStageDimension / iconSize.width);
  mStencilMaskActor.SetPositionInheritanceMode(USE_PARENT_POSITION);
  mStencilMaskActor.SetDrawMode(DrawMode::STENCIL);
  mStencilMaskActor.SetShaderEffect(maskShader);
  mContents.Add(mStencilMaskActor);

  StartAnimation1();
#else
  AddCircleView();
#endif
}

void RadialTest2Example::StartAnimation1()
{
#ifdef ENABLE_TRANSITION_EFFECT
  Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
  float minStageDimension = std::min(stageSize.width, stageSize.height);

  if(stageSize.height <= stageSize.width)
  {
	minStageDimension -= DemoHelper::DEFAULT_VIEW_STYLE.mToolBarHeight * 2.0f;
  }

  mAnimation1 = Animation::New(1.0f);

  Vector3 size = mIconActor1.GetSize();
  mAnimation1.AnimateTo(Property(mIconActor1, Actor::POSITION_Y), -(size.height * minStageDimension / size.width), TimePeriod(0.5f, 0.5f));
  mAnimation1.AnimateTo(Property(mIconActor2, Actor::POSITION_Y), size.height * minStageDimension / size.width, TimePeriod(0.5f, 0.5f));

  mAnimation1.FinishedSignal().Connect( this, &RadialTest2Example::OnAnimationFinished1 );

  mAnimationState = PLAYING;

  mAnimation1.Play();
#endif
}

void RadialTest2Example::OnAnimationFinished1( Animation& source )
{
#ifdef ENABLE_TRANSITION_EFFECT
	mIconActor1.Unparent();
	mIconActor2.Unparent();
	mStencilMaskActor.Unparent();

	AddCircleView();
#endif
}

void RadialTest2Example::AddCircleView(void)
{
	Image innerCircle = Image::New(TEST_BG_FILENAME);
	mInnerCircleActor = ImageActor::New(innerCircle);
	mInnerCircleActor.SetPositionInheritanceMode(USE_PARENT_POSITION);

    ShaderEffect shader = ShaderEffect::New("", BG_FRAGMENT_SHADER);
    mInnerCircleActor.SetShaderEffect(shader);

	mContents.Add(mInnerCircleActor);

	StartAnimation2();
}

void RadialTest2Example::StartAnimation2()
{
	Vector2 bgImgSize = Image::GetImageSize(TEST_BG_FILENAME);
	Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
	float minStageDimension = std::min(stageSize.width, stageSize.height);

	if(stageSize.height <= stageSize.width)
	{
	  minStageDimension -= DemoHelper::DEFAULT_VIEW_STYLE.mToolBarHeight * 2.0f;
	}

	float scale = minStageDimension / bgImgSize.width;

	mInnerCircleActor.SetOpacity(0.0f);
	mInnerCircleActor.SetScale(0.0f);

	mAnimation2 = Animation::New(0.5f);

	mAnimation2.OpacityTo(mInnerCircleActor, 1.0f);
	mAnimation2.AnimateTo(Property(mInnerCircleActor, Actor::SCALE), Vector3(scale, scale, 1.0f));

	mAnimation2.FinishedSignal().Connect( this, &RadialTest2Example::OnAnimationFinished2 );

	mAnimation2.Play();
}

void RadialTest2Example::OnAnimationFinished2( Animation& source )
{
	AddRadialView();
}

void RadialTest2Example::AddRadialView(void)
{
	Vector2 imgSize = Image::GetImageSize(TEST_OUTER_RING_FILENAME);
	Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
	float minStageDimension = std::min(stageSize.width, stageSize.height);

	if(stageSize.height <= stageSize.width)
	{
	  minStageDimension -= DemoHelper::DEFAULT_VIEW_STYLE.mToolBarHeight * 2.0f;
	}

	float scale = minStageDimension / imgSize.width;

    mRadialSweepView1 = CreateSweepView(TEST_OUTER_RING_FILENAME);
    mRadialSweepView1.SetScale(scale);

	// Text part
	mTitleTextView = TextView::New("Pedometer");
	mTitleTextView.SetParentOrigin(ParentOrigin::CENTER);
	mTitleTextView.SetAnchorPoint(AnchorPoint::CENTER);
	mTitleTextView.SetSize(200.0f, 50.0f);
	mTitleTextView.SetPosition(0.0f, -100.0f);
	mTitleTextView.SetStyleToCurrentText(GetTextStyle(12.0f, true, Vector4(1.0f, 1.0f, 1.0f, 1.0f)));
	mTitleTextView.SetSnapshotModeEnabled(true);
	mContents.Add(mTitleTextView);

	mBgActor1 = Toolkit::CreateSolidColorActor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	mBgActor1.SetParentOrigin(ParentOrigin::CENTER);
	mBgActor1.SetAnchorPoint(AnchorPoint::CENTER);
	mBgActor1.SetSize(200.0f, 200.0f);
	mContents.Add(mBgActor1);

	mTextView1 = TextView::New("0");
	mTextView1.SetParentOrigin(ParentOrigin::CENTER);
	mTextView1.SetAnchorPoint(AnchorPoint::CENTER);
	mTextView1.SetSize(200.0f, 100.0f);
	mTextView1.SetPosition(0.0f, -30.0f);
	mTextView1.SetStyleToCurrentText(GetTextStyle(30.0f, true, Vector4(0.0f, 0.7f, 0.2f, 1.0f)));
	mTextView1.SetSnapshotModeEnabled(false);
	mContents.Add(mTextView1);

	mTextView2 = TextView::New("Steps");
	mTextView2.SetParentOrigin(ParentOrigin::CENTER);
	mTextView2.SetAnchorPoint(AnchorPoint::CENTER);
	mTextView2.SetSize(100.0f, 50.0f);
	mTextView2.SetPosition(0.0f, 80.0f);
	mTextView2.SetStyleToCurrentText(GetTextStyle(8.0f, true, Vector4(0.5f, 0.5f, 0.5f, 1.0f)));
	mTextView2.SetSnapshotModeEnabled(true);
	mContents.Add(mTextView2);

	mBgActor2 = Toolkit::CreateSolidColorActor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	mBgActor2.SetParentOrigin(ParentOrigin::CENTER);
	mBgActor2.SetAnchorPoint(AnchorPoint::CENTER);
	mBgActor2.SetSize(100.0f, 50.0f);
	mBgActor2.SetPosition(0.0f, 80.0f);
	mContents.Add(mBgActor2);

	mTextView3 = TextView::New("0 km 0 Cal");
	mTextView3.SetParentOrigin(ParentOrigin::CENTER);
	mTextView3.SetAnchorPoint(AnchorPoint::CENTER);
	mTextView3.SetSize(200.0f, 100.0f);
	mTextView3.SetPosition(0.0f, 100.0f);
	mTextView3.SetStyleToCurrentText(GetTextStyle(10.0f, false, Vector4(1.0f, 1.0f, 1.0f, 1.0f)));
	mTextView3.SetSnapshotModeEnabled(false);
	mContents.Add(mTextView3);

	mPedometerNumber = 0;
	mCalorie = 0;
	mDistance = 0.0f;

	// Mesh
	MeshData::VertexContainer vertices(4);
    vertices[0] = MeshData::Vertex(Vector3(-0.5f, -0.5f, 0.0f), Vector2::ZERO, Vector3(0.6f, 0.6f, 0.6f));
    vertices[1] = MeshData::Vertex(Vector3( 0.5f, -0.5f, 0.0f), Vector2::ZERO, Vector3(0.6f, 0.6f, 0.6f));
    vertices[2] = MeshData::Vertex(Vector3(-0.5f,  0.5f, 0.0f), Vector2::ZERO, Vector3(0.6f, 0.6f, 0.6f));
    vertices[3] = MeshData::Vertex(Vector3( 0.5f,  0.5f, 0.0f), Vector2::ZERO, Vector3(0.6f, 0.6f, 0.6f));

	AnimatableMesh::Faces faces;
	faces.reserve(6);
	faces.push_back(0); faces.push_back(3); faces.push_back(1);
	faces.push_back(0); faces.push_back(2); faces.push_back(3);

	MeshData meshData;
    meshData.SetMaterial(Material::New("RectActorMaterial"));
	meshData.SetVertices(vertices);
	meshData.SetFaceIndices(faces);
	meshData.SetHasColor(true);

	mMeshActor = MeshActor::New(Mesh::New(meshData));
	mMeshActor.SetParentOrigin(ParentOrigin::CENTER);
	mMeshActor.SetAnchorPoint(AnchorPoint::CENTER);
	mMeshActor.SetAffectedByLighting(false);
	mMeshActor.SetPosition(0.0f, -100.0f);
    mMeshActor.SetScale(0.0f, 1.0f, 1.0f);
	mContents.Add(mMeshActor);

	StartAnimation3();
}

void RadialTest2Example::StartAnimation3()
{
    Animation animation = Animation::New(1.0f);

#if defined (TEST_INCREASE_ANGLE)

#if defined (TEST_CASE_1)
    mRadialSweepView1.SetStartAngle(-150.0f);
    mRadialSweepView1.SetEndAngle(-150.0f);
//    mRadialSweepView1.SetDirection(RadialSweepView::ANTI_CLOCKWISE);

    animation.AnimateTo(Property(mRadialSweepView1, RadialSweepView::END_ANGLE_PROPERTY_NAME), 90.0f);
#elif defined (TEST_CASE_2)
    mRadialSweepView1.SetStartAngle(-150.0f);
    mRadialSweepView1.SetEndAngle(210.0f);
//    mRadialSweepView1.SetDirection(RadialSweepView::ANTI_CLOCKWISE);

    animation.AnimateTo(Property(mRadialSweepView1, RadialSweepView::END_ANGLE_PROPERTY_NAME), 90.0f);
#elif defined (TEST_CASE_3)
    mRadialSweepView1.SetStartAngle(0.0f);
    mRadialSweepView1.SetEndAngle(270.0f);
//    mRadialSweepView1.SetDirection(RadialSweepView::ANTI_CLOCKWISE);

    animation.AnimateTo(Property(mRadialSweepView1, RadialSweepView::START_ANGLE_PROPERTY_NAME), 90.0f);
    animation.AnimateTo(Property(mRadialSweepView1, RadialSweepView::END_ANGLE_PROPERTY_NAME), 180.0f);
#elif defined (TEST_CASE_4)
    mRadialSweepView1.SetStartAngle(0.0f);
    mRadialSweepView1.SetEndAngle(359.9999f);

    animation.AnimateTo(Property(mRadialSweepView1, Actor::SCALE), Vector3(1.4f, 1.4f, 1.4f));
#endif

#elif defined (TEST_INCREASE_STEP)

#if defined (TEST_CASE_1)
    mRadialSweepView1.SetStepCount(12);
    mRadialSweepView1.SetCurrentStep(0.0f);

    animation.AnimateTo(Property(mRadialSweepView1, RadialSweepView::CURRENT_STEP_PROPERTY_NAME), 7.0f);
#elif defined (TEST_CASE_2)
    mRadialSweepView1.SetStepCount(12);
    mRadialSweepView1.SetCurrentStep(5.0f);

    animation.AnimateTo(Property(mRadialSweepView1, RadialSweepView::CURRENT_STEP_PROPERTY_NAME), 0.0f);
#endif

#else

#if defined (TEST_CASE_1)
    mRadialSweepView1.SetStepCount(12);
    mRadialSweepView1.SetCurrentStep(1.0f);

    animation.AnimateTo(Property(mRadialSweepView1, RadialSweepView::CURRENT_STEP_PROPERTY_NAME), 7.0f);
#elif defined (TEST_CASE_2)
    mRadialSweepView1.SetStepCount(8);
    mRadialSweepView1.SetCurrentStep(3.0f);

    animation.AnimateTo(Property(mRadialSweepView1, RadialSweepView::CURRENT_STEP_PROPERTY_NAME), 1.0f);
#endif

#endif

	animation.AnimateTo(Property(mTitleTextView, Actor::POSITION_Y), -125.0f, TimePeriod(0.5f));
	animation.AnimateTo(Property(mTextView2, Actor::POSITION_Y), 40.0f, TimePeriod(0.5f));
	animation.AnimateTo(Property(mMeshActor, Actor::SCALE_X), 230.0f, TimePeriod(0.5f));

	animation.FinishedSignal().Connect(this, &RadialTest2Example::OnAnimationFinished3);

	mAnimationTimer = Timer::New(80);
	mAnimationTimer.TickSignal().Connect(this, &RadialTest2Example::OnAnimationTick);
	mAnimationTimer.Start();

	animation.Play();
}

void RadialTest2Example::OnAnimationFinished3( Animation& source )
{
  mAnimationState = STOPPED;
}

bool RadialTest2Example::OnAnimationTick(void)
{
	mPedometerNumber += 324;
	mCalorie += 47;
	mDistance += 2.6;

	std::stringstream ss1;
	ss1 << mPedometerNumber;
	mTextView1.SetText(ss1.str());
	mTextView1.SetStyleToCurrentText(GetTextStyle(30.0f, true, Vector4(0.0f, 0.7f, 0.2f, 1.0f)));

	std::stringstream ss2;
	ss2 << mDistance << " km " << mCalorie << " Cal";
	mTextView3.SetText(ss2.str());
	mTextView3.SetStyleToCurrentText(GetTextStyle(10.0f, false, Vector4(1.0f, 1.0f, 1.0f, 1.0f)));

	if (mPedometerNumber > 3240)
	{
		return false;
	}

	return true;
}

Toolkit::RadialSweepView RadialTest2Example::CreateSweepView( std::string imageName )
{
  // Create the image
  Image image = Image::New(imageName);
  mImageActor = ImageActor::New(image);
  mImageActor.SetParentOrigin(ParentOrigin::CENTER);
  mImageActor.SetAnchorPoint(AnchorPoint::CENTER);
  mImageActor.SetPositionInheritanceMode(USE_PARENT_POSITION);

  ShaderEffect shader = ShaderEffect::New("", FRAGMENT_SHADER);
  mImageActor.SetShaderEffect(shader);

  // Create the stencil
  Vector2 imageSize = Image::GetImageSize(imageName);
  float size = std::max(imageSize.width, imageSize.height);

#if defined (TEST_INCREASE_ANGLE)
  Toolkit::RadialSweepView radialSweepView = Toolkit::RadialSweepView::New();
#elif defined (TEST_INCREASE_STEP)
  Toolkit::RadialSweepView radialSweepView = Toolkit::RadialSweepView::New(Toolkit::RadialSweepView::INCREASE_BY_STEP_CIRCULARLY);
#else
  Toolkit::RadialSweepView radialSweepView = Toolkit::RadialSweepView::New(Toolkit::RadialSweepView::INCREASE_BY_STEP_CIRCULARLY);
#endif

  radialSweepView.SetSize(size, size);
  radialSweepView.SetPositionInheritanceMode( USE_PARENT_POSITION );
  radialSweepView.Add( mImageActor );
  mContents.Add(radialSweepView);

  return radialSweepView;
}

void RadialTest2Example::OnKeyEvent(const KeyEvent& event)
{
  if(event.state == KeyEvent::Down)
  {
    if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
    {
	  mApplication.Quit();
    }
  }
}

void RunTest(Application app)
{
  RadialTest2Example test(app);

  app.MainLoop();
}

// Entry point for Linux & Tizen applications
int main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv);

  RunTest(app);

  return 0;
}
