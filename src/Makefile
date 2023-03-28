TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

# Use typed rset structure (see 3.16.1 release notes)
USR_CFLAGS += -DUSE_TYPED_RSET

LIBRARY_IOC += Ferrite

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install Ferrite.dbd into <top>/dbd
DBD += Ferrite.dbd

# specify all source files to be compiled and added to the library
Ferrite_SRCS += \
	Ferrite.c \
	_interface.c \
	_record.c \
	_variable.c \
	ai.c \
	ao.c \
	aai.c \
	aao.c \
	waveform.c \
	bi.c \
	bo.c \
	longin.c \
	longout.c \
	mbbi_direct.c \
	mbbo_direct.c \
	stringin.c \
	stringout.c

Ferrite_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

