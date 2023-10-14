#! /bin/bash

rm ./lambdaFunc.zip
cd ./lambda
zip -r ../lambdaFunc.zip .
cd ..
aws lambda update-function-code --function-name wedding-picture-upload --zip-file fileb://~/Programming/photo-booth/lambdaFunc.zip