//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

//------------------------------------------------------------------------------
//
// Dump out discoverable properties
//
//------------------------------------------------------------------------------

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <boost/scoped_ptr.hpp>

//#include <boost/regex.hpp>
#include "sys/stat.h"
#include <ctime>

#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const int INDENT = 2;

// dump property to stream in json format
void dumpProperty(Property::Value& value, const int currentIndent, std::ostream& out);

// dump properties to stream in json format
void dumpProperties(Handle handle, std::string& name, unsigned int indent, std::ostream& out);

inline int indentOut(std::ostream& out, int indent)
{
  for(int j =0; j < indent; ++j) out << " ";
  return indent;
}

// dump all properties of type in TypeRegistry to the stream
void dump( std::ostream& out )
{
  int indent = 0;
  out << "{" << std::endl;

  indent = indentOut(out, indent + INDENT);
  out << "// default properties" << std::endl;
  indentOut(out, indent);
  out << "\"styles\":{" << std::endl;
  indent = indentOut(out, indent + INDENT);

  typedef TypeRegistry::NameContainer Collection;
  Collection names = TypeRegistry::Get().GetTypeNames();
  for(Collection::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( *iter );

    BaseHandle baseHandle = typeInfo.CreateInstance();
    Handle handle = Handle::DownCast(baseHandle);
    if( !handle )
    {
      std::cout << "Can't create a property holder (or it isnt one):'" << *iter << "'" << std::endl;
    }
    else
    {
      out << "\"Default" << *iter << "\":" << "{" << std::endl;
      indent = indentOut(out, indent + INDENT);
      out << "\"type\":\"" << *iter << "\"";
      if(0 == handle.GetPropertyCount())
      {
        out << std::endl;
      }
      else
      {
        out << "," << std::endl;
      }
      dumpProperties(handle, *iter, indent, out);
      indent = indentOut(out, indent - INDENT);
      if(iter == names.end()-1)
      {
        out << "}" << std::endl;
      }
      else
      {
        out << "}," << std::endl;
        indent = indentOut(out, indent);
      }
    }
  }

  indent = indentOut(out, indent - INDENT);
  out << "}" << std::endl;

  indent = indentOut(out, indent - INDENT);
  out << "}" << std::endl;
}

void dumpProperties(Handle handle, std::string& name, unsigned int indent, std::ostream& out)
{
  for(unsigned int i = 0; i < handle.GetPropertyCount(); ++i)
  {
    std::string name( handle.GetPropertyName(i) );

    indentOut(out, indent);

    out << "\"" << name << "\":";

    Property::Value value = handle.GetProperty(i);
    dumpProperty( value, indent, out );

    if(i != (handle.GetPropertyCount() - 1))
    {
      out << "," << std::endl;
    }
    else
    {
      out << std::endl;
    }

  }
}

