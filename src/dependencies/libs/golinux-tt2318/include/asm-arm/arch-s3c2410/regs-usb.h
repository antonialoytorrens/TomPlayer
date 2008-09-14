#ifndef __ASM_ARCH_REGS_USB_H
#define __ASM_ARCH_REGS_USB_H

#define S3C2410_USBHREG(x) ((x) + S3C2410_VA_USBHOST)

#define S3C2410_UHC_REVISION		S3C2410_USBHREG(0x00)
#define S3C2410_UHC_CONTROL		S3C2410_USBHREG(0x04)
#define S3C2410_UHC_COMMON_STATUS	S3C2410_USBHREG(0x08)
#define S3C2410_UHC_INTERRUPT_STATUS	S3C2410_USBHREG(0x0c)
#define S3C2410_UHC_INTERRUPT_ENABLE	S3C2410_USBHREG(0x10)
#define S3C2410_UHC_INTERRUPT_DISABLE	S3C2410_USBHREG(0x14)

#define S3C2410_UHC_HCCA		S3C2410_USBHREG(0x18)




#define S3C2410_USBDREG(x) ((x) + S3C2410_VA_USBDEV)

#define S3C2410_UDC_FUNC_ADDR_REG	S3C2410_USBDREG(0x0140)
#define S3C2410_UDC_PWR_REG		S3C2410_USBDREG(0x0144)
#define S3C2410_UDC_EP_INT_REG		S3C2410_USBDREG(0x0148)

#define S3C2410_UDC_USB_INT_REG	S3C2410_USBDREG(0x0158)
#define S3C2410_UDC_EP_INT_EN_REG	S3C2410_USBDREG(0x015c)

#define S3C2410_UDC_USB_INT_EN_REG	S3C2410_USBDREG(0x016c)

#define S3C2410_UDC_FRAME_NUM1_REG	S3C2410_USBDREG(0x0170)
#define S3C2410_UDC_FRAME_NUM2_REG	S3C2410_USBDREG(0x0174)

#define S3C2410_UDC_EP0_FIFO_REG	S3C2410_USBDREG(0x01c0)
#define S3C2410_UDC_EP1_FIFO_REG	S3C2410_USBDREG(0x01c4)
#define S3C2410_UDC_EP2_FIFO_REG	S3C2410_USBDREG(0x01c8)
#define S3C2410_UDC_EP3_FIFO_REG	S3C2410_USBDREG(0x01cc)
#define S3C2410_UDC_EP4_FIFO_REG	S3C2410_USBDREG(0x01d0)

#define S3C2410_UDC_EP1_DMA_CON	S3C2410_USBDREG(0x0200)
#define S3C2410_UDC_EP1_DMA_UNIT	S3C2410_USBDREG(0x0204)
#define S3C2410_UDC_EP1_DMA_FIFO	S3C2410_USBDREG(0x0208)
#define S3C2410_UDC_EP1_DMA_TTC_L	S3C2410_USBDREG(0x020c)
#define S3C2410_UDC_EP1_DMA_TTC_M	S3C2410_USBDREG(0x0210)
#define S3C2410_UDC_EP1_DMA_TTC_H	S3C2410_USBDREG(0x0214)

#define S3C2410_UDC_EP2_DMA_CON	S3C2410_USBDREG(0x0218)
#define S3C2410_UDC_EP2_DMA_UNIT	S3C2410_USBDREG(0x021c)
#define S3C2410_UDC_EP2_DMA_FIFO	S3C2410_USBDREG(0x0220)
#define S3C2410_UDC_EP2_DMA_TTC_L	S3C2410_USBDREG(0x0224)
#define S3C2410_UDC_EP2_DMA_TTC_M	S3C2410_USBDREG(0x0228)
#define S3C2410_UDC_EP2_DMA_TTC_H	S3C2410_USBDREG(0x022c)

#define S3C2410_UDC_EP3_DMA_CON	S3C2410_USBDREG(0x0240)
#define S3C2410_UDC_EP3_DMA_UNIT	S3C2410_USBDREG(0x0244)
#define S3C2410_UDC_EP3_DMA_FIFO	S3C2410_USBDREG(0x0248)
#define S3C2410_UDC_EP3_DMA_TTC_L	S3C2410_USBDREG(0x024c)
#define S3C2410_UDC_EP3_DMA_TTC_M	S3C2410_USBDREG(0x0250)
#define S3C2410_UDC_EP3_DMA_TTC_H	S3C2410_USBDREG(0x0254)

#define S3C2410_UDC_EP4_DMA_CON	S3C2410_USBDREG(0x0258)
#define S3C2410_UDC_EP4_DMA_UNIT	S3C2410_USBDREG(0x025c)
#define S3C2410_UDC_EP4_DMA_FIFO	S3C2410_USBDREG(0x0260)
#define S3C2410_UDC_EP4_DMA_TTC_L	S3C2410_USBDREG(0x0264)
#define S3C2410_UDC_EP4_DMA_TTC_M	S3C2410_USBDREG(0x0268)
#define S3C2410_UDC_EP4_DMA_TTC_H	S3C2410_USBDREG(0x026c)

