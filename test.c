#include "stdio.h"
#include "stdint.h"

#ifndef NOMINMAX
  #define NOMINMAX
#endif

#include <windows.h>

// function prototype
typedef int WINAPI OodleLZ_Decompress_FP (
  uint8_t *src_buf, int src_len, uint8_t *dst_buff, size_t dst_size, int fuzz,
  int crc, int verbose, uint8_t *dst_base, size_t e, void *cb, void *cb_ctx, 
  void *scratch, size_t scratch_size, int threadPhase
);

// game uses some wrapper around oodle compressed block
struct upk {
  uint32_t signature;
  uint32_t unkn1;
  uint32_t max_block_size;
  uint32_t unkn2;
  uint32_t size_cmps;
  char unused2[28];
};

int main() {
  HMODULE oodle = LoadLibraryA("oo2core_9_win64.dll");
  
  OodleLZ_Decompress_FP *OodleLZ_Decompress = (OodleLZ_Decompress_FP *) GetProcAddress(oodle, "OodleLZ_Decompress");

  // compressed db from *.sav file
  FILE *fpin = fopen("compressed_db.upk", "rb");

  // uncompressed db (open in hex and delete first 8 bytes)
  FILE *fpout = fopen("db.sqlite", "wb+");

  fseek(fpin, 0, SEEK_END);
  long sz = ftell(fpin);
  fseek(fpin, 0, SEEK_SET);

  while (1) {
    struct upk f1;
    size_t rbytes = fread(&f1, sizeof(struct upk), 1, fpin);

    if (rbytes != 1) {
      printf("fuck\n");
      break;
    }

    char *buffer = (char *)malloc(f1.size_cmps);
    fread(buffer, sizeof (char), f1.size_cmps, fpin);

    uint32_t cmpsz = f1.max_block_size;

    char *output = (char *)malloc(cmpsz);

    int decompressed = OodleLZ_Decompress(buffer, sz, output, cmpsz, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3);

    printf("Decompressed: %d\n", decompressed);

    fwrite(output, cmpsz, 1, fpout);
  }

  fclose(fpout);
  fclose(fpin);

  FreeLibrary(oodle);

  return 0;
}
