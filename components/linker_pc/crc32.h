#pragma once

void crc32_calctable();
uint32_t crc32_calculate(const char *buffer, size_t length, uint32_t crc);