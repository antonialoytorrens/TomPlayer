/* include/barcelona/gotype.h
 *
 * Declarations for different TomTom GO types.
 *
 * Copyright (C) 2004,2005,2006 TomTom BV <http://www.tomtom.com/>
 * Authors: Dimitry Andric <dimitry.andric@tomtom.com>
 *          Jeroen Taverne <jeroen.taverne@tomtom.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __INCLUDE_BARCELONA_GOBOARD_H
#define __INCLUDE_BARCELONA_GOBOARD_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* GO id pin bits */
#define GOBOARD_ID0_LO			0
#define GOBOARD_ID0_HI			1
#define GOBOARD_ID1_LO			0
#define GOBOARD_ID1_HI			2
#define GOBOARD_ID2_LO			0
#define GOBOARD_ID2_HI			8
#define GOBOARD_ID3_LO			0 
#define GOBOARD_ID3_HI			32 
#define GOBOARD_ID4_LO			0 
#define GOBOARD_ID4_HI			64 

/* GO CPU type bits */
#define GOBOARD_S3C2442			128
#define GOBOARD_S3C2440			0
#define GOBOARD_S3C2410			4
#define GOBOARD_S3C2412			512
#define GOBOARD_S3C2443			1024

/* GO screen type bits */
#define GOBOARD_NEC_TFT			0
#define GOBOARD_SAMSUNG_TFT		16

/* Combined GO board identifiers */
#define GOBOARD_CLASSIC		(                 GOBOARD_ID1_HI | GOBOARD_ID0_HI | GOBOARD_S3C2410 | GOBOARD_NEC_TFT)
#define GOBOARD_M100		(                 GOBOARD_ID1_LO | GOBOARD_ID0_HI | GOBOARD_S3C2410 | GOBOARD_NEC_TFT)

#define GOBOARD_M300		(                 GOBOARD_ID1_HI | GOBOARD_ID0_HI | GOBOARD_S3C2440 | GOBOARD_NEC_TFT)
#define GOBOARD_M500		(                 GOBOARD_ID1_HI | GOBOARD_ID0_LO | GOBOARD_S3C2440 | GOBOARD_NEC_TFT)
#define GOBOARD_ATLANTA		(GOBOARD_ID2_HI | GOBOARD_ID1_LO | GOBOARD_ID0_HI | GOBOARD_S3C2440 | GOBOARD_SAMSUNG_TFT)
#define GOBOARD_ABERDEEN	(GOBOARD_ID2_HI | GOBOARD_ID1_HI | GOBOARD_ID0_HI | GOBOARD_S3C2440 | GOBOARD_SAMSUNG_TFT)
#define GOBOARD_NEWCASTLE	(GOBOARD_ID2_HI | GOBOARD_ID1_HI | GOBOARD_ID0_LO | GOBOARD_S3C2440 | GOBOARD_SAMSUNG_TFT)

#define GOBOARD_VALENCIA	(                 GOBOARD_ID1_LO | GOBOARD_ID0_LO | GOBOARD_S3C2442 | GOBOARD_SAMSUNG_TFT)
#define GOBOARD_MURCIA		(                 GOBOARD_ID1_LO | GOBOARD_ID0_HI | GOBOARD_S3C2442 | GOBOARD_SAMSUNG_TFT)

#define GOBOARD_EDINBURGH	(GOBOARD_ID2_LO | GOBOARD_ID1_LO | GOBOARD_ID0_LO | GOBOARD_S3C2412 | GOBOARD_SAMSUNG_TFT)
#define GOBOARD_COLOGNE		(GOBOARD_ID2_LO | GOBOARD_ID1_LO | GOBOARD_ID0_HI | GOBOARD_S3C2412 | GOBOARD_SAMSUNG_TFT)
#define GOBOARD_CORK_LTC	(GOBOARD_ID2_LO | GOBOARD_ID1_HI | GOBOARD_ID0_HI | GOBOARD_S3C2412 | GOBOARD_SAMSUNG_TFT)
#define GOBOARD_LIMERICK	(GOBOARD_ID2_HI | GOBOARD_ID1_LO | GOBOARD_ID0_LO | GOBOARD_S3C2412 | GOBOARD_SAMSUNG_TFT)
#define GOBOARD_CASABLANCA_GL	(GOBOARD_ID2_HI | GOBOARD_ID1_LO | GOBOARD_ID0_HI | GOBOARD_S3C2412 | GOBOARD_SAMSUNG_TFT)
#define GOBOARD_CASABLANCA_SIRF	(GOBOARD_ID2_HI | GOBOARD_ID1_HI | GOBOARD_ID0_LO | GOBOARD_S3C2412 | GOBOARD_SAMSUNG_TFT)
#define GOBOARD_CORKII		(GOBOARD_ID2_HI | GOBOARD_ID1_HI | GOBOARD_ID0_HI | GOBOARD_S3C2412 | GOBOARD_SAMSUNG_TFT)
#define GOBOARD_KNOCK		(GOBOARD_ID2_LO | GOBOARD_ID1_HI | GOBOARD_ID0_LO | GOBOARD_S3C2412 | GOBOARD_SAMSUNG_TFT)

