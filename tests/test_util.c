/* tests/test_scep.c */

#include <check.h>
#include <stdlib.h>
#include "scep.h"

SCEP *handle;

#define TEST_ERRMSG(ival, sval) \
	ck_assert_str_eq(scep_strerror(ival), sval)

#define TEST_CSR_1 "tests/test-files/test-1-csr.pem"
#define TEST_CSR_2 "tests/test-files/test-2-csr.pem"
#define TEST_B64_PKCS7_BIN "tests/test-files/util_b64_pkcs7.bin"
#define TEST_B64_PKCS7_PEM "tests/test-files/util_b64_pkcs7.pem"

char *test_new_key = "-----BEGIN RSA PRIVATE KEY-----\n"
"MIICXAIBAAKBgQCnCz5qi3kW8avPCPhmKOUwSRpCcqOi0RH3tGburtCoHl56nhL3\n"
"X1Xuv+3e6HWS74IOWbwuZXADdSWswFMefJuh6D4tRACzvgbOuXaxxopj9PYnieNu\n"
"nATNl1O1fy1QG3uJiy+QuQe3/xfIIwIVtvsx5ckMfRHk4g4lsOJwLofIvwIDAQAB\n"
"AoGAGt9dMCi11zITsJ/BzjWAAU+RUOU+W+AEYvP7pyQqXxFbo6AwbulAWsM3kieV\n"
"Woj7RDG9az1YUsYSxILAHGRxaMzpMtJISEECUqlmDYU+/vinU/vYp0a2oAuqFg4G\n"
"8nSoOQ2aTG5owNNcSrK7FbEcI2XdKZNNHM+82iYv7DA4tBECQQDYJLdeudpBhgiE\n"
"u6XaRfvlOeRWK7kfgIloz23qjfbgpDkVO40gIOMxUfU7ut19PuwJ5yJJG4mYCCbP\n"
"wR9Bu1snAkEAxdi7hfgj4Lkdh3C/Qki5K5Q7KR2K6Xhfzpn+fY4SmsLHd/v6QYhF\n"
"+igQv3Y357dz67+9dxWBzaMsMBFOM7QEqQJBAJadXzofADvQjncP246yXclqAfca\n"
"GLIe+6GRieJ8cqAvT6fAC6Nrx2VC20R3/oecJRbxfS68hbDvXTxAMuu3BtkCQGdP\n"
"q2xjjOiWAZNuDpFgREE7YEEyCg2sK+tIgpmxjIl/2IUQ8TczH8dnEIfKBZtcMo4S\n"
"S69ZbbSh1jsrbjiVcjECQAlyT5MO1eWxksYaW4aFx8w+QO9vxQh0vgkI1fBArbzt\n"
"sj4kcSMpE9Tn8CeAhi1d0Qwayo8QO1TPbIgay02syMo=\n"
"-----END RSA PRIVATE KEY-----";

char *test_new_csr = "-----BEGIN CERTIFICATE REQUEST-----\n"
"MIIBtTCCAR4CAQAwVzELMAkGA1UEBhMCQVUxEzARBgNVBAgTClNvbWUtU3RhdGUx\n"
"ITAfBgNVBAoTGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDEQMA4GA1UEAxMHZm9v\n"
"LmJhcjCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEApws+aot5FvGrzwj4Zijl\n"
"MEkaQnKjotER97Rm7q7QqB5eep4S919V7r/t3uh1ku+CDlm8LmVwA3UlrMBTHnyb\n"
"oeg+LUQAs74Gzrl2scaKY/T2J4njbpwEzZdTtX8tUBt7iYsvkLkHt/8XyCMCFbb7\n"
"MeXJDH0R5OIOJbDicC6HyL8CAwEAAaAeMBwGCSqGSIb3DQEJBzEPEw1GT09CQVJU\n"
"RVNUUFdEMA0GCSqGSIb3DQEBBQUAA4GBACHwu5U6KNAsgFkmgU6DNBQXriPwRvvn\n"
"uGCzClbjbwGnoi9XCtgepO6I6AbDokjpuuU8/JEGAqKwtRzOsvGJyq4tphAPf/89\n"
"/H+xoHva5tgIGv9zUQSj/6Q0B7TEUKLfVC4H0K9wde+5g13l82EzXXrsCjnyB3S7\n"
"SLYGjIEJ2RwX\n"
"-----END CERTIFICATE REQUEST-----";

