#!/bin/bash

hg incoming && hg pull && hg update --clean && ./deploy-courseclouds.sh
