/*******************************************************************************
* ------------------------------------------------------------------------------
*
* Copyright (c), 2013 Cypress Semiconductor.
*
*          ALL RIGHTS RESERVED
*
********************************************************************************
*
* File Name: nfctransport.h
*
* Abstract:
*   Header for the NFC transport class.
*******************************************************************************/

#ifndef __NFC_TRANSPORT_H_
#define __NFC_TRANSPORT_H_

#include "hidconfig.h"
#include "bthidtransport.h"
#include "gpiodriver.h"
#include "peripheral.h"
#include "apptimer.h"
#include "assertpanic.h"
#include "hidapp.h"

#ifndef NFC_DEBUG
//#define NFC_DEBUG
#endif

#ifdef NFC_DEBUG
#include "dbg_uart.h"
#define REG32(ADDR)  *((volatile UINT32*)(ADDR))
#endif

class NfcTransport : public Peripheral
{
    // Let NfcAppDriver be a friend so we can share things.
    friend class NfcAppDriver;

/*******************************************************************************
* Public Methods
*******************************************************************************/
public:
    // Constructor
    NfcTransport(void (*callback)(void*), void* parent,
                 BYTE activeSet = NFC_INT_REQ_ASSERT_H,
                 BYTE edgeTrig = GPIO_EN_INT_RISING_EDGE);

    // Destructor
    ~NfcTransport();

    // Initialize block, NFC Transport, and chip.
    virtual void init(void);

    // Resets and NFC chip and cleans up.
    virtual void nfcReset(void);

    // Reads NFC data from the NVRAM of the Bluetooth chip.
    virtual BYTE readNvRam(BYTE *dataPtr, UINT16 offset, UINT32 length);

    // Writes NFC data to the NVRAM of the Bluetooth chip.
    virtual BYTE writeNvRam(BYTE *dataPtr, UINT16 offset, UINT32 length);

    // Erases the NFC chip's NVRAM. Used especially for serial flash.
    virtual BYTE eraseNvRam(void);

    // Generic read and write wrappers for the app driver class.
    // Pure virtual. Child classes will make their own implementation.
    virtual BYTE readNfc(BYTE *dataPtr, UINT16 length) = 0;
    virtual BYTE writeNfc(BYTE *dataPtr, UINT16 length) = 0;

/*******************************************************************************
* Public Types
*******************************************************************************/
    enum
    {
        ACTIVE_LOW,
        ACTIVE_HIGH,
        NOT_USED
    } NFC_TOGGLE;

    // NFC_WAKE/RN_INT, When we need to send data,
    // we drive this line to wake up NFC
    enum
    {
        NFC_WAKE_GPIO       =    32,
        NFC_WAKE_PORT       =    NFC_WAKE_GPIO/16,
        NFC_WAKE_PIN        =    NFC_WAKE_GPIO%16,
        NFC_WAKE_ASSERT     =    ACTIVE_LOW,  // active low
        NFC_WAKE_DEASSERT   =    ACTIVE_HIGH,
        NFC_SLEEP_TIME      =    500     // in ms
    };

    // REG_EN
    enum
    {
        NFC_REG_PU_GPIO       =  35,
        NFC_REG_PU_PORT       =  NFC_REG_PU_GPIO/16,
        NFC_REG_PU_PIN        =  NFC_REG_PU_GPIO%16,
        NFC_REG_PU_ASSERT     =  ACTIVE_HIGH,
        NFC_REG_PU_DEASSERT   =  ACTIVE_LOW
    };

    // The HOST_WAKE line stays high until after the first byte is read by
    // the host.
    enum
    {
        NFC_INT_REQ_GPIO    =    33,
        NFC_INT_REQ_PORT    =    NFC_INT_REQ_GPIO/16,
        NFC_INT_REQ_PIN     =    NFC_INT_REQ_GPIO%16,
        NFC_INT_REQ_ASSERT_H  =    ACTIVE_HIGH,
        NFC_INT_REQ_ASSERT_L  =    ACTIVE_LOW,
    };

/*******************************************************************************
* Protected Methods
*******************************************************************************/
protected:
    virtual void startNfcIdleTimer(UINT16 idleExpiryTime);

    virtual void stopNfcIdleTimer();

    virtual BYTE getNfcWakePinStatus(void);

    virtual void nfcWake(BYTE active, BYTE manualOverride = FALSE,
                         BYTE manualOverrideVal = FALSE);

    virtual void nfcHwShutDown(void);

    // Returns the "enabled" switch variable value from the Peripheral class.
    virtual BYTE getEnableSwitchStatus(void);

/*******************************************************************************
* Protected Types
*******************************************************************************/
    // We have three main bus types. You can add more later if desired.
    //    I2C_BUS,
    //    SPI_BUS,
    //    UART_BUS,

    // Timer to deactivate NFC_Wakeup
    AppTimer *allowNfcDevSleepTimer;

/*******************************************************************************
* Private Methods
*******************************************************************************/
};

#endif //__NFC_TRANSPORT_H_
