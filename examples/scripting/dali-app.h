#ifndef DALI_APP_H
#define DALI_APP_H

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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

/**
 * Example can load both JSON and JavaScript files from command line
 * E.g.   scripting.example my-first.js
 */
class DaliApplication: public Dali::ConnectionTracker
{
public:

  DaliApplication( Dali::Application application, std::string layoutFileName, std::string scriptFileName );

  ~DaliApplication();

  void Create( Dali::Application& application );

  void MainLoop();

private:

  bool ExecuteScript();

  Dali::Toolkit::Script mScript;
  Dali::Toolkit::Builder mBuilder;
  Dali::Application mApplication;
  std::string mLayoutFileName;
  std::string mScriptFileName;
};

#endif // header
