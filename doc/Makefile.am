SHELL = /bin/sh
TXT2MAN = ./txt2man
PRODUCT = gmdb2

# To make the userguide, export DOCBOOK_DSL TO point to docbook.dsl.
dist_man_MANS =
if ENABLE_MAN
  dist_man_MANS += gmdb2.1
endif
CLEANFILES = ${dist_man_MANS}
EXTRA_DIST	= gmdb2.txt

.txt.1:
	$(TXT2MAN) -t $* -r "$(PRODUCT) $(VERSION)" -s 1 -v "Executable programs or shell commands" $(srcdir)/$< > $@
