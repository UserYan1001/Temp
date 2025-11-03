/**
 ******************************************************************************
 * @file    usbh_hid_core.c
 * @author  MCD Application Team
 * @version V2.2.1
 * @date    17-March-2018
 * @brief   This file is the HID Layer Handlers for USB Host HID class.
 *
 * @verbatim
 *
 *          ===================================================================
 *                                HID Class  Description
 *          ===================================================================
 *           This module manages the MSC class V1.11 following the "Device Class Definition
 *           for Human Interface Devices (HID) Version 1.11 Jun 27, 2001".
 *           This driver implements the following aspects of the specification:
 *             - The Boot Interface Subclass
 *             - The Mouse and Keyboard protocols
 *
 *  @endverbatim
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                      <http://www.st.com/SLA0044>
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "usbh_hid_core.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keybd.h"
#include "usbh_hid_gamepad.h"
/** @addtogroup USBH_LIB
 * @{
 */

/** @addtogroup USBH_CLASS
 * @{
 */

/** @addtogroup USBH_HID_CLASS
 * @{
 */

/** @defgroup USBH_HID_CORE
 * @brief    This file includes HID Layer Handlers for USB Host HID class.
 * @{
 */

/** @defgroup USBH_HID_CORE_Private_TypesDefinitions
 * @{
 */
/**
 * @}
 */

/** @defgroup USBH_HID_CORE_Private_Defines
 * @{
 */

/* 厂商请求命令定义 */
#define VENDOR_REQ_GET_VERSION     0x01    /* 获取设备版本 */
#define VENDOR_REQ_SET_MODE        0x02    /* 设置设备模式 */
#define VENDOR_REQ_GET_STATUS      0x03    /* 获取设备状态 */
#define VENDOR_REQ_CUSTOM_CMD      0x04    /* 自定义命令 */
#define USB_REQ_TYPE_MASK 0x60
/**
 * @}
 */

/** @defgroup USBH_HID_CORE_Private_Macros
 * @{
 */
/**
 * @}
 */

/** @defgroup USBH_HID_CORE_Private_Variables
 * @{
 */
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN HID_Machine_TypeDef HID_Machine __ALIGN_END;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN HID_Report_TypeDef HID_Report __ALIGN_END;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_Setup_TypeDef HID_Setup __ALIGN_END;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USBH_HIDDesc_TypeDef HID_Desc __ALIGN_END;

__IO uint8_t start_toggle = 0;

/* 厂商请求相关变量 */
static uint8_t vendor_request_active = 0;
static uint8_t vendor_buffer[64];
static uint16_t vendor_data_length = 0;

/**
 * @}
 */

/** @defgroup USBH_HID_CORE_Private_FunctionPrototypes
 * @{
 */

static USBH_Status USBH_HID_InterfaceInit(USB_OTG_CORE_HANDLE *pdev,
                                          void *phost);

static void USBH_ParseHIDDesc(USBH_HIDDesc_TypeDef *desc, uint8_t *buf);

static void USBH_HID_InterfaceDeInit(USB_OTG_CORE_HANDLE *pdev,
                                     void *phost);

static USBH_Status USBH_HID_Handle(USB_OTG_CORE_HANDLE *pdev,
                                   void *phost);

static USBH_Status USBH_HID_ClassRequest(USB_OTG_CORE_HANDLE *pdev,
                                         void *phost);

static USBH_Status USBH_Get_HID_ReportDescriptor(USB_OTG_CORE_HANDLE *pdev,
                                                 USBH_HOST *phost,
                                                 uint16_t length);

static USBH_Status USBH_Get_HID_Descriptor(USB_OTG_CORE_HANDLE *pdev,
                                           USBH_HOST *phost,
                                           uint16_t length);

static USBH_Status USBH_Set_Idle(USB_OTG_CORE_HANDLE *pdev,
                                 USBH_HOST *phost,
                                 uint8_t duration,
                                 uint8_t reportId);

static USBH_Status USBH_Set_Protocol(USB_OTG_CORE_HANDLE *pdev,
                                     USBH_HOST *phost,
                                     uint8_t protocol);

/* 厂商请求处理函数 */
static USBH_Status USBH_HID_HandleVendorRequest(USB_OTG_CORE_HANDLE *pdev,
                                               USBH_HOST *phost,
                                               uint8_t bRequest);
static USBH_Status USBH_HID_SendVendorRequest(USB_OTG_CORE_HANDLE *pdev,
                                             USBH_HOST *phost,
                                             uint8_t bRequest,
                                             uint16_t wValue,
                                             uint16_t wIndex,
                                             uint8_t *data,
                                             uint16_t wLength);

USBH_Class_cb_TypeDef HID_cb =
    {
        USBH_HID_InterfaceInit,
        USBH_HID_InterfaceDeInit,
        USBH_HID_ClassRequest,
        USBH_HID_Handle};
/**
 * @}
 */

