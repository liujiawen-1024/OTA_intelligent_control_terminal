// crc32.c
#include "crc32.h"

CRC32_Context crc_ctx;  // 全局CRC上下文

// 初始化CRC上下文（开始新校验前必须调用）
void CRC32_Init(CRC32_Context *ctx) {
    ctx->crc = 0xFFFFFFFF;  // Zlib初始值
}

// 追加数据包（可多次调用）
void CRC32_Update(CRC32_Context *ctx, const uint8_t *data, uint32_t len) {
    for (uint32_t i = 0; i < len; i++) {
        ctx->crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            ctx->crc = (ctx->crc >> 1) ^ (0xEDB88320 & -(ctx->crc & 1));
        }
    }
}


// 通用字节交换（适用于任何平台）
uint32_t swap_uint32(uint32_t val) {
    return ((val >> 24) & 0xFF)       |  // 移动最高字节到最低位
           ((val >> 8)  & 0xFF00)     |
           ((val << 8)  & 0xFF0000)   |
           ((val << 24) & 0xFF000000);
}

uint32_t CRC32_Final(CRC32_Context *ctx, int swap_endian) 
{
    uint32_t crc = ctx->crc ^ 0xFFFFFFFF;
    return swap_endian ? swap_uint32(crc) : crc;  // 按需交换字节序
}
