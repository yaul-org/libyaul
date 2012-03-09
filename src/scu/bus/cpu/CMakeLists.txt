SET(scu_bus_cpu_SRC
  cpu_intc_lvl_get.S
  cpu_intc_lvl_set.S
  cpu_intc_vct_get.S
  cpu_intc_vct_set.S
  cpu_bc_bus_mode_get.S
)

SET(scu_bus_cpu_INSTALL_FILES
  cpu.h
  cpu/intc.h
)
