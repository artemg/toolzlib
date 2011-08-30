#!/bin/sh
mkdir -p config m4
touch NEWS AUTHORS ChangeLog 
autoreconf --force --install -I config -I m4
