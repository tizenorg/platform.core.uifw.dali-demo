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

#include "shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/slider/slider.h>

using namespace Dali;

namespace
{

const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-gradient.jpg" );
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char* APPLICATION_TITLE( "Blending" );

const char* IMAGE_PATH[] = {
    DALI_IMAGE_DIR "heartsframe.9.png",
};

const char* RESOURCE_IMAGE_PATH[] = {
    DALI_IMAGE_DIR "heartsframe.9.png",
};

const unsigned int NUM_IMAGES = sizeof(IMAGE_PATH) / sizeof(char*);
const unsigned int NUM_RESOURCE_IMAGES = sizeof(RESOURCE_IMAGE_PATH) / sizeof(char*);

const unsigned int WIDTH = 1;
const unsigned int HEIGHT = 3;

}  // namespace

class BlendingController: public ConnectionTracker
{
 public:

  BlendingController( Application& application )
    : mApplication( application ),
      mCurrentPositionToggle( 0, 0 ),
      mCurrentPositionImage( 0, 0 ),
      mToggleOff( true ),
      mUseResource( false ),
      mImageIdx( 1 )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &BlendingController::Create );
  }

  ~BlendingController()
  {
    // Nothing to do here
  }

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            APPLICATION_TITLE );


    mTable = Toolkit::TableView::New( HEIGHT, WIDTH );
    mTable.SetAnchorPoint( AnchorPoint::CENTER );
    mTable.SetParentOrigin( ParentOrigin::CENTER );

    mTable.SetSize( Vector2( 300, 600 ) );

    mContentLayer.Add( mTable );

    bool hole = false;
    for( unsigned int y = 0; y < HEIGHT; ++y )
    {
      for( unsigned int x = 0; x < WIDTH; ++x )
      {
        const char* path = y != 2 ? IMAGE_PATH[ 0 ] : "invalid-image.9.png";

        mImageViews[x][y] = Toolkit::ImageView::New( path );
        mImageViews[x][y].SetParentOrigin( ParentOrigin::CENTER );
        mImageViews[x][y].SetAnchorPoint( AnchorPoint::CENTER );
        mImageViews[x][y].SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        mImageViews[x][y].SetSizeScalePolicy( SizeScalePolicy::FIT_WITH_ASPECT_RATIO );

        Property::Map property;
        property.Insert( "border-only", hole );
        property.Insert( "image-url", path  );

        mImageViews[x][y].SetProperty( Toolkit::ImageView::Property::IMAGE, property );

        mTable.AddChild( mImageViews[x][y], Toolkit::TableView::CellPosition( y, x ) );

        hole = !hole;
      }
    }

    Toolkit::TableView buttonsTable = Toolkit::TableView::New( 3, 1 );
    buttonsTable.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    buttonsTable.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    buttonsTable.SetFitHeight( 0 );
    buttonsTable.SetFitHeight( 1 );
    buttonsTable.SetFitHeight( 2 );
    buttonsTable.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );

    Toolkit::PushButton button = Toolkit::PushButton::New();
    button.SetLabelText( "Toggle on/off stage" );
    button.SetParentOrigin( ParentOrigin::CENTER );
    button.SetAnchorPoint( AnchorPoint::CENTER );
    button.ClickedSignal().Connect( this, &BlendingController::ToggleImageOnStage );
    button.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    buttonsTable.AddChild( button, Toolkit::TableView::CellPosition( 0, 0 ) );

    Toolkit::PushButton button2 = Toolkit::PushButton::New();
    button2.SetLabelText( "Change Image" );
    button2.SetParentOrigin( ParentOrigin::CENTER );
    button2.SetAnchorPoint( AnchorPoint::CENTER );
    button2.ClickedSignal().Connect( this, &BlendingController::ChangeImageClicked );
    button2.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    //buttonsTable.AddChild( button2, Toolkit::TableView::CellPosition( 1, 0 ) );

    Toolkit::CheckBoxButton button3 = Toolkit::CheckBoxButton::New();
    button3.SetLabelText( "Use Resource Images" );
    button3.SetParentOrigin( ParentOrigin::CENTER );
    button3.SetAnchorPoint( AnchorPoint::CENTER );
    button3.ClickedSignal().Connect( this, &BlendingController::UseResourceImagesClicked );
    button3.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    //buttonsTable.AddChild( button3, Toolkit::TableView::CellPosition( 2, 0 ) );

    mContentLayer.Add(buttonsTable);
  }

private:
  bool ToggleImageOnStage( Toolkit::Button button )
  {
    Toolkit::ImageView imageView =  mImageViews[ mCurrentPositionToggle.columnIndex ][ mCurrentPositionToggle.rowIndex ];

    if( mToggleOff )
    {
      imageView.Unparent();
    }
    else
    {
      mTable.AddChild( imageView, mCurrentPositionToggle );
    }

    ++mCurrentPositionToggle.columnIndex;
    if( mCurrentPositionToggle.columnIndex == WIDTH )
    {
      mCurrentPositionToggle.columnIndex = 0;
      ++mCurrentPositionToggle.rowIndex;
    }
    if( mCurrentPositionToggle.rowIndex == HEIGHT )
    {
      mCurrentPositionToggle.rowIndex = 0;
      mToggleOff = !mToggleOff;
    }

    return true;
  }

  bool ChangeImageClicked( Toolkit::Button button )
  {
    Toolkit::ImageView imageView =  mImageViews[ mCurrentPositionImage.columnIndex ][ mCurrentPositionImage.rowIndex ];

    if( mUseResource )
    {
      ResourceImage image = ResourceImage::New( RESOURCE_IMAGE_PATH[ mImageIdx ] );
      imageView.SetImage( image );
    }
    else
    {
      imageView.SetImage( IMAGE_PATH[ mImageIdx ] );
    }

    ++mCurrentPositionImage.columnIndex;
    if( mCurrentPositionImage.columnIndex == WIDTH )
    {
      mCurrentPositionImage.columnIndex = 0;
      ++mCurrentPositionImage.rowIndex;
    }
    if( mCurrentPositionImage.rowIndex == HEIGHT )
    {
      mCurrentPositionImage.rowIndex = 0;
      ++mImageIdx;

      int numImages = mUseResource ? NUM_RESOURCE_IMAGES : NUM_IMAGES;
      if( mImageIdx == numImages )
      {
        mImageIdx = 0;
      }
    }

    return true;
  }

  bool UseResourceImagesClicked( Toolkit::Button button )
  {
    mUseResource = !mUseResource;

    int numImages = mUseResource ? NUM_RESOURCE_IMAGES : NUM_IMAGES;
    if( mImageIdx >= numImages )
    {
      mImageIdx = 0;
    }

    return true;
  }

private:
  Application&  mApplication;

  Toolkit::Control           mView;                              ///< The View instance.
  Toolkit::ToolBar           mToolBar;                           ///< The View's Toolbar.
  Layer                      mContentLayer;                      ///< Content layer (scrolling cluster content)
  Toolkit::PushButton        mLoseContextButton;
  Toolkit::TableView         mTable;
  Toolkit::ImageView        mImageViews[ WIDTH ][ HEIGHT ];

  Toolkit::TableView::CellPosition mCurrentPositionToggle;
  Toolkit::TableView::CellPosition mCurrentPositionImage;

  bool mToggleOff;
  bool mUseResource;
  int mImageIdx;

};

void RunTest( Application& application )
{
  BlendingController test( application );

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
