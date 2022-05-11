// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

//#pragma once

#include "gandiva/gdv_function_stubs.h"

#include <utf8proc.h>
#include <string>
#include <vector>

#include "arrow/util/double_conversion.h"
#include "arrow/util/string_view.h"
#include "arrow/util/utf8.h"
#include "arrow/util/value_parsing.h"

#include "gandiva/engine.h"
#include "gandiva/exported_funcs.h"
#include "gandiva/formatting_utils.h"
#include "gandiva/precompiled/types.h"
#include "gandiva/regex_functions_holder.h"

extern "C" {

bool gdv_fn_like_utf8_utf8(int64_t ptr, const char* data, int data_len,
                           const char* pattern, int pattern_len) {
  gandiva::LikeHolder* holder = reinterpret_cast<gandiva::LikeHolder*>(ptr);
  return (*holder)(std::string(data, data_len));
}

bool gdv_fn_like_utf8_utf8_utf8(int64_t ptr, const char* data, int data_len,
                                const char* pattern, int pattern_len,
                                const char* escape_char, int escape_char_len) {
  gandiva::LikeHolder* holder = reinterpret_cast<gandiva::LikeHolder*>(ptr);
  return (*holder)(std::string(data, data_len));
}

bool gdv_fn_ilike_utf8_utf8(int64_t ptr, const char* data, int data_len,
                            const char* pattern, int pattern_len) {
  gandiva::LikeHolder* holder = reinterpret_cast<gandiva::LikeHolder*>(ptr);
  return (*holder)(std::string(data, data_len));
}

const char* gdv_fn_regexp_replace_utf8_utf8(
    int64_t ptr, int64_t holder_ptr, const char* data, int32_t data_len,
    const char* /*pattern*/, int32_t /*pattern_len*/, const char* replace_string,
    int32_t replace_string_len, int32_t* out_length) {
  gandiva::ExecutionContext* context = reinterpret_cast<gandiva::ExecutionContext*>(ptr);

  gandiva::ReplaceHolder* holder = reinterpret_cast<gandiva::ReplaceHolder*>(holder_ptr);

  return (*holder)(context, data, data_len, replace_string, replace_string_len,
                   out_length);
}

const char* gdv_fn_regexp_extract_utf8_utf8_int32(int64_t ptr, int64_t holder_ptr,
                                                  const char* data, int32_t data_len,
                                                  const char* /*pattern*/,
                                                  int32_t /*pattern_len*/,
                                                  int32_t extract_index,
                                                  int32_t* out_length) {
  gandiva::ExecutionContext* context = reinterpret_cast<gandiva::ExecutionContext*>(ptr);

  gandiva::ExtractHolder* holder = reinterpret_cast<gandiva::ExtractHolder*>(holder_ptr);

  return (*holder)(context, data, data_len, extract_index, out_length);
}

#define GDV_FN_CAST_VARLEN_TYPE_FROM_TYPE(IN_TYPE, CAST_NAME, ARROW_TYPE)         \
  GANDIVA_EXPORT                                                                  \
  const char* gdv_fn_cast##CAST_NAME##_##IN_TYPE##_int64(                         \
      int64_t context, gdv_##IN_TYPE value, int64_t len, int32_t * out_len) {     \
    if (len < 0) {                                                                \
      gdv_fn_context_set_error_msg(context, "Buffer length can not be negative"); \
      *out_len = 0;                                                               \
      return "";                                                                  \
    }                                                                             \
    if (len == 0) {                                                               \
      *out_len = 0;                                                               \
      return "";                                                                  \
    }                                                                             \
    arrow::internal::StringFormatter<arrow::ARROW_TYPE> formatter;                \
    char* ret = reinterpret_cast<char*>(                                          \
        gdv_fn_context_arena_malloc(context, static_cast<int32_t>(len)));         \
    if (ret == nullptr) {                                                         \
      gdv_fn_context_set_error_msg(context, "Could not allocate memory");         \
      *out_len = 0;                                                               \
      return "";                                                                  \
    }                                                                             \
    arrow::Status status = formatter(value, [&](arrow::util::string_view v) {     \
      int64_t size = static_cast<int64_t>(v.size());                              \
      *out_len = static_cast<int32_t>(len < size ? len : size);                   \
      memcpy(ret, v.data(), *out_len);                                            \
      return arrow::Status::OK();                                                 \
    });                                                                           \
    if (!status.ok()) {                                                           \
      std::string err = "Could not cast " + std::to_string(value) + " to string"; \
      gdv_fn_context_set_error_msg(context, err.c_str());                         \
      *out_len = 0;                                                               \
      return "";                                                                  \
    }                                                                             \
    return ret;                                                                   \
  }

