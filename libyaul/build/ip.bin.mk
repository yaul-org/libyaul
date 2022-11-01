ifeq ($(strip $(IP_VERSION)),)
  $(error Undefined IP_VERSION)
endif

ifeq ($(strip $(IP_RELEASE_DATE)),)
  $(error Undefined IP_RELEASE_DATE)
endif

ifeq ($(strip $(IP_AREAS)),)
  $(error Undefined IP_AREAS)
endif

ifeq ($(strip $(IP_PERIPHERALS)),)
  $(error Undefined IP_PERIPHERALS)
endif

ifeq ($(strip $(IP_TITLE)),)
  $(error Undefined IP_TITLE)
endif

ifeq ($(strip $(IP_MASTER_STACK_ADDR)),)
  $(error Undefined IP_MASTER_STACK_ADDR)
endif

ifeq ($(strip $(IP_SLAVE_STACK_ADDR)),)
  $(error Undefined IP_SLAVE_STACK_ADDR)
endif

ifeq ($(strip $(IP_1ST_READ_ADDR)),)
  $(error Undefined IP_1ST_READ_ADDR)
endif

ifeq ($(strip $(IP_1ST_READ_SIZE)),)
  $(error Undefined IP_1ST_READ_SIZE)
endif

SH_DEFSYMS+= \
	-Wl,--defsym=___master_stack=$(IP_MASTER_STACK_ADDR) \
	-Wl,--defsym=___slave_stack=$(IP_SLAVE_STACK_ADDR)
