#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <jansson.h>

#include "libjsonpp.h"
#include "libjsonp.h"
#include "jansson_extension.h"
#include "jansson_extension_ugly.h"

/*
	Helpers
*/
void jsonp_passert2(json_t *t, json_t *t2)
{
	if(t == NULL || t2 == NULL)
		return;
	char *s = json_dumps(t, JSON_ENCODE_ANY | JSON_COMPACT | JSON_SORT_KEYS);
	char *s2 = json_dumps(t, JSON_ENCODE_ANY | JSON_COMPACT | JSON_SORT_KEYS);
	if(strcmp(s, s2) != 0)
		printf("%s != %s\n", s, s2);
	assert(strcmp(s, s2) == 0);
	free(s);
	free(s2);
}

void jsonp_passert(json_t *t, char *s2)
{
	if(t == NULL || s2 == NULL)
		return;
	char *s = NULL;
	if(json_is_object(t) || json_is_array(t))
		s = json_dumps(t, JSON_ENCODE_ANY | JSON_COMPACT | JSON_SORT_KEYS);
	else
		s = json_value_copy(t);
	if(strcmp(s, s2) != 0)
		printf("%s != %s\n", s, s2);
	assert(strcmp(s, s2) == 0);
	free(s);
}

void jsonp_print(json_t *t)
{
	if(t == NULL)
		return;
	char *s = json_dumps(t, JSON_ENCODE_ANY | JSON_COMPACT | JSON_SORT_KEYS);// | JSON_INDENT(4));
	if(s != NULL)
		printf("%s\n", s);
	free(s);
}

/*
	Tests
*/
void jansson_extension_h_merge_object_test()
{
	json_t *a = json_object();
	json_t *b = json_object();
	json_t *c = json_object();
	json_t *d = json_object();
	json_t *e = json_object();
	json_t *f = json_object();

	jsonpp_create(a, "/foo/%03bar", NULL);
	jsonpp_create(b, "/foo/%04baz", NULL);
	jsonpp_create(c, "/bar/%05baz", NULL);
	jsonpp_create(d, "/%01bar/-/beng", NULL);
	jsonpp_create(e, "/%01bar/baz", NULL);
	jsonpp_create(f, "/%01bar/%02-", NULL);
	jsonpp_create(f, "/%01bar/%03-", NULL);

	assert(json_merge(a, b) == 0);
	assert(json_merge(b, c) == 0);
	assert(json_merge(c, a) == 0);
	assert(json_merge(d, e) == 0);
	assert(json_merge(e, f) == 0);

	jsonp_passert(a, "{\"foo\":{\"baz\":0.0}}");
	jsonp_passert(b, "{\"bar\":{\"baz\":true},\"foo\":{\"baz\":0.0}}");
	jsonp_passert(c, "{\"bar\":{\"baz\":true},\"foo\":{\"baz\":0.0}}");
	jsonp_passert(d, "{\"bar\":[]}");
	jsonp_passert(e, "{\"bar\":[\"\",0]}");
}

void jansson_extension_h_merge_array_test()
{
	json_t *a = json_array();
	json_t *b = json_array();
	json_t *c = json_array();

	json_set(a, NULL, json_true());
	json_set(a, NULL, json_null());
	json_set(b, NULL, json_false());

	assert(json_merge(c, b) == 0);
	assert(json_merge(b, a) == 0);
	assert(json_merge(a, b) == 0);

	jsonp_passert(c, "[false]");
	jsonp_passert(b, "[false,true,null]");
	jsonp_passert(a, "[true,null,false,true,null]");
}

void jansson_extension_h_merge_string_test()
{
	json_t *a = json_string("abc");
	json_t *b = json_string("def");
	json_t *c = json_string("");

	assert(json_merge(c, b) == 0);
	assert(json_merge(a, b) == 0);
	assert(json_merge(a, c) == 0);

	jsonp_passert(a, "abcdefdef");
}

