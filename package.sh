TGT_NAME=lab3release
TGT_DIR=$TGT_NAME
rm -rf $TGT_DIR
mkdir $TGT_DIR
git archive HEAD | tar -x -C $TGT_DIR
zip -r5 ${TGT_NAME}.zip $TGT_DIR

