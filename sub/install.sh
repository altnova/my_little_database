#!/bin/bash

unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=lin;;
    Darwin*)    machine=mac;;
    CYGWIN*)    machine=cyg;;
    MINGW*)     machine=mgw;;
    *)          machine="UNKNOWN:${unameOut}"
esac

if [ $machine == 'lin' ]; then
	st3_path=~/.config/sublime-text-3/Packages/User/
elif [ $machine == 'mac' ]; then
	st3_path=~/Library/Application\ Support/Sublime\ Text\ 3/Packages/User/
else
	echo "$machine unsupported"; exit
fi

echo "sublime => $st3_path"
#diff C-kelas.sublime-syntax "$st3_path/C-kelas.sublime-syntax"
#diff C-kelas.sublime-settings "$st3_path/C-kelas.sublime-settings"
#diff monokai-kelas.tmTheme "$st3_path/monokai-kelas.tmTheme"

cp C-kelas.sublime-syntax "$st3_path"
cp C-kelas.sublime-settings "$st3_path"
echo "c-kelas => installed"

cp monokai-kelas.tmTheme "$st3_path"
echo "monokai => installed"
