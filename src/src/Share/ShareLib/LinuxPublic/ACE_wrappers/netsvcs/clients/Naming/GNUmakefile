# -*- makefile -*-
#----------------------------------------------------------------------------
#       GNU ACE Workspace
#
# $Id: GNUACEWorkspaceCreator.pm 90264 2010-05-24 15:19:01Z mitza $
#
# This file was generated by MPC.  Any changes made directly to
# this file will be lost the next time it is generated.
#
# MPC Command:
# /tmp/DOC_ROOT/stage-4456/ACE_wrappers/bin/mwc.pl -type gnuace -exclude "TAO/TAO_*.mwc,TAO/CIAO/CIAO_*.mwc" -recurse -hierarchy -relative ACE_ROOT=/tmp/DOC_ROOT/stage-4456/ACE_wrappers -relative TAO_ROOT=/tmp/DOC_ROOT/stage-4456/ACE_wrappers/TAO -relative CIAO_ROOT=/tmp/DOC_ROOT/stage-4456/ACE_wrappers/TAO/CIAO -relative DANCE_ROOT=/tmp/DOC_ROOT/stage-4456/ACE_wrappers/TAO/CIAO/DAnCE
#
#----------------------------------------------------------------------------

MAKEFILE = GNUmakefile

ifeq ($(findstring k,$(MAKEFLAGS)),k)
  KEEP_GOING = -
endif

include $(ACE_ROOT)/include/makeinclude/macros.GNU

all: Netsvcs_Client_Test Netsvcs_Dump_Restore

REMAINING_TARGETS := $(filter-out all,$(TARGETS_NESTED:.nested=)) $(CUSTOM_TARGETS)

$(REMAINING_TARGETS):
	$(KEEP_GOING)@cd Client && $(MAKE) -f GNUmakefile.Netsvcs_Client_Test $(@)
	$(KEEP_GOING)@cd Dump_Restore && $(MAKE) -f GNUmakefile.Netsvcs_Dump_Restore $(@)

.PHONY: Netsvcs_Client_Test
Netsvcs_Client_Test:
	$(KEEP_GOING)@cd Client && $(MAKE) -f GNUmakefile.Netsvcs_Client_Test all

.PHONY: Netsvcs_Dump_Restore
Netsvcs_Dump_Restore:
	$(KEEP_GOING)@cd Dump_Restore && $(MAKE) -f GNUmakefile.Netsvcs_Dump_Restore all

project_name_list:
	@echo Netsvcs_Client_Test
	@echo Netsvcs_Dump_Restore
