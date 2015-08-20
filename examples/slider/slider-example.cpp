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
#include <dali-toolkit/devel-api/controls/slider/expanding-handle-slider.h>

// INTERNAL INCLUDES
#include "shared/view.h"

using namespace Dali;

namespace // unnamed namespace
{
const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-default.png" );
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char* APPLICATION_TITLE( "Sliders" );
const char* const CIRCLE_IMAGE( DALI_IMAGE_DIR "circle.9.png" );

}; // unnamed namespace

class SliderController: public ConnectionTracker
{
public:
  SliderController( Application& application )
    : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &SliderController::OnInit );
  }

  ~SliderController()
  {
    // Nothing to do here
  }

  void OnInit( Application& application )
  {
    Stage::GetCurrent().KeyEventSignal().Connect(this, &SliderController::OnKeyEvent);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            APPLICATION_TITLE );

    Toolkit::TableView table = Toolkit::TableView::New( 6, 1 );
    table.SetParentOrigin( ParentOrigin::CENTER );
    table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mContentLayer.Add( table );

    int column = 1;
    {
      mSliders.push_back( CreateSlider( 0.3f, 1.f, 100, 200, 0.f, 1.f ) );
      Toolkit::ExpandingHandleSlider& slider = mSliders.back();
      table.AddChild( slider, Toolkit::TableView::CellPosition( column++, 0 ) );
    }

    {
      mSliders.push_back( CreateSlider( 0.3f, 1.f, 100, 200, 0.f, 1.f ) );
      Toolkit::ExpandingHandleSlider& slider = mSliders.back();

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
      mSliders.push_back( CreateSlider( 1.f, 0.3f, 200, 100, 1.f, 0.f ) );
      Toolkit::ExpandingHandleSlider& slider = mSliders.back();
      table.AddChild(slider,Toolkit::TableView::CellPosition( column++, 0 ) );
    }

    {
      mSliders.push_back( CreateSlider( 0.3f, 1.f, 100, 100, 0.f, 0.f ) );
      Toolkit::ExpandingHandleSlider& slider = mSliders.back();
      slider.SetProperty( Toolkit::Slider::Property::SHOW_VALUE, Property::Value( false ) );
      table.AddChild(slider,Toolkit::TableView::CellPosition( column++, 0 ) );
    }

    {
      mSliders.push_back( CreateSlider( 0.2f, 1.6f, 500, 120, 0.f, 1.f ) );
      Toolkit::ExpandingHandleSlider& slider = mSliders.back();
      table.AddChild( slider, Toolkit::TableView::CellPosition( column++, 0 ) );
    }

  }

Toolkit::ExpandingHandleSlider CreateSlider( float scale, float scaleSliding, int height, int heightSliding, float labelOpacity, float labelOpacitySliding ) const
{
  Toolkit::ExpandingHandleSlider expandingSlider;
  expandingSlider = Toolkit::ExpandingHandleSlider::New();

  expandingSlider.SetProperty( Toolkit::ExpandingHandleSlider::Property::HANDLE_SCALE, scale );
  expandingSlider.SetProperty( Toolkit::ExpandingHandleSlider::Property::HANDLE_SCALE_SLIDING, scaleSliding );
  expandingSlider.SetProperty( Toolkit::ExpandingHandleSlider::Property::HANDLE_HEIGHT, height );
  expandingSlider.SetProperty( Toolkit::ExpandingHandleSlider::Property::HANDLE_HEIGHT_SLIDING, heightSliding );
  expandingSlider.SetProperty( Toolkit::ExpandingHandleSlider::Property::HANDLE_LABEL_OPACITY, labelOpacity );
  expandingSlider.SetProperty( Toolkit::ExpandingHandleSlider::Property::HANDLE_LABEL_OPACITY_SLIDING, labelOpacitySliding );
  expandingSlider.SetProperty( Toolkit::ExpandingHandleSlider::Property::DURATION, 0.25f );

  expandingSlider.SetProperty( Toolkit::Slider::Property::HANDLE_REGION, Vector2( 100.f, height ) );
  expandingSlider.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  expandingSlider.SetParentOrigin( ParentOrigin::CENTER );

  expandingSlider.SetProperty( Toolkit::Slider::Property::HANDLE_IMAGE_NAME, CIRCLE_IMAGE );

  expandingSlider.SetProperty( Toolkit::Slider::Property::LOWER_BOUND, 0.f );
  expandingSlider.SetProperty( Toolkit::Slider::Property::UPPER_BOUND,100.f );

  return expandingSlider;
}

void OnKeyEvent( const KeyEvent& event )
{
  if( event.state == KeyEvent::Down )
  {
    if( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
    {
      // Exit application when click back or escape.
      mApplication.Quit();
    }
  }
}

private:
  Application&      mApplication;
  Toolkit::Control  mView;                              ///< The View instance.
  Toolkit::ToolBar  mToolBar;                           ///< The View's Toolbar.
  Layer             mContentLayer;                      ///< Content layer
  std::vector<Toolkit::ExpandingHandleSlider> mSliders;
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
