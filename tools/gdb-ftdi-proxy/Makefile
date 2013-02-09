TARGET:= gdb-ftdi-proxy
# http://.c.gnu.org/onlinedocs/.c-4.5.2/.c/Warning-Options.html#Warning-Options
CCFLAGS:= -O2 \
	-Wall \
	-Wextra \
	-Wuninitialized \
	-Winit-self \
	-Wuninitialized \
	-Wshadow \
	-Wno-unused \
	-Wno-parentheses
LDFLAGS:=
L:= ftdi
S_DIR:= src
STORE:= .

SED:= sed

# Duplicates are taken care of when using the "$^" automatic variable
S:= $(S_DIR)/gdb-ftdi-proxy.c

O:= $(addprefix $(STORE)/,$(S:.c=.o))
D:= $(addprefix $(STORE)/,$(S:.c=.d))

.PHONY: clean

all: $(TARGET)

$(TARGET): $(O)
	@echo $(O)
	$(CC) -o $(TARGET) $^ $(foreach LIBRARY,$(L),-l$(LIBRARY)) $(LDFLAGS)

$(STORE)/%.o: %.c
	$(CC) -Wp,-MMD,$(STORE)/$*.d $(CCFLAGS) -c -o $@ $<
	$(SED) -e '1s/^\(.*\)$$/$(subst /,\/,$(dir $@))\1/' $(STORE)/$*.d \
		> $(STORE)/$*.d
	rm $(STORE)/$*.d

clean:
	-rm -f $(foreach DIR,$(S_DIR),\
		$(STORE)/$(DIR)/*.d \
		$(STORE)/$(DIR)/*.o) \
		$(TARGET)

-include $(D)
