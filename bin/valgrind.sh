#!/bin/bash
if [ ! -e "$1" ]
then
    echo "File does not exists: " $1
else
    filename=$(basename $1)
    ./server --file scenes/Avatar/avatar.txml --protocol tcp --startserver 2345 &
    ./server --file scenes/DayNight/TestWorld.txml protocol tcp --startserver 2346 &
    cp valgrind/$filename jsmodules/startup/
    valgrind --tool=memcheck ./viewer --storage scenes/
    cd valgrind/
    cat valgrindMemcheck.log | grep 'are definitely lost' -A12 > definitelyLost.log && cat valgrindMemcheck.log | grep 'LEAK SUMMARY' -A15 -B1 >> definitelyLost.log
    cat valgrindMemcheck.log | grep 'are definitely lost' -A12 > possiblyLost.log && cat valgrindMemcheck.log | grep 'LEAK SUMMARY' -A15 -B1 >> possiblyLost.log
    cd ..
    valgrind --tool=massif ./viewer --storage scenes/
    ms_print massif.out > valgrind/massif.log
    cat valgrind/massif.log | head -30
    cat valgrind/valgrindMemcheck.log | grep 'LEAK SUMMARY' -A15 -B1
    rm jsmodules/startup/$filename
    ps x | grep '\.\/server --file scenes\/.*--startserver' | cut -d" "  -f1 | xargs kill -9 
    echo ''
    echo 'Full valgrind logs are stored in '$PWD'/valgrind/'
    echo 'End of valgrind testdrive.'
    echo ''
fi
