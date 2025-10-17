/* Copyright 2025 OppositeNor

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#ifndef __WBE_REFLECTION_HH__
#define __WBE_REFLECTION_HH__

#include "utils/defs.hh"

#ifdef WBE_REFLECTION_PARSER
#define WBE_ATTR(...) __attribute__((annotate(#__VA_ARGS__)))
#define WBE_META(...) WBE_ATTR(WBE, __VA_ARGS__)
#define WBE_CLASS(class_name, ...) class WBE_META(__VA_ARGS__) class_name
#define WBE_STRUCT(struct_name, ...) struct WBE_META(__VA_ARGS__) struct_name
#define WBE_LABEL(label_name, ...) constexpr HashCode WBE_META(__VA_ARGS__) label_name = static_hash(WBE_EXPAND_STR(label_name));

#else
#define WBE_META(...)
#define WBE_CLASS(class_name, ...) class class_name
#define WBE_STRUCT(struct_name, ...) struct struct_name
#define WBE_LABEL(label_name, ...) constexpr HashCode label_name = static_hash(WBE_EXPAND_STR(label_name));

#endif

#endif
