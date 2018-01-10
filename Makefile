NAME = btbxTemplate

GAME_C_FILES = main.c btbxTemplate.c image_bg.c sounds.c lib/blitter/blitter.c lib/blitter/blitter_tmap.c lib/blitter/blitter_sprites.c lib/sampler/sampler.c 
GAME_BINARY_FILES = image_bg.tmap image_bg.tset ninja.spr 


include $(BITBOX)/kernel/bitbox.mk
main.c: image_bg.tmap ninja.spr 

%.tset %.tmap %.c %.h: %.tmx
	python tmx.py $< > $*.h

ninja.spr : ninja/ninja_??.png
	python $(BITBOX)/lib/blitter/scripts/couples_encode.py $@ $(sort $^)

sounds.c: sounds/*.wav mk_sounds.py
	python mk_sounds.py $< > $@

clean::
	rm -f *.tset *.tmap *.spr
	rm -f image_bg.[ch] sounds.[ch]
