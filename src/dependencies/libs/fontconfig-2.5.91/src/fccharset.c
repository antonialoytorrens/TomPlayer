/*
 * $RCSId: xc/lib/fontconfig/src/fccharset.c,v 1.18 2002/08/22 07:36:44 keithp Exp $
 *
 * Copyright © 2001 Keith Packard
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Keith Packard not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Keith Packard makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "fcint.h"
#include <stdlib.h>

/* #define CHECK */

FcCharSet *
FcCharSetCreate (void)
{
    FcCharSet	*fcs;

    fcs = (FcCharSet *) malloc (sizeof (FcCharSet));
    if (!fcs)
	return 0;
    FcMemAlloc (FC_MEM_CHARSET, sizeof (FcCharSet));
    fcs->ref = 1;
    fcs->num = 0;
    fcs->leaves_offset = 0;
    fcs->numbers_offset = 0;
    return fcs;
}

FcCharSet *
FcCharSetNew (void)
{
    return FcCharSetCreate ();
}

void
FcCharSetDestroy (FcCharSet *fcs)
{
    int i;
    
    if (fcs->ref == FC_REF_CONSTANT)
    {
	FcCacheObjectDereference (fcs);
	return;
    }
    if (--fcs->ref > 0)
	return;
    for (i = 0; i < fcs->num; i++)
    {
	FcMemFree (FC_MEM_CHARLEAF, sizeof (FcCharLeaf));
	free (FcCharSetLeaf (fcs, i));
    }
    if (fcs->num)
    {
	FcMemFree (FC_MEM_CHARSET, fcs->num * sizeof (intptr_t));
	free (FcCharSetLeaves (fcs));
    }
    if (fcs->num)
    {
	FcMemFree (FC_MEM_CHARSET, fcs->num * sizeof (FcChar16));
	free (FcCharSetNumbers (fcs));
    }
    FcMemFree (FC_MEM_CHARSET, sizeof (FcCharSet));
    free (fcs);
}

/*
 * Locate the leaf containing the specified char, return
 * its index if it exists, otherwise return negative of
 * the (position + 1) where it should be inserted
 */

static int
FcCharSetFindLeafPos (const FcCharSet *fcs, FcChar32 ucs4)
{
    FcChar16		*numbers = FcCharSetNumbers(fcs);
    FcChar16		page;
    int			low = 0;
    int			high = fcs->num - 1;

    if (!numbers)
	return -1;
    ucs4 >>= 8;
    while (low <= high)
    {
	int mid = (low + high) >> 1;
	page = numbers[mid];
	if (page == ucs4)
	    return mid;
	if (page < ucs4)
	    low = mid + 1;
	else
	    high = mid - 1;
    }
    if (high < 0 || (high < fcs->num && numbers[high] < ucs4))
	high++;
    return -(high + 1);
}

static FcCharLeaf *
FcCharSetFindLeaf (const FcCharSet *fcs, FcChar32 ucs4)
{
    int	pos = FcCharSetFindLeafPos (fcs, ucs4);
    if (pos >= 0)
	return FcCharSetLeaf(fcs, pos);
    return 0;
}

static FcBool
FcCharSetPutLeaf (FcCharSet	*fcs, 
		  FcChar32	ucs4,
		  FcCharLeaf	*leaf, 
		  int		pos)
{
    intptr_t	*leaves = FcCharSetLeaves (fcs);
    FcChar16	*numbers = FcCharSetNumbers (fcs);

    ucs4 >>= 8;
    if (ucs4 >= 0x10000)
	return FcFalse;
    if (!fcs->num)
	leaves = malloc (sizeof (*leaves));
    else
    {
	intptr_t    *new_leaves = realloc (leaves, (fcs->num + 1) * 
					   sizeof (*leaves));
	intptr_t    distance = (intptr_t) new_leaves - (intptr_t) leaves;
	
	if (new_leaves && distance)
	{
	    int i;

	    for (i = 0; i < fcs->num; i++)
		new_leaves[i] -= distance;
	}
	leaves = new_leaves;
    }
    if (!leaves)
	return FcFalse;
    
    if (fcs->num)
	FcMemFree (FC_MEM_CHARSET, fcs->num * sizeof (intptr_t));
    FcMemAlloc (FC_MEM_CHARSET, (fcs->num + 1) * sizeof (intptr_t));
    fcs->leaves_offset = FcPtrToOffset (fcs, leaves);
    
    if (!fcs->num)
	numbers = malloc (sizeof (FcChar16));
    else
	numbers = realloc (numbers, (fcs->num + 1) * sizeof (FcChar16));
    if (!numbers)
	return FcFalse;
    
    if (fcs->num)
	FcMemFree (FC_MEM_CHARSET, fcs->num * sizeof (FcChar16));
    FcMemAlloc (FC_MEM_CHARSET, (fcs->num + 1) * sizeof (FcChar16));
    fcs->numbers_offset = FcPtrToOffset (fcs, numbers);
    
    memmove (leaves + pos + 1, leaves + pos, 
	     (fcs->num - pos) * sizeof (*leaves));
    memmove (numbers + pos + 1, numbers + pos,
	     (fcs->num - pos) * sizeof (*numbers));
    numbers[pos] = (FcChar16) ucs4;
    leaves[pos] = FcPtrToOffset (leaves, leaf);
    fcs->num++;
    return FcTrue;
}

