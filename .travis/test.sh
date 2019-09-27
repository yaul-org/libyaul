#!/bin/bash -e

VARIABLES=("BUILD_TYPE" \
           "YAUL_INSTALL_ROOT" \
           "YAUL_ARCH_SH_PREFIX" \
           "YAUL_BUILD_ROOT" \
           "YAUL_BUILD" \
           "YAUL_OPTION_DEV_CARTRIDGE")

change_env_value() {
    local _variable="${1}"
    local _value="${2}"

    awk -F '=' '/^export '"${_variable}"'=.+/ { print $1 "='"${_value}"'"; getline; } { print; }' yaul.env > yaul.env.tmp
    mv yaul.env.tmp yaul.env
}

for var in ${VARIABLES[@]}; do
    if ! set 2>&1 | grep -q -E "^${var}=.+$"; then
        printf -- "Environment variable \`${var}' was not set\n"
        exit 1
    fi
done

pwd

set -x

cp yaul.env.in yaul.env
change_env_value "YAUL_INSTALL_ROOT" "${YAUL_INSTALL_ROOT}"
change_env_value "YAUL_PROG_SH_PREFIX" "${YAUL_PROG_SH_PREFIX}"
change_env_value "YAUL_ARCH_SH_PREFIX" "${YAUL_ARCH_SH_PREFIX}"
change_env_value "YAUL_BUILD_ROOT" "${YAUL_BUILD_ROOT}"
change_env_value "YAUL_BUILD" "${YAUL_BUILD}"
change_env_value "YAUL_CDB" 1
change_env_value "YAUL_OPTION_DEV_CARTRIDGE" "${YAUL_OPTION_DEV_CARTRIDGE}"

. ./yaul.env

make ${BUILD_TYPE}
sudo make install-${BUILD_TYPE}
