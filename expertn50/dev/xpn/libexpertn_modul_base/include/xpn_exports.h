/*
 * Summary: macros for marking symbols as exportable/importable.
 * Description: macros for marking symbols as exportable/importable.
 *
 * 
 *
 *  original author: Igor Zlatovic <igor@zlatkovic.com> for LIBXML2 under the MIT - LICENCE
 *  changed by Christian Klein <chrikle@berlios.de> for Expert - N 
*
 */

#ifndef __XPN_EXPORTS_H__
#define __XPN_EXPORTS_H__

/**
 * G_MODULE_EXPORT, XPNPUBVAR, XPNCALL
 *
 * Macros which declare an exportable function, an exportable variable and
 * the calling convention used for functions.
 *
 * Please use an extra block for every platform/compiler combination when
 * modifying this, rather than overlong #ifdef lines. This helps
 * readability as well as the fact that different compilers on the same
 * platform might need different definitions.
 */

/**
 * G_MODULE_EXPORT:
 *
 * Macros which declare an exportable function
 */
#define G_MODULE_EXPORT
/**
 * XPNPUBVAR:
 *
 * Macros which declare an exportable variable
 */
#define XPNPUBVAR extern
/**
 * XPNCALL:
 *
 * Macros which declare the called convention for exported functions
 */
#define XPNCALL
/**
 * XPNCDECL:
 *
 * Macro which declares the calling convention for exported functions that 
 * use '...'.
 */
#define XPNCDECL

/** DOC_DISABLE */

/* Windows platform with MS compiler */
#if defined(_WIN32) && defined(_MSC_VER)
  #undef G_MODULE_EXPORT
  #undef XPNPUBVAR
  #undef XPNCALL
  #undef XPNCDECL
  #if defined(IN_LIBXPN) && !defined(LIBXPN_STATIC)
    #define G_MODULE_EXPORT __declspec(dllexport)
    #define XPNPUBVAR __declspec(dllexport)
  #else
    #define G_MODULE_EXPORT
    #if !defined(LIBXPN_STATIC)
      #define XPNPUBVAR __declspec(dllimport) extern
    #else
      #define XPNPUBVAR extern
    #endif
  #endif
  #if defined(LIBXPN_FASTCALL)
    #define XPNCALL __fastcall
  #else
    #define XPNCALL __cdecl
  #endif
  #define XPNCDECL __cdecl
  #if !defined _REENTRANT
    #define _REENTRANT
  #endif
#endif

/* Windows platform with Borland compiler */
#if defined(_WIN32) && defined(__BORLANDC__)
  #undef G_MODULE_EXPORT
  #undef XPNPUBVAR
  #undef XPNCALL
  #undef XPNCDECL
  #if defined(IN_LIBXPN) && !defined(LIBXPN_STATIC)
    #define G_MODULE_EXPORT __declspec(dllexport)
    #define XPNPUBVAR __declspec(dllexport) extern
  #else
    #define G_MODULE_EXPORT
    #if !defined(LIBXPN_STATIC)
      #define XPNPUBVAR __declspec(dllimport) extern
    #else
      #define XPNPUBVAR extern
    #endif
  #endif
  #define XPNCALL __cdecl
  #define XPNCDECL __cdecl
  #if !defined _REENTRANT
    #define _REENTRANT
  #endif
#endif

/* Windows platform with GNU compiler (Mingw) */
#if defined(_WIN32) && defined(__MINGW32__)
  #undef G_MODULE_EXPORT
  #undef XPNPUBVAR
  #undef XPNCALL
  #undef XPNCDECL
  /*
   * if defined(IN_LIBXPN) this raises problems on mingw with msys
   * _imp__XPNFree listed as missing. Try to workaround the problem
   * by also making that declaration when compiling client code.
   */
  #if !defined(LIBXPN_STATIC)
    #define G_MODULE_EXPORT __declspec(dllexport)
    #define XPNPUBVAR __declspec(dllexport)
  #else
    #define G_MODULE_EXPORT
    #if !defined(LIBXPN_STATIC)
      #define XPNPUBVAR __declspec(dllimport) extern
    #else
      #define XPNPUBVAR extern
    #endif
  #endif
  #define XPNCALL __cdecl
  #define XPNCDECL __cdecl
  #if !defined _REENTRANT
    #define _REENTRANT
  #endif
#endif

/* Cygwin platform, GNU compiler */
#if defined(_WIN32) && defined(__CYGWIN__)
  #undef G_MODULE_EXPORT
  #undef XPNPUBVAR
  #undef XPNCALL
  #undef XPNCDECL
  #if defined(IN_LIBXPN) && !defined(LIBXPN_STATIC)
    #define G_MODULE_EXPORT __declspec(dllexport)
    #define XPNPUBVAR __declspec(dllexport)
  #else
    #define G_MODULE_EXPORT
    #if !defined(LIBXPN_STATIC)
      #define XPNPUBVAR __declspec(dllimport) extern
    #else
      #define XPNPUBVAR
    #endif
  #endif
  #define XPNCALL __cdecl
  #define XPNCDECL __cdecl
#endif

/* Compatibility */
#if !defined(LIBXPN_DLL_IMPORT)
#define LIBXPN_DLL_IMPORT XPNPUBVAR
#endif

#endif /* __XPN_EXPORTS_H__ */


