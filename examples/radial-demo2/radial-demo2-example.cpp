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
#include <iomanip>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

//#define ENABLE_INDEX

#define DIAMETER          360.0f

#ifdef ENABLE_INDEX
#define INDEX_NUM         10

#define FOCUSED_INDEX_COLOR     Vector4(0.2f, 0.4f, 1.0f, 1.0f)
#endif

const char* BG_IMAGE_FILENAME = DALI_IMAGE_DIR "layer11.png";
const char* RADIAL_IMAGE_FILENAME = DALI_IMAGE_DIR "layer9.png";
const char* ICON_IMAGE_FILENAME = DALI_IMAGE_DIR "volume.png";
const char* POINTER_IMAGE_FILENAME = DALI_IMAGE_DIR "pointer.png";
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" ); // Background for toolbar
const char* APPLICATION_TITLE( "Radial Demo 2" );

const char* BG_FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
    void main()
    {
        gl_FragColor = texture2D(sTexture, vTexCoord) * vec4(0.0, 0.0, 0.0, 1.0);
    }
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
	void main()
	{
#ifdef ENABLE_INDEX
        gl_FragColor = texture2D(sTexture, vTexCoord) * vec4(0.4, 0.80, 1.0, 0.6);
#else
        gl_FragColor = texture2D(sTexture, vTexCoord) * vec4(0.3, 0.68, 0.86, 1.0);
#endif
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

class Circle
{
public:
  Circle();
  virtual ~Circle();

  void SetCenter( Vector3 center );
  Vector3 GetCenter();

  void SetRadius( float radius );
  float GetRadius();

  Vector3 GetPoint( Degree angle );

private:
  Vector3 mCenter;
  float mRadius;
};

Circle::Circle()
: mCenter( Vector3() ),
  mRadius( 0.0f )
{
}

Circle::~Circle()
{
}

void Circle::SetCenter( Vector3 center )
{
  mCenter = center;
}

Vector3 Circle::GetCenter()
{
  return mCenter;
}

void Circle::SetRadius( float radius )
{
  mRadius = radius;
}

float Circle::GetRadius()
{
  return mRadius;
}

Vector3 Circle::GetPoint( Degree angle )
{
  Radian angleInRadians(angle);

  return Vector3( mRadius * sinf(angleInRadians), -mRadius * cosf(angleInRadians), mCenter.z );
}

// This example shows how to create a mesh actor for use as a stencil buffer
class RadialDemo2Example : public ConnectionTracker
{
public:
  /**
   * Constructor
   * @param[in] app The application handle
   */
  RadialDemo2Example(Application app);

  /**
   * Destructor
   */
  ~RadialDemo2Example();

private:

  /**
   * Initialization signal handler - all actor initialization should happen here
   * @param[in] app The application handle
   */
  void OnInit(Application& app);

  /**
   * Main key event handler
   *
   * @param[in] event The key event to respond to
   */
  void OnKeyEvent(const KeyEvent& event);

  void OnAnimationFinished( Animation& source );

private: // Member variables

  Application     mApplication; ///< The application handle
  Toolkit::View   mView;        ///< The toolbar view
  Layer           mContents;    ///< The toolbar contents pane

  ImageActor      mBgImageActor;
  ImageActor      mIconActor;
  MeshActor       mMeshActor;

  Toolkit::RadialSweepView mRadialSweepView;
  Toolkit::TextView   mTitleTextView;
  Toolkit::TextView   mNumberTextView;

#ifdef ENABLE_INDEX
  int             mCurrentIndex;
  Circle          mCircle;

  Toolkit::TextView   mIndexTextView[INDEX_NUM];
#endif
};

RadialDemo2Example::RadialDemo2Example(Application app)
: mApplication( app )
{
  // Connect to the Application's Init signal
  app.InitSignal().Connect(this, &RadialDemo2Example::OnInit);
}

RadialDemo2Example::~RadialDemo2Example()
{
  // Nothing to do here; actor handles will clean up themselves.
}

void RadialDemo2Example::OnInit(Application& app)
{
  Stage stage = Dali::Stage::GetCurrent();

  stage.SetBackgroundColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

  // The Init signal is received once (only) during the Application lifetime
  stage.KeyEventSignal().Connect(this, &RadialDemo2Example::OnKeyEvent);

  // Create toolbar & view
  Toolkit::ToolBar toolBar;
  mContents = DemoHelper::CreateView( mApplication,
                                      mView,
                                      toolBar,
                                      "",
                                      TOOLBAR_IMAGE,
                                      APPLICATION_TITLE );

  Image bgImage = Image::New(BG_IMAGE_FILENAME);
  mBgImageActor = ImageActor::New(bgImage);
  mBgImageActor.SetSize(DIAMETER - 1.0f, DIAMETER - 1.0f);
  mBgImageActor.SetParentOrigin(ParentOrigin::CENTER);
  mBgImageActor.SetAnchorPoint(AnchorPoint::CENTER);

  ShaderEffect bgShader = ShaderEffect::New("", BG_FRAGMENT_SHADER);
  mBgImageActor.SetShaderEffect(bgShader);

  mContents.Add(mBgImageActor);

  Image iconImage = Image::New(ICON_IMAGE_FILENAME);
  mIconActor = ImageActor::New(iconImage);
//  mIconActor.SetSize(DIAMETER / 3, DIAMETER / 3);
  mIconActor.SetParentOrigin(ParentOrigin::CENTER);
  mIconActor.SetAnchorPoint(AnchorPoint::CENTER);
  mContents.Add(mIconActor);

  // Text
  mTitleTextView = TextView::New("Volume");
  mTitleTextView.SetParentOrigin(ParentOrigin::CENTER);
  mTitleTextView.SetAnchorPoint(AnchorPoint::CENTER);
  mTitleTextView.SetSize(100.0f, 50.0f);
  mTitleTextView.SetPosition(0.0f, -110.0f);
  mTitleTextView.SetStyleToCurrentText(GetTextStyle(10.0f, false, Vector4(1.0f, 1.0f, 1.0f, 1.0f)));
  mTitleTextView.SetSnapshotModeEnabled(true);
  mContents.Add(mTitleTextView);

  mNumberTextView = TextView::New("00");
  mNumberTextView.SetParentOrigin(ParentOrigin::CENTER);
  mNumberTextView.SetAnchorPoint(AnchorPoint::CENTER);
  mNumberTextView.SetSize(100.0f, 100.0f);
  mNumberTextView.SetPosition(0.0f, 110.0f);
  mNumberTextView.SetStyleToCurrentText(GetTextStyle(16.0f, false, Vector4(1.0f, 1.0f, 1.0f, 1.0f)));
  mNumberTextView.SetSnapshotModeEnabled(false);
  mContents.Add(mNumberTextView);

  // Mesh
  MeshData::VertexContainer vertices(4);
  vertices[0] = MeshData::Vertex(Vector3(-0.5f, -0.5f, 0.0f), Vector2::ZERO, Vector3(0.6f, 0.6f, 0.6f));
  vertices[1] = MeshData::Vertex(Vector3( 0.5f, -0.5f, 0.0f), Vector2::ZERO, Vector3(0.6f, 0.6f, 0.6f));
  vertices[2] = MeshData::Vertex(Vector3(-0.5f,  0.5f, 0.0f), Vector2::ZERO, Vector3(0.6f, 0.6f, 0.6f));
  vertices[3] = MeshData::Vertex(Vector3( 0.5f,  0.5f, 0.0f), Vector2::ZERO, Vector3(0.6f, 0.6f, 0.6f));

  MeshData::FaceIndices faces;
  faces.reserve(6);
  faces.push_back(0); faces.push_back(3); faces.push_back(1);
  faces.push_back(0); faces.push_back(2); faces.push_back(3);

  MeshData meshData;
  meshData.SetMaterial(Material::New("Material"));
  meshData.SetVertices(vertices);
  meshData.SetFaceIndices(faces);
  meshData.SetHasColor(true);

  mMeshActor = MeshActor::New(Mesh::New(meshData));
  mMeshActor.SetParentOrigin(ParentOrigin::CENTER);
  mMeshActor.SetAnchorPoint(AnchorPoint::CENTER);
  mMeshActor.SetAffectedByLighting(false);
  mMeshActor.SetPosition(0.0f, -80.0f);
  mMeshActor.SetScale(220.0f, 1.0f, 1.0f);
  mContents.Add(mMeshActor);

#ifdef ENABLE_INDEX
  mCurrentIndex = 0;

  mCircle = Circle();
  mCircle.SetCenter(Vector3(0.0f, 0.0f, 0.0f));
  mCircle.SetRadius(DIAMETER / 2.0f - 12.0f );

  for( int i = 0; i < INDEX_NUM; i++ )
  {
    std::stringstream ss;
    ss << i;

    Degree angle(i * 360.0f / INDEX_NUM);

    mIndexTextView[i] = TextView::New(ss.str());
    mIndexTextView[i].SetParentOrigin(ParentOrigin::CENTER);
    mIndexTextView[i].SetAnchorPoint(AnchorPoint::CENTER);
    mIndexTextView[i].SetRotation(angle, Vector3::ZAXIS);
    mIndexTextView[i].SetPosition(mCircle.GetPoint(angle));
    mIndexTextView[i].SetStyleToCurrentText(GetTextStyle(6.0f, false, Vector4(1.0f, 1.0f, 1.0f, 1.0f)));
    mContents.Add(mIndexTextView[i]);
  }

  mIndexTextView[mCurrentIndex].SetStyleToCurrentText(GetTextStyle(6.0f, true, FOCUSED_INDEX_COLOR));
#endif

  // radial sweep view
  Image radialImage = Image::New(RADIAL_IMAGE_FILENAME);
  ImageActor radialActor = ImageActor::New(radialImage);
  radialActor.SetSize(DIAMETER, DIAMETER);
  radialActor.SetParentOrigin(ParentOrigin::CENTER);
  radialActor.SetAnchorPoint(AnchorPoint::CENTER);
  radialActor.SetPositionInheritanceMode(USE_PARENT_POSITION);

  ShaderEffect shader = ShaderEffect::New("", FRAGMENT_SHADER);
  radialActor.SetShaderEffect(shader);

  mRadialSweepView = Toolkit::RadialSweepView::New(Toolkit::RadialSweepView::INCREASE_BY_STEP);
  mRadialSweepView.SetSize(DIAMETER, DIAMETER);
  mRadialSweepView.SetPositionInheritanceMode(USE_PARENT_POSITION);
  mRadialSweepView.Add(radialActor);
  mRadialSweepView.SetStepCount(10);
//  mRadialSweepView.SetCurrentStep(4.0f);

  Image pointer = Image::New(POINTER_IMAGE_FILENAME);
#ifndef ENABLE_INDEX
  mRadialSweepView.SetPointerImage(pointer);
#endif

  mContents.Add(mRadialSweepView);
}

void RadialDemo2Example::OnKeyEvent(const KeyEvent& event)
{
  if(event.state == KeyEvent::Down)
  {
    if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
    {
	  mApplication.Quit();
    }
    else
    {
      std::string keyName = event.keyPressedName;
      float currentStep;
      bool bAnimate = false;

      if( keyName == "Left" )
      {
        currentStep = mRadialSweepView.GetCurrentStep() - 1.0f;
        if( currentStep < 0.0f )
          currentStep = 0.0f;

        bAnimate = true;
      }
      else if( keyName == "Right" )
      {
        currentStep = mRadialSweepView.GetCurrentStep() + 1.0f;
        if( currentStep > (float)mRadialSweepView.GetStepCount())
          currentStep = (float)mRadialSweepView.GetStepCount();

        bAnimate = true;
      }

      if( bAnimate )
      {
//        mRadialSweepView.SetCurrentStep(currentStep);

        Animation animation = Animation::New(0.15f);
        animation.AnimateTo(Property(mRadialSweepView, RadialSweepView::CURRENT_STEP_PROPERTY_NAME), currentStep, AlphaFunctions::EaseIn);
        animation.Play();

        animation.FinishedSignal().Connect(this, &RadialDemo2Example::OnAnimationFinished);
      }
    }
  }
}

void RadialDemo2Example::OnAnimationFinished( Animation& source )
{
  int volume = (int)mRadialSweepView.GetCurrentStep();

  std::stringstream ss;
  ss << std::setw(2) << std::setfill('0') << volume;

  mNumberTextView.SetText(ss.str());
  mNumberTextView.SetStyleToCurrentText(GetTextStyle(16.0f, false, Vector4(1.0f, 1.0f, 1.0f, 1.0f)));

#ifdef ENABLE_INDEX
  if( mCurrentIndex < (float)mRadialSweepView.GetStepCount() )
  {
    mIndexTextView[mCurrentIndex].SetStyleToCurrentText(GetTextStyle(6.0f, false, Vector4(1.0f, 1.0f, 1.0f, 1.0f)));
  }

  if( volume < (float)mRadialSweepView.GetStepCount() )
  {
    mIndexTextView[volume].SetStyleToCurrentText(GetTextStyle(6.0f, true, FOCUSED_INDEX_COLOR));
  }

  mCurrentIndex = volume;
#endif
}

void RunTest(Application app)
{
  RadialDemo2Example test(app);

  app.MainLoop();
}

// Entry point for Linux & Tizen applications
int main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv);

  RunTest(app);

  return 0;
}
