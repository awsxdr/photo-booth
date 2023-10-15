import { S3Client, GetObjectCommand, ListObjectsV2Command, PutObjectCommand } from '@aws-sdk/client-s3';
import parser from 'lambda-multipart-parser';
import { randomUUID } from 'crypto';

export const handler = async (event) => {
  switch (event.resource) {
    case '/photos':
      return await handlePhotos(event);
  }
  return NotFound();
};

async function handlePhotos(event) {
  switch(event.httpMethod) {
    case "GET":
      return Ok(await getPhotoList());
    
    case "POST":
      return await uploadPhoto(event);
  }
  
  return NotFound();
}

async function getPhotoList() {
  const s3 = new S3Client({ 
    region: 'eu-west-2', 
  });
  
  const response = await s3.send(new ListObjectsV2Command({
    Bucket: "tim-leanne-wedding-photos",
    Prefix: "photos/"
  }));

  const photos = response.Contents
    .filter(item => item.Key.endsWith(".jpg"))
    .filter(item => !item.Key.includes("/thumbnails/"))
    .sort((a, b) => new Date(b.LastModified) - new Date(a.LastModified))
    .map(item => item.Key)
    .map(item => item.match(/\/([^\/]+)$/)[1]);
  
  return { photos };
}

async function uploadPhoto(event) {
  const s3 = new S3Client({
    region: 'eu-west-2',
  });

  const formData = await parser.parse(event);

  if(formData.files.length !== 1) {
    return BadRequest();
  }

  const file = formData.files[0];
  const extensionStart = file.filename.lastIndexOf('.');
  if(extensionStart < 0) {
    return BadRequest();
  }
  const extension = file.filename.substring(extensionStart);
  const newFileName = `${randomUUID()}${extension}`;

  await s3.send(new PutObjectCommand({
    Bucket: "tim-leanne-wedding-photos",
    Key: `photoUploads/${newFileName}`,
    Body: file.content
  }));
  
  return Ok({});
}

function Ok(body) {
  return {
    statusCode: 200,
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify(body),
  };
}

function NotFound() {
  return {
    statusCode: 404,
  };
}

function BadRequest() {
    return {
        statusCode: 400,
    };
}