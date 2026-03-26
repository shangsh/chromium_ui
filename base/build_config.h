#ifndef __build_config_h__
#define __build_config_h__

#pragma once

// OS detection
#if defined(_WIN32)
#define OS_WIN              1
#define OS_MAC              0
#elif defined(__APPLE__)
#define OS_WIN              0
#define OS_MAC              1
#else
#define OS_WIN              0
#define OS_MAC              0
#endif

// Compiler detection
#if defined(_MSC_VER)
#define COMPILER_MSVC       1
#define COMPILER_CLANG      0
#define COMPILER_GCC        0
#elif defined(__clang__)
#define COMPILER_MSVC       0
#define COMPILER_CLANG      1
#define COMPILER_GCC        0
#else
#define COMPILER_MSVC       0
#define COMPILER_CLANG      0
#define COMPILER_GCC        1
#endif

// Architecture detection
#if defined(_M_X64) || defined(__x86_64__)
#define ARCH_CPU_X86_FAMILY 1
#define ARCH_CPU_X86_64     1
#define ARCH_CPU_64_BITS    1
#define ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(_M_IX86) || defined(__i386__)
#define ARCH_CPU_X86_FAMILY 1
#define ARCH_CPU_X86        1
#define ARCH_CPU_32_BITS    1
#define ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(__arm64__) || defined(_M_ARM64)
#define ARCH_CPU_ARM_FAMILY 1
#define ARCH_CPU_ARM64      1
#define ARCH_CPU_64_BITS    1
#define ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(__arm__) || defined(_M_ARM)
#define ARCH_CPU_ARM_FAMILY 1
#define ARCH_CPU_ARM        1
#define ARCH_CPU_32_BITS    1
#define ARCH_CPU_LITTLE_ENDIAN 1
#endif

// Platform macros for this project
#if OS_MAC
#define PLATFORM_MACOS      1
#define PLATFORM_WINDOWS     0
#else
#define PLATFORM_MACOS      0
#define PLATFORM_WINDOWS    1
#endif

#endif //__build_config_h__
