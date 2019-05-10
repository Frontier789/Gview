EXEC=gview
CXXFLAGS=-I include -std=c++11 -DGVIEW_ENABLE_TESTS
FPATH=Flib
CPP_FILES=src/*.cpp

ifeq ($(wildcard $(FPATH)/include/FRONTIER/Makefiles/fproject.mk),) 
  $(error Please specify valid FPATH (make FPATH=path/to/f) current FPATH is $(FPATH))
else 
  include $(FPATH)/include/FRONTIER/Makefiles/fproject.mk
$(FPATH)/$(F_LIB_DIR_NAME)/libf.a:
	-$(MAKE) --no-print-directory -C $(FPATH)/src -j4
clean_all: clean
	-$(MAKE) --no-print-directory -C $(FPATH)/src clean
endif
