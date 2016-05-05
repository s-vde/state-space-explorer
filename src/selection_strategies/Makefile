.PHONY:

#===== Makefile ==============================================================80
#
#               Makefile Template for Custom Selection Strategies
#
# author: 	Susanne van den Elsen
# date: 	2015
#
# This Makefile is to be specialized and extended with targets for custom
# selection strategies and included in the root directory for these strategies.
#
#===============================================================================

CUSTOM_SELECT               ?= .
custom_select_src           := $(CUSTOM_SELECT)
custom_select_srcs          := $(notdir $(shell find $(custom_select_src) -name "*.cpp")) dependence.cpp sleep_set.cpp
custom_select_include       := $(CUSTOM_SELECT)/.. $(CUSTOM_SELECT)/../sufficient_sets
custom_select_include_flag  := $(addprefix -I,$(custom_select_include))

VPATH                       += $(custom_select_src) $(custom_select_include)
