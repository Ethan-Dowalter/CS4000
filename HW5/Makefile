verify:coin_verifier.cc
	g++ -o verify coin_verifier.cc -I/usr/include/openssl -L/usr/lib/openssl -lcrypto
miner:cs_miner.cc
	g++ -o miner cs_miner.cc -I/usr/include/openssl -L/usr/lib/openssl -lcrypto
miner_openMP:cs_miner_openMP.cc
	g++ -fopenmp -o miner_openMP cs_miner_openMP.cc -I/usr/include/openssl -L/usr/lib/openssl -lcrypto
miner_openMPI:cs_miner_openMPI.cc
	mpic++ -o miner_openMPI cs_miner_openMPI.cc -I/usr/include/openssl -L/usr/lib/openssl -lcrypto
