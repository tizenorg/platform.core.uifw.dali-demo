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
#include <iostream>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace std;

// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application )
  {
    std::cout << "HelloWorldController::HelloWorldController" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &HelloWorldController::Create );
  }

  ~HelloWorldController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    std::cout << "HelloWorldController::Create" << std::endl;

    mTblView = TableView::New(3,3);
    mTblView.SetAnchorPoint( AnchorPoint::CENTER );
    mTblView.SetParentOrigin( ParentOrigin::CENTER );
    //mTblView.SetBackgroundColor(Vector4(1,0,0,0));
    Stage::GetCurrent().Add(mTblView);

    mTblView.SetCellPadding(Size(10,10));
    mTblView.SetSize(300,300);

    Actor c00 = ImageActor::New(ResourceImage::New(DALI_IMAGE_DIR "gallery-large-1.jpg"));
    c00.SetSize(50,50);
    mTblView.AddChild( c00, TableView::CellPosition( 0, 0 ) );

    Actor c22 = ImageActor::New(ResourceImage::New(DALI_IMAGE_DIR "gallery-large-2.jpg"));
    c22.SetSize(50,50);
    mTblView.AddChild( c22, TableView::CellPosition( 2, 2 ) );

    //mTblView.SetFixedWidth(0, 200);
    mTblView.SetCellAlignment( TableView::CellPosition(0,0), HorizontalAlignment::RIGHT,  VerticalAlignment::BOTTOM );

    // Respond to a click anywhere on the stage
    Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    cout << "c00" << endl;
    cout << "size " << mTblView.GetChildAt(TableView::CellPosition(0,0)).GetCurrentSize() << endl;
    cout << "pos " << mTblView.GetChildAt(TableView::CellPosition(0,0)).GetCurrentPosition() << endl;
    cout << "c22" << endl;
    cout << "size " << mTblView.GetChildAt(TableView::CellPosition(2,2)).GetCurrentSize() << endl;
    cout << "pos " << mTblView.GetChildAt(TableView::CellPosition(2,2)).GetCurrentPosition() << endl;
    cout << endl;
    return true;
  }

private:
  Application&  mApplication;
  TableView mTblView;
};

void RunTest( Application& application )
{
  HelloWorldController test( application );

  application.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
