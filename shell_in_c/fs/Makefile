build-fuse:
	mkdir -p bin/fuse
	gcc -Iinclude -DDISK_LOCAL -c src/fuse/disk.c       -o bin/fuse/disk.o
	gcc -Iinclude -DDISK_LOCAL -c src/common/fs.c       -o bin/fuse/fs.o 
	gcc -Iinclude -DDISK_LOCAL -c src/common/hash.c     -o bin/fuse/hash.o 
	gcc -Iinclude -DDISK_LOCAL -c src/fuse/fuse_binds.c -o bin/fuse/binds.o `pkg-config fuse --cflags --libs`
	gcc -o bin/fuse/ssfs bin/fuse/disk.o bin/fuse/hash.o bin/fuse/fs.o bin/fuse/binds.o `pkg-config fuse --cflags --libs`
	echo 'To Mount: ./bin/fuse/ssfs -f [mount point]'