#define GDV_FN_CAST_VARLEN_TYPE_FROM_REAL(IN_TYPE, CAST_NAME, ARROW_TYPE)         \
  GANDIVA_EXPORT                                                                  \
  const char* gdv_fn_cast##CAST_NAME##_##IN_TYPE##_int64(                         \
      int64_t context, gdv_##IN_TYPE value, int64_t len, int32_t * out_len) {     \
    if (len < 0) {                                                                \
      gdv_fn_context_set_error_msg(context, "Buffer length can not be negative"); \
      *out_len = 0;                                                               \
      return "";                                                                  \
    }                                                                             \
    if (len == 0) {                                                               \
      *out_len = 0;                                                               \
      return "";                                                                  \
    }                                                                             \
    gandiva::GdvStringFormatter<arrow::ARROW_TYPE> formatter;                     \
    char* ret = reinterpret_cast<char*>(                                          \
        gdv_fn_context_arena_malloc(context, static_cast<int32_t>(len)));         \
    if (ret == nullptr) {                                                         \
      gdv_fn_context_set_error_msg(context, "Could not allocate memory");         \
      *out_len = 0;                                                               \
      return "";                                                                  \
    }                                                                             \
    arrow::Status status = formatter(value, [&](arrow::util::string_view v) {     \
      int64_t size = static_cast<int64_t>(v.size());                              \
      *out_len = static_cast<int32_t>(len < size ? len : size);                   \
      memcpy(ret, v.data(), *out_len);                                            \
      return arrow::Status::OK();                                                 \
    });                                                                           \
    if (!status.ok()) {                                                           \
      std::string err = "Could not cast " + std::to_string(value) + " to string"; \
      gdv_fn_context_set_error_msg(context, err.c_str());                         \
      *out_len = 0;                                                               \
      return "";                                                                  \
    }                                                                             \
    return ret;                                                                   \
  }

#define CAST_VARLEN_TYPE_FROM_NUMERIC(VARLEN_TYPE)                   \
  GDV_FN_CAST_VARLEN_TYPE_FROM_TYPE(int32, VARLEN_TYPE, Int32Type)   \
  GDV_FN_CAST_VARLEN_TYPE_FROM_TYPE(int64, VARLEN_TYPE, Int64Type)   \
  GDV_FN_CAST_VARLEN_TYPE_FROM_TYPE(date64, VARLEN_TYPE, Date64Type) \
  GDV_FN_CAST_VARLEN_TYPE_FROM_REAL(float32, VARLEN_TYPE, FloatType) \
  GDV_FN_CAST_VARLEN_TYPE_FROM_REAL(float64, VARLEN_TYPE, DoubleType)

CAST_VARLEN_TYPE_FROM_NUMERIC(VARCHAR)
CAST_VARLEN_TYPE_FROM_NUMERIC(VARBINARY)

#undef CAST_VARLEN_TYPE_FROM_NUMERIC
#undef GDV_FN_CAST_VARLEN_TYPE_FROM_TYPE
#undef GDV_FN_CAST_VARLEN_TYPE_FROM_REAL

GDV_FORCE_INLINE
void gdv_fn_set_error_for_invalid_utf8(int64_t execution_context, char val) {
  char const* fmt = "unexpected byte \\%02hhx encountered while decoding utf8 string";
  int size = static_cast<int>(strlen(fmt)) + 64;
  char* error = reinterpret_cast<char*>(malloc(size));
  snprintf(error, size, fmt, (unsigned char)val);
  gdv_fn_context_set_error_msg(execution_context, error);
  free(error);
}

