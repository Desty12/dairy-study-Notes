#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(GNEST_LIB)
#  define GNEST_EXPORT Q_DECL_EXPORT
# else
#  define GNEST_EXPORT Q_DECL_IMPORT
# endif
#else
# define GNEST_EXPORT
#endif
