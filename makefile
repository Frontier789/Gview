EXEC=gview
CXXFLAGS=-I include -std=c++11
FPATH=../Flib
CPP_FILES=src/*.cpp

ifeq ($(wildcard $(FPATH)/include/FRONTIER/Makefiles/fproject.mk),) 
  $(error FPATH is invalid $(FPATH))
else 
  include $(FPATH)/include/FRONTIER/Makefiles/fproject.mk
endif 

