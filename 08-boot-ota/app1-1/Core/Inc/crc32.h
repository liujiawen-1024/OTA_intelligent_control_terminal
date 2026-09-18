// crc32_ota.h
#ifndef __CRC32_H__
#define __CRC32_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    uint32_t crc;  // 中间CRC值
} CRC32_Context;
extern CRC32_Context crc_ctx;  // 全局CRC上下文

void CRC32_Init(CRC32_Context *ctx);
void CRC32_Update(CRC32_Context *ctx, const uint8_t *data, uint32_t len);
uint32_t CRC32_Final(CRC32_Context *ctx, int swap_endian) ;



#ifdef __cplusplus
}
#endif
#endif /*__CRC32_H__ */

