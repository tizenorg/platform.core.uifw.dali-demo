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

#include <sstream>
#include "shared/view.h"

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/flex-container/flex-container.h>
//todor
#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;

//#define BASIC_DEMO
//#define ALL_OPTS
#define FORCE_RELAYOUT_PARENT
#define FORCE_RELAYOUT_CHILDREN
#define DEBUG_COLORS
#define USE_NEW_POLICY

namespace
{
const int NUM_FLEX_ITEMS = 18;
const int NUM_SETTING_ITEMS = 3;
const float ITEM_PERCENT_WIDTH = 16.0f;
const char* BORDER_IMAGE( DEMO_IMAGE_DIR "border-4px.9.png" );
const int BORDER_WIDTH = ( 11.0f + 4.0f ); // Shadow size = 11, border size = 4.
const char* RESIZE_HANDLE_IMAGE( DEMO_IMAGE_DIR "resize-handle.png" );
const char* BACKGROUND_IMAGE( DEMO_IMAGE_DIR "background-default.png" );
const char* TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );
const char* IMAGES[] = {
                            DEMO_IMAGE_DIR "settings-icon.png",
                            DEMO_IMAGE_DIR "wifi-icon.png",
                            DEMO_IMAGE_DIR "bluetooth-icon.png",
                            DEMO_IMAGE_DIR "datausage-icon.png"
};
const Vector4 backgroundColor( 0.06f, 0.08f, 0.1f, 1.0f );
//const DemoHelper::ViewStyle VIEW_STYLE( 0.08f, 0.45f, 80.f, 4.f );

const char* FLEX_DIRECTION[] = {
    "row",
    "row-reverse",
    "column",
    "column-reverse"
};

const unsigned int NUM_FLEX_DIRECTION = sizeof(FLEX_DIRECTION) / sizeof(char*);

const char* FLEX_WRAP[] = {
    "wrap",
    "no-wrap"
};

const unsigned int NUM_FLEX_WRAP = sizeof(FLEX_WRAP) / sizeof(char*);

const char* CONTENT_DIRECTION[] = {
    "LTR",
    "RTL"
};

const unsigned int NUM_CONTENT_DIRECTION = sizeof(CONTENT_DIRECTION) / sizeof(char*);

const char* JUSTIFY_CONTENT[] = {
    "flex-start",
    "center",
    "flex-end",
    "space-between",
    "space-around"
};

const unsigned int NUM_JUSTIFY_CONTENT = sizeof(JUSTIFY_CONTENT) / sizeof(char*);

const char* ALIGN_ITEMS[] = {
    "flex-start",
    "center",
    "flex-end",
    "stretch"
};

const unsigned int NUM_ALIGN_ITEMS = sizeof(ALIGN_ITEMS) / sizeof(char*);

const char* ALIGN_CONTENT[] = {
    "flex-start",
    "center",
    "flex-end",
    "stretch"
};

const unsigned int NUM_ALIGN_CONTENT = sizeof(ALIGN_CONTENT) / sizeof(char*);

} // unnamed namespace

/**
 * Layout Mirroring test using nested FlexBox containers.
 */
class LayoutMirroringExample : public ConnectionTracker
{
public:

  /**
   * Constructor
   * @param application class, stored as reference
   */
  LayoutMirroringExample( Application& application )
  : mApplication( application ),
    mCurrentFlexDirection( 0 ),
    mCurrentFlexWrap( 0 ),
    mCurrentContentDirection( 0 ),
    mCurrentJustifyContent( 0 ),
    mCurrentAlignItems( 0 ),
    mCurrentAlignContent( 0 )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &LayoutMirroringExample::OnInit );
  }

  /**
   * This method gets called once the main loop of application is up and running.
   */
  void OnInit( Application& app )
  {
    Stage stage = Dali::Stage::GetCurrent();
    stage.KeyEventSignal().Connect(this, &LayoutMirroringExample::OnKeyEvent);
    //stage.GetRootLayer().SetBehavior( Layer::LAYER_3D );
    stage.GetRootLayer().SetBehavior( Layer::LAYER_2D );
    Vector2 stageSize = Stage::GetCurrent().GetSize();

#if 1
    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    Layer contents = DemoHelper::CreateView( mApplication,
                                             mView,
                                             mToolBar,
                                             BACKGROUND_IMAGE,
                                             TOOLBAR_IMAGE,
                                             "Layout Mirroring" );
#endif
#if 1
    // Create a content direction toggle button. (left of toolbar)
    mContentDirectionButton = Toolkit::PushButton::New();
    mContentDirectionButton.SetName("mContentDirectionButton");
    mContentDirectionButton.SetUnselectedImage( DEMO_IMAGE_DIR "icon-replace.png" );
    mContentDirectionButton.SetSelectedImage( DEMO_IMAGE_DIR "icon-replace-selected.png" );
    mContentDirectionButton.ClickedSignal().Connect( this, &LayoutMirroringExample::OnContentDirectionButtonClicked);
    mContentDirectionButton.SetLeaveRequired( true );
    mToolBar.AddControl( mContentDirectionButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalLeft, DemoHelper::DEFAULT_MODE_SWITCH_PADDING );
#endif

    Vector2 flexSize( stageSize.width, stageSize.height - DemoHelper::DEFAULT_VIEW_STYLE.mToolBarHeight );
    flexSize = Vector2( 240.0f, 240.0f );

#if 0
    Actor container = Actor::New();
    container.SetName( "ContainerActor" );
    container.SetParentOrigin( ParentOrigin::TOP_CENTER );
    container.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    flexSize = Vector2( 240.0f, 240.0f );
    container.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    container.SetSize( flexSize );
    container.SetY( DemoHelper::DEFAULT_VIEW_STYLE.mToolBarHeight );
    stage.Add( container );
#endif

#ifdef BASIC_DEMO
    // Create the flex container for the flex items and add it to the base flex container
    mFlexItemContainer = FlexContainer::New();
    mFlexItemContainer.SetName( "FlexContainer" );
    //mFlexItemContainer.SetParentOrigin( ParentOrigin::TOP_LEFT );
    //mFlexItemContainer.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mFlexItemContainer.SetParentOrigin( ParentOrigin::CENTER );
    mFlexItemContainer.SetAnchorPoint( AnchorPoint::CENTER );
    mFlexItemContainer.SetPosition( 0.0f, 0.0f, 0.0f );
    //mFlexItemContainer.SetPosition( BORDER_WIDTH, BORDER_WIDTH, 0.0f );

    //mFlexItemContainer.SetSize( flexSize );
    mFlexItemContainer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS ); //todor: required?
    //mFlexItemContainer.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS ); //todor: required?
    mFlexItemContainer.SetBackgroundColor( Color::YELLOW );

    //mFlexItemContainer.RegisterProperty( "flexMargin", Vector4( 10.0f, 10.0f, 10.0f, 10.0f ), Property::READ_WRITE );
    //mFlexItemContainer.RegisterProperty( "flex", 0.05f, Property::READ_WRITE ); // 5 pecent of the container size in the main axis
    //mFlexItemContainer.RegisterProperty( "flex", 0.95f, Property::READ_WRITE ); // 95 percent of the container size in the main axis
    mFlexItemContainer.SetProperty( FlexContainer::Property::FLEX_DIRECTION, FLEX_DIRECTION[ mCurrentFlexDirection ] );
    //mFlexItemContainer.SetProperty( FlexContainer::Property::FLEX_DIRECTION, FLEX_DIRECTION[2] ); // column as main axis
    mFlexItemContainer.SetProperty( FlexContainer::Property::FLEX_WRAP, FLEX_WRAP[ mCurrentFlexWrap ] );
    //container.Add( mFlexItemContainer );
    float itemWidth = ( flexSize.width * ITEM_PERCENT_WIDTH ) / 100.0f;

    // Create flex items and add them to the container
    for( int i = 0; i < NUM_FLEX_ITEMS; ++i )
    {
      PushButton flexItem = PushButton::New();
      flexItem.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      flexItem.SetParentOrigin( ParentOrigin::TOP_LEFT );

      // Set different background colour to help to identify different items
      flexItem.SetBackgroundColor( Vector4( static_cast<float>( i ) / NUM_FLEX_ITEMS, static_cast<float>( NUM_FLEX_ITEMS - i ) / NUM_FLEX_ITEMS, 1.0f, 1.0f ) );
      flexItem.SetUnselectedImage( "" );
      flexItem.SetSelectedImage( "" );

      // Add a label to the button so that we can identify each item more easily
      std::ostringstream index;
      index << i + 1;
      flexItem.SetLabelText( index.str() );
      flexItem.SetName( "FlexItem " + index.str() );

      // Set a fixed size to the items so that we can wrap the line and test these
      // flex properties that only work when there are multiple lines in the layout
      flexItem.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
      // Make sure there are still extra space in the line after wrapping
      //flexItem.SetSize( ( flexSize.width / NUM_FLEX_ITEMS ) * 1.25f, flexSize.height * 0.95f / NUM_FLEX_ITEMS * 1.25f);
      flexItem.SetSize( itemWidth, itemWidth );

      mFlexItemContainer.Add( flexItem );
    }