/*
 * Locate the leaf containing the specified char, creating it
 * if desired
 */

FcCharLeaf *
FcCharSetFindLeafCreate (FcCharSet *fcs, FcChar32 ucs4)
{
    int			pos;
    FcCharLeaf		*leaf;

    pos = FcCharSetFindLeafPos (fcs, ucs4);
    if (pos >= 0)
	return FcCharSetLeaf(fcs, pos);
    
    leaf = calloc (1, sizeof (FcCharLeaf));
    if (!leaf)
	return 0;
    
    pos = -pos - 1;
    if (!FcCharSetPutLeaf (fcs, ucs4, leaf, pos))
    {
	free (leaf);
	return 0;
    }
    FcMemAlloc (FC_MEM_CHARLEAF, sizeof (FcCharLeaf));
    return leaf;
}

static FcBool
FcCharSetInsertLeaf (FcCharSet *fcs, FcChar32 ucs4, FcCharLeaf *leaf)
{
    int		    pos;

    pos = FcCharSetFindLeafPos (fcs, ucs4);
    if (pos >= 0)
    {
	FcMemFree (FC_MEM_CHARLEAF, sizeof (FcCharLeaf));
	free (FcCharSetLeaf (fcs, pos));
	FcCharSetLeaves(fcs)[pos] = FcPtrToOffset (FcCharSetLeaves(fcs),
						   leaf);
	return FcTrue;
    }
    pos = -pos - 1;
    return FcCharSetPutLeaf (fcs, ucs4, leaf, pos);
}

FcBool
FcCharSetAddChar (FcCharSet *fcs, FcChar32 ucs4)
{
    FcCharLeaf	*leaf;
    FcChar32	*b;
    
    if (fcs->ref == FC_REF_CONSTANT)
	return FcFalse;
    leaf = FcCharSetFindLeafCreate (fcs, ucs4);
    if (!leaf)
	return FcFalse;
    b = &leaf->map[(ucs4 & 0xff) >> 5];
    *b |= (1 << (ucs4 & 0x1f));
    return FcTrue;
}

/*
 * An iterator for the leaves of a charset
 */

typedef struct _fcCharSetIter {
    FcCharLeaf	    *leaf;
    FcChar32	    ucs4;
    int		    pos;
} FcCharSetIter;

/*
 * Set iter->leaf to the leaf containing iter->ucs4 or higher
 */

static void
FcCharSetIterSet (const FcCharSet *fcs, FcCharSetIter *iter)
{
    int		    pos = FcCharSetFindLeafPos (fcs, iter->ucs4);

    if (pos < 0)
    {
	pos = -pos - 1;
	if (pos == fcs->num)
	{
	    iter->ucs4 = ~0;
	    iter->leaf = 0;
	    return;
	}
        iter->ucs4 = (FcChar32) FcCharSetNumbers(fcs)[pos] << 8;
    }
    iter->leaf = FcCharSetLeaf(fcs, pos);
    iter->pos = pos;
}

static void
FcCharSetIterNext (const FcCharSet *fcs, FcCharSetIter *iter)
{
    int	pos = iter->pos + 1;
    if (pos >= fcs->num)
    {
	iter->ucs4 = ~0;
	iter->leaf = 0;
    }
    else
    {
	iter->ucs4 = (FcChar32) FcCharSetNumbers(fcs)[pos] << 8;
	iter->leaf = FcCharSetLeaf(fcs, pos);
	iter->pos = pos;
    }
}


static void
FcCharSetIterStart (const FcCharSet *fcs, FcCharSetIter *iter)
{
    iter->ucs4 = 0;
    iter->pos = 0;
    FcCharSetIterSet (fcs, iter);
}

FcCharSet *
FcCharSetCopy (FcCharSet *src)
{
    if (src->ref != FC_REF_CONSTANT)
	src->ref++;
    else
	FcCacheObjectReference (src);
    return src;
}

FcBool
FcCharSetEqual (const FcCharSet *a, const FcCharSet *b)
{
    FcCharSetIter   ai, bi;
    int		    i;
    
    if (a == b)
	return FcTrue;
    for (FcCharSetIterStart (a, &ai), FcCharSetIterStart (b, &bi);
	 ai.leaf && bi.leaf;
	 FcCharSetIterNext (a, &ai), FcCharSetIterNext (b, &bi))
    {
	if (ai.ucs4 != bi.ucs4)
	    return FcFalse;
	for (i = 0; i < 256/32; i++)
	    if (ai.leaf->map[i] != bi.leaf->map[i])
		return FcFalse;
    }
    return ai.leaf == bi.leaf;
}

static FcBool
FcCharSetAddLeaf (FcCharSet	*fcs,
		  FcChar32	ucs4,
		  FcCharLeaf	*leaf)
{
    FcCharLeaf   *new = FcCharSetFindLeafCreate (fcs, ucs4);
    if (!new)
	return FcFalse;
    *new = *leaf;
    return FcTrue;
}

