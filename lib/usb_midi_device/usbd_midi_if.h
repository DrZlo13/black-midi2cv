/**
  ******************************************************************************
  * @file    usbd_midi_if.h
  * @author  Sam Kent
  * @brief   Header for usbd_midi_if.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_MIDI_IF_H
#define __USBD_MIDI_IF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_midi.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

extern USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_MIDI_ItfTypeDef USBD_MIDI_fops;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* __USBD_MIDI_IF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/