GDV_FORCE_INLINE
int32_t gdv_fn_utf8_char_length(char c) {
  if ((signed char)c >= 0) {  // 1-byte char (0x00 ~ 0x7F)
    return 1;
  } else if ((c & 0xE0) == 0xC0) {  // 2-byte char
    return 2;
  } else if ((c & 0xF0) == 0xE0) {  // 3-byte char
    return 3;
  } else if ((c & 0xF8) == 0xF0) {  // 4-byte char
    return 4;
  }
  // invalid char
  return 0;
}

// Convert an utf8 string to its corresponding lowercase string
GANDIVA_EXPORT
const char* gdv_fn_lower_utf8(int64_t context, const char* data, int32_t data_len,
                              int32_t* out_len) {
  if (data_len == 0) {
    *out_len = 0;
    return "";
  }

  // If it is a single-byte character (ASCII), corresponding lowercase is always 1-byte
  // long; if it is >= 2 bytes long, lowercase can be at most 4 bytes long, so length of
  // the output can be at most twice the length of the input
  char* out = reinterpret_cast<char*>(gdv_fn_context_arena_malloc(context, 2 * data_len));
  if (out == nullptr) {
    gdv_fn_context_set_error_msg(context, "Could not allocate memory for output string");
    *out_len = 0;
    return "";
  }

  int32_t char_len, out_char_len, out_idx = 0;
  uint32_t char_codepoint;

  for (int32_t i = 0; i < data_len; i += char_len) {
    char_len = gdv_fn_utf8_char_length(data[i]);
    // For single byte characters:
    // If it is an uppercase ASCII character, set the output to its corresponding
    // lowercase character; else, set the output to the read character
    if (char_len == 1) {
      char cur = data[i];
      // 'A' - 'Z' : 0x41 - 0x5a
      // 'a' - 'z' : 0x61 - 0x7a
      if (cur >= 0x41 && cur <= 0x5a) {
        out[out_idx++] = static_cast<char>(cur + 0x20);
      } else {
        out[out_idx++] = cur;
      }
      continue;
    }

    // Control reaches here when we encounter a multibyte character
    const auto* in_char = (const uint8_t*)(data + i);

    // Decode the multibyte character
    bool is_valid_utf8_char =
        arrow::util::UTF8Decode((const uint8_t**)&in_char, &char_codepoint);

    // If it is an invalid utf8 character, UTF8Decode evaluates to false
    if (!is_valid_utf8_char) {
      gdv_fn_set_error_for_invalid_utf8(context, data[i]);
      *out_len = 0;
      return "";
    }

    // Convert the encoded codepoint to its lowercase codepoint
    int32_t lower_codepoint = utf8proc_tolower(char_codepoint);

    // UTF8Encode advances the pointer by the number of bytes present in the lowercase
    // character
    auto* out_char = (uint8_t*)(out + out_idx);
    uint8_t* out_char_start = out_char;

    // Encode the lowercase character
    out_char = arrow::util::UTF8Encode(out_char, lower_codepoint);

    out_char_len = static_cast<int32_t>(out_char - out_char_start);
    out_idx += out_char_len;
  }

  *out_len = out_idx;
  return out;
}