#else
    // Nested FlexBox demo.
    mFlexItemContainer = FlexContainer::New();
    FlexDefaults( mFlexItemContainer );
    mFlexItemContainer.SetName( "FlexContainerRoot" );
    mFlexItemContainer.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mFlexItemContainer.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mFlexItemContainer.SetPosition( BORDER_WIDTH, BORDER_WIDTH );
    //mFlexItemContainer.SetParentOrigin( ParentOrigin::CENTER );
    //mFlexItemContainer.SetAnchorPoint( AnchorPoint::CENTER );
#if 0
    mFlexItemContainer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS ); //todor: required?
#else
    mFlexItemContainer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mFlexItemContainer.SetResizePolicy( ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::HEIGHT );
    //Vector3 sizeFactor( 0.0f, -30.0f * 1.0f, 0.0f );
    Vector3 sizeFactor( -BORDER_WIDTH * 2.0f, -BORDER_WIDTH * 2.0f, 0.0f ); // Negate parents padding.
    mFlexItemContainer.SetSizeModeFactor( sizeFactor );
#endif
    Vector4 color = backgroundColor;
#ifdef DEBUG_COLORS
    color = Color::YELLOW;
#endif
    mFlexItemContainer.SetBackgroundColor( color );

    mFlexItemContainer.SetProperty( FlexContainer::Property::FLEX_DIRECTION, FLEX_DIRECTION[ 2 ] ); // column as main axis
    mFlexItemContainer.SetProperty( FlexContainer::Property::FLEX_WRAP, FLEX_WRAP[ 1 ] );
    mFlexItemContainer.SetProperty( FlexContainer::Property::JUSTIFY_CONTENT, JUSTIFY_CONTENT[ 3 ] );
    mFlexContainers.push_back( mFlexItemContainer ); // Index 0

    FlexContainer flexContainerHeading = FlexContainer::New();
//    FlexDefaults( flexContainerHeading );
    flexContainerHeading.SetName( "FlexContainerHeading" );
    flexContainerHeading.SetParentOrigin( ParentOrigin::TOP_LEFT );
    flexContainerHeading.SetAnchorPoint( AnchorPoint::TOP_LEFT );

#if 1
    //flexContainerHeading.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS ); //todor: required?
    //flexContainerHeading.SetResizePolicy( ResizePolicy::FIXED, Dimension::HEIGHT ); //todor: required?
    flexContainerHeading.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT ); //todor: required?
    //flexContainerHeading.SetResizePolicy( ResizePolicy::FIXED, Dimension::HEIGHT ); //todor: required?
    //flexContainerHeading.SetSize( 0.0f, 30.0f );
    flexContainerHeading.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::WIDTH ); //todor: required?
#else
    flexContainerHeading.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS ); //todor: required?
    flexContainerHeading.SetSize( 240.0f, 30.0f );
#endif
#ifdef DEBUG_COLORS
    color = Color::CYAN;