void dumpProperty(Property::Value& value, const int currentIndent, std::ostream& out)
{
  switch( value.GetType() )
  {
    case Property::BOOLEAN:
    {
      if(value.Get<bool>())
      {
        out << "true";
      }
      else
      {
        out << "false";
      }
    }
    break;
    case Property::FLOAT:
    {
      out << value.Get<float>();
    }
    break;
    case Property::INTEGER:
    {
      out << value.Get<int>();
    }
    break;
    case Property::UNSIGNED_INTEGER:
    {
      out << value.Get<unsigned int>();
    }
    break;
    case Property::VECTOR2:
    {
      Vector2 v = value.Get<Vector2>();
      out << "[" << v.x << "," << v.y << "]";
    }
    break;
    case Property::VECTOR3:
    {
      Vector3 v = value.Get<Vector3>();
      out << "[" << v.x << "," << v.y << "," << v.z << "]";
    }
    break;
    case Property:: VECTOR4:
    {
      Vector4 v = value.Get<Vector4>();
      out << "[" << v.x << "," << v.y << "," << v.z << "," << v.w << "]";
    }
    break;
    case Property::MATRIX3:
    {
      float* v = value.Get<Matrix3>().AsFloat();
      out << "[";
      for(int j = 0; j < 8; ++j)
      {
        out << v[j] << ",";
      }
      out << v[8] << "]";
    }
    break;
    case Property::MATRIX:
    {
      float *v = value.Get<Matrix>().AsFloat();
      out << "[";
      for(int j = 0; j < 15; ++j)
      {
        out << v[j] << ",";
      }
      out << v[15] << "]";
    }
    break;
    case Property::RECTANGLE:
    {
      Rect<int> v = value.Get<Rect<int> >();
      out << "[" << v.x << "," << v.y << "," << v.width << "," << v.height << "]";
    }
    break;
    case Property::ROTATION:
    {
      Vector4 v = value.Get<Quaternion>().AsVector();
      out << "[" << v.x << "," << v.y << "," << v.z << "," << v.w << "]";
    }
    break;
    case Property::STRING:
    {
      out << "\"" << value.Get<std::string>() << "\"" ; // @todo quote internal strings
    }
    break;
    case Property::ARRAY:
    {
      out << "[";
      if( value.GetSize() )
      {
        for(int j = 0; j < value.GetSize() - 1 ; ++j)
        {
          dumpProperty(value.GetItem(j), currentIndent, out);
          out << ",";
        }
        dumpProperty(value.GetItem(value.GetSize() - 1), currentIndent, out);
      }
      out << "]";
    }
    break;
    case Property::MAP:
    {
      out << "{";
      if( value.GetSize() )
      {
        for(int j = 0; j < value.GetSize() - 1 ; ++j)
        {
          out << "\"" << value.GetKey(j) << "\":";
          dumpProperty(value.GetItem(j), currentIndent, out);
          out << ",";
        }
        out << "\"" << value.GetKey(value.GetSize() - 1) << "\":";
        dumpProperty(value.GetItem(value.GetSize() - 1), currentIndent, out);
      }
      out << "}";
    }
    break;
    case Property::NONE:
    case Property::TYPE_COUNT:
    {
      out << "null";
      DALI_ASSERT_ALWAYS("Bad type");
    }
    break;
  }
} // dumpProperty


} // anon namespace

//------------------------------------------------------------------------------
//
// Writes a json file with all the discoverable types and properties in Dali
//
//------------------------------------------------------------------------------
class TheApp : public ConnectionTracker
{
public:
  TheApp(Application &app) : mApp(app), mDumped(false)
  {
    app.InitSignal().Connect(this, &TheApp::Create);
  }

  ~TheApp() {}

public:

  void Create(Application& app)
  {
    // reference something in toolkit to load it in
    Toolkit::ImageView imageView = Toolkit::ImageView::New();

    mTimer = Timer::New( 2000 ); // ms
    mTimer.TickSignal().Connect( this, &TheApp::OnTimer);
    mTimer.Start();
  }

  Timer mTimer;

  bool OnTimer(void)
  {
    mTimer.SetInterval(5000);

    if(!mDumped)
    {
      if(mFileName.size())
      {
        std::ofstream myfile;
        myfile.open(mFileName.c_str());
        dump(myfile);
        myfile.close();
      }
      else
      {
        dump(std::cout);
      }
      mDumped = true;
    }
    else
    {
      mApp.Quit();
    }
    return true;
  }

  void SetFileName(const std::string& f)
  {
    mFileName = f;
  }

private:
  Application& mApp;
  std::string mFileName;
  bool mDumped;
};


//------------------------------------------------------------------------------
//
//
//
//------------------------------------------------------------------------------
int main(int argc, char **argv)
{
  Application dali_app = Application::New(&argc, &argv);

  TheApp app(dali_app);

  if(argc > 1)
  {
    std::cout << "Saving to file:" << argc << " " << argv[1] << std::endl;

    app.SetFileName(argv[1]);
  }

  dali_app.MainLoop();

  return 0;
}
