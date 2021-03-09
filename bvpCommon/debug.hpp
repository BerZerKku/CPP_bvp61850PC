/*
 * debug.h
 *
 *  Created on: Aug 10, 2020
 *      Author: bear
 */

#ifndef DEBUG_HPP_
#define DEBUG_HPP_

#include <errno.h>

#if defined(QT_CORE_LIB)

#include <QDebug>
#define QDEBUG(x) qDebug() << x;

#else

#include <assert.h>
#define Q_ASSERT(x) (assert(x))
#define QDEBUG(x) void (0)

#endif

#ifdef __cplusplus
extern "C" {
#endif

  // FIXME В QT флаг NDEBUG не используется. Надо найти его замену!

#if defined(NDEBUG)

  #define PRINT_ERROR(x) (void(0))

#else

  #define PRINT_ERROR(x) printError( __FILE__, __LINE__, __FUNCTION__, x)

  /** Вывод на экран ошибки и места где она произошла.
   *
   *  @param[in] file
   *  @param[in] line
   *  @param[in] function
   *  @param[in] error
   */
  void printError(const char *file, int line, const char *function, int error);
#endif


#ifdef __cplusplus
}
#endif

#endif /* DEBUG_HPP_ */
