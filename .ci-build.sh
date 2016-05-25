change_env_value() {
    local _variable="${1}"
    local _value="${2}"

    awk -F '=' '/^export '"${_variable}"'=.+/ { print $1 "='"${_value}"'"; getline; } { print; }' yaul.env > yaul.env.tmp
    mv yaul.env.tmp yaul.env
}

for var in "TRAVIS_BRANCH" "BUILD_TYPE" "INSTALL_ROOT" "BUILD_ROOT" "BUILD" "OPTION_DEV_CARTRIDGE"; do
    if ! set 2>&1 | grep -q -E "^${var}=.+$"; then
        printf -- "Environment variable \`${var}' was not set\n"
        exit 1
    fi
done

pwd

set -x

git clone --depth=100 --branch=${TRAVIS_BRANCH} https://github.com/ijacquez/libyaul.git ijacquez/libyaul
cd ijacquez/libyaul
git submodule init
git submodule update -f
cp yaul.env.in yaul.env
change_env_value "INSTALL_ROOT" "${INSTALL_ROOT}"
change_env_value "BUILD_ROOT" "${BUILD_ROOT}"
change_env_value "BUILD" "${BUILD}"
change_env_value "OPTION_DEV_CARTRIDGE" "${OPTION_DEV_CARTRIDGE}"

. ./yaul.env

make ${BUILD_TYPE}
make install-${BUILD_TYPE}
