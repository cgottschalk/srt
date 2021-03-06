/*****************************************************************************
 * SRT - Secure, Reliable, Transport
 * Copyright (c) 2017 Haivision Systems Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; If not, see <http://www.gnu.org/licenses/>
 * 
 * Based on UDT4 SDK version 4.11
 *****************************************************************************/

/*****************************************************************************
Copyright (c) 2001 - 2011, The Board of Trustees of the University of Illinois.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the
  above copyright notice, this list of conditions
  and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the University of Illinois
  nor the names of its contributors may be used to
  endorse or promote products derived from this
  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

/*****************************************************************************
written by
   Yunhong Gu, last updated 02/12/2011
modified by
   Haivision Systems Inc.
*****************************************************************************/


//////////////////////////////////////////////////////////////////////////////
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                        Packet Header                          |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                                                               |
//   ~              Data / Control Information Field                 ~
//   |                                                               |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |0|                        Sequence Number                      |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |ff |o|kf |r|               Message Number                      |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                          Time Stamp                           |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                     Destination Socket ID                     |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//   bit 0:
//      0: Data Packet
//      1: Control Packet
//   bit ff:
//      11: solo message packet
//      10: first packet of a message
//      01: last packet of a message
//   bit o:
//      0: in order delivery not required
//      1: in order delivery required
//   bit kf: HaiCrypt Key Flags
//      00: not encrypted
//      01: encrypted with even key
//      10: encrypted with odd key
//   bit r: retransmission flag (set to 1 if this packet was sent again)
//
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |1|            Type             |             Reserved          |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                       Additional Info                         |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                          Time Stamp                           |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                     Destination Socket ID                     |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//   bit 1-15: Message type -- see @a UDTMessageType
//      0: Protocol Connection Handshake (UMSG_HANDSHAKE}
//              Add. Info:    Undefined
//              Control Info: Handshake information (see @a CHandShake)
//      1: Keep-alive (UMSG_KEEPALIVE)
//              Add. Info:    Undefined
//              Control Info: None
//      2: Acknowledgement (UMSG_ACK)
//              Add. Info:    The ACK sequence number
//              Control Info: The sequence number to which (but not include) all the previous packets have beed received
//              Optional:     RTT
//                            RTT Variance
//                            available receiver buffer size (in bytes)
//                            advertised flow window size (number of packets)
//                            estimated bandwidth (number of packets per second)
//      3: Negative Acknowledgement (UMSG_LOSSREPORT)
//              Add. Info:    Undefined
//              Control Info: Loss list (see loss list coding below)
//      4: Congestion/Delay Warning (UMSG_CGWARNING)
//              Add. Info:    Undefined
//              Control Info: None
//      5: Shutdown (UMSG_SHUTDOWN)
//              Add. Info:    Undefined
//              Control Info: None
//      6: Acknowledgement of Acknowledement (UMSG_ACKACK)
//              Add. Info:    The ACK sequence number
//              Control Info: None
//      7: Message Drop Request (UMSG_DROPREQ)
//              Add. Info:    Message ID
//              Control Info: first sequence number of the message
//                            last seqeunce number of the message
//      8: Error Signal from the Peer Side (UMSG_PEERERROR)
//              Add. Info:    Error code
//              Control Info: None
//      0x7FFF: Explained by bits 16 - 31 (UMSG_EXT)
//              
//   bit 16 - 31:
//      This space is used for future expansion or user defined control packets. 
//
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |1|                 Sequence Number a (first)                   |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |0|                 Sequence Number b (last)                    |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |0|                 Sequence Number (single)                    |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//   Loss List Field Coding:
//      For any consectutive lost seqeunce numbers that the differnece between
//      the last and first is more than 1, only record the first (a) and the
//      the last (b) sequence numbers in the loss list field, and modify the
//      the first bit of a to 1.
//      For any single loss or consectutive loss less than 2 packets, use
//      the original sequence numbers in the field.


#include <cstring>
#include "packet.h"


const int CHandShake::m_iContentSize = 48;


