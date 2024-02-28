/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name        : Pin.c
* Version          : 1.0.2
* Device(s)        : R5F572NNDxBD
* Description      : This file implements SMC pin code generation.
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Pins_Create
* Description  : This function initializes Smart Configurator pins
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Pins_Create(void)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

    /* Set A0 pin */
    PORTA.PMR.BYTE &= 0xFEU;

    /* Set A1 pin */
    PORTA.PMR.BYTE &= 0xFDU;

    /* Set A2 pin */
    PORTA.PMR.BYTE &= 0xFBU;

    /* Set A3 pin */
    PORTA.PMR.BYTE &= 0xF7U;

    /* Set A4 pin */
    PORTA.PMR.BYTE &= 0xEFU;

    /* Set A5 pin */
    PORTA.PMR.BYTE &= 0xDFU;

    /* Set A6 pin */
    PORTA.PMR.BYTE &= 0xBFU;

    /* Set A7 pin */
    PORTA.PMR.BYTE &= 0x7FU;

    /* Set A8 pin */
    PORTB.PMR.BYTE &= 0xFEU;

    /* Set A9 pin */
    PORTB.PMR.BYTE &= 0xFDU;

    /* Set A10 pin */
    PORTB.PMR.BYTE &= 0xFBU;

    /* Set A11 pin */
    PORTB.PMR.BYTE &= 0xF7U;

    /* Set A12 pin */
    PORTB.PMR.BYTE &= 0xEFU;

    /* Set A13 pin */
    PORTB.PMR.BYTE &= 0xDFU;

    /* Set A14 pin */
    PORTB.PMR.BYTE &= 0xBFU;

    /* Set A15 pin */
    PORTB.PMR.BYTE &= 0x7FU;

    /* Set CAS# pin */
    PORT6.PMR.BYTE &= 0xF7U;

    /* Set CKE pin */
    PORT6.PMR.BYTE &= 0xDFU;

    /* Set D0 pin */
    PORTD.PMR.BYTE &= 0xFEU;

    /* Set D1 pin */
    PORTD.PMR.BYTE &= 0xFDU;

    /* Set D2 pin */
    PORTD.PMR.BYTE &= 0xFBU;

    /* Set D3 pin */
    PORTD.PMR.BYTE &= 0xF7U;

    /* Set D4 pin */
    PORTD.PMR.BYTE &= 0xEFU;

    /* Set D5 pin */
    PORTD.PMR.BYTE &= 0xDFU;

    /* Set D6 pin */
    PORTD.PMR.BYTE &= 0xBFU;

    /* Set D7 pin */
    PORTD.PMR.BYTE &= 0x7FU;

    /* Set D8 pin */
    PORTE.PMR.BYTE &= 0xFEU;

    /* Set D9 pin */
    PORTE.PMR.BYTE &= 0xFDU;

    /* Set D10 pin */
    PORTE.PMR.BYTE &= 0xFBU;

    /* Set D11 pin */
    PORTE.PMR.BYTE &= 0xF7U;

    /* Set D12 pin */
    PORTE.PMR.BYTE &= 0xEFU;

    /* Set D13 pin */
    PORTE.PMR.BYTE &= 0xDFU;

    /* Set D14 pin */
    PORTE.PMR.BYTE &= 0xBFU;

    /* Set D15 pin */
    PORTE.PMR.BYTE &= 0x7FU;

    /* Set DQM0 pin */
    PORT6.PMR.BYTE &= 0xBFU;

    /* Set DQM1 pin */
    PORT6.PMR.BYTE &= 0x7FU;

    /* Set RAS# pin */
    PORT6.PMR.BYTE &= 0xFBU;

    /* Set SDCLK pin */
    PORT7.PMR.BYTE |= 0x01U;

    /* Set SDCS# pin */
    PORT6.PMR.BYTE &= 0xFDU;

    /* Set USB0_VBUS pin */
    MPC.P16PFS.BYTE = 0x11U;
    PORT1.PMR.BYTE |= 0x40U;

    /* Set WE# pin */
    PORT6.PMR.BYTE &= 0xEFU;

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}

