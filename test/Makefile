BUILDDIR   := bin

directories:
	@mkdir -p $(BUILDDIR)

test: directories 
	gcc -g ./dllist_test.c ../src/collections/dllist.c -l pthread -o ./$(BUILDDIR)/dllist_test
	gcc -g ./avl_test.c ../src/collections/avl.c ./user.c -l pthread -o ./$(BUILDDIR)/avl_test
	gcc -g ./htab_test.c ../src/collections/htab.c ./user.c -l pthread -o ./$(BUILDDIR)/htab_test
	gcc -g ./thpool_test.c ../src/threads/thpool.c ../src/threads/bsem.c ../src/threads/jobs.c ../src/collections/dllist.c -l pthread -o ./$(BUILDDIR)/thpool_test