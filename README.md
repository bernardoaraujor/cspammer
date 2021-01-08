# IOTA CSpammer

This is an IOTA Client Spammer writen in C99.
It acts as an IOTA Client sending spam transactions to an IOTA Node.

It was written for the IOTA 1.0 (Pre-Chrysalis pt.2) protocol.

Heavily inspired by **Sam Chen**'s [IOTA Commander](https://github.com/oopsmonk/iota_cmder).

## Build

```
$ git clone https://github.com/bernardoaraujor/iota_cspammer.git
$ cd iota_cspammer
$ mkdir build && cd build
$ cmake -DCMAKE_INSTALL_PREFIX=$PWD ..
$ make
``` 

## Spam

```
$ cspammer --help
Usage: cspammer [--help] -a <addr> -t <tps> -s <seed> -u <url> -p <port> [--https] -m <mwm> -d <depth> -y <security>
IOTA (Pre-Chrysalis pt.2) Client Spammer.

  --help     print this help and exit
  -a, --addr=<addr> Target IOTA Address Trytes
  -t, --tps=<tps> Spamrate (Tx Per Second)
  -s, --seed=<seed> Client Seed Trytes
  -u, --url=<url> Node's URL
  -p, --port=<port> Node's Port
  --https    Node has HTTPS
  -m, --mwm=<mwm> Node Minimum Weight Magnitude (MWM)
  -d, --depth=<depth> Node's Merkle Tree Depth
  -y, --sec=<security> Node's Security Level
```

for example:
```
$ cspammer --addr=PDVKAGKHVYKBLBRJZPP9JATUY9QYYXOMIQFPESXLSG9EHBCXSZKGSYWPGIOWGKHGNYTZYMOVPDMJDCUA9 --seed=HJTOWJRIZXGONIQZZGRNCKMJFSEJKRTZXDOQNAFDIOLIYBOHUDCARADLETVASHMYMWAWWRQHWEZUYONZ9 --url=nodes.iota.cafe --port=443 --https --mwm=14 --depth=13 --sec=2
```