#endif
    flexContainerHeading.SetBackgroundColor( color );
    flexContainerHeading.SetProperty( FlexContainer::Property::FLEX_DIRECTION, FLEX_DIRECTION[ 0 ] ); // column as main axis
    flexContainerHeading.SetProperty( FlexContainer::Property::FLEX_WRAP, FLEX_WRAP[ 1 ] );
//    flexContainerHeading.SetProperty( FlexContainer::Property::JUSTIFY_CONTENT, JUSTIFY_CONTENT[ 3 ] );
    mFlexContainers.push_back( flexContainerHeading ); // Index 1

#if 1
    ImageView headingIcon = ImageView::New( IMAGES[ 0 ] );
    headingIcon.SetName( "ImageView-headingIcon" );
    headingIcon.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    headingIcon.SetSize( 29.0f, 29.0f );
#else
    TextLabel headingIcon = TextLabel::New( "ICON" );
    headingIcon.SetName( "TextLabel-headingIcon" );
#ifdef DEBUG_COLORS
    color = Color::BLUE;
#endif
    headingIcon.SetBackgroundColor( color );
    headingIcon.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
#endif
    headingIcon.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    headingIcon.SetParentOrigin( ParentOrigin::TOP_LEFT );
    //headingIcon.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    headingIcon.RegisterProperty( "flexMargin", Vector4( 10.0f, 0.0f, 10.0f, 0.0f ) );
//    headingIcon.SetSize( 75.0f, 25.0f );
//    headingIcon.SetProperty( FlexContainer::Property::, FLEX_WRAP[ 1 ] );
    flexContainerHeading.Add( headingIcon );

    TextLabel headingTitle = TextLabel::New( "Settings" );
    headingTitle.SetName( "TextLabel-headingTitle" );
    headingTitle.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
#ifdef DEBUG_COLORS
    color = Color::BLUE;
#endif
    headingTitle.SetBackgroundColor( color );
    headingTitle.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    headingTitle.SetParentOrigin( ParentOrigin::TOP_LEFT );
    //headingTitle.RegisterProperty( "flex", 1 );
    headingTitle.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
//    headingTitle.SetSize( 75.0f, 25.0f );
    flexContainerHeading.Add( headingTitle );


    //todor scrollview of:
    FlexContainer scrollContainer = FlexContainer::New();
    scrollContainer.SetName( "FlexScrollContainer" );
    scrollContainer.SetParentOrigin( ParentOrigin::TOP_LEFT );
    scrollContainer.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    scrollContainer.SetPosition( 0.0f, 0.0f );

#if 0
    scrollContainer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    scrollContainer.SetResizePolicy( ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::HEIGHT );
    Vector3 sizeFactorSC( 0.0f, -30.0f * 1.0f, 0.0f );
    scrollContainer.SetSizeModeFactor( sizeFactorSC );
#else
//#ifdef USE_NEW_POLICY
//    scrollContainer.SetResizePolicy( ResizePolicy::USE_FLEXBOX_SIZE, Dimension::ALL_DIMENSIONS );
//#else
    scrollContainer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
//#endif
    //SIZE_FIXED_OFFSET_FROM_PARENT
#endif
    //scrollContainer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    //scrollContainer.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
#ifdef DEBUG_COLORS
    color = Color::WHITE;
#endif
    scrollContainer.SetBackgroundColor( color );
    FlexDefaults( scrollContainer );
    scrollContainer.SetProperty( FlexContainer::Property::FLEX_DIRECTION, FLEX_DIRECTION[ 2 ] ); // column as main axis
    scrollContainer.SetProperty( FlexContainer::Property::FLEX_WRAP, FLEX_WRAP[ 1 ] );
//    scrollContainer.SetProperty( FlexContainer::Property::JUSTIFY_CONTENT, JUSTIFY_CONTENT[ 3 ] );
    scrollContainer.SetProperty( FlexContainer::Property::JUSTIFY_CONTENT, JUSTIFY_CONTENT[ 0 ] );

#if 0
    Actor scrollContainer = Actor::New();
    scrollContainer.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    scrollContainer.SetParentOrigin( ParentOrigin::TOP_LEFT );
