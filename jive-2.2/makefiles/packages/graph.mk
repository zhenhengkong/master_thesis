
# Generated on zo 19 mei 2019 19:22:50 CEST.
# CHANGES TO THIS FILE WILL BE LOST.

ifndef JIVE_GRAPH_MAKEDEFS_MK
       JIVE_GRAPH_MAKEDEFS_MK = 1

ifndef JIVEPATH
  JIVEPATH := $(JIVEDIR)
endif

include $(JIVEPATH)/makefiles/jive.mk

include $(JIVEPATH)/makefiles/packages/util.mk

JIVE_PACKAGES += graph
JIVE_LIBS     := jivegraph $(JIVE_LIBS)

endif