#define GOBOARD_MILAN		(                 GOBOARD_ID1_HI | GOBOARD_ID0_HI | GOBOARD_S3C2443 | GOBOARD_SAMSUNG_TFT)

/* Atlanta board identifiers */
#define GOBOARD_BILBAO		(GOBOARD_ATLANTA | GOBOARD_ID4_HI | GOBOARD_ID3_HI)
#define GOBOARD_GLASGOW		(GOBOARD_ATLANTA | GOBOARD_ID4_LO | GOBOARD_ID3_HI)
#define GOBOARD_LISBON		(GOBOARD_ATLANTA | GOBOARD_ID4_HI | GOBOARD_ID3_LO)

/* Cork LTC board identifiers	*/
#define GOBOARD_CORK_LTC_Z1Z2	(GOBOARD_CORK_LTC | GOBOARD_ID3_LO )	/* With 1GB/512 MB MoviNAND */
#define GOBOARD_CORK_LTC_Z3	(GOBOARD_CORK_LTC | GOBOARD_ID3_HI )	/* WithOUT MoviNAND */

/* Cork II board identifiers	*/
#define GOBOARD_CORKII_Z1Z2	(GOBOARD_CORKII | GOBOARD_ID3_LO )	/* With 1GB / 512 MB MoviNAND */
#define GOBOARD_CORKII_Z3	(GOBOARD_CORKII | GOBOARD_ID3_HI )	/* WithOUT MoviNAND */

/* Limerick board identifiers */
#define GOBOARD_LIMERICK_L1L2	(GOBOARD_LIMERICK | GOBOARD_ID3_LO | GOBOARD_ID4_LO )	/* With 1GB / 512 MB MoviNAND */
#define GOBOARD_LIMERICK_L3	(GOBOARD_LIMERICK | GOBOARD_ID3_HI | GOBOARD_ID4_LO )	/* WithOUT MoviNAND */
#define GOBOARD_LIMERICK_L4	(GOBOARD_LIMERICK | GOBOARD_ID3_LO | GOBOARD_ID4_HI )	/* Like L1 but with 64 MB SDRAM */
#define GOBOARD_LIMERICK_L5	(GOBOARD_LIMERICK | GOBOARD_ID3_HI | GOBOARD_ID4_HI )	/* Like L3 but with 64 MB SDRAM */

/* Milan board identifiers */
#define GOBOARD_MILAN_M2	(GOBOARD_MILAN | GOBOARD_ID4_LO | GOBOARD_ID3_LO)
#define GOBOARD_MILAN_M4	(GOBOARD_MILAN | GOBOARD_ID4_LO | GOBOARD_ID3_HI)
#define GOBOARD_MILAN_M6	(GOBOARD_MILAN | GOBOARD_ID4_HI | GOBOARD_ID3_LO)
#define GOBOARD_MILAN_M8	(GOBOARD_MILAN | GOBOARD_ID4_HI | GOBOARD_ID3_HI)

/* Casablanca board identifiers */
/* 
 * NOTE: Casablanca can NOT identify movinand size using gpio
 * according to schematic, GPB8 / TYPE_SUB_ID0 / GOBOARD_ID3_HI should always be '0'
 */
#define GOBOARD_CASABLANCA_SIRF_Y1Y3	(GOBOARD_CASABLANCA_SIRF | GOBOARD_ID3_LO)
#define GOBOARD_CASABLANCA_GL_Y2Y4	(GOBOARD_CASABLANCA_GL   | GOBOARD_ID3_LO)
#define GOBOARD_CASABLANCA_SIRF_UNKNOWN	(GOBOARD_CASABLANCA_SIRF | GOBOARD_ID3_HI)
#define GOBOARD_CASABLANCA_GL_UNKNOWN	(GOBOARD_CASABLANCA_GL   | GOBOARD_ID3_HI)

/* Knock board identifiers */
#define GOBOARD_KNOCK_MOVINAND		(GOBOARD_KNOCK | GOBOARD_ID3_LO)
#define GOBOARD_KNOCK_NO_MOVINAND	(GOBOARD_KNOCK | GOBOARD_ID3_HI)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCLUDE_BARCELONA_GOBOARD_H */

/* EOF */
