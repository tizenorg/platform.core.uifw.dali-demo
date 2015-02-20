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

#include "dali-table-view.h"
#include "examples/shared/dali-demo-strings.h"

using namespace Dali;

int main(int argc, char **argv)
{
  // Configure gettext for internalization
  bindtextdomain(DALI_DEMO_DOMAIN_LOCAL, DALI_LOCALE_DIR);
  textdomain(DALI_DEMO_DOMAIN_LOCAL);
  setlocale(LC_ALL, DALI_LANG);

  Application app = Application::New(&argc, &argv);

  DaliTableView demo(app);

  demo.AddExample(Example("bubble-effect.example", DALI_DEMO_STR_TITLE_BUBBLES));
  demo.AddExample(Example("blocks.example", DALI_DEMO_STR_TITLE_BLOCKS));
  demo.AddExample(Example("cluster.example", DALI_DEMO_STR_TITLE_CLUSTER));
  demo.AddExample(Example("cube-transition-effect.example", DALI_DEMO_STR_TITLE_CUBE_TRANSITION));
  demo.AddExample(Example("dissolve-effect.example", DALI_DEMO_STR_TITLE_DISSOLVE_TRANSITION));
  demo.AddExample(Example("item-view.example", "Item View"));
  demo.AddExample(Example("magnifier.example", "Magnifier"));
  demo.AddExample(Example("motion-blur.example", "Motion Blur"));
  demo.AddExample(Example("motion-stretch.example", "Motion Stretch"));
  demo.AddExample(Example("page-turn-view.example", "Page Turn View"));
  demo.AddExample(Example("radial-menu.example", "Radial Menu"));
  demo.AddExample(Example("refraction-effect.example", "Refraction"));
  demo.AddExample(Example("scroll-view.example", "Scroll View"));
  demo.AddExample(Example("shadow-bone-lighting.example", "Lights and shadows"));
  demo.AddExample(Example("builder.example", "Script Based UI"));
  demo.AddExample(Example("image-scaling-irregular-grid.example", "Image Scaling Modes"));
  demo.AddExample(Example("text-view.example", "Text View"));
  app.MainLoop();

  return 0;
}
