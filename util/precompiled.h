#ifndef PRECOMPILED_H
#define PRECOMPILED_H

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>
#include <QtXml/QtXml>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>
#include <functional>
#include <typedefs.h>

inline auto operator"" _qs(const char *s, size_t size) { return QString(s); }

inline auto operator"" _i8(unsigned long long n) { return (int8_t)n; }
inline auto operator"" _i16(unsigned long long n) { return (int16_t)n; }
inline auto operator"" _i32(unsigned long long n) { return (int32_t)n; }
inline auto operator"" _i64(unsigned long long n) { return (int64_t)n; }

inline auto operator"" _ui8(unsigned long long n) { return (uint8_t)n; }
inline auto operator"" _ui16(unsigned long long n) { return (uint16_t)n; }
inline auto operator"" _ui32(unsigned long long n) { return (uint32_t)n; }
inline auto operator"" _ui64(unsigned long long n) { return (uint64_t)n; }

#endif // PRECOMPILED_H
