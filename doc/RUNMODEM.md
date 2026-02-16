<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# Running OAI 5G Softmodems

This document explains some options for running 5G executables.

After you have [built the softmodem executables](BUILD.md) you can set your
default directory  to the build directory `cmake_targets/ran_build/build/` and
start testing some use cases. Below, the description of the different OAI
functionalities should help you choose the OAI configuration that suits your
need.

> **Note:** UE documentation previously found in this document has been moved
> to [a separate page](./runtmodem-nrue.md)

> **Note:** NTN-specific configuration steps previously found in this document
> have been moved to [a separate page](./ntn-configuration.md)

[[_TOC_]]

## Simulators

### RFsimulator

The RFsimulator is an OAI device replacing the radio heads (for example the
USRP device). It allows connecting the oai UE (LTE or 5G) and respectively the
oai eNodeB or gNodeB through a network interface carrying the time-domain
samples, getting rid of over the air unpredictable perturbations. This is the
ideal tool to check signal processing algorithms and protocols implementation.
The RFsimulator has some preliminary support for channel modeling.

It is planned to enhance this simulator with the following functionalities:

- Support for multiple eNodeB's or gNodeB's for hand-over tests

This is an easy use-case to setup and test, as no specific hardware is required. The [rfsimulator page](../radio/rfsimulator/README.md) contains the detailed documentation.

### L2 nFAPI Simulator

This simulator connects an eNodeB and UEs through an nFAPI interface,
short-cutting the L1 layer. The objective of this simulator is to allow multi
UEs simulation, with a large number of UEs (ideally up to 255).

As for the RFsimulator, no specific hardware is required. The [L2 nfapi
simulator page](./L2NFAPI.md) contains the detailed documentation.

## Running with a true radio head

OAI supports different radio heads, the following are tested in the CI:

1. [Monolithic eNodeB](https://gitlab.eurecom.fr/oai/openairinterface5g/wikis/HowToConnectCOTSUEwithOAIeNBNew) where the whole signal processing is performed in a single process
2. IF4P5 mode, where frequency domain samples are carried over ethernet, from the RRU which implement part of L1(FFT,IFFT,part of PRACH),  to a RAU
3. Monolithic gNodeB: see next section, or the [standalone tutorial](NR_SA_Tutorial_COTS_UE.md)


## 5G NR

### NSA setup

Please refer to the [corresponding page](./TESTING_OAI_NSA_COTS_UE.md) for more
information on how to run in NSA mode:

### SA setup with OAI NR-UE

The standalone mode is the default mode. 

Before tag `2024.w45`, the default mode was NSA. Thus, in the past, to run
either the gNB or the UE in standalone mode, it was necessary to provide the
`--sa` flag in the command line. This is not the case anymore. If provided
the softmodem exits reporting that it does not know this option.

The default (SA) mode does the following:
- At the gNB:
* The RRC encodes SIB1 according to the configuration file and transmits it through NR-BCCH-DL-SCH.

- At the UE:
* Decode SIB1 and starts the 5G NR Initial Access Procedure for SA:
  1) 5G-NR RRC Connection Setup
  2) NAS Authentication and Security
  3) 5G-NR AS Security Procedure
  4) 5G-NR RRC Reconfiguration
  5) Start Downlink and Uplink Data Transfer

### Other/Special modes to run with OAI NR-UE

The gNB/nrUE support a number of special modes (phy-test, do-ra, noS1) that
cannot be used with COTS UE. As they specifically require the use of the OAI
nrUE, these modes are described [on the UE page](./runmodem-nrue.md).

### Common gNB and NR UE command line options

#### Three-quarter sampling

The command line option `-E` can be used to enable three-quarter sampling for split 8 sample rate. Required for certain radios (e.g., 40MHz with B210). If used on the gNB, it is a good idea to use for the UE as well (and vice versa).

#### Run OAI with SDAP & Custom DRBs

SDAP is enabled by default. To disable SDAP, include `--gNBs.[0].enable_sdap 0` to the binary's arguments.

