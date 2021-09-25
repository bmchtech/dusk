# atlas grit recipes

a_part_0.img.bin: a_part_0.png
	$(call atlpng2grit,1)

a_chars_0.img.bin: a_chars_0.png
	$(call atlpng2grit,2)

a_logo_0.img.bin: a_logo_0.png
	$(call atlpng2grit,8)

a_thred_0.img.bin: a_thred_0.png
	$(call atlpng2grit,8)

a_mus_0.img.bin: a_mus_0.png
	$(call atlpng2grit,8)