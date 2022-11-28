BUILD := $(abspath build)

all: $(BUILD) $(BUILD)/mcpde $(BUILD)/mcpde.pdf

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/mcpde: src/mcpde.c
	clang $^ -o $@ -O3 -lm

$(BUILD)/mcpde.pdf: notes/mcpde.tex
	latexmk -auxdir=$(BUILD) -outdir=$(BUILD) -pdf $^
