.PHONY: explorer install

#===== Makefile ==============================================================80
#
# 			    				State-Space-Explorer
#
# author: 	Susanne van den Elsen
# date: 	2015
#
# This file is to be included by the Makefile of projects building on
# state-space explorer. It includes the Makefiles of included subprojects,
# defines paths and defines targets for State-Space-Explorer files, specifying
# their dependencies.
#
#===============================================================================

#===== Variables =============================================================80
#
# User defined:
#	EXPLORER        : The root directory of State-Space-Explorer (absolute path
#                     or relative path from including project).
#
# Defined here:
#	explorer_src    : The directory of State-space explorer source files.
#	explorer_sdirs  : List of subdirectories of $(explorer_src).
#	explorer_srcs   : List of State-Space-Explorer source files.
# 	explorer_file   : The name of the file produced by target explore.
#	CUSTOM_SELECT	: The directory where the custom selection strategies are
#					  located.
#	explorer_objs_o : List of State-Space-Explorer object files.
#
#===============================================================================

EXPLORER			?= .
explorer_src		:= $(EXPLORER)/src
explorer_sdirs		:= $(explorer_src)/sufficient_sets $(explorer_src)/bound_functions
explorer_srcs		:= $(foreach subdir, $(explorer_src) $(explorer_sdirs), $(notdir $(wildcard $(subdir)/*.cpp)))
explorer_file		:= explore
CUSTOM_SELECT		:= $(explorer_src)/selection_strategies

#===== Include =====

include Makefile.config
include $(RECORD_REPLAY)/Makefile

#===== Variables =====

SUFFIXES            := .bc .cpp .hpp .o
VPATH               += $(explorer_src) $(explorer_sdirs)

objs                = $(addprefix $(BUILD)/,$(subst .cpp,$(2),$(1)))
explorer_objs	 	:= $(call objs,$(program_model_srcs) $(scheduler_srcs_replay) $(utils_srcs) $(explorer_srcs),.o)
explorer_deps		:= $(explorer_objs:.o=.d)

-include $(explorer_deps)

#===== Patterns =====

$(BUILD)/%.o: %.cpp
	@echo Compiling $(notdir $(basename $@)).cpp
	@$(CXX) \
	$(CPPFLAGS) \
	-I $(explorer_src) \
	-I $(program_model_src) \
	-I $(scheduler_src) \
	-I $(utils_src) \
	$(CXXFLAGS) \
	-std=c++14 \
	-c -MMD $< -o $@

#===== Targets =====

explorer: $(explorer_objs)
	rm -rf $(explorer_file)
	$(LD) $(LDFLAGS) -lboost_system -lboost_filesystem -o $(BUILD)/$(explorer_file) $(explorer_objs)
	ln -s $(BUILD)/$(explorer_file) $(explorer_file)

install: pass scheduler explorer
