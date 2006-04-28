#include <stdlib.h>
#include <check.h>
#include "libmjollnir.h"

START_TEST (test_create)
{
 Mjr_CTX ctx;
 char *infile = "/bin/ls";
 fail_unless(mjr_init(&ctx) == 1, "Init of mjollnir context faild.");
 ctx.obj = elfsh_map_obj("/bin/lsls");
 fail_unless(ctx.obj == NULL, "elfsh_map_obj on wrong file faild.");
 ctx.obj = elfsh_map_obj(infile);
 fail_unless(ctx.obj != NULL, "elfsh_map_obj fail.");
}
END_TEST

START_TEST(test_navigation)
{
 MjrNaviMap map;
 MjrBlock *c,*p,*n;
 fail_unless(mjrNaviMapInit(&map) == 1,"map init faild.");
 c = mjrNaviCreateBlock(0x6666);
 p = mjrNaviCreateBlock(0x5555);
 n = mjrNaviCreateBlock(0x7777);
 fail_unless(c->vaddr == 0x6666,"Vaddr doesn't match.");
 p->next = 0x6666;
 c->next = 0x7777;
 c->ret = 0x5555;
 fail_unless(mjrNaviAddBlock(&map,p) == 1,"add block faild.");
 mjrNaviAddBlock(&map,c);
 mjrNaviAddBlock(&map,n);
 mjrNaviDump(&map,0x5555);
}
END_TEST

Suite *mjollnir_suite(void)
{
 Suite *s = suite_create("Mjollnir");
 TCase *tc_core = tcase_create("Core");

 suite_add_tcase (s, tc_core);
 tcase_add_test(tc_core, test_create);
 tcase_add_test(tc_core, test_navigation);

 return s;
}

int main(void)
{
 int nf;
 Suite *s = mjollnir_suite();
 SRunner *sr = srunner_create(s);
 srunner_run_all(sr, CK_NORMAL);
 nf = srunner_ntests_failed(sr);
 srunner_free(sr);
 return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