static FcCharSet *
FcCharSetOperate (const FcCharSet   *a,
		  const FcCharSet   *b,
		  FcBool	    (*overlap) (FcCharLeaf	    *result,
						const FcCharLeaf    *al,
						const FcCharLeaf    *bl),
		  FcBool	aonly,
		  FcBool	bonly)
{
    FcCharSet	    *fcs;
    FcCharSetIter   ai, bi;

    fcs = FcCharSetCreate ();
    if (!fcs)
	goto bail0;
    FcCharSetIterStart (a, &ai);
    FcCharSetIterStart (b, &bi);
    while ((ai.leaf || (bonly && bi.leaf)) && (bi.leaf || (aonly && ai.leaf)))
    {
	if (ai.ucs4 < bi.ucs4)
	{
	    if (aonly)
	    {
		if (!FcCharSetAddLeaf (fcs, ai.ucs4, ai.leaf))
		    goto bail1;
		FcCharSetIterNext (a, &ai);
	    }
	    else
	    {
		ai.ucs4 = bi.ucs4;
		FcCharSetIterSet (a, &ai);
	    }
	}
	else if (bi.ucs4 < ai.ucs4 )
	{
	    if (bonly)
	    {
		if (!FcCharSetAddLeaf (fcs, bi.ucs4, bi.leaf))
		    goto bail1;
		FcCharSetIterNext (b, &bi);
	    }
	    else
	    {
		bi.ucs4 = ai.ucs4;
		FcCharSetIterSet (b, &bi);
	    }
	}
	else
	{
	    FcCharLeaf  leaf;

	    if ((*overlap) (&leaf, ai.leaf, bi.leaf))
	    {
		if (!FcCharSetAddLeaf (fcs, ai.ucs4, &leaf))
		    goto bail1;
	    }
	    FcCharSetIterNext (a, &ai);
	    FcCharSetIterNext (b, &bi);
	}
    }
    return fcs;
bail1:
    FcCharSetDestroy (fcs);
bail0:
    return 0;
}

static FcBool
FcCharSetIntersectLeaf (FcCharLeaf *result,
			const FcCharLeaf *al,
			const FcCharLeaf *bl)
{
    int	    i;
    FcBool  nonempty = FcFalse;

    for (i = 0; i < 256/32; i++)
	if ((result->map[i] = al->map[i] & bl->map[i]))
	    nonempty = FcTrue;
    return nonempty;
}

FcCharSet *
FcCharSetIntersect (const FcCharSet *a, const FcCharSet *b)
{
    return FcCharSetOperate (a, b, FcCharSetIntersectLeaf, FcFalse, FcFalse);
}

static FcBool
FcCharSetUnionLeaf (FcCharLeaf *result,
		    const FcCharLeaf *al,
		    const FcCharLeaf *bl)
{
    int	i;

    for (i = 0; i < 256/32; i++)
	result->map[i] = al->map[i] | bl->map[i];
    return FcTrue;
}

FcCharSet *
FcCharSetUnion (const FcCharSet *a, const FcCharSet *b)
{
    return FcCharSetOperate (a, b, FcCharSetUnionLeaf, FcTrue, FcTrue);
}

static FcBool
FcCharSetSubtractLeaf (FcCharLeaf *result,
		       const FcCharLeaf *al,
		       const FcCharLeaf *bl)
{
    int	    i;
    FcBool  nonempty = FcFalse;

    for (i = 0; i < 256/32; i++)
	if ((result->map[i] = al->map[i] & ~bl->map[i]))
	    nonempty = FcTrue;
    return nonempty;
}

FcCharSet *
FcCharSetSubtract (const FcCharSet *a, const FcCharSet *b)
{
    return FcCharSetOperate (a, b, FcCharSetSubtractLeaf, FcTrue, FcFalse);
}

FcBool
FcCharSetHasChar (const FcCharSet *fcs, FcChar32 ucs4)
{
    FcCharLeaf	*leaf = FcCharSetFindLeaf (fcs, ucs4);
    if (!leaf)
	return FcFalse;
    return (leaf->map[(ucs4 & 0xff) >> 5] & (1 << (ucs4 & 0x1f))) != 0;
}

static FcChar32
FcCharSetPopCount (FcChar32 c1)
{
    /* hackmem 169 */
    FcChar32	c2 = (c1 >> 1) & 033333333333;
    c2 = c1 - c2 - ((c2 >> 1) & 033333333333);
    return (((c2 + (c2 >> 3)) & 030707070707) % 077);
}

FcChar32
FcCharSetIntersectCount (const FcCharSet *a, const FcCharSet *b)
{
    FcCharSetIter   ai, bi;
    FcChar32	    count = 0;
    
    FcCharSetIterStart (a, &ai);
    FcCharSetIterStart (b, &bi);
    while (ai.leaf && bi.leaf)
    {
	if (ai.ucs4 == bi.ucs4)
	{
	    FcChar32	*am = ai.leaf->map;
	    FcChar32	*bm = bi.leaf->map;
	    int		i = 256/32;
	    while (i--)
		count += FcCharSetPopCount (*am++ & *bm++);
	    FcCharSetIterNext (a, &ai);
	} 
	else if (ai.ucs4 < bi.ucs4)
	{
	    ai.ucs4 = bi.ucs4;
	    FcCharSetIterSet (a, &ai);
	}
	if (bi.ucs4 < ai.ucs4)
	{
	    bi.ucs4 = ai.ucs4;
	    FcCharSetIterSet (b, &bi);
	}
    }
    return count;
}

