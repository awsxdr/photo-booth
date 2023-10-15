#! /bin/bash

rm ./wedding-picture-upload.zip
cd ./lambda/wedding-picture-upload
zip -r ../../wedding-picture-upload.zip .
cd ../..
aws lambda update-function-code --function-name wedding-picture-upload --zip-file fileb://~/Programming/photo-booth/wedding-picture-upload.zip --no-paginate --no-cli-pager

rm ./wedding-picture-processor.zip
cd ./lambda/wedding-picture-processor
zip -r ../../wedding-picture-processor.zip .
cd ../..
aws lambda update-function-code --function-name wedding-picture-processor --zip-file fileb://~/Programming/photo-booth/wedding-picture-processor.zip --no-paginate --no-cli-pager