/** @defgroup USBH_HID_CORE_Private_Functions
 * @{
 */

/**
 * @brief  USBH_HID_InterfaceInit
 *         The function init the HID class.
 * @param  pdev: Selected device
 * @param  hdev: Selected device property
 * @retval  USBH_Status :Response for USB HID driver initialization
 */
static USBH_Status USBH_HID_InterfaceInit(USB_OTG_CORE_HANDLE *pdev, void *phost)
{
    USBH_HOST *pphost = phost;
    uint8_t maxEP;
    uint8_t num = 0;
    USBH_Status status = USBH_BUSY;
    HID_Machine.state = HID_ERROR;

    printf(" === HID Interface Init Detailed === \r\n");
    printf(" bInterfaceNumber: %d\r\n", pphost->device_prop.Itf_Desc[0].bInterfaceNumber);
    printf(" bAlternateSetting: %d\r\n", pphost->device_prop.Itf_Desc[0].bAlternateSetting);
    printf(" bInterfaceClass: 0x%02X\r\n", pphost->device_prop.Itf_Desc[0].bInterfaceClass);
    printf(" bInterfaceSubClass: 0x%02X\r\n", pphost->device_prop.Itf_Desc[0].bInterfaceSubClass);
    printf(" bInterfaceProtocol: 0x%02X\r\n", pphost->device_prop.Itf_Desc[0].bInterfaceProtocol);
    printf(" bNumEndpoints: %d\r\n", pphost->device_prop.Itf_Desc[0].bNumEndpoints);
//	// 在 USBH_HID_InterfaceInit 中添加设备特定识别
	if (pphost->device_prop.Dev_Desc.idVendor == 0x045E && 
		pphost->device_prop.Dev_Desc.idProduct == 0x028E) {
		printf(" Microsoft Xbox Controller detected - applying special initialization\n");
		
		// Xbox控制器可能需要不同的设置
		HID_Machine.poll = 4;  // 增加轮询间隔
		printf(" Adjusted poll interval for Xbox controller: %d\n", HID_Machine.poll);
	}
    // 标准HID设备（游戏手柄）
    if (pphost->device_prop.Itf_Desc[0].bInterfaceSubClass == 0x00) {
        printf(" Standard HID Gamepad detected\r\n");
        HID_Machine.cb = &HID_GAMEPAD_cb;
    }
    // 自定义HID设备（特殊游戏手柄）
    else if (pphost->device_prop.Itf_Desc[0].bInterfaceSubClass == 0x5D || 
             pphost->device_prop.Itf_Desc[0].bInterfaceSubClass == 93) {
        printf(" Custom HID Gamepad detected (SubClass: 0x%02X, Protocol: 0x%02X)\r\n",
               pphost->device_prop.Itf_Desc[0].bInterfaceSubClass,
               pphost->device_prop.Itf_Desc[0].bInterfaceProtocol);
        HID_Machine.cb = &HID_GAMEPAD_cb;
    }
    else {
        printf(" Unsupported HID device\r\n");
        pphost->usr_cb->DeviceNotSupported();
        return USBH_FAIL;
    }

    HID_Machine.state = HID_IDLE;
    HID_Machine.ctl_state = HID_REQ_IDLE;
    
    // 使用自动检测的值
    HID_Machine.ep_addr = pphost->device_prop.Ep_Desc[0][0].bEndpointAddress;
    HID_Machine.length = pphost->device_prop.Ep_Desc[0][0].wMaxPacketSize;
    HID_Machine.poll = pphost->device_prop.Ep_Desc[0][0].bInterval;
	
    printf(" Endpoint Configuration:\r\n");
    printf("  Addr: 0x%02X\r\n", HID_Machine.ep_addr);
    printf("  Max Packet Size: %d\r\n", HID_Machine.length);
    printf("  Poll Interval: %d\r\n", HID_Machine.poll);

    // 详细端点扫描
    maxEP = ((pphost->device_prop.Itf_Desc[0].bNumEndpoints <= USBH_MAX_NUM_ENDPOINTS) ? 
             pphost->device_prop.Itf_Desc[0].bNumEndpoints : USBH_MAX_NUM_ENDPOINTS);
    
    printf(" Scanning %d endpoints:\r\n", maxEP);
    
    for (num = 0; num < maxEP; num++) {
        uint8_t ep_addr = pphost->device_prop.Ep_Desc[0][num].bEndpointAddress;
        uint16_t max_packet = pphost->device_prop.Ep_Desc[0][num].wMaxPacketSize;
        uint8_t interval = pphost->device_prop.Ep_Desc[0][num].bInterval;
        uint8_t ep_type = pphost->device_prop.Ep_Desc[0][num].bmAttributes & 0x03;
        uint8_t ep_dir = (ep_addr & 0x80) ? 1 : 0;
        
        printf("  EP%d: Addr=0x%02X, Dir=%s, Type=%d, MaxPacket=%d, Interval=%d\r\n",
               num, ep_addr, 
               ep_dir ? "IN" : "OUT",
               ep_type, max_packet, interval);

        if (ep_dir) { // IN endpoint
            HID_Machine.HIDIntInEp = ep_addr;
            HID_Machine.hc_num_in = USBH_Alloc_Channel(pdev, ep_addr);
            
            if (HID_Machine.hc_num_in != 0) {
                USBH_Open_Channel(pdev, HID_Machine.hc_num_in,
                                pphost->device_prop.address,
                                pphost->device_prop.speed,
                                EP_TYPE_INTR,
                                max_packet);
                printf("    Configured IN Endpoint: HC=%d\r\n", HID_Machine.hc_num_in);
            }
        } else { // OUT endpoint  
            HID_Machine.HIDIntOutEp = ep_addr;
            HID_Machine.hc_num_out = USBH_Alloc_Channel(pdev, ep_addr);
            
            if (HID_Machine.hc_num_out != 0) {
                USBH_Open_Channel(pdev, HID_Machine.hc_num_out,
                                pphost->device_prop.address,
                                pphost->device_prop.speed,
                                EP_TYPE_INTR,
                                max_packet);
                printf("    Configured OUT Endpoint: HC=%d\r\n", HID_Machine.hc_num_out);
            }
        }
    }

    start_toggle = 0;
    status = USBH_OK;
    printf(" HID Interface Init Complete - Status: %d\r\n", status);
    return status;
}



