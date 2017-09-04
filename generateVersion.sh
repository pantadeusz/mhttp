#!/bin/bash

hg head | head -n 1 | awk -F'[: \t]+' '{print $2}' | tr '\n' ' '
