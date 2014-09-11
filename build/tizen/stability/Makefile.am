# Copyright (c) 2014 Samsung Electronics Co., Ltd.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


stability_src_dir = ../../../stability

bin_PROGRAMS = \
		application-build-destroy.stability \
		application-build-run-destroy.stability \
		application-build-run-idle-destroy.stability \
		application-build-run-init-idle-destroy.stability \
		application-build-call-destroy.stability

daliimagedir = $(appdatadir)/images/
dalimodeldir = $(appdatadir)/models/
daliscriptdir = $(appdatadir)/scripts/

BASE_CXXFLAGS = -I../../../stability \
                   -DDALI_IMAGE_DIR="\"${daliimagedir}\"" \
                   -DDALI_MODEL_DIR="\"${dalimodeldir}\"" \
                   -DDALI_SCRIPT_DIR="\"${daliscriptdir}\"" \
                   $(DALIDEMO_CFLAGS) \
                   $(ECORE_X_CFLAGS) \
                   $(CAPI_MEDIA_PLAYER_CFLAGS) \
                   $(CAPI_APPFW_APPLICATION_CFLAGS) \
                   -I/usr/include/media \
                   -Werror -Wall

STABILITY_CXXFLAGS      = $(DALI_CFLAGS) \
                        	$(DALI_TOOLKIT_CFLAGS) \
                        	$(BASE_CXXFLAGS)


STABILITY_DEPS =

STABILITY_LDADD = $(DALI_LIBS) $(DALI_TOOLKIT_LIBS) $(ECORE_X_LIBS) $(CAPI_MEDIA_PLAYER_LIBS) $(CAPI_APPFW_APPLICATION_LIBS) -lrt -lEGL


application_build_destroy_stability_SOURCES = $(stability_src_dir)/stability-application-build-destroy.cpp
application_build_destroy_stability_CXXFLAGS = $(STABILITY_CXXFLAGS)
application_build_destroy_stability_DEPENDENCIES = $(STABILITY_DEPS)
application_build_destroy_stability_LDADD = $(STABILITY_LDADD)

application_build_run_destroy_stability_SOURCES = $(stability_src_dir)/stability-application-build-run-destroy.cpp
application_build_run_destroy_stability_CXXFLAGS = $(STABILITY_CXXFLAGS)
application_build_run_destroy_stability_DEPENDENCIES = $(STABILITY_DEPS)
application_build_run_destroy_stability_LDADD = $(STABILITY_LDADD)

application_build_run_idle_destroy_stability_SOURCES = $(stability_src_dir)/stability-application-build-run-idle-destroy.cpp
application_build_run_idle_destroy_stability_CXXFLAGS = $(STABILITY_CXXFLAGS)
application_build_run_idle_destroy_stability_DEPENDENCIES = $(STABILITY_DEPS)
application_build_run_idle_destroy_stability_LDADD = $(STABILITY_LDADD)

application_build_run_init_idle_destroy_stability_SOURCES = $(stability_src_dir)/stability-application-build-run-init-idle-destroy.cpp
application_build_run_init_idle_destroy_stability_CXXFLAGS = $(STABILITY_CXXFLAGS)
application_build_run_init_idle_destroy_stability_DEPENDENCIES = $(STABILITY_DEPS)
application_build_run_init_idle_destroy_stability_LDADD = $(STABILITY_LDADD)

application_build_call_destroy_stability_SOURCES = $(stability_src_dir)/stability-application-build-call-destroy.cpp
application_build_call_destroy_stability_CXXFLAGS = $(STABILITY_CXXFLAGS)
application_build_call_destroy_stability_DEPENDENCIES = $(STABILITY_DEPS)
application_build_call_destroy_stability_LDADD = $(STABILITY_LDADD)

