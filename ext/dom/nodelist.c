/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if HAVE_LIBXML && HAVE_DOM
#include "php_dom.h"


/*
* class domnodelist 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-536297177
* Since: 
*/

zend_function_entry php_dom_nodelist_class_functions[] = {
	PHP_FALIAS(item, dom_nodelist_item, NULL)
	{NULL, NULL, NULL}
};

/* {{{ attribute protos, not implemented yet */

/* {{{ proto length	unsigned long	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-203510337
Since: 
*/
int dom_nodelist_length_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	dom_nnodemap_object *objmap;
	xmlNodePtr nodep, curnode;
	int count = 0;

	objmap = (dom_nnodemap_object *)obj->ptr;
	if (objmap->ht) {
		count = xmlHashSize(objmap->ht);
	} else {
		nodep = dom_object_get_node(objmap->baseobj);
		if (nodep) {
			if (objmap->nodetype == XML_ATTRIBUTE_NODE || objmap->nodetype == XML_ELEMENT_NODE) {
				curnode = nodep->children;
				if (curnode) {
					count++;
					while (curnode->next != NULL) {
						count++;
						curnode = curnode->next;
					}
				}
			} else {
				if (nodep->type == XML_DOCUMENT_NODE || nodep->type == XML_HTML_DOCUMENT_NODE) {
					nodep = xmlDocGetRootElement((xmlDoc *) nodep);
				}
				curnode = dom_get_elements_by_tag_name_ns_raw(nodep, objmap->ns, objmap->local, &count, -1);
			}
		}
	}

	MAKE_STD_ZVAL(*retval);
	ZVAL_LONG(*retval, count);
	return SUCCESS;
}

/* }}} */


/* {{{ proto domnode dom_nodelist_item(unsigned long index);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-844377136
Since: 
*/
PHP_FUNCTION(dom_nodelist_item)
{
	zval *id, *rv = NULL;
	int index, ret;
	dom_object *intern;
	xmlNodePtr itemnode = NULL;

	dom_nnodemap_object *objmap;
	xmlNodePtr nodep, curnode;
	int count = 0;

	id = getThis();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
		return;
	}

	if (index >= 0) {
		intern = (dom_object *)zend_object_store_get_object(id TSRMLS_CC);

		objmap = (dom_nnodemap_object *)intern->ptr;
		if (objmap->ht) {
			if (objmap->nodetype == XML_ENTITY_NODE) {
				itemnode = php_dom_libxml_hash_iter(objmap->ht, index);
			} else {
				itemnode = php_dom_libxml_notation_iter(objmap->ht, index);
			}
		} else {
			nodep = dom_object_get_node(objmap->baseobj);
			if (nodep) {
				if (objmap->nodetype == XML_ATTRIBUTE_NODE || objmap->nodetype == XML_ELEMENT_NODE) {
					curnode = nodep->children;
					while (count < index && curnode != NULL) {
						count++;
						curnode = curnode->next;
					}
					itemnode = curnode;
				} else {
					if (nodep->type == XML_DOCUMENT_NODE || nodep->type == XML_HTML_DOCUMENT_NODE) {
						nodep = xmlDocGetRootElement((xmlDoc *) nodep);
					}
					itemnode = dom_get_elements_by_tag_name_ns_raw(nodep, objmap->ns, objmap->local, &count, index);
				}
			}
		}
	} else {
		RETURN_NULL();
	}

	if (itemnode) {
		DOM_RET_OBJ(rv, itemnode, &ret, objmap->baseobj);
	} else {
		RETVAL_NULL();
	}
}
/* }}} end dom_nodelist_item */
#endif