FcChar32
FcCharSetCount (const FcCharSet *a)
{
    FcCharSetIter   ai;
    FcChar32	    count = 0;
    
    for (FcCharSetIterStart (a, &ai); ai.leaf; FcCharSetIterNext (a, &ai))
    {
	int		    i = 256/32;
	FcChar32	    *am = ai.leaf->map;

	while (i--)
	    count += FcCharSetPopCount (*am++);
    }
    return count;
}

FcChar32
FcCharSetSubtractCount (const FcCharSet *a, const FcCharSet *b)
{
    FcCharSetIter   ai, bi;
    FcChar32	    count = 0;
    
    FcCharSetIterStart (a, &ai);
    FcCharSetIterStart (b, &bi);
    while (ai.leaf)
    {
	if (ai.ucs4 <= bi.ucs4)
	{
	    FcChar32	*am = ai.leaf->map;
	    int		i = 256/32;
	    if (ai.ucs4 == bi.ucs4)
	    {
		FcChar32	*bm = bi.leaf->map;
		while (i--)
		    count += FcCharSetPopCount (*am++ & ~*bm++);
	    }
	    else
	    {
		while (i--)
		    count += FcCharSetPopCount (*am++);
	    }
	    FcCharSetIterNext (a, &ai);
	}
	else if (bi.leaf)
	{
	    bi.ucs4 = ai.ucs4;
	    FcCharSetIterSet (b, &bi);
	}
    }
    return count;
}

/*
 * return FcTrue iff a is a subset of b
 */
FcBool
FcCharSetIsSubset (const FcCharSet *a, const FcCharSet *b)
{
    int		ai, bi;
    FcChar16	an, bn;
    
    if (a == b) return FcTrue;
    bi = 0;
    ai = 0;
    while (ai < a->num && bi < b->num)
    {
	an = FcCharSetNumbers(a)[ai];
	bn = FcCharSetNumbers(b)[bi];
	/*
	 * Check matching pages
	 */
	if (an == bn)
	{
	    FcChar32	*am = FcCharSetLeaf(a, ai)->map;
	    FcChar32	*bm = FcCharSetLeaf(b, bi)->map;
	    
	    if (am != bm)
	    {
		int	i = 256/32;
		/*
		 * Does am have any bits not in bm?
		 */
		while (i--)
		    if (*am++ & ~*bm++)
			return FcFalse;
	    }
	    ai++;
	    bi++;
	}
	/*
	 * Does a have any pages not in b?
	 */
	else if (an < bn)
	    return FcFalse;
	else
	{
	    int	    low = bi + 1;
	    int	    high = b->num - 1;

	    /*
	     * Search for page 'an' in 'b'
	     */
	    while (low <= high)
	    {
		int mid = (low + high) >> 1;
		bn = FcCharSetNumbers(b)[mid];
		if (bn == an)
		{
		    high = mid;
		    break;
		}
		if (bn < an)
		    low = mid + 1;
		else
		    high = mid - 1;
	    }
	    bi = high;
	    while (bi < b->num && FcCharSetNumbers(b)[bi] < an)
		bi++;
	}
    }
    /*
     * did we look at every page?
     */
    return ai >= a->num;
}

/*
 * These two functions efficiently walk the entire charmap for
 * other software (like pango) that want their own copy
 */

FcChar32
FcCharSetNextPage (const FcCharSet  *a, 
		   FcChar32	    map[FC_CHARSET_MAP_SIZE],
		   FcChar32	    *next)
{
    FcCharSetIter   ai;
    FcChar32	    page;

    ai.ucs4 = *next;
    FcCharSetIterSet (a, &ai);
    if (!ai.leaf)
	return FC_CHARSET_DONE;
    
    /*
     * Save current information
     */
    page = ai.ucs4;
    memcpy (map, ai.leaf->map, sizeof (ai.leaf->map));
    /*
     * Step to next page
     */
    FcCharSetIterNext (a, &ai);
    *next = ai.ucs4;

    return page;
}

FcChar32
FcCharSetFirstPage (const FcCharSet *a, 
		    FcChar32	    map[FC_CHARSET_MAP_SIZE],
		    FcChar32	    *next)
{
    *next = 0;
    return FcCharSetNextPage (a, map, next);
}

/*
 * old coverage API, rather hard to use correctly
 */
    
FcChar32
FcCharSetCoverage (const FcCharSet *a, FcChar32 page, FcChar32 *result)
{
    FcCharSetIter   ai;

    ai.ucs4 = page;
    FcCharSetIterSet (a, &ai);
    if (!ai.leaf)
    {
	memset (result, '\0', 256 / 8);
	page = 0;
    }
    else
    {
	memcpy (result, ai.leaf->map, sizeof (ai.leaf->map));
	FcCharSetIterNext (a, &ai);
	page = ai.ucs4;
    }
    return page;
}

/*
 * ASCII representation of charsets.
 * 
 * Each leaf is represented as 9 32-bit values, the code of the first character followed
 * by 8 32 bit values for the leaf itself.  Each value is encoded as 5 ASCII characters,
 * only 85 different values are used to avoid control characters as well as the other
 * characters used to encode font names.  85**5 > 2^32 so things work out, but
 * it's not exactly human readable output.  As a special case, 0 is encoded as a space
 */