#define S3C2410_UDC_INDEX_REG		S3C2410_USBDREG(0x0178)

/* indexed registers */

#define S3C2410_UDC_MAXP_REG		S3C2410_USBDREG(0x0180)

#define S3C2410_UDC_IN_CSR1_REG		S3C2410_USBDREG(0x0184)
#define S3C2410_UDC_IN_CSR2_REG		S3C2410_USBDREG(0x0188)

#define S3C2410_UDC_OUT_CSR1_REG	S3C2410_USBDREG(0x0190)
#define S3C2410_UDC_OUT_CSR2_REG	S3C2410_USBDREG(0x0194)
#define S3C2410_UDC_OUT_FIFO_CNT1_REG	S3C2410_USBDREG(0x0198)
#define S3C2410_UDC_OUT_FIFO_CNT2_REG	S3C2410_USBDREG(0x019c)



#define S3C2410_UDC_PWR_ISOUP		(1<<7) // R/W
#define S3C2410_UDC_PWR_RESET		(1<<3) // R
#define S3C2410_UDC_PWR_RESUME		(1<<2) // R/W
#define S3C2410_UDC_PWR_SUSPEND		(1<<1) // R
#define S3C2410_UDC_PWR_ENSUSPEND	(1<<0) // R/W

#define S3C2410_UDC_PWR_DEFAULT		0x00

#define S3C2410_UDC_INT_EP4		(1<<4) // R/W (clear only)
#define S3C2410_UDC_INT_EP3		(1<<3) // R/W (clear only)
#define S3C2410_UDC_INT_EP2		(1<<2) // R/W (clear only)
#define S3C2410_UDC_INT_EP1		(1<<1) // R/W (clear only)
#define S3C2410_UDC_INT_EP0		(1<<0) // R/W (clear only)

#define S3C2410_UDC_USBINT_RESET	(1<<2) // R/W (clear only)
#define S3C2410_UDC_USBINT_RESUME	(1<<1) // R/W (clear only)
#define S3C2410_UDC_USBINT_SUSPEND	(1<<0) // R/W (clear only)

#define S3C2410_UDC_INTE_EP4		(1<<4) // R/W
#define S3C2410_UDC_INTE_EP3		(1<<3) // R/W
#define S3C2410_UDC_INTE_EP2		(1<<2) // R/W
#define S3C2410_UDC_INTE_EP1		(1<<1) // R/W
#define S3C2410_UDC_INTE_EP0		(1<<0) // R/W

#define S3C2410_UDC_USBINTE_RESET	(1<<2) // R/W
#define S3C2410_UDC_USBINTE_SUSPEND	(1<<0) // R/W

/*
#define fUD_FRAMEL_NUM	Fld(8,0) // R
#define UD_FRAMEL_NUM	FMsk(fUD_FRAMEL_NUM)

#define fUD_FRAMEH_NUM	Fld(8,0) // R
#define UD_FRAMEH_NUM	FMsk(fUD_FRAMEH_NUM)
*/

#define S3C2410_UDC_INDEX_EP0		(0x00)
#define S3C2410_UDC_INDEX_EP1		(0x01) // ??
#define S3C2410_UDC_INDEX_EP2		(0x02) // ??
#define S3C2410_UDC_INDEX_EP3		(0x03) // ??
#define S3C2410_UDC_INDEX_EP4		(0x04) // ??

#define S3C2410_UDC_ICSR1_CLRDT	(1<<6) // R/W
#define S3C2410_UDC_ICSR1_SENTSTL	(1<<5) // R/W (clear only)
#define S3C2410_UDC_ICSR1_SENDSTL	(1<<4) // R/W
#define S3C2410_UDC_ICSR1_FFLUSH	(1<<3) // W	    (set only)
#define S3C2410_UDC_ICSR1_UNDRUN	(1<<2) // R/W (clear only)
#define S3C2410_UDC_ICSR1_PKTRDY	(1<<0) // R/W (set only)

#define S3C2410_UDC_ICSR2_AUTOSET	(1<<7) // R/W
#define S3C2410_UDC_ICSR2_ISO		(1<<6) // R/W
#define S3C2410_UDC_ICSR2_MODEIN	(1<<5) // R/W
#define S3C2410_UDC_ICSR2_DMAIEN	(1<<4) // R/W

#define S3C2410_UDC_OCSR1_CLRDT		(1<<7) // R/W
#define S3C2410_UDC_OCSR1_SENTSTL	(1<<6) // R/W (clear only)
#define S3C2410_UDC_OCSR1_SENDSTL	(1<<5) // R/W
#define S3C2410_UDC_OCSR1_FFLUSH	(1<<4) // R/W
#define S3C2410_UDC_OCSR1_DERROR	(1<<3) // R
#define S3C2410_UDC_OCSR1_OVRRUN	(1<<2) // R/W (clear only)
#define S3C2410_UDC_OCSR1_PKTRDY	(1<<0) // R/W (clear only)

#define S3C2410_UDC_OCSR2_AUTOCLR	(1<<7) // R/W
#define S3C2410_UDC_OCSR2_ISO		(1<<6) // R/W
#define S3C2410_UDC_OCSR2_DMAIEN	(1<<5) // R/W


#endif


