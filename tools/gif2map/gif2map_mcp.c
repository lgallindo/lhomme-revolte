#include "gif2map.h"
#include "jsonp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_CAP 65536
#define TOK_CAP 256

static int json_get_raw_id(const char *json, const jsmntok_t *tokens,
  int root, char *out, size_t out_cap)
{
  int idToken = jsonp_child(tokens, root, json, "id");
  size_t len;

  if (idToken < 0 || out_cap == 0)
    return 0;

  len = (size_t)(tokens[idToken].end - tokens[idToken].start);

  if (len + 1 > out_cap)
    return 0;

  memcpy(out, json + tokens[idToken].start, len);
  out[len] = '\0';

  if (tokens[idToken].type == JSMN_STRING)
  {
    memmove(out + 1, out, len);
    out[0] = '"';
    out[len + 1] = '"';
    out[len + 2] = '\0';
  }

  return 1;
}

static int json_get_string(const char *json, const jsmntok_t *tokens,
  int objToken, const char *key, char *out, size_t out_cap)
{
  int valueToken;

  if (objToken < 0 || out_cap == 0)
    return 0;

  valueToken = jsonp_child(tokens, objToken, json, key);

  if (valueToken < 0)
    return 0;

  if (jsonp_unescape(json, &tokens[valueToken], out, (int)out_cap) < 0)
    return 0;

  return 1;
}

static void json_write_escaped(FILE *out, const char *s)
{
  fputc('"', out);

  while (*s != '\0')
  {
    unsigned char c = (unsigned char)*s++;

    if (c == '"' || c == '\\')
      fprintf(out, "\\%c", c);
    else if (c == '\n')
      fputs("\\n", out);
    else if (c == '\r')
      fputs("\\r", out);
    else if (c == '\t')
      fputs("\\t", out);
    else if (c < 32)
      fprintf(out, "\\u%04x", c);
    else
      fputc(c, out);
  }

  fputc('"', out);
}

static char *read_stream(FILE *f)
{
  char *buf = NULL;
  size_t size = 0;
  size_t cap = 0;
  char chunk[4096];
  size_t n;

  while ((n = fread(chunk, 1, sizeof(chunk), f)) > 0)
  {
    char *next;

    if (size + n + 1 > cap)
    {
      size_t next_cap = cap == 0 ? 8192 : cap * 2;

      while (next_cap < size + n + 1)
        next_cap *= 2;

      next = (char *)realloc(buf, next_cap);

      if (next == NULL)
      {
        free(buf);
        return NULL;
      }

      buf = next;
      cap = next_cap;
    }

    memcpy(buf + size, chunk, n);
    size += n;
  }

  if (buf == NULL)
  {
    buf = (char *)malloc(1);

    if (buf == NULL)
      return NULL;
  }

  buf[size] = '\0';
  return buf;
}

static char *convert_to_string(const char *input_path, const char *format,
  const char *symbol_name, Gif2MapResult *out_result)
{
  Gif2MapLevel level;
  FILE *tmp;
  char *text;

  *out_result = gif2map_load_gif(input_path, &level);

  if (!out_result->ok)
    return NULL;

  tmp = tmpfile();

  if (tmp == NULL)
  {
    *out_result = (Gif2MapResult){0, 1, "could not create temporary output"};
    return NULL;
  }

  if (format != NULL && strcmp(format, "json") == 0)
    *out_result = gif2map_emit_json(tmp, &level);
  else
    *out_result = gif2map_emit_c(tmp, &level, symbol_name);

  if (!out_result->ok)
  {
    fclose(tmp);
    return NULL;
  }

  rewind(tmp);
  text = read_stream(tmp);
  fclose(tmp);

  if (text == NULL)
    *out_result = (Gif2MapResult){0, 1, "could not read temporary output"};

  return text;
}

static void respond_error(const char *id, int code, const char *message)
{
  printf("{\"jsonrpc\":\"2.0\",\"id\":%s,\"error\":{\"code\":%d,\"message\":",
    id, code);
  json_write_escaped(stdout, message);
  printf("}}\n");
  fflush(stdout);
}

