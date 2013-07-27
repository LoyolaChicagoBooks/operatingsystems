#!/bin/bash

pushd build
mkdir -p dist
zip -r dist/html.zip html/
popd
