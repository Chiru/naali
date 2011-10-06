#!/bin/bash

echo "Setting up proxy"
cat > $HOME/.realXtend/configuration/Asset.xml <<EOF
<config>
       <Asset>
         <http_proxy_host>chiru.cie.fi</http_proxy_host>
         <http_proxy_port>3128</http_proxy_port>
       </Asset>
</config>
EOF

echo "Removing assetcache"
rm -rf $HOME/.realXtend/assetcache/*

echo "Running viewer"
./viewer "$@"

