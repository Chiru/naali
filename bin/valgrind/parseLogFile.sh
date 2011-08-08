#!/bin/bash
cat valgrindMemcheck.log | grep 'are definitely lost' -A12 > definitelyLost.log && cat valgrindMemcheck.log | grep 'LEAK SUMMARY' -A15 -B1 >> definitelyLost.log
cat valgrindMemcheck.log | grep 'are definitely lost' -A12 > possiblyLost.log && cat valgrindMemcheck.log | grep 'LEAK SUMMARY' -A15 -B1 >> possiblyLost.log
