make
echo "-------------------------------"

## kaspa
# ./cfpgaminer_x86 -x socks5://127.0.0.1:7890  -w kaspa:qr36zdxs0dn3n0h799jhdl02qks5742lxjgmfsfj9xmlca7n4l6mw0s0n48nx -h 47.243.93.104:8877 -s x -b 115200 -d

## radiant
# ./cfpgaminer_x86 -x socks5://127.0.0.1:7890 -w 13HtkCUyeBmWDqFh1Vqanto6t724n8YBnu -h pool.vipor.net:5066 -s x -b 115200 -d
./cfpgaminer_x86 -x socks5://192.168.2.40:7890 -w 13HtkCUyeBmWDqFh1Vqanto6t724n8YBnu -h 3.37.189.198:5066 -s x -b 115200 -d