static const unsigned char	charToValue[256] = {
    /*     "" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /*   "\b" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\020" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\030" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /*    " " */ 0xff,  0x00,  0xff,  0x01,  0x02,  0x03,  0x04,  0xff, 
    /*    "(" */ 0x05,  0x06,  0x07,  0x08,  0xff,  0xff,  0x09,  0x0a, 
    /*    "0" */ 0x0b,  0x0c,  0x0d,  0x0e,  0x0f,  0x10,  0x11,  0x12, 
    /*    "8" */ 0x13,  0x14,  0xff,  0x15,  0x16,  0xff,  0x17,  0x18, 
    /*    "@" */ 0x19,  0x1a,  0x1b,  0x1c,  0x1d,  0x1e,  0x1f,  0x20, 
    /*    "H" */ 0x21,  0x22,  0x23,  0x24,  0x25,  0x26,  0x27,  0x28, 
    /*    "P" */ 0x29,  0x2a,  0x2b,  0x2c,  0x2d,  0x2e,  0x2f,  0x30, 
    /*    "X" */ 0x31,  0x32,  0x33,  0x34,  0xff,  0x35,  0x36,  0xff, 
    /*    "`" */ 0xff,  0x37,  0x38,  0x39,  0x3a,  0x3b,  0x3c,  0x3d, 
    /*    "h" */ 0x3e,  0x3f,  0x40,  0x41,  0x42,  0x43,  0x44,  0x45, 
    /*    "p" */ 0x46,  0x47,  0x48,  0x49,  0x4a,  0x4b,  0x4c,  0x4d, 
    /*    "x" */ 0x4e,  0x4f,  0x50,  0x51,  0x52,  0x53,  0x54,  0xff, 
    /* "\200" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\210" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\220" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\230" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\240" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\250" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\260" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\270" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\300" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\310" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\320" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\330" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\340" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\350" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\360" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\370" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
};

static const FcChar8 valueToChar[0x55] = {
    /* 0x00 */ '!', '#', '$', '%', '&', '(', ')', '*',
    /* 0x08 */ '+', '.', '/', '0', '1', '2', '3', '4',
    /* 0x10 */ '5', '6', '7', '8', '9', ';', '<', '>',
    /* 0x18 */ '?', '@', 'A', 'B', 'C', 'D', 'E', 'F',
    /* 0x20 */ 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    /* 0x28 */ 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    /* 0x30 */ 'W', 'X', 'Y', 'Z', '[', ']', '^', 'a',
    /* 0x38 */ 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
    /* 0x40 */ 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q',
    /* 0x48 */ 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
    /* 0x50 */ 'z', '{', '|', '}', '~',
};

static FcChar8 *
FcCharSetParseValue (FcChar8 *string, FcChar32 *value)
{
    int		i;
    FcChar32	v;
    FcChar32	c;
    
    if (*string == ' ')
    {
	v = 0;
	string++;
    }
    else
    {
	v = 0;
	for (i = 0; i < 5; i++)
	{
	    if (!(c = (FcChar32) (unsigned char) *string++))
		return 0;
	    c = charToValue[c];
	    if (c == 0xff)
		return 0;
	    v = v * 85 + c;
	}
    }
    *value = v;
    return string;
}

static FcBool
FcCharSetUnparseValue (FcStrBuf *buf, FcChar32 value)
{
    int	    i;
    if (value == 0)
    {
	return FcStrBufChar (buf, ' ');
    }
    else
    {
	FcChar8	string[6];
	FcChar8	*s = string + 5;
	string[5] = '\0';
	for (i = 0; i < 5; i++)
	{
	    *--s = valueToChar[value % 85];
	    value /= 85;
	}
	for (i = 0; i < 5; i++)
	    if (!FcStrBufChar (buf, *s++))
		return FcFalse;
    }
    return FcTrue;
}

FcCharSet *
FcNameParseCharSet (FcChar8 *string)
{
    FcCharSet	*c;
    FcChar32	ucs4;
    FcCharLeaf	*leaf;
    FcCharLeaf	temp;
    FcChar32	bits;
    int		i;

    c = FcCharSetCreate ();
    if (!c)
	goto bail0;
    while (*string)
    {
	string = FcCharSetParseValue (string, &ucs4);
	if (!string)
	    goto bail1;
	bits = 0;
	for (i = 0; i < 256/32; i++)
	{
	    string = FcCharSetParseValue (string, &temp.map[i]);
	    if (!string)
		goto bail1;
	    bits |= temp.map[i];
	}
	if (bits)
	{
	    leaf = malloc (sizeof (FcCharLeaf));
	    if (!leaf)
		goto bail1;
	    *leaf = temp;
	    if (!FcCharSetInsertLeaf (c, ucs4, leaf))
		goto bail1;
	}
    }
    return c;
bail1:
    if (c->num)
    {
	FcMemFree (FC_MEM_CHARSET, c->num * sizeof (FcCharLeaf *));
	free (FcCharSetLeaves (c));
    }
    if (c->num)
    {
	FcMemFree (FC_MEM_CHARSET, c->num * sizeof (FcChar16));
	free (FcCharSetNumbers (c));
    }
    FcMemFree (FC_MEM_CHARSET, sizeof (FcCharSet));
    free (c);
bail0:
    return NULL;
}

