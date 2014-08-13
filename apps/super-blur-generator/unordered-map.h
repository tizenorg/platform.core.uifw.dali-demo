/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

#ifndef SUPER_BLUR_GENERATOR_UNORDERED_MAP_H_
#define SUPER_BLUR_GENERATOR_UNORDERED_MAP_H_

#ifndef __GXX_EXPERIMENTAL_CXX0X__
#if ! ( __cplusplus > 199711L )

#include<tr1/unordered_map>

template<typename Key, typename Value>
struct UnorderedMap
{
  typedef std::tr1::unordered_map<Key, Value> Type;
};
#define UNORDERED_MAP_DEFINED

#endif
#endif

#ifndef UNORDERED_MAP_DEFINED

#include<unordered_map>
template<typename Key, typename Value>
struct UnorderedMap
{
  typedef std::unordered_map<Key, Value> Type;
};
typedef std::unordered_map UnorderedMap;

#endif

#endif // SUPER_BLUR_GENERATOR_UNORDERED_MAP_H_