The DRB creation is dependent on the 5QI. 
If the 5QI corresponds to a GBR Flow it assigns a dedicated data radio bearer.
The Non-GBR flows use a shared data radio bearer.

To hardcode the DRBs for testing purposes, simply add `--gNBs.[0].drbs x` to the binary's arguements, where `x` is the number of DRBs, along with SDAP.
The hardcoded DRBs will be treated like GBR Flows. Due to code limitations at this point the max. number of DRBs is 4. 

### IF setup with OAI

OAI is also compatible with Intermediate Frequency (IF) equipment, allowing the
use of RF front-ends operating on arbitrary frequency bands that do not conform
to the standardized 3GPP NR bands.

### OAIUE configuration
To configure IF frequencies on the UE side, provide the following command-line
options:
- `if_freq`: DL frequency in Hz
- `if_freq_off`: UL frequency offset in Hz

### gNB configuration
On the gNB side, the corresponding parameters must be set in the RUs section of
the configuration file:
- `if_freq`: DL frequency in Hz
- `if_offset`: UL frequency offset in Hz

> Note: When using a libconfig-based configuration file for the gNB, ensure that
> `if_freq` numeric value is suffixed with "L" so it is correctly parsed as
> 64-bit integer.

#### Run OAI with custom DL/UL arbitrary frequencies

The following example uses DL frequency 2169.080 MHz and UL frequency offset
-400 MHz, with a configuration file for band 66 (FDD) at gNB side.

On two separate machines with USRPs, run:

```
sudo ./nr-softmodem -O ../../../targets/PROJECTS/GENERIC-LTE-EPC/CONF/gnb.band66.tm1.106PRB.usrpx300.conf
sudo ./nr-uesoftmodem --if_freq 2169080000 --if_freq_off -400000000
```

## 5G gNB MIMO configuration

In order to enable DL-MIMO in OAI 5G softmodem, the prerequisite is to have `do_CSIRS = 1` in the configuration file. This allows the gNB to schedule CSI reference signal and to acquire from the UE CSI measurements to be able to schedule DLSCH with MIMO.

The following step is to set the number of PDSCH logical antenna ports. These needs to be larger or equal to the maximum number of MIMO layers requested (for 2-layer MIMO it is necessary to have at least two logical antenna ports).

![mimo_antenna_ports](./images/mimo_antenna_ports.png)

This image shows an example of gNB 5G MIMO logical antenna port configuration. It has to be noted that logical antenna ports might not directly correspond to physical antenna ports and each logical antenna port might consist of a sub-array of antennas.

In 5G the basic element is a dual-polarized antenna, therefore the minimal DL MIMO setup with two logical antenna ports would consist of two cross-polarized antenna elements. In a single panel configuration, as the one in the picture, this element can be repeated vertically and/or horizontally to form an equi-spaced 1D or 2D array. The values N1 and N2 represent the number of antenna ports in the two dimensions and the supported configurations are specified in Section 5.2.2.2.1 of TS 38.214.

The DL logical antenna port configuration can be selected through configuration file. `pdsch_AntennaPorts_N1` can be used to set N1 parameter, `pdsch_AntennaPorts_N2` to set N2 and `pdsch_AntennaPorts_XP` to set the cross-polarization configuration (1 for single pol, 2 for cross-pol). To be noted that if XP is 1 but N1 and/or N2 are larger than 1, this would result in a non-standard configuration and the PMI selected would be the identity matrix regardless of CSI report. The default value for each of these parameters is 1. The total number of PDSCH logical antenna ports is the multiplication of those 3 parameters.

Finally the number of TX physical antenna in the RU part of the configuration file, `nb_tx`, should be equal or larger than the total number of PDSCH logical antenna ports.

It is possible to limit the number supported DL MIMO layers via RRC configuration, e.g. to a value lower than the number of logical antenna ports configured, by using the configuration file parameter `maxMIMO_layers`.

[Example of configuration file with parameters for 2-layer MIMO](https://gitlab.eurecom.fr/oai/openairinterface5g/-/blob/develop/targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb.sa.band77.fr1.273PRB.2x2.usrpn300.conf)
