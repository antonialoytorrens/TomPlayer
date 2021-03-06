#ifndef QT_COMP_H
#define QT_COMP_H

DECL_COMPONENT(sorenson3,"SMD_ComponentDispatch",1)
DECL_COMPONENT(sorenson,"SorensonYUV9Dispatcher",1)
DECL_COMPONENT(sorenson1,"SVD_ComponentDispatch",1)
DECL_COMPONENT(genericd,"GenericComponentDispatch",0)
DECL_COMPONENT(stmedia,"STMediaComponentDispatch",0)
DECL_COMPONENT(handlecd,"HandleComponentDispatch",0)
DECL_COMPONENT(pcxcd,"PCExchangeComponentDispatch",0)
DECL_COMPONENT(capcd,"CapComponentDispatch",0)
DECL_COMPONENT(videocd,"VideoComponentDispatch",0)
DECL_COMPONENT(soundcd,"SoundComponentDispatch",0)
DECL_COMPONENT(scalingcd,"ScalingCodecDispatcher",0)
DECL_COMPONENT(formatcnv,"FormatConverterDispatch",0)

DECL_COMPONENT(basecd,"Base_CDComponentDispatch",1)
DECL_COMPONENT(ddcd,"DD_CDComponentDispatch",0)
DECL_COMPONENT(fakergb,"FakeRGBDispatcher",0)
DECL_COMPONENT(rawcd,"RAW_CDComponentDispatch",0)
DECL_COMPONENT(yuvsgi,"YUVSGI_CDComponentDispatch",0)
DECL_COMPONENT(yuvs,"YUVS_CDComponentDispatch",0)
DECL_COMPONENT(yuvu,"YUVU_CDComponentDispatch",0)

DECL_COMPONENT(qdm2d,"QDM2Decompressor",0)
DECL_COMPONENT(mace3d,"Mace3DecompDispatch",0)

//DECL_COMPONENT(,"",0)

#endif /* QT_COMP_H */