// Convert an utf8 string to its corresponding uppercase string
GANDIVA_EXPORT
const char* gdv_fn_upper_utf8(int64_t context, const char* data, int32_t data_len,
                              int32_t* out_len) {
  if (data_len == 0) {
    *out_len = 0;
    return "";
  }

  // If it is a single-byte character (ASCII), corresponding uppercase is always 1-byte
  // long; if it is >= 2 bytes long, uppercase can be at most 4 bytes long, so length of
  // the output can be at most twice the length of the input
  char* out = reinterpret_cast<char*>(gdv_fn_context_arena_malloc(context, 2 * data_len));
  if (out == nullptr) {
    gdv_fn_context_set_error_msg(context, "Could not allocate memory for output string");
    *out_len = 0;
    return "";
  }

  int32_t char_len, out_char_len, out_idx = 0;
  uint32_t char_codepoint;

  for (int32_t i = 0; i < data_len; i += char_len) {
    char_len = gdv_fn_utf8_char_length(data[i]);
    // For single byte characters:
    // If it is a lowercase ASCII character, set the output to its corresponding uppercase
    // character; else, set the output to the read character
    if (char_len == 1) {
      char cur = data[i];
      // 'A' - 'Z' : 0x41 - 0x5a
      // 'a' - 'z' : 0x61 - 0x7a
      if (cur >= 0x61 && cur <= 0x7a) {
        out[out_idx++] = static_cast<char>(cur - 0x20);
      } else {
        out[out_idx++] = cur;
      }
      continue;
    }

    // Control reaches here when we encounter a multibyte character
    const auto* in_char = (const uint8_t*)(data + i);

    // Decode the multibyte character
    bool is_valid_utf8_char =
        arrow::util::UTF8Decode((const uint8_t**)&in_char, &char_codepoint);

    // If it is an invalid utf8 character, UTF8Decode evaluates to false
    if (!is_valid_utf8_char) {
      gdv_fn_set_error_for_invalid_utf8(context, data[i]);
      *out_len = 0;
      return "";
    }

    // Convert the encoded codepoint to its uppercase codepoint
    int32_t upper_codepoint = utf8proc_toupper(char_codepoint);

    // UTF8Encode advances the pointer by the number of bytes present in the uppercase
    // character
    auto* out_char = (uint8_t*)(out + out_idx);
    uint8_t* out_char_start = out_char;

    // Encode the uppercase character
    out_char = arrow::util::UTF8Encode(out_char, upper_codepoint);

    out_char_len = static_cast<int32_t>(out_char - out_char_start);
    out_idx += out_char_len;
  }

  *out_len = out_idx;
  return out;
}

// Any codepoint, except the ones for lowercase letters, uppercase letters,
// titlecase letters, decimal digits and letter numbers categories will be
// considered as word separators.
//
// The Unicode characters also are divided between categories. This link
// https://www.compart.com/en/unicode/category shows
// more information about characters categories.
GDV_FORCE_INLINE
bool gdv_fn_is_codepoint_for_space(uint32_t val) {
  auto category = utf8proc_category(val);

  return category != utf8proc_category_t::UTF8PROC_CATEGORY_LU &&
         category != utf8proc_category_t::UTF8PROC_CATEGORY_LL &&
         category != utf8proc_category_t::UTF8PROC_CATEGORY_LT &&
         category != utf8proc_category_t::UTF8PROC_CATEGORY_NL &&
         category != utf8proc_category_t ::UTF8PROC_CATEGORY_ND;
}