void jsonpp_stress_test()
{
	size_t i = 0;
	json_t *root1 = json_object();
	json_t *root2 = json_object();
	json_t *root3 = json_object();
	json_t *root4 = json_object();
	json_t *root5 = json_object();
	json_t *root6 = json_object();
	json_t *selected = NULL;

	const char *pointer1[] = { "/a~1b/fo%25%25/-/hh~0hh//0/ /a~0b", "/a~1b%2f%2f%2f/bar/1334//\\s/0",
	 "/a~1b/%01foo/-/-/joo", "/a~1b/foo/0/moo/goo/", "/usr/local/bin/bash",
	 "/%01/-/a", "/%02zap/67.4", "/%03zap/67.43", "\\foo%25%5f/zip/pi", "/a~1b/%01foo/-/uuu",
	 "/a~1b/%01foo/-/2uuu", "/foo%2fbar%2Fbaz%25", "/usr", "//asd", "/%00zap/68.43" };

	for(i = 0; i < 13; ++i)
		jsonpp_create(root1, pointer1[i], NULL);
	jsonpp_create(root1, pointer1[i++], json_null());
	jsonpp_create(root1, pointer1[i++], json_null());

	jsonp_passert(root1, "{\"\":[{\"a\":null}],\"\\\\foo%_/zip/pi\":null,\"a/b\":{\"fo%%\":{\"-\":{\"hh~hh\":{\"\":{\"0\":{\" \":{\"a~b\":null}}}}}},\"foo\":[{\"-\":{\"joo\":null},\"moo\":{\"goo\":{\"\":null}}},{\"uuu\":null},{\"2uuu\":null}]},\"a/b///\":{\"bar\":{\"1334\":{\"\":{\"\\\\s\":{\"0\":null}}}}},\"foo/bar/baz%\":null,\"usr\":null,\"zap\":{\"68.43\":null}}");
	//jsonp_print(root1);

	const char *pointer2[] = { "/a~1b/fo%25%25/-/hh~0hh//0/ /a~0b", "/a~1b%2f%2f%2f/bar/1334//\\s/0",
	 "/a~1b/%01foo~2/-/-/joo", "/a~1b/foo~2/0/moo/goo/", "/%01/-/a", "/usr/local/bin/bash" };
	
	for(i = 0; i < 6; ++i)
		jsonpp_create(root2, pointer2[i], json_null());

	jsonp_passert(root2, "{\"\":[{\"a\":null}],\"a/b\":{\"fo%%\":{\"-\":{\"hh~hh\":{\"\":{\"0\":{\" \":{\"a~b\":null}}}}}}},\"a/b///\":{\"bar\":{\"1334\":{\"\":{\"\\\\s\":{\"0\":null}}}}},\"usr\":{\"local\":{\"bin\":{\"bash\":null}}}}");
	//jsonp_print(root2);

	const char *pointer3[] = { "/%02zap", "/%03zap", "\\foo%25%5f/zip/pi", "/a~1b/%01foo~2/-/uuu",
	 "/a~1b/%01foo~2/-/2uuu", "/foo%2fbar%2Fbaz%25", "/usr", "//asd", "/zap/68.43" };

	for(i = 0; i < 9; ++i)
		jsonpp_create(root3, pointer3[i], json_null());

	jsonp_passert(root3, "{\"\":{\"asd\":null},\"\\\\foo%_/zip/pi\":null,\"a/b\":{},\"foo/bar/baz%\":null,\"usr\":null,\"zap\":0}");
	//jsonp_print(root3);

	const char *pointer4[] = { "/%01/-/a", "//-/b", "//foo" };

	for(i = 0; i < 2; ++i)
		jsonpp_create(root4, pointer4[i], json_null());
	jsonpp_create(root4, pointer4[i], json_null());

	jsonp_passert(root4, "{\"\":[{\"a\":null},{\"b\":null}]}");
	//jsonp_print(root4);

	const char *pointer5[] = { "//asd" , "/%01/-/a"};

	for(i = 0; i < 2; ++i)
		jsonpp_create(root5, pointer5[i], json_null());

	jsonp_passert(root5, "{\"\":[{\"a\":null}]}");
	//jsonp_print(root5);

	jsonpp_set(root6, "/zip1/zap/%01zue/-", json_false());
	jsonpp_set(root6, "/zap2", json_false());

	jsonpp_create(root6, "/%05zap", json_null());
	jsonpp_create(root6, "/man/maus", json_null());
	jsonpp_create(root6, "/%02man/3.1415", NULL);
	jsonpp_create(root6, "/up/%02musk", NULL);
	jsonpp_create(root6, "/1man/maus", json_null());
	jsonpp_create(root6, "/1man/maus/moo", NULL);

	jsonpp_create(root6, "/1", NULL);
	jsonpp_create(root6, "/1/2", NULL);
	jsonpp_create(root6, "/3/4", NULL);
	jsonpp_create(root6, "/3/5", NULL);
	jsonpp_create(root6, "/3/4/6", NULL);

	jsonpp_create(root6, "/a/1", json_null());
	jsonpp_create(root6, "/a/1/2", json_null());
	jsonpp_create(root6, "/a/3/4", json_null());
	jsonpp_create(root6, "/a/3/5", json_null());
	jsonpp_create(root6, "/a/3/4/6", json_null());

	jsonpp_create(root6, "/f/1", json_object());
	jsonpp_create(root6, "/f/1/2", json_object());
	jsonpp_create(root6, "/f/3/4", json_object());
	jsonpp_create(root6, "/f/3/5", json_object());
	jsonpp_create(root6, "/f/3/4/6", json_object());

	jsonpp_create(root6, "/g/1", json_array());
	jsonpp_create(root6, "/g/1/2", json_array());
	jsonpp_create(root6, "/g/3/4", json_array());
	jsonpp_create(root6, "/g/3/5", json_array());
	jsonpp_create(root6, "/g/3/4/6", json_array());

	jsonpp_create(root6, "/h/1", json_string(""));
	jsonpp_create(root6, "/h/1/2", json_integer(0));
	jsonpp_create(root6, "/h/3/4", json_real(0.0));
	jsonpp_create(root6, "/h/3/5", json_false());
	jsonpp_create(root6, "/h/3/4/6", json_true());

	jsonpp_create(root6, "/b/%021", NULL);
	jsonpp_create(root6, "/b/1/%022", NULL);
	jsonpp_create(root6, "/b/3/%024", NULL);
	jsonpp_create(root6, "/b/3/%025", NULL);
	jsonpp_create(root6, "/b/3/4/%026", NULL);

	jsonpp_create(root6, "/c/%021", NULL);
	jsonpp_create(root6, "/c/%001/%022", NULL);
	jsonpp_create(root6, "/c/3/%024", NULL);
	jsonpp_create(root6, "/c/3/%025", NULL);
	jsonpp_create(root6, "/c/3/%004/%026", NULL);

	jsonpp_create(root6, "/d/%001/-", NULL);
	jsonpp_create(root6, "/d/%011/0/a", NULL);
	jsonpp_create(root6, "/d/%011/-/2", NULL);
	jsonpp_create(root6, "/d/1/-/3", NULL);
	jsonpp_create(root6, "/d/3/%014/-", NULL);
	jsonpp_create(root6, "/d/3/%015/-", NULL);
	jsonpp_create(root6, "/d/3/4/%016/-", NULL);

	jsonpp_create(root6, "/e/%021/a", NULL);
	jsonpp_create(root6, "/e/%031/%012/b", NULL);
	jsonpp_create(root6, "/e/%043/4/c", NULL);
	jsonpp_create(root6, "/e/%053/5/d", NULL);
	jsonpp_create(root6, "/e/%063/4/6/e", NULL);

	jsonp_passert(root6, "{\"1\":{\"2\":null},\"1man\":{\"maus\":{\"moo\":null}},\"3\":{\"4\":{\"6\":null},\"5\":null},\"a\":{\"1\":{\"2\":null},\"3\":{\"4\":{\"6\":null},\"5\":null}},\"b\":{\"1\":\"\",\"3\":{\"4\":\"\",\"5\":\"\"}},\"c\":{\"1\":{\"2\":\"\"},\"3\":{\"4\":{\"6\":\"\"},\"5\":\"\"}},\"d\":{\"1\":[{\"2\":null},{\"3\":null}],\"3\":{\"4\":[null],\"5\":[null]}},\"e\":{\"1\":0,\"3\":false},\"f\":{\"1\":{\"2\":{}},\"3\":{\"4\":{\"6\":{}},\"5\":{}}},\"g\":{\"1\":[],\"3\":{\"4\":[],\"5\":[]}},\"h\":{\"1\":\"\",\"3\":{\"4\":0.0,\"5\":false}},\"man\":\"\",\"up\":{\"musk\":\"\"},\"zap\":true,\"zap2\":false}");
	//jsonp_print(root6);

	selected = jsonpp_get(root6, "/h");
	jsonpp_delete(selected, "/1");

	jsonp_passert(selected, "{\"3\":{\"4\":0.0,\"5\":false}}");
	//jsonp_print(selected);

	selected = jsonpp_get(root6, "/e/1");
	
	jsonp_passert(selected, "0");
	//jsonp_print(selected);

}

