#pragma once
#include <stddef.h>
#include "journal.h"

void ui_print_menu();
void ui_report_error(int status);
void ui_report_deleted(size_t n);

int ui_read_command(int* out_cmd);
int ui_prompt_record(char* shop, size_t shop_cap, int* discount, char* expiry, size_t expiry_cap);
int ui_prompt_shop(char* shop, size_t cap);

void ui_show_sorted(const Journal* j);
