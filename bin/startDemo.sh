#!/bin/bash
cp wetabstartdemo.js jsmodules/startup/
./server --file scenes/Avatar/avatar.txml --startserver 2345 &
./server --file scenes/QMLDemo/TestScene.txml --startserver 2346 &
./viewer --storage scenes/Avatar/ --storage scenes/QMLDemo/
rm jsmodules/startup/wetabstartdemo.js
ps x | grep 'server --file scenes/' | awk '{print $1 }' | xargs kill
