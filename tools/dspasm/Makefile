TARGET:= dspasm
# http://gcc.gnu.org/onlinedocs/gcc-4.5.2/gcc/Warning-Options.html#Warning-Options
CXXFLAGS:= -ggdb \
	-Wall \
	-Wextra \
	-Wuninitialized \
	-Winit-self \
	-Wuninitialized \
	-Wshadow \
	-Wno-unused \
	-Wno-parentheses \
	-O2
LDFLAGS:=
L:=
L_DIR:= lib
S_DIR:= src
I:= include
STORE:= .

# Use absolute or relative paths
FLEX:= flex
BISON:= bison
SED:= sed

# Duplicates are taken care of when using the "$^" automatic variable
S:= $(S_DIR)/lexer.cc \
	$(S_DIR)/parser.cc \
	$(S_DIR)/driver.cc \
	$(S_DIR)/dspasm.cc

O:= $(addprefix $(STORE)/,$(S:.cc=.o))
D:= $(addprefix $(STORE)/,$(S:.cc=.dd))

.PHONY: clean

all: $(TARGET)

$(TARGET): $(O)
	$(CXX) -o $(TARGET) $^ $(foreach LIBRARY,$(L),-l$(LIBRARY)) \
		$(foreach LIBRARY_DIR,$(L_DIR),-L$(LIBRARY_DIR)) \
		$(LDFLAGS)

$(STORE)/%.cc: %.ll
	$(FLEX) -+ -o $@ $<

# FLEX(1) is a prerequisite of BISON(1)
$(STORE)/%.cc: %.yy $(S_DIR)/lexer.cc
	$(BISON) -v -o $@ $<

$(STORE)/%.o: %.cc $(S_DIR)/parser.cc
	$(CXX) -Wp,-MMD,$(STORE)/$*.dd $(CXXFLAGS) \
	$(foreach INCLUDE_DIR,$(I),-I$(INCLUDE_DIR)) -c $< -o $@
	$(SED) -e '1s/^\(.*\)$$/$(subst /,\/,$(dir $@))\1/' $(STORE)/$*.dd \
		> $(STORE)/$*.dd
	rm $(STORE)/$*.dd

clean:
	-rm -f $(foreach DIR,$(S_DIR),\
		$(STORE)/$(DIR)/*.dd \
		$(STORE)/$(DIR)/*.o) \
		$(TARGET) \
		$(S_DIR)/parser.output \
		$(S_DIR)/parser.cc \
		$(S_DIR)/parser.hh \
		$(S_DIR)/location.hh \
		$(S_DIR)/stack.hh \
		$(S_DIR)/position.hh \
		$(S_DIR)/lexer.cc

-include $(D)
