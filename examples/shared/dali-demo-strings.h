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

/* This header file includes all multi language strings which need display */
#ifndef __DALI_DEMO_STRINGS_H__
#define __DALI_DEMO_STRINGS_H__

#include <libintl.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define DALI_DEMO_DOMAIN_LOCAL "dali-demo"

#define DALI_DEMO_STR_EMPTY _("")

#define DALI_DEMO_STR_TEST dgettext(DALI_DEMO_DOMAIN_LOCAL, "IDS_ST_HEADER_PRIVACY")

#define DALI_DEMO_STR_TITLE_BUBBLES dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_BUBBLES")

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __DALI_DEMO_STRINGS_H__
