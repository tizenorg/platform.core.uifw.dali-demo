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

#include <dali-toolkit/dali-toolkit.h>
#include "visual-profiler.h"
#include <stdio.h>

using namespace Dali;
using namespace Dali::Toolkit;

class PerformanceViewer : public ConnectionTracker
{
public:

  PerformanceViewer( Application& application )
  {
    application.InitSignal().Connect( this, &PerformanceViewer::Create );
  }

  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();

    mServerText = TextField::New();
    mServerText.SetProperty( TextField::Property::TEXT, "127.0.0.1" );
    mServerText.SetProperty( TextField::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mServerText.SetProperty( TextField::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    mServerText.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mServerText.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mServerText.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "SIZE_RELATIVE_TO_PARENT" );
    mServerText.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "SIZE_RELATIVE_TO_PARENT" );
    mServerText.SetProperty( Actor::Property::SIZE_MODE_FACTOR, Vector3( 0.4f, 0.1f, 1.0f ) );
    Property::Map map;
    map[ "rendererType" ] = "color";
    map[ "mixColor" ] = Color::WHITE;
    mServerText.SetProperty( Toolkit::Control::Property::BACKGROUND, map );
    stage.Add( mServerText );

    mPortText = TextField::New();
    mPortText.SetProperty( TextField::Property::TEXT, "3031" );
    mPortText.SetProperty( TextField::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mPortText.SetProperty( TextField::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    mPortText.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mPortText.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mPortText.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "SIZE_RELATIVE_TO_PARENT" );
    mPortText.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "SIZE_RELATIVE_TO_PARENT" );
    mPortText.SetProperty( Actor::Property::SIZE_MODE_FACTOR, Vector3( 0.2f, 0.1f, 1.0f ) );
    map[ "mixColor" ] = Color::YELLOW;
    mPortText.SetProperty( Control::Property::BACKGROUND, map );
    stage.Add( mPortText );

    mConnectButton = PushButton::New();
    mConnectButton.SetAnchorPoint( AnchorPoint::TOP_RIGHT );
    mConnectButton.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    mConnectButton.SetProperty( Button::Property::LABEL_TEXT ,"Connect" );
    mConnectButton.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "SIZE_RELATIVE_TO_PARENT" );
    mConnectButton.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "SIZE_RELATIVE_TO_PARENT" );
    mConnectButton.SetProperty( Actor::Property::SIZE_MODE_FACTOR, Vector3( 0.4f, 0.1f, 1.0f ) );
    stage.Add( mConnectButton );

    mConnectButton.ClickedSignal().Connect( this, &PerformanceViewer::OnButtonClicked );
  }

  bool OnButtonClicked( Button button )
  {
    std::string server = mServerText.GetProperty( TextField::Property::TEXT ).Get< std::string >();
    std::string port = mPortText.GetProperty( TextField::Property::TEXT ).Get< std::string >();
    mProfiler.Connect( server.c_str(), atoi( port.c_str() ) );
    BufferImage bufferImage = mProfiler.GetGraph();

    ImageView profileGraph = ImageView::New( bufferImage );
    profileGraph.SetSize( Vector2( bufferImage.GetWidth(),bufferImage.GetHeight()));

    profileGraph.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    profileGraph.SetPosition( 0,0);

    Stage stage = Stage::GetCurrent();
    stage.Add( profileGraph );
    stage.Remove( mServerText );
    stage.Remove( mPortText );
    stage.Remove( mConnectButton );
    return true;
  }

private:
  Profiler mProfiler;
  TextField mServerText;
  TextField mPortText;
  Button mConnectButton;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  PerformanceViewer test( application );
  application.MainLoop();
  return 0;
}
