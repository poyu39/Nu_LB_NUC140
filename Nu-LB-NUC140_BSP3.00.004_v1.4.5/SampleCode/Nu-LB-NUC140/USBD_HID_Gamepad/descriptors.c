/******************************************************************************
 * @file     descriptors.c
 * @brief    NUC100 series USBD descriptor
 * @version  2.0.0
 * @date     22, March, 2013
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#ifndef __DESCRIPTORS_C__
#define __DESCRIPTORS_C__

/*!<Includes */
#include "NUC100Series.h"
#include "usbd.h"
#include "hid_gamepad.h"

/*!<USB HID Report Descriptor */
uint8_t HID_GamepadReportDescriptor[] =
{
		0x05, 0x01,	// USAGE_PAGE (Generic Desktop)
		0x09, 0x05,	// USAGE (Game Pad)
		0xa1, 0x01,	// COLLECTION (Application)
		0xa1, 0x00,	//   COLLECTION (Physical)
		0x05, 0x09,	//     USAGE_PAGE (Button)
		0x19, 0x01,	//     USAGE_MINIMUM (Button 1)
		0x29, 0x10,	//     USAGE_MAXIMUM (Button 16)
		0x15, 0x00,	//     LOGICAL_MINIMUM (0)
		0x25, 0x01,	//     LOGICAL_MAXIMUM (1)
		0x75, 0x01,	//     REPORT_SIZE (1)
		0x95, 0x10,	//     REPORT_COUNT (16)
		0x81, 0x02,	//     INPUT (Data,Var,Abs)
		0x05, 0x01,	//     USAGE_PAGE (Generic Desktop)
		0x09, 0x30,	//     USAGE (X)
		0x09, 0x31,	//     USAGE (Y)
		0x15, 0x81,	//     LOGICAL_MINIMUM (-127)
		0x25, 0x7f,	//     LOGICAL_MAXIMUM (127)
		0x75, 0x08,	//     REPORT_SIZE (8)
		0x95, 0x02,	//     REPORT_COUNT (2)
		0x81, 0x02,	//     INPUT (Data,Var,Abs)
		0xc0,      	//   END_COLLECTION
		0xc0       	// END_COLLECTION
};




/*----------------------------------------------------------------------------*/
/*!<USB Device Descriptor */
uint8_t gu8DeviceDescriptor[] =
{
    LEN_DEVICE,     /* bLength */
    DESC_DEVICE,    /* bDescriptorType */
    0x10, 0x01,     /* bcdUSB */
    0x00,           /* bDeviceClass */
    0x00,           /* bDeviceSubClass */
    0x00,           /* bDeviceProtocol */
    EP0_MAX_PKT_SIZE,   /* bMaxPacketSize0 */
    /* idVendor */
    USBD_VID & 0x00FF,
    (USBD_VID & 0xFF00) >> 8,
    /* idProduct */
    USBD_PID & 0x00FF,
    (USBD_PID & 0xFF00) >> 8,
    0x00, 0x00,     /* bcdDevice */
    0x01,           /* iManufacture */
    0x02,           /* iProduct */
    0x00,           /* iSerialNumber - no serial */
    0x01            /* bNumConfigurations */
};

/*!<USB Configure Descriptor */
uint8_t gu8ConfigDescriptor[] =
{
    LEN_CONFIG,     /* bLength */
    DESC_CONFIG,    /* bDescriptorType */
    /* wTotalLength */
    LEN_CONFIG_AND_SUBORDINATE & 0x00FF,
    (LEN_CONFIG_AND_SUBORDINATE & 0xFF00) >> 8,
    0x01,           /* bNumInterfaces */
    0x01,           /* bConfigurationValue */
    0x00,           /* iConfiguration */
    0x80 | (USBD_SELF_POWERED << 6) | (USBD_REMOTE_WAKEUP << 5),/* bmAttributes */
    USBD_MAX_POWER,         /* MaxPower */

    /* I/F descr: HID */
    LEN_INTERFACE,  /* bLength */
    DESC_INTERFACE, /* bDescriptorType */
    0x00,           /* bInterfaceNumber */
    0x00,           /* bAlternateSetting */
    0x01,           /* bNumEndpoints */
    0x03,           /* bInterfaceClass */
    0x01,           /* bInterfaceSubClass */
    HID_FUNCTION,   /* bInterfaceProtocol */
    0x00,           /* iInterface */

    /* HID Descriptor */
    LEN_HID,        /* Size of this descriptor in UINT8s. */
    DESC_HID,       /* HID descriptor type. */
    0x10, 0x01,     /* HID Class Spec. release number. */
    0x00,           /* H/W target country. */
    0x01,           /* Number of HID class descriptors to follow. */
    DESC_HID_RPT,   /* Dscriptor type. */
    /* Total length of report descriptor. */
    sizeof(HID_GamepadReportDescriptor) & 0x00FF,
    (sizeof(HID_GamepadReportDescriptor) & 0xFF00) >> 8,

    /* EP Descriptor: interrupt in. */
    LEN_ENDPOINT,   /* bLength */
    DESC_ENDPOINT,  /* bDescriptorType */
    (INT_IN_EP_NUM | EP_INPUT), /* bEndpointAddress */
    EP_INT,         /* bmAttributes */
    /* wMaxPacketSize */
    EP2_MAX_PKT_SIZE & 0x00FF,
    (EP2_MAX_PKT_SIZE & 0xFF00) >> 8,
    HID_DEFAULT_INT_IN_INTERVAL     /* bInterval */
};

/*!<USB Language String Descriptor */
uint8_t gu8StringLang[4] =
{
    4,              /* bLength */
    DESC_STRING,    /* bDescriptorType */
    0x09, 0x04
};

/*!<USB Vendor String Descriptor */
uint8_t gu8VendorStringDesc[] =
{
    16,
    DESC_STRING,
    'N', 0, 'u', 0, 'v', 0, 'o', 0, 't', 0, 'o', 0, 'n', 0
};

/*!<USB Product String Descriptor */
uint8_t gu8ProductStringDesc[] =
{
    26,
    DESC_STRING,
    'H', 0, 'I', 0, 'D', 0, ' ', 0, 'G', 0, 'a', 0, 'm', 0, 'e', 0, 'p', 0, 'a', 0, 'd', 0, ' ', 0
};


const uint8_t gu8StringSerial[26] =
{
    26,             // bLength
    DESC_STRING,    // bDescriptorType
    'A', 0, '0', 0, '2', 0, '0', 0, '0', 0, '8', 0, '0', 0, '4', 0, '0', 0, '1', 0, '1', 0, '4', 0
};

const uint8_t *gpu8UsbString[4] =
{
    gu8StringLang,
    gu8VendorStringDesc,
    gu8ProductStringDesc,
    gu8StringSerial
};

const S_USBD_INFO_T gsInfo =
{
    gu8DeviceDescriptor,
    gu8ConfigDescriptor,
    gpu8UsbString,
    HID_GamepadReportDescriptor
};




#endif  /* __DESCRIPTORS_C__ */
