#  This needs to be cleared explicitly, as the libfreeradius-ldap.mk
#  might not always be available, and the TARGETNAME from the previous
#  target may stick around.
TARGETNAME=
-include $(top_builddir)/src/modules/rlm_ldap/libfreeradius-ldap.mk

ifneq "${TARGETNAME}" ""
  TARGETNAME	:= rlm_ldap
  TARGET	    := $(TARGETNAME).a
endif

SOURCES		    := $(TARGETNAME).c attrmap.c clients.c groups.c user.c conn.c

SRC_CFLAGS	+= -I$(top_builddir)/src/modules/rlm_ldap
TGT_PREREQS	:= libfreeradius-ldap.a
