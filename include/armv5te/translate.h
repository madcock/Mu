/* Declarations for translate.c */

#ifndef H_TRANSLATE
#define H_TRANSLATE

#include <stdbool.h>
#include <stdint.h>

#include "fixings.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
__pragma(pack(push, 1))
#endif

struct translation {
    uintptr_t unused;
    void** jump_table;
    uint32_t *start_ptr;
    uint32_t *end_ptr;
}
#if !defined(_MSC_VER)
__attribute__((packed))
#endif
;

#if defined(_MSC_VER)
__pragma(pack(pop))
#endif

extern struct translation translation_table[] __asm__("translation_table");
#define INSN_BUFFER_SIZE 0x1000000

bool translate_init();
void translate_deinit();
void translate(uint32_t start_pc, uint32_t *insnp);
void flush_translations();
void invalidate_translation(int index);
void translate_fix_pc();

#ifdef __cplusplus
}
#endif

#endif
