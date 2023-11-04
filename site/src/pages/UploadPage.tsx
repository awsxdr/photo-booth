import { Stack, Typography } from "@mui/material";
import { PageTemplate } from "./PageTemplate";
import { ImageUploadItem, MultiImageUpload } from "../components";
import { useState } from "react";

interface PhotoUploadResponse {
    uploadUrl: string
}

export const UploadPage = () => {

    const [files, setFiles] = useState<ImageUploadItem[]>([]);
    const [isUploading, setIsUploading] = useState(false);
    const [uploadsComplete, setUploadsComplete] = useState(false);
    const [uploadsFailedCount, setUploadsFailedCount] = useState(0);

    const handleUpload = () => {
        setIsUploading(true);

        let completedUploadCount = 0;

        const handleUploadComplete = () => {
            ++completedUploadCount;

            if(completedUploadCount === files.length) {
                setUploadsComplete(true);
            }
        }

        const handleUploadError = () => {
            setUploadsFailedCount(current => current + 1);
            handleUploadComplete();
        }

        files.forEach(file => {
            fetch('/api/photos', {
                method: 'POST',
                body: JSON.stringify({
                    fileName: file.file.name
                }),
            }).then((response: Response) => {
                response.json().then((uploadResponse: PhotoUploadResponse) => {
                    const request = new XMLHttpRequest();

                    request.upload.addEventListener('loadstart', () => {
                        file.uploadProgress = 0;
                        setFiles(files);
                    });
        
                    request.upload.addEventListener('progress', (event) => {
                        file.uploadProgress = event.loaded / event.total * 100;
                        setFiles(files);
                    });
        
                    request.upload.addEventListener('load', () => {
                        file.uploadProgress = 100;
                        handleUploadComplete();
                    });

                    request.upload.addEventListener('error', () => {
                        file.uploadProgress = 0;
                        handleUploadError();
                    });
        
                    request.open("PUT", uploadResponse.uploadUrl, true);
                    request.send(file.file);
                });
            });
        });
    };

    return (
        <PageTemplate selectedTab={1}>
            <Stack spacing={2}>
                { !uploadsComplete && <MultiImageUpload files={files} isUploading={isUploading} onFilesSelected={setFiles} onUploadClicked={handleUpload} /> }
                { uploadsComplete && !!!uploadsFailedCount && <Typography variant="h3">Upload complete!</Typography>}
                { uploadsComplete && !!uploadsFailedCount && 
                    <>
                        <Typography variant="h3">Upload failed</Typography>
                        <Typography variant="h5">{uploadsFailedCount} pictures failed to upload. Please check your internet connection and try again shortly.</Typography>
                    </>}
            </Stack>
        </PageTemplate>
    );
};