if test ! "$CITY_NAME"
then
	echo "What's your CITY_NAME?"
	exit 1
fi

# Note: can't adjust db dump here since not owner of file.

PSQLDB="active_$CITY_NAME"
BACKUPDIR="/www/db-backups/$CITY_NAME"

echo "######## POSTGRES:"
echo "Destroying $PSQLDB database"
# For postgres < 7:
# destroydb $PSQLDB
# For postgres >= 7:
dropdb $PSQLDB
echo "Recreating $PSQLDB database"
createdb $PSQLDB
# Only needed once per initdb:
# psql -d $PSQLDB -c 'create user php with password fr0gg3r'
psql -d $PSQLDB -f $BACKUPDIR/$PSQLDB.psql > /tmp/db.out 2>&1
echo
