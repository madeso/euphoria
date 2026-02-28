#pragma once


/** \addtogroup debug Debug
 *  @{
*/

#if FF_HAS(ENABLE_GL_DEBUG)
	#define DEBUG_LABEL_ARG_MANY const std::string& debug_label,
	#define DEBUG_LABEL_ARG_SINGLE const std::string &debug_label
	#define DEBUG_LABEL_EXPLICIT_MANY
	#define SEND_DEBUG_LABEL_MANY(X) X,

	#define USE_DEBUG_LABEL(name) name

	#define USE_DEBUG_LABEL_MANY(name) name,
#else
	#define DEBUG_LABEL_ARG_MANY
	#define DEBUG_LABEL_ARG_SINGLE
	#define DEBUG_LABEL_EXPLICIT_MANY explicit
	#define SEND_DEBUG_LABEL_MANY(X)

	#define USE_DEBUG_LABEL(name)
	#define USE_DEBUG_LABEL_MANY(name)
#endif

/// \def DEBUG_LABEL_ARG_MANY
/// first debug label argument of many

/// \def DEBUG_LABEL_ARG_SINGLE
/// the only debug label argument (the function doesn't take any more rguments)

/// \def DEBUG_LABEL_EXPLICIT_MANY
/// optional explicit for when the constructor only takes one more argument and needs to be explicit if the label is removed

/// \def SEND_DEBUG_LABEL_MANY
/// forwards a label when there is more than one argument
/// @param X The label to forward

/// \def USE_DEBUG_LABEL
/// use this label and there are no more arguments
/// @param name the label

/// \def USE_DEBUG_LABEL_MANY
/// use this label and there are more arguments besides this one
/// @param name the label


/**
 * @}
*/
