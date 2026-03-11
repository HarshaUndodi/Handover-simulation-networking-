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

void nr_mac_rrc_sync_ind(const module_id_t module_id, const frame_t frame, const bool in_sync)
{
  UNUSED(module_id);
  UNUSED(frame);
  UNUSED(in_sync);
}

void nr_mac_rrc_msg3_ind(const module_id_t mod_id, int rnti, int gnb_id)
{
  UNUSED(mod_id);
  UNUSED(gnb_id);
  UNUSED(rnti);
}

void nr_mac_rrc_ra_ind(const module_id_t mod_id, bool success)
{
  UNUSED(mod_id);
  UNUSED(success);
}

void nr_mac_rrc_inactivity_timer_ind(const module_id_t mod_id)
{
  UNUSED(mod_id);
}

void rrc_data_ind(const protocol_ctxt_t *const ctxt_pP,
                  const rb_id_t Srb_id,
                  const sdu_size_t sdu_sizeP,
                  const uint8_t *const buffer_pP)
{
  UNUSED(ctxt_pP);
  UNUSED(Srb_id);
  UNUSED(sdu_sizeP);
  UNUSED(buffer_pP);
}

typedef uint32_t channel_t;
int8_t nr_mac_rrc_data_ind_ue(const module_id_t module_id,
                              const int CC_id,
                              const uint8_t gNB_index,
                              const int hfn,
                              const frame_t frame,
                              const int slot,
                              const rnti_t rnti,
                              const uint32_t cellid,
                              const long arfcn,
                              const channel_t channel,
                              const uint8_t *pduP,
                              const sdu_size_t pdu_len)
{
  UNUSED(module_id);
  UNUSED(CC_id);
  UNUSED(gNB_index);
  UNUSED(hfn);
  UNUSED(frame);
  UNUSED(slot);
  UNUSED(rnti);
  UNUSED(arfcn);
  UNUSED(channel);
  UNUSED(pduP);
  UNUSED(pdu_len);
  UNUSED(cellid);
  return 0;
}

void nr_mac_rrc_meas_ind_ue(module_id_t module_id,
                            uint32_t gNB_index,
                            uint16_t Nid_cell,
                            bool csi_meas,
                            bool is_neighboring_cell,
                            int rsrp_dBm)
{
  UNUSED(module_id);
  UNUSED(Nid_cell);
  UNUSED(gNB_index);
  UNUSED(csi_meas);
  UNUSED(is_neighboring_cell);
  UNUSED(rsrp_dBm);
}
