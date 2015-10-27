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
#include <stdio.h>
#include <dali-toolkit/dali-toolkit.h>
#include <cstdarg>
#include <sstream>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace std;

std::string stringf(const char* format, ...)
{
    va_list arg_list;                                                           
    va_start(arg_list, format);                                                 

    // SUSv2 version doesn't work for buf NULL/size 0, so try printing
    // into a small buffer that avoids the double-rendering and alloca path too...
    char short_buf[256];                                                        
    const size_t needed = vsnprintf(short_buf, sizeof short_buf,
                                    format, arg_list) + 1;
    if (needed <= sizeof short_buf)
        return short_buf;

    // need more space...
    char* p = static_cast<char*>(alloca(needed));
    vsnprintf(p, needed, format, arg_list);
    return p;
}


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

    // exceed policy

    //TextView text = TextView::New("This example shows how to create and display Hello World! using a simple TextActor. The Init signal is received once (only) during the Application lifetime. Connect to the Application's Init signal.");
    //text.SetSize(100,100);

    //text.SetWidthExceedPolicy(TextView::Fade);
    //text.SetWidthExceedPolicy(TextView::Split);
    //text.SetWidthExceedPolicy(TextView::ShrinkToFit);
    //text.SetWidthExceedPolicy(TextView::EllipsizeEnd);

    //text.SetHeightExceedPolicy(TextView::Fade);
    //text.SetHeightExceedPolicy(TextView::ShrinkToFit);
    //text.SetHeightExceedPolicy(TextView::EllipsizeEnd);

    // alignment

    //TextView text = TextView::New("Connect to the Application's Init signal.");
    TextLabel text = TextLabel::New();
    text.SetProperty( TextLabel::Property::TEXT, "Connect to the Application's Init signal." );
    text.SetSize(400,400);

    //text.SetTextAlignment(Alignment::HorizontalLeft);
    //text.SetTextAlignment(Alignment::VerticalBottom);
    //text.SetTextAlignment(Alignment::Type(Alignment::HorizontalLeft | Alignment::VerticalBottom));

    text.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    text.SetProperty( TextLabel::Property::MULTI_LINE, true );
    text.SetProperty( TextLabel::Property::TEXT_COLOR, Vector4(1,1,1,1) );

    text.SetParentOrigin(ParentOrigin::CENTER);
    text.SetBackgroundColor(Vector4(0,0,1,1));
    text.SetBackgroundImage(ResourceImage::New(DALI_IMAGE_DIR "gallery-large-1.jpg"));
    Stage::GetCurrent().Add(text);

    cout << "=============================" << endl;
    cout << "text.GetTargetSize() " << text.GetTargetSize() << endl;
    //cout << "text length " << text.GetText().size() << endl;
    cout << "text length " << text.GetProperty(TextLabel::Property::TEXT).Get<std::string>().size() << endl;
    cout << endl;

    //TextView::TextLayoutInfo textInfo;
    //text.GetTextLayoutInfo(textInfo);
    //cout << "TextLayoutInfo mTextSize: " << textInfo.mTextSize << endl;
    //cout << endl;
    //cout << "LineLayoutInfoContainer size: " << textInfo.mLines.size() << endl;
    //for(uint i=0; i<textInfo.mLines.size(); ++i)
    //{
      //stringstream ss;
      //ss << textInfo.mLines[i].mSize;
      //cout << stringf("  line[%d].mSize: %s", i, ss.str().c_str()) << endl;
    //}
    //cout << endl;
    //cout << "CharacterLayoutInfoContainer size: " << textInfo.mCharacterLayoutInfoTable.size() << endl;
    //for(uint i=0; i<textInfo.mCharacterLayoutInfoTable.size(); ++i)
    //{
      ////TextView::CharacterLayoutInfo& charInfo = textInfo.mCharacterLayoutInfoTable[i];
      ////stringstream ss1, ss2;
      ////ss1 << charInfo.mPosition;
      ////ss2 << charInfo.mSize;
      //////cout << stringf("  char[%d]: %c, mPosition: %s, mSize: %s ", i, text.GetText()[i], ss1.str().c_str(), ss2.str().c_str()) << endl;
    //}
    //cout << "=============================" << endl;

  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    //mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
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
