## Process this file with automake to produce Makefile.in

## Created by Anjuta



AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(GNOME_HTML_DESKTOP_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = gnome-html-desktop

gnome_html_desktop_SOURCES = \
	main.c


gnome_html_desktop_LDFLAGS = \
	-Wl,--export-dynamic

gnome_html_desktop_LDADD = $(GNOME_HTML_DESKTOP_LIBS)




