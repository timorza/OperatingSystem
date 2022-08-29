#ifndef TYPES_H
#define TYPES_H
#pragma once

#define uint32_t unsigned int
#define uint16_t unsigned short
#define uint8_t unsigned char

#define low_16(address) (uint16_t)((address) & 0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)

#define STR (char*)

#endif