FcBool
FcNameUnparseCharSet (FcStrBuf *buf, const FcCharSet *c)
{
    FcCharSetIter   ci;
    int		    i;
#ifdef CHECK
    int		    len = buf->len;
#endif

    for (FcCharSetIterStart (c, &ci);
	 ci.leaf;
	 FcCharSetIterNext (c, &ci))
    {
	if (!FcCharSetUnparseValue (buf, ci.ucs4))
	    return FcFalse;
	for (i = 0; i < 256/32; i++)
	    if (!FcCharSetUnparseValue (buf, ci.leaf->map[i]))
		return FcFalse;
    }
#ifdef CHECK
    {
	FcCharSet	*check;
	FcChar32	missing;
	FcCharSetIter	ci, checki;
	
	/* null terminate for parser */
	FcStrBufChar (buf, '\0');
	/* step back over null for life after test */
	buf->len--;
	check = FcNameParseCharSet (buf->buf + len);
	FcCharSetIterStart (c, &ci);
	FcCharSetIterStart (check, &checki);
	while (ci.leaf || checki.leaf)
	{
	    if (ci.ucs4 < checki.ucs4)
	    {
		printf ("Missing leaf node at 0x%x\n", ci.ucs4);
		FcCharSetIterNext (c, &ci);
	    }
	    else if (checki.ucs4 < ci.ucs4)
	    {
		printf ("Extra leaf node at 0x%x\n", checki.ucs4);
		FcCharSetIterNext (check, &checki);
	    }
	    else
	    {
		int	    i = 256/32;
		FcChar32    *cm = ci.leaf->map;
		FcChar32    *checkm = checki.leaf->map;

		for (i = 0; i < 256; i += 32)
		{
		    if (*cm != *checkm)
			printf ("Mismatching sets at 0x%08x: 0x%08x != 0x%08x\n",
				ci.ucs4 + i, *cm, *checkm);
		    cm++;
		    checkm++;
		}
		FcCharSetIterNext (c, &ci);
		FcCharSetIterNext (check, &checki);
	    }
	}
	if ((missing = FcCharSetSubtractCount (c, check)))
	    printf ("%d missing in reparsed result\n", missing);
	if ((missing = FcCharSetSubtractCount (check, c)))
	    printf ("%d extra in reparsed result\n", missing);
	FcCharSetDestroy (check);
    }
#endif
    
    return FcTrue;
}

typedef struct _FcCharLeafEnt FcCharLeafEnt;

struct _FcCharLeafEnt {
    FcCharLeafEnt   *next;
    FcChar32	    hash;
    FcCharLeaf	    leaf;
};

#define FC_CHAR_LEAF_BLOCK	(4096 / sizeof (FcCharLeafEnt))
#define FC_CHAR_LEAF_HASH_SIZE	257

typedef struct _FcCharSetEnt FcCharSetEnt;

struct _FcCharSetEnt {
    FcCharSetEnt	*next;
    FcChar32		hash;
    FcCharSet		set;
};

typedef struct _FcCharSetOrigEnt FcCharSetOrigEnt;

struct _FcCharSetOrigEnt {
    FcCharSetOrigEnt	*next;
    const FcCharSet    	*orig;
    const FcCharSet    	*frozen;
};

#define FC_CHAR_SET_HASH_SIZE    67

struct _FcCharSetFreezer {
    FcCharLeafEnt   *leaf_hash_table[FC_CHAR_LEAF_HASH_SIZE];
    FcCharLeafEnt   **leaf_blocks;
    int		    leaf_block_count;
    FcCharSetEnt    *set_hash_table[FC_CHAR_SET_HASH_SIZE];
    FcCharSetOrigEnt	*orig_hash_table[FC_CHAR_SET_HASH_SIZE];
    FcCharLeafEnt   *current_block;
    int		    leaf_remain;
    int		    leaves_seen;
    int		    charsets_seen;
    int		    leaves_allocated;
    int		    charsets_allocated;
};

static FcCharLeafEnt *
FcCharLeafEntCreate (FcCharSetFreezer *freezer)
{
    if (!freezer->leaf_remain)
    {
	FcCharLeafEnt **newBlocks;

	freezer->leaf_block_count++;
	newBlocks = realloc (freezer->leaf_blocks, freezer->leaf_block_count * sizeof (FcCharLeafEnt *));
	if (!newBlocks)
	    return 0;
	freezer->leaf_blocks = newBlocks;
	freezer->current_block = freezer->leaf_blocks[freezer->leaf_block_count-1] = malloc (FC_CHAR_LEAF_BLOCK * sizeof (FcCharLeafEnt));
	if (!freezer->current_block)
	    return 0;
	FcMemAlloc (FC_MEM_CHARLEAF, FC_CHAR_LEAF_BLOCK * sizeof (FcCharLeafEnt));
	freezer->leaf_remain = FC_CHAR_LEAF_BLOCK;
    }
    freezer->leaf_remain--;
    freezer->leaves_allocated++;
    return freezer->current_block++;
}

static FcChar32
FcCharLeafHash (FcCharLeaf *leaf)
{
    FcChar32	hash = 0;
    int		i;

    for (i = 0; i < 256/32; i++)
	hash = ((hash << 1) | (hash >> 31)) ^ leaf->map[i];
    return hash;
}

