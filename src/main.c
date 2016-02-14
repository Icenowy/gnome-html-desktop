/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2016 Icenowy Zheng <icenowy@outlook.com>
 * 
 * gnome-html-desktop is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * gnome-html-desktop is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

#include <webkit2/webkit2.h>

#include <X11/Xatom.h>

#include <glib/gi18n.h>

#include <stdlib.h>

static struct GnomeHTMLDesktop {
	GtkWindow *window;
	WebKitWebView *view;
	gulong size_changed_id;
} main_desktop;

typedef struct GnomeHTMLDesktop GnomeHTMLDesktop;

static void
gnome_html_desktop_window_screen_size_changed (GdkScreen *screen,
	GtkWindow *window)
{
	int width_request, height_request;

	width_request = gdk_screen_get_width (screen);
	height_request = gdk_screen_get_height (screen);
	
	g_object_set (window,
		      "width_request", width_request,
		      "height_request", height_request,
		      NULL);
}

static GtkWindow* create_window (GnomeHTMLDesktop *desktop)
{
	GtkWindow *window;
	GdkDisplay *display;
	GdkScreen *screen;
	char desktop_manager_name[32];
	GdkAtom desktop_atom, atom;

	window = GTK_WINDOW (gtk_window_new (GTK_WINDOW_TOPLEVEL));
	gtk_window_set_title (GTK_WINDOW (window), "GNOME HTML Desktop");

	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

	// Initialize the window as a desktop window

	gtk_window_set_decorated (GTK_WINDOW (window), false);
	gtk_window_set_resizable (GTK_WINDOW (window), false);

	gtk_window_move (GTK_WINDOW (window), 0, 0);
	g_object_set_data (G_OBJECT (window), "is_desktop_window", GINT_TO_POINTER (1));

	screen = gdk_screen_get_default ();
	display = gdk_screen_get_display (screen);

	g_snprintf (desktop_manager_name, sizeof (desktop_manager_name),
		"_NET_DESKTOP_MANAGER_S%d", gdk_screen_get_number (screen));
	desktop_atom = gdk_atom_intern (desktop_manager_name, FALSE);

	if (XGetSelectionOwner (GDK_DISPLAY_XDISPLAY (display),
		gdk_x11_atom_to_xatom_for_display (display, desktop_atom))
		!= None) {
		g_critical ("Another desktop manager in use; desktop window can't be created");
		exit (1);
	}

	gtk_widget_add_events (window, GDK_PROPERTY_CHANGE_MASK);

	gtk_window_set_wmclass (GTK_WINDOW (window), "desktop_window", "Nautilus");

	gtk_widget_realize (GTK_WIDGET (window));
	gdk_flush ();

	atom = gdk_atom_intern ("_NET_WM_WINDOW_TYPE_DESKTOP", FALSE);
	gdk_property_change (gtk_widget_get_window(GTK_WIDGET(window)),
		gdk_atom_intern ("_NET_WM_WINDOW_TYPE", FALSE),
		gdk_x11_xatom_to_atom (XA_ATOM), 32,
		GDK_PROP_MODE_REPLACE, (guchar *) &atom, 1);

	desktop->size_changed_id =
		g_signal_connect (gtk_window_get_screen (GTK_WINDOW (window)), "size-changed",
		G_CALLBACK (gnome_html_desktop_window_screen_size_changed), window);

	gnome_html_desktop_window_screen_size_changed (gtk_window_get_screen (GTK_WINDOW (window)), window);

	desktop->window = window;

	return window;
}

static WebKitWebView* create_view (GnomeHTMLDesktop *desktop)
{
	WebKitWebView *view;

	view = WEBKIT_WEB_VIEW (webkit_web_view_new ());

	webkit_web_view_load_uri (view, "http://www.baidu.com");

	desktop->view = view;

	return view;
}

static void show_window (GnomeHTMLDesktop *desktop)
{
	gtk_container_add (GTK_CONTAINER (desktop->window), GTK_WIDGET (desktop->view));

	gtk_widget_show (GTK_WIDGET (desktop->view));
	gtk_widget_show (GTK_WIDGET (desktop->window));
}

int main (int argc, char *argv[])
{
#ifdef ENABLE_NLS

	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif
	
	gtk_init (&argc, &argv);

	create_window (&main_desktop);
	create_view (&main_desktop);

	show_window (&main_desktop);

	gtk_main ();

	return 0;
}

