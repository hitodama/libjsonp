#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "jsonp_options.h"

#include <jansson.h>

#include "jansson_extension.h"
#include "libjsonpp.h"

#include "libjsonp_helper.h"
#include "libjsonp_config.h"

json_foreach_iteration jsonp_required_scan(const char *key, json_t *val, void *mem)
{
	json_t *retobj = ((json_t **)mem)[0];
	
	if(json_is_string(val))
	{
		const char *v = json_string_value(val);
		if(v == NULL)
			return json_foreach_break;

		if(json_get(json_get(retobj, "arguments"), v) != NULL)
			return json_foreach_continue;
		if(json_get(json_get(retobj, "options"), v) != NULL)
			return json_foreach_continue;
		if(json_get(json_get(retobj, "switches"), v) != NULL)
			return json_foreach_continue;
	}

	return json_foreach_break;
}

json_foreach_iteration jsonp_arguments_scan(const char *key, json_t *val, void *mem)
{
	json_t *retobj = ((json_t **)mem)[0];
	json_t *args = ((json_t **)mem)[1];
	json_t *opts = ((json_t **)mem)[2];
	json_t *swit = ((json_t **)mem)[3];
	json_t **optnext = ((json_t ***)mem)[4];
	int *argidx = ((int **)mem)[5];
	json_foreach_iteration r = json_foreach_continue;

	if(json_is_string(val))
	{
		json_t *exists;
		size_t i;
		size_t l;

		const char *cc = json_string_value(val);
		if(cc == NULL)
			return json_foreach_break;
		l = strlen(cc);

		if(*optnext != NULL)
		{
			const char *c = json_string_value(*optnext);
			exists = json_get(opts, c);
			if(exists)
			{
				retobj = json_get(retobj, "options");
				json_set_new(retobj, json_string_value(*optnext), json_primitive(cc));
				json_decref(*optnext);
				*optnext = NULL;
			}
			else
			{
				if(cc[0] == '-')
				{
					while(*cc == '-' && *cc != '\0')
						++cc;
					retobj = json_get(retobj, "unnamed");
					retobj = json_get(retobj, "switches");
					json_set_new(retobj, json_string_value(*optnext), json_null());
					json_decref(*optnext);
					*optnext = json_string(cc);
				}
				else
				{
					retobj = json_get(retobj, "unnamed");
					retobj = json_get(retobj, "options");
					json_set_new(retobj, json_string_value(*optnext), json_primitive(cc));
					json_decref(*optnext);
					*optnext = NULL;
				}
			}
		}
		else if(strncmp(cc, "-", 1) == 0)
		{
			for(i = 1; cc[i] == '-' && cc[i] != '\0' && i <= l; ++i)
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
							json_t *newO = json_primitive(c + i);
							c[i] = '\0';
							json_set_new(retobj, c, newO);
						}
						break;
					}

					c[i] = t;
				}

				if(exists == NULL)
				{
					/*r = json_foreach_break;*/
					*optnext = json_string(c);
				}

				free((void *)c);
			}
		}
		else
		{
			args = json_array_get(args, *argidx);
			if(args != NULL)
			{
				retobj = json_get(retobj, "arguments");
				json_set_new(retobj, json_string_value(args), json_primitive(cc));
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

json_t *jsonp_options(json_t *args, json_t *opts)
{
	json_t *t = json_object();
	json_t *obj = json_object();
	json_t *oargs = json_get(opts, "arguments");
	json_t *oopts = json_invert(json_get(opts, "options"));
	json_t *oswit = json_invert(json_get(opts, "switches"));
	json_t *reqs = json_get(opts, "required");
	json_t *optnext = NULL;
	int argidx = 0;
	void *forargs[] = {obj, oargs, oopts, oswit, &optnext, &argidx};
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

	if(reqs != NULL && json_foreach(reqs, jsonp_required_scan, forargs) == json_foreach_break)
	{
		json_decref(obj);
		json_decref(oopts);
		json_decref(oswit);
		return NULL;
	}

	return obj;
}