void jsonpp_rfc_example_test()
{
	char *json_str = "{\
		\"foo\": [\"bar\", \"baz\"], \"\": 0,\"a/b\": 1,\
		\"c%d\": 2, \"e^f\": 3, \"g|h\": 4, \"i\\\\j\": 5,\
		\"k\\\"l\": 6, \" \": 7, \"m~n\": 8\
	}";
	json_t *json = json_loads(json_str, 0, NULL);
	json_str = json_dumps(json, JSON_ENCODE_ANY | JSON_COMPACT | JSON_SORT_KEYS);

	jsonp_passert(jsonp_get(json, ""), json_str);
	jsonp_passert(jsonp_get(json, "/foo"), "[\"bar\",\"baz\"]");
	jsonp_passert(jsonp_get(json, "/foo/0"), "bar");
	jsonp_passert(jsonp_get(json, "/"), "0");
	jsonp_passert(jsonp_get(json, "/a~1b"), "1");
	jsonp_passert(jsonp_get(json, "/c%d"), "2");
	jsonp_passert(jsonp_get(json, "/e^f"), "3");
	jsonp_passert(jsonp_get(json, "/g|h"), "4");
	jsonp_passert(jsonp_get(json, "/i\\\\j"), "5");
	jsonp_passert(jsonp_get(json, "/k\\\"l"), "6");
	jsonp_passert(jsonp_get(json, "/ "), "7");
	jsonp_passert(jsonp_get(json, "/m~0n"), "8");


	jsonp_passert(jsonpp_get(json, ""), json_str);
	jsonp_passert(jsonpp_get(json, "/foo"), "[\"bar\",\"baz\"]");
	jsonp_passert(jsonpp_get(json, "/foo/0"), "bar");
	jsonp_passert(jsonpp_get(json, "/"), "0");
	jsonp_passert(jsonpp_get(json, "/a~1b"), "1");
	jsonp_passert(jsonpp_get(json, "/c%25d"), "2");
	jsonp_passert(jsonpp_get(json, "/e%5Ef"), "3");
	jsonp_passert(jsonpp_get(json, "/g%7Ch"), "4");
	jsonp_passert(jsonpp_get(json, "/i%5Cj"), "5");
	jsonp_passert(jsonpp_get(json, "/k%22l"), "6");
	jsonp_passert(jsonpp_get(json, "/%20"), "7");
	jsonp_passert(jsonpp_get(json, "/m~0n"), "8");
}

void jsonp_helper_h_test()
{

}

void jansson_extension_h_test()
{
	jansson_extension_h_merge_object_test();
	jansson_extension_h_merge_array_test();
	//jansson_extension_h_merge_string_test();
}

void jansson_extension_ugly_h_test()
{

}

void jsonp_h_test()
{

}

void jsonpp_h_test()
{

}

int main(int argc, char **argv)
{
	// jsonp_helper_h_test();
	jansson_extension_h_test();
	// jansson_extension_ugly_h_test();
	// jsonp_h_test();
	// jsonpp_h_test();
	/*ref count tests*/
	jsonpp_stress_test();
	jsonpp_rfc_example_test();
}