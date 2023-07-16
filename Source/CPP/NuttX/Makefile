############################################################################
# apps/examples/ssem/Makefile
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.  The
# ASF licenses this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance with the
# License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.
#
############################################################################

include $(APPDIR)/Make.defs

# SSEM Application

MAINSRC = NuttX/ssem_main.cxx

#
#	We need to ignnore the ssem_main.cxx file here as it is included in the MAINSRC
#	variable.  Including the file in the source file list will generate a warning
#	when we build.
#
srcfiles := $(shell find . -type f \( -iname "*.cxx" ! -iname "ssem_main.cxx" \))

CXXSRCS = $(srcfiles)

INCLUDES += -I./Core -I./UnitTests -I./include -I./Nuttx -I.

CXXFLAGS += -Wall -Wextra -pedantic $(INCLUDES)

# SSEM built-in application info

PROGNAME = ssem
PRIORITY = SCHED_PRIORITY_DEFAULT
STACKSIZE = $(CONFIG_DEFAULT_TASK_STACKSIZE)
MODULE = $(CONFIG_EXAMPLES_SSEM)

checkgenromfs:
	@genromfs -h 1>/dev/null 2>&1 || { \
 echo "Host executable genromfs not available in PATH"; \
 echo "You may need to download in from http://romfs.sourceforge.net/"; \
 exit 1; \
	}

ssemapps.img : checkgenromfs ssemapps
	@genromfs -f $@ -d SSEMApps -V "SSEMApps" || { echo "genromfs failed" ; exit 1 ; }

romfs_ssemapps.h : ssemapps.img
	@xxd -i $< >$@ || { echo "xxd of $< failed" ; exit 1 ; }

context:: romfs_ssemapps.h

# clean::
# 	$(call DELFILE, ssemapps.img)
# 	$(call DELFILE, romfs_ssemapps.h)

distclean::
	$(call DELFILE, ssemapps.img)
	$(call DELFILE, romfs_ssemapps.h)

include $(APPDIR)/Application.mk
