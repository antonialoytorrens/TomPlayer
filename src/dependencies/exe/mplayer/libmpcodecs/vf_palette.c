#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"

#include "img_format.h"
#include "mp_image.h"
#include "vf.h"

#include "libswscale/rgb2rgb.h"

//===========================================================================//

// commented out 16 and 15 bit output support, because the conversion
// routines are incorrrect.  they assume the palette to be of the same
// depth as the output, which is incorrect. --Joey

static unsigned int bgr_list[]={
    IMGFMT_BGR32,
    IMGFMT_BGR24,
//    IMGFMT_BGR16,
//    IMGFMT_BGR15,
    0
};
static unsigned int rgb_list[]={
    IMGFMT_RGB32,
    IMGFMT_RGB24,
//    IMGFMT_RGB16,
//    IMGFMT_RGB15,
    0
};

static unsigned int gray_pal[256];

static unsigned int find_best(struct vf_instance_s* vf, unsigned int fmt){
    unsigned int best=0;
    int ret;
    unsigned int* p;
    if(fmt==IMGFMT_BGR8) p=bgr_list;
    else if(fmt==IMGFMT_RGB8) p=rgb_list;
    else return 0;
    while(*p){
	ret=vf->next->query_format(vf->next,*p);
	mp_msg(MSGT_VFILTER,MSGL_DBG2,"[%s] query(%s) -> %d\n",vf->info->name,vo_format_name(*p),ret&3);
	if(ret&VFCAP_CSP_SUPPORTED_BY_HW){ best=*p; break;} // no conversion -> bingo!
	if(ret&VFCAP_CSP_SUPPORTED && !best) best=*p; // best with conversion
	++p;
    }
    return best;
}

//===========================================================================//

struct vf_priv_s {
    unsigned int fmt;
    int pal_msg;
};

static int config(struct vf_instance_s* vf,
        int width, int height, int d_width, int d_height,
	unsigned int flags, unsigned int outfmt){
    if (!vf->priv->fmt)
	vf->priv->fmt=find_best(vf,outfmt);
    if(!vf->priv->fmt){
	// no matching fmt, so force one...
	if(outfmt==IMGFMT_RGB8) vf->priv->fmt=IMGFMT_RGB32;
	else if(outfmt==IMGFMT_BGR8) vf->priv->fmt=IMGFMT_BGR32;
	else return 0;
    }
    return vf_next_config(vf,width,height,d_width,d_height,flags,vf->priv->fmt);
}