char *issuedCert_str ="-----BEGIN CERTIFICATE-----\n"
"MIIB7TCCAZegAwIBAgIBBDANBgkqhkiG9w0BAQUFADBHMQswCQYDVQQGEwJERTEN\n"
"MAsGA1UECAwEYXNkZjENMAsGA1UEBwwEYXNkZjENMAsGA1UECgwEYXNkZjELMAkG\n"
"A1UEAwwCY2EwHhcNMTUwMzE1MTQyMzI1WhcNMTYwMzE0MTQyMzI1WjBXMQswCQYD\n"
"VQQGEwJBVTETMBEGA1UECBMKU29tZS1TdGF0ZTEhMB8GA1UEChMYSW50ZXJuZXQg\n"
"V2lkZ2l0cyBQdHkgTHRkMRAwDgYDVQQDEwdmb28uYmFyMIGfMA0GCSqGSIb3DQEB\n"
"AQUAA4GNADCBiQKBgQCnCz5qi3kW8avPCPhmKOUwSRpCcqOi0RH3tGburtCoHl56\n"
"nhL3X1Xuv+3e6HWS74IOWbwuZXADdSWswFMefJuh6D4tRACzvgbOuXaxxopj9PYn\n"
"ieNunATNl1O1fy1QG3uJiy+QuQe3/xfIIwIVtvsx5ckMfRHk4g4lsOJwLofIvwID\n"
"AQABoxowGDAJBgNVHRMEAjAAMAsGA1UdDwQEAwIF4DANBgkqhkiG9w0BAQUFAANB\n"
"AGZRYophSHisfLzjA0EV766X+e7hAK1J+G3IZHHn4WvxRGEGRZmEYMwbV3/gIRW8\n"
"bIEcl2LeuPgUGWhLIowjKF0=\n"
"-----END CERTIFICATE-----\n";


void setup()
{
	ck_assert(scep_init(&handle) == SCEPE_OK);
}

void teardown()
{
	ERR_print_errors_fp(stderr);
	scep_cleanup(handle);
}

START_TEST(test_scep_strerror)
{
	int i;
	for(i=SCEPE_OK; i < SCEPE_DUMMY_LAST_ERROR; i++)
		ck_assert_int_ne(strlen(scep_strerror(i)), 0);
	for(i=SCEPE_DUMMY_LAST_ERROR; i <= SCEPE_DUMMY_LAST_ERROR + 1; ++i)
		TEST_ERRMSG(i, "Unknown error");
}
END_TEST

START_TEST(test_scep_fail_info_str)
{
	int i;
	for(i=SCEP_BAD_ALG; i <= SCEP_BAD_CERT_ID; i++)
		ck_assert_int_ne(strlen(scep_fail_info_str(i)), 0);
	ck_assert_str_eq(scep_fail_info_str(5), "Unknown failInfo");
}
END_TEST

START_TEST(test_scep_calculate_transaction_id_pubkey)
{
	X509_REQ *req;
	FILE *fp;
	char *tid;
	SCEP_ERROR error;
	fp = fopen(TEST_CSR_1, "r");
	req = PEM_read_X509_REQ(fp, NULL, NULL, NULL);
	EVP_PKEY *pubkey = X509_REQ_get_pubkey(req);
	fclose(fp);
	X509_REQ_free(req);

	error = scep_calculate_transaction_id_pubkey(handle, pubkey, &tid);
	ck_assert(error == SCEPE_OK);
	ck_assert_str_eq(tid, "5418898A0D8052E60EB9E9F9BEB2E402F8138122C8503213CF5FD86DBB8267CF");
	free(tid);
	EVP_PKEY_free(pubkey);

	fp = fopen(TEST_CSR_2, "r");
	req = PEM_read_X509_REQ(fp, NULL, NULL, NULL);
	pubkey = X509_REQ_get_pubkey(req);
	fclose(fp);
	X509_REQ_free(req);

	error = scep_calculate_transaction_id_pubkey(handle, pubkey, &tid);
	ck_assert(error == SCEPE_OK);
	ck_assert_str_eq(tid, "569673452595B161A6F8D272D9A214152F828133994D5B166EFFB2C140A88EA2");
	free(tid);
	EVP_PKEY_free(pubkey);
}
END_TEST