// Set up the aliases in the constructure
CPacket::CPacket():
__pad(),
m_iSeqNo((int32_t&)(m_nHeader[PH_SEQNO])),
m_iMsgNo((int32_t&)(m_nHeader[PH_MSGNO])),
m_iTimeStamp((int32_t&)(m_nHeader[PH_TIMESTAMP])),
m_iID((int32_t&)(m_nHeader[PH_ID])),
m_pcData((char*&)(m_PacketVector[PV_DATA].iov_base))
{
   m_nHeader.clear();

   // The part at PV_HEADER will be always set to a builtin buffer
   // containing SRT header.
   m_PacketVector[PV_HEADER].iov_base = m_nHeader.raw();
   m_PacketVector[PV_HEADER].iov_len = HDR_SIZE;

   // The part at PV_DATA is zero-initialized. It should be
   // set (through m_pcData and setLength()) to some externally
   // provided buffer before calling CChannel::sendto().
   m_PacketVector[PV_DATA].iov_base = NULL;
   m_PacketVector[PV_DATA].iov_len = 0;
}

CPacket::~CPacket()
{
}

int CPacket::getLength() const
{
   return m_PacketVector[PV_DATA].iov_len;
}

void CPacket::setLength(int len)
{
   m_PacketVector[PV_DATA].iov_len = len;
}

void CPacket::pack(UDTMessageType pkttype, void* lparam, void* rparam, int size)
{
   // Set (bit-0 = 1) and (bit-1~15 = type)
   m_nHeader[PH_SEQNO] = SEQNO_CONTROL::mask | SEQNO_MSGTYPE::wrap(pkttype);

   // Set additional information and control information field
   switch (pkttype)
   {
   case UMSG_ACK: //0010 - Acknowledgement (ACK)
      // ACK packet seq. no.
      if (NULL != lparam)
         m_nHeader[PH_MSGNO] = *(int32_t *)lparam;

      // data ACK seq. no. 
      // optional: RTT (microsends), RTT variance (microseconds) advertised flow window size (packets), and estimated link capacity (packets per second)
      m_PacketVector[PV_DATA].iov_base = (char *)rparam;
      m_PacketVector[PV_DATA].iov_len = size;

      break;

   case UMSG_ACKACK: //0110 - Acknowledgement of Acknowledgement (ACK-2)
      // ACK packet seq. no.
      m_nHeader[PH_MSGNO] = *(int32_t *)lparam;

      // control info field should be none
      // but "writev" does not allow this
      m_PacketVector[PV_DATA].iov_base = (char *)&__pad; //NULL;
      m_PacketVector[PV_DATA].iov_len = 4; //0;

      break;

   case UMSG_LOSSREPORT: //0011 - Loss Report (NAK)
      // loss list
      m_PacketVector[PV_DATA].iov_base = (char *)rparam;
      m_PacketVector[PV_DATA].iov_len = size;

      break;

   case UMSG_CGWARNING: //0100 - Congestion Warning
      // control info field should be none
      // but "writev" does not allow this
      m_PacketVector[PV_DATA].iov_base = (char *)&__pad; //NULL;
      m_PacketVector[PV_DATA].iov_len = 4; //0;
  
      break;

   case UMSG_KEEPALIVE: //0001 - Keep-alive
      // control info field should be none
      // but "writev" does not allow this
      m_PacketVector[PV_DATA].iov_base = (char *)&__pad; //NULL;
      m_PacketVector[PV_DATA].iov_len = 4; //0;

      break;

   case UMSG_HANDSHAKE: //0000 - Handshake
      // control info filed is handshake info
      m_PacketVector[PV_DATA].iov_base = (char *)rparam;
      m_PacketVector[PV_DATA].iov_len = size; //sizeof(CHandShake);

      break;

   case UMSG_SHUTDOWN: //0101 - Shutdown
      // control info field should be none
      // but "writev" does not allow this
      m_PacketVector[PV_DATA].iov_base = (char *)&__pad; //NULL;
      m_PacketVector[PV_DATA].iov_len = 4; //0;

      break;

   case UMSG_DROPREQ: //0111 - Message Drop Request
      // msg id 
      m_nHeader[PH_MSGNO] = *(int32_t *)lparam;

      //first seq no, last seq no
      m_PacketVector[PV_DATA].iov_base = (char *)rparam;
      m_PacketVector[PV_DATA].iov_len = size;

      break;

   case UMSG_PEERERROR: //1000 - Error Signal from the Peer Side
      // Error type
      m_nHeader[PH_MSGNO] = *(int32_t *)lparam;

      // control info field should be none
      // but "writev" does not allow this
      m_PacketVector[PV_DATA].iov_base = (char *)&__pad; //NULL;
      m_PacketVector[PV_DATA].iov_len = 4; //0;

      break;

   case UMSG_EXT: //0x7FFF - Reserved for user defined control packets
      // for extended control packet
      // "lparam" contains the extended type information for bit 16 - 31
      // "rparam" is the control information
      m_nHeader[PH_SEQNO] |= *(int32_t *)lparam;

      if (NULL != rparam)
      {
         m_PacketVector[PV_DATA].iov_base = (char *)rparam;
         m_PacketVector[PV_DATA].iov_len = size;
      }
      else
      {
         m_PacketVector[PV_DATA].iov_base = (char *)&__pad;
         m_PacketVector[PV_DATA].iov_len = 4;
      }

      break;

   default:
      break;
   }
}

