mkdir -p ./coverage
rm -rf ./coverage/*
lcov --directory . --capture --output-file coverage/coverage.info
lcov --remove coverage/coverage.info '/usr/*' '/test/*' --output-file coverage/coverage.info 
#lcov --remove coverage/coverage.info '/test/*' --output-file coverage/coverage.info 
genhtml coverage/coverage.info -o coverage


