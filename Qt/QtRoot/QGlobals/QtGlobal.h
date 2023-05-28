#pragma once

#include <cstdint>
#include <cmath>
#include <math.h>
#include <assert.h>

using qint8 = int8_t;
using quint8 = uint8_t;
using qint16 = int16_t;
using quint16 = uint16_t;
using qint32 = int32_t;
using quint32 = uint32_t;
using qint64 = int64_t;
using quint64 = uint64_t;
using qlonglong = long long;
using qulonglong = unsigned long long;

#define emit
#define qQNaN() NAN
#define qIsNaN(n) isnan(n)
#define Q_ASSERT(x) assert(x)
#define Q_UNUSED(x)