static void respond_initialize(const char *id)
{
  printf("{\"jsonrpc\":\"2.0\",\"id\":%s,\"result\":{"
    "\"protocolVersion\":\"2025-06-18\","
    "\"serverInfo\":{\"name\":\"gif2map\",\"version\":\"0.1.0\"},"
    "\"capabilities\":{\"tools\":{}}}}\n", id);
  fflush(stdout);
}

static void respond_tools_list(const char *id)
{
  printf("{\"jsonrpc\":\"2.0\",\"id\":%s,\"result\":{\"tools\":[{"
    "\"name\":\"gif2map_convert\","
    "\"description\":\"Convert a 139x139 indexed GIF map to SFG level data.\","
    "\"inputSchema\":{\"type\":\"object\",\"properties\":{"
    "\"input_path\":{\"type\":\"string\"},"
    "\"format\":{\"type\":\"string\",\"enum\":[\"c\",\"json\"]},"
    "\"symbol_name\":{\"type\":\"string\"}},"
    "\"required\":[\"input_path\"]}}]}}\n", id);
  fflush(stdout);
}

static void respond_tools_call(const char *id, const char *line,
  const jsmntok_t *tokens, int root)
{
  int paramsToken = jsonp_child(tokens, root, line, "params");
  int argsToken;
  char name[128];
  char input_path[1024];
  char format[32];
  char symbol_name[128];
  Gif2MapResult result;
  char *text;

  argsToken = jsonp_child(tokens, paramsToken, line, "arguments");

  if (argsToken < 0)
    argsToken = paramsToken;

  if (!json_get_string(line, tokens, paramsToken, "name", name, sizeof(name)) ||
    strcmp(name, "gif2map_convert") != 0)
  {
    respond_error(id, -32602, "unknown tool name");
    return;
  }

  if (!json_get_string(line, tokens, argsToken, "input_path", input_path,
    sizeof(input_path)))
  {
    respond_error(id, -32602, "input_path is required");
    return;
  }

  if (!json_get_string(line, tokens, argsToken, "format", format,
    sizeof(format)))
    strcpy(format, "c");

  if (!json_get_string(line, tokens, argsToken, "symbol_name", symbol_name,
    sizeof(symbol_name)))
    symbol_name[0] = '\0';

  text = convert_to_string(input_path, format,
    symbol_name[0] != '\0' ? symbol_name : NULL, &result);

  if (!result.ok)
  {
    respond_error(id, -32000, result.error_message);
    return;
  }

  printf("{\"jsonrpc\":\"2.0\",\"id\":%s,\"result\":{\"content\":[{"
    "\"type\":\"text\",\"text\":", id);
  json_write_escaped(stdout, text);
  printf("}]}}\n");
  fflush(stdout);
  free(text);
}

static void handle_line(const char *line)
{
  jsmn_parser parser;
  jsmntok_t tokens[TOK_CAP];
  int tokenCount;
  int root = 0;
  char id[128];
  char method[128];

  jsmn_init(&parser);

  tokenCount = jsmn_parse(&parser, line, strlen(line), tokens, TOK_CAP);

  if (tokenCount < 1 || tokens[root].type != JSMN_OBJECT)
    return;

  if (!json_get_raw_id(line, tokens, root, id, sizeof(id)))
    return;

  if (!json_get_string(line, tokens, root, "method", method, sizeof(method)))
  {
    respond_error(id, -32600, "method is required");
    return;
  }

  if (strcmp(method, "initialize") == 0)
    respond_initialize(id);
  else if (strcmp(method, "ping") == 0)
  {
    printf("{\"jsonrpc\":\"2.0\",\"id\":%s,\"result\":{}}\n", id);
    fflush(stdout);
  }
  else if (strcmp(method, "tools/list") == 0)
    respond_tools_list(id);
  else if (strcmp(method, "tools/call") == 0)
    respond_tools_call(id, line, tokens, root);
  else
    respond_error(id, -32601, "method not found");
}

int main(void)
{
  char line[LINE_CAP];

  while (fgets(line, sizeof(line), stdin) != NULL)
    handle_line(line);

  return 0;
}
