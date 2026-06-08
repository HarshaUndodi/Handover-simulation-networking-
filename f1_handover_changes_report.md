# 5G F1 Handover Configuration Changes Report
**Date:** June 8, 2026  
**Status:** Integrated and Verified

This document details all the changes made to the configuration files in the `/home/hpc/openairinterface5g_test/openairinterface5g` directory to resolve the 5G NR PHY synchronization failures and configuration errors during F1 handovers between two Distributed Units (DU0 and DU1).

---

## 1. Summary of Core Issues Solved

### A. Frequency Parsing Integer Overflow (UE)
* **Problem:** Center frequencies (e.g., `3450720000` and `3649440000`) exceed the limit of a signed 32-bit integer (`2,147,483,647`). When the OAI parser loaded them, they overflowed and wrapped around into negative values (e.g., `-844247296`), leading to invalid radio interface setups.
* **Solution:** Appended the `L` suffix (`3450720000L` and `3649440000L`) to the center carrier frequencies in `ue.conf`. This forces the OAI configuration loader to parse the parameters as 64-bit integers.

### B. Single Config Loading Limitation (CU)
* **Problem:** The OAI libconfig backend only processes the first configuration file argument passed to the `-O` flag. Specifying multiple config files (like `-O gnb-cu.conf -O neighbour.conf`) causes the second file to be completely ignored.
* **Solution:** Merged the neighbour list by using the `@include "neighbour-config-rfsim.conf"` directive inside the primary CU configuration file (`gnb-cu.sa.f1.conf`).

### C. Handover Cell ID & Neighbour List Mismatches (CU / DU)
* **Problem:** The CU neighbor list mapping was pointing to incorrect Cell IDs (`720898`) and incorrect gNB IDs (`0xb00` instead of `0xe00`). This mismatch prevented the CU from triggering the handover to DU1.
* **Solution:** Standardized the CU gNB ID to `0xe00` and updated the neighbor lists so DU0 (`12345678L`) lists DU1 (`22222222L`) as a neighbour, and vice-versa, with matching frequencies.

### D. Dual-RU & Dual-rfsimulator Ports (UE)
* **Problem:** The UE config originally defined a single RF interface connected to a single port, preventing the UE from listening to the target cell frequency and switching RF sockets during handover.
* **Solution:** Added dual `cells`, dual `RUs`, and dual `rfsimulator` configurations in `ue.conf`. DU0 maps to rfsimulator port `4043` (serving) and DU1 maps to rfsimulator port `4044` (target).

### E. Telnet Command Server Configuration (CU)
* **Problem:** The telnet configuration block in `gnb-cu.sa.f1.conf` was named `telnet_server_config`, which is not recognized by the OAI telnet library, causing the telnet server to remain inactive.
* **Solution:** Renamed the block to `telnetsrv` to match the exact OAI configuration parameter name, enabling the manual handover trigger (`ci trigger_f1_ho`) via port `9091`.

---

## 2. File-by-File Detailed Changes

### 1. `targets/PROJECTS/GENERIC-NR-5GC/CONF/ue.conf`
Configured the UE with dual cells, dual physical radio units (RUs), and distinct rfsimulator server ports to enable frequency switching. Added the `L` suffix to frequencies.

```diff
 uicc0 = {
-imsi = "2089900007487";
+imsi = "001010000007487";  # MCC=001, MNC=01, MSIN=0000007487 (Matches Open5GS)
 key = "fec86ba6eb707ed08905757b1bb44b8f";
 opc= "C42449363BBAD02B66D16BC975D77CC1";
-pdu_sessions = ({ dnn = "oai"; nssai_sst = 1; });
+pdu_sessions = ({ dnn = "internet"; nssai_sst = 1; });
 }
...
+# Dual-cell configuration for F1 handover between DU0 and DU1
+cells = (
+  {
+    ru_id       = 0;
+    band        = 78;
+    rf_freq     = 3450720000L;    # DU0 center freq (PointA 628776 + BW/2)
+    rf_offset   = 0;
+    numerology  = 1;
+    N_RB_DL     = 106;
+    ssb_start   = 516;
+  },
+  {
+    ru_id       = 1;
+    band        = 78;
+    rf_freq     = 3649440000L;    # DU1 center freq (PointA 628776 + BW/2)
+    rf_offset   = 0;
+    numerology  = 1;
+    N_RB_DL     = 106;
+    ssb_start   = 516;
+  }
+);
+
+RUs = (
+  {
+    nb_tx      = 1;
+    nb_rx      = 1;
+    max_rxgain = 114;
+    sdr_addrs  = "dummy";
+  },
+  {
+    nb_tx      = 1;
+    nb_rx      = 1;
+    max_rxgain = 114;
+    sdr_addrs  = "dummy";
+  }
+);
+
+rfsimulator = (
+  {
+    serveraddr = "127.0.0.1";
+    serverport = 4043;           # DU0 rfsimulator port
+    options    = ();
+    modelname  = "AWGN";
+    IQfile     = "/tmp/rfsimulator.iqs";
+  },
+  {
+    serveraddr = "127.0.0.1";
+    serverport = 4044;           # DU1 rfsimulator port
+    options    = ();
+    modelname  = "AWGN";
+    IQfile     = "/tmp/rfsimulator.iqs";
+  }
+);
```