START_TEST(test_scep_calculate_transaction_id_ias_type)
{
	char *tid1, *tid2, *tid3, *tid4;
	SCEP_ERROR error;

	BIO *b = BIO_new(BIO_s_mem());
	BIO_puts(b, issuedCert_str);
	X509 *cert = PEM_read_bio_X509(b, NULL, 0, 0);
	ck_assert(issuedCert_str != NULL);
	BIO_free(b);

	PKCS7_ISSUER_AND_SERIAL *ias = malloc(sizeof(PKCS7_ISSUER_AND_SERIAL));
	ias->serial = X509_get_serialNumber(cert);
	ias->issuer = X509_get_issuer_name(cert);

	error = scep_calculate_transaction_id_ias_type(handle, ias, "foo", &tid1);
	ck_assert(error == SCEPE_OK);
	ck_assert_str_eq(tid1, "AADB3ED997777E20F63B1C30999274F8C07A0849CA11633F7CBBDC8090CF8F5F");

	error = scep_calculate_transaction_id_ias_type(handle, ias, "foO", &tid2);
	ck_assert(error == SCEPE_OK);
	ck_assert_str_ne(tid1, tid2);

	ASN1_INTEGER *old_serial = ias->serial;
	ias->serial = ASN1_INTEGER_new();
	ASN1_INTEGER_set(ias->serial, 1337);

	error = scep_calculate_transaction_id_ias_type(handle, ias, "foo", &tid3);
	ck_assert(error == SCEPE_OK);
	ck_assert_str_ne(tid1, tid2);
	ck_assert_str_ne(tid2, tid3);

	ASN1_INTEGER_free(ias->serial);
	ias->serial = old_serial;

	X509_NAME_ENTRY *e = sk_X509_NAME_ENTRY_pop(ias->issuer->entries);
	X509_NAME_ENTRY_free(e);
	ias->issuer->modified = 1;

	error = scep_calculate_transaction_id_ias_type(handle, ias, "foo", &tid4);
	ck_assert(error == SCEPE_OK);
	ck_assert_str_ne(tid1, tid4);
	ck_assert_str_ne(tid2, tid4);
	ck_assert_str_ne(tid3, tid4);

	X509_free(cert);
	free(ias);
	free(tid1);
	free(tid2);
	free(tid3);
	free(tid4);
}
END_TEST

START_TEST(test_scep_PKCS7_base64_encode)
{
	BIO *inbio;
	PKCS7 *p7;
	char *out = NULL, *b64_pem;
	FILE *f;
	int f_size;

	inbio = BIO_new_file(TEST_B64_PKCS7_BIN, "rb");
	p7 = d2i_PKCS7_bio(inbio, NULL);
	BIO_free(inbio);

	f = fopen(TEST_B64_PKCS7_PEM, "rb");
	fseek(f, 0, SEEK_END);
	f_size = ftell(f);
	fseek(f, 0, SEEK_SET);
	b64_pem = malloc(f_size + 1);
	ck_assert(fread(b64_pem, 1, f_size, f) == f_size);
	b64_pem[f_size] = '\0';

	ck_assert(scep_PKCS7_base64_encode(handle, p7, &out) == SCEPE_OK);
	ck_assert_str_eq(out, b64_pem);
	free(b64_pem);
	free(out);
	PKCS7_free(p7);
	fclose(f);
}
END_TEST

