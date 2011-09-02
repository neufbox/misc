#TGT = test_join_ip test_split_ip test_join_mac test_split_mac \
#	  test_join_hostname test_split_hostname \
#	  test_netmask test_validate_ip test_snprintf test_ctors_order \
#	  test_dmalloc_page_size test_elseif_evaluation_order \
#	  ewf_file_basename
#TEST_TGT = ewf_string_printf ewf_string_copy ewf_string_length ewf_string_compare ewf_log_write

TEST_TGT = test_version_check

EWF_OBJ = $(addprefix ../$(OBJDIR)/, $(CMN_WITHOUT_MAIN_OBJ))

######################################

all: $(TEST_TGT)

test_version_check: test_version_check.o $(EWF_OBJ)

clean:
	rm -f $(TEST_TGT) $(TEST_OBJ) *~

.PHONY: clean
