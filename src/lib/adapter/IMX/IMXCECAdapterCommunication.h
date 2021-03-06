#pragma once
/*
 * This file is part of the libCEC(R) library.
 *
 * libCEC(R) is Copyright (C) 2011-2013 Pulse-Eight Limited.  All rights reserved.
 * libCEC(R) is an original work, containing original code.
 *
 * libCEC(R) is a trademark of Pulse-Eight Limited.
 *
 * IMX adpater port is Copyright (C) 2013 by Stephan Rafin
 *                     Copyright (C) 2014 by Matus Kral
 * 
 * You can redistribute this file and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 */

#if defined(HAVE_IMX_API)

#include "lib/platform/threads/mutex.h"
#include "lib/platform/threads/threads.h"
#include "lib/platform/sockets/socket.h"
#include "lib/adapter/IMX/mxc_hdmi-cec.h"
#include "lib/adapter/AdapterCommunication.h"
#include <map>

#define IMX_ADAPTER_VID 0x0471 /*FIXME TBD*/
#define IMX_ADAPTER_PID 0x1001

typedef struct hdmi_cec_event{
  uint8_t event_type;
  uint8_t msg_len;
  uint8_t msg[MAX_MESSAGE_LEN];
}hdmi_cec_event;


namespace PLATFORM
{
  class CCDevSocket;
};

namespace CEC
{
  class CIMXCECAdapterMessageQueueEntry;

  class CCECPAChangedReporter : public PLATFORM::CThread
  {
  public:
    CCECPAChangedReporter(IAdapterCommunicationCallback *callback, uint16_t newPA);
    void* Process(void);

  private:
    IAdapterCommunicationCallback *m_callback;
    uint16_t                       m_newPA;
  };

  class CIMXCECAdapterCommunication : public IAdapterCommunication, public PLATFORM::CThread
  {
  public:
    /*!
     * @brief Create a new USB-CEC communication handler.
     * @param callback The callback to use for incoming CEC commands.
     */
    CIMXCECAdapterCommunication(IAdapterCommunicationCallback *callback);
    virtual ~CIMXCECAdapterCommunication(void);

    /** @name IAdapterCommunication implementation */
    ///{
    bool Open(uint32_t iTimeoutMs = CEC_DEFAULT_CONNECT_TIMEOUT, bool bSkipChecks = false, bool bStartListening = true);
    void Close(void);
    bool IsOpen(void);
    std::string GetError(void) const;
    cec_adapter_message_state Write(const cec_command &data, bool &bRetry, uint8_t iLineTimeout, bool bIsReply);

    bool SetLineTimeout(uint8_t UNUSED(iTimeout)) { return true; }
    bool StartBootloader(void) { return false; }
    bool SetLogicalAddresses(const cec_logical_addresses &addresses);
    cec_logical_addresses GetLogicalAddresses(void);
    bool PingAdapter(void) { return IsInitialised(); }
    uint16_t GetFirmwareVersion(void);
    uint32_t GetFirmwareBuildDate(void) { return 0; }
    bool IsRunningLatestFirmware(void) { return true; }
    bool PersistConfiguration(const libcec_configuration & UNUSED(configuration)) { return false; }
    bool GetConfiguration(libcec_configuration & UNUSED(configuration)) { return false; }
    std::string GetPortName(void) { return std::string("IMX"); }
    uint16_t GetPhysicalAddress(void);
    bool SetControlledMode(bool UNUSED(controlled)) { return true; }
    cec_vendor_id GetVendorId(void);
    void HandleLogicalAddressLost(cec_logical_address UNUSED(oldAddress));
    bool SupportsSourceLogicalAddress(const cec_logical_address address) { return address > CECDEVICE_TV && address <= CECDEVICE_BROADCAST; }
    cec_adapter_type GetAdapterType(void) { return ADAPTERTYPE_IMX; }
    uint16_t GetAdapterVendorId(void) const { return IMX_ADAPTER_VID; }
    uint16_t GetAdapterProductId(void) const { return IMX_ADAPTER_PID; }
    void SetActiveSource(bool UNUSED(bSetTo), bool UNUSED(bClientUnregistered)) {}
    ///}

    /** @name PLATFORM::CThread implementation */
    ///{
    void *Process(void);
    ///}

  private:
    bool IsInitialised(void) { return m_bInitialised; };
    bool RegisterLogicalAddress(const cec_logical_address address);
    bool UnregisterLogicalAddress(void);

    std::string                 m_strError; /**< current error message */

    cec_logical_address         m_logicalAddress;
    uint16_t                    m_physicalAddress;

    PLATFORM::CMutex            m_mutex;
    PLATFORM::CCDevSocket       *m_dev;	/**< the device connection */
    
    PLATFORM::CMutex            m_messageMutex;
    uint32_t                    m_iNextMessage;
    std::map<uint32_t, CIMXCECAdapterMessageQueueEntry *> m_messages;

    bool                        m_bLogicalAddressRegistered;
    bool                        m_bInitialised;

    CCECPAChangedReporter       *m_PAReporter;
  };
  
};

#endif