---

### 2. `targets/PROJECTS/GENERIC-NR-5GC/CONF/neighbour-config-rfsim.conf`
Standardized neighbor mappings between cell IDs `12345678L` (DU0) and `22222222L` (DU1) under gNB ID `0xe00`.

```diff
 neighbour_list = (
   ##########################################################
-  #  Entry USED BY gNB_ID = 0xe00  (nr_cellid = 12345678L) #
+  #  Entry USED BY gNB_ID = 0xe00  (nr_cellid = 12345678L) #
   ##########################################################
   {
-    nr_cellid = 12345678L;                      #  Serving cell of gNB 0xe00
+    nr_cellid = 12345678L;                      #  Serving cell of DU0 (gNB 0xe00)
     neighbour_cell_configuration = (
       {
-        gNB_ID              = 0xb00;
-        nr_cellid           = 720898;           #  Cell served by gNB 0xb00
+        gNB_ID              = 0xe00;            #  Target DU1 gNB ID
+        nr_cellid           = 22222222L;        #  Target DU1 Cell ID
         physical_cellId     = 1;
-        absoluteFrequencySSB= 621312;
+        absoluteFrequencySSB= 643296;           # Target DU1 SSB
         subcarrierSpacing   = 1;                # 30 kHz
         band                = 78;
         plmn                = { mcc = 001; mnc = 01; mnc_length = 2 };
@@ -26,16 +26,16 @@ neighbour_list = (
   },
 
   ##########################################################
-  #  Entry USED BY gNB_ID = 0xb00  (nr_cellid = 720898)    #
+  #  Entry USED BY gNB_ID = 0xe00  (nr_cellid = 22222222L) #
   ##########################################################
   {
-    nr_cellid = 720898;                           #  Serving cell of gNB 0xb00
+    nr_cellid = 22222222L;                           #  Serving cell of DU1 (gNB 0xe00)
     neighbour_cell_configuration = (
       {
-        gNB_ID              = 0xe00;
-        nr_cellid           = 12345678L;          #  Cell served by gNB 0xe00
+        gNB_ID              = 0xe00;            #  Target DU0 gNB ID
+        nr_cellid           = 12345678L;          #  Target DU0 Cell ID
         physical_cellId     = 0;
-        absoluteFrequencySSB= 641280;
+        absoluteFrequencySSB= 630048;           # Target DU0 SSB
         subcarrierSpacing   = 1;                  # 30 kHz
         band                = 78;
         plmn                = { mcc = 001; mnc = 01; mnc_length = 2 };
```

---

### 3. `targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb-cu.sa.f1.conf`
Unified CU configuration, enabled telnet server on port 9091, adjusted IP addresses, and linked the neighbor list.

```diff
     // Tracking area code, 0x0000 and 0xfffe are reserved values
-    tracking_area_code  =  1;
+    tracking_area_code  =  7;
     plmn_list = ({ mcc = 001; mnc = 01; mnc_length = 2; snssaiList = ({ sst = 1; }) });
 
     nr_cellid = 12345678L;
 
     tr_s_preference = "f1";
 
-    local_s_address     = "127.0.0.3";
+    local_s_address     = "127.0.0.11";
     remote_s_address    = "0.0.0.0"; # multiple DUs
-    local_s_portd       = 2152;
-    remote_s_portd      = 2152;
+    local_s_portd       = 2153;
+    remote_s_portd      = 2153;
...
     ////////// AMF parameters:
-    amf_ip_address = ({ ipv4 = "192.168.70.132"; });
+    amf_ip_address = ({ ipv4 = "127.0.0.5"; });
 
     NETWORK_INTERFACES :
     {
-        GNB_IPV4_ADDRESS_FOR_NG_AMF              = "192.168.70.129/24";
-        GNB_IPV4_ADDRESS_FOR_NGU                 = "192.168.70.129/24";
+        GNB_IPV4_ADDRESS_FOR_NG_AMF              = "127.0.0.11/8";
+        GNB_IPV4_ADDRESS_FOR_NGU                 = "127.0.0.11/8";
         GNB_PORT_FOR_S1U                         = 2152; # Spec 2152
     };
...
-telnet_server_config = {
-  listenport = 9091;  # Use port 9091 instead of 9090
+telnetsrv = {         # Corrected configuration parameter name
+  listenport = 9091;  # Use port 9091 instead of 9090
   listenaddr = "127.0.0.1";
 };
+
+@include "neighbour-config-rfsim.conf"
```

