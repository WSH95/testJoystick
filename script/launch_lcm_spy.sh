#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd ${DIR}/../lcmFiles/java
export CLASSPATH=${DIR}/../lcmFiles/java/my_types.jar
pwd
lcm-spy
