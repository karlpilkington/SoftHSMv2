MAINTAINERCLEANFILES =	$(srcdir)/Makefile.in

AM_CPPFLAGS = 		-I$(srcdir)/../../lib/cryptoki_compat \
			-I$(srcdir)/../common \
			@SQLITE3_INCLUDES@

dist_man_MANS =		softhsm2-migrate.1

bin_PROGRAMS =		softhsm2-migrate

softhsm2_migrate_SOURCES =	softhsm2-migrate.cpp \
				../common/getpw.cpp \
				../common/library.cpp
softhsm2_migrate_LDADD =	@SQLITE3_LIBS@ \
				@YIELD_LIB@

EXTRA_DIST =		$(srcdir)/*.h
