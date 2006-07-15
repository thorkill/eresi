
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: mjollnirUnitTest.c,v 1.4 2006-07-15 17:06:08 thor Exp $
 *
 */

#include <stdlib.h>
#include <check.h>
#include <libmjollnir.h>

START_TEST (test_create)
{
 char *infile = "ptrfnc";
 mjrSession sess;
 fail_unless(mjr_init_session(&sess) == 1,"mjrInitSession faild.");
 sess.obj = elfsh_map_obj(infile);
 fail_unless(sess.obj != NULL, "elfsh_map_obj faild.");
 fail_unless(mjr_setup_processor(&sess) == 1, "mjrSetupProcessor faild.");
// fail_unless(mjrFindCalls(&sess,".abstractSection") != NULL, "mjrFindCalls on abstract section returned without error.");
 fail_unless(mjr_find_calls(&sess,".text") == NULL, "mjrFindCalls on .text section faild.");
 fail_unless(mjr_analize(&sess,NULL) == 1, "mjrAnalize faild.");
}
END_TEST

Suite *mjollnir_suite(void)
{
 Suite *s = suite_create("Mjollnir");
 TCase *tc_core = tcase_create("Core");

 suite_add_tcase (s, tc_core);
 tcase_add_test(tc_core, test_create);
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

