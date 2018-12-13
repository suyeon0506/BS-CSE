#pragma once

#define INPUT_FILE_NAME_0	"32_random.binary"
#define INPUT_FILE_NAME_1	"1024_random.binary"
#define INPUT_FILE_NAME_2   "16384_random.binary"
#define INPUT_FILE_NAME_3   "131072_random.binary"
#define INPUT_FILE_NAME_4	"1048576_random.binary"

#define SORTED_INPUT_FILE_NAME_0	"32_random.binary"
#define SORTED_INPUT_FILE_NAME_1	"1024_random.binary"
#define SORTED_INPUT_FILE_NAME_2    "16384_random.binary"
#define SORTED_INPUT_FILE_NAME_3    "131072_random.binary"
#define SORTED_INPUT_FILE_NAME_4	"1048576_random.binary"

int test_cases_sorting = 5;
char test_data_file_sorting[5][128] = { INPUT_FILE_NAME_0, INPUT_FILE_NAME_1, INPUT_FILE_NAME_2, INPUT_FILE_NAME_3, INPUT_FILE_NAME_4 };
char test_data_file_sorted[5][128] = { SORTED_INPUT_FILE_NAME_0, SORTED_INPUT_FILE_NAME_1, SORTED_INPUT_FILE_NAME_2, SORTED_INPUT_FILE_NAME_3, SORTED_INPUT_FILE_NAME_4 };

int test_cases_selection = 5;
char test_data_file_selection[5][128] = { INPUT_FILE_NAME_0, INPUT_FILE_NAME_1, INPUT_FILE_NAME_2, INPUT_FILE_NAME_3, INPUT_FILE_NAME_4 };
int test_data_k_selection[3] = { 13, 483, 44493 };			// k's for selection
int test_data_indices_selected[3] = { 15, 132, 575038 };	// indices of selected elements
