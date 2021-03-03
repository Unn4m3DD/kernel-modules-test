cd $1
make build
cp main.ko ../build/main.ko
make clean
cd ..