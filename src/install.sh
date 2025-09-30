#!/bin/bash
# Copyright (c) 2025 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.

has_base=
all_modules=$*
main_module=

while true
do
 if [ "$1" == "" ]; then
  break;
 fi

 if [ "$1" == "ciyam_base.so" ]; then
  has_base=1
 else
  if [ "$main_module" = "" ]; then
   if [ ! "$1" == "Meta.so" ]; then
    main_module=${1%.*}
   fi
  fi
 fi

 shift
done

if [ "$WEBDIR" = "" ]; then
 echo "Error: Missing WEBDIR environment variable."
 exit
fi

if [ "$has_base" = "" ]; then
 echo "Error: Missing 'ciyam_base.so' target (need to make all?)."
else
 CIYAM_USER=$USER
 CIYAM_GROUP=$(id -gn)

 if [ "$main_module" = "" ]; then
  echo "Error: No main module found (need to generate application?)."
  exit
 fi

 if [ ! -d ciyam.0.1 ]; then
  mkdir ciyam.0.1
  if [ ! -d ciyam.0.1 ]; then
   echo "Error: Unable to create 'ciyam.0.1' directory."
   exit
  fi
  mkdir ciyam.0.1/ciyam
  if [ ! -d ciyam.0.1/ciyam ]; then
   echo "Error: Unable to create 'ciyam.0.1/ciyam' directory."
   exit
  fi
 fi

 cp [l-t]*.sh ciyam.0.1/ciyam

 cp [a-z]*.cin ciyam.0.1/ciyam

 cp *.default ciyam.fissile timezones.sio ciyam.0.1/ciyam

 cp Meta.dat Meta.idx Meta.hdr Meta.tlg Meta.sql Meta.txt Meta.modules.lst ciyam.0.1/ciyam

 cp modules.lst packages.lst ciyam_demo_identities.lst ciyam.0.1/ciyam

 cp mnemonics.txt ciyam_strings.txt module_strings.txt ciyam.0.1/ciyam

 cp add_user backup_check backup_export backup_import construct create_ciyam_pem ciyam.0.1/ciyam
 cp create_db drop_db export_files generate_next_block hub_check import_files init_identity ciyam.0.1/ciyam
 cp prepare_for_import restore shared_export shared_check shared_import system_variable update unlock_identity ciyam.0.1/ciyam

 cp ext_request run_script run_temp script set_password ciyam.0.1/ciyam

 cp bundle unbundle ciyam_client ciyam_server ods_fsed $all_modules ciyam.0.1/ciyam

 if [ ! "$main_module" = "" ]; then
  cp $main_module.sql ciyam.0.1/ciyam
  cp $main_module.txt ciyam.0.1/ciyam
  cp $main_module.map.new ciyam.0.1/ciyam
  cp $main_module.modules.lst ciyam.0.1/ciyam

  if grep -q "^ <ods_use_encrypted>false$" ciyam_server.sio; then
   ./ods_fsed -quiet "-exec=export $main_module" $main_module
  else
   ./export_encrypted_ods_db $main_module
  fi

  mv $main_module ciyam.0.1/ciyam

  ./ciyam_command storage_backup -init Meta
  ./ciyam_command storage_backup -init $main_module

  if [ -f Meta.backup.sql ]; then
   mv Meta.backup.sql ciyam.0.1/ciyam
  fi

  if [ -f $main_module.backup.sql ]; then
   mv $main_module.backup.sql ciyam.0.1/ciyam
  fi

  echo "export CIYAM_APP_DIR=${main_module,,}" > ciyam.0.1/env_vars.sh
  echo "export CIYAM_APP_MOD=$main_module" >> ciyam.0.1/env_vars.sh

  chmod a+x ciyam.0.1/env_vars.sh

  sudo systemctl restart apache2

  cp -R $WEBDIR/${main_module,,} ciyam.0.1/${main_module,,}
  rm -f ciyam.0.1/${main_module,,}/ciyam.pem
 fi

 cp autoscript.sio.default ciyam.0.1/ciyam/autoscript.sio

 cp manuscript.sio.default ciyam.0.1/ciyam/manuscript.sio

 cp ciyam_server.sio.default ciyam.0.1/ciyam/ciyam_server.sio

 if [ ! -f ciyam.service ]; then
  ./init_service
 fi

 cp ../install ciyam.0.1

 chmod a+x ciyam.0.1/install

 cp ciyam.service ciyam.0.1

 rm -f ciyam.0.1.tar.gz
 tar -czvf ciyam.0.1.tar.gz ciyam.0.1

 rm -R ciyam.0.1
fi
