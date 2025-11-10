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

#muziek.png: muziek.ly build/funcharmonie
#	lilypond --png -dresolution=600 -dcrop=#t muziek.ly
#	mv muziek.cropped.png muziek.png

muziek.png: muziek.svg
	convert muziek.svg muziek.png

toon: muziek.svg
	gwenview muziek.svg

toon-png: muziek.png
	gwenview muziek.png

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


