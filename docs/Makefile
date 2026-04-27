NM=funcharmony

all: view

${NM}-kort.png: ${NM}-kort.puml
	plantuml.sh ${NM}-kort.puml

view: ${NM}-kort.png
	gwenview ${NM}-kort.png


${NM}.pdf: ${NM}.md ${NM}-kort.png
	~/bin/mkpan ${NM}.pdf

view2: ${NM}.pdf
	okular ${NM}.pdf

clean:
	rm -vf *.pdf
	rm -vf *.png
