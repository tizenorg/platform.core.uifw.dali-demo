#
# Copyright (c) 2014 Samsung Electronics Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Builds super blur generator

bin_PROGRAMS = \
    super-blur-generator

apps_src_dir = ../../../apps
include ../../../apps/file.list

superblurgeneratorresourcesdir = $(appdatadir)resources/
superblurgeneratorresources_DATA = $(super_blur_generator_resource_files)


super_blur_generator_SOURCES = \
        $(super_blur_generator_src_files)

super_blur_generator_CXXFLAGS = \
        -DRESOURCES_DIR="\"$(superblurgeneratorresourcesdir)\"" \
        -I../../../apps/super-blur-generator -I../../.. \
        $(DLOG_CFLAGS) \
        $(DALI_CFLAGS) \
        $(DALI_TOOLKIT_CFLAGS) \
        $(OPENGLES20_CFLAGS) \
        -Werror -Wall -g

super_blur_generator_DEPENDENCIES =

super_blur_generator_LDADD = \
        $(DLOG_LIBS) \
        $(DALI_LIBS) \
        $(DALI_TOOLKIT_LIBS) \
        $(OPENGLES20_LIBS)