iovec* CPacket::getPacketVector()
{
   return m_PacketVector;
}

UDTMessageType CPacket::getType() const
{
    return UDTMessageType(SEQNO_MSGTYPE::unwrap(m_nHeader[PH_SEQNO]));
}

int CPacket::getExtendedType() const
{
    return SEQNO_EXTTYPE::unwrap(m_nHeader[PH_SEQNO]);
}

int32_t CPacket::getAckSeqNo() const
{
   // read additional information field
   // This field is used only in UMSG_ACK and UMSG_ACKACK,
   // so 'getAckSeqNo' symbolically defines the only use of it
   // in case of CONTROL PACKET.
   return m_nHeader[PH_MSGNO];
}

uint16_t CPacket::getControlFlags() const
{
    // This returns exactly the "extended type" value,
    // which is not used at all in case when the standard
    // type message is interpreted. This can be used to pass
    // additional special flags.
    return SEQNO_EXTTYPE::unwrap(m_nHeader[PH_SEQNO]);
}

PacketBoundary CPacket::getMsgBoundary() const
{
    return PacketBoundary(MSGNO_PACKET_BOUNDARY::unwrap(m_nHeader[PH_MSGNO]));
}

bool CPacket::getMsgOrderFlag() const
{
    return MSGNO_PACKET_INORDER::unwrap(m_nHeader[PH_MSGNO]);
}

int32_t CPacket::getMsgSeq(bool has_rexmit) const
{
    if ( has_rexmit )
    {
        return MSGNO_SEQ::unwrap(m_nHeader[PH_MSGNO]);
    }
    else
    {
        return MSGNO_SEQ_OLD::unwrap(m_nHeader[PH_MSGNO]);
    }
}

bool CPacket::getRexmitFlag() const
{
    // return false; //
    return MSGNO_REXMIT::unwrap(m_nHeader[PH_MSGNO]);
}

EncryptionKeySpec CPacket::getMsgCryptoFlags() const
{
    return EncryptionKeySpec(MSGNO_ENCKEYSPEC::unwrap(m_nHeader[PH_MSGNO]));
}

EncryptionStatus CPacket::encrypt(HaiCrypt_Handle hcrypto)
{
    if ( !hcrypto )
        return ENCS_FAILED;

   int rc = HaiCrypt_Tx_Data(hcrypto, (uint8_t *)m_nHeader.raw(), (uint8_t *)m_pcData, m_PacketVector[PV_DATA].iov_len);
   if ( rc < 0 )
   {
       // -1: encryption failure
       // 0: key not received yet
       return ENCS_FAILED;
   } else if (rc > 0) {
       m_PacketVector[PV_DATA].iov_len = rc;
   }
   return ENCS_CLEAR;
}

