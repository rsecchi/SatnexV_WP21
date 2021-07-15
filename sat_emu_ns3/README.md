
=============  Installing Ns-3 ============

1) Download ns-3 from the website, unzip it and configure it:
```
wget https://www.nsnam.org/releases/ns-allinone-3.33.tar.bz2
tar xvjf ns-allinone-3.33.tar.bz2
cd ns-allinone-3.33/ns-3.33
./waf configure --build-profile=optimized
```

2) Copy the script into the "scratch" directory and compile:
```
cp /path/to/SatnexV_WP21/sat_emu_ns3/satdelay.cc scratch/
./waf
```

3) Enable ns-3 script to start at startup:
- Change the variables `LOCAL_ETH` and `NS3_DIR` in satconfig.sh to point to the right external interface (eg eth0) and to the directory of ns-3 respectively
- Change the variable `ExecStart` in ns3sat.service to point to the script satconfig.sh
- Copy satconfig.sh in /etc/systemd/system
- Enable the ns3sat unit to be run at startup: `systemctl enable ns3sat.service`