// For a given text, initialize the first letter after a word-separator and lowercase
// the others e.g:
//     - "IT is a tEXt str" -> "It Is A Text Str"
GANDIVA_EXPORT
const char* gdv_fn_initcap_utf8(int64_t context, const char* data, int32_t data_len,
                                int32_t* out_len) {
  if (data_len == 0) {
    *out_len = data_len;
    return "";
  }

  // If it is a single-byte character (ASCII), corresponding uppercase is always 1-byte
  // long; if it is >= 2 bytes long, uppercase can be at most 4 bytes long, so length of
  // the output can be at most twice the length of the input
  char* out = reinterpret_cast<char*>(gdv_fn_context_arena_malloc(context, 2 * data_len));
  if (out == nullptr) {
    gdv_fn_context_set_error_msg(context, "Could not allocate memory for output string");
    *out_len = 0;
    return "";
  }

  int32_t char_len = 0;
  int32_t out_char_len = 0;
  int32_t out_idx = 0;
  uint32_t char_codepoint;

  // Any character is considered as space, except if it is alphanumeric
  bool last_char_was_space = true;

  for (int32_t i = 0; i < data_len; i += char_len) {
    // An optimization for single byte characters:
    if (static_cast<signed char>(data[i]) >= 0) {  // 1-byte char (0x00 ~ 0x7F)
      char_len = 1;
      char cur = data[i];

      if (cur >= 0x61 && cur <= 0x7a && last_char_was_space) {
        // Check if the character is the first one of the word and it is
        // lowercase -> 'a' - 'z' : 0x61 - 0x7a.
        // Then turn it into uppercase -> 'A' - 'Z' : 0x41 - 0x5a
        out[out_idx++] = static_cast<char>(cur - 0x20);
        last_char_was_space = false;
      } else if (cur >= 0x41 && cur <= 0x5a && !last_char_was_space) {
        out[out_idx++] = static_cast<char>(cur + 0x20);
      } else {
        // Check if the ASCII character is not an alphanumeric character:
        // '0' - '9': 0x30 - 0x39
        // 'a' - 'z' : 0x61 - 0x7a
        // 'A' - 'Z' : 0x41 - 0x5a
        last_char_was_space = (cur < 0x30) || (cur > 0x39 && cur < 0x41) ||
                              (cur > 0x5a && cur < 0x61) || (cur > 0x7a);
        out[out_idx++] = cur;
      }
      continue;
    }

    char_len = gdv_fn_utf8_char_length(data[i]);

    // Control reaches here when we encounter a multibyte character
    const auto* in_char = (const uint8_t*)(data + i);

    // Decode the multibyte character
    bool is_valid_utf8_char =
        arrow::util::UTF8Decode((const uint8_t**)&in_char, &char_codepoint);

    // If it is an invalid utf8 character, UTF8Decode evaluates to false
    if (!is_valid_utf8_char) {
      gdv_fn_set_error_for_invalid_utf8(context, data[i]);
      *out_len = 0;
      return "";
    }

    bool is_char_space = gdv_fn_is_codepoint_for_space(char_codepoint);

    int32_t formatted_codepoint;
    if (last_char_was_space && !is_char_space) {
      formatted_codepoint = utf8proc_toupper(char_codepoint);
    } else {
      formatted_codepoint = utf8proc_tolower(char_codepoint);
    }

    // UTF8Encode advances the pointer by the number of bytes present in the character
    auto* out_char = (uint8_t*)(out + out_idx);
    uint8_t* out_char_start = out_char;

    // Encode the character
    out_char = arrow::util::UTF8Encode(out_char, formatted_codepoint);

    out_char_len = static_cast<int32_t>(out_char - out_char_start);
    out_idx += out_char_len;

    last_char_was_space = is_char_space;
  }

  *out_len = out_idx;
  return out;
}
}