static FcCharLeaf *
FcCharSetFreezeLeaf (FcCharSetFreezer *freezer, FcCharLeaf *leaf)
{
    FcChar32			hash = FcCharLeafHash (leaf);
    FcCharLeafEnt		**bucket = &freezer->leaf_hash_table[hash % FC_CHAR_LEAF_HASH_SIZE];
    FcCharLeafEnt		*ent;
    
    for (ent = *bucket; ent; ent = ent->next)
    {
	if (ent->hash == hash && !memcmp (&ent->leaf, leaf, sizeof (FcCharLeaf)))
	    return &ent->leaf;
    }

    ent = FcCharLeafEntCreate(freezer);
    if (!ent)
	return 0;
    ent->leaf = *leaf;
    ent->hash = hash;
    ent->next = *bucket;
    *bucket = ent;
    return &ent->leaf;
}

static FcChar32
FcCharSetHash (FcCharSet *fcs)
{
    FcChar32	hash = 0;
    int		i;

    /* hash in leaves */
    for (i = 0; i < fcs->num; i++)
	hash = ((hash << 1) | (hash >> 31)) ^ FcCharLeafHash (FcCharSetLeaf(fcs,i));
    /* hash in numbers */
    for (i = 0; i < fcs->num; i++)
	hash = ((hash << 1) | (hash >> 31)) ^ *FcCharSetNumbers(fcs);
    return hash;
}

static FcBool
FcCharSetFreezeOrig (FcCharSetFreezer *freezer, const FcCharSet *orig, const FcCharSet *frozen)
{
    FcCharSetOrigEnt	**bucket = &freezer->orig_hash_table[((uintptr_t) orig) & FC_CHAR_SET_HASH_SIZE];
    FcCharSetOrigEnt	*ent;
    
    ent = malloc (sizeof (FcCharSetOrigEnt));
    if (!ent)
	return FcFalse;
    ent->orig = orig;
    ent->frozen = frozen;
    ent->next = *bucket;
    *bucket = ent;
    return FcTrue;
}

static FcCharSet *
FcCharSetFreezeBase (FcCharSetFreezer *freezer, FcCharSet *fcs, const FcCharSet *orig)
{
    FcChar32		hash = FcCharSetHash (fcs);
    FcCharSetEnt	**bucket = &freezer->set_hash_table[hash % FC_CHAR_SET_HASH_SIZE];
    FcCharSetEnt	*ent;
    int			size;
    int			i;

    for (ent = *bucket; ent; ent = ent->next)
    {
	if (ent->hash == hash &&
	    ent->set.num == fcs->num &&
	    !memcmp (FcCharSetNumbers(&ent->set), 
		     FcCharSetNumbers(fcs),
		     fcs->num * sizeof (FcChar16)))
	{
	    FcBool ok = FcTrue;
	    int i;

	    for (i = 0; i < fcs->num; i++)
		if (FcCharSetLeaf(&ent->set, i) != FcCharSetLeaf(fcs, i))
		    ok = FcFalse;
	    if (ok)
		return &ent->set;
	}
    }

    size = (sizeof (FcCharSetEnt) +
	    fcs->num * sizeof (FcCharLeaf *) +
	    fcs->num * sizeof (FcChar16));
    ent = malloc (size);
    if (!ent)
	return 0;
    FcMemAlloc (FC_MEM_CHARSET, size);
    
    freezer->charsets_allocated++;
    
    ent->set.ref = FC_REF_CONSTANT;
    ent->set.num = fcs->num;
    if (fcs->num)
    {
	intptr_t    *ent_leaves;

	ent->set.leaves_offset = sizeof (ent->set);
	ent->set.numbers_offset = (ent->set.leaves_offset +
				   fcs->num * sizeof (intptr_t));
    
	ent_leaves = FcCharSetLeaves (&ent->set);
	for (i = 0; i < fcs->num; i++)
	    ent_leaves[i] = FcPtrToOffset (ent_leaves,
					   FcCharSetLeaf (fcs, i));
	memcpy (FcCharSetNumbers (&ent->set), 
		FcCharSetNumbers (fcs), 
		fcs->num * sizeof (FcChar16));
    }
    else
    {
	ent->set.leaves_offset = 0;
	ent->set.numbers_offset = 0;
    }

    ent->hash = hash;
    ent->next = *bucket;
    *bucket = ent;

    return &ent->set;
}

static const FcCharSet *
FcCharSetFindFrozen (FcCharSetFreezer *freezer, const FcCharSet *orig)
{
    FcCharSetOrigEnt    **bucket = &freezer->orig_hash_table[((uintptr_t) orig) & FC_CHAR_SET_HASH_SIZE];
    FcCharSetOrigEnt	*ent;
    
    for (ent = *bucket; ent; ent = ent->next)
	if (ent->orig == orig)
	    return ent->frozen;
    return NULL;
}

const FcCharSet *
FcCharSetFreeze (FcCharSetFreezer *freezer, const FcCharSet *fcs)
{
    FcCharSet	    *b;
    const FcCharSet *n = 0;
    FcCharLeaf	    *l;
    int		    i;

    b = FcCharSetCreate ();
    if (!b)
	goto bail0;
    for (i = 0; i < fcs->num; i++)
    {
	l = FcCharSetFreezeLeaf (freezer, FcCharSetLeaf(fcs, i));
	if (!l)
	    goto bail1;
	if (!FcCharSetInsertLeaf (b, FcCharSetNumbers(fcs)[i] << 8, l))
	    goto bail1;
    }
    n = FcCharSetFreezeBase (freezer, b, fcs);
    if (!FcCharSetFreezeOrig (freezer, fcs, n))
    {
	n = NULL;
	goto bail1;
    }
    freezer->charsets_seen++;
    freezer->leaves_seen += fcs->num;
bail1:
    if (b->num)
    {
	FcMemFree (FC_MEM_CHARSET, b->num * sizeof (FcCharLeaf *));
	free (FcCharSetLeaves (b));
    }
    if (b->num)
    {
	FcMemFree (FC_MEM_CHARSET, b->num * sizeof (FcChar16));
	free (FcCharSetNumbers (b));
    }
    FcMemFree (FC_MEM_CHARSET, sizeof (FcCharSet));
    free (b);
bail0:
    return n;
}

