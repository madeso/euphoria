#pragma once

// typeinfo.h:
// Exposes macros and types for basic type information of known-at-compile-time
// type information. The type can be a simple as a int and as complex
// as a template argument
//
// Macros:
//
// TYPEID_ID(TYPE)
// The "integer" id of a type
//
// TYPEID_NAME(TYPE)
// The name of a type
//
// TYPEID_SETUP_TYPE(TYPE)
// Sets up typeinfo for a type


#define TYPEID_ID(X) 42
#define TYPEID_NAME(X) "<unknown name>"
#define TYPEID_SETUP_TYPE(X)