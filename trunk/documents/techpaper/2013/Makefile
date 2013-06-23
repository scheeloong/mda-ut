default: paper.pdf

SRC = abstract.tex conclusions.tex introduction.tex paper.tex electronics.tex embedded.tex software.tex frame.tex  

paper.pdf: $(SRC)
	pdflatex paper
	# Need to run twice to get references to work on my machine - Victor
	pdflatex paper

clean:
	rm -f paper.pdf paper.aux paper.log

.PHONY: default clean