static int put_image(struct vf_instance_s* vf, mp_image_t *mpi, double pts){
    mp_image_t *dmpi;
    
    // hope we'll get DR buffer:
    dmpi=vf_get_image(vf->next,vf->priv->fmt,
	MP_IMGTYPE_TEMP, MP_IMGFLAG_ACCEPT_STRIDE,
	mpi->w, mpi->h);

    if (!mpi->planes[1])
    {
	if(!vf->priv->pal_msg){
	    mp_msg(MSGT_VFILTER,MSGL_V,"[%s] no palette given, assuming builtin grayscale one\n",vf->info->name);
	    vf->priv->pal_msg=1;
	}
	mpi->planes[1] = (unsigned char*)gray_pal;
    }

    if(mpi->w==mpi->stride[0] && dmpi->w*(dmpi->bpp>>3)==dmpi->stride[0]){
	// no stride conversion needed
	switch(IMGFMT_RGB_DEPTH(dmpi->imgfmt)){
	case 15:
	    if (IMGFMT_IS_BGR(dmpi->imgfmt))
		palette8tobgr15(mpi->planes[0],dmpi->planes[0],mpi->h*mpi->w,mpi->planes[1]);
	    else
		palette8torgb15(mpi->planes[0],dmpi->planes[0],mpi->h*mpi->w,mpi->planes[1]);
	    break;
	case 16:
	    if (IMGFMT_IS_BGR(dmpi->imgfmt))
		palette8tobgr16(mpi->planes[0],dmpi->planes[0],mpi->h*mpi->w,mpi->planes[1]);
	    else
		palette8torgb16(mpi->planes[0],dmpi->planes[0],mpi->h*mpi->w,mpi->planes[1]);
	    break;
	case 24:
	    if (IMGFMT_IS_BGR(dmpi->imgfmt))
		palette8tobgr24(mpi->planes[0],dmpi->planes[0],mpi->h*mpi->w,mpi->planes[1]);
	    else
		palette8torgb24(mpi->planes[0],dmpi->planes[0],mpi->h*mpi->w,mpi->planes[1]);
	    break;
	case 32:
	    if (IMGFMT_IS_BGR(dmpi->imgfmt))
		palette8tobgr32(mpi->planes[0],dmpi->planes[0],mpi->h*mpi->w,mpi->planes[1]);
	    else
		palette8torgb32(mpi->planes[0],dmpi->planes[0],mpi->h*mpi->w,mpi->planes[1]);
	    break;
	}
    } else {
	int y;
	for(y=0;y<mpi->h;y++){
	    unsigned char* src=mpi->planes[0]+y*mpi->stride[0];
	    unsigned char* dst=dmpi->planes[0]+y*dmpi->stride[0];
	    switch(IMGFMT_RGB_DEPTH(dmpi->imgfmt)){
	    case 15:
		if (IMGFMT_IS_BGR(dmpi->imgfmt))
		    palette8tobgr15(src,dst,mpi->w,mpi->planes[1]);
		else
		    palette8torgb15(src,dst,mpi->w,mpi->planes[1]);
		break;
	    case 16:
		if (IMGFMT_IS_BGR(dmpi->imgfmt))
		    palette8tobgr16(src,dst,mpi->w,mpi->planes[1]);
		else
		    palette8torgb16(src,dst,mpi->w,mpi->planes[1]);
		break;
	    case 24:
		if (IMGFMT_IS_BGR(dmpi->imgfmt))
		    palette8tobgr24(src,dst,mpi->w,mpi->planes[1]);
		else
		    palette8torgb24(src,dst,mpi->w,mpi->planes[1]);
		break;
	    case 32:
		if (IMGFMT_IS_BGR(dmpi->imgfmt))
		    palette8tobgr32(src,dst,mpi->w,mpi->planes[1]);
		else
		    palette8torgb32(src,dst,mpi->w,mpi->planes[1]);
		break;
	    }
	}
    }
    
    return vf_next_put_image(vf,dmpi, pts);
}

//===========================================================================//

static int query_format(struct vf_instance_s* vf, unsigned int fmt){
    int best=find_best(vf,fmt);
    if(!best) return 0; // no match
    return vf->next->query_format(vf->next,best);
}

static void uninit(vf_instance_t *vf) {
  free(vf->priv);
}

static int open(vf_instance_t *vf, char* args){
    unsigned int i;
    vf->config=config;
    vf->uninit=uninit;
    vf->put_image=put_image;
    vf->query_format=query_format;
    vf->priv=malloc(sizeof(struct vf_priv_s));
    memset(vf->priv, 0, sizeof(struct vf_priv_s));
    for(i=0;i<256;i++) gray_pal[i]=0x01010101*i;
    if (args)
    {
	if (!strcasecmp(args,"rgb15")) vf->priv->fmt=IMGFMT_RGB15; else
	if (!strcasecmp(args,"rgb16")) vf->priv->fmt=IMGFMT_RGB16; else
	if (!strcasecmp(args,"rgb24")) vf->priv->fmt=IMGFMT_RGB24; else
	if (!strcasecmp(args,"rgb32")) vf->priv->fmt=IMGFMT_RGB32; else
	if (!strcasecmp(args,"bgr15")) vf->priv->fmt=IMGFMT_BGR15; else
	if (!strcasecmp(args,"bgr16")) vf->priv->fmt=IMGFMT_BGR16; else
	if (!strcasecmp(args,"bgr24")) vf->priv->fmt=IMGFMT_BGR24; else
	if (!strcasecmp(args,"bgr32")) vf->priv->fmt=IMGFMT_BGR32; else
	{
	    mp_msg(MSGT_VFILTER, MSGL_WARN, MSGTR_MPCODECS_UnknownFormatName, args);
	    return(0);
	}
    }
    return 1;
}

const vf_info_t vf_info_palette = {
    "8bpp indexed (using palette) -> BGR 15/16/24/32 conversion",
    "palette",
    "A'rpi & Alex",
    "",
    open,
    NULL
};

//===========================================================================//