/**
 * @brief  USBH_HID_InterfaceDeInit
 *         The function DeInit the Host Channels used for the HID class.
 * @param  pdev: Selected device
 * @param  hdev: Selected device property
 * @retval None
 */
void USBH_HID_InterfaceDeInit(USB_OTG_CORE_HANDLE *pdev,
                              void *phost)
{
  // USBH_HOST *pphost = phost;

  if (HID_Machine.hc_num_in != 0x00)
  {
    USB_OTG_HC_Halt(pdev, HID_Machine.hc_num_in);
    USBH_Free_Channel(pdev, HID_Machine.hc_num_in);
    HID_Machine.hc_num_in = 0; /* Reset the Channel as Free */
  }

  if (HID_Machine.hc_num_out != 0x00)
  {
    USB_OTG_HC_Halt(pdev, HID_Machine.hc_num_out);
    USBH_Free_Channel(pdev, HID_Machine.hc_num_out);
    HID_Machine.hc_num_out = 0; /* Reset the Channel as Free */
  }

  start_toggle = 0;
  vendor_request_active = 0;
}

/**
 * @brief  USBH_HID_ClassRequest
 *         The function is responsible for handling HID Class requests
 *         for HID class.
 * @param  pdev: Selected device
 * @param  hdev: Selected device property
 * @retval  USBH_Status :Response for USB Set Protocol request
 */
