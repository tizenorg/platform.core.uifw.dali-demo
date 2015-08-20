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
#include <dali-toolkit/devel-api/controls/slider/slider.h>

// INTERNAL INCLUDES
#include "shared/view.h"

using namespace Dali;

namespace // unnamed namespace
{
const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-default.png" );
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char* APPLICATION_TITLE( "Sliders" );
const char* const CIRCLE_IMAGE( DALI_IMAGE_DIR "circle.9.png" );

/** Helper that returns an alpha function f(t) = ( scaleRatio + ( 1.f - scaleRatio ) * t ) * t when scaleRatio = 0.5
 *
 * @param progress the progress to calculate the alpha for
 * @return the applied alpha value
 */
float EaseInHalfSquare( float progress )
{
  return ( 0.5f + 0.5f * progress ) * progress;
}

/** Helper that returns an alpha function g(t) = 1 - f(t-1), where f(t) = ( scaleRatio + ( 1.f - scaleRatio ) * t ) * t and when scaleRatio = 0.5
 *
 * @param progress the progress to calculate the alpha for
 * @return the applied alpha value
 */
float EaseOutHalfSquare( float progress )
{
  return ( 1.5f - 0.5f * progress ) * progress;
}

/**
 * Helper that returns an alpha function that approximates f(t) = ( scaleRatio + ( 1.f - scaleRatio ) * t ) * t
 * This alpha function is used to match the scaling of the pill such that the top of the lower semi-circle is the origin of the scaling
 * See "X" below
 *
 *        /--------\
 *     /--          --\
 *   /-                -\
 *  /                    \
 * /                      \
 * +----------------------+
 * |                      |
 * |                      |
 * |                      |
 * |          The Origin of the scaling
 * +----------X-----------+
 * \                      /
 *  \                    /
 *   \-                -/
 *     \--          --/
 *        \--------/
 *
 * This alpha function is derived from:
 *
 * h = mHeight0 + ( mHeight1 - mHeight0 ) * t = The height of the pill at t
 * s = mScale0 + ( mScale1 - mScale0 ) * t = The scaling factor of the pill at t
 *
 * f(t) = s * ( mHeight0 - h ) ) / ( mScale1 * ( mHeight0 - mHeight1 ) ) = The normalised distance between the origin at t and the origin at 0
 *
 * @param scaleRatio the ratio of the scales given as scale0/scale1
 * @return the alpha function that approximates f(t) = ( scaleRatio + ( 1.f - scaleRatio ) * t ) * t
 */
AlphaFunction GetOriginOffsetAlphaFunctionIn( float scaleRatio )
{
  if( scaleRatio < 0.333f )
  {
    return AlphaFunction::EASE_IN_SQUARE;
  }
  else if( scaleRatio > 0.666f )
  {
    return AlphaFunction::LINEAR;
  }
  else
  {
    return &EaseInHalfSquare;
  }
}

/**
 * Helper that returns an alpha function that approximates g(t) = 1 - f(1-t), where f(t) is given by GetOriginOffsetAlphaFunctionIn
 * This alpha function is used to match the scaling of the pill such that the top of the lower semi-circle is the origin of the scaling

 * @param scaleRatio the ratio of the scales given as scale0/scale1
 * @return the alpha function that approximates g(t) = 1 - f(1-t), where f(t) is given by GetOriginOffsetAlphaFunctionIn
 */

AlphaFunction GetOriginOffsetAlphaFunctionOut( float scaleRatio )
{
  if( scaleRatio < 0.333f )
  {
    return AlphaFunction::EASE_OUT_SQUARE;
  }
  else if( scaleRatio > 0.666f )
  {
    return AlphaFunction::LINEAR;
  }
  else
  {
    return &EaseOutHalfSquare;
  }
}

class ExpandingSlider : public ConnectionTracker, public RefObject
{
public:
  ExpandingSlider( float scale0, float scale1, float height0, float height1 )
  : mScale0( scale0 ),
    mScale1( scale1 ),
    mHeight0( height0 ),
    mHeight1( height1 )
  {
    mSlider = Toolkit::Slider::New();
    mSlider.SlidingStartedSignal().Connect( this, &ExpandingSlider::SlideStarted );
    mSlider.SlidingFinishedSignal().Connect( this, &ExpandingSlider::SlideFinished );
  }

  Toolkit::Slider& GetSlider()
  {
    return mSlider;
  }

private:

  void SlideAnimate( Toolkit::Slider slider, float duration, bool animateOut ) const
  {
    Actor sliderHandle = slider.GetSliderHandle();
    if( sliderHandle )
    {
      float naturalHeight = sliderHandle.GetNaturalSize().height;
      float scaleRatio = mScale0 / mScale1;
      AlphaFunction alphaFunction = animateOut ? GetOriginOffsetAlphaFunctionOut( scaleRatio ) : GetOriginOffsetAlphaFunctionIn( scaleRatio );

      float height = animateOut ? mHeight0 : mHeight1;
      float scale = animateOut ? mScale0 : mScale1;

      Animation animHandle = Animation::New( duration );
      animHandle.AnimateTo( Property( sliderHandle, Actor::Property::SCALE ), Vector3( scale, scale, scale ) );
      animHandle.AnimateTo( Property( sliderHandle, Actor::Property::SIZE_HEIGHT ), height );
      animHandle.AnimateTo( Property( sliderHandle, Actor::Property::POSITION_Y ), 0.5f * scale * ( naturalHeight - height ), alphaFunction );

      Actor sliderHandleLabel = slider.GetSliderHandleLabel();
      if( sliderHandleLabel )
      {
        float handleLabelAlpha = animateOut ? 0.f : 1.f;
        animHandle.AnimateTo( Property( sliderHandleLabel, Actor::Property::POSITION_Y ), 0.5f * scale * ( naturalHeight - height ), alphaFunction );
        sliderHandleLabel.SetProperty( Actor::Property::COLOR_ALPHA, Property::Value( handleLabelAlpha ) );
      }

      animHandle.Play();
    }
  }


