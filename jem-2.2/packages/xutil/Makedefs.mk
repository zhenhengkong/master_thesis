
# Generated on zo 19 mei 2019 19:19:39 CEST.
# CHANGES TO THIS FILE WILL BE LOST.


ifndef JEM_XUTIL_MAKEDEFS_MK
       JEM_XUTIL_MAKEDEFS_MK = 1

ifndef JEMPATH
  JEMPATH := $(JEMDIR)
endif
include $(JEMPATH)/makefiles/packages/util.mk

JEM_PACKAGES += xutil
JEM_LIBS     := jemxutil $(JEM_LIBS)


endif
