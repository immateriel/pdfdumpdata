all:
	gcc `pkg-config --cflags --libs poppler-glib glib-2.0 cairo` pdfdumpdata.c -o pdfdumpdata
