#!/bin/bash
#
# Maintainer script for publishing releases.

set -e

source=$(dpkg-parsechangelog -S Source)
version=$(dpkg-parsechangelog -S Version)
distribution=$(dpkg-parsechangelog -S Distribution)
codename=$(debian-distro-info --codename --${distribution})

./release-doc.sh ${version} stable

OS=debian DIST=${codename} ARCH=amd64 pbuilder-ev3dev build
OS=debian DIST=${codename} ARCH=i386 PBUILDER_OPTIONS="--binary-arch" pbuilder-ev3dev build
OS=debian DIST=${codename} ARCH=armhf PBUILDER_OPTIONS="--binary-arch" pbuilder-ev3dev build
OS=debian DIST=${codename} ARCH=armel PBUILDER_OPTIONS="--binary-arch" pbuilder-ev3dev build
OS=raspbian DIST=${codename} ARCH=armhf pbuilder-ev3dev build

debsign ~/pbuilder-ev3dev/debian/${codename}-amd64/${source}_${version}_amd64.changes
debsign ~/pbuilder-ev3dev/debian/${codename}-i386/${source}_${version}_i386.changes
debsign ~/pbuilder-ev3dev/debian/${codename}-armhf/${source}_${version}_armhf.changes
debsign ~/pbuilder-ev3dev/debian/${codename}-armel/${source}_${version}_armel.changes
debsign ~/pbuilder-ev3dev/raspbian/${codename}-armhf/${source}_${version}_armhf.changes

dput ev3dev-debian ~/pbuilder-ev3dev/debian/${codename}-amd64/${source}_${version}_amd64.changes
dput ev3dev-debian ~/pbuilder-ev3dev/debian/${codename}-i386/${source}_${version}_i386.changes
dput ev3dev-debian ~/pbuilder-ev3dev/debian/${codename}-armhf/${source}_${version}_armhf.changes
dput ev3dev-debian ~/pbuilder-ev3dev/debian/${codename}-armel/${source}_${version}_armel.changes
dput ev3dev-raspbian ~/pbuilder-ev3dev/raspbian/${codename}-armhf/${source}_${version}_armhf.changes

gbp buildpackage --git-tag-only
