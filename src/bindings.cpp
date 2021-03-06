
#ifdef __AVX2__
#include "bindings.h"


bool simdjsonphp::isValid(std::string_view p) {
  ParsedJson pj = build_parsed_json(p);
  return pj.isValid();
}

bool cplus_isValid(const char *json){
    return simdjsonphp::isValid(json);
}

void simdjsonphp::parse(std::string_view p, zval* return_value) {
  ParsedJson pj = build_parsed_json(p);
  if (!pj.isValid()) {
      return;
  }
  ParsedJson::iterator pjh(pj);
  *return_value = simdjsonphp::makeArray(pjh);
}

void cplus_parse(const char *json, zval* return_value){
  simdjsonphp::parse(json, return_value);
}

zval simdjsonphp::makeArray(ParsedJson::iterator & pjh) {
  zval v;
  if (pjh.is_object()) {
    zval obj;
    array_init(&obj);
    if (pjh.down()) {
      const char* key = pjh.get_string();
      pjh.next();
      zval value = simdjsonphp::makeArray(pjh);
      add_assoc_zval(&obj, key, &value);
      while (pjh.next()) {
        key = pjh.get_string();
        pjh.next();
        zval value = simdjsonphp::makeArray(pjh);
        add_assoc_zval(&obj, key, &value);
      }
      pjh.up();
    }
    v = obj;
  } else if (pjh.is_array()) {
    zval arr;
    array_init(&arr);
    if (pjh.down()) {
      zval value = simdjsonphp::makeArray(pjh);
      add_next_index_zval(&arr, &value);
      while (pjh.next()) {
         zval value = simdjsonphp::makeArray(pjh);
         add_next_index_zval(&arr, &value);
      }
      pjh.up();
    }
    v = arr;
  } else if (pjh.is_string()) {
    ZVAL_STRING(&v, pjh.get_string());
  } else if (pjh.is_double()) {
    ZVAL_DOUBLE(&v, pjh.get_double());
  } else if (pjh.is_integer()) {
    ZVAL_LONG(&v, pjh.get_integer());
  } else {
    switch (pjh.get_type()) {
      case 't':  {
        ZVAL_TRUE(&v);
        break;
      }
      case 'f': {
        ZVAL_FALSE(&v);
        break;
      }
      case 'n': {
        ZVAL_NULL(&v);
        break;
      }
      default : break;
    }
  }

  return v;
}

#endif