EncryptionStatus CPacket::decrypt(HaiCrypt_Handle hcrypto)
{
   if (getMsgCryptoFlags() == EK_NOENC)
       return ENCS_CLEAR; // not encrypted, no need do decrypt, no flags to be modified

   if (!hcrypto)
       return ENCS_FAILED; // "invalid argument" (leave encryption flags untouched)

   int rc = HaiCrypt_Rx_Data(hcrypto, (uint8_t *)m_nHeader.raw(), (uint8_t *)m_pcData, m_PacketVector[PV_DATA].iov_len);
   if ( rc <= 0 )
   {
       // -1: decryption failure
       // 0: key not received yet
       return ENCS_FAILED;
   }
   // Otherwise: rc == decrypted text length.
   m_PacketVector[PV_DATA].iov_len = rc; /* In case clr txt size is different from cipher txt */

   // Decryption succeeded. Update flags.
   m_nHeader[PH_MSGNO] &= ~MSGNO_ENCKEYSPEC::mask; // sets EK_NOENC to ENCKEYSPEC bits.

   return ENCS_CLEAR;
}

#ifdef SRT_ENABLE_TSBPD
uint32_t CPacket::getMsgTimeStamp() const
{
   // SRT_DEBUG_TSBPD_WRAP may enable smaller timestamp for faster wraparoud handling tests
   return (uint32_t)m_nHeader[PH_TIMESTAMP] & TIMESTAMP_MASK;
}
#endif /* SRT_ENABLE_TSBPD */

CPacket* CPacket::clone() const
{
   CPacket* pkt = new CPacket;
   memcpy(pkt->m_nHeader, m_nHeader, HDR_SIZE);
   pkt->m_pcData = new char[m_PacketVector[PV_DATA].iov_len];
   memcpy(pkt->m_pcData, m_pcData, m_PacketVector[PV_DATA].iov_len);
   pkt->m_PacketVector[PV_DATA].iov_len = m_PacketVector[PV_DATA].iov_len;

   return pkt;
}

#if ENABLE_LOGGING
std::string CPacket::MessageFlagStr()
{
    using namespace std;

    stringstream out;

    static const string boundary [] = { "PB_SUBSEQUENT", "PB_LAST", "PB_FIRST", "PB_SOLO" };
    static const string order [] = { "ORD_RELAXED", "ORD_REQUIRED" };
    static const string crypto [] = { "EK_NOENC", "EK_EVEN", "EK_ODD", "EK*ERROR" };
    static const string rexmit [] = { "SN_ORIGINAL", "SN_REXMIT" };

    out << boundary[int(getMsgBoundary())] << " ";
    out << order[int(getMsgOrderFlag())] << " ";
    out << crypto[int(getMsgCryptoFlags())] << " ";
    out << rexmit[int(getRexmitFlag())];

    return out.str();
}
#endif

CHandShake::CHandShake():
m_iVersion(AF_UNSPEC),
m_iType(UDT_UNDEFINED),
m_iISN(0),
m_iMSS(0),
m_iFlightFlagSize(0),
m_iReqType(0),
m_iID(0),
m_iCookie(0)
{
   for (int i = 0; i < 4; ++ i)
      m_piPeerIP[i] = 0;
}

int CHandShake::serialize(char* buf, int& size)
{
   if (size < m_iContentSize)
      return -1;

   int32_t* p = (int32_t*)buf;
   *p++ = m_iVersion;
   *p++ = m_iType;
   *p++ = m_iISN;
   *p++ = m_iMSS;
   *p++ = m_iFlightFlagSize;
   *p++ = m_iReqType;
   *p++ = m_iID;
   *p++ = m_iCookie;
   for (int i = 0; i < 4; ++ i)
      *p++ = m_piPeerIP[i];

   size = m_iContentSize;

   return 0;
}

int CHandShake::deserialize(const char* buf, int size)
{
   if (size < m_iContentSize)
      return -1;

   int32_t* p = (int32_t*)buf;
   m_iVersion = *p++;
   m_iType = *p++;
   m_iISN = *p++;
   m_iMSS = *p++;
   m_iFlightFlagSize = *p++;
   m_iReqType = *p++;
   m_iID = *p++;
   m_iCookie = *p++;
   for (int i = 0; i < 4; ++ i)
      m_piPeerIP[i] = *p++;

   return 0;
}
