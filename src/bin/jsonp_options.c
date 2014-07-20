#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "jsonp_options.h"

#include <jansson.h>

#include "jansson_extension.h"
#include "jansson_extension_ugly.h"
#include "libjsonp_helper.h"

json_foreach_iteration jsonp_arguments_scan(const char *key, json_t *val, void *mem)
{
	json_t *retobj = ((json_t **)mem)[0];
	json_t *args = ((json_t **)mem)[1];
	json_t *opts = ((json_t **)mem)[2];
	json_t *swit = ((json_t **)mem)[3];
	json_t **optnext = ((json_t ***)mem)[4];
	int *argidx = ((int **)mem)[5];
	char prefix = *((char **)mem)[6];
	json_foreach_iteration r = json_foreach_continue;

	if(json_is_string(val))
	{
		json_t *exists;
		size_t i;

		const char *cc = json_string_value(val);
		if(cc == NULL)
			return json_foreach_break;

		if(*optnext != NULL)
		{
			const char *c = json_string_value(*optnext);
			exists = json_get(opts, c);
			if(exists)
			{
				retobj = json_get(retobj, "options");
				json_set_new(retobj, json_string_value(*optnext), json_ofvalue(cc));
				json_decref(*optnext);
				*optnext = NULL;
			}
			else
			{
				retobj = json_get(retobj, "unnamed");
				retobj = json_get(retobj, "switches");
				if(cc[0] == prefix)
				{
					while(*cc == prefix && *cc != '\0')
						++cc;
					json_set_new(retobj, json_string_value(*optnext), json_null());
					*optnext = json_string(cc);
				}
				else
				{
					json_set_new(retobj, json_string_value(*optnext), json_ofvalue(cc));
					*optnext = NULL;
				}
				json_decref(*optnext);
			}
		}
		else if(cc[0] == prefix)
		{
			size_t l = strlen(cc);
			for(i = 1; cc[i] == prefix && cc[i] != '\0' && i <= l; ++i)
				;
			l -= i;

			exists = json_get(swit, cc + i);
			if(exists != NULL)// && strlen(cc + i) == 0)
			{
				retobj = json_get(retobj, "switches");
				json_set_new(retobj, cc + i, json_null());
			}
			else
			{
				retobj = json_get(retobj, "options");
				char *c = jsonp_strndup(cc + i, l);
		
				for(i = l; i > 0; --i)
				{
					char t = c[i];
					c[i] = '\0';

					exists = json_get(opts, c);
					if(exists != NULL)
					{
						c[i] = t;
						if(strlen(c + i) == 0)
						{
							c[i] = '\0';
							*optnext = json_string(c);
						}
						else
						{
							json_t *newO = json_ofvalue(c + i);
							c[i] = '\0';
							json_set_new(retobj, c, newO);
						}
						break;
					}

					c[i] = t;
				}

				if(exists == NULL)
					*optnext = json_string(c);

				free((void *)c);
			}
		}
		else
		{
			args = json_array_get(args, *argidx);
			if(args != NULL)
			{
				retobj = json_get(retobj, "arguments");
				json_set_new(retobj, json_string_value(args), json_ofvalue(cc));
			}
			else
			{
				retobj = json_get(retobj, "unnamed");
				retobj = json_get(retobj, "arguments");
				json_set_new(retobj, NULL, json_string(cc));
			}
			++*argidx;
		}
	}

	return r;
}

json_t *jsonp_options(json_t *args, json_t *opts, char prefix)
{
	json_t *t = json_object();
	json_t *obj = json_object();
	json_t *oargs = json_get(opts, "arguments");
	json_t *oopts = json_invert(json_get(opts, "options"));
	json_t *oswit = json_invert(json_get(opts, "switches"));
	json_t *optnext = NULL;
	int argidx = 0;
	void *forargs[] = {obj, oargs, oopts, oswit, &optnext, &argidx, &prefix};

	json_set_new(obj, "arguments", json_object());
	json_set_new(obj, "options", json_object());
	json_set_new(obj, "switches", json_object());

	json_set_new(obj, "unnamed", t);
	json_set_new(t, "arguments", json_array());
	json_set_new(t, "options", json_object());
	json_set_new(t, "switches", json_object());

	if(json_foreach(args, jsonp_arguments_scan, forargs) == json_foreach_break)
	{
		json_decref(obj);
		json_decref(oopts);
		json_decref(oswit);
		return NULL;
	}

	return obj;
}