#if 0
    scrollContainer.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    scrollContainer.SetSize( 100.0f, 90.0f );
#else
    scrollContainer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
#endif

    //scrollContainer.SetBackgroundColor( Color::RED );
#endif
#if 0
    Control scrollContainer = Control::New();
    scrollContainer.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    scrollContainer.SetParentOrigin( ParentOrigin::TOP_LEFT );
    //scrollContainer.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    //scrollContainer.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    scrollContainer.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    scrollContainer.SetSize( 100.0f, 90.0f );
#ifdef DEBUG_COLORS
    color = Color::RED;
#endif
    scrollContainer.SetBackgroundColor( color );
#endif

#if 1
    for( int i = 0; i < NUM_SETTING_ITEMS; ++i )
    {
      std::cout << "todor: Setting up setting row #" << i << std::endl;
      FlexContainer scrollItem = FlexContainer::New();
      scrollItem.RegisterProperty( "flex", 1, Property::READ_WRITE );
      std::stringstream scrollItemName;
      scrollItemName << "FlexContainerScrollItem-" << ( i + 1 );
      scrollItem.SetName( scrollItemName.str() );
      scrollItem.SetParentOrigin( ParentOrigin::TOP_LEFT );
      scrollItem.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      //scrollItem.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
      //scrollItem.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
//      scrollItem.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      //scrollItem.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );// NOT VISIBLE, overruns end of box vertically
      //scrollItem.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );// VISIBLE, TAILING
//      scrollItem.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );// VISIBLE, fits in box vertically, but does not grow to size of text.
#ifdef USE_NEW_POLICY
#if 0
      scrollItem.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      scrollItem.SetResizePolicy( ResizePolicy::USE_FLEXBOX_SIZE, Dimension::HEIGHT );
#else
      scrollItem.SetResizePolicy( ResizePolicy::USE_FLEXBOX_SIZE, Dimension::ALL_DIMENSIONS );
#endif
#else
      scrollItem.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
#endif
//      scrollItem.SetResizePolicy( ResizePolicy::FIXED, Dimension::WIDTH );
//      scrollItem.SetSize( 200.0f, 30.0f );
//      scrollItem.SetSize( 150.0f, 0.0f );

      //scrollItem.SetBackgroundColor( Color::BLUE );
#ifdef DEBUG_COLORS
    color = Vector4( 1.0f, static_cast<float>( i ) / NUM_SETTING_ITEMS, static_cast<float>( NUM_SETTING_ITEMS - i ) / NUM_SETTING_ITEMS, 1.0f );
#endif
      scrollItem.SetBackgroundColor( color );
      FlexDefaults( scrollItem );

      scrollItem.SetProperty( FlexContainer::Property::FLEX_DIRECTION, FLEX_DIRECTION[ 0 ] );
      scrollItem.SetProperty( FlexContainer::Property::FLEX_WRAP, FLEX_WRAP[ 1 ] );
      scrollItem.SetProperty( FlexContainer::Property::JUSTIFY_CONTENT, JUSTIFY_CONTENT[ 0 ] );
      //mFlexContainers.push_back( scrollItem );

      //Dali::ResizePolicy::Type settingPolicyW = ResizePolicy::USE_NATURAL_SIZE;
      //Dali::ResizePolicy::Type settingPolicyH = ResizePolicy::USE_NATURAL_SIZE;
#if 1
      //Dali::ResizePolicy::Type settingPolicyH = ResizePolicy::USE_NATURAL_SIZE;
      //Vector2 settingSize( 0.0f, 0.0f );
#else
      //Dali::ResizePolicy::Type settingPolicyH = ResizePolicy::FIXED;
      //Vector2 settingSize( 0.0f, 30.0f );
#endif
      //Dali::ResizePolicy::Type settingPolicyW = ResizePolicy::USE_NATURAL_SIZE;
      //Dali::ResizePolicy::Type settingPolicyH = ResizePolicy::FILL_TO_PARENT;
#define ENABLE_TEXT_ALIGN_VERT

#if 1
    ImageView settingIcon = ImageView::New( IMAGES[ i + 1 ] );
    scrollItemName.str( "" );
    scrollItemName << "ImageView-settingIcon-" << ( i + 1 );
    settingIcon.SetName( scrollItemName.str() );
    settingIcon.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    settingIcon.SetSize( 29.0f, 29.0f );
#else
    TextLabel settingIcon = TextLabel::New( "S" );
    settingIcon.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    settingIcon.SetProperty( Dali::Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "BEGIN" );
    settingIcon.SetProperty( Dali::Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
#endif
#ifdef DEBUG_COLORS
    color = Vector4( 0.0f, 1.0f, 0.8f, 1.0f );
#endif
      settingIcon.SetBackgroundColor( color );
      settingIcon.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      settingIcon.SetParentOrigin( ParentOrigin::TOP_LEFT );

      //settingIcon.RegisterProperty( "flexMargin", Vector4( 5.0f, 0.0f, 5.0f, 0.0f ) );
#ifdef ENABLE_TEXT_ALIGN_VERT
      settingIcon.RegisterProperty( "alignSelf", "center", Property::READ_WRITE );
#endif
#if 1

      //settingIcon.SetResizePolicy( settingPolicyW, Dimension::WIDTH );
      //settingIcon.SetResizePolicy( settingPolicyH, Dimension::HEIGHT );
      //settingIcon.SetSize( settingSize );
#else
      settingIcon.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
      settingIcon.SetSize( 25.0f, 25.0f );
#endif
      scrollItem.Add( settingIcon );

      TextLabel settingTitle = TextLabel::New( "text" );
      scrollItemName.str( "" );
      scrollItemName << "TextLabel-settingTitle-" << ( i + 1 );
      settingTitle.SetName( scrollItemName.str() );
      settingTitle.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
      settingTitle.RegisterProperty( "flex", 1.0f, Property::READ_WRITE ); // 100 percent of the REMAINING container size in the main axis
#ifdef DEBUG_COLORS
    color = Vector4( 0.4f, 1.0f, 0.4f, 1.0f );
#endif
      settingTitle.SetBackgroundColor( color );
      settingTitle.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      settingTitle.SetParentOrigin( ParentOrigin::TOP_LEFT );
      settingTitle.SetProperty( Dali::Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "BEGIN" );
      settingTitle.SetProperty( Dali::Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
#ifdef ENABLE_TEXT_ALIGN_VERT
      settingTitle.RegisterProperty( "alignSelf", "center", Property::READ_WRITE );
#endif
      //settingTitle.RegisterProperty( "flex", 0.5f );
      //settingTitle.RegisterProperty( "flexMargin", Vector4( 5.0f, 0.0f, 5.0f, 0.0f ) );

      //settingTitle.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
#ifdef USE_NEW_POLICY
      settingTitle.SetResizePolicy( ResizePolicy::USE_FLEXBOX_SIZE, Dimension::WIDTH );
#else
      settingTitle.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
#endif
      settingTitle.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
#if 1
      //settingTitle.SetResizePolicy( settingPolicyW, Dimension::WIDTH );
      //settingTitle.SetResizePolicy( settingPolicyH, Dimension::HEIGHT );
      //settingTitle.SetSize( settingSize );
#else
      settingTitle.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
      settingTitle.SetSize( 75.0f, 25.0f );
#endif
      scrollItem.Add( settingTitle );

      TextLabel settingControl = TextLabel::New( "CTRL" );
      scrollItemName.str( "" );
      scrollItemName << "TextLabel-settingControl-" << ( i + 1 );
      settingControl.SetName( scrollItemName.str() );
      settingControl.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
#ifdef DEBUG_COLORS
    color = Vector4( 0.8f, 1.0f, 0.0f, 1.0f );
#endif
      settingControl.SetBackgroundColor( color );
      settingControl.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      settingControl.SetParentOrigin( ParentOrigin::TOP_LEFT );
      settingControl.SetProperty( Dali::Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "BEGIN" );
      settingControl.SetProperty( Dali::Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
#ifdef ENABLE_TEXT_ALIGN_VERT
      settingControl.RegisterProperty( "alignSelf", "flex-end", Property::READ_WRITE );
#endif
      //      settingControl.RegisterProperty( "flex", 1 );
      //settingControl.RegisterProperty( "flexMargin", Vector4( 5.0f, 0.0f, 5.0f, 0.0f ) );
#if 1
//      settingControl.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      settingControl.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
//      settingControl.SetSize( settingSize );
#else
      settingControl.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
      settingControl.SetSize( 40.0f, 25.0f );
#endif
      scrollItem.Add( settingControl );

      scrollItem.SetY( i * 30.0f );

      mFlexContainers.push_back( scrollItem ); // Index 2 +

      scrollContainer.Add( scrollItem );
    }
#endif

    mFlexItemContainer.Add( flexContainerHeading );
    mFlexItemContainer.Add( scrollContainer );


#endif
    mDesiredBox = Toolkit::ImageView::New( BORDER_IMAGE );
    stage.Add( mDesiredBox );
    mDesiredBox.SetSize( flexSize );
    mDesiredBox.SetParentOrigin( ParentOrigin::CENTER );
    mDesiredBox.SetAnchorPoint( AnchorPoint::CENTER );
    // Display the actor on the stage
    mDesiredBox.Add( mFlexItemContainer );
    //mDesiredBox.SetPadding( Padding( 20.0f, 20.0f, 20.0f, 20.0f ) );
    mDesiredBox.SetPadding( Padding( BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH ) );

    mGrabCorner = Toolkit::ImageView::New( RESIZE_HANDLE_IMAGE );
    mGrabCorner.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    mGrabCorner.SetName( "GrabCorner" );
    mGrabCorner.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
    mGrabCorner.SetParentOrigin( ParentOrigin::BOTTOM_RIGHT );
    mGrabCorner.SetPosition( -BORDER_WIDTH, -BORDER_WIDTH );
    mGrabCorner.SetOpacity( 0.6f );

    Layer grabCornerLayer = Layer::New();
    grabCornerLayer.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
    grabCornerLayer.SetParentOrigin( ParentOrigin::BOTTOM_RIGHT );
    grabCornerLayer.Add( mGrabCorner );
    mDesiredBox.Add( grabCornerLayer );

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach( mGrabCorner );
    mPanGestureDetector.DetectedSignal().Connect( this, &LayoutMirroringExample::OnPan );
  }

  bool OnContentDirectionButtonClicked( Toolkit::Button button )
  {
    mCurrentContentDirection = ( mCurrentContentDirection + 1 ) % NUM_CONTENT_DIRECTION;

#if 0
    mFlexItemContainer.SetProperty( FlexContainer::Property::CONTENT_DIRECTION, CONTENT_DIRECTION[ mCurrentContentDirection ] );
    //std::string value( CONTENT_DIRECTION[mCurrentContentDirection] );
    //SetTitle( "Content direction", value );
#ifndef BASIC_DEMO
    mFlexItemContainer.SetProperty( FlexContainer::Property::CONTENT_DIRECTION, CONTENT_DIRECTION[ mCurrentContentDirection ] );
#endif
#endif

#if 0
    int size = mFlexContainers.size();
    for( int i = 0; i < size; ++i )
    {
      std::cout << "todor: Changing direction of flex container: " << i << std::endl;
      mFlexContainers[i]->SetProperty( FlexContainer::Property::CONTENT_DIRECTION, CONTENT_DIRECTION[ mCurrentContentDirection ] );
    }
#endif
    //int size = mFlexContainers.size();
    for( std::vector<FlexContainer>::iterator it = mFlexContainers.begin(); it != mFlexContainers.end(); ++it )
    {
      //std::cout << "todor: Changing direction of flex container: " << i << std::endl;
      it->SetProperty( FlexContainer::Property::CONTENT_DIRECTION, CONTENT_DIRECTION[ mCurrentContentDirection ] );
    }
    return true;
  }

  void FlexDefaults( FlexContainer& flexContainer )
  {
    flexContainer.SetProperty( FlexContainer::Property::FLEX_DIRECTION, FLEX_DIRECTION[ 0 ] );
    flexContainer.SetProperty( FlexContainer::Property::FLEX_WRAP, FLEX_WRAP[ 1 ] );
    flexContainer.SetProperty( FlexContainer::Property::JUSTIFY_CONTENT, JUSTIFY_CONTENT[ 3 ] );
    flexContainer.SetProperty( FlexContainer::Property::ALIGN_CONTENT, ALIGN_CONTENT[ 0 ] );
    flexContainer.SetProperty( FlexContainer::Property::ALIGN_ITEMS, ALIGN_ITEMS[ 0 ] );
    flexContainer.SetProperty( FlexContainer::Property::CONTENT_DIRECTION, CONTENT_DIRECTION[ 0 ] );
  }

private:

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:

  void OnPan( Actor actor, const PanGesture& gesture )
  {
    std::cout << "todor: OnPan: ScrollItem policies: " <<
        mFlexContainers[2].GetResizePolicy( Dimension::WIDTH ) << "," <<
        mFlexContainers[2].GetResizePolicy( Dimension::HEIGHT ) <<
        "  ################################################################################################################" << std::endl;
    //mDesiredBox.RelayoutRequest( Dimension::ALL_DIMENSIONS );
    mDesiredBox.SetSize( mDesiredBox.GetTargetSize().GetVectorXY() + ( gesture.displacement * 2.0f ) );

#ifdef FORCE_RELAYOUT_PARENT
    mDesiredBox.SetInheritScale( true );
#endif
    //mFlexContainers.begin()->SetInheritScale( true );
#ifdef FORCE_RELAYOUT_CHILDREN
    int i = 0;
    //for( std::vector<FlexContainer>::iterator it = mFlexContainers.begin(); it != mFlexContainers.end(); ++it, ++i )
    for( std::vector<FlexContainer>::reverse_iterator it = mFlexContainers.rbegin(); it != mFlexContainers.rend(); ++it, ++i )
    {
      //if( i++ != 0 ) continue;
      //std::cout << "todor: Changing direction of flex container: " << i << std::endl;
      it->SetInheritScale( true );
    }
#endif
    //mFlexItemContainer.RelayoutRequest();
    //Stage stage = Stage::GetCurrent();
    // 1.0f and 0.75f are the maximum size caps of the resized image, as a factor of stage-size.
    //mImageStageScale.x = std::max( 0.05f, std::min( 0.95f,  mImageStageScale.x + ( gesture.displacement.x * 2.0f / stage.GetSize().width ) ) );
    //mImageStageScale.y = std::max( 0.05f, std::min( 0.70f, mImageStageScale.y + ( gesture.displacement.y * 2.0f / stage.GetSize().height ) ) );
    //ResizeImage();
  }

  Application& mApplication;

  Toolkit::Control mView;
  Toolkit::ToolBar mToolBar;
  FlexContainer mFlexContainer;
  FlexContainer mFlexItemContainer;

  int mCurrentFlexDirection;
  int mCurrentFlexWrap;
  int mCurrentContentDirection;
  int mCurrentJustifyContent;
  int mCurrentAlignItems;
  int mCurrentAlignContent;

  Toolkit::PushButton mFlexDirectionButton;
  Toolkit::PushButton mFlexWrapButton;
  Toolkit::PushButton mContentDirectionButton;
  Toolkit::PushButton mJustifyContentButton;
  Toolkit::PushButton mAlignItemsButton;
  Toolkit::PushButton mAlignContentButton;


  Toolkit::ImageView mDesiredBox; //< Background rectangle to show requested image size.
  Toolkit::ImageView mGrabCorner;
  PanGestureDetector mPanGestureDetector;
  std::vector< FlexContainer > mFlexContainers;

};

void RunTest( Application& app )
{
  LayoutMirroringExample test( app );

  app.MainLoop();
}

int main( int argc, char **argv )
{
  Application app = Application::New( &argc, &argv, DEMO_THEME_PATH );

  RunTest( app );

  return 0;
}
