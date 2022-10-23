#!/usr/bin/env bash

MOD_INDIVIDUAL_DROP_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

source $MOD_INDIVIDUAL_DROP_ROOT"/conf/conf.sh.dist"

if [ -f $MOD_INDIVIDUAL_DROP_ROOT"/conf/conf.sh" ]; then
    source $MOD_INDIVIDUAL_DROP_ROOT"/conf/conf.sh"
fi