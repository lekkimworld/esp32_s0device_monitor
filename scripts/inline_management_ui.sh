#!/bin/sh
echo 'R"(' > management_ui_include/styles.css
cat management_ui/styles.css >> management_ui_include/styles.css
echo ')"' >> management_ui_include/styles.css

echo 'R"(' > management_ui_include/script.js
cat management_ui/script.js >> management_ui_include/script.js
echo ')"' >> management_ui_include/script.js

echo 'R"(' > management_ui_include/root.html
cat management_ui/root.html >> management_ui_include/root.html
echo ')"' >> management_ui_include/root.html

echo 'R"(' > management_ui_include/wificonfig.html
cat management_ui/wificonfig.html >> management_ui_include/wificonfig.html
echo ')"' >> management_ui_include/wificonfig.html
