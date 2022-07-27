#check for the _RLD_LIB_PATH set in the current environment or not
if [ -z "$_RLD_LIB_PATH" ]
then
    #If the path is not set throw the message
    echo "Please set the _RLD_LIB_PATH and try again..."
else 
    #Path is set trying to start the application
    run -cpu=2 -name=/G/ftiap ./HA-App 
fi
