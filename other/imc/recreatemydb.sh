if test ! "$CITY_NAME"
then
	echo "What's your CITY_NAME?"
	exit 1
fi

if test `hostname` = buggy && test ! $MYSQLDESTDB
then
	echo "ERROR: Running on buggy but didn't specify alternative MYSQLDESTDB."
	echo "Doing this you might overwrite IMC Iraq (in mysql db 'imc').  Aborting."
	exit 1
fi

MYSQLDB="imc"
test $MYSQLDESTDB || MYSQLDESTDB=$MYSQLDB
test $MYSQLDB = $MYSQLDESTDB || echo "Since you are saving to a different mysql db, you'll need to hack active's settings."
BACKUPDIR="/www/db-backups/$CITY_NAME"

echo "######## MYSQL:"
echo "Destroying and creating fresh $MYSQLDESTDB database"
mysql << !
drop database $MYSQLDESTDB;
!
mysql << !
create database $MYSQLDESTDB;
!
echo "Repopulating $MYSQLDESTDB database"
mysql $MYSQLDESTDB < "$BACKUPDIR/$MYSQLDB.mysql"
mysql $MYSQLDESTDB << !
grant SELECT,INSERT,UPDATE,DELETE,CREATE,DROP on * to imc_user@localhost IDENTIFIED BY 'lkjc925r42';
INSERT INTO IMC_SITE_INFO  (NAME, NUMBER_TO_LIST) VALUES( 'bristol', 5 );
!
echo
