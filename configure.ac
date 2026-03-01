AC_CONFIG_AUX_DIR([build-aux])

AC_INIT([fih], [1.0], [support@example.com])
AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_PROG_CC

AS_IF([test "x$ac_cv_prog_cc_c99" = "xno"], [
    AC_MSG_ERROR([This project requires a C99 compliant compiler.])
], [
	FIH_FLAGS="-std=c99"
])

AS_IF([test "x$ac_cv_prog_cc_c11" = "xno"], [
	AC_MSG_WARN([C11 support not detected; falling back to C99.])
], [
	AC_MSG_NOTICE([C11 support is enabled.])
	FIH_FLAGS="-std=c11 -Wall"
])

AC_SUBST([FIH_FLAGS])

PKG_CHECK_MODULES([X11], [x11])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