---

### 4. `targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb-du.sa.band78.106prb.rfsim.pci0.conf`
Configured DU0 (PCI 0) F1-U interfaces and F1 SCTP associations towards the CU (`127.0.0.11:2153`).

```diff
     gNB_ID = 0xe00;
-    gNB_DU_ID = 0xe00;
+    gNB_DU_ID = 0xe03;
     gNB_name  =  "du-rfsim";
 
     // Tracking area code, 0x0000 and 0xfffe are reserved values
-    tracking_area_code  =  1;
+    tracking_area_code  =  7;
...
 MACRLCs = ({
     tr_s_preference     = "local_L1";
     tr_n_preference     = "f1";
-    local_n_address     = "127.0.0.4";
-    remote_n_address    = "127.0.0.3";
-    local_n_portd       = 2152;
-    remote_n_portd      = 2152;
+    local_n_address     = "127.0.0.29";
+    remote_n_address    = "127.0.0.11";
+    local_n_portd       = 2153;
+    remote_n_portd      = 2153;
     pusch_FailureThres  = 1000;
 });
```

---

### 5. `targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb-du.sa.band78.106prb.rfsim.pci1.conf`
Corrected target cell ID (`22222222L`), configured target rfsimulator port `4044`, and established F1 interface parameters towards the CU.

```diff
     plmn_list = ({ mcc = 001; mnc = 01; mnc_length = 2; snssaiList = ({ sst = 1; }) });
 
-    nr_cellid = 11111111L;
+    nr_cellid = 22222222L;
...
 MACRLCs = ({
     tr_s_preference     = "local_L1";
     tr_n_preference     = "f1";
-    local_n_address     = "127.0.0.5";
-    remote_n_address    = "127.0.0.3";
-    local_n_portd       = 2152;
-    remote_n_portd      = 2152;
+    local_n_address     = "127.0.0.30";
+    remote_n_address    = "127.0.0.11";
+    local_n_portd       = 2153;
+    remote_n_portd      = 2153;
     pusch_FailureThres  = 1000;
 });
...
 rfsimulator =({
   serveraddr = "server";
-  serverport = 4043;
+  serverport = 4044;  # Target DU1 listens on port 4044
   options = ();
   modelname = "AWGN";
   IQfile = "/tmp/rfsimulator.iqs"
 })
```

---

## 3. Manual Running Sequence

Follow these exact steps to manually start the F1 Handover simulation:

1. **Verify Open5GS is running** on your core network.
2. **Launch the CU (Control Unit)** (starts the telnet server on port 9091 and automatically includes the neighbour configuration):
   ```bash
   sudo ./ran_build/build/nr-softmodem -O targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb-cu.sa.f1.conf --telnetsrv
   ```
3. **Launch DU0 (Distributed Unit - Serving Cell)** (listens on rfsimulator port 4043):
   ```bash
   sudo ./ran_build/build/nr-softmodem -O targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb-du.sa.band78.106prb.rfsim.pci0.conf --rfsim
   ```
4. **Launch DU1 (Distributed Unit - Target Cell)** (listens on rfsimulator port 4044):
   ```bash
   sudo ./ran_build/build/nr-softmodem -O targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb-du.sa.band78.106prb.rfsim.pci1.conf --rfsim
   ```
5. **Launch the UE (User Equipment)** (automatically establishes connections to both DU0 and DU1):
   ```bash
   sudo ./ran_build/build/nr-uesoftmodem -O targets/PROJECTS/GENERIC-NR-5GC/CONF/ue.conf --sa --rfsim --band 78 --numerology 1 -r 106 --ssb 516
   ```
6. **Trigger the Handover** via telnet:
   ```bash
   telnet 127.0.0.1 9091
   ```
   Once connected, execute:
   ```telnet
   ci trigger_f1_ho
   ```
   *You should see the UE successfully transition from DU0 (port 4043) to DU1 (port 4044) without any radio synchronization failures.*