namespace gandiva {

void ExportedStringFunctions::AddMappings(Engine* engine) const {
  std::vector<llvm::Type*> args;
  auto types = engine->types();

  // gdv_fn_like_utf8_utf8
  args = {types->i64_type(),     // int64_t ptr
          types->i8_ptr_type(),  // const char* data
          types->i32_type(),     // int data_len
          types->i8_ptr_type(),  // const char* pattern
          types->i32_type()};    // int pattern_len

  engine->AddGlobalMappingForFunc("gdv_fn_like_utf8_utf8",
                                  types->i1_type() /*return_type*/, args,
                                  reinterpret_cast<void*>(gdv_fn_like_utf8_utf8));

  // gdv_fn_like_utf8_utf8_utf8
  args = {types->i64_type(),     // int64_t ptr
          types->i8_ptr_type(),  // const char* data
          types->i32_type(),     // int data_len
          types->i8_ptr_type(),  // const char* pattern
          types->i32_type(),     // int pattern_len
          types->i8_ptr_type(),  // const char* escape_char
          types->i32_type()};    // int escape_char_len

  engine->AddGlobalMappingForFunc("gdv_fn_like_utf8_utf8_utf8",
                                  types->i1_type() /*return_type*/, args,
                                  reinterpret_cast<void*>(gdv_fn_like_utf8_utf8_utf8));

  // gdv_fn_ilike_utf8_utf8
  args = {types->i64_type(),     // int64_t ptr
          types->i8_ptr_type(),  // const char* data
          types->i32_type(),     // int data_len
          types->i8_ptr_type(),  // const char* pattern
          types->i32_type()};    // int pattern_len

  engine->AddGlobalMappingForFunc("gdv_fn_ilike_utf8_utf8",
                                  types->i1_type() /*return_type*/, args,
                                  reinterpret_cast<void*>(gdv_fn_ilike_utf8_utf8));

  // gdv_fn_regexp_replace_utf8_utf8
  args = {types->i64_type(),       // int64_t ptr
          types->i64_type(),       // int64_t holder_ptr
          types->i8_ptr_type(),    // const char* data
          types->i32_type(),       // int data_len
          types->i8_ptr_type(),    // const char* pattern
          types->i32_type(),       // int pattern_len
          types->i8_ptr_type(),    // const char* replace_string
          types->i32_type(),       // int32_t replace_string_len
          types->i32_ptr_type()};  // int32_t* out_length

  engine->AddGlobalMappingForFunc(
      "gdv_fn_regexp_replace_utf8_utf8", types->i8_ptr_type() /*return_type*/, args,
      reinterpret_cast<void*>(gdv_fn_regexp_replace_utf8_utf8));

  // gdv_fn_regexp_extract_utf8_utf8_int32
  args = {types->i64_type(),       // int64_t ptr
          types->i64_type(),       // int64_t holder_ptr
          types->i8_ptr_type(),    // const char* data
          types->i32_type(),       // int data_len
          types->i8_ptr_type(),    // const char* pattern
          types->i32_type(),       // int pattern_len
          types->i32_type(),       // int32_t extract_index
          types->i32_ptr_type()};  // int32_t* out_length

  engine->AddGlobalMappingForFunc(
      "gdv_fn_regexp_extract_utf8_utf8_int32", types->i8_ptr_type() /*return_type*/, args,
      reinterpret_cast<void*>(gdv_fn_regexp_extract_utf8_utf8_int32));

  // gdv_fn_castVARCHAR_int32_int64
  args = {types->i64_type(),       // int64_t execution_context
          types->i32_type(),       // int32_t value
          types->i64_type(),       // int64_t len
          types->i32_ptr_type()};  // int32_t* out_len
  engine->AddGlobalMappingForFunc(
      "gdv_fn_castVARCHAR_int32_int64", types->i8_ptr_type() /*return_type*/, args,
      reinterpret_cast<void*>(gdv_fn_castVARCHAR_int32_int64));

  // gdv_fn_castVARCHAR_int64_int64
  args = {types->i64_type(),       // int64_t execution_context
          types->i64_type(),       // int64_t value
          types->i64_type(),       // int64_t len
          types->i32_ptr_type()};  // int32_t* out_len
  engine->AddGlobalMappingForFunc(
      "gdv_fn_castVARCHAR_int64_int64", types->i8_ptr_type() /*return_type*/, args,
      reinterpret_cast<void*>(gdv_fn_castVARCHAR_int64_int64));

  // gdv_fn_castVARCHAR_milliseconds
  args = {types->i64_type(),       // int64_t execution_context
          types->i64_type(),       // gdv_date64 value
          types->i64_type(),       // int64_t len
          types->i32_ptr_type()};  // int32_t* out_len
  engine->AddGlobalMappingForFunc(
      "gdv_fn_castVARCHAR_date64_int64", types->i8_ptr_type() /*return_type*/, args,
      reinterpret_cast<void*>(gdv_fn_castVARCHAR_date64_int64));

  // gdv_fn_castVARCHAR_float32_int64
  args = {types->i64_type(),       // int64_t execution_context
          types->float_type(),     // float value
          types->i64_type(),       // int64_t len
          types->i32_ptr_type()};  // int32_t* out_len
  engine->AddGlobalMappingForFunc(
      "gdv_fn_castVARCHAR_float32_int64", types->i8_ptr_type() /*return_type*/, args,
      reinterpret_cast<void*>(gdv_fn_castVARCHAR_float32_int64));

  // gdv_fn_castVARCHAR_float64_int64
  args = {types->i64_type(),       // int64_t execution_context
          types->double_type(),    // double value
          types->i64_type(),       // int64_t len
          types->i32_ptr_type()};  // int32_t* out_len
  engine->AddGlobalMappingForFunc(
      "gdv_fn_castVARCHAR_float64_int64", types->i8_ptr_type() /*return_type*/, args,
      reinterpret_cast<void*>(gdv_fn_castVARCHAR_float64_int64));

  // gdv_fn_castVARBINARY_int32
  args = {
      types->i64_type(),     // context
      types->i32_type(),     // int32_t value
      types->i64_type(),     // int64_t out value length
      types->i32_ptr_type()  // int32_t out_length
  };

  engine->AddGlobalMappingForFunc(
      "gdv_fn_castVARBINARY_int32_int64", types->i8_ptr_type() /*return_type*/, args,
      reinterpret_cast<void*>(gdv_fn_castVARBINARY_int32_int64));

  // gdv_fn_castVARBINARY_int64
  args = {
      types->i64_type(),     // context
      types->i64_type(),     // int64_t value
      types->i64_type(),     // int64_t out value length
      types->i32_ptr_type()  // int32_t out_length
  };

  engine->AddGlobalMappingForFunc(
      "gdv_fn_castVARBINARY_int64_int64", types->i8_ptr_type() /*return_type*/, args,
      reinterpret_cast<void*>(gdv_fn_castVARBINARY_int64_int64));

  // gdv_fn_castVARBINARY_float32
  args = {
      types->i64_type(),     // context
      types->float_type(),   // float value
      types->i64_type(),     // int64_t out value length
      types->i64_ptr_type()  // int32_t out_length
  };

  engine->AddGlobalMappingForFunc(
      "gdv_fn_castVARBINARY_float32_int64", types->i8_ptr_type() /*return_type*/, args,
      reinterpret_cast<void*>(gdv_fn_castVARBINARY_float32_int64));

  // gdv_fn_castVARBINARY_float64
  args = {
      types->i64_type(),     // context
      types->i64_type(),     // double value
      types->i64_type(),     // int64_t out value length
      types->i32_ptr_type()  // int32_t out_length
  };

  engine->AddGlobalMappingForFunc(
      "gdv_fn_castVARBINARY_float64_int64", types->i8_ptr_type() /*return_type*/, args,
      reinterpret_cast<void*>(gdv_fn_castVARBINARY_float64_int64));

  // gdv_fn_lower_utf8
  args = {
      types->i64_type(),      // context
      types->i8_ptr_type(),   // data
      types->i32_type(),      // data_len
      types->i32_ptr_type(),  // out_len
  };

  engine->AddGlobalMappingForFunc("gdv_fn_lower_utf8",
                                  types->i8_ptr_type() /*return_type*/, args,
                                  reinterpret_cast<void*>(gdv_fn_lower_utf8));

  // gdv_fn_upper_utf8
  args = {
      types->i64_type(),      // context
      types->i8_ptr_type(),   // data
      types->i32_type(),      // data_len
      types->i32_ptr_type(),  // out_len
  };

  engine->AddGlobalMappingForFunc("gdv_fn_upper_utf8",
                                  types->i8_ptr_type() /*return_type*/, args,
                                  reinterpret_cast<void*>(gdv_fn_upper_utf8));

  // gdv_fn_initcap_utf8
  args = {
      types->i64_type(),     // context
      types->i8_ptr_type(),  // const char*
      types->i32_type(),     // value_length
      types->i32_ptr_type()  // out_length
  };

  engine->AddGlobalMappingForFunc("gdv_fn_initcap_utf8",
                                  types->i8_ptr_type() /*return_type*/, args,
                                  reinterpret_cast<void*>(gdv_fn_initcap_utf8));
}
}  // namespace gandiva
