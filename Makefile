all: build/funcharmonie

lsp:
	cmake -S . -G "Unix Makefiles" -B cmake

build: build/funcharmonie

build/funcharmonie:
	mkdir build;cd build;cmake ..;make

run: build/funcharmonie
	rm -vf muziek.*
	build/funcharmonie

valgrind: build/funcharmonie
	valgrind -s --leak-check=full --show-leak-kinds=all build/funcharmonie

muziek.ly: build/funcharmonie
	build/funcharmonie

muziek.svg: muziek.ly build/funcharmonie
	lilypond --svg -dresolution=600 -dcrop=#t muziek.ly
	mv muziek.cropped.svg muziek.svg

toon: muziek.svg
	gwenview muziek.svg

clean-svg:
	rm -vf *.svg

clean:
	rm -Rvf build
	rm -vf *.svg


# akkoorden tekst
akkoorden.md: build/funcharmonie
	build/funcharmonie >akkoorden.md

akkoorden.pdf: akkoorden.md
	~/bin/mkpan akkoorden.pdf

view: akkoorden.pdf
	okular akkoorden.pdf


