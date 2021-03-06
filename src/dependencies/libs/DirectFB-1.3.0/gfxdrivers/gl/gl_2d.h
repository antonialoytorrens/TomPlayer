/*
   (c) Copyright 2001-2008  The world wide DirectFB Open Source Community (directfb.org)
   (c) Copyright 2000-2004  Convergence (integrated media) GmbH

   All rights reserved.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __GL_2D_H__
#define __GL_2D_H__


#define GL_SUPPORTED_DRAWINGFLAGS      (DSDRAW_BLEND | \
                                        DSDRAW_SRC_PREMULTIPLY)

#define GL_SUPPORTED_DRAWINGFUNCTIONS  (DFXL_FILLRECTANGLE | \
                                        DFXL_DRAWRECTANGLE | \
                                        DFXL_DRAWLINE      | \
                                        DFXL_FILLTRIANGLE)

#define GL_SUPPORTED_BLITTINGFLAGS     (DSBLIT_BLEND_ALPHACHANNEL | \
                                        DSBLIT_BLEND_COLORALPHA   | \
                                        DSBLIT_COLORIZE           | \
                                        DSBLIT_ROTATE180          | \
                                        DSBLIT_SRC_PREMULTCOLOR)

#define GL_SUPPORTED_BLITTINGFUNCTIONS (DFXL_BLIT | \
                                        DFXL_STRETCHBLIT)


DFBResult glEngineSync   ( void                *drv,
                           void                *dev );

void      glEngineReset  ( void                *drv,
                           void                *dev );

void      glEmitCommands ( void                *drv,
                           void                *dev );

void      glCheckState   ( void                *drv,
                           void                *dev,
                           CardState           *state,
                           DFBAccelerationMask  accel );

void      glSetState     ( void                *drv,
                           void                *dev,
                           GraphicsDeviceFuncs *funcs,
                           CardState           *state,
                           DFBAccelerationMask  accel );

bool      glFillRectangle( void                *drv,
                           void                *dev,
                           DFBRectangle        *rect );

bool      glDrawRectangle( void                *drv,
                           void                *dev,
                           DFBRectangle        *rect );

bool      glDrawLine     ( void                *drv,
                           void                *dev,
                           DFBRegion           *line );

bool      glFillTriangle ( void                *drv,
                           void                *dev,
                           DFBTriangle         *tri );

bool      glBlit         ( void                *drv,
                           void                *dev,
                           DFBRectangle        *srect,
                           int                  dx,
                           int                  dy );

bool      glStretchBlit  ( void                *drv,
                           void                *dev,
                           DFBRectangle        *srect,
                           DFBRectangle        *drect );

#endif

