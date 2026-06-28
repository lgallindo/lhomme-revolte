/*
  JSON parse helpers based on the Relic project implementation.
  Source acknowledgement:
  https://github.com/felixrieseberg/relic (src/core/jsonp.h, jsonp.c)
*/
#ifndef GIF2MAP_JSONP_H
#define GIF2MAP_JSONP_H

#define JSMN_HEADER
#include "third_party/jsmn.h"

int jsonp_skip(const jsmntok_t *toks, int i);
int jsonp_child(const jsmntok_t *toks, int obj, const char *js, const char *k);
int jsonp_unescape_span(const char *s, int n, char *dst, int cap);
int jsonp_unescape(const char *js, const jsmntok_t *t, char *dst, int cap);

#endif
