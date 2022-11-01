# The targets which .SECONDARY depends on are treated as intermediate files,
# except that they are never automatically deleted
.SECONDARY: pre-build-iso post-build-iso

pre-build-iso:

post-build-iso:

include $(YAUL_INSTALL_ROOT)/share/ip.bin.mk

IMAGE_DIRECTORY?= cd
IMAGE_1ST_READ_BIN?= A.BIN

OUTPUT_FILES= $(SH_PROGRAM).iso $(SH_PROGRAM).cue
CLEAN_OUTPUT_FILES= $(OUTPUT_FILES) $(SH_BUILD_PATH)/IP.BIN $(SH_BUILD_PATH)/IP.BIN.map

include $(YAUL_INSTALL_ROOT)/share/build.post.bin.mk

$(SH_PROGRAM).iso: $(SH_BUILD_PATH)/$(SH_PROGRAM).bin $(SH_BUILD_PATH)/IP.BIN
	@printf -- "$(V_BEGIN_YELLOW)$@$(V_END)\n"
    # This is a rather nasty hack to suppress any output from running the
    # pre/post-build-iso targets
	$(ECHO)$(MAKE) --no-print-directory $$([ -z "$(SILENT)" ] || printf -- "-s") -f $(THIS_FILE) pre-build-iso
	$(ECHO)mkdir -p $(IMAGE_DIRECTORY)
	$(ECHO)cp $(SH_BUILD_PATH)/$(SH_PROGRAM).bin $(IMAGE_DIRECTORY)/$(IMAGE_1ST_READ_BIN)
	$(ECHO)for txt in "ABS.TXT" "BIB.TXT" "CPY.TXT"; do \
	    if ! [ -s $(IMAGE_DIRECTORY)/$$txt ]; then \
		printf -- "empty\n" > $(IMAGE_DIRECTORY)/$$txt; \
	    fi \
	done
	$(ECHO)$(YAUL_INSTALL_ROOT)/share/wrap-error $(YAUL_INSTALL_ROOT)/bin/make-iso $(IMAGE_DIRECTORY) $(SH_BUILD_PATH)/IP.BIN $(SH_PROGRAM)
	$(ECHO)$(MAKE) --no-print-directory $$([ -z "$(SILENT)" ] || printf -- "-s") -f $(THIS_FILE) post-build-iso

$(SH_PROGRAM).cue: | $(SH_PROGRAM).iso
	@printf -- "$(V_BEGIN_YELLOW)$@$(V_END)\n"
	$(ECHO)$(YAUL_INSTALL_ROOT)/share/wrap-error $(YAUL_INSTALL_ROOT)/bin/make-cue "$(SH_PROGRAM).iso"

$(SH_BUILD_PATH)/IP.BIN: $(YAUL_INSTALL_ROOT)/share/yaul/ip/ip.sx $(SH_BUILD_PATH)/$(SH_PROGRAM).bin
	$(ECHO)$(YAUL_INSTALL_ROOT)/share/wrap-error $(YAUL_INSTALL_ROOT)/bin/make-ip \
	    "$(SH_BUILD_PATH)/$(SH_PROGRAM).bin" \
		"$(IP_VERSION)" \
		$(IP_RELEASE_DATE) \
		"$(IP_AREAS)" \
		"$(IP_PERIPHERALS)" \
		'"$(IP_TITLE)"' \
		$(IP_MASTER_STACK_ADDR) \
		$(IP_SLAVE_STACK_ADDR) \
		$(IP_1ST_READ_ADDR) \
	    $(IP_1ST_READ_SIZE)
