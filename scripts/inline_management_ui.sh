#!/bin/sh
rm data/*.html
rm data/*.css
rm data/*.js
mkdir data
cp management_ui/*.html data
cp management_ui/*.js data
cp management_ui/*.css data
