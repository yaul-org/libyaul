#!/bin/sh

# requires: GNU md5sum and bash, xxd from vim
# known bug: file ordering is not actually specified

NTESTS=13
FAIL=0
I=0
V=
tdir=`pwd`
CMD=$tdir/../genromfs
mkdir testimg
cd testimg

function testlabel () {
  I=`expr $I + 1`
  echo "$I/$NTESTS" "$@"
  rm -f *
  rm -f $tdir/img.bin
}

function testsummary () {
  xxd -a <$tdir/img.bin >$tdir/$I.out
  if ! diff -u $tdir/$I.ok $tdir/$I.out; then
    echo FAILED - Check differences above
    FAIL=`expr $FAIL + 1`
  else
    echo OK
    rm -f $tdir/$I.out
  fi
}

testlabel Test image with no files
eval $CMD $V -V EMPTY -f $tdir/img.bin
testsummary

testlabel Test image with a single file
echo dummy >dummy
eval $CMD $V -V SINGLEFILE -f $tdir/img.bin
testsummary

testlabel Test image with a hard links
echo dummy >dummy
ln dummy hardlink
eval $CMD $V -V HARDLINK -f $tdir/img.bin
testsummary

testlabel Test image with alignment
echo t01abc >t01xyz
echo '(512aligned0123456789abcdef)' >t02uvw
echo t03ghi >t03rst
eval $CMD $V -V ALIGNED -A '512,t02*' -f $tdir/img.bin
testsummary

testlabel Test image with alignment with absolute path
echo t01abc >t01xyz
echo t02def >t02uvw
echo '(128aligned0123456789abcdef)' >t03_123aligned
eval $CMD $V -V ABSALIGNED -A '128,/t03*' -f $tdir/img.bin
testsummary

testlabel Test image with file data
echo 01230123uuu1012 >rom1a.txt
echo 01230123xyza012 >rom1b.txt
ln -s 01230123xyz1    rom1c.dat
ln -s rom1b.txt       rom1d.txt
eval $CMD $V -V EXTDATA -f $tdir/img.bin
testsummary

testlabel Test default alignment
echo 0123 >abc.txt
echo 4567 >def.txt
eval $CMD $V -V DEFALIGNED -a128 -f $tdir/img.bin
testsummary

testlabel Test multiple alignment
echo boot512 >m68k.boot
echo boot2048 >sparc.boot
eval $CMD $V -V MULTIALIGNED -A2048,*.boot -A1024,m68k.boot -A512,m68k.boot -f $tdir/img.bin
testsummary

testlabel Test excludes
echo unwanted >alpha
echo needed >beta
echo unwanted >gamma
eval $CMD $V -V EXCLUDES -x '"*a*a"' -f $tdir/img.bin
testsummary

testlabel Test exclude/includes
echo wanted >good.txt
echo not >stuff.txt
echo del >erase.txt
eval $CMD $V -V INCLUDES -x '"*"' '"-i*good*"' -f $tdir/img.bin
testsummary

testlabel Test new style alignment option
echo aligned >stuff.txt
echo del >*.sh
eval $CMD $V -V ALIGNNEW -ealign:256,*.txt -f $tdir/img.bin
testsummary

testlabel Test permissions
echo xxxx >u63.txt
echo xxxx >g62.txt
echo yyyy >u63g62.txt
echo qqqq >u127g128.txt
echo zzzz >su11.txt
echo zzzz >sug62u63.txt
eval $CMD $V -V PERMISSIONS -euid:63,*u63* -egid:62,*g62* -euid:127,*127* -egid:128,*128* -eperm:4511,su* -f $tdir/img.bin
testsummary

testlabel Test devices/permissions
mkdir dev
touch dev/@null,c,1,3
touch dev/@zero,c,1,5
echo 1234567890 >feb14.txt
eval $CMD $V -V DEVICES -eperm:711,/dev -eperm:666,/dev/* -etime:1234567890,feb14* -euid:100,feb* -egid:200,feb* -f $tdir/img.bin
testsummary
rm -rf dev

# remove stray files
rm -f *

# end
echo $FAIL failures found
cd $tdir
rm -f img.bin
rmdir testimg

if [ "x$FAIL" != "x0" ]; then
  exit 1
fi

exit 0
