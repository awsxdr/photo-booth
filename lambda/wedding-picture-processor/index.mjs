import { S3Client, DeleteObjectCommand, GetObjectCommand, PutObjectCommand } from '@aws-sdk/client-s3';

import { Readable } from 'stream';

import sharp from 'sharp';

const VALID_IMAGE_FORMATS = [ 'jpg', 'jpeg', 'png' ];
const THUMBNAIL_WIDTH = 400;
const BUCKET_NAME = 'tim-leanne-wedding-photos';

export const handler = async (event) => {
    const sourceKey = decodeURIComponent(event.Records[0].s3.object.key.replace(/\+/g, " "));

    const fileNameMatch = sourceKey.match(/\/([^\/]+)$/);
    if(!fileNameMatch) {
        console.log(`Could not determine file name for key ${sourceKey}`);
        return;
    }
    const fileName = fileNameMatch[1];
    
    const fileTypeMatch = fileName.match(/\.([^\.]+)$/);
    if(!fileTypeMatch) {
        console.log(`Could not determine file type for file name ${fileName}`);
        return;
    }
    const fileType = fileTypeMatch[1];

    if(!VALID_IMAGE_FORMATS.includes(fileType.toLocaleLowerCase())) {
        console.log(`File type ${fileType} not supported`);
        return;
    }

    const fileNameWithoutExtension = fileName.substring(0, fileName.length - (fileType.length + 1));

    console.log(`Key: ${sourceKey}`);
    console.log(`File name: ${fileName}`);
    console.log(`File extension: ${fileType}`);
    console.log(`File name without extension: ${fileNameWithoutExtension}`);

    try {
        const s3 = new S3Client({ 
            region: 'eu-west-2', 
        });

        const getResponse = await s3.send(new GetObjectCommand({
            Bucket: BUCKET_NAME,
            Key: `photoUploads/${fileName}`
        }));

        const getStream = getResponse.Body;

        if (getStream instanceof Readable) {
            const buffer = Buffer.concat(await getStream.toArray());

            try {
                var thumbnailBuffer = await sharp(buffer).jpeg().resize(THUMBNAIL_WIDTH).toBuffer();
                console.log(`Creating thumbnail 'photos/thumbnails/${fileNameWithoutExtension}.jpg'`);
                await s3.send(new PutObjectCommand({
                    Bucket: BUCKET_NAME,
                    Key: `photos/thumbnails/${fileNameWithoutExtension}.jpg`,
                    Body: thumbnailBuffer,
                    ContentType: 'image/jpeg',
                }));
        
                var jpegBuffer = await sharp(buffer).jpeg({ quality: 100 }).toBuffer();
                console.log(`Creating photo 'photos/${fileNameWithoutExtension}.jpg'`);
                await s3.send(new PutObjectCommand({
                    Bucket: BUCKET_NAME,
                    Key: `photos/${fileNameWithoutExtension}.jpg`,
                    Body: jpegBuffer,
                    ContentType: 'image/jpeg',
                }));

                console.log(`Deleting '${sourceKey}'`);
                await s3.send(new DeleteObjectCommand({
                    Bucket: BUCKET_NAME,
                    Key: sourceKey
                }));
            } catch (error) {
                console.log(error);
                return;
            }

        } else {
            throw new Error("Unknown stream type");
        }
    } catch (error) {
        console.log(error);
        return;
    }

};
