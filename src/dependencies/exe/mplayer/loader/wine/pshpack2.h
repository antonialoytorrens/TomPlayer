#ifndef WINE_PSHPACK_H
#define WINE_PSHPACK_H 2

#if defined(__GNUC__) || defined(__SUNPRO_C) || defined(__SUNPRO_CC)
//#pragma pack(2)
#elif !defined(RC_INVOKED)
#error "2 as alignment isn't supported by the compiler"
#endif /* defined(__GNUC__) || defined(__SUNPRO_CC) ; !defined(RC_INVOKED) */

#else /* WINE_PSHPACK_H */
#error "Nested pushing of alignment isn't supported by the compiler"
#endif /* WINE_PSHPACK_H */
