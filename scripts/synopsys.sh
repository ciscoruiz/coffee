../../tools/cov-analysis-linux64-2017.07/bin/cov-build -dir cov-int make -j 2
tar czvf coffee.tgz ./cov-int/
#wget https://scan.coverity.com/download/linux64 --post-data "token=nqpTm3ovDENTU2dk3Gyczw&project=ciscoruiz%2Fcoffee&md5=1" -O coffee.tgz 
