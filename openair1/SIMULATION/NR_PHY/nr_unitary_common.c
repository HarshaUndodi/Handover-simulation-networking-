/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!\file openair1/SIMULATION/NR_PHY/nr_unitary_common.c
 * \brief common functions to NR PHY unitary tests
 * \company EURECOM
 */
#include "nfapi/oai_integration/vendor_ext.h"
#include "nfapi/open-nFAPI/nfapi/public_inc/nfapi_nr_interface_scf.h"
#include "openair2/NR_PHY_INTERFACE/NR_IF_Module.h"
#include "openair2/NR_UE_PHY_INTERFACE/NR_IF_Module.h"
#include "openair1/PHY/defs_nr_UE.h"
#include "position_interface.h"

// Define signal handler to attempt graceful termination
bool stop = false;

void sigint_handler(int arg)
{
  static const char msg[] = "\nCaught SIGINT, shutting down\nPress again CTRL+C to force shutdown\n";
  __attribute__((unused)) int unused = write(STDOUT_FILENO, msg, sizeof(msg) - 1);
  UNUSED(arg);
  stop = true;
}

int oai_nfapi_nr_crc_indication(nfapi_nr_crc_indication_t *ind)
{
  UNUSED(ind);
  return (0);
}
int oai_nfapi_nr_srs_indication(nfapi_nr_srs_indication_t *ind)
{
  UNUSED(ind);
  return (0);
}
int oai_nfapi_nr_uci_indication(nfapi_nr_uci_indication_t *ind)
{
  UNUSED(ind);
  return (0);
}
int oai_nfapi_nr_rach_indication(nfapi_nr_rach_indication_t *ind)
{
  UNUSED(ind);
  return (0);
}
int oai_nfapi_nr_rx_data_indication(nfapi_nr_rx_data_indication_t *ind)
{
  UNUSED(ind);
  return 0;
}

void handle_nr_slot_ind(uint16_t sfn, uint16_t slot)
{
  UNUSED(sfn);
  UNUSED(slot);
}

int pack_nr_srs_beamforming_report(void *pMessageBuf, void *pPackedBuf, uint32_t packedBufLen)
{
  UNUSED(pMessageBuf);
  UNUSED(pPackedBuf);
  UNUSED(packedBufLen);
  return 0;
}
int unpack_nr_srs_beamforming_report(void *pMessageBuf, uint32_t messageBufLen, void *pUnpackedBuf, uint32_t unpackedBufLen)
{
  UNUSED(pMessageBuf);
  UNUSED(messageBufLen);
  UNUSED(pUnpackedBuf);
  UNUSED(unpackedBufLen);
  return 0;
}
int pack_nr_srs_normalized_channel_iq_matrix(void *pMessageBuf, void *pPackedBuf, uint32_t packedBufLen)
{
  UNUSED(pMessageBuf);
  UNUSED(pPackedBuf);
  UNUSED(packedBufLen);
  return 0;
}
int unpack_nr_srs_normalized_channel_iq_matrix(void *pMessageBuf,
                                               uint32_t messageBufLen,
                                               void *pUnpackedBuf,
                                               uint32_t unpackedBufLen)
{
  UNUSED(pMessageBuf);
  UNUSED(messageBufLen);
  UNUSED(pUnpackedBuf);
  UNUSED(unpackedBufLen);
  return 0;
}

void configure_nr_nfapi_pnf(char *vnf_ip_addr, int vnf_p5_port, char *pnf_ip_addr, int pnf_p7_port, int vnf_p7_port)
{
  UNUSED(vnf_ip_addr);
  UNUSED(vnf_p5_port);
  UNUSED(pnf_ip_addr);
  UNUSED(pnf_p7_port);
  UNUSED(vnf_p7_port);
}
void configure_nr_nfapi_vnf(eth_params_t params)
{
  UNUSED(params);
}
int nfapi_nr_p7_message_pack(void *pMessageBuf, void *pPackedBuf, uint32_t packedBufLen, nfapi_p7_codec_config_t *config)
{
  UNUSED(pMessageBuf);
  UNUSED(pPackedBuf);
  UNUSED(packedBufLen);
  UNUSED(config);
  return 0;
}
bool nfapi_nr_p7_message_unpack(void *pMessageBuf,
                               uint32_t messageBufLen,
                               void *pUnpackedBuf,
                               uint32_t unpackedBufLen,
                               nfapi_p7_codec_config_t *config)
{
  UNUSED(pMessageBuf);
  UNUSED(messageBufLen);
  UNUSED(pUnpackedBuf);
  UNUSED(unpackedBufLen);
  UNUSED(config);
  return false;
}
int nfapi_p7_message_header_unpack(void *pMessageBuf,
                                   uint32_t messageBufLen,
                                   void *pUnpackedBuf,
                                   uint32_t unpackedBufLen,
                                   nfapi_p7_codec_config_t *config)
{
  UNUSED(pMessageBuf);
  UNUSED(messageBufLen);
  UNUSED(pUnpackedBuf);
  UNUSED(unpackedBufLen);
  UNUSED(config);
  return 0;
}
