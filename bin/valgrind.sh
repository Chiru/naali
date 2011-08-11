#!/bin/bash
bashtrap()
{
    exitCode="$?"
    #This trap is used to clean up possible temp files, etc. if script recieves INT TERM KILL signal.
    if [ ! $exitCode -eq '0' ]; then 
        echo 'Program sent exit code: ' $exitCode
        rm -f jsmodules/startup/$filename
        echo 'Killing servers now...'
        ps x | grep '\.\/server --file scenes\/.*--startserver' | awk '{print $1}' | xargs kill
        exit $?
    fi
}
# Test if file exists which is given as 1st parameter for this script.
if [ ! -e "$1" ];
then
    echo "File '"$1"' does not exists!"
else
    trap bashtrap ERR TERM KILL EXIT
    filename=$(basename $1)
    
    # Start two local servers.
    ./server --file scenes/Avatar/avatar.txml --protocol tcp --startserver 2345 &
    ./server --file scenes/DayNight/TestWorld.txml protocol tcp --startserver 2346 &
    
    # Copy desired javascript file to jsmodules/startup/ for viewer to execute.
    cp valgrind/$filename jsmodules/startup/
    
    # Start viewer with valgrind tool memcheck.
     valgrind --tool=memcheck ./viewer --storage scenes/
    #./viewer --storage scenes/
    # When valgrind run ends, parse logs to separate files in valgrind directory.
    if [ -e "valgrind/valgrindMemcheck.log" ]; then
        cd valgrind/
        cat valgrindMemcheck.log | grep 'are definitely lost' -A12 > definitelyLost.log && cat valgrindMemcheck.log | grep 'LEAK SUMMARY' -A15 -B1 >> definitelyLost.log
        cat valgrindMemcheck.log | grep 'are definitely lost' -A12 > possiblyLost.log && cat valgrindMemcheck.log | grep 'LEAK SUMMARY' -A15 -B1 >> possiblyLost.log
        cd ..
    fi
    
    # Start viewer with valgrind tool massif.
    valgrind --tool=massif ./viewer --storage scenes/
    #./viewer --storage scenes/
    # Make massif.out human readable and save it to valgrind directory.
    if [ -e "valgrind/massif.out" ]; then
        cd valgrind/
        ms_print massif.out > massif.log
        cd ..
    fi
    
    # Print massif graph and leak summary for the user.
    if [ -e "valgrind/massif.log" ]; then
        cat valgrind/massif.log | head -30
    else
        echo 'massif.log not found.'
    fi
    if [ -e "valgrind/valgrindMemcheck.log" ]; then    
        cat valgrind/valgrindMemcheck.log | grep 'LEAK SUMMARY' -A15 -B1
    else
        echo 'valgrindMemcheck.log not found.'
    fi
    
    echo ''
    echo 'Full valgrind logs are stored in '$PWD'/valgrind/'
    echo 'End of valgrind testdrive.'
    echo ''
fi
