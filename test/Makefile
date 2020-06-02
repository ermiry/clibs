BUILDDIR   := bin

directories:
	@mkdir -p $(BUILDDIR)

test: directories 
	gcc ./dllist_test.c ../src/collections/dllist.c -g -l pthread -o ./$(BUILDDIR)/dllist_test
	gcc ./avl_test.c ../src/collections/avl.c -g -l pthread -o ./$(BUILDDIR)/avl_test
	gcc ./htab_test.c ../src/collections/htab.c -g -o ./$(BUILDDIR)/htab_test