PWD:= $(notdir $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST))))))

TARGET:= bin2c
# http://.c.gnu.org/onlinedocs/.c-4.5.2/.c/Warning-Options.html#Warning-Options
CCFLAGS:= -O2 \
	-s \
	-pipe \
	-Wall \
	-Wextra \
	-Wuninitialized \
	-Winit-self \
	-Wuninitialized \
	-Wshadow \
	-Wno-unused \
	-Wno-parentheses
LDFLAGS:=
INCLUDE_DIRS:=
LIBRARY_DIRS:=
LIBRARIES:=
SRC_DIR:= .
STORE_FILES:= .

SED:= sed

# Duplicates are taken care of when using the "$^" automatic variable
SRCS:= $(SRC_DIR)/bin2c.c

OBJS:= $(addprefix $(STORE_FILES)/,$(SRCS:.c=.o))
DEPS:= $(addprefix $(STORE_FILES)/,$(SRCS:.c=.d))

.PHONY: clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $^ \
		$(foreach LIBRARY_DIR,$(LIBRARY_DIRS),-L$(LIBRARY_DIR)) \
		$(foreach LIBRARY,$(LIBRARIES),-l$(LIBRARY)) \
		$(LDFLAGS)

$(STORE_FILES)/%.o: %.c
	$(CC) -Wp,-MMD,$(STORE_FILES)/$*.d $(CCFLAGS) -c -o $@ $< \
		$(foreach INCLUDE_DIR,$(INCLUDE_DIRS),-I$(INCLUDE_DIR))
	$(SED) -e '1s/^\(.*\)$$/$(subst /,\/,$(dir $@))\1/' $(STORE_FILES)/$*.d \
		> $(STORE_FILES)/$*.d
	rm $(STORE_FILES)/$*.d

clean:
	-rm -f $(foreach DIR,$(SRC_DIR),\
		$(STORE_FILES)/$(DIR)/*.d \
		$(STORE_FILES)/$(DIR)/*.o) \
		$(TARGET)

-include $(DEPS)
