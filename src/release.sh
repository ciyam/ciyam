#!/bin/bash
# Copyright (c) 2025 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.

has_base=
all_modules=$*

if [ "$1" = "" ]; then
 echo "Usage: release.sh [module names]"

 exit
fi

if [ ! -f .app_name ]; then
 echo "Error: Missing '.app_name' file."

 exit
fi

app_name=$(<.app_name)
app_dir_name=${app_name,,}

if [ "$app_name" = "" ]; then
 echo "Error: Unexpected empty '.app_name' file."

 exit
fi

if [ ! -f $app_name.txt ]; then
 echo "Error: Missing application strings file '$app_name.txt'."

 exit
fi

app_version=$(grep "^version " $app_name.txt | sed 's/[^0-9.]//g')

if [ "$app_version" = "" ]; then
 echo "Error: Application strings file '$app_name.txt' is missing expected 'version' string."

 exit
fi

release_name=ciyam_$app_dir_name.$app_version

main_module=

while true
do
 if [ "$1" = "" ]; then
  break;
 fi

 if [ "$1" = "ciyam_base.so" ]; then
  has_base=1
 else
  if [ "$1" = "$app_name.so" ]; then
   main_module=${1%.*}
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
 echo "Error: Application server needs to be running to create a release archive."

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

 cp ciyam.service.xrep ciyam_sftp.sh.xrep xrep $release_name/ciyam

 cp autoscript.sio.default ciyam_server.sio.default manuscript.sio.default $release_name/ciyam

 cp Meta.cin Meta.sql Meta.txt Meta.modules.lst $release_name/ciyam

 cp modules.lst packages.lst ciyam_demo_identities.lst $release_name/ciyam

 cp channel_readme.md mnemonics.txt ciyam_strings.txt module_strings.txt $release_name/ciyam

 cp add_user at_init at_term backup_check backup_export backup_import block_tree_hash ciyam_backup ciyam_command ciyam_restore $release_name/ciyam
 cp construct create_ciyam_pem create_db drop_db export_files generate_next_block hub_check import_files init_identity kill_current_pid_for_user $release_name/ciyam
 cp prepare_for_import process_ciyam_logins restore set_random_password shared_export shared_check shared_import system_variable $release_name/ciyam
 cp tree_list update unlock_identity $release_name/ciyam

 cp ext_request run_script run_temp script set_password $release_name/ciyam

 cp bundle unbundle ciyam_client ciyam_server ods_fsed $all_modules $release_name/ciyam

 if [ "$main_module" = "Meta" ]; then
  ./ods_fsed -quiet "-exec=export $main_module" $main_module

  mv $main_module $release_name/ciyam

  cp Meta_init_std.cin $release_name/ciyam

  ./ciyam_command storage_backup -init $main_module

  if [ -f $main_module.backup.sql ]; then
   mv $main_module.backup.sql $release_name/ciyam
  fi

  cat Meta.log | tail -n +2 > $release_name/ciyam/$main_module.log.app
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

  ./ods_fsed -quiet "-exec=export Meta" Meta

  mv Meta $release_name/ciyam

  cat Meta.log | tail -n +2 | sed '/meta std (start)/!q' > $release_name/ciyam/Meta.log.app

  ./ciyam_command storage_backup -init Meta
  ./ciyam_command storage_backup -init $main_module

  if [ -f Meta.backup.sql ]; then
   mv Meta.backup.sql $release_name/ciyam
  fi

  if [ -f $main_module.backup.sql ]; then
   mv $main_module.backup.sql $release_name/ciyam
  fi

  cat $main_module.log | tail -n +2 > $release_name/ciyam/$main_module.log.app
 fi

 echo "export CIYAM_APP_DIR=$app_dir_name" > $release_name/env_vars.sh
 echo "export CIYAM_APP_MOD=$main_module" >> $release_name/env_vars.sh

 chmod a+x $release_name/env_vars.sh

 sudo systemctl restart apache2

 cp -R $WEBDIR/$app_dir_name $release_name/$app_dir_name

 if [ "$main_module" = "Meta" ]; then
  rm -f $release_name/$app_dir_name/*.log
  rm -f $release_name/$app_dir_name/*.sav
  rm -f $release_name/$app_dir_name/ciyam.pem
  rm -f $release_name/$app_dir_name/identity.txt
  rm -f $release_name/$app_dir_name/encrypted.txt
 fi

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
