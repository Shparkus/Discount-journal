#include <stdio.h>
#include <stdlib.h>

#include "../lib/journal.h"
#include "../lib/ui.h"

int main() {
    Journal j;
    if (journal_init(&j) != 0) {
        printf("Init error\n");
        return 1;
    }

    for (;;) {
        ui_print_menu();
        int cmd;
        if (ui_read_command(&cmd) != 0) break;

        switch (cmd) {
            // add
            case 1: {
                char shop[128], expiry[16];
                int discount = 0;
                if (ui_prompt_record(shop, sizeof(shop), &discount, expiry, sizeof(expiry)) == 0) {
                    int status = journal_add(&j, shop, discount, expiry);
                    ui_report_error(status);
                }
            } break;

            // delete
            case 2: {
                char shop[128];
                if (ui_prompt_shop(shop, sizeof(shop)) == 0) {
                    size_t n = journal_delete_by_shop(&j, shop);
                    ui_report_deleted(n);
                }
            } break;

            //show sorted
            case 3: ui_show_sorted(&j); break;
            
            // exit
            case 4: {
                journal_destroy(&j);
                return 0;
            }

            default:
                printf("Unknown command\n");
                break;
        }
    }
}