static USBH_Status USBH_HID_ClassRequest(USB_OTG_CORE_HANDLE *pdev, void *phost)
{
    USBH_HOST *pphost = phost;
    USBH_Status status = USBH_BUSY;

    printf("HID Class Request - State: %d\r\n", HID_Machine.ctl_state);

    /* 检查是否有活动的厂商请求 */
    if (vendor_request_active) {
        printf(" Vendor request active, handling vendor request first\r\n");
        USBH_Status vendor_status = USBH_HID_HandleVendorRequest(pdev, pphost, 
                                                                pphost->Control.setup.b.bRequest);
        if (vendor_status != USBH_BUSY) {
            vendor_request_active = 0;
        }
        return vendor_status;
    }

    /* 检查是否为厂商请求 */
    if ((pphost->Control.setup.b.bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_VENDOR) {
        printf(" Vendor request detected: bRequest=0x%02X\r\n", pphost->Control.setup.b.bRequest);
        vendor_request_active = 1;
        return USBH_HID_HandleVendorRequest(pdev, pphost, pphost->Control.setup.b.bRequest);
    }

    /* 原有的类请求处理 */
    switch (HID_Machine.ctl_state) {
    case HID_IDLE:
    case HID_REQ_GET_HID_DESC:
        printf(" Getting HID Descriptor...\r\n");
        if (USBH_Get_HID_Descriptor(pdev, pphost, USB_HID_DESC_SIZE) == USBH_OK) 
		{
            USBH_ParseHIDDesc(&HID_Desc, pdev->host.Rx_Buffer);
            HID_Machine.ctl_state = HID_REQ_GET_REPORT_DESC;
            printf(" HID Descriptor OK\r\n");
        } else {
            printf(" HID Descriptor FAILED\r\n");
        }
        break;
        
    case HID_REQ_GET_REPORT_DESC:
        printf(" Getting Report Descriptor, length=%d...\n", HID_Desc.wItemLength);
        if (USBH_Get_HID_ReportDescriptor(pdev, pphost, HID_Desc.wItemLength) == USBH_OK) {
            HID_Machine.ctl_state = HID_REQ_SET_IDLE;
            printf(" Report Descriptor OK\r\n");
            
            // 对于自定义设备，可能需要特殊处理
            if (pphost->device_prop.Itf_Desc[0].bInterfaceSubClass == 0x5D || 
                pphost->device_prop.Itf_Desc[0].bInterfaceSubClass == 93) {
                printf(" Custom device - may require special initialization\n");
            }
        } else {
            printf(" Report Descriptor FAILED\r\n");
        }
        break;

    case HID_REQ_SET_IDLE:
        printf(" Setting Idle...\r\n");
        if (USBH_Set_Idle(pdev, pphost, 0, 0) == USBH_OK) {
            HID_Machine.ctl_state = HID_REQ_SET_PROTOCOL;
            printf(" Set Idle OK\r\n");
        } else {
            // 对于某些自定义设备，Set Idle可能失败但可以继续
            printf(" Set Idle failed\r\n");
//            HID_Machine.ctl_state = HID_REQ_SET_PROTOCOL;
        }
        break;

    case HID_REQ_SET_PROTOCOL:
	  printf("Setting Protocol...\n");
	  if (USBH_Set_Protocol(pdev, pphost, 0) == USBH_OK) {
		HID_Machine.ctl_state = HID_REQ_IDLE;
		status = USBH_OK;
		printf("HID Class Requests COMPLETED successfully!\n");

		// 针对Xbox One手柄的初始化
		if (pphost->device_prop.Dev_Desc.idVendor == 0x045E && 
			pphost->device_prop.Dev_Desc.idProduct == 0x028E) {
		  printf("Initializing Xbox One controller...\n");
		  // 发送激活命令
		  uint8_t xbox_activate_cmd[] = {0x05, 0x20, 0x00, 0x01, 0x00};
		  USBH_Set_Report(pdev, pphost, 0x21, 0, sizeof(xbox_activate_cmd), xbox_activate_cmd);
		  printf("Xbox activate command sent.\n");
		}
	  }
	  break;

    default:
        break;
    }

    return status;
}

/**
 * @brief  USBH_HID_HandleVendorRequest
 *         处理厂商特定的USB请求
 * @param  pdev: USB设备句柄
 * @param  phost: USB主机状态
 * @param  bRequest: 厂商请求命令
 * @retval USBH_Status
 */
static USBH_Status USBH_HID_HandleVendorRequest(USB_OTG_CORE_HANDLE *pdev,
                                               USBH_HOST *phost,
                                               uint8_t bRequest)
{
    USBH_Status status = USBH_BUSY;
    static uint8_t vendor_phase = 0;
    
    printf(" Handling Vendor Request: 0x%02X, Phase: %d\r\n", bRequest, vendor_phase);

    switch (bRequest) {
    case VENDOR_REQ_GET_VERSION:
        printf(" Vendor: Get Version Request\r\n");
        if (vendor_phase == 0) {
            // 准备版本数据
            vendor_buffer[0] = 0x01; // 主版本
            vendor_buffer[1] = 0x00; // 次版本
            vendor_data_length = 2;
            
            // 发送数据到主机
            phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_VENDOR;
            phost->Control.setup.b.bRequest = VENDOR_REQ_GET_VERSION;
            phost->Control.setup.b.wValue.w = 0;
            phost->Control.setup.b.wIndex.w = 0;
            phost->Control.setup.b.wLength.w = vendor_data_length;
            
            status = USBH_CtlReq(pdev, phost, vendor_buffer, vendor_data_length);
            vendor_phase = 1;
        } else {
            if (USBH_CtlReq(pdev, phost, vendor_buffer, vendor_data_length) == USBH_OK) {
                vendor_phase = 0;
                status = USBH_OK;
                printf(" Vendor: Version sent successfully\r\n");
            }
        }
        break;
        
    case VENDOR_REQ_SET_MODE:
        printf(" Vendor: Set Mode Request\r\n");
        if (vendor_phase == 0) {
            // 接收模式设置数据
            phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_VENDOR;
            phost->Control.setup.b.bRequest = VENDOR_REQ_SET_MODE;
            phost->Control.setup.b.wValue.w = phost->Control.setup.b.wValue.w; // 使用原始值
            phost->Control.setup.b.wIndex.w = phost->Control.setup.b.wIndex.w; // 使用原始值
            phost->Control.setup.b.wLength.w = phost->Control.setup.b.wLength.w;
            
            status = USBH_CtlReq(pdev, phost, vendor_buffer, phost->Control.setup.b.wLength.w);
            vendor_phase = 1;
        } else {
            if (USBH_CtlReq(pdev, phost, vendor_buffer, phost->Control.setup.b.wLength.w) == USBH_OK) {
                // 处理接收到的模式数据
                uint8_t mode = vendor_buffer[0];
                printf(" Vendor: Mode set to %d\r\n", mode);
                vendor_phase = 0;
                status = USBH_OK;
            }
        }
        break;
        
    case VENDOR_REQ_GET_STATUS:
        printf(" Vendor: Get Status Request\r\n");
        if (vendor_phase == 0) {
            // 准备状态数据
            vendor_buffer[0] = 0x01; // 设备状态
            vendor_buffer[1] = 0x00; // 错误代码
            vendor_data_length = 2;
            
            phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_VENDOR;
            phost->Control.setup.b.bRequest = VENDOR_REQ_GET_STATUS;
            phost->Control.setup.b.wValue.w = 0;
            phost->Control.setup.b.wIndex.w = 0;
            phost->Control.setup.b.wLength.w = vendor_data_length;
            
            status = USBH_CtlReq(pdev, phost, vendor_buffer, vendor_data_length);
            vendor_phase = 1;
        } else {
            if (USBH_CtlReq(pdev, phost, vendor_buffer, vendor_data_length) == USBH_OK) {
                vendor_phase = 0;
                status = USBH_OK;
                printf(" Vendor: Status sent successfully\r\n");
            }
        }
        break;
        
    default:
        printf(" Vendor: Unknown request 0x%02X\r\n", bRequest);
        status = USBH_FAIL;
        break;
    }
    
    return status;
}

/**
 * @brief  USBH_HID_SendVendorRequest
 *         发送厂商请求到设备
 * @param  pdev: USB设备句柄
 * @param  phost: USB主机状态
 * @param  bRequest: 厂商请求命令
 * @param  wValue: 请求值
 * @param  wIndex: 请求索引
 * @param  data: 数据缓冲区
 * @param  wLength: 数据长度
 * @retval USBH_Status
 */
static USBH_Status USBH_HID_SendVendorRequest(USB_OTG_CORE_HANDLE *pdev,
                                             USBH_HOST *phost,
                                             uint8_t bRequest,
                                             uint16_t wValue,
                                             uint16_t wIndex,
                                             uint8_t *data,
                                             uint16_t wLength)
{
    USBH_Status status;
    
    printf(" Sending Vendor Request: 0x%02X\r\n", bRequest);
    
    // 设置请求类型和方向
    if (data && wLength > 0) {
        // 主机到设备的请求（有数据发送）
        phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_VENDOR;
    } else {
        // 设备到主机的请求（无数据或接收数据）
        phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_VENDOR;
    }
    
    phost->Control.setup.b.bRequest = bRequest;
    phost->Control.setup.b.wValue.w = wValue;
    phost->Control.setup.b.wIndex.w = wIndex;
    phost->Control.setup.b.wLength.w = wLength;
    
    // 发送控制请求
    status = USBH_CtlReq(pdev, phost, data, wLength);
    
    if (status == USBH_OK) {
        printf(" Vendor request 0x%02X completed successfully\r\n", bRequest);
    } else {
        printf(" Vendor request 0x%02X failed: %d\r\n", bRequest, status);
    }
    
    return status;
}

/**
 * @brief  USBH_HID_Handle
 *         The function is for managing state machine for HID data transfers
 * @param  pdev: Selected device
 * @param  hdev: Selected device property
 * @retval USBH_Status
 */
static USBH_Status USBH_HID_Handle(USB_OTG_CORE_HANDLE *pdev, void *phost)
{
	USBH_HOST *pphost = phost;
	USBH_Status status = USBH_OK;
  static uint8_t last_state = 0xFF;
  static uint32_t state_count = 0;
  
  if (last_state != HID_Machine.state) {
    //printf("HID State Change: %d -> %d (count: %lu)\r\n", last_state, HID_Machine.state, state_count);
    last_state = HID_Machine.state;
    state_count = 0;
  } else {
    state_count++;
  }

  switch (HID_Machine.state) {
    case HID_IDLE:
      //printf("HID_IDLE -> Calling Init()\r\n");
      HID_Machine.cb->Init();
      HID_Machine.state = HID_SYNC;
      break;

    case HID_SYNC:
      if (USB_OTG_IsEvenFrame(pdev) == TRUE) {
        //printf("HID_SYNC -> Even frame detected, moving to HID_GET_DATA\r\n");
        HID_Machine.state = HID_GET_DATA;
      } else {
        //printf("HID_SYNC -> Waiting for even frame...\r\n");
      }
      break;

    case HID_GET_DATA:
      //printf("HID_GET_DATA -> Starting interrupt receive, length=%d, hc_num=%d\r\n", HID_Machine.length, HID_Machine.hc_num_in);
      USBH_InterruptReceiveData(pdev, HID_Machine.buff, 
                               HID_Machine.length, HID_Machine.hc_num_in);
      start_toggle = 1;
      HID_Machine.state = HID_POLL;
      HID_Machine.timer = HCD_GetCurrentFrame(pdev);
      break;

    case HID_POLL:
      
		uint32_t current_frame = HCD_GetCurrentFrame(pdev);
		uint32_t frame_diff = current_frame - HID_Machine.timer;
		uint8_t urb_state = HCD_GetURB_State(pdev, HID_Machine.hc_num_in);

		//printf("HID_POLL: frame_diff=%lu, poll=%d, urb_state=%d\n", frame_diff, HID_Machine.poll, urb_state);

		if (frame_diff >= HID_Machine.poll)
		{
			//printf("HID_POLL -> Poll timeout, returning to HID_GET_DATA\n");
			// 先停止当前通道，确保URB被重置
			USB_OTG_HC_Halt(pdev, HID_Machine.hc_num_in);
			// 然后重新打开通道
			USBH_Open_Channel(pdev,
							 HID_Machine.hc_num_in,
							 pphost->device_prop.address,
							 pphost->device_prop.speed,
							 EP_TYPE_INTR,
							 HID_Machine.length);
			HID_Machine.state = HID_GET_DATA;
		}
		else if (urb_state == URB_DONE)
		{
			if (start_toggle == 1)
			{
				start_toggle = 0;
				//printf("HID_POLL -> Data received! Length: %d\n", HID_Machine.length);
				for(int i = 0; i < HID_Machine.length; i++) {
					//printf("%02X ", HID_Machine.buff[i]);
				}
				//printf("\n");
				HID_Machine.cb->Decode(HID_Machine.buff);
			}
		}
		else if (urb_state == URB_STALL)
		{
			//printf("HID_POLL -> Endpoint stalled, clearing feature...\n");
			if (USBH_ClrFeature(pdev,
							   pphost,
							   HID_Machine.ep_addr,
							   HID_Machine.hc_num_in) == USBH_OK)
			{
				HID_Machine.state = HID_GET_DATA;
			}
		}
		// 如果URB_BUSY状态持续了很长时间，我们也可以考虑强制重启
		else if (urb_state == URB_ERROR && frame_diff > HID_Machine.poll * 2)
		{
			//printf("HID_POLL -> URB_BUSY for too long, forcing restart\n");
			USB_OTG_HC_Halt(pdev, HID_Machine.hc_num_in);
			USBH_Open_Channel(pdev,
							 HID_Machine.hc_num_in,
							 pphost->device_prop.address,
							 pphost->device_prop.speed,
							 EP_TYPE_INTR,
							 HID_Machine.length);
			HID_Machine.state = HID_GET_DATA;
		}
		break;

    default:
      printf("HID Unknown State: %d\r\n", HID_Machine.state);
      break;
  }
  return status;
}
//static USBH_Status USBH_HID_Handle(USB_OTG_CORE_HANDLE *pdev, void *phost)
//{
//    USBH_HOST *pphost = phost;
//    USBH_Status status = USBH_OK;
//    static uint8_t init_done = 0;

//    switch (HID_Machine.state) {
//    case HID_IDLE:
//        printf(" HID_IDLE -> Calling device init\r\n");
//        if (!init_done) {
//            HID_Machine.cb->Init();  // 这会调用 USR_GAMEPAD_Init
//            
//			// Xbox控制器特殊初始化
//			if (pphost->device_prop.Dev_Desc.idVendor == 0x045E && 
//				pphost->device_prop.Dev_Desc.idProduct == 0x028E) {
//				printf(" Initializing Xbox Controller\n");
//				
//				if (HID_Machine.HIDIntOutEp != 0) {
//					// Xbox控制器特定的初始化命令
//					uint8_t xbox_init_cmds[][32] = {
//						// Xbox控制器激活序列
//						{0x08, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x00, 0x00,
//						 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//						 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//						 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
//						 
//						{0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//						 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//						 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//						 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
//						 
//						{0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//						 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//						 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//						 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
//					};
//					
//					for (int i = 0; i < 3; i++) {
//						printf(" Sending Xbox init command %d\n", i);
//						USBH_InterruptSendData(pdev, xbox_init_cmds[i], 
//											  sizeof(xbox_init_cmds[i]), 
//											  HID_Machine.hc_num_out);
//						Delay_ms(50);
//					}
//				}
//			} 
//			else 
//			{
//				// 其他设备的通用激活命令
//				if (HID_Machine.HIDIntOutEp != 0) {
//					printf(" Sending generic activation command\n");
//					uint8_t activate_cmd[32] = {0x01};
//					USBH_InterruptSendData(pdev, activate_cmd, sizeof(activate_cmd), 
//										  HID_Machine.hc_num_out);
//				}
//			}
//            init_done = 1;
//        }
//        HID_Machine.state = HID_SYNC;
//        break;
//        
//    case HID_SYNC:
//        printf(" HID_SYNC -> Waiting for even frame\r\n");
//        if (USB_OTG_IsEvenFrame(pdev) == TRUE) {
//            HID_Machine.state = HID_GET_DATA;
//            printf(" Even frame detected -> Moving to HID_GET_DATA\r\n");
//        }
//        break;

//    case HID_GET_DATA:
//        printf(" HID_GET_DATA -> Starting interrupt receive on EP 0x%02X\r\n", 
//               HID_Machine.HIDIntInEp);
//        
//        URB_STATE current_state = HCD_GetURB_State(pdev, HID_Machine.hc_num_in);
//        printf(" Current URB state: %d\r\n", current_state);
//        
//        if (current_state == URB_IDLE || current_state == URB_DONE || current_state == URB_NOTREADY) {
//            USBH_InterruptReceiveData(pdev,
//                                    HID_Machine.buff,
//                                    HID_Machine.length,
//                                    HID_Machine.hc_num_in);
//            start_toggle = 1;
//            HID_Machine.state = HID_POLL;
//            HID_Machine.timer = HCD_GetCurrentFrame(pdev);
//            printf(" Interrupt receive started successfully\r\n");
//        } else 
//		{
//            printf(" URB busy (state: %d), will retry\r\n", current_state);
//            HID_Machine.timer = HCD_GetCurrentFrame(pdev);
//        }
//        break;

//    case HID_POLL:
//        // 检查轮询超时
//        if ((HCD_GetCurrentFrame(pdev) - HID_Machine.timer) >= HID_Machine.poll) {
//            printf(" HID_POLL -> Poll timeout, restarting transfer\r\n");
//            HID_Machine.state = HID_GET_DATA;
//        }
//        else {
//            URB_STATE urb_state = HCD_GetURB_State(pdev, HID_Machine.hc_num_in);
//            
//            if (urb_state == URB_DONE) {
//                printf(" HID_POLL -> Data received! Processing...\r\n");
////                if (start_toggle == 1) {
////                    start_toggle = 0;
//                    HID_Machine.cb->Decode(HID_Machine.buff);
//                    // 立即重新启动传输
//                    HID_Machine.state = HID_GET_DATA;
////                }
//            }
//            else if (urb_state == URB_STALL) {
//                printf(" HID_POLL -> Endpoint STALLED, attempting recovery...\r\n");
//                
//                if (USBH_ClrFeature(pdev, pphost, HID_Machine.HIDIntInEp, HID_Machine.hc_num_in) == USBH_OK) {
//                    printf(" STALL cleared, resetting endpoint\r\n");
//                    
//                    // 重新打开通道
//                    USBH_Open_Channel(pdev,
//                                    HID_Machine.hc_num_in,
//                                    pphost->device_prop.address,
//                                    pphost->device_prop.speed,
//                                    EP_TYPE_INTR,
//                                    HID_Machine.length);
//                    
//                    // 重置数据toggle
////                    USBH_ResetToggle(pdev, HID_Machine.HIDIntInEp);
//                    
//                    HID_Machine.state = HID_GET_DATA;
//                } else {
//                    printf(" Failed to clear STALL, retrying...\r\n");
//                    HID_Machine.state = HID_GET_DATA;
//                }
//            }
//            else if (urb_state == URB_ERROR) {
//                printf(" HID_POLL -> Transfer error, restarting...\r\n");
//                HID_Machine.state = HID_GET_DATA;
//            }
//        }
//        break;

//    default:
//        break;
//    }
//    
//    return status;
//}

/**
 * @brief  USBH_Get_HID_ReportDescriptor
 *         Issue report Descriptor command to the device. Once the response
 *         received, parse the report descriptor and update the status.
 * @param  pdev   : Selected device
 * @param  Length : HID Report Descriptor Length
 * @retval USBH_Status : Response for USB HID Get Report Descriptor Request
 */
static USBH_Status USBH_Get_HID_ReportDescriptor(USB_OTG_CORE_HANDLE *pdev,
                                                 USBH_HOST *phost,
                                                 uint16_t length)
{

  USBH_Status status;

  status = USBH_GetDescriptor(pdev,
                              phost,
                              USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_STANDARD,
                              USB_DESC_HID_REPORT,
                              pdev->host.Rx_Buffer,
                              length);

  /* HID report descriptor is available in pdev->host.Rx_Buffer.
  In case of USB Boot Mode devices for In report handling ,
  HID report descriptor parsing is not required.
  In case, for supporting Non-Boot Protocol devices and output reports,
  user may parse the report descriptor*/

  return status;
}

/**
 * @brief  USBH_Get_HID_Descriptor
 *         Issue HID Descriptor command to the device. Once the response
 *         received, parse the report descriptor and update the status.
 * @param  pdev   : Selected device
 * @param  Length : HID Descriptor Length
 * @retval USBH_Status : Response for USB HID Get Report Descriptor Request
 */
static USBH_Status USBH_Get_HID_Descriptor(USB_OTG_CORE_HANDLE *pdev,
                                           USBH_HOST *phost,
                                           uint16_t length)
{

  USBH_Status status;

  status = USBH_GetDescriptor(pdev,
                              phost,
                              USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_STANDARD,
                              USB_DESC_HID,
                              pdev->host.Rx_Buffer,
                              length);

  return status;
}

/**
 * @brief  USBH_Set_Idle
 *         Set Idle State.
 * @param  pdev: Selected device
 * @param  duration: Duration for HID Idle request
 * @param  reportID : Targeted report ID for Set Idle request
 * @retval USBH_Status : Response for USB Set Idle request
 */
static USBH_Status USBH_Set_Idle(USB_OTG_CORE_HANDLE *pdev,
                                 USBH_HOST *phost,
                                 uint8_t duration,
                                 uint8_t reportId)
{

  phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE |
                                         USB_REQ_TYPE_CLASS;

  phost->Control.setup.b.bRequest = USB_HID_SET_IDLE;
  phost->Control.setup.b.wValue.w = (duration << 8) | reportId;

  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = 0;

  return USBH_CtlReq(pdev, phost, 0, 0);
}

/**
 * @brief  USBH_Set_Report
 *         Issues Set Report
 * @param  pdev: Selected device
 * @param  reportType  : Report type to be sent
 * @param  reportID    : Targeted report ID for Set Report request
 * @param  reportLen   : Length of data report to be send
 * @param  reportBuff  : Report Buffer
 * @retval USBH_Status : Response for USB Set Idle request
 */
USBH_Status USBH_Set_Report(USB_OTG_CORE_HANDLE *pdev,
                            USBH_HOST *phost,
                            uint8_t reportType,
                            uint8_t reportId,
                            uint8_t reportLen,
                            uint8_t *reportBuff)
{

  phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE |
                                         USB_REQ_TYPE_CLASS;

  phost->Control.setup.b.bRequest = USB_HID_SET_REPORT;
  phost->Control.setup.b.wValue.w = (reportType << 8) | reportId;

  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = reportLen;

  return USBH_CtlReq(pdev, phost, reportBuff, reportLen);
}

/**
 * @brief  USBH_Set_Protocol
 *         Set protocol State.
 * @param  pdev: Selected device
 * @param  protocol : Set Protocol for HID : boot/report protocol
 * @retval USBH_Status : Response for USB Set Protocol request
 */
static USBH_Status USBH_Set_Protocol(USB_OTG_CORE_HANDLE *pdev,
                                     USBH_HOST *phost,
                                     uint8_t protocol)
{

  phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE |
                                         USB_REQ_TYPE_CLASS;

  phost->Control.setup.b.bRequest = USB_HID_SET_PROTOCOL;

  if (protocol != 0)
  {
    /* Boot Protocol */
    phost->Control.setup.b.wValue.w = 0;
  }
  else
  {
    /*Report Protocol*/
    phost->Control.setup.b.wValue.w = 1;
  }

  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = 0;

  return USBH_CtlReq(pdev, phost, 0, 0);
}

/**
 * @brief  USBH_ParseHIDDesc
 *         This function Parse the HID descriptor
 * @param  buf: Buffer where the source descriptor is available
 * @retval None
 */
static void USBH_ParseHIDDesc(USBH_HIDDesc_TypeDef *desc, uint8_t *buf)
{

  desc->bLength = *(uint8_t *)(buf + 0);
  desc->bDescriptorType = *(uint8_t *)(buf + 1);
  desc->bcdHID = LE16(buf + 2);
  desc->bCountryCode = *(uint8_t *)(buf + 4);
  desc->bNumDescriptors = *(uint8_t *)(buf + 5);
  desc->bReportDescriptorType = *(uint8_t *)(buf + 6);
  desc->wItemLength = LE16(buf + 7);
}

/**
 * @brief  USBH_HID_SendVendorCommand
 *         外部函数：发送厂商命令到HID设备
 * @param  pdev: USB设备句柄
 * @param  phost: USB主机状态
 * @param  command: 厂商命令
 * @param  data: 数据缓冲区
 * @param  length: 数据长度
 * @retval USBH_Status
 */
USBH_Status USBH_HID_SendVendorCommand(USB_OTG_CORE_HANDLE *pdev,
                                      USBH_HOST *phost,
                                      uint8_t command,
                                      uint8_t *data,
                                      uint16_t length)
{
    return USBH_HID_SendVendorRequest(pdev, phost, command, 0, 0, data, length);
}

/**
 * @brief  USBH_HID_GetVendorData
 *         外部函数：从HID设备获取厂商数据
 * @param  pdev: USB设备句柄
 * @param  phost: USB主机状态
 * @param  command: 厂商命令
 * @param  data: 数据缓冲区
 * @param  length: 数据长度
 * @retval USBH_Status
 */
USBH_Status USBH_HID_GetVendorData(USB_OTG_CORE_HANDLE *pdev,
                                  USBH_HOST *phost,
                                  uint8_t command,
                                  uint8_t *data,
                                  uint16_t length)
{
    return USBH_HID_SendVendorRequest(pdev, phost, command, 0, 0, data, length);
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/