START_TEST(test_scep_log)
{
	BIO *bio;
	char *log_str, *check_str;
	int lineno;
	size_t log_str_len;
	scep_conf_set(handle, SCEPCFG_VERBOSITY, DEBUG);

	bio = BIO_new(BIO_s_mem());
	scep_conf_set(handle, SCEPCFG_LOG, bio);
	// hack needed for log testing
	lineno = __LINE__; scep_log(handle, WARN, "This is a test");
	int ref_len = snprintf(NULL, 0, "test_util.c:%d: This is a test\n", lineno) + 1;
	check_str = malloc(ref_len);
	snprintf(check_str, ref_len, "test_util.c:%d: This is a test\n", lineno);
	log_str = malloc(ref_len);
	BIO_gets(bio, log_str, ref_len);
	ck_assert_str_eq(check_str, log_str);
	free(check_str);
	free(log_str);
	BIO_free(bio);
}
END_TEST

START_TEST(test_scep_new_selfsigned)
{
	X509_REQ *req;
	EVP_PKEY *req_key;
	BIO *data;
	X509 *cert;
	data = BIO_new(BIO_s_mem());
	BIO_puts(data, test_new_csr);
	req = PEM_read_bio_X509_REQ(data, NULL, 0, 0);
	ck_assert(req != NULL);
	BIO_free(data);

	data = BIO_new(BIO_s_mem());
	BIO_puts(data, test_new_key);
	req_key = PEM_read_bio_PrivateKey(data, NULL, 0, 0);
	ck_assert(req_key != NULL);
	BIO_free(data);

	ck_assert(scep_new_selfsigned_X509(handle, req, req_key, &cert) == SCEPE_OK);
	char *tmp = X509_NAME_oneline(X509_get_subject_name(cert), NULL, 0);
	ck_assert_str_eq(tmp, "/C=AU/ST=Some-State/O=Internet Widgits Pty Ltd/CN=foo.bar");
	free(tmp);
	tmp = X509_NAME_oneline(X509_get_issuer_name(cert), NULL, 0);
	ck_assert_str_eq(tmp, "/C=AU/ST=Some-State/O=Internet Widgits Pty Ltd/CN=foo.bar");
	free(tmp);
	tmp = i2s_ASN1_INTEGER(NULL, X509_get_serialNumber(cert));
	ck_assert_str_eq(tmp, "1");
	free(tmp);
	ck_assert_int_ne(X509_verify(cert, req_key), 0);
	EVP_PKEY_free(req_key);
	X509_free(cert);
	X509_REQ_free(req);
}
END_TEST

START_TEST(test_X509_REQ_cmp)
{
	X509_REQ *a, *b;
	BIO *data = BIO_new(BIO_s_mem());
	BIO_puts(data, test_new_csr);
	a = PEM_read_bio_X509_REQ(data, NULL, 0, 0);
	ck_assert(a != NULL);
	BIO_free(data);

	data = BIO_new(BIO_s_mem());
	BIO_puts(data, test_new_csr);
	b = PEM_read_bio_X509_REQ(data, NULL, 0, 0);
	ck_assert(b != NULL);
	BIO_free(data);

	ck_assert_int_eq(X509_REQ_cmp(a, b), 0);
	X509_REQ_free(a);
	X509_REQ_free(b);
}
END_TEST

Suite * scep_util_suite(void)
{
	Suite *s = suite_create("Util");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_checked_fixture(tc_core, setup, teardown);
	tcase_add_test(tc_core, test_scep_strerror);
	tcase_add_test(tc_core, test_scep_fail_info_str);
	tcase_add_test(tc_core, test_scep_calculate_transaction_id_pubkey);
	tcase_add_test(tc_core, test_scep_calculate_transaction_id_ias_type);
	tcase_add_test(tc_core, test_scep_PKCS7_base64_encode);
	tcase_add_test(tc_core, test_scep_log);
	tcase_add_test(tc_core, test_scep_new_selfsigned);
	tcase_add_test(tc_core, test_X509_REQ_cmp);

	suite_add_tcase(s, tc_core);

	return s;
}

int main(void)
{
	int number_failed;
	Suite *s = scep_util_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	srunner_set_fork_status(sr, CK_NOFORK);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
