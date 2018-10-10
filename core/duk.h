#ifndef EUPHORIA_DUK_H
#define EUPHORIA_DUK_H


#include <string>
#include <functional>
#include <map>
#include <memory>
#include <vector>
#include <utility>

#include "core/assert.h"
#include "core/str.h"
#include "core/stringmerger.h"
#include "core/cpp.h"
#include "core/duk_types.h"
#include "core/duk_config.h"
#include "core/duk_util.h"
#include "core/duk_prototype.h"

class Duk;
class Context;


class StoredReference;

#include "core/duk_value.h"

#include "core/duk_context.h"

#include "core/duk_pushvarimpl.h"

#include "core/duk_function.h"

#include "core/duk_overload.h"

#include "core/duk_bindfunction.h"

#include "duk_template.h"

#include "duk_genericoverload.h"

#include "core/duk_bind.h"

#include "core/duk_bindclass.h"

#include "core/duk_bindobject.h"

#include "core/duk_referencestorage.h"

#include "core/duk_storedreference.h"

#include "core/duk_duk.h"

#endif  // EUPHORIA_DUK_H
