#!/bin/bash
# Copyright (c) 2025 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.

has_base=
all_modules=$*
main_module=
release_name=ciyam.0.1

if [ "$1" = "" ]; then
 echo "Usage: release.sh [module names]"

 exit
fi

while true
do
 if [ "$1" = "" ]; then
  break;
 fi

 if [ "$1" = "ciyam_base.so" ]; then
  has_base=1
 else
  if [ "$main_module" = "" ]; then
   if [ ! "$1" = "Meta.so" ]; then
    main_module=${1%.*}
   fi
  fi
 fi

 shift
done

if [ "$main_module" = "" ]; then
 main_module=Meta
fi

if [ "$WEBDIR" = "" ]; then
 echo "Error: Missing WEBDIR environment variable."

 exit
fi

# NOTE: Check if the application server is running.
touch ciyam_server.cmd
sleep 1

if [ -f ciyam_server.cmd ]; then
 echo "Error: Application server needs to be running to create release archive."

 exit
fi

if [ "$has_base" = "" ]; then
 echo "Error: Missing 'ciyam_base.so' target (need to make all?)."
else
 CIYAM_USER=$USER
 CIYAM_GROUP=$(id -gn)

 if [ ! -d $release_name ]; then
  mkdir $release_name
  if [ ! -d $release_name ]; then
   echo "Error: Unable to create '$release_name' directory."

   exit
  fi
  mkdir $release_name/ciyam
  if [ ! -d $release_name/ciyam ]; then
   echo "Error: Unable to create '$release_name/ciyam' directory."

   exit
  fi
 fi

 cp [l-t]*.sh $release_name/ciyam

 cp [a-z]*.cin $release_name/ciyam

 cp ciyam.fissile timezones.sio $release_name/ciyam

 cp autoscript.sio.default ciyam_server.sio.default manuscript.sio.default $release_name/ciyam

 cp Meta.cin Meta.dat Meta.idx Meta.hdr Meta.tlg Meta.sql Meta.txt Meta.modules.lst $release_name/ciyam

 cp modules.lst packages.lst ciyam_demo_identities.lst $release_name/ciyam

 cp channel_readme.md mnemonics.txt ciyam_strings.txt module_strings.txt $release_name/ciyam

 cp add_user backup_check backup_export backup_import ciyam_backup ciyam_command ciyam_restore construct $release_name/ciyam
 cp create_ciyam_pem create_db drop_db export_files generate_next_block hub_check import_files init_identity $release_name/ciyam
 cp prepare_for_import restore shared_export shared_check shared_import system_variable update unlock_identity $release_name/ciyam

 cp ext_request run_script run_temp script set_password $release_name/ciyam

 cp bundle unbundle ciyam_client ciyam_server ods_fsed $all_modules $release_name/ciyam

 if [ "$main_module" = "Meta" ]; then
  ./ciyam_command storage_backup -init $main_module

  if [ -f $main_module.backup.sql ]; then
   mv $main_module.backup.sql $release_name/ciyam
  fi

  echo "export CIYAM_APP_DIR=${main_module,,}" > $release_name/env_vars.sh
  echo "export CIYAM_APP_MOD=$main_module" >> $release_name/env_vars.sh

  chmod a+x $release_name/env_vars.sh

  sudo systemctl restart apache2

  cp -R $WEBDIR/${main_module,,} $release_name/${main_module,,}

  rm -f $release_name/${main_module,,}/*.log
  rm -f $release_name/${main_module,,}/*.sav
  rm -f $release_name/${main_module,,}/ciyam.pem
  rm -f $release_name/${main_module,,}/identity.txt
  rm -f $release_name/${main_module,,}/encrypted.txt
 else
  cp $main_module.sql $release_name/ciyam
  cp $main_module.txt $release_name/ciyam
  cp $main_module.map.new $release_name/ciyam
  cp $main_module.modules.lst $release_name/ciyam

  cp $main_module*.cin $main_module*.csv $main_module.*.vars.lst $main_module.init.lst $release_name/ciyam

  sed -e '1,/^### .<start generated/ d' manuscript.sio > $release_name/ciyam/manuscript.sio.generated

  if grep -q "^ <ods_use_encrypted>false$" ciyam_server.sio; then
   ./ods_fsed -quiet "-exec=export $main_module" $main_module
  else
   ./export_encrypted_ods_db $main_module
  fi

  mv $main_module $release_name/ciyam

  rm -f $release_name/ciyam/$main_module.generate.*.cin

  ./ciyam_command storage_backup -init Meta
  ./ciyam_command storage_backup -init $main_module

  if [ -f Meta.backup.sql ]; then
   mv Meta.backup.sql $release_name/ciyam
  fi

  if [ -f $main_module.backup.sql ]; then
   mv $main_module.backup.sql $release_name/ciyam
  fi

  cat $main_module.log | tail -n +2 > $release_name/ciyam/$main_module.log.app

  echo "export CIYAM_APP_DIR=${main_module,,}" > $release_name/env_vars.sh
  echo "export CIYAM_APP_MOD=$main_module" >> $release_name/env_vars.sh

  chmod a+x $release_name/env_vars.sh

  sudo systemctl restart apache2

  cp -R $WEBDIR/${main_module,,} $release_name/${main_module,,}
 fi

 if [ ! -f ciyam.service ]; then
  ./init_service
 fi

 cp ciyam.service $release_name

 had_install=

 # NOTE: Assuming was not already present will copy
 # the 'install' bash script (then remove it after)
 # in order to stop 'tar' complaining about needing
 # to remove a '../' prefix.
 if [ -f install ]; then
  had_install=1
 else
  cp ../install .
 fi

 rm -f ../$release_name.tar.gz
 tar -czvf ../$release_name.tar.gz install $release_name

 if [ "$had_install" = "" ]; then
  rm -f install
 fi

 rm -R $release_name
fi
