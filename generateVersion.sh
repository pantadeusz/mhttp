#!/bin/bash

X=$(git log | grep ^commit | wc -l)
echo -n $X