  bool SlideFinished( Toolkit::Slider slider, float position )
  {
    SlideAnimate( slider, 0.25f, true );
    return true;
  }

  bool SlideStarted( Toolkit::Slider slider, float position )
  {
    SlideAnimate( slider, 0.25f, false );
    return true;
  }

private:
  Toolkit::Slider mSlider;
  float mScale0;
  float mScale1;
  float mHeight0;
  float mHeight1;
};

typedef IntrusivePtr<ExpandingSlider> ExpandingSliderPtr;

}; // unnamed namespace

class SliderController: public ConnectionTracker
{
public:
  SliderController( Application& application )
    : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &SliderController::Create );
  }

  ~SliderController()
  {
    // Nothing to do here
  }

  void Create( Application& application )
  {
    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            APPLICATION_TITLE );

    Toolkit::TableView table = Toolkit::TableView::New( 4, 1 );
    table.SetParentOrigin( ParentOrigin::CENTER );
    table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mContentLayer.Add( table );

    int column = 1;
    {
      mSliders.push_back( CreateSlider( 0.3f, 1.f, 100.f, 200.f ) );
      Toolkit::Slider& slider = mSliders.back()->GetSlider();
      table.AddChild( slider, Toolkit::TableView::CellPosition( column++, 0 ) );
    }

    {
      mSliders.push_back( CreateSlider( 0.3f, 1.f, 100.f, 200.f ) );
      Toolkit::Slider& slider = mSliders.back()->GetSlider();

      Property::Array marks;
      marks.PushBack( Property::Value(   0.f ) );
      marks.PushBack( Property::Value(  20.f ) );
      marks.PushBack( Property::Value(  60.f ) );
      marks.PushBack( Property::Value( 100.f ) );

      slider.SetProperty( Toolkit::Slider::Property::MARKS, marks );
      slider.SetProperty( Toolkit::Slider::Property::SNAP_TO_MARKS, Property::Value( true ) );

      table.AddChild( slider, Toolkit::TableView::CellPosition( column++, 0 ) );
    }

    {
      mSliders.push_back( CreateSlider( 0.3f, 1.f, 100.f, 100.f ) );
      Toolkit::Slider& slider = mSliders.back()->GetSlider();
      slider.SetProperty( Toolkit::Slider::Property::SHOW_VALUE, Property::Value( false ) );
      table.AddChild(slider,Toolkit::TableView::CellPosition( column++, 0 ) );
    }

    {
      mSliders.push_back( CreateSlider( 0.2f, 1.6f, 500.f, 120.f ) );
      Toolkit::Slider& slider = mSliders.back()->GetSlider();
      table.AddChild( slider, Toolkit::TableView::CellPosition( column++, 0 ) );
    }

    {
      mSliders.push_back( CreateSlider( 1.5f, 0.3f, 150.f, 100.f ) );
      Toolkit::Slider& slider = mSliders.back()->GetSlider();
      table.AddChild(slider,Toolkit::TableView::CellPosition( column++, 0 ) );
    }
  }

ExpandingSliderPtr CreateSlider( float scale0, float scale1, float height0, float height1 ) const
{
  ExpandingSliderPtr expandingSlider = new ExpandingSlider( scale0, scale1, height0, height1 );

  Toolkit::Slider& slider = expandingSlider->GetSlider();
  slider.SetProperty(Toolkit::Slider::Property::HANDLE_REGION,Property::Value( Vector2( 100.f, height0 ) ) );
  slider.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  slider.SetParentOrigin( ParentOrigin::CENTER );

  slider.SetProperty( Toolkit::Slider::Property::HANDLE_IMAGE_NAME, Property::Value( CIRCLE_IMAGE ) );

  slider.SetProperty( Toolkit::Slider::Property::LOWER_BOUND,Property::Value( 0.f ) );
  slider.SetProperty( Toolkit::Slider::Property::UPPER_BOUND,Property::Value( 100.f ) );

  Actor sliderHandle = slider.GetSliderHandle();
  if( sliderHandle )
  {
    float naturalHeight = sliderHandle.GetNaturalSize().height;

    sliderHandle.SetProperty( Actor::Property::SCALE , Property::Value( Vector3( scale0, scale0, scale0 ) ) );
    sliderHandle.SetProperty( Actor::Property::POSITION_Y, Property::Value( 0.5f * scale0 * ( naturalHeight - height0 ) ) );
  }

  Actor sliderHandleLabel = slider.GetSliderHandleLabel();
  if( sliderHandleLabel )
  {
    sliderHandleLabel.SetProperty( Actor::Property::COLOR_ALPHA, Property::Value( 0.f ) );
  }

  return expandingSlider;
}


 private:

  Application&      mApplication;
  Toolkit::Control  mView;                              ///< The View instance.
  Toolkit::ToolBar  mToolBar;                           ///< The View's Toolbar.
  Layer             mContentLayer;                      ///< Content layer
  std::vector<ExpandingSliderPtr> mSliders;
};

void RunTest( Application& application )
{
  SliderController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DALI_DEMO_THEME_PATH );

  RunTest( application );

  return 0;
}
