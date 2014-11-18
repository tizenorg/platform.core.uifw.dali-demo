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

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

#define DIAMETER          360.0f
#define BG_IMAGE_COUNT    5

const char* BG_IMAGE1_FILENAME = DALI_IMAGE_DIR "circle3.png";
const char* BG_IMAGE2_FILENAME = DALI_IMAGE_DIR "circle4.png";
const char* BG_IMAGE3_FILENAME = DALI_IMAGE_DIR "circle5.png";
const char* BG_IMAGE4_FILENAME = DALI_IMAGE_DIR "circle6.png";
const char* BG_IMAGE5_FILENAME = DALI_IMAGE_DIR "circle7.png";
const char* RADIAL_IMAGE_FILENAME = DALI_IMAGE_DIR "layer8.png";
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" ); // Background for toolbar
const char* APPLICATION_TITLE( "Radial Test 2" );

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
    void main()
    {
        mediump vec4 color = texture2D(sTexture, vTexCoord);
        gl_FragColor = color * vec4(0.7, 0.7, 0.7, 1.0);
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
class RadialDemo1Example : public ConnectionTracker
{
public:
  /**
   * Constructor
   * @param[in] app The application handle
   */
  RadialDemo1Example(Application app);

  /**
   * Destructor
   */
  ~RadialDemo1Example();

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

  Toolkit::RadialSweepView mRadialSweepView;

  int             mBgImageIndex;
};

RadialDemo1Example::RadialDemo1Example(Application app)
: mApplication( app ),
  mBgImageIndex( 0 )
{
  // Connect to the Application's Init signal
  app.InitSignal().Connect(this, &RadialDemo1Example::OnInit);
}

RadialDemo1Example::~RadialDemo1Example()
{
  // Nothing to do here; actor handles will clean up themselves.
}

void RadialDemo1Example::OnInit(Application& app)
{
  Stage stage = Dali::Stage::GetCurrent();

  stage.SetBackgroundColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

  // The Init signal is received once (only) during the Application lifetime
  stage.KeyEventSignal().Connect(this, &RadialDemo1Example::OnKeyEvent);

  // Create toolbar & view
  Toolkit::ToolBar toolBar;
  mContents = DemoHelper::CreateView( mApplication,
                                      mView,
                                      toolBar,
                                      "",
                                      TOOLBAR_IMAGE,
                                      APPLICATION_TITLE );

  Image bgImage = Image::New(BG_IMAGE1_FILENAME);
  mBgImageActor = ImageActor::New(bgImage);
  mBgImageActor.SetSize(DIAMETER, DIAMETER);
  mBgImageActor.SetParentOrigin(ParentOrigin::CENTER);
  mBgImageActor.SetAnchorPoint(AnchorPoint::CENTER);
  mContents.Add(mBgImageActor);

  Image radialImage = Image::New(RADIAL_IMAGE_FILENAME);
  ImageActor radialActor = ImageActor::New(radialImage);
  radialActor.SetSize(DIAMETER, DIAMETER);
  radialActor.SetParentOrigin(ParentOrigin::CENTER);
  radialActor.SetAnchorPoint(AnchorPoint::CENTER);
  radialActor.SetPositionInheritanceMode(USE_PARENT_POSITION);

  ShaderEffect shader = ShaderEffect::New("", FRAGMENT_SHADER);
  radialActor.SetShaderEffect(shader);

  mRadialSweepView = Toolkit::RadialSweepView::New(Toolkit::RadialSweepView::INCREASE_BY_STEP_CIRCULARLY);
  mRadialSweepView.SetSize(DIAMETER, DIAMETER);
  mRadialSweepView.SetPositionInheritanceMode(USE_PARENT_POSITION);
  mRadialSweepView.Add(radialActor);
  mRadialSweepView.SetStepCount(16);
  mRadialSweepView.SetCurrentStep(14.0f);
  mContents.Add(mRadialSweepView);
}

void RadialDemo1Example::OnKeyEvent(const KeyEvent& event)
{
  if( event.state == KeyEvent::Down )
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
//        if( currentStep < 1.0f )
//          currentStep = (float)mRadialSweepView.GetStepCount();

        bAnimate = true;
      }
      else if( keyName == "Right" )
      {
        currentStep = mRadialSweepView.GetCurrentStep() + 1.0f;
//        if( currentStep > (float)mRadialSweepView.GetStepCount() )
//          currentStep = 1.0f;

        bAnimate = true;
      }

      if( bAnimate )
      {
//        mRadialSweepView.SetCurrentStep(currentStep);

        Animation animation = Animation::New(0.15f);
        animation.AnimateTo(Property(mRadialSweepView, RadialSweepView::CURRENT_STEP_PROPERTY_NAME), currentStep, AlphaFunctions::EaseIn);
        animation.Play();

        animation.FinishedSignal().Connect(this, &RadialDemo1Example::OnAnimationFinished);
      }
    }
  }
}

void RadialDemo1Example::OnAnimationFinished( Animation& source )
{
  mBgImageIndex++;
  if( mBgImageIndex >= BG_IMAGE_COUNT)
    mBgImageIndex = 0;

  std::string fileName;

  switch(mBgImageIndex)
  {
  case 0:
    fileName = BG_IMAGE1_FILENAME;
    break;
  case 1:
    fileName = BG_IMAGE2_FILENAME;
    break;
  case 2:
    fileName = BG_IMAGE3_FILENAME;
    break;
  case 3:
    fileName = BG_IMAGE4_FILENAME;
    break;
  case 4:
    fileName = BG_IMAGE5_FILENAME;
    break;
  }

  Image bgImage = Image::New(fileName);
  mBgImageActor.SetImage(bgImage);
}

void RunTest(Application app)
{
  RadialDemo1Example test(app);

  app.MainLoop();
}

// Entry point for Linux & Tizen applications
int main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv);

  RunTest(app);

  return 0;
}
