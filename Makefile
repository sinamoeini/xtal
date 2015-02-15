SHELL 	    = /bin/bash
SRC         = src/
PROGRAMS    = main
CC          = g++ 
OBJ         = obj/
MAKEFILE    = Makefile
CFLAGS      = -O3
LIBS        = -lreadline 
INCLUDES    = 
        
CPP_FILES   = $(wildcard $(SRC)*.cpp)
H_FILES     = $(wildcard $(SRC)*.h)
OBJ_FILES   = $(addprefix $(OBJ),$(notdir $(CPP_FILES:.cpp=.o))) 


$(OBJ)%.o: $(SRC)%.cpp $(MAKEFILE)
	$(CC) -c $(CFLAGS) -o $@ $(INCLUDES) $<

xtal:	prep $(OBJ_FILES) $(MAKEFILE) 
	$(CC) $(CFLAGS) $(OBJ_FILES) $(LIBS) -o $@ $(INCLUDES)

clean:  
	rm -rf $(OBJ)
prep:
	@mkdir -p $(OBJ); \
	cd src; \
	rm -rf command_styles.h; \
	for i in `ls command_*.h`; do \
	echo \#include \"$$i\" >>command_styles.h; \
	done; \
	cd ..
