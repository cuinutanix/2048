CFLAGS=-m32 -march=i686 -Os -ffreestanding -nostdinc -Wall -Werror
ASFLAGS=-m32

.PHONY: clean all

all: boot.bin

IMAGE_FILES := $(addprefix assets/, \
  tile_0.h \
  tile_2.h \
  tile_4.h \
  tile_8.h \
  tile_16.h \
  tile_32.h \
  tile_64.h \
  tile_128.h \
  tile_256.h \
  tile_512.h \
  tile_1024.h \
  tile_2048.h \
  tile_4096.h \
  tile_8192.h \
  tile_16384.h \
  tile_32768.h \
  tile_65536.h \
  tile_131072.h \
  container.h \
  title.h)

ifneq ($(WITH_NUTANIX_LOGO),)
main.o: assets/login_logo.h
CFLAGS += -DHAS_NUTANIX_LOGO
endif

assets/%.h : assets/%.png
	./png_to_c.py $^ $@.tmp >/dev/null
	mv $@.tmp $@

FONT_FILES = assets/font10x20.h

assets/font%.h : assets/%.bdf
	./bdf_to_c.py < $^ > $@.tmp
	mv $@.tmp $@

fb.o: $(FONT_FILES)

2048.o: $(IMAGE_FILES)

boot.elf: mbr.o main.o realmode.o irq.o console.o fb.o 2048.o image.o
	ld -m elf_i386 -T linker.lds -o $@ $^

%.bin: %.elf
	objcopy -O binary $^ $@

clean:
	rm -rf *.bin *.elf *.o assets/*.h