FcCharSetFreezer *
FcCharSetFreezerCreate (void)
{
    FcCharSetFreezer	*freezer;

    freezer = calloc (1, sizeof (FcCharSetFreezer));
    return freezer;
}

void
FcCharSetFreezerDestroy (FcCharSetFreezer *freezer)
{
    int i;

    if (FcDebug() & FC_DBG_CACHE)
    {
	printf ("\ncharsets %d -> %d leaves %d -> %d\n",
		freezer->charsets_seen, freezer->charsets_allocated,
		freezer->leaves_seen, freezer->leaves_allocated);
    }
    for (i = 0; i < FC_CHAR_SET_HASH_SIZE; i++)
    {
	FcCharSetEnt	*ent, *next;
	for (ent = freezer->set_hash_table[i]; ent; ent = next)
	{
	    next = ent->next;
	    free (ent);
	}
    }

    for (i = 0; i < FC_CHAR_SET_HASH_SIZE; i++)
    {
	FcCharSetOrigEnt	*ent, *next;
	for (ent = freezer->orig_hash_table[i]; ent; ent = next)
	{
	    next = ent->next;
	    free (ent);
	}
    }

    for (i = 0; i < freezer->leaf_block_count; i++)
	free (freezer->leaf_blocks[i]);

    free (freezer->leaf_blocks);
    free (freezer);
}

FcBool
FcCharSetSerializeAlloc (FcSerialize *serialize, const FcCharSet *cs)
{
    intptr_t	    *leaves;
    FcChar16	    *numbers;
    int		    i;
    
    if (cs->ref != FC_REF_CONSTANT)
    {
	if (!serialize->cs_freezer)
	{
	    serialize->cs_freezer = FcCharSetFreezerCreate ();
	    if (!serialize->cs_freezer)
		return FcFalse;
	}
	if (FcCharSetFindFrozen (serialize->cs_freezer, cs))
	    return FcTrue;
    
        cs = FcCharSetFreeze (serialize->cs_freezer, cs);
    }
    
    leaves = FcCharSetLeaves (cs);
    numbers = FcCharSetNumbers (cs);
    
    if (!FcSerializeAlloc (serialize, cs, sizeof (FcCharSet)))
	return FcFalse;
    if (!FcSerializeAlloc (serialize, leaves, cs->num * sizeof (intptr_t)))
	return FcFalse;
    if (!FcSerializeAlloc (serialize, numbers, cs->num * sizeof (FcChar16)))
	return FcFalse;
    for (i = 0; i < cs->num; i++)
	if (!FcSerializeAlloc (serialize, FcCharSetLeaf(cs, i),
			       sizeof (FcCharLeaf)))
	    return FcFalse;
    return FcTrue;
}
    
FcCharSet *
FcCharSetSerialize(FcSerialize *serialize, const FcCharSet *cs)
{
    FcCharSet	*cs_serialized;
    intptr_t	*leaves, *leaves_serialized;
    FcChar16	*numbers, *numbers_serialized;
    FcCharLeaf	*leaf, *leaf_serialized;
    int		i;

    if (cs->ref != FC_REF_CONSTANT && serialize->cs_freezer)
    {
	cs = FcCharSetFindFrozen (serialize->cs_freezer, cs);
	if (!cs)
	    return NULL;
    }
		    
    cs_serialized = FcSerializePtr (serialize, cs);
    if (!cs_serialized)
	return NULL;
    
    cs_serialized->ref = FC_REF_CONSTANT;
    cs_serialized->num = cs->num;

    if (cs->num)
    {
	leaves = FcCharSetLeaves (cs);
	leaves_serialized = FcSerializePtr (serialize, leaves);
	if (!leaves_serialized)
	    return NULL;
    
	cs_serialized->leaves_offset = FcPtrToOffset (cs_serialized,
						      leaves_serialized);
	
	numbers = FcCharSetNumbers (cs);
	numbers_serialized = FcSerializePtr (serialize, numbers);
	if (!numbers)
	    return NULL;
    
	cs_serialized->numbers_offset = FcPtrToOffset (cs_serialized,
						       numbers_serialized);
    
	for (i = 0; i < cs->num; i++)
	{
	    leaf = FcCharSetLeaf (cs, i);
	    leaf_serialized = FcSerializePtr (serialize, leaf);
	    if (!leaf_serialized)
		return NULL;
	    *leaf_serialized = *leaf;
	    leaves_serialized[i] = FcPtrToOffset (leaves_serialized, 
						  leaf_serialized);
	    numbers_serialized[i] = numbers[i];
	}
    }
    else
    {
	cs_serialized->leaves_offset = 0;
	cs_serialized->numbers_offset = 0;
    }
    
    return cs_serialized;
}
#define __fccharset__
#include "fcaliastail.h"
#undef __fccharset__
